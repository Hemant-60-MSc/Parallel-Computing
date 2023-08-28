/*
reduction clause for + over variable sum
has been added
*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{
int   i, n;
float a[100], b[100], sum; 

/* Some initializations */
n = 100;
for (i=0; i < n; i++)
  a[i] = b[i] = i * 1.0;
sum = 0.0;

//FIXME: parallelize me using reduction clause
#pragma omp parallel for reduction(+: sum) 
  for (i=0; i < n; i++)
    sum = sum + (a[i] * b[i]);

printf("   Sum = %f\n",sum);

return 0;
}
