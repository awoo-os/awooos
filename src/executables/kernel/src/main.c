#include <awoo.h>
#include <awoo/tests.h>
#include <hal.h>
#include <timer.h>
#include <ali/event.h>
#include <stdnoreturn.h>

typedef void (constructor)(void);

extern size_t __CTOR_LIST__;

noreturn void kernel_main()
{
    bool test_build = (AWOO_TEST_BUILD == 1);

    // Run all of the constructors.
    constructor **ctors = (constructor**)&__CTOR_LIST__;
    for (size_t i = 0; ctors[i] != NULL; i++) {
        ctors[i]();
    }

    hal_init();
    timer_init();
    tests_run(test_build);
    event_trigger("shell init", NULL);

    while(1){
        // Do nothing forever.
    }
}
