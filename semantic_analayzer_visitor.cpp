#include <algorithm>
#include <iostream>

#include "semantic_analayzer_visitor.hpp"

SemanticAnalayzerVisitor::SemanticAnalayzerVisitor() : is_inside_while(false) {}

void SemanticAnalayzerVisitor::visit(ast::Funcs &node) {
    // adding global scope offset
    offset_stack.push(0);

    // emit library functions
    scope_printer.emitFunc("print", ast::BuiltInType::VOID, {ast::BuiltInType::STRING});
    scope_printer.emitFunc("printi", ast::BuiltInType::VOID, {ast::BuiltInType::INT});
    FunctionSymbolEntry print_entry = {"print", offset_stack.top()++, ast::BuiltInType::VOID, {ast::BuiltInType::STRING}};
    FunctionSymbolEntry printi_entry = {"printi", offset_stack.top()++, ast::BuiltInType::VOID, {ast::BuiltInType::INT}};
    function_symbol_table.push_back(print_entry);
    function_symbol_table.push_back(printi_entry);

    // Adding first all the symbols of the functions to the function_symbol_table attribute. 
    bool has_valid_main = false;

    for (const auto& function : node.funcs) {
        std::vector<ast::BuiltInType> arguments;
        arguments.reserve(function->formals->formals.size());
        std::transform(function->formals->formals.begin(), function->formals->formals.end(), std::back_inserter(arguments),
            [](const std::shared_ptr<ast::Formal>& formal) {
                return formal->type->type; 
            });
        
        FunctionSymbolEntry function_entry = {function->id->value, offset_stack.top()++, function->return_type->type, arguments};
        for (const auto& function_symbol : function_symbol_table) {
            if (function_entry.name == function_symbol.name) {
                output::errorDef(function->line, function_entry.name);
            }
        }
        scope_printer.emitFunc(function_entry.name, function_entry.return_type, function_entry.arguments);
        function_symbol_table.push_back(function_entry);

        if (function_entry.name == "main" && function_entry.return_type == ast::BuiltInType::VOID && function_entry.arguments.size() == 0) {
            has_valid_main = true;
        }
    }

    if (!has_valid_main) {
        output::errorMainMissing();
    }

    for (auto it = node.funcs.begin(); it != node.funcs.end(); ++it) {
        // correction of 2 beacouse of 'print' and 'printi' functionns 
        current_function = function_symbol_table[std::distance(node.funcs.begin(), it) + 2];
        (*it)->accept(*this);
    }
}

void SemanticAnalayzerVisitor::visit(ast::FuncDecl &node) {
    // Creating a new scope in the symbol_table attribute and adding symbols for the arguments of the function. 
    // Also, creating a new scope offset corresponding to the new scope. 
    scope_printer.beginScope();
    offset_stack.push(-1);
    std::vector<SymbolEntry> symbols_in_scope; 
    std::transform(node.formals->formals.begin(), node.formals->formals.end(), std::back_inserter(symbols_in_scope),
        [this](const std::shared_ptr<ast::Formal>& formal) {
            SymbolEntry entry = {formal->id->value, formal->type->type, offset_stack.top()--};
            scope_printer.emitVar(entry.name, entry.type, entry.offset);
            return entry;
        });
        
    symbol_table.push_back(symbols_in_scope);
    offset_stack.top() = 0;

    node.id->accept(*this);
    node.return_type->accept(*this);
    node.formals->accept(*this);
    node.body->accept(*this);

    // Remove the function scope
    scope_printer.endScope();
    offset_stack.pop();
    symbol_table.pop_back();
}

