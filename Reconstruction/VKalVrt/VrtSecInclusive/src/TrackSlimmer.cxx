/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/TrackSlimmer.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"

// ROOT Classes
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
#include "TRandom2.h"

#include <string>
#include <new>
#include <iostream>
#include <tuple>
#include <functional>
#include <chrono>

using namespace std;

namespace VKalVrtAthena {
  
  //Constructor and destructor
  //__________________________________________________________________________
  TrackSlimmer::TrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm( name, pSvcLocator )
    , m_thinningSvc( "ThinningSvc",  name )
  {
    
    declareProperties();
    
  }
  
  

  //__________________________________________________________________________
  TrackSlimmer::~TrackSlimmer()
  { 
    ATH_MSG_DEBUG("destructor called");
  }



  //__________________________________________________________________________
  StatusCode TrackSlimmer::beginRun()  { 

    ATH_MSG_DEBUG("beginRun()");

    return StatusCode::SUCCESS;
  }
  
  
  
  //__________________________________________________________________________
  StatusCode TrackSlimmer::initialize()
  {
    ATH_MSG_INFO("initialize: begin");
    return StatusCode::SUCCESS; 
  }



  //__________________________________________________________________________
  StatusCode TrackSlimmer::finalize()
  {
    
    ATH_MSG_INFO("finalize: TrackSlimmer finalize()");
    return StatusCode::SUCCESS; 
  }

  //__________________________________________________________________________
  StatusCode TrackSlimmer::initEvent()
  {

    ATH_MSG_DEBUG("initEvent: begin");
    
    ATH_MSG_DEBUG("initEvent: from initEvent ");
    return StatusCode::SUCCESS;
   
  }
  
  
  //__________________________________________________________________________
  StatusCode TrackSlimmer::execute()
  {
    //
    ATH_MSG_DEBUG("TrackSlimmer execute()");

    // clear ntuple variables
    StatusCode sc = this->initEvent();
    if(sc.isFailure()) {
      ATH_MSG_WARNING("Problem in initEvent ");
      return StatusCode::SUCCESS;
    }
    
    const xAOD::TrackParticleContainer* tpc ( nullptr );
    ATH_CHECK( evtStore()->retrieve( tpc, m_jp.containerName.c_str() ) );
    
    std::vector<bool> selected( tpc->size(), false );
    
    unsigned count { 0 };
    for( const auto* trk : *tpc ) {
      bool flag = ( trk->isAvailable<char>("is_svtrk_final") ? ( trk->auxdataConst<char>("is_svtrk_final") == true ) : false );
      if( flag ) count++;
      selected.emplace_back( flag );
    }
    
    ATH_MSG_INFO( "execut: filtered = " << count << " / " << tpc->size()  );
    
    ATH_CHECK( m_thinningSvc->filter( *tpc, selected, IThinningSvc::Operator::Or ) );
    
    ATH_MSG_DEBUG( "execute: process done." );
    // end
    return StatusCode::SUCCESS;   
    
  }
  
  //__________________________________________________________________________
  void TrackSlimmer::declareProperties() {
    
    declareProperty("containerName", m_jp.containerName = "1p0" );
    declareProperty( "thinningSvc", m_thinningSvc, "Name of the thinningSvc" );
    
  }
    
} // end of namespace bracket
