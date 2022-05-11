// emacs this file is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSIMTOOLS_JETTRUTHPARTICLESELECTORTOOL_H
#define JETSIMTOOLS_JETTRUTHPARTICLESELECTORTOOL_H

#include "AsgTools/AsgTool.h"

#include "xAODTruth/TruthParticle.h"

// #include "TruthHelper/IsGenInteracting.h"
// #include "TruthHelper/IsGenStable.h"


#include <map>

static const InterfaceID IID_JetTruthParticleSelectorTool("JetTruthParticleSelectorTool", 1, 0);

//using asg::AsgTool;
class JetTruthParticleSelectorTool : public asg::AsgTool
{
  ASG_TOOL_CLASS0(JetTruthParticleSelectorTool)

 public:
  
  JetTruthParticleSelectorTool(const std::string &s);
  
  virtual ~JetTruthParticleSelectorTool();
  
  virtual StatusCode initialize() override;
  
  virtual StatusCode finalize() override;


  /// alias 'selector' to operator(const IN4M*)
  bool operator()(const xAOD::TruthParticle* truth) { return this->selector(truth);}

  
  bool selector(const xAOD::TruthParticle* truth);

  void setupEvent();

  static const InterfaceID& interfaceID( ){return IID_JetTruthParticleSelectorTool;}


  bool passKinematics(const xAOD::TruthParticle* truth) const ;

protected:
  enum SelectionMode {
    Invalid = -1,
    StableNoMuonNoNu,
    MuonOnly,
    NuOnly,
    NoWZDecay,
  };

 private:
  
  
  double  m_min_pt;
  double  m_max_pt;
  double  m_maxAbsEta;
  double  m_minEta;
  double  m_maxEta;
  double  m_wzPhotonCone;
  
  // bool    m_includeMuons;
  // bool    m_useOnlyInteracting;

  bool    m_useOnlyCharged;
  bool    m_listPDGofStables;

  std::string m_selectionModeName;
  SelectionMode m_selectionMode;
  
  // TruthHelper::IsGenInteracting isInteracting;
  // TruthHelper::IsGenStable      isStable;
  
  typedef std::map< int, long long >  PDGList;
  PDGList m_pdgList;
  
  typedef std::map< int, double >     Average;
  Average m_avP;
  Average m_av2P;
  Average m_avPt;
  Average m_av2Pt;
  Average m_avEta;
  Average m_av2Eta;
  Average m_avPhi;
  Average m_av2Phi;
  Average m_avM;
  Average m_av2M;

  std::vector<const xAOD::TruthParticle*> m_wzLeptons;
};
#endif 

