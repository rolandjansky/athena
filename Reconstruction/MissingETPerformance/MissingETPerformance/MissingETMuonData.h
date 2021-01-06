/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETMUONDATA_H
#define MISSINGETMUONDATA_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "MissingETEvent/MissingEtCalo.h"

#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"

#include "JetEvent/JetCollection.h"

#include "GeneratorObjects/McEventCollection.h"

static const InterfaceID IID_MissingETMuonData("MissingETMuonData", 1, 0);

class MissingETMuonData : public AthAlgTool {
   
 private:
  //storegate input keys
  std::string m_mcEventColKey;
  std::string m_MuonContainerKey;
  std::string m_CaloMuonContainerKey;
  std::string m_JetKey;
   
  //Muon containers
  std::vector<HepMC::ConstGenParticlePtr> m_truth_Muons;
  std::vector<const Analysis::Muon*> m_crack_Muons;
  std::vector<const Analysis::Muon*> m_MuonBoy_Spectro_Muons;
  std::vector<const Analysis::Muon*> m_MuonBoy_Track_Muons;
  std::vector<CLHEP::HepLorentzVector*> m_all_used_muons;
  //with standalone muons at |eta| < 2.5 removed
  std::vector<const Analysis::Muon*> m_MuonBoy_Spectro_Muons_standalone_fix;
  std::vector<const Analysis::Muon*> m_MuonBoy_Track_Muons_standalone_fix;
  std::vector<CLHEP::HepLorentzVector*> m_all_used_muons_standalone_fix;

  //MissingET re-calculations
  MissingET *m_MET_MuonBoy_v1510;
  MissingET *m_MET_MuonBoy_standalone_fix;

public:
  MissingETMuonData(const std::string& type, const std::string& name,
                   const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MissingETMuonData; };

  MissingETMuonData();
  ~MissingETMuonData();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();

  StatusCode retrieveMuons();
  void findCrackMuons(std::vector<const Analysis::Muon*> *crack_Muons,const Analysis::MuonContainer *muonContainer,
		      const Analysis::MuonContainer *calomuonContainer);

  bool isIsolated(const Analysis::Muon *mu, const JetCollection *jets);

  const std::vector<HepMC::ConstGenParticlePtr> &used_truth_muons() {return m_truth_Muons;}
  const std::vector<const Analysis::Muon*> &used_crack_Muons() {return m_crack_Muons;}
  const std::vector<const Analysis::Muon*> &used_MuonBoy_Spectro_Muons() {return m_MuonBoy_Spectro_Muons;}
  const std::vector<const Analysis::Muon*> &used_MuonBoy_Track_Muons() {return m_MuonBoy_Track_Muons;}
  const std::vector<CLHEP::HepLorentzVector*> &all_used_muons() {return m_all_used_muons;}

  const std::vector<const Analysis::Muon*> &used_MuonBoy_Spectro_Muons_standalone_fix() {return m_MuonBoy_Spectro_Muons_standalone_fix;}
  const std::vector<const Analysis::Muon*> &used_MuonBoy_Track_Muons_standalone_fix() {return m_MuonBoy_Track_Muons_standalone_fix;}
  const std::vector<CLHEP::HepLorentzVector*> &all_used_muons_standalone_fix() {return m_all_used_muons_standalone_fix;}

  //returns MET_MuonBoy from release 15.1.0
  const MissingET *MET_MuonBoy_v1510() {return m_MET_MuonBoy_v1510;}
  //returns MET_MuonBoy similar to 15.1.0 with standalone muons at |eta| < 2.5 removed
  const MissingET *MET_MuonBoy_standalone_fix() {return m_MET_MuonBoy_standalone_fix;}

};



#endif // MISSINGETMUONDATA_H
