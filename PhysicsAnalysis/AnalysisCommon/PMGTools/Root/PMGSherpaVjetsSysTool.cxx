/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PMGSherpaVjetsSysTool.cxx.cxx 2016-07-12 tripiana $

// ROOT include(s):
#include "TKey.h"
#include "TIterator.h"
#include "TH2F.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"

// Tool include(s):
#include "PathResolver/PathResolver.h"

// Local include(s):
#include "PMGTools/PMGSherpaVjetsSysTool.h"

namespace PMGTools {

  using namespace std;

  PMGSherpaVjetsSysTool::PMGSherpaVjetsSysTool( const std::string& name )
    : asg::AsgTool( name ),
      m_current_njets(-1),
      m_use_cache(false),
      m_cache_ptype(SysParType::None),
      m_cache_ptidx(-1),
      m_weightsFile(""),
      m_truthJetContainer(""),
      m_truthParticleContainer("")
  {

    // Declare the properties of the tool:
    declareProperty( "WeightsFile",
                     m_weightsFile =
                     "dev/SUSYTools/Vjets_SysParameterization/AllVariationWeights.root" );

    declareProperty( "TruthJetContainer",
		     m_truthJetContainer = "AntiKt4TruthWZJets" );
    declareProperty( "TruthParticleContainer",
		     m_truthParticleContainer = "TruthParticles" );
  }

  StatusCode PMGSherpaVjetsSysTool::initialize() {

    // Tell the user what's happening:
    ATH_MSG_INFO( "Initializing " << name() << "..." );
    ATH_MSG_DEBUG( "Will be using:" );
    ATH_MSG_DEBUG( "  WeightsFile            = " << m_weightsFile );
    ATH_MSG_DEBUG( "  TruthJetContainer      = " << m_truthJetContainer );
    ATH_MSG_DEBUG( "  TruthParticleContainer = " << m_truthParticleContainer );

    //load parametrization maps
    m_map_w     = {};
    m_map_zll   = {};
    m_map_znunu = {};
    m_map_gamma = {};

    //open the rootfiles
    const std::string fileName = PathResolverFindCalibFile( m_weightsFile );
    m_file.reset( TFile::Open( fileName.c_str(), "READ" ) );
    if( ( ! m_file ) || m_file->IsZombie() ) {
       ATH_MSG_ERROR( "Couldn't open file: " << m_weightsFile );
       return StatusCode::FAILURE;
    }

    TKey *key;
    std::string hname="";
    std::string keyname="";
    TIter next( m_file->GetListOfKeys());
    while ((key = (TKey *) next())) {

      keyname = std::string(key->GetName());

      //load Zee histograms
      if( keyname.find("Zee_") != std::string::npos ){
	keyname.erase(0,4);
	hname = "PMGSherpaVjetsSysTool::ZllJets_"+keyname;
       
	m_map_zll[keyname] = (TH2F*)m_file->Get(key->GetName())->Clone(hname.c_str()); 
	m_supported_zll.push_back(key->GetName());
      }

      //load Znunu histograms
      if( keyname.find("Znunu_") != std::string::npos ){
	keyname.erase(0,6);
	hname = "PMGSherpaVjetsSysTool::ZnunuJets_"+keyname;
       
	m_map_znunu[keyname] = (TH2F*)m_file->Get(key->GetName())->Clone(hname.c_str()); 
	m_supported_znunu.push_back(keyname);
      }

      //load Wlnu histograms
      if( keyname.find("Wenu_") != std::string::npos ){ 
	keyname.erase(0,5);
	hname = "PMGSherpaVjetsSysTool::WlnuJets_"+keyname;
       
	m_map_w[keyname] = (TH2F*)m_file->Get(key->GetName())->Clone(hname.c_str()); 
	m_supported_w.push_back(keyname);
      }

      //load gammaJets histograms
      if( keyname.find("Gamma_") != std::string::npos ){ 
	keyname.erase(0,6);
	hname = "PMGSherpaVjetsSysTool::GammaJets_"+keyname;
       
	m_map_gamma[keyname] = (TH2F*)m_file->Get(key->GetName())->Clone(hname.c_str()); 
	m_supported_gamma.push_back(keyname);
      }

    }

    // Return gracefully:
    return StatusCode::SUCCESS;
  }


  void PMGSherpaVjetsSysTool::cacheDSID(bool do_it){

    m_use_cache = do_it;
    if(do_it){
      m_cache_ptype = SysParType::None; 
      m_cache_ptidx = getPtMapping(m_cache_ptype); 
    }
  }


