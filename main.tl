:c
int a = 677;
int b = -23;
char* text = "this is stupid :/";

:c
#include <tcclib.h>

extern int a;
extern int b;
extern char* text;

int run() {
    printf("C: a+b=%d\n", (a+b));
    printf("C: some might think %s\n", text);
    return 0;
}

:python
def huh():
    h = 99
    print("Py: a-h=", (a-h))

huh()
print("Py: but despite everyones' expectations,", text)
