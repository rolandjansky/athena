/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/TrackRandomizer.h"

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
  TrackRandomizer::TrackRandomizer(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm( name, pSvcLocator )
  {
    
    declareProperties();
    
  }
  
  

  //__________________________________________________________________________
  TrackRandomizer::~TrackRandomizer()
  { 
    ATH_MSG_DEBUG("destructor called");
  }



  //__________________________________________________________________________
  StatusCode TrackRandomizer::beginRun()  { 

    ATH_MSG_DEBUG("beginRun()");

    return StatusCode::SUCCESS;
  }
  
  
  
  //__________________________________________________________________________
  StatusCode TrackRandomizer::initialize()
  {
    ATH_MSG_INFO("initialize: begin");
    return StatusCode::SUCCESS; 
  }



  //__________________________________________________________________________
  StatusCode TrackRandomizer::finalize()
  {
    
    ATH_MSG_INFO("finalize: TrackRandomizer finalize()");
    return StatusCode::SUCCESS; 
  }

  //__________________________________________________________________________
  StatusCode TrackRandomizer::initEvent()
  {

    ATH_MSG_DEBUG("initEvent: begin");
    
    ATH_MSG_DEBUG("initEvent: from initEvent ");
    return StatusCode::SUCCESS;
   
  }
  
  
  //__________________________________________________________________________
  StatusCode TrackRandomizer::execute()
  {
    //
    ATH_MSG_DEBUG("TrackRandomizer execute()");

    // clear ntuple variables
    StatusCode sc = this->initEvent();
    if(sc.isFailure()) {
      ATH_MSG_WARNING("Problem in initEvent ");
      return StatusCode::SUCCESS;
    }
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, "InDetTrackParticles") );
    
    auto *outputTrackContainer    = new xAOD::TrackParticleContainer;
    auto *outputTrackAuxContainer = new xAOD::TrackParticleAuxContainer;
    
    outputTrackContainer ->setStore( outputTrackAuxContainer );
    
    ATH_CHECK( evtStore()->record( outputTrackContainer,    Form("InDetTrackParticlesRandomized%s", m_jp.outputContainerName.c_str() )     ) );
    ATH_CHECK( evtStore()->record( outputTrackAuxContainer, Form("InDetTrackParticlesRandomized%sAux.", m_jp.outputContainerName.c_str() ) ) );
    
    for( auto *trk : *trackParticleContainer ) {
      auto *a_trk = new xAOD::TrackParticle;
      outputTrackContainer->emplace_back( a_trk );
      
      auto& covMatrix = trk->definingParametersCovMatrix();
      
      a_trk->setDefiningParameters( gRandom->Gaus( trk->d0(),     sqrt( covMatrix(0, 0) ) * m_jp.shuffleStrength ),
                                    gRandom->Gaus( trk->z0(),     sqrt( covMatrix(1, 1) ) * m_jp.shuffleStrength ),
                                    gRandom->Gaus( trk->phi(),    sqrt( covMatrix(2, 2) ) * m_jp.shuffleStrength ),
                                    gRandom->Gaus( trk->theta(),  sqrt( covMatrix(3, 3) ) * m_jp.shuffleStrength ),
                                    gRandom->Gaus( trk->qOverP(), sqrt( covMatrix(4, 4) ) * m_jp.shuffleStrength ) );
      
      a_trk->setDefiningParametersCovMatrix( covMatrix );
      a_trk->setFitQuality( trk->chiSquared(), trk->numberDoF() );
      a_trk->setParametersOrigin( trk->vx(), trk->vy(), trk->vz() );
      a_trk->setBeamlineTiltX( trk->beamlineTiltX() );
      a_trk->setBeamlineTiltY( trk->beamlineTiltY() );
      a_trk->setHitPattern( trk->hitPattern() );

      for (int ival = 0 ; ival < xAOD::numberOfTrackSummaryTypes ; ival++ ) {
        
        std::vector<int> excs { 13, 21, 22, 23, 24, 25, 26, 27, 45, 47, 48, 49, 50, 51, 65 };
        
        if( std::find( excs.begin(), excs.end(), ival ) != excs.end() ) continue;
        
        uint8_t val { 0 };
        
        if( !(trk->summaryValue( val, static_cast<xAOD::SummaryType>( ival ) ) ) ) val = 0;
        
        if ( val ) a_trk->setSummaryValue( val, static_cast<xAOD::SummaryType>( ival ) );
      }
      
    }
    
    ATH_MSG_DEBUG( "execute: process done." );
    // end
    return StatusCode::SUCCESS;   
    
  }
  
  //__________________________________________________________________________
  void TrackRandomizer::declareProperties() {
    
    declareProperty("shuffleStrength", m_jp.shuffleStrength = 1.0 );
    declareProperty("outputContainerName", m_jp.outputContainerName = "1p0" );
    
  }
    
} // end of namespace bracket
