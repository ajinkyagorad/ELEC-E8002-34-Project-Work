#include <stdio.h>
#include <stdlib.h>
#include "sig_filter.h"

void sig_butterworth_filter_4_to_11(double *X, double *Y)
{
	int j,k;
	double dbuffer[5]={0,0,0,0,0};
	static const double dv0[5] = { 0.0020805671354598072, 0.0,
    -0.0041611342709196144, 0.0, 0.0020805671354598072 };
	static const double dv1[5] = { 1.0, -3.8478114968688084, 5.5721601378375674,
    -3.5994720753697242, 0.87521454825368439 };

		 for (k = 0; k < 4; k++) {
    dbuffer[k + 1] = 0.0;
  }

  for (j = 0; j < SAMPLE_SIZE; j++) {
    for (k = 0; k < 4; k++) {
      dbuffer[k] = dbuffer[k + 1];
    }

    dbuffer[4] = 0.0;
    for (k = 0; k < 5; k++) {
      dbuffer[k] += X[j] * dv0[k];
    }

    for (k = 0; k < 4; k++) {
      dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
    }

    Y[j] = dbuffer[0];
  }
}