void SemanticAnalayzerVisitor::visit(ast::If &node) {
    // Creating a new scope in the symbol_table attribute to the 'If' statment.
    scope_printer.beginScope();
    //offset_stack.push(0);
    symbol_table.push_back(std::vector<SymbolEntry>());

    // Check if condition is boolean expression
    bool is_cond_bool = false;
    if (typeid(*node.condition) == typeid(ast::Bool)) {
        is_cond_bool = true;
    } else if (typeid(*node.condition) == typeid(ast::ID)) {            
        for (const auto& scope : symbol_table) {
            for (const auto& symbol : scope) {
                if (symbol.name == dynamic_cast<ast::ID*>(node.condition.get())->value && symbol.type == ast::BuiltInType::BOOL) {
                    is_cond_bool = true;
                }
            }
        }
    }

    if (!is_cond_bool) {
        output::errorMismatch(node.condition->line);
    }

    node.condition->accept(*this);

    // Create a new scope if the 'then' code starts a scope
    if (typeid(*node.then) == typeid(ast::Statements)) {
        scope_printer.beginScope();
        //offset_stack.push(0);
        symbol_table.push_back(std::vector<SymbolEntry>());

        node.then->accept(*this);

        scope_printer.endScope();
        //offset_stack.pop();
        symbol_table.pop_back();
    } else {
        node.then->accept(*this);
    }
    
    
    // Remove the 'If' statment scope
    scope_printer.endScope();
    //offset_stack.pop();
    symbol_table.pop_back();

    if (node.otherwise) {
        // Creating a new scope in the symbol_table attribute to the 'Else' statment.
        scope_printer.beginScope();
        //offset_stack.push(0);
        symbol_table.push_back(std::vector<SymbolEntry>());

        // Create a new scope if the 'otherwise' code starts a scope
        if (typeid(*node.otherwise) == typeid(ast::Statements)) {
            scope_printer.beginScope();
            //offset_stack.push(0);
            symbol_table.push_back(std::vector<SymbolEntry>());

            node.otherwise->accept(*this);

            scope_printer.endScope();
            //offset_stack.pop();
            symbol_table.pop_back();
        } else {
            node.otherwise->accept(*this);
        }

        // Remove the 'Else' statment scope
        scope_printer.endScope();
        //offset_stack.pop();
        symbol_table.pop_back();
    }
}

void SemanticAnalayzerVisitor::visit(ast::While &node) {
    // Creating a new scope in the symbol_table attribute to the 'While' statment.
    scope_printer.beginScope();
    offset_stack.push(0);
    symbol_table.push_back(std::vector<SymbolEntry>());

    // Check if condition is boolean expression
    if (typeid(*node.condition) != typeid(ast::Bool)) {
        output::errorMismatch(node.condition->line);
    }
    node.condition->accept(*this);
    is_inside_while = true;

    // Create a new scope if the 'body' code starts a scope
    if (typeid(*node.body) == typeid(ast::Statements)) {
        scope_printer.beginScope();
        offset_stack.push(0);
        symbol_table.push_back(std::vector<SymbolEntry>());

        node.body->accept(*this);

        scope_printer.endScope();
        offset_stack.pop();
        symbol_table.pop_back();   
    } else {
        node.body->accept(*this);
    }

    is_inside_while = false;
    // Remove the 'While' statment scope
    scope_printer.endScope();
    offset_stack.pop();
    symbol_table.pop_back();
}

void SemanticAnalayzerVisitor::visit(ast::Statements &node) {
    for (auto it = node.statements.begin(); it != node.statements.end(); ++it) {
        (*it)->accept(*this);
    }
}

void SemanticAnalayzerVisitor::visit(ast::VarDecl &node) {
    // Check if variable name is occupied
    for (const auto& scope : symbol_table) {
        for (const auto& symbol : scope) {
            if (symbol.name == node.id->value) {
                output::errorDef(node.line, node.id->value);
            }
        }
    } 

    // Check if init_exp appropriate
    if (node.init_exp) {
        if (typeid(*node.init_exp) == typeid(ast::ID)) {            
        for (const auto& function : function_symbol_table) {
                if (function.name == dynamic_cast<ast::ID*>(node.init_exp.get())->value) {
                    output::errorDefAsFunc(node.line, function.name);
                }
            }
        }
    }

    SymbolEntry entry = {node.id->value, node.type->type, offset_stack.top()++};
    symbol_table.back().push_back(entry);
    scope_printer.emitVar(entry.name, entry.type, entry.offset);
}

