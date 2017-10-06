#include "PixelStubletsMaker/dEdxID.h"

#include <TMath.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

using namespace dEdxIDFun;
using namespace DEdx;

double dEdxIDFun::CrystalBall(double x, double x0, double sig, double alp, double n){

  double A = pow(n/fabs(alp),n)*exp(-pow(fabs(alp),2)/2);
  double B = n/fabs(alp)-fabs(alp);
  
  double pull = (x-x0)/sig;

  double N = sig*(n/alp*(1/(n-1))*exp(-alp*alp/2)+sqrt(M_PI/2)*(1+erf(alp/sqrt(2))));
  if (pull<alp){
    return exp(-pull*pull/2)/N;
  } else {
    return A*pow(B+pull,-n)/N;
  }

}

double dEdxIDFun::AsymGaus(double x, double x0, double sig, double asym){

  double sigp = sig*asym;
  double sigm = sig/asym;

  double fun;
  if (x>x0){
    fun = TMath::Gaus(x,x0,sigp,false); 
  } else {
    fun = TMath::Gaus(x,x0,sigm,false); 
  }

  return fun/(sqrt(2*TMath::Pi())*(sigp/2+sigm/2));

}

double dEdxIDFun::Moyal(double x, double Ep, double R){
  double lambda = (x-Ep)/R;
  return sqrt(exp(-lambda-exp(-lambda))/(2*TMath::Pi()));
}


double dEdxIDFun::dEdx_5p_BG_aleph(double* xbg, double *pp){


  double gamma = sqrt((*xbg)*(*xbg)+1);
  double beta  = sqrt(1-1/(gamma*gamma));
  return     pp[0]/pow(beta,pp[3])*(pp[1]-pow(beta,pp[3])-log(pp[2] + 1/pow(beta*gamma,pp[4])));
}
double dEdxIDFun::dEdx_5p_aleph(double p, double mass, double *pp){
  double bg = p/mass;
  return dEdx_5p_BG_aleph(&bg,pp);
}

double dEdxIDFun::dEdx_5p_BG(double* xbg, double *pp){
  double gamma   = sqrt((*xbg)*(*xbg)+1);
  double beta    = sqrt(1-1/(gamma*gamma));
  double fdedx   = pp[0]/pow(beta,pp[2])*log(1+pow(std::abs(pp[1])*beta*gamma,pp[4])) - pp[3];
  return fdedx;
}

double dEdxIDFun::dEdx_5p(double p, double mass, double *pp){
  double bg = p/mass;
  return dEdx_5p_BG(&bg,pp);
}

double dEdxIDFun::dEdx_BG(double* xbg, double *pp){
  double gamma     = sqrt((*xbg)*(*xbg)+1);
  double beta      = sqrt(1-1/(gamma*gamma));
  double beta2     = beta*beta;
  double gamma2    = gamma*gamma;
  double fdedx = (pp[0]/pow(beta2,pp[2]))*(log(std::abs(pp[1])*beta2*gamma2))- pp[0];
  return fdedx;
}

double dEdxIDFun::dEdx_def(double p, double mass, double *pp){
  double bg = p/mass;
  return dEdx_BG(&bg,pp);
}

double dEdxIDFun::dEdx_3p(double p, double mass, double *pp){
  double bg = p/mass;
  return dEdx_BG(&bg,pp);
}


double dEdxID::dEdxPdf(double dedx, double p, double mass, double *par, int offset){

  p = fabs(p);

  //double x0pi  = m_mydEdx(p,.13957,par);
  double x0    = m_mydEdx(p,mass,par);
  //double xx0pi;
  //double xx0;

  double pLG[4]={1,1,1,1};

  if (m_fun=="AG") {
    //xx0    = log(x0);
    //xx0pi  = log(x0pi);
    x0     = log(x0)   - 9.4;
    pLG[1] = x0;
    pLG[0] = par[offset]+par[offset+2];
    pLG[3] = par[offset+1]+par[offset+3];
    return AsymGaus(dedx, pLG[1], pLG[0], pLG[3]);
  } else if (m_fun=="CB") {
    x0     = log(x0)   - 9.4;
    pLG[0] = x0;
    pLG[1] = par[offset];
    pLG[2] = par[offset+1];
    pLG[3] = par[offset+2];
    return CrystalBall(dedx, pLG[0], pLG[1], pLG[2], pLG[3]);
  } else {
    return 0;
  }
  
}



double dEdxID::fdEdx_zero(double* x, double* par){
  return m_mydEdx(par[6],x[0],par)-par[5];
}


dEdxID::dEdxID(){

  setFun("CB","5p");

  double param[NPAR]={1400.99,1.42481e+06,1.11148,24897.9,1.90363,0.134277,1.10046,144.662,1};
  for (int i=0;i<2*NCLASS;i++){
    m_par[i] = new double[NPAR];
    setPar(i,param);
  }

  m_posneg = true;

  m_pi = 0.13957;
  m_k  = 0.49368;
  m_p  = 0.93827;


}

dEdxID::~dEdxID(){
  for (int i=0;i<2*NCLASS;i++){
    delete[] m_par[i];
  }
}

