/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixeldEdxData.h"
#include "TMath.h"
#include <cmath>
#include <algorithm>

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

double PixeldEdxData::dEdxPdf(const double dedx,const double signedP,const double mass, const std::array<double,9> &par, const int offset) const {
  const auto p = std::abs(signedP);

  double x0 = getdEdx(p,mass,par);

  double pLG[4]={1,1,1,1};

  if (m_fun=="AG") {
    x0     = std::log(x0)   - 9.4;
    pLG[1] = x0;
    pLG[0] = par[offset]+par[offset+2];
    pLG[3] = par[offset+1]+par[offset+3];
    return asymGaus(dedx, pLG[1], pLG[0], pLG[3]);
  } 
  else if (m_fun=="CB") {
    x0     = std::log(x0)   - 9.4;
    pLG[0] = x0;
    pLG[1] = par[offset];
    pLG[2] = par[offset+1];
    pLG[3] = par[offset+2];
    return crystalBall(dedx, pLG[0], pLG[1], pLG[2], pLG[3]);
  } 
  return 0;
}

double PixeldEdxData::fdEdxZero(const double x, const std::array<double,9> & par) const {
  return getdEdx(par[6],x,par)-par[5];
}

double PixeldEdxData::getPar(const int i, const int j) const {
  auto itr = m_par.find(i);
  if (itr!=m_par.end()) {
    const std::vector<double> &param = itr->second;
    if (j<(int)param.size()) {
      return param[j];
    }
  }
  return 0;
}

std::array<double,3> 
PixeldEdxData::getP(const double dedxArg, const double signedP, const int nGoodPixels) const {
  std::array<double,3> vhypo{-2,-2,-2};
  if (nGoodPixels<=0) { return vhypo; }
  const auto & par = getFirstNPar(signedP,nGoodPixels,9);
  const auto p = std::fabs(signedP);

  const auto dedx = std::log(dedxArg)-9.4;

  double pk  = dEdxPdf(dedx,p,m_kMass ,par,5);
  double pp  = dEdxPdf(dedx,p,m_pMass ,par,5);
  double ppi = dEdxPdf(dedx,p,m_piMass,par,5);

  if (ppi+pk+pp>=0) {
    vhypo = {ppi, pk,pp};
  } 
  else { 
    vhypo ={-1,-1,-1};
  }
  return vhypo;
}

std::array<double,9> 
PixeldEdxData::getFirstNPar(const double p, const int nGoodPixels, const int npar) const {
  std::array<double,9> par{};
  auto setNGoodPixels{nGoodPixels};
  if (setNGoodPixels>5) { setNGoodPixels=5; }
  if (p<0 && m_posneg) { setNGoodPixels += 5; }
  for (int i=0;i<npar;i++){
    par[i] = getPar(setNGoodPixels-1,i);
  }
  return par;
}

double PixeldEdxData::getMass(const double dedx, const double signedP, const int nGoodPixels) const {

  if (dedx<m_mindedxformass) { return m_piMass; }

  if (nGoodPixels<=0) { return -2; }
  auto  par = getFirstNPar(signedP,nGoodPixels,5);

  const auto p = std::abs(signedP);

  par[5] = dedx;
  par[6] = p;

  double xmin=0;
  double xmax=14000;

  double xmed = (xmin+xmax)*0.5;
  double prec=1e-6;

  if (dedx<15000){ return m_piMass; }

  do {
    if (fdEdxZero(xmed,par)==0) { return xmed; }
    if (fdEdxZero(xmed,par)*fdEdxZero(xmax,par)>0) { xmax=xmed; }
    else { xmin=xmed; }
    xmed=(xmin+xmax)*0.5;
  } while (std::abs(xmax-xmin)>prec);
  return xmed;
}

double PixeldEdxData::getdEdx(const double signedP, const double mass, const int nGoodPixels) const {
  if (nGoodPixels<=0) { return -2; }
  const auto & par = getFirstNPar(signedP,nGoodPixels,5);
  const auto p = std::abs(signedP);
  if      (m_bb=="3p")       { return dEdx_3p(p,mass,par); } 
  else if (m_bb=="5p")       { return dEdx_5p(p,mass,par); } 
  else if (m_bb=="5p_aleph") { return dEdx_5p_aleph(p,mass,par); }
  return 0;
}

