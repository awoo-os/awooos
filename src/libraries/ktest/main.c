#include <ktest.h>
#include <stddef.h>
#include <badmalloc.h>
#include <kernel.h>

/*
 * Test suite for the AwooOS kernel.
 * 
 * How to add a test:
 *    Assume for this example your test is named "cow"
 * 
 *    TestReturn *TestCow()
 *    {
 *       TEST_RETURN(status, message);
 *    }
 *
 *    status is one of the TEST_* variables in include/kernel/colpa/test.h:
 *     - TEST_SUCCESS (test passed)
 *     - TEST_FAIL    (test failed, nonfatal)
 *     - TEST_FATAL   (test failed, fatal - will not let the system boot)
 *
 *    message is a string or NULL.
 * 
 *    If message is NULL, no explanatory message is printed.
 */

static TestCase *firsttest = NULL;
static TestCase *lasttest = NULL;

static const char *test_status_messages[4] = {
	"PASS",
	"FAIL",
	"FATAL",
	"SKIP",
};

TestCase *test_add(const char *n, TestReturn* (*fn)())
{
    TestCase *t = (TestCase*)badmalloc(sizeof(TestCase));
    TestCase *tmp;

    t->name = n;
    t->func = fn;

    if(firsttest == NULL) {
        firsttest = t;
        firsttest->prev = NULL;
        firsttest->next = NULL;
    }
    if(lasttest == NULL) {
        lasttest = t;

        for(tmp = firsttest; tmp->next != NULL; tmp = tmp->next)
            ;

        if(tmp != lasttest) {
            lasttest->prev = tmp;
            tmp->next = lasttest;
        }

    } else {
        lasttest->next = t;
        lasttest->prev = lasttest;
        lasttest = t;
        lasttest->next = NULL;
    }
    return t;
}

int test_run(size_t ran, TestCase *test)
{
    TestReturn *ret;

    ret = test->func();

    if(ret->status == TEST_SUCCESS) {
        kprint(".");
    } else {
        if (ran > 0) {
            kprint("\n");
        }

        kprint(test_status_messages[ret->status]);
        kprint(": ");
        kprint(test->name);
        kprint(": ");
        kprint(ret->message);
        kprint("\n");
    }

    return ret->status;
}

bool test_run_all()
{
    TestCase *test;
    int status;
    size_t ran = 0;
    size_t passed = 0;
    size_t failed = 0;
    size_t fatal  = 0;
    size_t skipped = 0;

    kprint("\nRunning tests:\n\n");

    for(test = firsttest; test != NULL; test=test->next) {
        status = test_run(ran, test);
        ran++;

        if(status == TEST_SUCCESS) {
            passed++;
        } else if (status == TEST_FAIL) {
            failed++;
        } else if (status == TEST_FATAL) {
            failed++;
            fatal++;
        } else if (status == TEST_SKIP) {
            skipped++;
        }
    }

    kprint("\n\n");

    /*printf("Total tests: %i\n", ran);
      printf("     Passed: %i\n", passed);
      printf("     Failed: %i\n", failed);
      printf("      Fatal: %i\n", fatal);
      printf("    Skipped: %i\n\n", skipped);*/

    if (fatal > 0) {
        kprint("Fatal error encountered; not booting.\n");
        return false;
    } else if (failed > passed) {
        kprint("Majority of tests failed; not booting.\n");
        return false;
    }

    return true;
}