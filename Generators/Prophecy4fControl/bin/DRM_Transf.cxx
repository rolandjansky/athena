#include "DRM.h"

int DRM::alulb4(double *ps, double *pi, double *pf){

  static double fn, pf4;
  
  if(ps[3] == ps[4]){
    pf[0] = pi[0];
    pf[1] = pi[1];
    pf[2] = pi[2];
    pf[3] = pi[3];
  }
  else{
    pf4 = (pi[0] * ps[0] + pi[1] * ps[1] + pi[2] * ps[2] + pi[3] * ps[3]) / ps[4];
    fn = (pf4 + pi[3]) / (ps[3] + ps[4]);
    pf[0] = pi[0] + fn * ps[0];
    pf[1] = pi[1] + fn * ps[1];
    pf[2] = pi[2] + fn * ps[2];
    pf[3] = pf4;
  }
  return 0;
  
}

int DRM::alulf4(double *ps, double *pi, double *pf){

  static double fn, pf4;
  
  if(ps[3] == ps[4]){
    pf[0] = pi[0];
    pf[1] = pi[1];
    pf[2] = pi[2];
    pf[3] = pi[3];
  }
  else{
    pf4 = (pi[3] * ps[3] - pi[2] * ps[2] - pi[1] * ps[1] - pi[0] * ps[0]) / ps[4]; 
    fn = (pf4 + pi[3]) / (ps[3] + ps[4]); 
    pf[0] = pi[0] - fn * ps[0]; 
    pf[1] = pi[1] - fn * ps[1]; 
    pf[2] = pi[2] - fn * ps[2]; 
    pf[3] = pf4;
  }
  return 0;
  
}

int DRM::alulob(double *ps, double *pi, double *pf){
  
  //extern int alulb4(double *, double *, double *);
  
  alulb4(ps, pi, pf);
  pf[4] = pi[4];
  return 0;
  
} 

int DRM::alulof(double *ps, double *pi, double *pf){
  
  //extern int alulf4(double *, double *,  double *);
  
  alulf4(ps, pi, pf);
  pf[4] = pi[4];
  
  return 0;
  
} 

double DRM::alupcm(double em0, double em1, double em2){

  double ret_val;
  static double emd, ems;
  
  //ems = abs(em1 + em2);
  //emd = abs(em1 - em2);
  ems = (em1 + em2);
  if(ems < 0){
    ems=-(em1 + em2);
  }
  emd = (em1 - em2);
  if(emd < 0){
    emd = -(em1 - em2);
  }
  
  if(em0 < ems || em0 < emd){
    ret_val = -1.f;
  }
  else if(em0 == ems || em0 == emd){
    ret_val = 0.f;
  }
  else{
    ret_val = std::sqrt((em0 + emd) * (em0 - emd) * (em0 + ems) * (em0 - ems)) * .5f / em0;
  }
  return ret_val;
  
}

int DRM::rescms(double *p, double *p1, double *p2, double m1, double m2){

  static double m;
  static int il;
  static double mo1, mo2, po1[5], po2[5], pcm, pcmo;
  //extern int alulob(double *, double *, double *);
  //extern double alupcm(double , double , double );
  //extern int alulof(double *, double *, double *);
  
  m = p[4];
  mo1 = p1[4];
  mo2 = p2[4];
  
  alulof(p, p1, po1);
  alulof(p, p2, po2);
  
  if (std::max(mo1,mo2) < 1e-6){
    pcmo = m / 2.;
  }
  else{
    pcmo = alupcm(m, mo1, mo2);
  }
  pcm = alupcm(m, m1, m2);
  
  for (il = 0; il < 4; il++) {
    po1[il] = pcm / pcmo * po1[il];
    po2[il] = pcm / pcmo * po2[il];
  }
  
  po1[3] = std::sqrt(pcm*pcm + m1*m1);
  po2[3] = std::sqrt(pcm*pcm + m2*m2);
  po1[4] = m1;
  po2[4] = m2;
  
  alulob(p, po1, p1);
  alulob(p, po2, p2);	
  
  return 0;
  
}
