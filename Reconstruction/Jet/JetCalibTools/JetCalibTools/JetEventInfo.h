/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JETEVENTINFO_H
#define JETCALIBTOOLS_JETEVENTINFO_H

class JetEventInfo {

 public:
  JetEventInfo() : m_rho(0), m_mu(0), m_npv(0), m_nJet(0), m_PVindex(0) { }
  virtual ~JetEventInfo() { }

  void setRho(double rho) { m_rho = rho; }
  void setMu(double mu) { m_mu = mu; }
  void setNPV(double NPV) { m_npv = NPV; }
  void setNjet(double nJet) { m_nJet = nJet; }
  void setPVIndex(int PVindex) { m_PVindex = PVindex; }

  double rho() { return m_rho; }
  double mu() { return m_mu; }
  double NPV() { return m_npv; }
  int nJet() { return m_nJet; }
  int PVIndex() { return m_PVindex; }

 private:
  //bool m_filled;
  double m_rho;
  double m_mu;
  double m_npv;
  int m_nJet;
  int m_PVindex;

};

#endif //> !JETCALIBTOOLS_JETEVENTINFO_H
