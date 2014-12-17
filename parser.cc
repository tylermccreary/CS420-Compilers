#include "parser.h"

Parser::Parser(Scanner *s) 
{
    lex = s;
    // Init the word variable.
    word = lex->next_token();

    // Create the symbol table
    stab = new Symbol_Table();

    //create emitter
    e = new Emitter();

    ra = new Register_Allocator(e, stab);
    
    // Init the environments to null
    current_env = NULL;
    main_env = NULL;
    parm_pos = -1;
}

Parser::~Parser() 
{
    if (lex != NULL) {
        delete lex;
    }
    if (word != NULL) {
        delete word;
    }
    if (stab != NULL) {
        delete stab;
    }
    if (current_env != NULL) {
        delete current_env;
    }
    if (main_env != NULL) {
        delete main_env;
    }
}

// If we have parsed the entire program, then word
// should be the EOF Token.  This function tests
// that condition.
bool Parser::done_with_input()
{
    return word->get_token_type() == TOKEN_EOF;
}

bool Parser::parse_program()
{
    // PROGRAM -> program identifier ; DECL_LIST BLOCK ;
    // Predict (program identifier ; DECL_LIST BLOCK ;) == {program}

    // Match keyword program
    if (word->get_token_type() == TOKEN_KEYWORD 
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROGRAM) {

        /* ADVANCE  - Notice that we only delete a token on an ADVANCE, and, if
         * we ADVANCE, it is the ADVANCE code that is responsible for 
         * getting the next token.
         */
	    //print_token(word);
        delete word; 
        word = lex->next_token();

        // Match identifier
        if (word->get_token_type() == TOKEN_ID) {
            string *external;
            external = new string("_EXTERNAL");

            stab->install (static_cast<IdToken *>(word)->get_attribute(),
                external, PROGRAM_T);
                current_env = main_env = static_cast<IdToken *>(word)->get_attribute();

            string *program_id = e->get_new_label("start");
            e->emit_label(program_id);
		
            // ADVANCE
		    //print_token(word);
            delete word; 
            word = lex->next_token();
	        // Match ;
	        if (word->get_token_type() == TOKEN_PUNC 
	            && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	            // ADVANCE
		        //print_token(word);
	            delete word; 
	            word = lex->next_token();

	            // Match DECL_LIST - ACTION
	            if (parse_decl_list()) {
	    
	                // Match BLOCK - ACTION
	                if (parse_block()) {
	      
	                    if (word->get_token_type() == TOKEN_PUNC
		                    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
		                    // ADVANCE
		                    //print_token(word);
		                    delete word;
		                    word = lex->next_token();
		                    // Parsing of non-terminal PROGRAM succeeded
                            e->emit_halt();
                            stab->data_directives();
		                    return true;
		
		                    // We failed to match the second semicolon
	                    } else {
		                    string *expected = new string ("';'");
		                    //string *func = new string("parse_program()");
		                    //func_stack(func);
		                    parse_error (expected, word);
		                    delete expected;
		                    delete word;
		                    return false;
	                    }
	      
	                    // We failed to parse BLOCK
	                } else {
	                     return false;
	                }
	    
	                 // We failed to parse DECL_LIST
	            } else {
	                return false;
	            }
	  
	        // We failed to match the first semicolon
	        } else {
	            string *expected = new string ("';'");
	            //string *func = new string("parse_program()");
	            //func_stack(func);
	            parse_error (expected, word);
	            delete expected;
	            delete word;
	            return false;
	        }
	
	    // We failed to match an identifier
        } else {
            string *expected = new string ("identifier");
		    //string *func = new string("parse_program()");
		    //func_stack(func);
            parse_error (expected, word);
            delete expected;
            delete word;
            return false;
        }
     
    // We failed to match the keyword program
    } else {
        string *expected = new string ("program");
	    //string *func = new string("parse_program()");
	    //func_stack(func);
        parse_error (expected, word);
        return false;
    }
   
   // We shouldn't reach this statement.
   return false;
}

bool Parser::parse_decl_list()
{
    /* DECL_LIST -> VARIABLE_DECL_LIST PROCEDURE_DECL_LIST
     * Predict (VARIABLE_DECL_LIST PROCEDURE_DECL_LIST) = 
     *          First (VARIABLE_DECL_LIST)
     *    union First (PROCEDURE_DECL_LIST)
     *    union Follow (DECL_LIST) = {identifier, procedure, begin}
     */
    if (word->get_token_type() == TOKEN_ID 
        || (word->get_token_type() == TOKEN_KEYWORD 
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE)
        || (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN)) {

        if (parse_variable_decl_list()) {  // ACTION

            if (parse_procedure_decl_list()) { // ACTION
	            return true;
	        } else {
	            // parse_procedure_decl_list() failed
	            return false;
	        }

      
        } else {
            // parse_variable_decl_list() failed
            return false;
        }

    } else {
        // word isn't in predict (DECL_LIST)
        string *expected = new string ("identifier, keyword \"begin\" or keyword \"procedure\"");
		//string *func = new string("parse_decl_list()");
		//func_stack(func);
        parse_error (expected, word);
        return false;
    }
}

