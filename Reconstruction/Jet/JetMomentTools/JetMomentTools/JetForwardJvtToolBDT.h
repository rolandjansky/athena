///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardJvtToolBDT.h
// Header file for class JetForwardJvtToolBDT
// Author: Louis Portales<louis.portales@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef FORWARDJVTTOOLBDT_JVT_FORWARDJVTTOOLBDT_H
#define FORWARDJVTTOOLBDT_JVT_FORWARDJVTTOOLBDT_H 1

// STL includes
#include <string>

// Root includes
#include "TMVA/Reader.h"
#include <TH3.h>
#include <TString.h>
#include <TFile.h>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/PropertyWrapper.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgTools/IAsgTool.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandle.h"


namespace TMVA{ class Reader; }

class JetForwardJvtToolBDT
  : public asg::AsgTool,
    virtual public IJetDecorator{
  ASG_TOOL_CLASS(JetForwardJvtToolBDT,IJetDecorator)

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
  public:


  /// Constructor with parameters:
  JetForwardJvtToolBDT(const std::string& name);

  /// Destructor:
  virtual ~JetForwardJvtToolBDT();

  // Athena algtool's Hooks
  virtual StatusCode  initialize() override;

  virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;

  // MVfJVT
  StatusCode  getInputs    ( const xAOD::Jet *jet ) const;
  float       getMVfJVT    ( const xAOD::Jet *jet, int pvind , std::vector<TVector2> pileupMomenta ) const;
  float       getFJVT      ( const xAOD::Jet *jet, int pvind , std::vector<TVector2> pileupMomenta ) const;
  bool        forwardJet   ( const xAOD::Jet *jet ) const;
  bool        centralJet   ( const xAOD::Jet *jet ) const;
  float       getDrpt      ( const xAOD::Jet *jet ) const;
  int         getJetVertex ( const xAOD::Jet *jet ) const;
  bool        passMVfJVT   ( float mvfjvt , float pt, float eta ) const;

  StatusCode tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets);
  std::vector<TVector2> calculateVertexMomenta(const xAOD::JetContainer *jets, int pvind) const;
  int getPV() const;

