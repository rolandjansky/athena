#ifndef _ASYMMETRY_H_
#define _ASYMMETRY_H_
// Calculates error bars in an efficiency measurement where N successes were
// recorded out of M trials. 
#define ASYMM_NORMAL 0
#define ASYMM_ERROR  1
int asymmetry (int M, int N, double & xmean, double & xplus, double & xminus);


#endif
