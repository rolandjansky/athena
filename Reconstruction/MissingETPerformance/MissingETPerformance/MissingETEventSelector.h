/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENTSELECTOR_H
#define MISSINGETEVENTSELECTOR_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "SGTools/BuiltinsClids.h"
#include "SGTools/StlVectorClids.h"

#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "MissingETPerformance/MissingETData.h"

#include "AnalysisTools/AnalysisTools.h"
//#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
//#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include "MissingETPerformance/JetVariablesTool.h"

#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/egammaParamDefs.h"

#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"

#include "JetEvent/JetCollection.h"

#include "MissingETGoodness/EtmissGoodness.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1.h"
#include "TH2.h"

static const InterfaceID IID_MissingETEventSelector("MissingETEventSelector", 1, 0);

class MissingETEventSelector : public AthAlgTool {
public:
  MissingETEventSelector(const std::string& type, const std::string& name,
                   const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MissingETEventSelector; };

  MissingETEventSelector();
  ~MissingETEventSelector();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();

  StatusCode retrieveContainers();
  void passMissingETData(MissingETData *data) {m_METdata = data;}
  MissingETData *METData() {return m_METdata;}

  bool isSelectedEvent();

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }
  inline void SetGoodnessConfig(const std::string& goodnessConfig) { m_goodnessConfig=goodnessConfig; }

 private:
  std::string m_folderName;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  /** tool handles */
/*   ToolHandle<MBTSTimeFilterTool> m_mbtsfilter; */
/*   ToolHandle<CaloTimeFilterTool> m_calofilter; */
  ToolHandle<JetVariablesTool> m_jetVariables;
  ToolHandle<AnalysisTools> m_analysisTools;

  const ElectronContainer *m_electrons;
  const JetCollection *m_jet;
  const Analysis::MuonContainer *m_muons;
  MissingETData *m_METdata;

  float m_minEventEMfraction;
  float m_minPerJetEMfraction;
  float m_maxelecIsolCone;
  float m_maxmuonIsolCone;
  float m_jetPtCut;
  float m_jetElDeltaR;
  int m_minNjets;
  int m_maxNjets;
  float m_elecPtCut;
  unsigned int m_isEM;
  float m_muonPtCut;
  float m_minRefFinalMET;
  float m_minNonIntMET;
  float m_minInvMass;
  float m_maxInvMass;
  int m_N90CellsCut;

  std::vector<std::string> m_selectionList;

  //storegate input keys
  std::string m_muonContainerKey;
  std::string m_electronContainerName;
  std::string m_jetKey;
  
  std::string m_passflagZtautau;
  std::string m_lepton_container;
  std::string m_tau_container;
  std::string m_selectionType;
  std::string m_selectionZtautau;
  
  std::string m_goodnessConfig;

  bool ANDedSelection();
  bool hasOneJet();
  bool hasNJets();
  bool hasNJetsOverlapRemoval();
  bool hasOneElectron();
  bool hasOneMuon();
  bool hasMinMissingETRefFinal();
  bool hasMinMissingETNonInt();
  bool hasZboson_mumu();
  bool hasZboson_ee();
/*   bool passesMBTSCut(); */
/*   bool passesCALOCut(); */
  bool passesMETGoodness();
  bool hasMinEventEMfraction();
  bool hasMinPerJetEMfraction();
  bool passesN90CellsCut();
  bool hasgoodZ_tautau();
  
  std::string m_selectedMuonKey;
  std::string m_selectedElectronKey;

  TH1D *m_hasZboson_mumu_nmuons;
  TH1D *m_hasZboson_mumu_leadpt;
  TH1D *m_hasZboson_mumu_secondpt;
  TH1D *m_hasZboson_mumu_InvMass;

  TH1D *m_hasZboson_ee_nelecs;
  TH1D *m_hasZboson_ee_leadpt;
  TH1D *m_hasZboson_ee_secondpt;
  TH1D *m_hasZboson_ee_InvMass;

  MET::EtmissGoodness m_metgoodness;
};

#endif // MISSINGETEVENTSELECTOR_H
