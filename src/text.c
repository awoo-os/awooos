#include <ali.h>
#include <ali/event.h>
#include <ali/number.h>
#include <string.h>

char *print(const char *string)
{
    event_trigger("print string", (char*)string);

    return string;
}

char *println(const char *string){
    size_t length = strlen(string);
    char *new_string = ali_malloc(length + 2);
    strncpy(new_string, string, length);
    new_string[length] = '\n';
    new_string[length + 1] = '\0';

    print(new_string);
    ali_free(new_string);

    return string;
}
