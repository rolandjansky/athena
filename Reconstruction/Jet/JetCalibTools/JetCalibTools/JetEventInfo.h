/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JETEVENTINFO_H
#define JETCALIBTOOLS_JETEVENTINFO_H

class JetEventInfo {

 public:
  JetEventInfo() : m_rho(0), m_mu(0), m_npv(0), m_nJet(0), m_PVindex(0), m_runNumber(0), m_bcidDistanceFromFront(0), m_bcidGapBeforeTrain(0), m_bcidGapBeforeTrainMinus12(0) { }
  virtual ~JetEventInfo() { }

  void setRho(double rho) { m_rho = rho; }
  void setMu(double mu) { m_mu = mu; }
  void setNPV(double NPV) { m_npv = NPV; }
  void setNjet(int nJet) { m_nJet = nJet; }
  void setPVIndex(int PVindex) { m_PVindex = PVindex; }
  void setRunNumber(UInt_t RunNumber) { m_runNumber = RunNumber; }
  void setBcidDistanceFromFront(Int_t BcidDistanceFromFront) { m_bcidDistanceFromFront = BcidDistanceFromFront; }
  void setBcidGapBeforeTrain(Int_t BcidGapBeforeTrain) { m_bcidGapBeforeTrain = BcidGapBeforeTrain; }
  void setBcidGapBeforeTrainMinus12(Int_t BcidGapBeforeTrainMinus12) { m_bcidGapBeforeTrainMinus12 = BcidGapBeforeTrainMinus12; }

  double rho() { return m_rho; }
  double mu() { return m_mu; }
  double NPV() { return m_npv; }
  int nJet() { return m_nJet; }
  int PVIndex() { return m_PVindex; }
  UInt_t runNumber() { return m_runNumber; }
  Int_t bcidDistanceFromFront() { return m_bcidDistanceFromFront; }
  Int_t bcidGapBeforeTrain() { return m_bcidGapBeforeTrain; }
  Int_t bcidGapBeforeTrainMinus12() { return m_bcidGapBeforeTrainMinus12; }

 private:
  //bool m_filled;
  double m_rho;
  double m_mu;
  double m_npv;
  int m_nJet;
  int m_PVindex;
  UInt_t m_runNumber;
  Int_t m_bcidDistanceFromFront;
  Int_t m_bcidGapBeforeTrain;
  Int_t m_bcidGapBeforeTrainMinus12;

};

#endif //> !JETCALIBTOOLS_JETEVENTINFO_H