void SemanticAnalayzerVisitor::visit(ast::Assign &node) {
    // Check if variable exists
    bool is_variable_exists = false;
    for (const auto& scope : symbol_table) {
        for (const auto& symbol : scope) {
            if (symbol.name == node.id->value) {
                is_variable_exists = true;
            }
        }
    } 

    if (!is_variable_exists) {
        for (const auto& function : function_symbol_table) {
            if (node.id->value == function.name) {
                output::errorDefAsFunc(node.line, node.id->value);
            }
        }
        output::errorUndef(node.line, node.id->value);
    }
}

void SemanticAnalayzerVisitor::visit(ast::Call &node) {
    // Check if the function exists
    bool is_function_exists = false;
    for (const auto& function : function_symbol_table) {
        if (node.func_id->value == function.name) {
            is_function_exists = true;
        }
    }

    if (!is_function_exists) {

        for (const auto& scope : symbol_table) {
            for (const auto& variable : scope) {
                if (node.func_id->value == variable.name) {
                    output::errorDefAsVar(node.line, node.func_id->value);
                }
            }
        }

        output::errorUndefFunc(node.line, node.func_id->value);
    }

}


void SemanticAnalayzerVisitor::visit(ast::Break &node) {
    if (!is_inside_while) {
        output::errorUnexpectedBreak(node.line);
    }
}

void SemanticAnalayzerVisitor::visit(ast::Continue &node) {
    if (!is_inside_while) {
        output::errorUnexpectedContinue(node.line);
    }
}

void SemanticAnalayzerVisitor::visit(ast::Return &node) {
    ast::BuiltInType type_to_return = current_function.return_type;

    if (!node.exp && type_to_return != ast::BuiltInType::VOID) {
        output::errorMismatch(node.line);
    }

    switch (type_to_return) {
        case ast::BuiltInType::VOID:
            if (node.exp) {
                output::errorMismatch(node.line);
            }
            break;
        case ast::BuiltInType::BOOL:
            if (typeid(*node.exp) != typeid(ast::Bool)) {
                output::errorMismatch(node.line);
            }
            break;
        case ast::BuiltInType::BYTE:
            if (typeid(*node.exp) != typeid(ast::NumB)) {
                output::errorMismatch(node.line);
            }
            break;
        case ast::BuiltInType::INT:
            if (typeid(*node.exp) != typeid(ast::Num)) {
                output::errorMismatch(node.line);
            }
            break;
        case ast::BuiltInType::STRING:
            if (typeid(*node.exp) != typeid(ast::String)) {
                output::errorMismatch(node.line);
            }
            break;
    }
}

void SemanticAnalayzerVisitor::visit(ast::Num &node) {}

void SemanticAnalayzerVisitor::visit(ast::NumB &node) {}

void SemanticAnalayzerVisitor::visit(ast::String &node) {}

void SemanticAnalayzerVisitor::visit(ast::Bool &node) {}

void SemanticAnalayzerVisitor::visit(ast::ID &node) {}

void SemanticAnalayzerVisitor::visit(ast::BinOp &node) {}

void SemanticAnalayzerVisitor::visit(ast::RelOp &node) {}

void SemanticAnalayzerVisitor::visit(ast::Not &node) {}

void SemanticAnalayzerVisitor::visit(ast::And &node) {}

void SemanticAnalayzerVisitor::visit(ast::Or &node) {}

void SemanticAnalayzerVisitor::visit(ast::Type &node) {}

void SemanticAnalayzerVisitor::visit(ast::Cast &node) {}

void SemanticAnalayzerVisitor::visit(ast::ExpList &node) {}

void SemanticAnalayzerVisitor::visit(ast::Formal &node) {}

void SemanticAnalayzerVisitor::visit(ast::Formals &node) {}