  double PMGSherpaVjetsSysTool::getWeight( const std::string& sysVariation ) {

    SysParType pType(SysParType::None); 
    int ptIdx(0);

    //look for DSID type and pt-bin unless previously fixed by user
    if(!m_use_cache){ 
      ptIdx = getPtMapping(pType); 
    }
    else{
      pType = m_cache_ptype;
      ptIdx = m_cache_ptidx;
    }

    //if not found in mapped samples, then return 1.
    if(pType == SysParType::None) return 1.;

    // get number of truth jets
    unsigned int njets = (m_current_njets >=0 ? m_current_njets : this->getNtruthJets());

    // re-define last bin for n_jets >= 11
    if(njets>11)
      njets = 11;


    // retrieve weight
    if(pType == SysParType::Zll)       return m_map_zll[sysVariation]->GetBinContent(ptIdx, njets+1);
    if(pType == SysParType::Znunu)     return m_map_znunu[sysVariation]->GetBinContent(ptIdx, njets+1);
    if(pType == SysParType::Wlnu)      return m_map_w[sysVariation]->GetBinContent(ptIdx, njets+1);
    if(pType == SysParType::GammaJets) return m_map_gamma[sysVariation]->GetBinContent(ptIdx, njets+1);
    //...
    
    return 1.;
  }


