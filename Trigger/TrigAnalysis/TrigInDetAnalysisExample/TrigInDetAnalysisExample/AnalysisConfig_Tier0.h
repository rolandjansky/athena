// emacs: this is -*- c++ -*-
/** @file AnalysisConfig_Ntuple.h */

#ifndef TrigInDetAnalysisExample_AnalysisConfig_Tier0_H
#define TrigInDetAnalysisExample_AnalysisConfig_Tier0_H



#include "TrigHLTMonitoring/IHLTMonTool.h"
// #include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigInDetAnalysis/TrackEvent.h"
#include "TrigInDetAnalysis/TrackVertex.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"

#include "TrigInDetAnalysisExample/Analysis_Tier0.h"
#include "TrigInDetAnalysisExample/ChainString.h"

#include "TTree.h"
#include "TFile.h"

class AnalysisConfig_Tier0 : public T_AnalysisConfig<IHLTMonTool> { 
    
public:
    
    // Full constructor: test/reference/selection
    // - analysisInstanceName: the name of the analysis chain being created
    // - xxxChainName: the name of the chain to be used as test/reference/selection; must be "StoreGate" in case of direct access to SG containers
    // - xxxType: the type of tracks to be retrieved from the test/reference/selection chain or container
    // - xxxKey:  the key for tracks to be retrieved from the test/reference/selection chain or container
    // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths; in case the test chain is a fake chain, this is used for RoI position too
    // - all standard operations are performed in loops over 0=test 1=reference 2=selection
  AnalysisConfig_Tier0(const std::string& analysisInstanceName,
		       const std::string& testChainName,      const std::string& testType,      const std::string& testKey,
		       const std::string& referenceChainName, const std::string& referenceType, const std::string& referenceKey,
		       TIDARoiDescriptor* roiInfo,
		       TrackFilter* testFilter, TrackFilter* referenceFilter,
		       TrackAssociator* associator,
		       TrackAnalysis* analysis) :
    T_AnalysisConfig<IHLTMonTool>( analysisInstanceName,
				   testChainName,      testType,      testKey,
				   referenceChainName, referenceType, referenceKey,
				   roiInfo,
				   testFilter, referenceFilter, 
				   associator,
				   analysis ),
    m_doOffline(true),
    m_doMuons(false),
    m_doElectrons(false),
    m_doTaus(false),
    m_doBjets(false),
    m_hasTruthMap(false)
    
    
    
  {  
    m_event = new TrackEvent();
    m_chainNames.push_back(testChainName);
    m_testType = testType;
  }


  virtual ~AnalysisConfig_Tier0() {
    delete m_event;
  }

Analysis_Tier0* m_analysisT0;  

protected:

  virtual void loop();

  virtual void book();

  virtual void finalize();

  IBeamCondSvc*  m_iBeamCondSvc;
  IBeamCondSvc*  m_iOnlineBeamCondSvc;

  TrackEvent*  m_event;

  TFile*    mFile;  
  TTree*    mTree;

  std::vector<ChainString> m_chainNames;
  std::vector<Analysis_Tier0*> m_analyses;
  std::string m_testType;
  bool m_doOffline;
  bool m_doMuons;
  bool m_doElectrons;
  bool m_doTaus;
  bool m_doBjets;
  bool m_hasTruthMap;
  bool m_doTauThreeProng;
  bool m_tauEtCutOffline;
  std::string m_outputFileName;
  
};


  
#endif  // TrigInDetAnalysisExample_AnalysisConfig_Tier0_H