dEdxID::dEdxID(const dEdxID& dedx){
  for (int i=0;i<2*NCLASS;i++){
    m_par[i] = new double[NPAR];
    for (int j=0;j<NPAR;j++)
      m_par[i][j] = dedx.m_par[i][j];
  }
  m_mydEdx = dedx.m_mydEdx;
  m_posneg=dedx.m_posneg;
  m_pi=dedx.m_pi;
  m_k=dedx.m_k;
  m_p=dedx.m_p;
}

dEdxID& dEdxID::operator=(const dEdxID& dedx){
  if (this!=&dedx){
    for (int i=0;i<2*NCLASS;i++){
      for (int j=0;j<NPAR;j++)
        m_par[i][j] = dedx.m_par[i][j];
    }
    m_mydEdx = dedx.m_mydEdx;
    m_posneg=dedx.m_posneg;
    m_pi=dedx.m_pi;
    m_k=dedx.m_k;
    m_p=dedx.m_p;
  }
  return *this;
}


void  dEdxID::setFun(std::string fun, std::string bb){
  if (bb=="3p"){
    m_mydEdx = dEdxIDFun::dEdx_3p;
  } else if (bb=="5p"){
    m_mydEdx = dEdxIDFun::dEdx_5p;
  } else if (bb=="5p_aleph"){
    m_mydEdx = dEdxIDFun::dEdx_5p_aleph;
  }
  m_fun=fun;
}

void dEdxID::setPar(int i, double *param){
  for (int j=0;j<NPAR;j++){
    m_par[i][j] = param[j];
  }
}

void dEdxID::setPosNeg(bool posneg){
  m_posneg=posneg;
}

dEdxID::dEdxID(const char *file){
  std::ifstream f(file);

  if (!f){
    std::cout << "dEdx file calibration " << file << " doesn't exist " << std::endl;
  }

  int         fit_type;
  std::string fun_type;
  std::string bb_type;
  f >> fit_type;
  f >> fun_type;
  f >> bb_type;

  setFun(fun_type,bb_type);

  if (fit_type==0){
    for (int i=0;i<NCLASS;i++){
      m_par[i] = new double[NPAR];
      for (int j=0;j<NPAR;j++){
	f >> m_par[i][j];
      }
    }
    m_posneg = false;
  } else {
    for (int i=0;i<2*NCLASS;i++){
      m_par[i] = new double[NPAR];
      for (int j=0;j<NPAR;j++){
	f >> m_par[i][j];
      }
    }
    m_posneg = true;
  }

  m_pi = 0.13957;
  m_k  = 0.49368;
  m_p  = 0.93827;


}

double dEdxID::getPar(int i, int j){
  return m_par[i][j];
}

void dEdxID::getFirstNPar(double *par, double p, int nGoodPixels, int npar){

  if ( nGoodPixels>5 ) nGoodPixels=5;
  if ( p<0 && m_posneg)
    nGoodPixels += 5;
  //  std::cout << "read params" << std::endl;
  for (int i=0;i<npar;i++){
    par[i] = m_par[nGoodPixels-1][i];
    //    std::cout << i << " " << par[i] << std::endl;
  }

}

double dEdxID::getdEdx(double p, double mass, int nGoodPixels){


  double par[5];

  if (nGoodPixels<=0) return -2;
  getFirstNPar(par,p,nGoodPixels,5);
  p = fabs(p);

  return m_mydEdx(p, mass, par);

}

double dEdxID::getdEdx(double p, double mass, double *par){

  p = fabs(p);
  return m_mydEdx(p, mass, par);

}

double dEdxID::getMass(double dedx, double p, int nGoodPixels){

  double par[7];

  if (nGoodPixels<=0) return -2;
  getFirstNPar(par,p,nGoodPixels,5);
  p = fabs(p);

  par[5] = dedx;
  par[6] = p;

  double xmin=0;
  double xmax=14000;

  double xmed;  
  double prec=1e-6;

  if (dedx<15000){
    return m_pi;
  }

  xmed = (xmin+xmax)/2;
  do {
    
    if (fdEdx_zero(&xmed,par)==0)
      return xmed;
    
    if (fdEdx_zero(&xmed,par)*fdEdx_zero(&xmax,par) > 0)
      xmax=xmed;
    else 
      xmin=xmed;
    
    xmed=(xmin+xmax)/2;

  } while (std::abs(xmax-xmin)>prec);

  return xmed;
}

void dEdxID::getP(double dedx, double p, int nGoodPixels, double *vhypo){

  double par[9];

  vhypo[0] = -2;
  vhypo[1] = -2;
  vhypo[2] = -2;

  if (nGoodPixels<=0) return;
  getFirstNPar(par,p,nGoodPixels,9);
  p = fabs(p);

  dedx = log(dedx) - 9.4;


  double pk    = dEdxPdf(dedx,p,m_k ,par,5);
  double pp    = dEdxPdf(dedx,p,m_p ,par,5);
  double ppi   = dEdxPdf(dedx,p,m_pi,par,5);

  if (ppi+pk+pp>=0){
    vhypo[0] = ppi; 
    vhypo[1] =  pk;
    vhypo[2] =  pp; 
  } else { 
    vhypo[0] = -1;
    vhypo[1] = -1;
    vhypo[2] = -1;
  }

  return;

}



