#include <iostream>

#include "output.hpp"
#include "nodes.hpp"
#include "semantic_analayzer_visitor.hpp"

// Extern from the bison-generated parser
extern int yyparse();

extern std::shared_ptr<ast::Node> program;

int main() {
    // Parse the input. The result is stored in the global variable `program`
    yyparse();

    // Print the AST using the PrintVisitor
    //TODO: create the semantic visitor and run it over the AST 
    
    
    SemanticAnalayzerVisitor visitor; 
    program->accept(visitor);
    std::cout << visitor.scope_printer;
}
