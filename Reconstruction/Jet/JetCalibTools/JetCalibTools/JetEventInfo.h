/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JETEVENTINFO_H
#define JETCALIBTOOLS_JETEVENTINFO_H

class JetEventInfo {

 public:
  JetEventInfo() : m_filled(false), m_rho(0), m_mu(0), m_npv(0) { }
  virtual ~JetEventInfo() { }

  void setRho(double rho) { m_rho = rho; }
  void setMu(double mu) { m_mu = mu; }
  void setNPV(double NPV) { m_npv = NPV; }

  double rho() { return m_rho; }
  double mu() { return m_mu; }
  double NPV() { return m_npv; }

 private:
  bool m_filled;
  double m_rho;
  double m_mu;
  double m_npv;

};

#endif //> !JETCALIBTOOLS_JETEVENTINFO_H
