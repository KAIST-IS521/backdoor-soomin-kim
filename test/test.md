# Test program specification

My test program is about an easy buffer overflow example.
Since there are many restrictions on programming with Mini language,
it just check whether the original value is changed or not.

The below is pseudo code for my example.
```
char buf[0x80];
char target[4] = "ABCD";

void main(void)
{
    printf("Before: %s\n", target);
    printf("Your input: ");
    gets(buf);
    printf("After: %s\n", target);
    if (!strncmp(target, "\xde\xad\xbe\xef", 4))
    {
        puts("$ ");
    }
}
```

Assumptions not represented in my pseudo code are,
- Global variables, `buf` and `target`, are located in heap
- `buf` is located at address 0, and
- `target` is located at address 0x80

Expected outputs are too obvious to explain,
but when you give some inputs to my program,
please be careful because some predefined strings like `Before: ` are placed at the end of heap.
