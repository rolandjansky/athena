///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMCAnalysis.h 
// Header file for class HepMCAnalysis
// Author: W. Ehrenfeld, S.Johnert
/////////////////////////////////////////////////////////////////// 
#ifndef HEPMCANALYSIS_I_HEPMCANALYSIS_H
#define HEPMCANALYSIS_I_HEPMCANALYSIS_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TH1.h"

// forward declarations
class ITHistSvc;
class baseAnalysis;

class HepMCAnalysis
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  HepMCAnalysis( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~HepMCAnalysis(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  //virtual StatusCode  finish();

  //some storegate variables
  std::string m_key; 
  std::string m_infokey;
  

  bool m_JetFinder;
  bool m_JetAnalysis;
  bool m_WplusJetAnalysis;
  bool m_ZAnalysis;
  bool m_ZtautauAnalysis;
  bool m_WtaunuAnalysis;
  bool m_ttbarAnalysis;  
  bool m_bbbarAnalysis;
  bool m_UEAnalysis;
  bool m_EtmissAnalysis;
  bool m_ElasScatAnalysis;
  bool m_UserAnalysis;

  bool m_LeptonJetAnalysis;
  bool m_ParticleContentAnalysis;
  bool m_PdfAnalysis;

  //root output name
  std::string m_OutputFileName;

  //vector of analysis kinds
  std::vector<baseAnalysis*> m_analysis;
  std::vector<baseAnalysis*> m_jetfinder;

  //some analysis variables
  double m_max_eta;
  double m_min_pt;
  double m_jet_coneRadius;
  double m_jet_overlapThreshold;
  double m_jet_ptmin;
  double m_lepton_ptmin;
  double m_DeltaR_lepton_track;

  ServiceHandle<ITHistSvc> m_histSvc;
  std::vector<TH1D*> m_histVector;

  /// Default constructor: 
  HepMCAnalysis();

  /// Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !HEPMCANALYSIS_I_HEPMCANALYSIS_H