bool Parser::parse_variable_decl_list()
{
	/* VARIABLE_DECL_LIST -> VARIABLE_DECL ; VARIABLE_DECL_LIST | lamda
	 * Predict (VARIABLE_DECL ; VARIABLE_DECL_LIST) = identifier
	 */

	if (word->get_token_type() == TOKEN_ID) {
		if (parse_variable_decl()) { //ACTION

			if (word->get_token_type() == TOKEN_PUNC 
	    		&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	  			// ADVANCE
				//print_token(word);
	  			delete word; 
	 			word = lex->next_token();


				if (parse_variable_decl_list()) { //Action
					return true;
				} else {
					// parse_variable_decl_list failed
					return false;
				}
			} else {
				//failed to match semicolon
				string *expected = new string ("';'");
				//string *func = new string("parse_variable_decl_list()");
				//func_stack(func);
	  			parse_error (expected, word);
	  			delete expected;
	  			delete word;
	  			return false;
			}
		} else {
			//parse_variable_decl failed
			return false;
		}

	//Predict (lamda) = begin, procedure
	} else if ((word->get_token_type() == TOKEN_KEYWORD 
	  	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE)
       	|| (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN)) {
			
		return true;
	} else {
		// word isn't in predict (VEARIABLE_DECL_LIST)
		string *expected = new string("identifier, keyword \"begin\" or keyword \"procedure\"");
		//string *func = new string("parse_variable_decl_list()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

bool Parser::parse_variable_decl()
{
	/* VARIABLE_DECL -> IDENTIFIER_LIST : STANDARD_TYPE
	 * Predict (IDENTIFIER_LIST : STANDARD_TYPE) = identifier
	 */

	if (word->get_token_type() == TOKEN_ID) {
		if(parse_identifier_list()) {
			if (word->get_token_type() == TOKEN_PUNC 
	    		&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON) {
	  			// ADVANCE
				//print_token(word);
	  			delete word; 
	 			word = lex->next_token();

                expr_type the_standard_type;
				if (parse_standard_type(the_standard_type)) {

                    stab->update_type (the_standard_type);

					return true;
				} else {
					//parse_standard_type failed
					return false;
				}
			} else {
				//failed to match colon
				string *expected = new string ("':'");
				//string *func = new string("parse_variable_decl()");
				//func_stack(func);
	  			parse_error (expected, word);
	  			delete expected;
	  			delete word;
	  			return false;
			}
		} else {
			//parse_identifier_list faild
			return false;
		}
	} else {
		// word isn't in predict (VEARIABLE_DECL)
		string *expected = new string("identifier");
		//string *func = new string("parse_variable_decl()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

bool Parser::parse_procedure_decl_list()
{
	/* PROCEDURE_DECL_LIST -> PROCEDURE_DECL ; PROCEDURE_DECL_LIST | lamda
	 * Predict (PROCEDURE_DECL ; PROCEDURE_DECL_LIST) = procedure
	 */

	if (word->get_token_type() == TOKEN_KEYWORD 
		&& static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE) {

		if (parse_procedure_decl()) {
			if (word->get_token_type() == TOKEN_PUNC 
	    	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	  		    // ADVANCE
				//print_token(word);
	  		    delete word; 
	 			word = lex->next_token();

				if (parse_procedure_decl_list()) {
				    return true;
        } else {
        	//parse_procedure_decl_list() failed
          return false;
        }
     	} else {
                //failed to match semi
				string *expected = new string ("';'");
				//string *func = new string("parse_procedure_decl_list()");
				//func_stack(func);
	  			parse_error (expected, word);
	  			delete expected;
	  			delete word;
	  			return false;
      }
 		} else {
    	//parse_procedure_decl() failed
      return false;
    }

    //Predict (lamda) = begin
  } else if (word->get_token_type() == TOKEN_KEYWORD
	  && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN) {
        
    return true;
  } else {
        // word isn't in predict (PROCEDURE_DECL_LIST)
		string *expected = new string("keyword \"begin\" or keyword \"procedure\"");
        //string *func = new string("parse_procedure_decl_list()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_procedure_decl()
{
    /* PROCEDURE_DECL -> procedure identifier ( ARG_LIST ) VARIABLE_DECL_LIST BLOCK
	 * Predict (procedure identifier ( ARG_LIST ) VARIABLE_DECL_LIST BLOCK) = procedure
	 */

    if (word->get_token_type() == TOKEN_KEYWORD 
		&& static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE) {
		//print_token(word);
        delete word; 
        word = lex->next_token();

        if (word->get_token_type() == TOKEN_ID) {

            stab->install (static_cast<IdToken *>(word)->get_attribute(),
                current_env, PROCEDURE_T);
            current_env = static_cast<IdToken *>(word)->get_attribute();
            parm_pos = 0;
   
			//print_token(word);
            delete word; 
            word = lex->next_token();
        
            if (word->get_token_type() == TOKEN_PUNC
                && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN) {
                //print_token(word);
                delete word;
                word = lex->next_token();

                if (parse_arg_list()) {
                    if (word->get_token_type() == TOKEN_PUNC
                        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
                        //print_token(word);
                        delete word;
                        word = lex->next_token();
            
                        if (parse_variable_decl_list()) {
                            if (parse_block()) {
                                current_env = main_env;
                                return true;

                            } else {
                                //parse_block() failed
                                return false;
                            }
                        } else {
                            //parse_variable_decl_list() failed
                            return false;
                        }
                    } else {
                        //failed to match close
				        string *expected = new string ("')'");
						//string *func = new string("parse_procedure_decl()");
						//func_stack(func);
	  			        parse_error (expected, word);
	  			        delete expected;
	  			        delete word;
	  			        return false;
                    }
                } else {
                    //parse_arg_list() failed
                    return false;
                }
            } else {
                //failed to match open
				string *expected = new string ("'('");
				//string *func = new string("parse_procedure_decl()");
				//func_stack(func);
	  			parse_error (expected, word);
	  			delete expected;
	  			delete word;
	  			return false;
            }
        } else {
            string *expected = new string ("identifier");
			//string *func = new string("parse_procedure_decl()");
			//func_stack(func);
            parse_error (expected, word);
            delete expected;
            delete word;
            return false;
        }
    } else {
        string *expected = new string ("procedure");
		//string *func = new string("parse_procedure_decl()");
		//func_stack(func);
        parse_error (expected, word);
        return false;
    }
}

bool Parser::parse_arg_list()
{
    /* ARG_LIST -> IDENTIFIER_LIST : STANDARD_TYPE ARG_LIST_HAT | lamda
	 * Predict (IDENTIFIER_LIST : STANDARD_TYPE ARG_LIST_HAT) = identifier
	 */
    if (word->get_token_type() == TOKEN_ID) {
        if (parse_identifier_list()) {
            if (word->get_token_type() == TOKEN_PUNC 
	    	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON) {
	  		    // ADVANCE
				//print_token(word);
	  		    delete word; 
	 			word = lex->next_token();

                expr_type the_standard_type_type;
                if (parse_standard_type(the_standard_type_type)) {

                    stab->set_parm_pos_update(parm_pos, the_standard_type_type);

                    if (parse_arg_list_hat()) {
                        return true;
                    } else {
                        //parse_arg_list_hat() failed
                        return false;
                    }
                } else {
                    //parse_standard_type() failed
                    return false;
                }
            } else {
                //failed to match colon
				string *expected = new string ("':'");
				//string *func = new string("parse_arg_list()");
				//func_stack(func);
	  			parse_error (expected, word);
	  			delete expected;
	  			delete word;
	  			return false;
            }
        } else {
            //parse_identifier_list() failed
            return false;
        }

    //Predict (lamda) = )
    } else if (word->get_token_type() == TOKEN_PUNC 
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
            return true;
    } else {
        // word isn't in predict (ARG_LIST)
		string *expected = new string("identifier or punctation \")\"");
		//string *func = new string("parse_arg_list()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_arg_list_hat()
{
    /* ARG_LIST_HAT -> ; ARG_LIST | lamda
	 * Predict (; ARG_LIST) = ;
	 */
     if (word->get_token_type() == TOKEN_PUNC 
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	    // ADVANCE
		//print_token(word);
	  	delete word; 
	 	word = lex->next_token();
        
        if (parse_arg_list()) {
            return true;
        } else {
            //parse_arg_list() failed
            return false;
        }

    //Predict (lamda) = )
    } else if (word->get_token_type() == TOKEN_PUNC 
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
            return true;
    } else {
        // word isn't in predict (ARG_LIST_HAT)
		string *expected = new string("punctuation \";\" or \")\"");
		//string *func = new string("parse_arg_list_hat()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_identifier_list()
{
    /* IDENTIFIER_LIST -> identifier IDENTIFIER_LIST_PRM
	 * Predict (identifier IDENTIFIER_LIST_PRM) = identifier
	 */
    if (word->get_token_type() == TOKEN_ID) {
        //Install id in stab with type no_t.
        stab->install (static_cast<IdToken *>(word)->get_attribute(),
            current_env, UNKNOWN_T);
		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        if (parse_identifier_list_prm()) {
            return true;
        } else {
            //parse_identifier_list_prm() failed
            return false;
        }
    } else {
        // word isn't in predict (IDENTIFIER_LIST)
		string *expected = new string("identifier");
		//string *func = new string("parse_identifier_list()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_identifier_list_prm()
{
    /* IDENTIFIER_LIST_PRM -> , identifier IDENTIFIER_LIST_PRM | lamda
	 * Predict (, identifier IDENTIFIER_LIST_PRM) = ,
	 */
    if (word->get_token_type() == TOKEN_PUNC 
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA) {
        //print_token(word);
        delete word; 
	 	word = lex->next_token();
        
        if (word->get_token_type() == TOKEN_ID) {

            stab->install (static_cast<IdToken *>(word)->get_attribute(),
                current_env, UNKNOWN_T);

			//print_token(word);
            delete word; 
	 	    word = lex->next_token();

            if (parse_identifier_list_prm()) {
                return true;
            } else {
                //parse_identifier_list_prm() failed
                return false;
            }
        } else {
            string *expected = new string("identifier");
			//string *func = new string("parse_identifier_list_prm()");
			//func_stack(func);
      	    parse_error (expected, word);
            delete expected;
            delete word;
      	    return false;
        }

    //Predict (lamda) = :
    } else if (word->get_token_type() == TOKEN_PUNC 
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON) {
        
        return true;
    } else {
        // word isn't in predict (IDENTIFIER_LIST_PRM)
		string *expected = new string("punctuation \",\" or \":\"");
		//string *func = new string("parse_identifier_list_prm()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_standard_type(expr_type &the_standard_type)
{
    /* STANDARD_TYPE -> int | bool
	 * Predict (int) = int
	 */
    if ((word->get_token_type() == TOKEN_KEYWORD 
		&& static_cast<KeywordToken *>(word)->get_attribute() == KW_INT)) {

        the_standard_type = INT_T;

		//print_token (word);
		delete word; 
	 	word = lex->next_token();
        return true;

    //Predict (bool) = bool
    } else if (word->get_token_type() == TOKEN_KEYWORD 
		&& static_cast<KeywordToken *>(word)->get_attribute() == KW_BOOL) {

        the_standard_type = BOOL_T;

		//print_token (word);
		delete word; 
	 	word = lex->next_token();
        return true;
    } else {
        // word isn't in predict (STANDARD_TYPE)
		string *expected = new string("int or keyword \"bool\"");
		//string *func = new string("parse_standard_type()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_block()
{
    /* BLOCK -> begin STMT_LIST end
	 * Predict (begin STMT_LIST end) = begin
	 */
    if (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN) {
		//print_token(word);
        delete word; 
	 	word = lex->next_token();
        
        if (parse_stmt_list()) {
            if (word->get_token_type() == TOKEN_KEYWORD
	   	        && static_cast<KeywordToken *>(word)->get_attribute() == KW_END) {
                //print_token(word);
                delete word; 
	 	        word = lex->next_token();
                return true;
            } else {
                string *expected = new string("keyword \"end\"");
				//string *func = new string("parse_block()");
				//func_stack(func);
      	        parse_error (expected, word);
                delete expected;
                delete word;
      	        return false;
            }
        } else {
            //parse_stmt_list() failed
            return true;
        }
    } else {
        // word isn't in predict (STANDARD_TYPE)
		string *expected = new string("keyword \"begin\"");
		//string *func = new string("parse_block()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_stmt_list()
{
    /* STMT_LIST -> STMT ; STMT_LIST_PRM
	 * Predict (STMT ; STMT_LIST_PRM) = identifier, if, print, while
	 */
    if ((word->get_token_type() == TOKEN_ID)
        || (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_IF)
        || (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT)
        || (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE)) {

        if (parse_stmt()) {
            if (word->get_token_type() == TOKEN_PUNC 
	            && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
                //print_token(word);
                delete word; 
	 	        word = lex->next_token();

                if (parse_stmt_list_prm()) {
                    return true;
                } else {
                    //parse_stmt_list_prm() failed
                    return false;
                }
            } else {
                string *expected = new string("punctuation \";\"");
				//string *func = new string("parse_stmt_list()");
				//func_stack(func);
      	        parse_error (expected, word);
                delete expected;
                delete word;
      	        return false;     
            }
        } else {
            //parse_stmt() failed
            return false;
        }
    } else {
        // word isn't in predict (STMT_LIST)
		string *expected = new string("identifier, keyword \"if\", keyword \"print\", or keyword \"while\"");
		//string *func = new string("parse_stmt_list()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_stmt_list_prm()
{
    /* STMT_LIST_PRM -> STMT ; STMT_LIST_PRM | lamda
	 * Predict (STMT ; STMT_LIST_PRM) = identifier, if, print, while
	 */
    if ((word->get_token_type() == TOKEN_ID)
        || (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_IF)
        || (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT)
        || (word->get_token_type() == TOKEN_KEYWORD
	   	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE)) {

        if (parse_stmt()) {
            if (word->get_token_type() == TOKEN_PUNC 
	            && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
                //print_token(word);
                delete word; 
	 	        word = lex->next_token();

                if (parse_stmt_list_prm()) {
                    return true;
                } else {
                    //parse_stmt_list_prm() failed
                    return false;
                }
            } else {
                string *expected = new string("punctuation \";\"");
				//string *func = new string("parse_stmt_list_prm()");
				//func_stack(func);
      	        parse_error (expected, word);
                delete expected;
                delete word;
      	        return false;     
            }
        } else {
            //parse_stmt() failed
            return false;
        }

    //Predict(lamda) = end
    } else if (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_END) {
        return true;
    } else {
        // word isn't in predict (STMT_LIST)
		string *expected = new string("identifier, keyword \"if\", keyword \"print\", keyword \"while\", or keyword \"end\"");
		//string *func = new string("parse_stmt_list_prm()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_stmt()
{
    /* STMT -> IF_STMT
              | WHILE_STMT
              | PRINT_STMT
              | identifier STMT_ASS_PROC_TAIL
	 * Predict (IF_STMT) = if
	 */
    if (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_IF) {
    
        if (parse_if_stmt()) {
            return true;
        } else {
            //parse_if_stmt() failed
            return false;
        }
    
    //Predict (WHILE_STMT) = while
    } else if (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE) {
        
        if (parse_while_stmt()) {
            return true;
        } else {
            //parse_while_stmt() failed
            return false;
        }
    
    //Predict (PRINT_STMT) = print
    } else if (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT) {
        
        if (parse_print_stmt()) {
            return true;
        } else {
            //parse_print_stmt() failed
            return false;
        }

    //Predict (identifier STMT_ASS_PROC_TAIL) = identifier
    } else if (word->get_token_type() == TOKEN_ID) {

        if (!(stab->is_decl(static_cast<IdToken *>(word)->get_attribute(), current_env))
            && !(stab->is_decl(static_cast<IdToken *>(word)->get_attribute(), main_env))) {
            undeclared_id_error(static_cast<IdToken *>(word)->get_attribute(), current_env);
        }

        expr_type identifier_type = stab->get_type(static_cast<IdToken *>(word)->get_attribute(), current_env);
        if (identifier_type == GARBAGE_T) {
            identifier_type = stab->get_type(static_cast<IdToken *>(word)->get_attribute(), main_env);
        }
        Token * identifier = word;

		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        expr_type stmt_ass_proc_tail_type;
        if(parse_stmt_ass_proc_tail(stmt_ass_proc_tail_type, identifier)) {
            if (identifier_type != stmt_ass_proc_tail_type) {
                type_error(word);
            }            

            return true;
        } else {
            //parse_stmt_ass_proc_tail() failed
            return false;
        }
    } else {
        // word isn't in predict (STMT)
		string *expected = new string("identifier, keyword \"if\", keyword \"print\", or keyword \"while\"");
		//string *func = new string("parse_stmt()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_stmt_ass_proc_tail(expr_type &stmt_ass_proc_tail_type, Token *identifier)
{
    /* Parse_expr will leave the register containing the value
     of the expression here. */
    Register *expr_reg;

   /* STMT_ASS_PROC_TAIL -> ASSIGNMENT_STMT_TAIL
                            | PROCEDURE_CALL_STMT_TAIL
	 * Predict (ASSIGNMENT_STMT_TAIL) = :=
	 */
	if (word->get_token_type() == TOKEN_PUNC 
	 	&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_ASSIGN) {

        expr_type the_ass_stmt_tail_type;
        if (parse_assignment_stmt_tail(the_ass_stmt_tail_type, expr_reg)) {

            stmt_ass_proc_tail_type = the_ass_stmt_tail_type;
            string *id = static_cast<IdToken *>(identifier)->get_attribute();
            e->emit_move(id, expr_reg);
            ra->deallocate_register(expr_reg);
    
            return true;
        } else {
            //parse_assignment_stmt_tail() failed
            return false;
        }
    
    //Predict (PROCEDURE_CALL_STMT_TAIL) = (
    } else if (word->get_token_type() == TOKEN_PUNC 
	 	&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN) {
        if(parse_procedure_call_stmt_tail(identifier)) {
            stmt_ass_proc_tail_type = PROCEDURE_T;
            return true;
        } else {
            //parse_procedure-call_stmt_tail() failed
            return false;
        }
    } else {
        // word isn't in predict (STMT_ASS_PROC_TAIL)
		string *expected = new string("punctuation \":=\" or punctuation \"(\"");
		//string *func = new string("parse_stmt_ass_proc_tail()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_assignment_stmt_tail(expr_type &the_ass_stmt_tail_type, Register *&expr_reg)
{
    /* ASSIGMENT_STMT_TAIL -> := EXPR
	 * Predict (:= EXPR) = :=
	 */
    if (word->get_token_type() == TOKEN_PUNC 
	 	&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_ASSIGN) {
		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        expr_type the_expr_type;
        if (parse_expr(the_expr_type, expr_reg)) {
            the_ass_stmt_tail_type = the_expr_type;
            return true;
        } else {
            //parse_expr() failed
            return false;
        }
    } else {
        // word isn't in predict (ASSIGNMENT_STMT_TAIL)
		string *expected = new string("punctuation \":=\"");
		//string *func = new string("parse_assignment_stmt_tail()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_if_stmt()
{
    /* Parse_expr will leave the register containing the value
     of the expression here. */
    Register *expr_reg;

    /* IF_STMT -> if EXPR then BLOCK IF_STMT_HAT
	 * Predict (if EXPR then BLOCK IF_STMT_HAT) = if
	 */
    if (word->get_token_type() == TOKEN_KEYWORD
	 	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_IF) {
        //print_token(word);
        delete word; 
	 	word = lex->next_token();

        expr_type the_expr_type;
        if (parse_expr(the_expr_type, expr_reg)) {
            if (the_expr_type != BOOL_T) {
                type_error(word);
                return false;
            }

            /* code generation */
            string *if_false = e->get_new_label ("if_false");
            string *if_done = e->get_new_label ("if_done");   

            e->emit_branch (BREZ, expr_reg, if_false);
            ra->deallocate_register (expr_reg);        
                
            if (word->get_token_type() == TOKEN_KEYWORD 
	 	        && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN) {
        		//print_token(word);
                delete word; 
	 	        word = lex->next_token();

                if (parse_block()) {
                    
                    /* code generation */
	                e->emit_branch (if_done);

	                e->emit_label (if_false);
	                delete if_false;  

                    if (parse_if_stmt_hat()) {
                        
                        /* code generation */
	                    e->emit_label (if_done);
	                    delete if_done;
	                    /**/

                        return true;
                    } else {
                        //parse_if_stmt_hat() failed
                        return false;
                    }
                } else {
                    //parse_block() failed
                }
            } else {
                string *expected = new string("keyword \"then\"");
				//string *func = new string("parse_if_stmt()");
				//func_stack(func);
      	        parse_error (expected, word);
                delete expected;
                delete word;
      	        return false; 
            }
        } else {
            //parse_expr() failed
            return false;
        }
    } else {
        // word isn't in predict (IF_STMT)
		string *expected = new string("keyword \"if\"");
		//string *func = new string("parse_if_stmt()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_if_stmt_hat()
{
    /* IF_STMT_HAT -> else BLOCK | lamda
	 * Predict (else BLOCK) = else
	 */
    if (word->get_token_type() == TOKEN_KEYWORD 
	 	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_ELSE) {
        //print_token(word);
        delete word; 
	 	word = lex->next_token();

        if (parse_block()) {
            return true;
        } else {
            //parse_block() failed
            return false;
        }

    //Predict (lamda) = ;
    } else if (word->get_token_type() == TOKEN_PUNC 
	 	&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {

        return true;
    } else {
        // word isn't in predict (IF_STMT_HAT)
		string *expected = new string("keyword \"else\" or punctuation \";\"");
		//string *func = new string("parse_if_stmt_hat()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_while_stmt()
{
    /* Parse_expr will leave the register containing the value
     of the expression here. */
    Register *expr_reg;

    /* WHILE_STMT -> while EXPR BLOCK
	 * Predict (while EXPR BLOCK) = while
	 */
    if (word->get_token_type() == TOKEN_KEYWORD
	 	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE) {
		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        /* code generation */
        string *w_loop = e->get_new_label ("w_loop");
        string *w_done = e->get_new_label ("w_done");
        e->emit_label(w_loop);

        expr_type the_expr_type;
        if (parse_expr(the_expr_type, expr_reg)) {
            if (the_expr_type != BOOL_T) {
                type_error(word);
                return false;
            }

            e->emit_branch(BREZ, expr_reg, w_done);
            ra->deallocate_register (expr_reg);

            if (parse_block()) {
                e->emit_branch(w_loop);
                e->emit_label(w_done);
                return true;
            } else {
                //parse_block() failed
                return false;
            }
        } else {
            //parse_expr() failed
            return false;
        }
    } else {
        // word isn't in predict (WHILE_STMT)
		string *expected = new string("keyword \"while\"");
		//string *func = new string("parse_while_stmt()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_print_stmt()
{
    /* Parse_expr will leave the register containing the value
     of the expression here. */
    Register *expr_reg;

    /* PRINT_STMT -> print EXPR
	 * Predict (print EXPR) = print
	 */
    if (word->get_token_type() == TOKEN_KEYWORD 
	 	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT) {
		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        expr_type the_expr_type;
        if (parse_expr(the_expr_type, expr_reg)) {
            if (the_expr_type != INT_T) {
                type_error(word);
                return false;
            }

            inst_type outb (OUTB);
            e->emit_1addr (outb, expr_reg);
            ra->deallocate_register(expr_reg);

            return true;
        } else {
            //parse_expr() failed
            return false;
        }
    } else {
        // word isn't in predict (PRINT_STMT)
		string *expected = new string("keyword \"print\"");
		//string *func = new string("parse_print_stmt()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_procedure_call_stmt_tail(Token *identifier)
{
    /* PROCEDURE_CALL_STMT_TAIL -> ( EXPR_LIST )
	 * Predict ( ( EXPR_LIST ) ) = (
	 */
    if (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN) {

        parm_pos = 0;
		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        if (parse_expr_list(identifier)) {
            if (word->get_token_type() == TOKEN_PUNC
	 	        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
                //print_token(word);
                delete word; 
	 	        word = lex->next_token();
                return true;
            } else {
                string *expected = new string("punctuation \")\"");
				//string *func = new string("parse_procedure_call_stmt_tail()");
				//func_stack(func);
      	        parse_error (expected, word);
                delete expected;
                delete word;
      	        return false; 
            }
        } else {
            //parse_expr_list() failed
            return false;
        }
    } else {
        // word isn't in predict (PROCEDURE_CALL_STMT_TAIL)
		string *expected = new string("punctuation \"(\"");
		//string *func = new string("parse_procedure_call_stmt_tail()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_expr_list(Token *identifier)
{
    /* EXPR_LIST -> EXPR EXPR_LIST_HAT | lamda
	 * Predict (EXPR EXPR_LIST_HAT) = identifier, num, (, not, +, -
	 */
    if ((word->get_token_type() == TOKEN_ID)
        || (word->get_token_type() == TOKEN_NUM)
        || (word->get_token_type() == TOKEN_PUNC
	 	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
        || (word->get_token_type() == TOKEN_KEYWORD
	 	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)) {

        expr_type the_expr_type;
        Register *expr_reg;
        if (parse_expr(the_expr_type, expr_reg)) {
            if (the_expr_type != stab->get_type(static_cast<IdToken *>(identifier)->get_attribute(), parm_pos)) {
                type_error(word);
                return false;
            }
            parm_pos++;

            if (parse_expr_list_hat(identifier)) {
                return true;
            } else {
                //parse_expr_list_hat() failed
                return false;
            }
        } else {
            //parse_expr() failed
            return false;
        }
    
    //Predict (lamda) = )
    } else if (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {

        return true;
    } else {
        // word isn't in predict (EXPR_LIST)
		string *expected = new string("identifier, num, punctuation \"(\", keyword \"not\", sign \"+\", sign \"-\", or punctuation \")\"");
		//string *func = new string("parse_expr_list()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_expr_list_hat(Token *identifier)
{
    /* EXPR_LIST_HAT -> , EXPR_LIST | lamda
	 * Predict (, EXPR_LIST) = ,
	 */
    if (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA) {
		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        if (parse_expr_list(identifier)) {
            return true;
        } else {
            //parse_expr_list() failed
            return false;
        }

    //Predict (lamda) = )
    } else if (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
    
        return true;
    } else {
        // word isn't in predict (EXPR_LIST_HAT)
		string *expected = new string("punctuation \",\" or punctuation \")\"");
		//string *func = new string("parse_expr_list_hat()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_expr(expr_type &the_expr_type, Register *&expr_reg) {
    expr_type simple_expr_type, expr_hat_type;

    /* EXPR -> SIMPLE_EXPR EXPR_HAT
	 * Predict (SIMPLE_EXPR EXPR_HAT) = identifier, num, (, not, +, -
	 */
    if ((word->get_token_type() == TOKEN_ID)
        || (word->get_token_type() == TOKEN_NUM)
        || (word->get_token_type() == TOKEN_PUNC
	 	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
        || (word->get_token_type() == TOKEN_KEYWORD
	 	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)) {

        if (parse_simple_expr(simple_expr_type, expr_reg)) {
            if (parse_expr_hat(expr_hat_type, expr_reg)) {

                /* semantics actions: calculate expr_type */
                if (expr_hat_type == NO_T) {
                    the_expr_type = simple_expr_type;
                } else if (simple_expr_type == INT_T && expr_hat_type == INT_T) {
                    the_expr_type = BOOL_T;
                } else {
                    type_error(word);
                }

                return true;
            } else {
                //parse_expr_hat() failed
                return false;
            }
        } else {
            //parse_simple_expr() failed
            return false;
        }
    } else {
        // word isn't in predict (EXPR)
		string *expected = new string("identifier, num, punctuation \"(\", keyword \"not\", sign \"+\", or sign \"-\"");
		//string *func = new string("parse_expr()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_expr_hat(expr_type &the_expr_hat_type, Register *&left_reg)
{
    /* EXPR_HAT -> relop SIMPLE_EXPR | lamda
	 * Predict (relop SIMPLE_EXPR) = relop
	 */
    if (word->get_token_type() == TOKEN_RELOP) {
        relop_attr_type relop_type = (static_cast<RelopToken *>(word)->get_attribute());      


		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        expr_type the_simple_expr_type; 
        Register *right_reg;
        if (parse_simple_expr(the_simple_expr_type, right_reg)) {
            if (the_simple_expr_type == INT_T) {
                e->emit_2addr(SUB, left_reg, right_reg);
                ra->deallocate_register(right_reg);
                string *done = e->get_new_label("done");
                if (relop_type == RELOP_EQ) {
                    string *tral_true = e->get_new_label ("true");
                    e->emit_branch(BREZ, left_reg, tral_true);
                    e->emit_move(left_reg, 0);
                    e->emit_branch(done);
                    e->emit_label(tral_true);
                    e->emit_move(left_reg, 1);
                    
                } else if (relop_type == RELOP_NEQ) {
                    string *tral_false = e->get_new_label ("false");
                    e->emit_branch(BREZ, left_reg, tral_false);
                    e->emit_move(left_reg, 1);
                    e->emit_branch(done);
                    e->emit_label(tral_false);
                    e->emit_move(left_reg, 0);
                    
                } else if (relop_type == RELOP_LT) {
                    string *tral_true = e->get_new_label ("true");
                    e->emit_branch(BRNE, left_reg, tral_true);
                    e->emit_move(left_reg, 0);
                    e->emit_branch(done);
                    e->emit_label(tral_true);
                    e->emit_move(left_reg, 1);
                        
                } else if (relop_type == RELOP_LE) {
                    string *tral_false = e->get_new_label ("false");
                    e->emit_branch(BRPO, left_reg, tral_false);
                    e->emit_move(left_reg, 1);
                    e->emit_branch(done);
                    e->emit_label(tral_false);
                    e->emit_move(left_reg, 0);
                    
                } else if (relop_type == RELOP_GT) {
                    string *tral_true = e->get_new_label ("true");
                    e->emit_branch(BRPO, left_reg, tral_true);
                    e->emit_move(left_reg, 0);
                    e->emit_branch(done);
                    e->emit_label(tral_true);
                    e->emit_move(left_reg, 1);

                } else {
                    string *tral_false = e->get_new_label ("false");
                    e->emit_branch(BRNE, left_reg, tral_false);
                    e->emit_move(left_reg, 1);
                    e->emit_branch(done);
                    e->emit_label(tral_false);
                    e->emit_move(left_reg, 0);
                }
                e->emit_label(done);

                the_expr_hat_type = INT_T;
            } else {
                type_error(word);
            }

            return true;
        } else {
            //parse_simple_expr() failed
            return false;
        }
    
    //Predict (lamda) = ), ,, begin, then, ;
    } else if ((word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE)
        || (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA)
        || (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN)
        || (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN)
        || (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI)) {

        the_expr_hat_type = NO_T;
        return true;
    } else {
        // word isn't in predict (EXPR_HAT)
		string *expected = new string("relop, punctuation \")\", punctuation \",\", keyword \"begin\", keyword\"then\", or punctuation \";\"");
		//string *func = new string("parse_expr_hat()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
    }
}

bool Parser::parse_simple_expr(expr_type &the_simple_expr_type, Register *&expr_reg)
{
    /* SIMPLE_EXPR -> TERM SIMPLE_EXPR_PRM
	 * Predict (TERM SIMPLE_EXPR_PRM) = identifier, num, (, not, +, -
	 */
	if ((word->get_token_type() == TOKEN_ID)
        || (word->get_token_type() == TOKEN_NUM)
        || (word->get_token_type() == TOKEN_PUNC
	 	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
        || (word->get_token_type() == TOKEN_KEYWORD
	 	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)) {

        expr_type the_term_type;
		if (parse_term(the_term_type, expr_reg)) {

            expr_type simple_expr_prm_type;
			if (parse_simple_expr_prm(simple_expr_prm_type, expr_reg)) {
                if (simple_expr_prm_type == NO_T) {
                    the_simple_expr_type = the_term_type;
                } else if (the_term_type == simple_expr_prm_type) {
                    the_simple_expr_type = the_term_type;
                } else {
                    type_error(word);
                    return false;
                }
                
				return true;
			} else {
				//parse_simple_expr_prm() failed
				return false;
			}
		} else {
			//parse_term() failed
			return false;
		}
	} else {
		// word isn't in predict (SIMPLE_EXPR)
		string *expected = new string("identifier, num, punctuation \"(\", keyword \"not\", sign \"+\", or sign \"-\"");
		//string *func = new string("parse_simple_expr()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

bool Parser::parse_simple_expr_prm(expr_type &simple_expr_prm0, Register *&left_reg)
{
	/* SIMPLE_EXPR_PRM -> addop TERM SIMPLE_EXPR_PRM | lamda
	 * Predict (addop TERM SIMPLE_EXPR_PRM) = addop
	 */
	if ((word->get_token_type() == TOKEN_ADDOP
	 	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
		|| (word->get_token_type() == TOKEN_ADDOP
	 	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)
		|| (word->get_token_type() == TOKEN_ADDOP
	 	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_OR)) {

        expr_type addop_type;
        if (static_cast<AddopToken *>(word)->get_attribute() == ADDOP_OR) {
            addop_type = BOOL_T;
        } else {
            addop_type = INT_T;
        }

        addop_attr_type the_addop = static_cast<AddopToken *>(word)->get_attribute();

		//print_token(word);
		delete word; 
	 	word = lex->next_token();

        expr_type the_term_type;
        Register *right_reg;
		if (parse_term(the_term_type, right_reg)) {

            if (the_addop == ADDOP_SUB) {
                e->emit_2addr (SUB, left_reg, right_reg);
            } else {
                e->emit_2addr (ADD, left_reg, right_reg);
            }
            ra->deallocate_register (right_reg);

            expr_type simple_expr_prm1;
			if (parse_simple_expr_prm(simple_expr_prm1, left_reg)) {

                if (simple_expr_prm1 == NO_T) {
                    if (addop_type == the_term_type) {
                        simple_expr_prm0 = addop_type;
                    } else {
                        type_error(word);
                    }
                } else if (addop_type == the_term_type && the_term_type == simple_expr_prm1) {
                    simple_expr_prm0 = addop_type;
                } else {
                    type_error(word);
                }
       
				return true;
			} else {
				//parse_simple_expr_prm() failed
				return false;
			}
		} else {
			//parse_term() failed
			return false;
		}
	
	//Predict (lamda) = relop, ), ,, begin, then, ;
	} else if ((word->get_token_type() == TOKEN_RELOP)
		|| (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE)
        || (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA)
        || (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN)
        || (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN)
        || (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI)) {

        simple_expr_prm0 = NO_T;
		return true;
	} else {
		// word isn't in predict (SIMPLE_EXPR_PRM)
		string *expected = new string("relop, punctuation \")\", punctuation \",\", keyword \"begin\", keyword\"then\", or punctuation \";\"");
		//string *func = new string("parse_simple_expr_prm()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

bool Parser::parse_term(expr_type &the_term_type, Register *&expr_reg)
{
	/* TERM -> FACTOR TERM_PRM
	 * Predict (FACTOR TERM_PRM) = identifier, num, (, not, +, -
	 */
	if ((word->get_token_type() == TOKEN_ID)
        || (word->get_token_type() == TOKEN_NUM)
        || (word->get_token_type() == TOKEN_PUNC
	 	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
        || (word->get_token_type() == TOKEN_KEYWORD
	 	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
        || (word->get_token_type() == TOKEN_ADDOP
	 	    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)) {

        expr_type the_factor_type;
		if (parse_factor(the_factor_type, expr_reg)) {

            expr_type the_term_prm_type;
			if (parse_term_prm(the_term_prm_type, expr_reg)) {
                if (the_term_prm_type == NO_T) {
                    the_term_type = the_factor_type;
                } else if (the_factor_type == the_term_prm_type) {
                    the_term_type = the_factor_type;
                } else {
                    type_error(word);
                }

				return true;
			} else {
				//parse_term_prm() failed
				return false;
			}
		} else {
			//parse_factor() failed
			return false;
		}
	} else {
		// word isn't in predict (TERM)
		string *expected = new string("identifier, num, punctuation \"(\", keyword \"not\", sign \"+\", or sign \"-\"");
		//string *func = new string("parse_term()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

bool Parser::parse_term_prm(expr_type &term_prm0, Register *&left_reg)
{
	/* TERM_PRM -> mulop FACTOR TERM_PRM | lamda
	 * Predict (mulop FACTOR TERM_PRM) = mulop
	 */
	if (word->get_token_type() == TOKEN_MULOP) {
        expr_type mulop_type;
        if (static_cast<MulopToken *>(word)->get_attribute() == MULOP_AND) {
            mulop_type = BOOL_T;
        } else {
            mulop_type = INT_T;
        }

        mulop_attr_type the_mulop = static_cast<MulopToken *>(word)->get_attribute();
		//print_token(word);
		delete word; 
	 	word = lex->next_token();

        expr_type the_factor_type;
        Register *right_reg;
		if (parse_factor(the_factor_type, right_reg)) {
            expr_type term_prm1;
            
            if(the_mulop == MULOP_MUL || the_mulop == MULOP_AND) {
                e->emit_2addr(MUL, left_reg, right_reg);
            } else if (the_mulop == MULOP_DIV) {
                e->emit_2addr(DIV, left_reg, right_reg);
            }
            ra->deallocate_register(right_reg);

			if (parse_term_prm(term_prm1, left_reg)) {
                if (term_prm1 == NO_T && mulop_type == the_factor_type) {
                    term_prm0 = mulop_type;
                } else if (mulop_type == the_factor_type && the_factor_type == term_prm1) {
                    term_prm0 = mulop_type;
                } else {
                    type_error(word);
                }

				return true;
			} else {
				//parse_term_prm() failed
				return false;
			}
		} else {
			//parse_factor() failed
			return false;
		}
	
	//Predict (lamda) = addop, relop, ), ,, begin, then, ;
	} else if ((word->get_token_type() == TOKEN_ADDOP)
		|| (word->get_token_type() == TOKEN_RELOP)
		|| (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE)
        || (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA)
        || (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN)
        || (word->get_token_type() == TOKEN_KEYWORD
	    && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN)
        || (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI)) {

        term_prm0 = NO_T;
		return true;
	} else {
		// word isn't in predict (TERM_PRM)
		string *expected = new string("addop, relop, punctuation \")\", punctuation \",\", keyword \"begin\", keyword\"then\", or punctuation \";\"");
		//string *func = new string("parse_term_prm()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

bool Parser::parse_factor(expr_type &factor0, Register *&expr_reg)
{
    /* Parse_expr will leave the register containing the value
     of the expression here. */

	/* FACTOR -> identifier
				|| num
				|| ( EXPR )
				|| not FACTOR
				|| SIGN FACTOR
	 * Predict (identifier) = identifier
	 */
	if (word->get_token_type() == TOKEN_ID) {
        if (stab->is_decl(static_cast<IdToken *>(word)->get_attribute(), current_env)) {
            factor0 = stab->get_type(static_cast<IdToken *>(word)->get_attribute(), current_env);
        } else {
            undeclared_id_error(static_cast<IdToken *>(word)->get_attribute(), current_env);
        }
		//print_token(word);

        expr_reg = ra->allocate_register();
        e->emit_move(expr_reg, static_cast<IdToken *>(word)->get_attribute());
		delete word; 
	 	word = lex->next_token();
		return true;

	//Predict (num) = num
	} else if (word->get_token_type() == TOKEN_NUM) {
        factor0 = INT_T;
		//print_token(word);
        expr_reg = ra->allocate_register();
        e->emit_move(expr_reg, static_cast<NumToken *>(word)->get_attribute());
		delete word; 
	 	word = lex->next_token();
		return true;

	//Predict ( ( EXPR ) ) = (
	} else if (word->get_token_type() == TOKEN_PUNC
	 	&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN) {
		//print_token(word);
        delete word; 
	 	word = lex->next_token();

        expr_type the_expr_type;
        if (parse_expr(the_expr_type, expr_reg)) {
            if (word->get_token_type() == TOKEN_PUNC
	 	        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
                factor0 = the_expr_type;
                //print_token(word);
                delete word; 
	 	        word = lex->next_token();
                return true;
            } else {
                string *expected = new string("punctuation \")\"");
				//string *func = new string("parse_factor()");
				//func_stack(func);
      	        parse_error (expected, word);
                delete expected;
                delete word;
      	        return false; 
            }
        } else {
            //parse_expr() failed
            return false;
        }

	//Predict (not FACTOR) = not
	} else if (word->get_token_type() == TOKEN_KEYWORD
	 	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) {
		//print_token(word);
		delete word; 
	 	word = lex->next_token();
		
        expr_type factor1;
		if (parse_factor(factor1, expr_reg)) {
            if (factor1 == BOOL_T) {
                factor0 = BOOL_T;
                e->emit_1addr(NOT, expr_reg);
            } else {
                type_error(word);
            }
			return true;
		} else {
			//parse_factor()
			return false;
		}
	
	//Predict (SIGN FACTOR) = +, -
	} else if ((word->get_token_type() == TOKEN_ADDOP
	 	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
		|| (word->get_token_type() == TOKEN_ADDOP
	 	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)) {
	
        bool positive;
		if (parse_sign(positive)) {
            expr_type factor1;
			if (parse_factor(factor1, expr_reg)) {
                if (factor1 == INT_T) {
                    factor0 = INT_T;
                    if(!positive) {
                        e->emit_1addr(NEG, expr_reg);
                    }
                    
                } else {
                    type_error(word);
                }
				return true;
			} else {
				//parse_factor() failed
				return false;
			}
		} else {
			//parse_sign() failed
			return false;
		}
	} else {
		// word isn't in predict (FACTOR)
		string *expected = new string("identifier, num, punctuation \"(\", keyword \"not\", sign \"+\", or sign \"-\"");
		//string *func = new string("parse_factor()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

bool Parser::parse_sign(bool positive)
{
	/* SIGN -> + | -
	 * Predict (+) = +
	 */
	if (word->get_token_type() == TOKEN_ADDOP
	 	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD) {
		//print_token(word);
		delete word; 
	 	word = lex->next_token();
        positive = true;
		return true;
	
	//Predict (-) = -
	} else if (word->get_token_type() == TOKEN_ADDOP
	 	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB) {
		//print_token(word);
		delete word; 
	 	word = lex->next_token();
        positive = false;
		return true;
	} else {
		// word isn't in predict (SIGN)
		string *expected = new string("sign \"+\" or sign \"-\"");
		//string *func = new string("parse_sign()");
		//func_stack(func);
      	parse_error (expected, word);
      	return false;
	}
}

void Parser::parse_error (string *expected, Token *word)
{
	cout << "The parser expected " << *expected << ", but found " << *word->to_string() << endl;
}

void Parser::type_error(Token *where)
{
    cout << "Type error: " << *where->to_string() << endl;
    cout << "The parse failed." << endl;
    exit(1);
}

void Parser::undeclared_id_error(string *id, string *env)
{
    cout << "Undeclared id: " << *id << " in " << *env << endl;
    cout << "The parse failed." << endl;
    exit(1);
}

void Parser::func_stack (string *func_name)
{
	cout << "Called by function: " << *func_name << endl;
}

void Parser::print_token(Token *token)
{
	cout << *token->to_string() << endl;
}
