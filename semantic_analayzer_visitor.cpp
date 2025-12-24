#include <algorithm>

#include "semantic_analayzer_visitor.hpp"

SemanticAnalayzerVisitor::SemanticAnalayzerVisitor() : current_offset(0) {}

void SemanticAnalayzerVisitor::visit(ast::Funcs &node) {

    // emit library functions
    scope_printer.emitFunc("print", ast::BuiltInType::VOID, {ast::BuiltInType::STRING});
    scope_printer.emitFunc("printi", ast::BuiltInType::VOID, {ast::BuiltInType::INT});
    FunctionSymbolEntry print_entry = {"print", current_offset, ast::BuiltInType::VOID, {ast::BuiltInType::STRING}};
    FunctionSymbolEntry printi_entry = {"printi", current_offset, ast::BuiltInType::VOID, {ast::BuiltInType::INT}};
    function_symbol_table.push_back(print_entry);
    function_symbol_table.push_back(printi_entry);

    for (auto it = node.funcs.begin(); it != node.funcs.end(); ++it) {
        (*it)->accept(*this);
    }
}

void SemanticAnalayzerVisitor::visit(ast::FuncDecl &node) {
    //TODO: parse the node params correctly. maybe when debugging.
    std::vector<ast::BuiltInType> arguments;
    arguments.reserve(node.formals->formals.size());
    std::transform(node.formals->formals.begin(), node.formals->formals.end(), std::back_inserter(arguments),
        [](const std::shared_ptr<ast::Formal>& formal) {
            // You'll need a way to map your Type object to ast::BuiltInType
            // Example: accessing a member or calling a conversion method
            return formal->type->type; 
        });
    
    FunctionSymbolEntry function_entry = {node.id->value, current_offset, node.return_type->type, arguments};

    scope_printer.emitFunc(function_entry.name, function_entry.return_type, function_entry.arguments);

    function_symbol_table.push_back(function_entry);
    current_offset++;

    node.id->accept(*this);

    node.return_type->accept(*this);

    node.formals->accept(*this);

    node.body->accept(*this);
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

