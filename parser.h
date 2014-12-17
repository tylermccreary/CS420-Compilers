#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "keywordtoken.h"
#include "punctoken.h"
#include "reloptoken.h"
#include "addoptoken.h"
#include "muloptoken.h"
#include "idtoken.h"
#include "numtoken.h"
#include "eoftoken.h"
#include "scanner.h"
#include "symbol_table.h"
#include "register.h"
#include "register_allocator.h"
#include "emitter.h"

#include <iostream>

using namespace std;

class Parser {
   public:
      Parser (Scanner *s);
      ~Parser();
  
      bool parse_program();  
      // Return true if current word is the EOF token
      bool done_with_input();
  
   private:
      bool parse_decl_list();
      // etc, etc...; one for each non-terminal in TruPL 2.0

      bool parse_variable_decl_list();

      bool parse_variable_decl();

      bool parse_procedure_decl_list();

      bool parse_procedure_decl();

      bool parse_arg_list();

      bool parse_arg_list_hat();

      bool parse_identifier_list();

      bool parse_identifier_list_prm();

      bool parse_standard_type(expr_type &the_standard_type);

      bool parse_block();

      bool parse_stmt_list();

      bool parse_stmt_list_prm();

      bool parse_stmt();

      bool parse_stmt_ass_proc_tail(expr_type &stmt_ass_proc_tail_type,
                                     Token *identifier);

      bool parse_assignment_stmt_tail(expr_type &the_ass_stmt_tail_type, Register *&expr_reg);

      bool parse_if_stmt();

      bool parse_if_stmt_hat();

      bool parse_while_stmt();

      bool parse_print_stmt();

      bool parse_procedure_call_stmt_tail(Token *identifier);

      bool parse_expr_list(Token *identifier);

      bool parse_expr_list_hat(Token *identifier);

      bool parse_expr(expr_type &the_expr_type, Register *&expr_reg);

      bool parse_expr_hat(expr_type &expr_hat_type, Register *&expr_reg);

      bool parse_simple_expr(expr_type &simple_expr_type, Register *&expr_reg);

      bool parse_simple_expr_prm(expr_type &simple_expr_prm_type, Register *&expr_reg);

      bool parse_term(expr_type &the_term_type, Register *&expr_reg);

      bool parse_term_prm(expr_type &the_term_prm_type, Register *&expr_reg);

      bool parse_factor(expr_type &the_factor_type, Register *&expr_reg);

      bool parse_sign(bool positive);

  
      // The lexical analyzer
      Scanner *lex;
      // The current token the parser is looking at
      Token *word;

      Symbol_Table *stab;

      Emitter *e;

      Register_Allocator *ra;

      /* The current environment we are doing semantic analysis in.
       * It will be either the main program name or a procedure
       * name.
       */
      string *current_env;

      //The main environment. Always the name of the main program.
      string *main_env;

      /* Used in semantic analysis to determine which position a parameter
       * is in during the parsing of actual parameters and formal parameters.
       */
      int parm_pos;
			
      string *expected;

  
      void parse_error (string *expected, Token *found);

      // Called when a type error occurs.  "where" is the current token.
      void type_error(Token *where);

      /* Called when an undeclared identifier is referenced in a statement.
       * Print the identifier and the current environment.
       */
      void undeclared_id_error(string *id, string *env);

      // Other helper functions that you may define
      void func_stack (string *func_name);

      void print_token (Token *token);
};

#endif
