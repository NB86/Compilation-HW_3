#include "semantic_analayzer_visitor.hpp"

SemanticAnalayzerVisitor::SemanticAnalayzerVisitor() : currect_offset(0) {}

void SemanticAnalayzerVisitor::visit(ast::Funcs &node) {

    // emit library functions
    scope_printer.emitFunc("print", BuiltInType::VOID, {BuiltInType::STRING});
    scope_printer.emitFunc("printi", BuiltInType::VOID, {BuiltInType::INT});
    FunctionSymbolEntry print_entry = {"print", currect_offset, BuiltInType::VOID, {BuiltInType::STRING}}
    FunctionSymbolEntry printi_entry = {node.id.value, currect_offset, BuiltInType::VOID, {BuiltInType::INT}}
    function_symbol_table.push_back(print_entry);
    function_symbol_table.push_back(printi_entry);

    for (auto it = node.funcs.begin(); it != node.funcs.end(); ++it) {
        (*it)->accept(*this);
    }
}

void SemanticAnalayzerVisitor::visit(ast::FuncDecl &node) {
        //TODO: parse the node params correctly. maybe when debugging. 
        FunctionSymbolEntry function_entry = {node.id.value, currect_offset, node.return_type, node.formals}

        scope_printer.emitFunc(function_entry.name, function_entry.return_type, function_entry.arguments);

        function_symbol_table.push_back(function_entry);
        current_offset++;

        node.id->accept(*this);

        node.return_type->accept(*this);

        node.formals->accept(*this);

        node.body->accept(*this);
    }
