/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_L1ROIOBJECTS_H
#define TRIGGERMENUANALYSIS_L1ROIOBJECTS_H
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

  std::vector<const Muon_ROI*>& getMuonRoIs() { return m_muonRoIs; }
  const std::vector<const Muon_ROI*>& getMuonRoIs() const { return m_muonRoIs; }

  std::vector<const EmTau_ROI*>& getEmTauRoIs() { return m_emTauRoIs; }
  const std::vector<const EmTau_ROI*>& getEmTauRoIs() const {
    return m_emTauRoIs;
  }

  std::vector<const Jet_ROI*>& getJetRoIs() { return m_jetRoIs; }
  const std::vector<const Jet_ROI*>& getJetRoIs() const { return m_jetRoIs; }

  std::vector<const JetET_ROI*>& getJetEtRoIs() { return m_jetEtRoIs; }
  const std::vector<const JetET_ROI*>& getJetEtRoIs() const {
    return m_jetEtRoIs;
  }

  std::vector<const EnergySum_ROI*>& getEnergyRoIs() { return m_energyRoIs; }
  const std::vector<const EnergySum_ROI*>& getEnergyRoIs() const {
    return m_energyRoIs;
  }

  void setMuonRoIs(const std::vector<Muon_ROI>& x);
  void setEmTauRoIs(const std::vector<EmTau_ROI>& x);
  void setJetRoIs(const std::vector<Jet_ROI>& x);
  void setJetEtRoIs(const std::vector<JetET_ROI>& x);
  void setEnergyRoIs(const std::vector<EnergySum_ROI>& x);

  void addMuonRoI(const Muon_ROI* x) { m_muonRoIs.push_back(x); }
  void addEmTauRoI(const EmTau_ROI* x) { m_emTauRoIs.push_back(x); }
  void addJetRoI(const Jet_ROI* x) { m_jetRoIs.push_back(x); }
  void addJetEtRoI(const JetET_ROI* x) { m_jetEtRoIs.push_back(x); }
  void addEnergyRoI(const EnergySum_ROI* x) { m_energyRoIs.push_back(x); }


protected:
  std::vector<const Muon_ROI*> m_muonRoIs;
  std::vector<const EmTau_ROI*> m_emTauRoIs;
  std::vector<const Jet_ROI*> m_jetRoIs;
  std::vector<const JetET_ROI*> m_jetEtRoIs;
  std::vector<const EnergySum_ROI*> m_energyRoIs;

};

#endif // TRIGGERMENUANALYSIS_L1ROIOBJECTS_H
