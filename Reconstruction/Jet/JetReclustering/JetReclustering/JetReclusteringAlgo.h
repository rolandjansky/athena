/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE
#ifndef JetReclustering_JetReclusteringAlgo_H
#define JetReclustering_JetReclusteringAlgo_H

#include <EventLoop/Algorithm.h>

#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>

// reclustering
#include <AsgTools/AnaToolHandle.h>
#include <JetInterface/IJetExecuteTool.h>

class JetReclusteringAlgo : public EL::Algorithm
{
public:
  std::string m_name = "GiordonStark",
              m_inputJetContainer,
              m_outputJetContainer,
              m_rc_alg,
              m_outputXAODName;
  float m_radius = 1.0;
  float m_ptMin_input = 25.0; // GeV
  float m_ptMin_rc = 50.0; // GeV
  std::string m_groomAlg = "trim";
  float m_trim_ptFrac = 0.05;
  float m_trim_subjet_radius = 0.2;
  float m_sd_zcut = 0.1;
  float m_sd_beta = 0.0;
  float m_sd_R0 = 1.0;
  float m_varR_minR = -1.0;
  float m_varR_mass = -1.0; // GeV
  bool m_doArea = false;
  std::string m_areaAttributes = "ActiveArea ActiveArea4vec";
  
  // options for substructure tools
  std::vector<float> m_ecf_betaVals = {};
  bool m_ecf_doC3 = false;
  bool m_ecf_doDichroic = false;
  bool m_ecfg_doN3 = false;
  bool m_ecfg_doLSeries = false;
  
  // tracks
  std::string m_ghostTracksInputContainer = "";
  std::string m_ghostTracksVertexAssociationName = "";
  // b-tagging jets
  std::string m_ghostBTagJetInputContainer = "";
  std::string m_ghostBTagJetLabel = "";
  // truth B quarks
  std::string m_ghostTruthBQuarksInputContainer = "";
  std::string m_ghostTruthBQuarksLabel = "";
  // truth B hadrons
  std::string m_ghostTruthBHadronsInputContainer = "";
  std::string m_ghostTruthBHadronsLabel = "";
  // truth C quarks
  std::string m_ghostTruthCQuarksInputContainer = "";
  std::string m_ghostTruthCQuarksLabel = "";
  // truth C hadrons
  std::string m_ghostTruthCHadronsInputContainer = "";
  std::string m_ghostTruthCHadronsLabel = "";
  // truth T quarks
  std::string m_ghostTruthTQuarksInputContainer = "";
  std::string m_ghostTruthTQuarksLabel = "";
  // truth W bosons
  std::string m_ghostTruthWBosonsInputContainer = "";
  std::string m_ghostTruthWBosonsLabel = "";
  // truth Z bosons
  std::string m_ghostTruthZBosonsInputContainer = "";
  std::string m_ghostTruthZBosonsLabel = "";
  // truth H bosons
  std::string m_ghostTruthHBosonsInputContainer = "";
  std::string m_ghostTruthHBosonsLabel = "";

  float m_ghostScale = 1e-20;
  bool m_debug = false;

private:
  /* For counting and statistics */
  xAOD::TEvent *m_event; //!
  xAOD::TStore *m_store; //!

  asg::AnaToolHandle<IJetExecuteTool> m_jetReclusteringTool{"JetReclusteringTool", this}; //!

public:
  // this is a standard constructor
  JetReclusteringAlgo ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(JetReclusteringAlgo, 1);
};

#endif
#endif // ROOTCORE