double PixeldEdxData::getdEdx(const double signedP, const double mass, const std::array<double,9> & par) const {
  const auto p = std::abs(signedP);
  if      (m_bb=="3p")       { return dEdx_3p(p,mass,par); } 
  else if (m_bb=="5p")       { return dEdx_5p(p,mass,par); } 
  else if (m_bb=="5p_aleph") { return dEdx_5p_aleph(p,mass,par); }
  return 0;
}

// Crystal Ball distribution
double PixeldEdxData::crystalBall(const double x,const double x0,const double sig,const double alp,const double n) {
  double A = std::pow(n/std::abs(alp),n)*std::exp(-std::pow(std::abs(alp),2)*0.5);
  double B = n/std::abs(alp)-std::abs(alp);
  double pull = (x-x0)/sig;
  double N = sig*(n/alp*(1/(n-1))*std::exp(-alp*alp*0.5)+std::sqrt(M_PI_2)*(1+std::erf(alp*M_SQRT1_2)));
  if (pull<alp){
    return std::exp(-pull*pull*0.5)/N;
  }
  return A*std::pow(B+pull,-n)/N;
}

// Asymetric Gaussian distribution
double PixeldEdxData::asymGaus(const double x,const double x0,const double sig,const double asym) {
  double sigp = sig*asym;
  double sigm = sig/asym;
  double fun;
  if (x>x0) { fun = TMath::Gaus(x,x0,sigp,false); } 
  else      { fun = TMath::Gaus(x,x0,sigm,false); }
  return fun/(std::sqrt(2*M_PI)*((sigp*0.5) + (sigm*0.5)));
}

// Moyal distribution
double PixeldEdxData::moyal(const double x,const double Ep,const double R) {
  double lambda = (x-Ep)/R;
  return std::sqrt(std::exp(-lambda-std::exp(-lambda))*M_1_PI*0.5);
}

double PixeldEdxData::dEdx_5p_BG_aleph(const double xbg, const std::array<double,9>& pp) {
  double gamma = std::sqrt(xbg*xbg+1.);
  double beta  = std::sqrt(1.-1./(gamma*gamma));
  return pp[0]/std::pow(beta,pp[3])*(pp[1]-std::pow(beta,pp[3])-std::log(pp[2]+1./std::pow(beta*gamma,pp[4])));
}

double PixeldEdxData::dEdx_5p_aleph(const double p,const double mass, const std::array<double,9>& pp) {
  double bg = p/mass;
  return dEdx_5p_BG_aleph(bg,pp);
}

double PixeldEdxData::dEdx_5p_BG(const double xbg, const std::array<double,9>& pp) {
  double gamma   = std::sqrt(xbg*xbg+1);
  double beta    = std::sqrt(1.-1./(gamma*gamma));
  double fdedx   = pp[0]/std::pow(beta,pp[2])*std::log(1+std::pow(std::abs(pp[1])*beta*gamma,pp[4]))-pp[3];
  return fdedx;
}

double PixeldEdxData::dEdx_5p(const double p,const double mass, const std::array<double,9>& pp) {
  double bg = p/mass;
  return dEdx_5p_BG(bg,pp);
}

double PixeldEdxData::dEdx_BG(const double xbg, const std::array<double,9>& pp) {
  double gamma     = std::sqrt(xbg*xbg+1);
  double beta      = std::sqrt(1-1/(gamma*gamma));
  double beta2     = beta*beta;
  double gamma2    = gamma*gamma;
  double fdedx = (pp[0]/std::pow(beta2,pp[2]))*(std::log(std::abs(pp[1])*beta2*gamma2))-pp[0];
  return fdedx;
}

double PixeldEdxData::dEdx_def(const double p,const double mass, const std::array<double,9>& pp) {
  double bg = p/mass;
  return dEdx_BG(bg,pp);
}

double PixeldEdxData::dEdx_3p(const double p,const double mass, const std::array<double,9>& pp) {
  double bg = p/mass;
  return dEdx_BG(bg,pp);
}

std::vector<float> 
PixeldEdxData::getLikelihoods(const double dedx2, const double p2, const int nGoodPixels) const {
  double dedx=dedx2; 
  double p=p2*0.001; 
  std::vector<float> vecResult(3,-1); 
  if (nGoodPixels>0){
    const auto & array { getP(dedx,p,nGoodPixels)};
    std::copy(array.begin(), array.end(),vecResult.begin());
  }
  return vecResult;
}

