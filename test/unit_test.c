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
 * Unit tests for gfal
 * @author : Devresse Adrien
 * @version : 0.0.1
 */

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include "common/gfal__test_verbose.c"
#include "common/gfal__test_catalog.c"

Suite* common_suite (void)
{
  Suite *s = suite_create ("Common");

  /* verbose test case */
  TCase *tc_core = tcase_create ("Verbose");
  tcase_add_test (tc_core, test_verbose_set_get);
  suite_add_tcase (s, tc_core);
  /* catalog */
  TCase *tc_cata = tcase_create ("Catalog");
  tcase_add_test (tc_cata, test_get_cat_type);
  suite_add_tcase (s, tc_cata);
  return s;
}




int main (int argc, char** argv)
{
  int number_failed;
  Suite *s = common_suite ();
  SRunner *sr = srunner_create (s);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  if( number_failed > 0){
	  fprintf(stderr, "Error occured while unit tests");
  }
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}
