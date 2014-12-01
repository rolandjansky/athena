// emacs: this is -*- c++ -*-
/** @file AnalysisConfig_Ntuple.h */

#ifndef TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H
#define TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
// #include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigInDetAnalysis/TrackEvent.h"
#include "TrigInDetAnalysis/TrackVertex.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"

#include "TrigInDetAnalysisExample/ChainString.h"

#include "TrkParticleCreator/TrackParticleCreatorTool.h"


#include "TTree.h"
#include "TFile.h"

class AnalysisConfig_Ntuple : public T_AnalysisConfig<IHLTMonTool> { 
    
public:
    
    // Full constructor: test/reference/selection
    // - analysisInstanceName: the name of the analysis chain being created
    // - xxxChainName: the name of the chain to be used as test/reference/selection; must be "StoreGate" in case of direct access to SG containers
    // - xxxType: the type of tracks to be retrieved from the test/reference/selection chain or container
    // - xxxKey:  the key for tracks to be retrieved from the test/reference/selection chain or container
    // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths; in case the test chain is a fake chain, this is used 
    //   for RoI position too
    // - all standard operations are performed in loops over 0=test 1=reference 2=selection
  AnalysisConfig_Ntuple(TIDARoiDescriptor* roiInfo, 
			const std::vector<std::string>& chainNames, std::string outputFileName="TrkNtuple.root", 
			double tauEtCutOffline=0.0, int TruthPdgId = 0, bool _keepAllEvents=false) : 
    T_AnalysisConfig<IHLTMonTool>( "Ntple",
				   "", "", "",
				   "", "", "",
				   "", "", "",
				   roiInfo,
				   0, 0, 0,
				   0,
				   0 ),
    m_event(0),
    mFile(0),
    mTree(0),
    m_doOffline(false),
    m_doVertices(false),
    m_doMuons(false),
    m_doElectrons(false),
    m_doTaus(false),
    m_doBjets(false),
    m_hasTruthMap(false),
    m_doTauThreeProng(false),
    m_tauEtCutOffline(tauEtCutOffline),
    m_TruthPdgId(TruthPdgId),
    m_finalised(true),
    m_printInfo(true)
  {  
    this->keepAllEvents( _keepAllEvents ); /// this is now i nthe base class

    for ( unsigned i=0 ; i<chainNames.size() ; i++ ) { 
      if ( chainNames[i] != "Offline" &&
	   chainNames[i] != "Electrons" &&
	   chainNames[i] != "Muons" &&
	   chainNames[i] != "Taus" &&
	   chainNames[i] != "Taus3" 
	   )  m_chainNames.push_back(chainNames[i]);
      if ( chainNames[i]=="Offline" )  m_doOffline = true;
      if ( chainNames[i]=="Vertex" )   m_doVertices = true;
      if ( chainNames[i]=="Electrons" )m_doElectrons   = true;
      if ( chainNames[i]=="Muons" )    m_doMuons   = true;
      if ( chainNames[i]=="Taus" )m_doTaus   = true;
      if ( chainNames[i]=="Taus3" )m_doTauThreeProng   = true;
      if ( chainNames[i]=="Bjets" )m_doBjets = true;
    }
    m_event = new TrackEvent();
    m_outputFileName=outputFileName;
  }
  

  virtual ~AnalysisConfig_Ntuple() {
    //    std::cout << "AnalysisConfig_Ntuple::~AnalysisConfig_Ntuple() running destructor" << std::endl;
    if ( !m_finalised ) finalize();
    delete m_event;
  }


protected:

  virtual void loop();

  virtual void book();

  virtual void finalize();

  IBeamCondSvc*  m_iBeamCondSvc;
  IBeamCondSvc*  m_iOnlineBeamCondSvc;
  

  TrackEvent*  m_event;

  TFile*      mFile;  
  TTree*      mTree;
  TDirectory* mDir;

  std::vector<ChainString> m_chainNames;

  bool m_doOffline;
  bool m_doVertices;
  bool m_doMuons;
  bool m_doElectrons;
  bool m_doTaus;
  bool m_doBjets;
  bool m_hasTruthMap;
  bool m_doTauThreeProng;
  bool m_tauEtCutOffline;

  std::string m_outputFileName;

  int m_TruthPdgId;
  
  bool m_finalised;

  //  bool m_keepAllEvents; // nw in base class

  bool m_printInfo;

};


  
#endif  // TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H

