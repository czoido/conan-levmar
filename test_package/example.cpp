#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "levmar.h"

#ifndef LM_DBL_PREC
#error Demo program assumes that levmar has been compiled with double precision, see LM_DBL_PREC!
#endif


/* Boggs - Tolle problem 3 (linearly constrained), minimum at (-0.76744, 0.25581, 0.62791, -0.11628, 0.25581)
 * constr1: p[0] + 3*p[1] = 0;
 * constr2: p[2] + p[3] - 2*p[4] = 0;
 * constr3: p[1] - p[4] = 0;
 */
void bt3(double *p, double *x, int m, int n, void *data)
{
register int i;
double t1, t2, t3, t4;

  t1=p[0]-p[1];
  t2=p[1]+p[2]-2.0;
  t3=p[3]-1.0;
  t4=p[4]-1.0;

  for(i=0; i<n; ++i)
    x[i]=t1*t1 + t2*t2 + t3*t3 + t4*t4;
}

void jacbt3(double *p, double *jac, int m, int n, void *data)
{
register int i, j;
double t1, t2, t3, t4;

  t1=p[0]-p[1];
  t2=p[1]+p[2]-2.0;
  t3=p[3]-1.0;
  t4=p[4]-1.0;

  for(i=j=0; i<n; ++i){
    jac[j++]=2.0*t1;
    jac[j++]=2.0*(t2-t1);
    jac[j++]=2.0*t2;
    jac[j++]=2.0*t3;
    jac[j++]=2.0*t4;
  }
}

int main()
{
register int i, j;
int problem, ret;
double p[5], // 5 is max(2, 3, 5)
	   x[16]; // 16 is max(2, 3, 5, 6, 16)
int m, n;
double opts[LM_OPTS_SZ], info[LM_INFO_SZ];

  opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20;
  opts[4]= LM_DIFF_DELTA; // relevant only if the Jacobian is approximated using finite differences; specifies forward differencing 
  //opts[4]=-LM_DIFF_DELTA; // specifies central differencing to approximate Jacobian; more accurate but more expensive to compute!

  /* Boggs-Tolle problem 3 */
    m=5; n=5;
    p[0]=2.0; p[1]=2.0; p[2]=2.0;
    p[3]=2.0; p[4]=2.0;
    for(i=0; i<n; i++) x[i]=0.0;

    {
      double A[3*5]={1.0, 3.0, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0, 1.0, -2.0,  0.0, 1.0, 0.0, 0.0, -1.0},
             b[3]={0.0, 0.0, 0.0};

    ret=dlevmar_lec_der(bt3, jacbt3, p, x, m, n, A, b, 3, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, analytic Jacobian
    //ret=dlevmar_lec_dif(bt3, p, x, m, n, A, b, 3, 1000, opts, info, NULL, NULL, NULL); // lin. constraints, no Jacobian
    }
  
  printf("Results:\n");
  printf("Levenberg-Marquardt returned %d in %g iter, reason %g\nSolution: ", ret, info[5], info[6]);
  for(i=0; i<m; ++i)
    printf("%.7g ", p[i]);
  printf("\n\nMinimization info:\n");
  for(i=0; i<LM_INFO_SZ; ++i)
    printf("%g ", info[i]);
  printf("\n");

  return 0;
}