  int PMGSherpaVjetsSysTool::getPtMapping(SysParType& pType) {
    //get MCID --> PT-BIN mapping 

    const xAOD::EventInfo* ei = 0;
    if(! evtStore()->retrieve( ei, "EventInfo").isSuccess() ){
      ATH_MSG_ERROR("No EventInfo found in TStore(). Not able to get the sample ID. Dummy null weight retrieved.");
      return 0;
    }

    unsigned int MCID = ei->mcChannelNumber();

    //Check Znunu 2.1 samples 
    if( MCID>=361444 && MCID<361447 ){ pType=SysParType::Znunu;   return 1; }
    if( MCID>=361447 && MCID<361450 ){ pType=SysParType::Znunu;   return 2; }    
    if( MCID>=361450 && MCID<361453 ){ pType=SysParType::Znunu;   return 3; }    
    if( MCID>=361453 && MCID<361456 ){ pType=SysParType::Znunu;   return 4; }    
    if( MCID>=361456 && MCID<361459 ){ pType=SysParType::Znunu;   return 5; }    
    if( MCID>=361459 && MCID<361462 ){ pType=SysParType::Znunu;   return 6; }    
    if( MCID>=361462 && MCID<361465 ){ pType=SysParType::Znunu;   return 7; }    
    if( MCID>=361465 && MCID<361468 ){ pType=SysParType::Znunu;   return 8; }    

    //Check Zee 2.1 samples
    if( MCID>=361372 && MCID<361375 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=361375 && MCID<361378 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=361378 && MCID<361381 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=361381 && MCID<361384 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID>=361384 && MCID<361387 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID>=361387 && MCID<361390 ){ pType=SysParType::Zll;   return 6; }    
    if( MCID>=361390 && MCID<361393 ){ pType=SysParType::Zll;   return 7; }    
    if( MCID>=361393 && MCID<361396 ){ pType=SysParType::Zll;   return 8; }    

    //Check Zmumu 2.1 samples
    if( MCID>=361396 && MCID<361399 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=361399 && MCID<361402 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=361402 && MCID<361405 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=361405 && MCID<361408 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID>=361408 && MCID<361411 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID>=361411 && MCID<361414 ){ pType=SysParType::Zll;   return 6; }    
    if( MCID>=361414 && MCID<361417 ){ pType=SysParType::Zll;   return 7; }    
    if( MCID>=361417 && MCID<361420 ){ pType=SysParType::Zll;   return 8; }    

    //Check Ztautau 2.1 samples
    if( MCID>=361420 && MCID<361423 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=361423 && MCID<361426 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=361426 && MCID<361429 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=361429 && MCID<361432 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID>=361432 && MCID<361435 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID>=361435 && MCID<361438 ){ pType=SysParType::Zll;   return 6; }    
    if( MCID>=361438 && MCID<361441 ){ pType=SysParType::Zll;   return 7; }    
    if( MCID>=361441 && MCID<361444 ){ pType=SysParType::Zll;   return 8; }    

    //Check Wenu 2.1 samples
    if( MCID>=361300 && MCID<361303 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=361303 && MCID<361306 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=361306 && MCID<361309 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=361309 && MCID<361312 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID>=361312 && MCID<361315 ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID>=361315 && MCID<361318 ){ pType=SysParType::Wlnu;   return 6; }    
    if( MCID>=361318 && MCID<361321 ){ pType=SysParType::Wlnu;   return 7; }    
    if( MCID>=361321 && MCID<361324 ){ pType=SysParType::Wlnu;   return 8; }    

    //Check Wmunu 2.1 samples
    if( MCID>=361324 && MCID<361327 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=361327 && MCID<361330 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=361330 && MCID<361333 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=361333 && MCID<361336 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID>=361336 && MCID<361339 ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID>=361339 && MCID<361342 ){ pType=SysParType::Wlnu;   return 6; }    
    if( MCID>=361342 && MCID<361345 ){ pType=SysParType::Wlnu;   return 7; }    
    if( MCID>=361345 && MCID<361348 ){ pType=SysParType::Wlnu;   return 8; }    

    //Check Wtaunu 2.1 samples
    if( MCID>=361348 && MCID<361351 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=361351 && MCID<361354 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=361354 && MCID<361357 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=361357 && MCID<361360 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID>=361360 && MCID<361363 ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID>=361363 && MCID<361366 ){ pType=SysParType::Wlnu;   return 6; }    
    if( MCID>=361366 && MCID<361369 ){ pType=SysParType::Wlnu;   return 7; }    
    if( MCID>=361369 && MCID<361372 ){ pType=SysParType::Wlnu;   return 8; }    

    //Check Znunu 2.2 samples
    if( MCID>=363412 && MCID<363415 ){ pType=SysParType::Znunu;   return 1; }
    if( MCID>=363415 && MCID<363418 ){ pType=SysParType::Znunu;   return 2; }    
    if( MCID>=363418 && MCID<363421 ){ pType=SysParType::Znunu;   return 3; }    
    if( MCID>=363421 && MCID<363424 ){ pType=SysParType::Znunu;   return 4; }    
    if( MCID>=363424 && MCID<363427 ){ pType=SysParType::Znunu;   return 5; }    
    if( MCID>=363427 && MCID<363430 ){ pType=SysParType::Znunu;   return 6; }    
    if( MCID>=363430 && MCID<363433 ){ pType=SysParType::Znunu;   return 7; }    
    if( MCID>=363433 && MCID<363436 ){ pType=SysParType::Znunu;   return 8; }    

    //Check Zee 2.2 samples
    if( MCID>=363388 && MCID<363391 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=363391 && MCID<363394 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=363394 && MCID<363397 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=363397 && MCID<363400 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID>=363400 && MCID<363403 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID>=363403 && MCID<363406 ){ pType=SysParType::Zll;   return 6; }    
    if( MCID>=363406 && MCID<363409 ){ pType=SysParType::Zll;   return 7; }    
    if( MCID>=363409 && MCID<363412 ){ pType=SysParType::Zll;   return 8; }    

    //Check Zmumu 2.2 samples
    if( MCID>=363364 && MCID<363367 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=363367 && MCID<363370 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=363370 && MCID<363373 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=363373 && MCID<363376 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID>=363376 && MCID<363379 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID>=363379 && MCID<363382 ){ pType=SysParType::Zll;   return 6; }    
    if( MCID>=363382 && MCID<363385 ){ pType=SysParType::Zll;   return 7; }    
    if( MCID>=363385 && MCID<363388 ){ pType=SysParType::Zll;   return 8; }    

    //Check Ztautau 2.2 samples
    if( MCID>=363099 && MCID<363102 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=363102 && MCID<363105 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=363105 && MCID<363108 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=363108 && MCID<363111 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID>=363111 && MCID<363114 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID>=363114 && MCID<363117 ){ pType=SysParType::Zll;   return 6; }    
    if( MCID>=363117 && MCID<363120 ){ pType=SysParType::Zll;   return 7; }    
    if( MCID>=363120 && MCID<363123 ){ pType=SysParType::Zll;   return 8; }    

    //Check Wenu 2.2 samples
    if( MCID>=363460 && MCID<363463 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=363463 && MCID<363466 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=363466 && MCID<363469 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=363469 && MCID<363472 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID>=363472 && MCID<363475 ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID>=363475 && MCID<363478 ){ pType=SysParType::Wlnu;   return 6; }    
    if( MCID>=363478 && MCID<363481 ){ pType=SysParType::Wlnu;   return 7; }    
    if( MCID>=363481 && MCID<363484 ){ pType=SysParType::Wlnu;   return 8; }    

    //Check Wmunu 2.2 samples
    if( MCID>=363436 && MCID<363439 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=363439 && MCID<363442 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=363442 && MCID<363445 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=363445 && MCID<363448 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID>=363448 && MCID<363451 ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID>=363451 && MCID<363454 ){ pType=SysParType::Wlnu;   return 6; }    
    if( MCID>=363454 && MCID<363457 ){ pType=SysParType::Wlnu;   return 7; }    
    if( MCID>=363457 && MCID<363460 ){ pType=SysParType::Wlnu;   return 8; }    

    //Check Wtaunu 2.2 samples
    if( MCID>=363331 && MCID<363334 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=363334 && MCID<363337 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=363337 && MCID<363340 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=363340 && MCID<363343 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID>=363343 && MCID<363346 ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID>=363346 && MCID<363349 ){ pType=SysParType::Wlnu;   return 6; }    
    if( MCID>=363349 && MCID<363352 ){ pType=SysParType::Wlnu;   return 7; }    
    if( MCID>=363352 && MCID<363355 ){ pType=SysParType::Wlnu;   return 8; }    

    //Check gamma+Jets samples
    if( MCID>=361039 && MCID<361042 ){ pType=SysParType::GammaJets;   return 1; }
    if( MCID>=361042 && MCID<361045 ){ pType=SysParType::GammaJets;   return 2; }    
    if( MCID>=361045 && MCID<361048 ){ pType=SysParType::GammaJets;   return 3; }    
    if( MCID>=361048 && MCID<361051 ){ pType=SysParType::GammaJets;   return 4; }    
    if( MCID>=361051 && MCID<361054 ){ pType=SysParType::GammaJets;   return 5; }    
    if( MCID>=361054 && MCID<361057 ){ pType=SysParType::GammaJets;   return 6; }    
    if( MCID>=361057 && MCID<361061 ){ pType=SysParType::GammaJets;   return 7; }    


    //Check Znunu 2.2.1 samples
    if( MCID>=364142 && MCID<364145 ){ pType=SysParType::Znunu;   return 1; }
    if( MCID>=364145 && MCID<364148 ){ pType=SysParType::Znunu;   return 2; }    
    if( MCID>=364148 && MCID<364151 ){ pType=SysParType::Znunu;   return 3; }    
    if( MCID>=364151 && MCID<364154 ){ pType=SysParType::Znunu;   return 4; }    
    if( MCID==364154  ){ pType=SysParType::Znunu;   return 5; }    
    if( MCID==364155  ){ pType=SysParType::Znunu;   return 7; }    

    //Check Zee 2.2.1 samples
    if( MCID>=364114 && MCID<364117 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=364117 && MCID<364120 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=364120 && MCID<364123 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=364123 && MCID<364126 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID==364126 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID==364127 ){ pType=SysParType::Zll;   return 7; }    

    //Check Zmumu 2.2.1 samples
    if( MCID>=364100 && MCID<364103 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=364103 && MCID<364106 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=364106 && MCID<364109 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=364109 && MCID<364112 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID==364112 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID==364113 ){ pType=SysParType::Zll;   return 7; }    

    //Check Ztautau 2.2.1 samples
    if( MCID>=364128 && MCID<364131 ){ pType=SysParType::Zll;   return 1; }
    if( MCID>=364131 && MCID<364134 ){ pType=SysParType::Zll;   return 2; }    
    if( MCID>=364134 && MCID<364137 ){ pType=SysParType::Zll;   return 3; }    
    if( MCID>=364137 && MCID<364140 ){ pType=SysParType::Zll;   return 4; }    
    if( MCID==364140 ){ pType=SysParType::Zll;   return 5; }    
    if( MCID==364141 ){ pType=SysParType::Zll;   return 7; }    

    //Check Wenu 2.2.1 samples
    if( MCID>=364170 && MCID<364173 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=364173 && MCID<364176 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=364176 && MCID<364179 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=364179 && MCID<364182 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID==364182 ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID==364183 ){ pType=SysParType::Wlnu;   return 7; }    

    //Check Wmunu 2.2.1 samples
    if( MCID>=364156 && MCID<364159 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=364159 && MCID<364162 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=364162 && MCID<364165 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=364165 && MCID<364168 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID==364168  ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID==364169  ){ pType=SysParType::Wlnu;   return 7; }    

    //Check Wtaunu 2.2.1 samples
    if( MCID>=364184 && MCID<364187 ){ pType=SysParType::Wlnu;   return 1; }
    if( MCID>=364187 && MCID<364190 ){ pType=SysParType::Wlnu;   return 2; }    
    if( MCID>=364190 && MCID<364193 ){ pType=SysParType::Wlnu;   return 3; }    
    if( MCID>=364193 && MCID<364196 ){ pType=SysParType::Wlnu;   return 4; }    
    if( MCID==364196  ){ pType=SysParType::Wlnu;   return 5; }    
    if( MCID==364197  ){ pType=SysParType::Wlnu;   return 7; }    


    return -1;
  }


