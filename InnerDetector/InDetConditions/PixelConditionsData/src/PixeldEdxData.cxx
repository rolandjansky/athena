/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixeldEdxData.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TMath.h"

void PixeldEdxData::setPar(const int i, const double param) {
  m_par[i].push_back(param);
}

void PixeldEdxData::setPosNeg(const bool posneg) {
  m_posneg = posneg;
}

void PixeldEdxData::setFunctionType(const std::string &fun) {
  m_fun = fun;
}

void PixeldEdxData::setBetheBlochType(const std::string &bb) {
  m_bb = bb;
}

void PixeldEdxData::setMinimumdEdxForMass(const double mindedxMass) {
  m_mindedxformass = mindedxMass;
}

double PixeldEdxData::dEdxPdf(double dedx,double p,double mass, std::array<double,9> par, int offset) const {
  p = fabs(p);

  double x0 = getdEdx(p,mass,par);

  double pLG[4]={1,1,1,1};

  if (m_fun=="AG") {
    x0     = log(x0)   - 9.4;
    pLG[1] = x0;
    pLG[0] = par[offset]+par[offset+2];
    pLG[3] = par[offset+1]+par[offset+3];
    return Func2(dedx, pLG[1], pLG[0], pLG[3]);
  } 
  else if (m_fun=="CB") {
    x0     = log(x0)   - 9.4;
    pLG[0] = x0;
    pLG[1] = par[offset];
    pLG[2] = par[offset+1];
    pLG[3] = par[offset+2];
    return Func1(dedx, pLG[0], pLG[1], pLG[2], pLG[3]);
  } 
  return 0;
}

double PixeldEdxData::fdEdx_zero(double x, std::array<double,9> par) const {
  return getdEdx(par[6],x,par)-par[5];
}

double PixeldEdxData::getPar(int i, int j) const {
  auto itr = m_par.find(i);
  if (itr!=m_par.end()) {
    const std::vector<double> &param = itr->second;
    if (j<(int)param.size()) {
      return param[j];
    }
  }
  return 0;
}

void PixeldEdxData::getP(double dedx, double p, int nGoodPixels, std::array<double,3> vhypo) const {
  std::array<double,9> par;

  vhypo[0] = -2;
  vhypo[1] = -2;
  vhypo[2] = -2;

  if (nGoodPixels<=0) { return; }
  getFirstNPar(par,p,nGoodPixels,9);
  p = fabs(p);

  dedx = log(dedx)-9.4;

  double pk  = dEdxPdf(dedx,p,m_k ,par,5);
  double pp  = dEdxPdf(dedx,p,m_p ,par,5);
  double ppi = dEdxPdf(dedx,p,m_pi,par,5);

  if (ppi+pk+pp>=0) {
    vhypo[0] = ppi; 
    vhypo[1] =  pk;
    vhypo[2] =  pp; 
  } 
  else { 
    vhypo[0] = -1;
    vhypo[1] = -1;
    vhypo[2] = -1;
  }
  return;
}

void PixeldEdxData::getFirstNPar(std::array<double,9> par, double p, int nGoodPixels, int npar) const {
  if (nGoodPixels>5) { nGoodPixels=5; }
  if (p<0 && m_posneg) { nGoodPixels += 5; }
  for (int i=0;i<npar;i++){
    par[i] = getPar(nGoodPixels-1,i);
  }
}

double PixeldEdxData::getMass(double dedx, double p, int nGoodPixels) const {

  if (dedx<m_mindedxformass) { return m_pi; }

  std::array<double,9> par;
  if (nGoodPixels<=0) { return -2; }
  getFirstNPar(par,p,nGoodPixels,5);

  p = fabs(p);

  par[5] = dedx;
  par[6] = p;

  double xmin=0;
  double xmax=14000;

  double xmed = (xmin+xmax)/2;
  double prec=1e-6;

  if (dedx<15000){ return m_pi; }

  do {
    if (fdEdx_zero(xmed,par)==0) { return xmed; }
    if (fdEdx_zero(xmed,par)*fdEdx_zero(xmax,par)>0) { xmax=xmed; }
    else { xmin=xmed; }
    xmed=(xmin+xmax)/2;
  } while (std::abs(xmax-xmin)>prec);
  return xmed;
}

double PixeldEdxData::getdEdx(double p, double mass, int nGoodPixels) const {
  std::array<double,9> par;

  if (nGoodPixels<=0) { return -2; }
  getFirstNPar(par,p,nGoodPixels,5);
  p = fabs(p);
  if      (m_bb=="3p")       { return dEdx_3p(p,mass,par); } 
  else if (m_bb=="5p")       { return dEdx_5p(p,mass,par); } 
  else if (m_bb=="5p_aleph") { return dEdx_5p_aleph(p,mass,par); }
  return 0;
}

