// emacs: this is -*- c++ -*-
/** @file AnalysisConfig_Ntuple.h */

#ifndef TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H
#define TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
// #include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigInDetAnalysis/TIDAEvent.h"
#include "TrigInDetAnalysis/TIDAVertex.h"
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
    m_doElectrons_tightCB(false),
    m_doElectrons_mediumCB(false),
    m_doElectrons_looseCB(false),
    m_doElectrons_tightLH(false),
    m_doElectrons_mediumLH(false),
    m_doElectrons_looseLH(false),
    m_doTaus_1Prong(false),
    m_doTaus_tight_1Prong(false),
    m_doTaus_medium_1Prong(false),
    m_doTaus_loose_1Prong(false),
    m_doTaus_3Prong(false),
    m_doTaus_tight_3Prong(false),
    m_doTaus_medium_3Prong(false),
    m_doTaus_loose_3Prong(false),
    m_doBjets(false),
    m_hasTruthMap(false),
    m_tauEtCutOffline(tauEtCutOffline),
    m_TruthPdgId(TruthPdgId),
    m_finalised(true),
    m_printInfo(true)
  {  
    //    std::cout << "AnalysisConfig_Ntuple::AnalysisConfig_Ntuple() " << chainNames.size() << std::endl;

    this->keepAllEvents( _keepAllEvents ); /// this is now i nthe base class

    for ( unsigned i=0 ; i<chainNames.size() ; i++ ) { 
      if ( chainNames[i] != "Offline"     &&
	   chainNames[i] != "Electrons"   &&
	   chainNames[i] != "Muons"       &&
	   chainNames[i] != "Taus_1Prong" &&
	   chainNames[i] != "Taus_3Prong" &&
	   chainNames[i] != "Bjets" )   { 
	
	//	std::cout << "AnalysisConfig_Ntuple: chain[" << i << "] " << chainNames[i] << std::endl;
	
	m_chainNames.push_back( ChainString(chainNames[i]) );

	
      }

      if ( chainNames[i]=="Offline" )     m_doOffline     = true;
      if ( chainNames[i]=="Vertex" )      m_doVertices    = true;
      if ( chainNames[i]=="Electrons" )   m_doElectrons   = true;
      if ( chainNames[i]=="Electrons_TightCB" )  m_doElectrons_tightCB  = true;
      if ( chainNames[i]=="Electrons_MediumCB" ) m_doElectrons_mediumCB = true;
      if ( chainNames[i]=="Electrons_LooseCB" )  m_doElectrons_looseCB  = true;
      if ( chainNames[i]=="Electrons_TightLH" )  m_doElectrons_tightLH  = true;
      if ( chainNames[i]=="Electrons_MediumLH" ) m_doElectrons_mediumLH = true;
      if ( chainNames[i]=="Electrons_LooseLH" )  m_doElectrons_looseLH  = true;
      if ( chainNames[i]=="Muons" )       m_doMuons       = true;
      if ( chainNames[i]=="Taus_1Prong" ) m_doTaus_1Prong = true;
      if ( chainNames[i]=="Taus_Tight_1Prong" )  m_doTaus_tight_1Prong  = true;
      if ( chainNames[i]=="Taus_Medium_1Prong" ) m_doTaus_medium_1Prong = true;
      if ( chainNames[i]=="Taus_Loose_1Prong" )  m_doTaus_loose_1Prong  = true;
      if ( chainNames[i]=="Taus_3Prong" ) m_doTaus_3Prong = true;
      if ( chainNames[i]=="Taus_Tight_3Prong" )  m_doTaus_tight_3Prong  = true;
      if ( chainNames[i]=="Taus_Medium_3Prong" ) m_doTaus_medium_3Prong = true;
      if ( chainNames[i]=="Taus_Loose_3Prong" )  m_doTaus_loose_3Prong  = true;
      if ( chainNames[i]=="Bjets" )       m_doBjets       = true;
    }
    m_event = new TIDA::Event();
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
  

  TIDA::Event*  m_event;

  TFile*      mFile;  
  TTree*      mTree;
  TDirectory* mDir;

  std::vector<ChainString> m_chainNames;

  bool m_doOffline;
  bool m_doVertices;
  bool m_doMuons;
  bool m_doElectrons;
  bool m_doElectrons_tightCB;
  bool m_doElectrons_mediumCB;
  bool m_doElectrons_looseCB;
  bool m_doElectrons_tightLH;
  bool m_doElectrons_mediumLH;
  bool m_doElectrons_looseLH;
  bool m_doTaus_1Prong;
  bool m_doTaus_tight_1Prong;
  bool m_doTaus_medium_1Prong;
  bool m_doTaus_loose_1Prong;
  bool m_doTaus_3Prong;
  bool m_doTaus_tight_3Prong;
  bool m_doTaus_medium_3Prong;
  bool m_doTaus_loose_3Prong;
  bool m_doBjets;
  bool m_hasTruthMap;
  bool m_tauEtCutOffline;

  std::string m_outputFileName;

  int m_TruthPdgId;
  
  bool m_finalised;

  //  bool m_keepAllEvents; // nw in base class

  bool m_printInfo;

};


  
#endif  // TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H

