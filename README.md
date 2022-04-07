<img src="./assets/take.png" width=100px>

# Take(Lang)
*A language to take them all*
---

---

*Ever felt like using one language at a time is not enough?* (ignore web devs for a second)  
*Why not have them all\* with the ultimate language:* **Take(Lang)™**

*It has the simplest syntax of them all:*  
+ *`:<LANG>` specifies the following language (without the braces)*  
+ *And after that you just write in the* ***language you prefer!***

An example you say? Very well.
---

`main.tl`
```
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
print("but despite everyones' expectations,", text)
```
### outputs:
```
C: a+b=654
C: some might think this is stupid :/
Py: a-h= 578
Py: but despite everyones' expectations, this is stupid :/

```

---

*brainfuck not included