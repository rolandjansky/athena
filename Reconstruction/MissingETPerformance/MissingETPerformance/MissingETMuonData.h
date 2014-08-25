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
  std::string _mcEventColKey;
  std::string _MuonContainerKey;
  std::string _CaloMuonContainerKey;
  std::string _JetKey;
   
  //Muon containers
  std::vector<const HepMC::GenParticle*> _truth_Muons;
  std::vector<const Analysis::Muon*> _crack_Muons;
  std::vector<const Analysis::Muon*> _MuonBoy_Spectro_Muons;
  std::vector<const Analysis::Muon*> _MuonBoy_Track_Muons;
  std::vector<CLHEP::HepLorentzVector*> _all_used_muons;
  //with standalone muons at |eta| < 2.5 removed
  std::vector<const Analysis::Muon*> _MuonBoy_Spectro_Muons_standalone_fix;
  std::vector<const Analysis::Muon*> _MuonBoy_Track_Muons_standalone_fix;
  std::vector<CLHEP::HepLorentzVector*> _all_used_muons_standalone_fix;

  //MissingET re-calculations
  MissingET *_MET_MuonBoy_v1510;
  MissingET *_MET_MuonBoy_standalone_fix;

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
  void findCrackMuons(std::vector<const Analysis::Muon*> *_crack_Muons,const Analysis::MuonContainer *muonContainer,
		      const Analysis::MuonContainer *calomuonContainer);

  bool isIsolated(const Analysis::Muon *mu, const JetCollection *jets);

  const std::vector<const HepMC::GenParticle*> &used_truth_muons() {return _truth_Muons;}
  const std::vector<const Analysis::Muon*> &used_crack_Muons() {return _crack_Muons;}
  const std::vector<const Analysis::Muon*> &used_MuonBoy_Spectro_Muons() {return _MuonBoy_Spectro_Muons;}
  const std::vector<const Analysis::Muon*> &used_MuonBoy_Track_Muons() {return _MuonBoy_Track_Muons;}
  const std::vector<CLHEP::HepLorentzVector*> &all_used_muons() {return _all_used_muons;}

  const std::vector<const Analysis::Muon*> &used_MuonBoy_Spectro_Muons_standalone_fix() {return _MuonBoy_Spectro_Muons_standalone_fix;}
  const std::vector<const Analysis::Muon*> &used_MuonBoy_Track_Muons_standalone_fix() {return _MuonBoy_Track_Muons_standalone_fix;}
  const std::vector<CLHEP::HepLorentzVector*> &all_used_muons_standalone_fix() {return _all_used_muons_standalone_fix;}

  //returns MET_MuonBoy from release 15.1.0
  const MissingET *MET_MuonBoy_v1510() {return _MET_MuonBoy_v1510;}
  //returns MET_MuonBoy similar to 15.1.0 with standalone muons at |eta| < 2.5 removed
  const MissingET *MET_MuonBoy_standalone_fix() {return _MET_MuonBoy_standalone_fix;}

};



#endif // MISSINGETMUONDATA_H
