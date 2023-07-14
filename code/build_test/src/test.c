#include <stdio.h>

#define PRINT_MACRO_HELPER(x)   #x
#define PRINT_MACRO(x)          #x "=" PRINT_MACRO_HELPER(x)

#define NUMBER_MACRO            3.14159
#define STRING_MACRO            "This is a string."

#pragma message(PRINT_MACRO(NUMBER_MACRO))
#pragma message(PRINT_MACRO(STRING_MACRO))
#pragma message(PRINT_MACRO(UNDEF_MACRO))

#define UNUSED(x) ((void)(x))

extern int hello(void);

int main(int para)
{
  UNUSED(para);

  printf("Hello,world!, %d\r\n", hello());
  return 0;
}

//gcc -Wunused-parameter -c test.c -o test.o