private:

  Gaudi::Property<std::string> m_configDir{this, "configDir", "JetPileupTag/MVfJVT/", "Configuration directory"};
  Gaudi::Property<std::vector<std::string> > m_MVconfig{this, "ConfigFiles", {
    "weights/MVfJVT_pt2030_etaHigh_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt2030_etaLow_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt3040_etaHigh_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt3040_etaLow_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt4050_etaHigh_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt4050_etaLow_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt50plus_etaHigh_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt50plus_etaLow_muHigh.May2019.weights.xml",
    "weights/MVfJVT_pt2030_etaHigh_muLow.May2019.weights.xml",
    "weights/MVfJVT_pt2030_etaLow_muLow.May2019.weights.xml",
    "weights/MVfJVT_pt3040_etaHigh_muLow.May2019.weights.xml",
    "weights/MVfJVT_pt3040_etaLow_muLow.May2019.weights.xml",
    "weights/MVfJVT_pt4050_etaHigh_muLow.May2019.weights.xml",
    "weights/MVfJVT_pt4050_etaLow_muLow.May2019.weights.xml",
    "weights/MVfJVT_pt50plus_etaHigh_muLow.May2019.weights.xml",
    "weights/MVfJVT_pt50plus_etaLow_muLow.May2019.weights.xml"
  }, "List of config file names"}; // pt [20,30,40,50,120] || |eta| [2.5,3.2,4.5] || mu [0,50,inf.];
  Gaudi::Property<std::string> m_wpFile{this, "WPfile", "MVfJVT_WPs.Nov2019.root", "WP file"};
  
  std::unique_ptr< TFile > m_wpFileIn;
  std::unique_ptr< TMVA::Reader > m_MVreader;
  std::unique_ptr< TH3D > m_mvfjvtThresh;


  Gaudi::Property<std::string> m_orLabel{this, "OverlapDec", "", "SG key for the overlap decoration"};

  Gaudi::Property<double> m_etaThresh{this, "EtaThresh", 2.5, "Eta threshold"};
  Gaudi::Property<double> m_forwardMinPt{this, "ForwardMinPt", 20e3, "Forward minimum pt"};
  Gaudi::Property<double> m_forwardMaxPt{this, "ForwardMaxPt", 120e3, "Forward maximum pt"};
  Gaudi::Property<double> m_centerMinPt{this, "CentralMinPt", 20e3, "Central minimum pt"};
  Gaudi::Property<double> m_centerMaxPt{this, "CentralMaxPt", -1, "Central maximum pt (set to -1 for no limit)"};
  Gaudi::Property<double> m_centerJvtThresh{this, "CentralJvtThresh", 0.11, "Central JVT threshold"};
  Gaudi::Property<std::string> m_jvtMomentName{this, "JvtMomentName", "Jvt", "SG key for JVT moment"};
  Gaudi::Property<double> m_centerDrptThresh{this, "CentralDrptThresh", 0.2, "Central drpt threshold"};
  Gaudi::Property<double> m_maxStochPt{this, "CentralMaxStochPt", 35e3, "Central maximum stochpt"};
  Gaudi::Property<double> m_jetScaleFactor{this, "JetScaleFactor", 0.4, "Jet scale factor"};
  Gaudi::Property<std::string> m_OP{this, "OperatingPoint", "DEFAULT", "Selected operating point, can be 'LOOSE', 'TIGHT' or 'TIGHTER'"};
  Gaudi::Property<bool> m_getTagger{this, "retrieveTagger", false, "Whether to retrieve the tagger"};
  Gaudi::Property<bool> m_isAna{this, "AnaToolMode", false, "True if running in AnaTool mode"};
  Gaudi::Property<int> m_pvind{this, "PVIndexHS", -1, ""};

  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key of input jet container"};
  
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_outMVKey{this, "OutputDecMV", "passMVFJVT", "SG key for the output MV decoration"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_mvfjvtKey{this, "MVFJVTName", "MVfJVT", "SG key for the output MVfJVT decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_cllambda2Key{this, "cllambda2Name", "LeadclSecondLambda", "SG key for the LeadclSecondLambda decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_clwidthKey{this, "cletawidthName", "LeadclWidth", "SG key for the cluster eta width decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_clisoKey{this, "clisoName", "SumclIso", "SG key for the cluster isolation decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_clemprobKey{this, "clemprobName", "SumclEMprob", "SG key for the cluster EMprob decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_cleKey{this, "cleName", "Sumcle", "SG key for the cluster energy decoration"};

  SG::ReadDecorHandleKey<xAOD::JetContainer> m_lcllambda2NTKey{this, "lcllambda2NTName", "LeadingClusterSecondLambda", "Leading cluster second lambda to use if getTagger is false"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_lcllambda2Key{this, "lcllambda2Name", "DFCommonJets_MVfJVT_LeadclSecondLambda", "Leading cluster second lambda to use if getTagger is true"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_lclwidthKey{this, "lclwidthName", "DFCommonJets_MVfJVT_LeadclWidth", "Leading cluster width to use if getTagger is true"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_lclisoKey{this, "lclisoName", "DFCommonJets_MVfJVT_SumclIso", "Leading cluster isolation to use if getTagger is true"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_lclemprobKey{this, "lclemprobName", "DFCommonJets_MVfJVT_SumclEMprob", "Leading cluster EMprob to use if getTagger is true"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_lcleKey{this, "lcleName", "DFCommonJets_MVfJVT_Sumcle", "Leading cluster energy to use if getTagger is true"};

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoName", "EventInfo", "SG key for input EventInfo"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerKey{this, "VertexContainerName", "PrimaryVertices", "SG key for input vertex container"};
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusterContainerKey{this, "CaloClusterContainerName" "CaloCalTopoClusters", "SG key for input calo cluster container"};
  SG::ReadHandleKey<xAOD::MissingETContainer> m_trkMetKey{this, "TrackMetName", "MET_Track", "SG key for input track MET container"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_isHSKey{this, "isHSName", "isJvtHS", "SG key for output isHS decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_isPUKey{this, "isPUName", "isJvtPU", "SG key for output isPU decoration"};

};
#endif //> !FORWARDJVTTOOLBDT_JVT_FORWARDJVTTOOLBDT_H
