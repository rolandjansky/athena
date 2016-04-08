#ifndef ROIBASE_H
#define ROIBASE_H

#include <cmath>
#include "Ntuple.h"


class RoIBase {

public:

  RoIBase() { m_phi = 0.0; m_eta = 0.0; m_flav=0; m_et=0; m_phiq=0; m_etaq=0; m_ptq=0; m_deta=0.2; m_dphi=0.2;};
  RoIBase(double phi,double eta) {m_phi = phi; m_eta = eta; m_flav=0; m_et=0; m_phiq=0; m_etaq=0; m_ptq=0; m_deta=0.2; m_dphi=0.2;};

  double Eta() {return m_eta;};
  double Phi() {return m_phi;};
  double Et()  {return m_et;};
  void   Eta(double eta) {m_eta = eta;};
  void   Phi(double phi) {m_phi = phi;};
  void   Et (double et)  {m_et  =  et;};

  double DEta() {return m_deta;};
  double DPhi() {return m_dphi;};
  void   DEta(double deta) {m_deta = deta;};
  void   DPhi(double dphi) {m_dphi = dphi;};

  double ZVtx(){return m_zvtx;};
  void   ZVtx(double z){m_zvtx = z;};

  double EtaQ() {return m_etaq;};
  double PhiQ() {return m_phiq;};
  double PtQ()  {return m_ptq;};
  void   EtaQ(double eta) {m_etaq = eta;};
  void   PhiQ(double phi) {m_phiq = phi;};
  void   PtQ(double ptq)  {m_ptq=ptq;};

  void   Flav(int flav) {m_flav=flav;};
  int    Flav() {return m_flav;};

  int   checkQuark(Ntuple*, double);

  double phiCorr(double);

private:

  double m_eta,m_phi,m_et;
  double m_etaq,m_phiq,m_ptq;
  double m_zvtx;
  int    m_flav;
  double m_deta,m_dphi;

};

#endif
