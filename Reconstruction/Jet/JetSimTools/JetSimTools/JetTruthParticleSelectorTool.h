// emacs this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  
  virtual StatusCode initialize();
  
  virtual StatusCode finalize();


  /// alias 'selector' to operator(const IN4M*)
  bool operator()(const xAOD::TruthParticle* truth) const { return this->selector(truth);}

  
  bool selector(const xAOD::TruthParticle* truth) const ;

  void setupEvent() const ;

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
  mutable PDGList m_pdgList;
  
  typedef std::map< int, double >     Average;
  mutable Average m_avP;
  mutable Average m_av2P;
  mutable Average m_avPt;
  mutable Average m_av2Pt;
  mutable Average m_avEta;
  mutable Average m_av2Eta;
  mutable Average m_avPhi;
  mutable Average m_av2Phi;
  mutable Average m_avM;
  mutable Average m_av2M;

  mutable std::vector<const xAOD::TruthParticle*> m_wzLeptons;
};
#endif 

