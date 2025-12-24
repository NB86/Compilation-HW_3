#include <algorithm>

#include "semantic_analayzer_visitor.hpp"

SemanticAnalayzerVisitor::SemanticAnalayzerVisitor() {}

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

    for (auto it = node.funcs.begin(); it != node.funcs.end(); ++it) {
        (*it)->accept(*this);
    }
}

void SemanticAnalayzerVisitor::visit(ast::FuncDecl &node) {
    // Adding the function symbol to the function_symbol_table attribute. 
    std::vector<ast::BuiltInType> arguments;
    arguments.reserve(node.formals->formals.size());
    std::transform(node.formals->formals.begin(), node.formals->formals.end(), std::back_inserter(arguments),
        [](const std::shared_ptr<ast::Formal>& formal) {
            return formal->type->type; 
        });
    
    FunctionSymbolEntry function_entry = {node.id->value, offset_stack.top()++, node.return_type->type, arguments};
    scope_printer.emitFunc(function_entry.name, function_entry.return_type, function_entry.arguments);
    function_symbol_table.push_back(function_entry);

    // Creating a new scope in the symbol_table attribute and adding symbols for the arguments of the function. 
    // Also, creating a new scope offset corresponding to the new scope. 
    offset_stack.push(-1);
    std::vector<SymbolEntry> symbols_in_scope; 
    std::transform(node.formals->formals.begin(), node.formals->formals.end(), std::back_inserter(symbols_in_scope),
        [this](const std::shared_ptr<ast::Formal>& formal) {
            SymbolEntry entry = {formal->id->value, offset_stack.top()--, formal->type->type};
            return entry;
        });
        
    symbol_table.push_back(symbols_in_scope);
    offset_stack.top() = 0;

    node.id->accept(*this);

    node.return_type->accept(*this);

    node.formals->accept(*this);

    node.body->accept(*this);

    offset_stack.pop();
    symbol_table.pop_back();
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

void SemanticAnalayzerVisitor::visit(ast::Call &node) {}

void SemanticAnalayzerVisitor::visit(ast::Statements &node) {}

void SemanticAnalayzerVisitor::visit(ast::Break &node) {}

void SemanticAnalayzerVisitor::visit(ast::Continue &node) {}

void SemanticAnalayzerVisitor::visit(ast::Return &node) {}

void SemanticAnalayzerVisitor::visit(ast::If &node) {}

void SemanticAnalayzerVisitor::visit(ast::While &node) {}

void SemanticAnalayzerVisitor::visit(ast::VarDecl &node) {}

void SemanticAnalayzerVisitor::visit(ast::Assign &node) {}

void SemanticAnalayzerVisitor::visit(ast::Formal &node) {}

void SemanticAnalayzerVisitor::visit(ast::Formals &node) {}

