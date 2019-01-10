/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PixeldEdxTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Hide Oide (Hideyuki.Oide@cern.ch) based on TrackParticleThinning class

#include "DerivationFrameworkInDet/PixeldEdxTrackParticleThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::PixeldEdxTrackParticleThinning::PixeldEdxTrackParticleThinning(const std::string& t,
                                                                                    const std::string& n,
                                                                                    const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_parser(0),
m_selectionString(""),
m_ntot(0),
m_npass(0),
m_inDetSGKey("InDetTrackParticles"),
m_unprescalePtCut( 10.e3 ),
m_globalScale ( 1.0 ),
m_d0SignifCut ( 5.0 ),
m_z0Cut( 3.0 ),
m_etaCut( 2.0 ),
m_and(false),
counter( 50, 0 ),
counter_picked( 50, 0 )
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    //thinning service
    declareProperty("ThinningService", m_thinningSvc);
    // logic and selection settings
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd", m_and);
    //keys for xAOD::TrackParticle container
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey);
}

// Destructor
DerivationFramework::PixeldEdxTrackParticleThinning::~PixeldEdxTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::PixeldEdxTrackParticleThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    
    //check xAOD::InDetTrackParticle collection
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    //check availability of xAOD::TrackStateValidation and xAOD::TrackMeasurementValidation containers

    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::PixeldEdxTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::PixeldEdxTrackParticleThinning::doThinning() const
{
    
    // Retrieve main TrackParticle collection
  const xAOD::TrackParticleContainer* importedTrackParticles { nullptr };
  ATH_CHECK( evtStore()->retrieve(importedTrackParticles,m_inDetSGKey) );
  
  // Check the event contains tracks
  size_t nTracks = importedTrackParticles->size();
  if (nTracks==0) return StatusCode::SUCCESS;
  
  // Retrieve vertex collection
  const xAOD::VertexContainer* primaryVertices { nullptr };
  ATH_CHECK( evtStore()->retrieve( primaryVertices, "PrimaryVertices" ) );
  
  const xAOD::Vertex* priVtx { nullptr };
  
  for( const auto* vtx : *primaryVertices ) {
    if( vtx->vertexType() == xAOD::VxType::PriVtx ) {
      priVtx = vtx;
      break;
    }
  }
  
  if( !priVtx ) return StatusCode::SUCCESS;
  
  
  // Set up a mask with the same entries as the full TrackParticle collection
  std::vector<bool> mask( nTracks, false );
  
  //////////////////////////////////////////////////////////////////////////////////////////
  static std::vector<double> pTbins;
  
  if( 0 == pTbins.size() ) {
    for( size_t ip = 0; ip <= 50; ++ip ) {
      pTbins.emplace_back( pow( 10, 2.0 + 0.04 * ip ) );
    }
  }
  
  auto getBin = [&]( const double& pT ) -> size_t {
    for( size_t ip = 0; ip < pTbins.size()-1; ip++ ) {
      if( pT > pTbins.at(ip) && pT < pTbins.at(ip+1) ) {
        return ip;
      }
    }
    return 0;
  };
  
  static std::vector<double> preScales {
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 124.261, 111.448, 97.7357, 85.5616, 72.3294,
      61.0017, 50.8592, 41.6827, 33.7413, 26.9884, 21.3822, 16.8352, 13.1081, 10.2368, 7.96723,
      6.23636, 4.91757, 3.91519, 3.15519, 2.57768, 2.13622, 1.80524, 1.53102, 1.30677, 1.13582
      };
  //////////////////////////////////////////////////////////////////////////////////////////
  
  const double m_globalScale { 1.0 };
  
  // Count the mask
  for (size_t i=0; i<nTracks; ++i) {
    
    const auto* trk = importedTrackParticles->at(i);
    
    // Keep all tracks if IBL is overflow
    if( trk->numberOfIBLOverflowsdEdx() > 0 ) {
      
      mask.at(i) = true; ++m_npass; continue;
      
    }
    
    
    // Keep all track for pT above 10 GeV
    if( std::abs( trk->pt() ) > m_unprescalePtCut ) {
      
      mask.at(i) = true; ++m_npass; continue;
      
    }      
    
    m_ntot++;
    
    // Hereafter only low-pT and no-overflow tracks
    
    // Keep only d0 significancd is small enough
    if( std::abs( xAOD::TrackingHelpers::d0significance( trk ) ) > m_d0SignifCut ) {
      continue;
    }
    
    // Keep only delta(z0) is less than 10 mm
    if( std::abs( trk->z0() - priVtx->z() ) > m_z0Cut ) {
      continue;
    }
    
    // Reject forward low-pT tracks
    if( std::abs( trk->eta() ) > m_etaCut ) {
      continue;
    }
    
    // Prescaled track keeping
    auto bin = getBin( std::abs( trk->pt() ) );
    
    const auto preScale   = static_cast<unsigned long long>( std::floor( preScales.at( bin ) * m_globalScale ) );
    const auto preScale10 = std::max( static_cast<unsigned long long>( std::floor( preScales.at( bin ) * m_globalScale / 10. ) ), 1ull );
    
    // Increment the counter
    counter.at(bin)++;
    
    float dEdx { 0 };
    trk->summaryValue( dEdx, xAOD::pixeldEdx );
    
    
    // Relatively higher dE/dx tracks
    static const float dEdxThr { pow( 10, 0.1 ) };
    if( dEdx > dEdxThr ) {
      
      mask.at(i) = ( (counter.at(bin) % preScale10) == 0 );
      
      // There are also tracks with dE/dx == -1.
    } else if( dEdx > 0. ) {
      
      mask.at(i) = ( (counter.at(bin) % preScale) == 0 );
      
    } else {
      
      mask.at(i) = false;
      
    }
    
    if( mask.back() ) { ++m_npass; counter_picked.at(bin)++; }
    
  }
  
  assert( importedTrackParticles->size() == mask.size() );
  
  // Apply the filter
  IThinningSvc::Operator::Type thinningOperator =
    ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
  
  ATH_CHECK( m_thinningSvc->filter(*importedTrackParticles, mask, thinningOperator) );
  
  return StatusCode::SUCCESS;
}

