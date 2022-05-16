/* emacs: this is -*- c++ -*- */
/**
 **     @file    AnalysisConfig_Ntuple.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:37 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H
#define TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H

#include "TrigInDetAnalysis/TIDAEvent.h"
#include "TrigInDetAnalysis/TIDAVertex.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"

#include "TrigInDetAnalysisExample/ChainString.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

// #include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "TTree.h"
#include "TFile.h"

#define endmsg endmsg

class AnalysisConfig_Ntuple : public T_AnalysisConfig<AthReentrantAlgorithm> {
    
public:
    
    // Full constructor: test/reference/selection
    // - analysisInstanceName: the name of the analysis chain being created
    // - xxxChainName: the name of the chain to be used as test/reference/selection; must be "StoreGate" in case of direct access to SG containers
    // - xxxType: the type of tracks to be retrieved from the test/reference/selection chain or container
    // - xxxKey:  the key for tracks to be retrieved from the test/reference/selection chain or container
    // - all standard operations are performed in loops over 0=test 1=reference 2=selection

  AnalysisConfig_Ntuple(const std::vector<std::string>& chainNames, std::string outputFileName="TrkNtuple.root", 
			  double tauEtCutOffline=0.0, int TruthPdgId = 0, bool _keepAllEvents=false, int parentTruthPdgId = 0) : 
    T_AnalysisConfig<AthReentrantAlgorithm>( "Ntple",
				   "", "", "",
				   "", "", "",
				   "", "", "",
				   0, 0, 0,
				   0,
				   0 ),
    m_event(0),
    mFile(0),
    mTree(0),
    m_doOffline(false),
    m_doVertices(false),
    m_doMuons(false),
    m_doMuonsSP(false),
    m_muonType(),
    m_electronType(),
    m_rawElectrons(),
    m_tauType(),
    m_tauProngs(),
    m_vertexType(),
    m_doBjets(false),
    m_hasTruthMap(false),
    m_tauEtCutOffline(tauEtCutOffline),
    m_TruthPdgId(TruthPdgId),
    m_finalised(true),
    m_printInfo(true),
    m_ptmin(0),
    m_parentTruthPdgId(parentTruthPdgId),
    m_tida_first(true)
  {  
    /// leave in this debug printout ...
    /// std::cout << "AnalysisConfig_Ntuple::AnalysisConfig_Ntuple() " << chainNames.size() << std::endl;

    this->keepAllEvents( _keepAllEvents ); /// this is now i nthe base class

    for ( unsigned i=0 ; i<chainNames.size() ; i++ ) {

      if ( chainNames[i].empty() ) continue;

      ChainString chain(chainNames[i]);
      
      std::string cs = chain.head();
 
      if      ( cs=="Offline" )     m_doOffline     = true;
      else if ( cs=="Truth"   )     m_mcTruth       = true;
      else if ( cs=="Vertex" ) {
        m_vertexType.push_back(chain.tail());
        m_doVertices    = true;
      }    
      else if ( cs=="MuonsSP" )     m_doMuonsSP     = true;
      else if ( cs=="Muons" )       m_muonType.push_back(chain.tail());
      else if ( cs=="Electrons" ) { 
        m_electronType.push_back(chain.tail());
        m_rawElectrons.push_back(chain.roi()); /// abuse the ChainString, now use the roi as the default third field
      }
      else if ( cs=="Taus" ) { 
        m_tauType.push_back(chain.tail());
        m_tauProngs.push_back(chain.roi()); /// abuse the ChainString, now use the roi as the default third field
      }  
      else if ( cs=="Bjets" )       m_doBjets       = true;
      else { 

	if ( cs!="" && cs.find("HLT")==std::string::npos ) { 
	  /// provider is not assigned yet ...
	  //	  if ( m_provider ) m_provider->msg(MSG::WARNING) << "Unknown chain type: " << cs << endmsg; 
	  //	  else              std::cout << "Unknown chain type: " << cs << std::endl;
	  continue;
      	}

	m_chainNames.push_back( chain );
	
      }
    }

    m_event = new TIDA::Event();
    m_outputFileName=outputFileName;

  }
  
  

  virtual ~AnalysisConfig_Ntuple() {
    /// leave in this debug printout ...
    ///    std::cout << "AnalysisConfig_Ntuple::~AnalysisConfig_Ntuple() running destructor" << std::endl;
    if ( !m_finalised ) finalize();
    delete m_event;
  }

  void set_ptmin( double d=0 ) { m_ptmin=d; }

  size_t get_jets( Trig::FeatureContainer::combination_const_iterator citr, 
		   std::vector<TrackTrigObject>& objects, const std::string& key="" );

protected:

  virtual void loop();

  virtual void book();

  virtual void finalize();
  
  bool           m_useBeamCondSvc;

  TIDA::Event*  m_event;

  TFile*      mFile;  
  TTree*      mTree;
  TDirectory* mDir;

  std::vector<ChainString> m_chainNames;

  bool m_doOffline;
  bool m_doVertices;
  bool m_doMuons;
  bool m_doMuonsSP;

  std::vector<std::string>  m_muonType;

  std::vector<std::string>  m_electronType;
  std::vector<std::string>  m_rawElectrons;

  std::vector<std::string>  m_tauType;
  std::vector<std::string>  m_tauProngs;

  std::vector<std::string> m_vertexType;

  bool m_doBjets;
  bool m_hasTruthMap;
  bool m_tauEtCutOffline;

  std::string m_outputFileName;

  int m_TruthPdgId;
  
  bool m_finalised;

  //  bool m_keepAllEvents; // nw in base class

  bool m_printInfo;

  double m_ptmin;

  int m_parentTruthPdgId;

  bool m_tida_first;
  
};


  
#endif  // TrigInDetAnalysisExample_AnalysisConfig_Ntuple_H

