# include <stdio.h>

main()
{
  char str[] = "Hello World!";
  char *f =" This is the string";
  char *first = "Times";
  char *second = "Times";
  
  *(str +2) = 'x';
  
  *(f + 1) = 'x';
  printf("%s \n %s", str, f);
  
  printf("\n %p %p",first, second);
  
}
