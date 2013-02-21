/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006-2013 Belledonne Communications, Grenoble

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include "mediastreamer-config.h"
#endif
#include "mediastreamer2_tester.h"

#include <stdio.h>
#include "CUnit/Basic.h"
#if HAVE_CU_CURSES
#include "CUnit/CUCurses.h"
#endif
#include <mediastreamer2/mediastream.h>


#if HAVE_CU_CURSES
static unsigned char curses = 0;
#endif


static test_suite_t * test_suite[] = {
	&dtmfgen_test_suite
};


static int run_test_suite(test_suite_t *suite) {
	unsigned int i;

	CU_pSuite pSuite = CU_add_suite(suite->name, NULL, NULL);

	for (i = 0; i < suite->nb_tests; i++) {
		if (NULL == CU_add_test(pSuite, suite->tests[i].name, suite->tests[i].func)) {
			return CU_get_error();
		}
	}

	return 0;
}

unsigned int mediastreamer2_tester_nb_test_suites(void) {
	return (sizeof(test_suite) / sizeof(test_suite[0]));
}

unsigned int mediastreamer2_tester_nb_tests(const char *suite_name) {
	unsigned int i;

	for (i = 0; i < mediastreamer2_tester_nb_test_suites(); i++) {
		if ((strcmp(suite_name, test_suite[i]->name) == 0) && (strlen(suite_name) == strlen(test_suite[i]->name))) {
			return test_suite[i]->nb_tests;
		}
	}

	return 0;
}

const char * mediastreamer2_tester_test_suite_name(unsigned int suite_index) {
	if (suite_index >= mediastreamer2_tester_nb_test_suites()) return NULL;
	return test_suite[suite_index]->name;
}

const char * mediastreamer2_tester_test_name(unsigned int suite_index, unsigned int test_index) {
	if (suite_index >= mediastreamer2_tester_nb_test_suites()) return NULL;
	if (test_index >= test_suite[suite_index]->nb_tests) return NULL;
	return test_suite[suite_index]->tests[test_index].name;
}

int mediastreamer2_tester_run_tests(const char *suite_name, const char *test_name) {
	unsigned int i;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	for (i = 0; i < mediastreamer2_tester_nb_test_suites(); i++) {
		run_test_suite(test_suite[i]);
	}

#if HAVE_CU_GET_SUITE
	if (suite_name){
		CU_pSuite suite;
		CU_basic_set_mode(CU_BRM_VERBOSE);
		suite=CU_get_suite(suite_name);
		if (test_name) {
			CU_pTest test=CU_get_test_by_name(test_name, suite);
			CU_basic_run_test(suite, test);
		} else
			CU_basic_run_suite(suite);
	} else
#endif
	{
#if HAVE_CU_CURSES
		if (curses) {
			/* Run tests using the CUnit curses interface */
			CU_curses_run_tests();
		}
		else
#endif
		{
			/* Run all tests using the CUnit Basic interface */
			CU_basic_set_mode(CU_BRM_VERBOSE);
			CU_basic_run_tests();
		}
	}

	CU_cleanup_registry();
	return CU_get_error();
}


#if !WINAPI_FAMILY_APP
int main (int argc, char *argv[]) {
	int i;
	char *suite_name = NULL;
	char *test_name = NULL;
	unsigned char verbose = FALSE;

	for(i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0) {
			fprintf(stderr, "%s \t--help\n"
					"\t\t\t--verbose\n"
#if HAVE_CU_GET_SUITE
					"\t\t\t--suite <suite name>\n"
					"\t\t\t--test <test name>\n"
#endif
#if HAVE_CU_CURSES
					"\t\t\t--curses\n"
#endif
					, argv[0]);
			return 0;
		} else if (strcmp(argv[i], "--verbose") == 0) {
			verbose = TRUE;
		}
#if HAVE_CU_GET_SUITE
		else if (strcmp(argv[i], "--test")==0) {
			i++;
			test_name = argv[i];
		} else if (strcmp(argv[i], "--suite") == 0) {
			i++;
			suite_name = argv[i];
		}
#endif
#if HAVE_CU_CURSES
		else if (strcmp(argv[i], "--curses") == 0) {
			i++;
			curses = 1;
		}
#endif
	}

	if (verbose) {
		setenv("MEDIASTREAMER_DEBUG", "1", 1);
	} else {
		unsetenv("MEDIASTREAMER_DEBUG");
	}

	return mediastreamer2_tester_run_tests(suite_name, test_name);
}
#endif