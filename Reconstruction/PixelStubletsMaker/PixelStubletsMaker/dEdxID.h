#ifndef DEDXID_H
#define DEDXID_H

#define NCLASS 5
#define NPAR   9
// Main Pid class
#include <string>

namespace dEdxIDFun{
  double CrystalBall(double x, double x0, double sig, double alp, double n);
  double AsymGaus(double x, double x0, double sig, double asym);
  double Moyal(double x, double Ep, double R);
  double dEdx_BG(double* xgamma, double *pp);
  double dEdx_def(double p, double mass, double *pp);
  double dEdx(double p, double mass, double *pp);
  double dEdx_3p(double p, double mass, double *pp);
  double dEdx_5p_BG(double *xgamma, double *pp);
  double dEdx_5p(double p, double mass, double *pp);
  double dEdx_5p_BG_aleph(double *xgamma, double *pp);
  double dEdx_5p_aleph(double p, double mass, double *pp);
  double fdEdx_zero(double* x, double* par);
}

namespace DEdx{
class dEdxID{
 public:
  dEdxID();
  dEdxID(const char *);
  dEdxID(const dEdxID& dedx);
  dEdxID& operator=(const dEdxID& dedx);
  ~dEdxID();
  double dEdxPdf(double dedx, double p, double mass, double *par, int offset);
  void   getP(double dedx, double p, int nGoodPixels, double *vhypo);
  void   getFirstNPar(double *par, double p, int nGoodPixels, int np);
  double getMass(double dedx, double p, int nGoodPixels);
  double getdEdx(double p, double mass, int nGoodPixels);
  double getdEdx(double p, double mass, double *par);
  double getPar(int i, int j); 
  void   setPar(int i, double *param);
  void   setPosNeg(bool posneg);
  void   setFun(std::string fun, std::string bb_fun);
  double fdEdx_zero(double* x, double* par);
 private:
  double *m_par[2*NCLASS];
  double m_pi;
  double m_p;
  double m_k;
  std::string m_fun;
  bool   m_posneg;
  double (*m_mydEdx)(double p, double mass, double *pp);
};
}
#endif




