///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "JetInterface/IJetModifier.h"
#include "AsgTools/IAsgTool.h"



namespace TMVA{ class Reader; }

class JetForwardJvtToolBDT
  : public asg::AsgTool,
    virtual public IJetModifier{
  ASG_TOOL_CLASS(JetForwardJvtToolBDT,IJetModifier)

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
  public:


  /// Default constructor
  JetForwardJvtToolBDT();

  /// Constructor with parameters:
  JetForwardJvtToolBDT(const std::string& name);

  /// Destructor:
  virtual ~JetForwardJvtToolBDT();

  // Athena algtool's Hooks
  virtual StatusCode  initialize() override;

  virtual int modify(xAOD::JetContainer& jetCont) const override;

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

  std::string m_configDir;
  std::vector<std::string> m_MVconfig;
  std::string m_wpFile;
  std::unique_ptr< TFile > m_wpFileIn;
  std::unique_ptr< TMVA::Reader > m_MVreader;

  std::unique_ptr< TH3D > m_mvfjvtThresh1516;
  std::unique_ptr< TH3D > m_mvfjvtThresh17;
  std::unique_ptr< TH3D > m_mvfjvtThresh18;

  std::string m_outLabelMV;
  std::string m_orLabel;

  double m_etaThresh;
  double m_forwardMinPt;
  double m_forwardMaxPt;
  double m_centerMinPt;
  double m_centerMaxPt;
  double m_centerJvtThresh;
  std::string m_jvtMomentName;
  double m_centerDrptThresh;
  double m_maxStochPt;
  double m_jetScaleFactor;
  std::string m_OP;
  bool m_getTagger;
  bool m_isAna;
  int m_pvind;

  std::string m_vtxcont;

  std::unique_ptr< SG::AuxElement::Decorator<char> > Dec_outMV;
  std::unique_ptr< SG::AuxElement::Decorator<float> > Dec_mvfjvt;
  std::unique_ptr< SG::AuxElement::Decorator<float> > Dec_cllambda2;
  std::unique_ptr< SG::AuxElement::Decorator<float> > Dec_cletawidth;
  std::unique_ptr< SG::AuxElement::Decorator<float> > Dec_cle;
  std::unique_ptr< SG::AuxElement::Decorator<float> > Dec_cliso;
  std::unique_ptr< SG::AuxElement::Decorator<float> > Dec_clemprob;

  std::unique_ptr< SG::AuxElement::Decorator<char> > Dec_isHS;
  std::unique_ptr< SG::AuxElement::Decorator<char> > Dec_isPU;

};
#endif //> !FORWARDJVTTOOLBDT_JVT_FORWARDJVTTOOLBDT_H