  unsigned int PMGSherpaVjetsSysTool::getNtruthJets(){
   //
   // Count number of truthjets compatible with evaluation of correction
   //
   // MT : ~same to what's done in PMGSherpa22VJetsWeightTool already. Might want to inherit/merge/move to PMGUtils-like common code?

   // Check whether to do overlap removal with truth leptons:
   const bool doRemoval = ( m_truthJetContainer == "AntiKt4TruthJets" );

   // Retrieve the truth jets
   const xAOD::JetContainer* truthJets( 0 );
   if( evtStore()->retrieve( truthJets, m_truthJetContainer ).isFailure() ) {
      ATH_MSG_ERROR( "No TruthJetContainer (\"" << m_truthJetContainer << "\") found! Dummy null weight retrieved." );
      return 0;
   }

   size_t nTruthJets = 0;  

   // Retrieve the truth particles to get the leptons
   const xAOD::TruthParticleContainer* truthParticles( 0 );
   if( doRemoval ) {
      if( evtStore()->retrieve( truthParticles,
                                m_truthParticleContainer ).isFailure() ) {
         ATH_MSG_ERROR( "Couldn't retrieve truth particle container: " << m_truthParticleContainer );
         return 0;
      }
   }

   // Loop over all truth jets passing basic cuts:
   for( const xAOD::Jet* truthJet : *truthJets ) {

      // Start with a simple kinematic cut:
     if( truthJet->pt() <= 20000. || std::abs( truthJet->eta() ) >= 2.8 ) { //MT :  until John updates the parametrization with a 4.5 upper cut...
       continue;
     }
     
     // Remove hadronic taus e.g. from Ztautau, Wtaunu decays
     static const SG::AuxElement::ConstAccessor< int >
       acc( "HadronConeExclTruthLabelID" );
     if( acc.isAvailable( *truthJet ) && ( acc( *truthJet ) == 15 ) ) {
       continue;
     }
     
     // If no overlap removal is to be made, we're done already:
     if( ! doRemoval ) {
       ++nTruthJets;
       continue;
     }

     // Loop over the truth particles, to veto some jets:
     bool vetoJet = false;
     for( const xAOD::TruthParticle* truthParticle : *truthParticles ) {
       
       // Only select final state electrons and muons:
       if( ( truthParticle->status() != 1 ) ||
	   ( ( std::abs( truthParticle->pdgId() ) != 11 ) &&
	     ( std::abs( truthParticle->pdgId() ) != 13 ) ) ) {
	 continue;
       }
       // ...which also pass some basic kinematic cuts:
       if( ( truthParticle->pt() <= 20000.0 ) ||
	   ( std::abs( truthParticle->eta() ) >= 2.8 ) ) {  //MT :  until John updates the parametrization with a 4.5 upper cut... 
	 continue;
       }

       // Veto the jet if it's close to this lepton:
       static const double MIN_DISTANCE = 0.2 * 0.2;
       const double dR2 =
	 xAOD::P4Helpers::deltaR2( truthJet, truthParticle, true );
       if( dR2 < MIN_DISTANCE ) {
	 vetoJet = true;
	 break;
       }
     }
     
     // If the jet was not vetoed:
     if( ! vetoJet ) {
       ++nTruthJets;
     }
   }
   
   // Return the calculated value:
   return nTruthJets;
  }
  
  
  void PMGSherpaVjetsSysTool::setNjets(int njets){ 
    m_current_njets = njets; 
  }  
  

  std::vector<std::string> PMGSherpaVjetsSysTool::getSupportedVariations(SysParType ptype){ 
    
    if(ptype == SysParType::Zll)       return m_supported_zll;
    if(ptype == SysParType::Znunu)     return m_supported_znunu;
    if(ptype == SysParType::Wlnu)      return m_supported_w;
    if(ptype == SysParType::GammaJets) return m_supported_gamma;
    
    std::vector<std::string> vdummy={};
    return vdummy;
  }
  
}
