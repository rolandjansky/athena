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
  void passMissingETData(MissingETData *data) {_METdata = data;}
  MissingETData *METData() {return _METdata;}

  bool isSelectedEvent();

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }
  inline void SetGoodnessConfig(const std::string& goodnessConfig) { _goodnessConfig=goodnessConfig; }

 private:
  std::string m_folderName;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  /** tool handles */
/*   ToolHandle<MBTSTimeFilterTool> m_mbtsfilter; */
/*   ToolHandle<CaloTimeFilterTool> m_calofilter; */
  ToolHandle<JetVariablesTool> _jetVariables;
  ToolHandle<AnalysisTools> m_analysisTools;

  const ElectronContainer *electrons;
  const JetCollection *jet;
  const Analysis::MuonContainer *muons;
  MissingETData *_METdata;

  float _minEventEMfraction;
  float _minPerJetEMfraction;
  float _maxelecIsolCone;
  float _maxmuonIsolCone;
  float _jetPtCut;
  float _jetElDeltaR;
  int _minNjets;
  int _maxNjets;
  float _elecPtCut;
  unsigned int _isEM;
  float _muonPtCut;
  float _minRefFinalMET;
  float _minNonIntMET;
  float _minInvMass;
  float _maxInvMass;
  int _N90CellsCut;

  std::vector<std::string> _selectionList;

  //storegate input keys
  std::string _muonContainerKey;
  std::string _electronContainerName;
  std::string _jetKey;
  
  std::string _passflagZtautau;
  std::string _lepton_container;
  std::string _tau_container;
  std::string _selectionType;
  std::string _selectionZtautau;
  
  std::string _goodnessConfig;

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
  
  std::string _selectedMuonKey;
  std::string _selectedElectronKey;

  TH1D *hasZboson_mumu_nmuons;
  TH1D *hasZboson_mumu_leadpt;
  TH1D *hasZboson_mumu_secondpt;
  TH1D *hasZboson_mumu_InvMass;

  TH1D *hasZboson_ee_nelecs;
  TH1D *hasZboson_ee_leadpt;
  TH1D *hasZboson_ee_secondpt;
  TH1D *hasZboson_ee_InvMass;

  MET::EtmissGoodness m_metgoodness;
};

#endif // MISSINGETEVENTSELECTOR_H
