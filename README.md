# Backdoor

What I did for this homework:

1. ./interpreter/
    - Implemented an interpreter for programs written in Mini Language.
    - Files that I modified were minivm.h, minivm.c, interpreter.c
    - I added extra fields, codeSegment, dataSegment, and pc, into VMContext.
    - My interpreter loads maximum 1024 bytes for byte codes.

2. ./login/login.mini
    - Simple login program written in Mini Language.

3. ./test/test.mini
    - Simple test program written in Mini Language.

4. ./test/test.md
    - Description about test.mini program.

5. ./backdoor/
    - Simply modified interpreter.c to plant my own backdoor.
    - My backdoor-built interpreter checks the hash value of input program.
    - If hash value is matched, then it checks a specific location for gets.
    - At this location, after read user's input from stdin, check whether the input is superuser or not.

6. ./compiler/compiler.ml
    - I didn't anything for this file.

7. ./README.md
    - This document.

What I leared from this homework:

- We cannot trust any binaries :)
