/*
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
 


/**
 * Unit tests for gfal based on the cgreen library
 * @author : Devresse Adrien
 * @version : 0.0.1
 */

#include <cgreen/cgreen.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_gridftp.h"

TestSuite * gridftp_suite (void)
{
   TestSuite *s1 = create_test_suite();
  // verbose test case /
   add_test(s1, load_gridftp);
   add_test(s1, handle_creation);
   add_test(s1, gridftp_parseURL);

   return s1;
 }
 

 


int main (int argc, char** argv)
{
	//fprintf(stderr, " tests : %s ", getenv("LD_LIBRARY_PATH"));
	TestSuite *global = create_test_suite();
	add_suite(global, gridftp_suite());
    if (argc > 1){
        return run_single_test(global, argv[1], create_text_reporter());
    }
	return run_test_suite(global, create_text_reporter());
}
