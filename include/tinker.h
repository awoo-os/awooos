#ifndef TINKER_H
#define TINKER_H

#ifndef TINKER_VERBOSE
/// 0 = normal
/// 1 = verbose
#define TINKER_VERBOSE 0
#endif

typedef int (TinkerPutcharFn)(int c);
// This would normally be set to NULL, but this avoids needing that defined.
static TinkerPutcharFn *_tinker_putchar = 0;

typedef int (TinkerTestcaseFn)(void);

#define TINKER_TEST_NAME_BUFFER_LENGTH 1024
typedef struct TestCase_s {
    char name[TINKER_TEST_NAME_BUFFER_LENGTH];
    TinkerTestcaseFn *func;
} TestCase;

void _tinker_add_test(const char *n, TinkerTestcaseFn *func);
int tinker_run_tests(TinkerPutcharFn *putcharfn);
void _tinker_print_results(int status,
        const char *message, const char *file, unsigned long line);

char *tinker_print(const char *string);

#define tinker_add_test(NAME) _tinker_add_test(#NAME, test_##NAME)

#define TEST_SUCCESS            0
#define TEST_FAILURE            1
#define TEST_SKIP               2
#define TEST_ASSERTION_FAILURE  3

#define TEST_RETURN2(STATUS, MESSAGE, PASSED_ASSERTIONS)            \
    _tinker_print_results(STATUS, MESSAGE, __FILE__, __LINE__);    \
    return PASSED_ASSERTIONS

#define TEST_RETURN(STATUS, MESSAGE) TEST_RETURN2(STATUS, MESSAGE, 0)

// NOTE: the type for `passed_assertions` *must* match the return type
//       for `TinkerTestcaseFn`.
#define TEST_HAS_ASSERTIONS() int passed_assertions = 0;

#define TEST_ASSERT(CODE)   if (CODE) {                             \
    passed_assertions += 1;             \
    if (TINKER_VERBOSE) { \
        tinker_print("-- "); tinker_print(#CODE); tinker_print("\n"); \
    } else { \
        tinker_print(".");                  \
    } \
} else {                                \
    TEST_RETURN2(TEST_ASSERTION_FAILURE,\
#CODE, passed_assertions);      \
}

#define TEST_ASSERTIONS_RETURN()    TEST_RETURN2(TEST_SUCCESS, "All assertions passed.", passed_assertions)

#endif
