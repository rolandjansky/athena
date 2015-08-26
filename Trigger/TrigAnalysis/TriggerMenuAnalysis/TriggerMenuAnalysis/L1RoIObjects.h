/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __L1RoIObjects_h__
#define __L1RoIObjects_h__
/*
  L1RoIObjects.h
*/
#include <vector>
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/JetET_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"


class L1RoIObjects {
public:
  L1RoIObjects();
  ~L1RoIObjects();

  std::vector<const Muon_ROI*>& getMuonRoIs() { return mMuonRoIs; }
  const std::vector<const Muon_ROI*>& getMuonRoIs() const { return mMuonRoIs; }

  std::vector<const EmTau_ROI*>& getEmTauRoIs() { return mEmTauRoIs; }
  const std::vector<const EmTau_ROI*>& getEmTauRoIs() const {
    return mEmTauRoIs;
  }

  std::vector<const Jet_ROI*>& getJetRoIs() { return mJetRoIs; }
  const std::vector<const Jet_ROI*>& getJetRoIs() const { return mJetRoIs; }

  std::vector<const JetET_ROI*>& getJetEtRoIs() { return mJetEtRoIs; }
  const std::vector<const JetET_ROI*>& getJetEtRoIs() const {
    return mJetEtRoIs;
  }

  std::vector<const EnergySum_ROI*>& getEnergyRoIs() { return mEnergyRoIs; }
  const std::vector<const EnergySum_ROI*>& getEnergyRoIs() const {
    return mEnergyRoIs;
  }

  void setMuonRoIs(const std::vector<Muon_ROI>& x);
  void setEmTauRoIs(const std::vector<EmTau_ROI>& x);
  void setJetRoIs(const std::vector<Jet_ROI>& x);
  void setJetEtRoIs(const std::vector<JetET_ROI>& x);
  void setEnergyRoIs(const std::vector<EnergySum_ROI>& x);

  void addMuonRoI(const Muon_ROI* x) { mMuonRoIs.push_back(x); }
  void addEmTauRoI(const EmTau_ROI* x) { mEmTauRoIs.push_back(x); }
  void addJetRoI(const Jet_ROI* x) { mJetRoIs.push_back(x); }
  void addJetEtRoI(const JetET_ROI* x) { mJetEtRoIs.push_back(x); }
  void addEnergyRoI(const EnergySum_ROI* x) { mEnergyRoIs.push_back(x); }


protected:
  std::vector<const Muon_ROI*> mMuonRoIs;
  std::vector<const EmTau_ROI*> mEmTauRoIs;
  std::vector<const Jet_ROI*> mJetRoIs;
  std::vector<const JetET_ROI*> mJetEtRoIs;
  std::vector<const EnergySum_ROI*> mEnergyRoIs;

};

#endif // __L1RoIObjects_h__