double PixeldEdxData::getdEdx(double p, double mass, std::array<double,9> par) const {
  p = fabs(p);
  if      (m_bb=="3p")       { return dEdx_3p(p,mass,par); } 
  else if (m_bb=="5p")       { return dEdx_5p(p,mass,par); } 
  else if (m_bb=="5p_aleph") { return dEdx_5p_aleph(p,mass,par); }
  return 0;
}

// Crystal Ball distribution
double PixeldEdxData::Func1(double x,double x0,double sig,double alp,double n) const {
  double A = pow(n/fabs(alp),n)*exp(-pow(fabs(alp),2)/2);
  double B = n/fabs(alp)-fabs(alp);
  double pull = (x-x0)/sig;
  double N = sig*(n/alp*(1/(n-1))*exp(-alp*alp/2)+sqrt(M_PI/2)*(1+erf(alp/sqrt(2))));
  if (pull<alp){
    return exp(-pull*pull/2)/N;
  }
  return A*pow(B+pull,-n)/N;
}

// Asymetric Gaussian distribution
double PixeldEdxData::Func2(double x,double x0,double sig,double asym) const {
  double sigp = sig*asym;
  double sigm = sig/asym;
  double fun;
  if (x>x0) { fun = TMath::Gaus(x,x0,sigp,false); } 
  else      { fun = TMath::Gaus(x,x0,sigm,false); }
  return fun/(sqrt(2*TMath::Pi())*(sigp/2+sigm/2));
}

// Moyal distribution
double PixeldEdxData::Func3(double x,double Ep,double R) const {
  double lambda = (x-Ep)/R;
  return sqrt(exp(-lambda-exp(-lambda))/(2*TMath::Pi()));
}

double PixeldEdxData::dEdx_5p_BG_aleph(double xbg, std::array<double,9>& pp) const {
  double gamma = sqrt(xbg*xbg+1);
  double beta  = sqrt(1-1/(gamma*gamma));
  return pp[0]/pow(beta,pp[3])*(pp[1]-pow(beta,pp[3])-log(pp[2]+1/pow(beta*gamma,pp[4])));
}

double PixeldEdxData::dEdx_5p_aleph(double p,double mass, std::array<double,9>& pp) const {
  double bg = p/mass;
  return dEdx_5p_BG_aleph(bg,pp);
}

double PixeldEdxData::dEdx_5p_BG(double xbg, std::array<double,9>& pp) const {
  double gamma   = sqrt(xbg*xbg+1);
  double beta    = sqrt(1-1/(gamma*gamma));
  double fdedx   = pp[0]/pow(beta,pp[2])*log(1+pow(std::abs(pp[1])*beta*gamma,pp[4]))-pp[3];
  return fdedx;
}

double PixeldEdxData::dEdx_5p(double p,double mass, std::array<double,9>& pp) const {
  double bg = p/mass;
  return dEdx_5p_BG(bg,pp);
}

double PixeldEdxData::dEdx_BG(double xbg, std::array<double,9>& pp) const {
  double gamma     = sqrt(xbg*xbg+1);
  double beta      = sqrt(1-1/(gamma*gamma));
  double beta2     = beta*beta;
  double gamma2    = gamma*gamma;
  double fdedx = (pp[0]/pow(beta2,pp[2]))*(log(std::abs(pp[1])*beta2*gamma2))-pp[0];
  return fdedx;
}

double PixeldEdxData::dEdx_def(double p,double mass, std::array<double,9>& pp) const {
  double bg = p/mass;
  return dEdx_BG(bg,pp);
}

double PixeldEdxData::dEdx_3p(double p,double mass, std::array<double,9>& pp) const {
  double bg = p/mass;
  return dEdx_BG(bg,pp);
}

std::vector<float> PixeldEdxData::getLikelihoods(double dedx2, double p2, int nGoodPixels) const {
  double dedx=dedx2; 
  double p=p2/1000; 
  std::vector<float> vhypo; 
  vhypo.resize(3); 
  vhypo[0] = vhypo[1] = vhypo[2] = -1;
  if (nGoodPixels<=0) return vhypo;
  std::array<double,3> vhypo2;
  vhypo2[0] = vhypo2[1] = vhypo2[2] = -1;
  getP(dedx,p,nGoodPixels,vhypo2);
  vhypo[0]=vhypo2[0];
  vhypo[1]=vhypo2[1];
  vhypo[2]=vhypo2[2];
  return vhypo;
}

