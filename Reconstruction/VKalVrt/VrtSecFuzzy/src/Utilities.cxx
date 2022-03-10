/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecFuzzy/VrtSecFuzzy.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <xAODEventInfo/EventInfo.h>
#include <xAODTruth/TruthParticleContainer.h>
#include <xAODTruth/TruthVertexContainer.h>

#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"

#include <iostream>
#include <map>
#include <vector>
#include <deque>

//-------------------------------------------------

using namespace std;

namespace Trk {
  extern void dsinv(long int * , double *, long int , long int *);             
}

namespace VKalVrtAthena {
  
  //____________________________________________________________________________________________________
  bool isAssociatedToVertices( const xAOD::TrackParticle *trk, const xAOD::VertexContainer* vertices ) {
    
      bool is_pv_associated = false;
      
      for( auto* vtx : *vertices ) {
        for( size_t iv = 0; iv < vtx->nTrackParticles(); iv++ ) {
          auto* pvtrk = vtx->trackParticle( iv );
          if( trk == pvtrk ) {
            is_pv_associated = true;
            break;
          }
        }
      }
      return is_pv_associated;
  }
  
  //____________________________________________________________________________________________________
  void VrtSecFuzzy::declareProperties() {
    
    declareProperty("GeoModel",                        m_jp.geoModel                        = VKalVrtAthena::GeoModel::Run2 );
    
    declareProperty("TrackLocation",                   m_jp.TrackLocation                   = "InDetTrackParticles"         );
    declareProperty("PrimVrtLocation",                 m_jp.PrimVrtLocation                 = "PrimaryVertices"             );
    declareProperty("McParticleContainer",             m_jp.truthParticleContainerName      = "TruthParticles"              );
    declareProperty("MCEventContainer",                m_jp.mcEventContainerName            = "TruthEvents"                 );
    declareProperty("All2trkVerticesContainerName",    m_jp.all2trksVerticesContainerName   = "All2TrksVertices"            );
    declareProperty("SecondaryVerticesContainerName",  m_jp.secondaryVerticesContainerName  = "SecondaryVertices"           );
    declareProperty("twoTrkVerticesInDVContainerName", m_jp.twoTrksVerticesInDVContainerName = "twoTrksVerticesInDV"        );
    
    declareProperty("FillIntermediateVertices",        m_jp.FillIntermediateVertices        = false                         );
    declareProperty("DoTruth",                         m_jp.doTruth                         = false                         );
    declareProperty("doPVcompatibilityCut",            m_jp.doPVcompatibilityCut            = true                          );
    declareProperty("RemoveFake2TrkVrt",               m_jp.removeFakeVrt                   = true                          );
    declareProperty("CheckHitPatternStrategy",         m_checkPatternStrategy               = "Classical"                   ); // Either Classical or Extrapolation
    
    // default values are set upstream 
    
    // selection for tracks
    declareProperty("SelTrkMaxCutoff",                 m_jp.SelTrkMaxCutoff                 = 10000                            ); // max number of tracks
    declareProperty("do_PVvetoCut",                    m_jp.do_PVvetoCut                    = true                          );
    declareProperty("do_d0Cut",                        m_jp.do_d0Cut                        = false                          );
    declareProperty("do_z0Cut",                        m_jp.do_z0Cut                        = false                          );
    declareProperty("do_trkChi2Cut",                   m_jp.do_trkChi2Cut                   = false                          );
    declareProperty("TrkPtCut",                        m_jp.TrkPtCut                        = 1000.                         ); // low pT threshold. in [MeV]
    declareProperty("d0TrkPVDstMinCut",                m_jp.d0TrkPVDstMinCut                = 0.                            ); // in [mm]
    declareProperty("d0TrkPVDstMaxCut",                m_jp.d0TrkPVDstMaxCut                = 1000.                         ); // in [mm]
    declareProperty("zTrkPVDstMinCut",                 m_jp.z0TrkPVDstMinCut                = 0.                            ); // in [mm]
    declareProperty("zTrkPVDstMaxCut",                 m_jp.z0TrkPVDstMaxCut                = 1000.                         ); // in [mm]
    declareProperty("TrkChi2Cut",                      m_jp.TrkChi2Cut                      = 100000.                       ); // in terms of chi2 / ndof

    // selection for 2-trk seed vertices
    declareProperty("PVcompatibilityCut",              m_jp.pvCompatibilityCut              = -20.                          ); // in [mm]
    declareProperty("SelVrtChi2Cut",                   m_jp.SelVrtChi2Cut                   = 50                           ); // in terms of chi2 / ndof

    declareProperty("TrkExtrapolator",                 m_jp.trkExtrapolator                 = 2                             );
    
    // Additional ToolHandles
    declareProperty("VertexFitterTool",                m_fitSvc, " Private TrkVKalVrtFitter"                                );
    declareProperty("Extrapolator",                    m_extrapolator                                                       );
    
    // BDT, rushioda
    declareProperty("BDTFilesName",                    m_jp.BDTFilesName                                                       );
    declareProperty("BDTMins_1stTry",                  m_jp.BDTMins_1stTry                                                     );
    declareProperty("BDTMins_2ndTry",                  m_jp.BDTMins_2ndTry                                                     );
    declareProperty("BDTMins_primary",                 m_jp.BDTMins_primary                                                    );
    
    // Seed clustering, rushioda
    declareProperty("SeedDist",                        m_jp.SeedDist                                                           );
 
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecFuzzy::processPrimaryVertices() {
    
    //--------------------------------------------------------
    //  Primary vertex extraction from TES
    //
    
    ATH_CHECK( evtStore()->retrieve( m_primaryVertices, "PrimaryVertices") );
    
    m_thePV = nullptr;
    
    ATH_MSG_DEBUG( "processPrimaryVertices(): pv_size = " << m_primaryVertices->size() );
    
    // Loop over PV container and get number of tracks of each PV
    
    for( auto *vertex : *m_primaryVertices ) {
      
      // Hide (2015-04-21): xAOD::Vertex may contain several types of vertices
      // e.g. if VertexType==NoVtx, this is a dummy vertex.
      // We only need to extract primary vertices here, and skip otherwise.
      
      if( xAOD::VxType::PriVtx != vertex->vertexType() ) continue;
      
      // Not considering pile-up; pick-up the first PV
      m_thePV = vertex;
      
      ATH_MSG_DEBUG("PrimVertex x/y/z/nDOF "
		    << vertex->x() << ","
		    << vertex->y() << ","
		    << vertex->z() << ","
		    << vertex->numberDoF()     );
      
    }
    
    // Use the dummy PV if no PV is composed
    if( !m_thePV ) {
      ATH_MSG_DEBUG("No Reconstructed PV was found. Using the dummy PV instead.");
      for( auto *vertex : *m_primaryVertices ) {
	      if( xAOD::VxType::NoVtx != vertex->vertexType() ) continue;
      }
    }
    
    // if thePV is null, the PV is not found.
    if( !m_thePV ) {
      ATH_MSG_DEBUG("No PV is found in the PV collection!");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG(" Primary vertex successful. thePV = " << m_thePV );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  VrtSecFuzzy::ExtrapolatedPattern* VrtSecFuzzy::extrapolatedPattern( const xAOD::TrackParticle* trk, enum Trk::PropDirection direction ) {
    
    auto* pattern = new ExtrapolatedPattern;
    
    const auto* paramsVector = m_extrapolator->extrapolateBlindly( trk->perigeeParameters(), direction );
    
    TVector3 prevPos( AlgConsts::invalidFloat, AlgConsts::invalidFloat, AlgConsts::invalidFloat );
    
    auto nDisabled = 0;
    
    for( auto* params : *paramsVector ) {
      
      const TVector3 position( params->position().x(), params->position().y(), params->position().z() );
      
      if( prevPos == position ) {
        continue;
      }
      
      prevPos = position;
      
      const auto* detElement = params->associatedSurface().associatedDetectorElement();
      
      if( detElement ) {
        
        enum { Pixel = 1, SCT = 2 };
        
        const auto& id = detElement->identify();
        Flag active = false;
        
        if( m_atlasId->is_pixel(id) ) {
          
          auto idHash = m_pixelId->wafer_hash( id );
          active = m_pixelCondSummarySvc->isActive( idHash );
          
          pattern->emplace_back( std::make_tuple( position, Pixel, m_pixelId->barrel_ec(id), m_pixelId->layer_disk(id), active ) );
          
        } else if( m_atlasId->is_sct(id) ) {
          
          auto idHash = m_sctId->wafer_hash( id );
          active = m_sctCondSummarySvc->isActive( idHash );
          
          pattern->emplace_back( std::make_tuple( position, SCT, m_sctId->barrel_ec(id), m_sctId->layer_disk(id), active ) );
          
        }
        
        if( pattern->size() > 0 ) {
          
          ATH_MSG_VERBOSE(" >> " << __FUNCTION__ << ", track " << trk << ": position = (" << position.Perp() << ", " << position.z() << ", " << position.Phi() << "), detElement ID = " << id << ", active = " << active
                          << ": (det, bec, layer) = (" << std::get<1>( pattern->back() ) << ", " << std::get<2>( pattern->back() ) << ", "  << std::get<3>( pattern->back() ) << ")" );
          
          if( !active ) nDisabled++;
        }
        
      }
      
    }
    
    // cleanup
    for( auto* params : *paramsVector ) { delete params; }
    delete paramsVector;
    
    return pattern;

  }
  
  
  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::checkTrackHitPatternToVertexByExtrapolation( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex )
  {
    
    if( m_extrapolatedPatternBank.find( trk ) == m_extrapolatedPatternBank.end() ) {
      
      std::unique_ptr<ExtrapolatedPattern> exPattern_along( extrapolatedPattern( trk, Trk::alongMomentum ) );
      std::unique_ptr<ExtrapolatedPattern> exPattern_oppos( extrapolatedPattern( trk, Trk::oppositeMomentum ) );
      
      m_extrapolatedPatternBank.emplace( trk, std::make_pair( std::move(exPattern_along), std::move(exPattern_oppos) ) );
      
    }
    
    auto& exPattern = m_extrapolatedPatternBank.at( trk );
    
    using LayerCombination = std::vector<int>;
    
    static std::map<LayerCombination, unsigned> layerMap;
    if( layerMap.size() == 0 ) {
      layerMap[ { 1, 0, 0 } ] = Trk::pixelBarrel0;
      layerMap[ { 1, 0, 1 } ] = Trk::pixelBarrel1;
      layerMap[ { 1, 0, 2 } ] = Trk::pixelBarrel2;
      layerMap[ { 1, 0, 3 } ] = Trk::pixelBarrel3;
      
      layerMap[ { 1, 2, 0 } ] = Trk::pixelEndCap0;
      layerMap[ { 1, 2, 1 } ] = Trk::pixelEndCap1;
      layerMap[ { 1, 2, 2 } ] = Trk::pixelEndCap2;
      layerMap[ { 1,-2, 0 } ] = Trk::pixelEndCap0;
      layerMap[ { 1,-2, 1 } ] = Trk::pixelEndCap1;
      layerMap[ { 1,-2, 2 } ] = Trk::pixelEndCap2;
      
      layerMap[ { 2, 0, 0 } ] = Trk::sctBarrel0;
      layerMap[ { 2, 0, 1 } ] = Trk::sctBarrel1;
      layerMap[ { 2, 0, 2 } ] = Trk::sctBarrel2;
      layerMap[ { 2, 0, 3 } ] = Trk::sctBarrel3;

      layerMap[ { 2, 2, 0 } ] = Trk::sctEndCap0;
      layerMap[ { 2, 2, 1 } ] = Trk::sctEndCap1;
      layerMap[ { 2, 2, 2 } ] = Trk::sctEndCap2;
      layerMap[ { 2, 2, 3 } ] = Trk::sctEndCap3;
      layerMap[ { 2, 2, 4 } ] = Trk::sctEndCap4;
      layerMap[ { 2, 2, 5 } ] = Trk::sctEndCap5;
      layerMap[ { 2, 2, 6 } ] = Trk::sctEndCap6;
      layerMap[ { 2, 2, 7 } ] = Trk::sctEndCap7;
      layerMap[ { 2, 2, 8 } ] = Trk::sctEndCap8;
      layerMap[ { 2,-2, 0 } ] = Trk::sctEndCap0;
      layerMap[ { 2,-2, 1 } ] = Trk::sctEndCap1;
      layerMap[ { 2,-2, 2 } ] = Trk::sctEndCap2;
      layerMap[ { 2,-2, 3 } ] = Trk::sctEndCap3;
      layerMap[ { 2,-2, 4 } ] = Trk::sctEndCap4;
      layerMap[ { 2,-2, 5 } ] = Trk::sctEndCap5;
      layerMap[ { 2,-2, 6 } ] = Trk::sctEndCap6;
      layerMap[ { 2,-2, 7 } ] = Trk::sctEndCap7;
      layerMap[ { 2,-2, 8 } ] = Trk::sctEndCap8;
    }
    
    enum { position=0, detector=1, bec=2, layer=3, isActive=4 };
    
    // Labmda!
    auto getDetectorType = [&]( const ExtrapolatedPoint& point ) -> unsigned {
      
      const LayerCombination comb { std::get<detector>( point ), std::get<bec>( point ), std::get<layer>( point ) };
      
      for( auto& pair : layerMap ) {
        if( pair.first == comb ) {
          return pair.second;
        }
      }
      
      return AlgConsts::invalidUnsigned;
    };
    
    enum { kShouldNotHaveHit, kShouldHaveHit, kMayHaveHit };
    std::vector<unsigned> expectedHitPattern(Trk::numberOfDetectorTypes, kShouldNotHaveHit);
    
    auto minExpectedRadius = AlgConsts::maxValue;
    
    // Loop over extrapolated points (along direction)
    auto& exPattern_along = *( exPattern.first  );
    
    for( auto itr = exPattern_along.begin(); itr != exPattern_along.end(); ++itr ) {
      if( std::next( itr ) == exPattern_along.end() ) continue;
      
      const auto& point      = *itr;
      const auto& nextPoint  = *( std::next( itr ) );
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__ << ": isActive = " << std::get<isActive>( point ) );
      
      auto& thisPos = std::get<position>( point );
      auto& nextPos = std::get<position>( nextPoint );
      
      auto sectionVector = nextPos - thisPos;
      auto vertexVector  = TVector3( vertex.x(), vertex.y(), vertex.z() ) - thisPos;
      
      
      const auto& detectorType = getDetectorType( point );
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__ << ": detType = " << detectorType );
      
      if( detectorType == AlgConsts::invalidUnsigned ) continue;
      if( detectorType >= Trk::numberOfDetectorTypes ) continue;
      
      // if the vertex is nearby (within 10 mm), the hit may be presnet ("X")
      if( vertexVector.Mag() < 10. ) {
        expectedHitPattern.at( detectorType ) = kMayHaveHit;
        continue;
      }
      
      // if the front-end module is not active, then the hit is not expected,
      // which means the hit may be present
      if( false == std::get<isActive>( point ) ) {
        expectedHitPattern.at( detectorType ) = kMayHaveHit;
        continue;
      }
      
      // if the inner product of the above two vectors is positive,
      // then point is inner than the vertex.
      // Else, the point is outer than the vertex and expect to have hits
      // when the track is originated from the vertex.
      
      if( sectionVector.Mag() == 0. ) continue;
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__
                       << ": hitPos = (" << thisPos.Perp() << ", " << thisPos.z() << ", " << thisPos.Phi() << ")"
                       << ", sectionVec = (" << sectionVector.Perp() << ", " << sectionVector.z() << ", " << sectionVector.Phi() << ")"
                       << ", vertexVec = (" << vertexVector.Perp() << ", " << vertexVector.z() << ", " << vertexVector.Phi() << ")"
                       << ", cos(s,v)  = " << sectionVector * vertexVector / ( sectionVector.Mag() * vertexVector.Mag() + AlgConsts::infinitesimal ) );
      
      if( sectionVector * vertexVector > 0. ) continue;
      
      if( minExpectedRadius > thisPos.Perp() ) minExpectedRadius = thisPos.Perp();
      
      // now, the hit is expected to present.
      
      expectedHitPattern.at( detectorType ) = kShouldHaveHit;
    }
    
    // Loop over extrapolated points (opposite direction)
    auto& exPattern_oppos = *( exPattern.second );
    bool oppositeFlag = false;
    
    for( auto itr = exPattern_oppos.begin(); itr != exPattern_oppos.end(); ++itr ) {
      if( std::next( itr ) == exPattern_oppos.end() ) continue;
      
      const auto& point      = *itr;
      const auto& nextPoint  = *( std::next( itr ) );
      
      auto& thisPos = std::get<position>( point );
      auto& nextPos = std::get<position>( nextPoint );
      
      auto sectionVector = nextPos - thisPos;
      auto vertexVector  = TVector3( vertex.x(), vertex.y(), vertex.z() ) - thisPos;
      
      const auto& detectorType = getDetectorType( point );
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__ << ": detType = " << detectorType );
      
      ATH_MSG_DEBUG( " > " <<  __FUNCTION__
                       << ": hitPos = (" << thisPos.Perp() << ", " << thisPos.z() << ", " << thisPos.Phi() << ")"
                       << ", vertex = (" << vertex.perp() << ", " << vertex.z() << ", " << vertex.phi() << ")"
                       << ", cos(s,v)  = " << sectionVector * vertexVector / ( sectionVector.Mag() * vertexVector.Mag() + AlgConsts::infinitesimal ) );
      
      if( detectorType == AlgConsts::invalidUnsigned ) continue;
      if( detectorType >= Trk::numberOfDetectorTypes ) continue;
      
      if( sectionVector * vertexVector < 0. ) {
        oppositeFlag = true;
      }
    }
    
    // If the first expected point's radius is smaller than the vertex radius,
    // it's the case that the vertex was reconstructed in the opposite phi-direction
    // to the track outgoing direction. Such a case should be rejected.
    // bool oppositeFlag = ( minExpectedRadius < vertex.perp() );
    
    std::string msg = "Expected hit pattern: ";
    for( unsigned i=0; i<Trk::numberOfDetectorTypes; i++) {
      msg += Form("%s", expectedHitPattern.at(i) < kMayHaveHit? Form("%u", expectedHitPattern.at(i)) : "X" );
    }
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": " << msg );
    
    msg = "Recorded hit pattern: ";
    for( unsigned i=0; i<Trk::numberOfDetectorTypes; i++) {
      msg += Form("%u", ( trk->hitPattern() >> i ) & 1 );
    }
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": " << msg );
    
    std::vector<unsigned> matchedLayers;
    
    for( unsigned i=0; i<Trk::numberOfDetectorTypes; i++) {
      const unsigned recordedPattern = ( (trk->hitPattern()>>i) & 1 );
      
      if( expectedHitPattern.at(i) == kMayHaveHit ) {
        matchedLayers.emplace_back( i );
      } else if( expectedHitPattern.at(i) == kShouldHaveHit ) {
        if( expectedHitPattern.at(i) != recordedPattern ) {
          break;
        } else {
          matchedLayers.emplace_back( i );
        }
      } else {
        if( expectedHitPattern.at(i) != recordedPattern ) {
          break;
        }
      }
      
    }
    
    uint8_t PixelHits = 0;
    uint8_t SctHits   = 0; 
    uint8_t TRTHits   = 0;
    if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits ) ) ) PixelHits =0;
    if( !(trk->summaryValue( SctHits,   xAOD::numberOfSCTHits   ) ) ) SctHits   =0;
    if( !(trk->summaryValue( TRTHits,   xAOD::numberOfTRTHits   ) ) ) TRTHits   =0;
    
    auto dphi = trk->phi() - vertex.phi();
    while( dphi >  TMath::Pi() ) dphi -= TMath::TwoPi();
    while( dphi < -TMath::Pi() ) dphi += TMath::TwoPi();
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": vtx phi = " << vertex.phi() << ", track phi = " << trk->phi() << ", dphi = " << dphi
                   << ", oppositeFlag = " << oppositeFlag
                   << ", nPixelHits = " << static_cast<int>(PixelHits)
                   << ", nSCTHits = " << static_cast<int>(SctHits)
                   << ", nTRTHits = " << static_cast<int>(TRTHits)
                   << ", nMatchedLayers = " << matchedLayers.size() );
    
    if( PixelHits == 0 && vertex.perp() > 300. ) {
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": vertex r > 300 mm, w/o no pixel hits" );
    }
    
    
    // Requires the first 2 layers with the hit matches.
    if( matchedLayers.size() < 2 ) return false;
    
    // In case of the first matched layer is not within pixel barrel, requires the first 4 layers with the hit match
    if( matchedLayers.at(0) >= Trk::pixelEndCap0 ) {
      if( matchedLayers.size() < 4 ) return false;
    }
    
    // Sometimes the vertex is reconstructed at the opposite phi direction.
    // In this case, the pattern match may pass.
    // This can be avoided by requiring that the 
    if( oppositeFlag ) return false;
    
    return true;
  }
    

  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::patternCheckRun2( const uint32_t& pattern, const Amg::Vector3D& vertex ) {
    
    //
    // rough guesses for active layers:
    // BeamPipe: 23.5-24.3
    // IBL: 31.0-38.4
    // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1
    // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
    //
      
    const double rad  = vertex.perp();
    const double absz = std::abs( vertex.z() );
    
    // vertex area classification
    enum vertexArea {
      insideBeamPipe,
	
      insidePixelBarrel0,
      aroundPixelBarrel0,
	
      outsidePixelBarrel0_and_insidePixelBarrel1,
      aroundPixelBarrel1,
	
      outsidePixelBarrel1_and_insidePixelBarrel2,
      aroundPixelBarrel2,
	
      outsidePixelBarrel2_and_insidePixelBarrel3,
      aroundPixelBarrel3,
	
      outsidePixelBarrel3_and_insideSctBarrel0,
      aroundSctBarrel0,
	
      outsideSctBarrel0_and_insideSctBarrel1,
      aroundSctBarrel1,
    };
      
    // Mutually exclusive vertex position pattern
    int vertex_pattern = 0;
    if( rad < 23.50 ) {
      vertex_pattern = insideBeamPipe;
	
    } else if( rad < 31.0 && absz < 331.5 ) {
      vertex_pattern = insidePixelBarrel0;
	
    } else if( rad < 38.4 && absz < 331.5 ) {
      vertex_pattern = aroundPixelBarrel0;
	
    } else if( rad < 47.7 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel0_and_insidePixelBarrel1;
	
    } else if( rad < 54.4 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel1;
	
    } else if( rad < 85.5 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel1_and_insidePixelBarrel2;
	
    } else if( rad < 92.2 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel2;
	
    } else if( rad < 119.3 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel2_and_insidePixelBarrel3;
	
    } else if( rad < 126.1 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel3;
	
    } else if( rad < 290 && absz < 749.0 ) {
      vertex_pattern = outsidePixelBarrel3_and_insideSctBarrel0;
	
    } else if( rad < 315 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel0;
	
    } else if( rad < 360 && absz < 749.0 ) {
      vertex_pattern = outsideSctBarrel0_and_insideSctBarrel1;
	
    } else if( rad < 390 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel1;
	
    } else {
    }
    
    unsigned nPixelLayers { 0 };
    {
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel0) );
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel1) );
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel2) );
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel3) );
      nPixelLayers += ( pattern & (1 << Trk::pixelEndCap0) );
      nPixelLayers += ( pattern & (1 << Trk::pixelEndCap1) );
      nPixelLayers += ( pattern & (1 << Trk::pixelEndCap2) );
    }
    
    //////////////////////////////////////////////////////////////////////////////////
    if( vertex_pattern == insideBeamPipe ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if( nPixelLayers < 3 )                     return false;
	
	
    } else if( vertex_pattern == insidePixelBarrel0 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if( nPixelLayers < 3 )                     return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel0 ) {
	
      // require nothing for PixelBarrel0
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( nPixelLayers < 2 )                     return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel0_and_insidePixelBarrel1 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( nPixelLayers < 2 )                     return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel1 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      // require nothing for PixelBarrel1
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( nPixelLayers < 2 )                     return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( nPixelLayers < 2 )                     return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel2 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      // require nothing for PixelBarrel2
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
      

    else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
	
    else if( vertex_pattern == aroundPixelBarrel3 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      // require nothing for PixelBarrel3
      if( ! (pattern & (1<<Trk::sctBarrel0)) )   return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel0 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      // require nothing for SctBarrel0
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel1 ) {
      if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
      // require nothing for SctBarrel1
      if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
    }
    //////////////////////////////////////////////////////////////////////////////////
      
    return true;
      
  }
  
  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::patternCheckRun2OuterOnly( const uint32_t& pattern, const Amg::Vector3D& vertex ) {
    
    //
    // rough guesses for active layers:
    // BeamPipe: 23.5-24.3
    // IBL: 31.0-38.4
    // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1
    // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
    //
      
    const double rad  = vertex.perp();
    const double absz = std::abs( vertex.z() );
    
    // vertex area classification
    enum vertexArea {
      insideBeamPipe,
	
      insidePixelBarrel0,
      aroundPixelBarrel0,
	
      outsidePixelBarrel0_and_insidePixelBarrel1,
      aroundPixelBarrel1,
	
      outsidePixelBarrel1_and_insidePixelBarrel2,
      aroundPixelBarrel2,
	
      outsidePixelBarrel2_and_insidePixelBarrel3,
      aroundPixelBarrel3,
	
      outsidePixelBarrel3_and_insideSctBarrel0,
      aroundSctBarrel0,
	
      outsideSctBarrel0_and_insideSctBarrel1,
      aroundSctBarrel1,
    };
      
    // Mutually exclusive vertex position pattern
    int vertex_pattern = 0;
    if( rad < 23.50 ) {
      vertex_pattern = insideBeamPipe;
	
    } else if( rad < 31.0 && absz < 331.5 ) {
      vertex_pattern = insidePixelBarrel0;
	
    } else if( rad < 38.4 && absz < 331.5 ) {
      vertex_pattern = aroundPixelBarrel0;
	
    } else if( rad < 47.7 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel0_and_insidePixelBarrel1;
	
    } else if( rad < 54.4 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel1;
	
    } else if( rad < 85.5 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel1_and_insidePixelBarrel2;
	
    } else if( rad < 92.2 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel2;
	
    } else if( rad < 119.3 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel2_and_insidePixelBarrel3;
	
    } else if( rad < 126.1 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel3;
	
    } else if( rad < 290 && absz < 749.0 ) {
      vertex_pattern = outsidePixelBarrel3_and_insideSctBarrel0;
	
    } else if( rad < 315 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel0;
	
    } else if( rad < 360 && absz < 749.0 ) {
      vertex_pattern = outsideSctBarrel0_and_insideSctBarrel1;
	
    } else if( rad < 390 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel1;
	
    } else {
    }
      
      
    unsigned nPixelLayers { 0 };
    {
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel0) );
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel1) );
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel2) );
      nPixelLayers += ( pattern & (1 << Trk::pixelBarrel3) );
      nPixelLayers += ( pattern & (1 << Trk::pixelEndCap0) );
      nPixelLayers += ( pattern & (1 << Trk::pixelEndCap1) );
      nPixelLayers += ( pattern & (1 << Trk::pixelEndCap2) );
    }
    
    //////////////////////////////////////////////////////////////////////////////////
    if( vertex_pattern == insideBeamPipe ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( nPixelLayers < 3                     ) return false;
      
    } else if( vertex_pattern == insidePixelBarrel0 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( nPixelLayers < 3                     ) return false;
      
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel0 ) {
	
      // require nothing for PixelBarrel0
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( nPixelLayers < 3                     ) return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel0_and_insidePixelBarrel1 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( nPixelLayers < 3                     ) return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel1 ) {
	
      // require nothing for PixelBarrel1
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if( nPixelLayers < 2                     ) return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if( nPixelLayers < 2                     ) return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel2 ) {
	
      // require nothing for PixelBarrel2
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
      

    else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
	
    else if( vertex_pattern == aroundPixelBarrel3 ) {
	
      // require nothing for PixelBarrel3
      if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
      if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel0 ) {
	
      // require nothing for SctBarrel0
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
    }
      
      
    else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
      
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel1 ) {
      // require nothing for SctBarrel1
      if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel3)) ) return false;
    }
    //////////////////////////////////////////////////////////////////////////////////
      
    return true;
      
  }
  
  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::patternCheckRun1( const uint32_t& pattern, const Amg::Vector3D& vertex ) {
    //
    // rough guesses for active layers:
    // BeamPipe: 25.0
    // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1
    // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
    //
      
    const double rad  = vertex.perp();
    const double absz = std::abs( vertex.z() );
    
    // vertex area classification
    enum vertexArea {
      insideBeamPipe,
	
      insidePixelBarrel1,
      aroundPixelBarrel1,
	
      outsidePixelBarrel1_and_insidePixelBarrel2,
      aroundPixelBarrel2,
	
      outsidePixelBarrel2_and_insidePixelBarrel3,
      aroundPixelBarrel3,
	
      outsidePixelBarrel3_and_insideSctBarrel0,
      aroundSctBarrel0,
	
      outsideSctBarrel0_and_insideSctBarrel1,
      aroundSctBarrel1,
    };
      
    // Mutually exclusive vertex position pattern
    Int_t vertex_pattern = 0;
    if( rad < 25.00 ) {
      vertex_pattern = insideBeamPipe;
	
    } else if( rad < 47.7 && absz < 400.5 ) {
      vertex_pattern = insidePixelBarrel1;
	
    } else if( rad < 54.4 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel1;
	
    } else if( rad < 85.5 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel1_and_insidePixelBarrel2;
	
    } else if( rad < 92.2 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel2;
	
    } else if( rad < 119.3 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel2_and_insidePixelBarrel3;
	
    } else if( rad < 126.1 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel3;
	
    } else if( rad < 290 && absz < 749.0 ) {
      vertex_pattern = outsidePixelBarrel3_and_insideSctBarrel0;
	
    } else if( rad < 315 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel0;
	
    } else if( rad < 360 && absz < 749.0 ) {
      vertex_pattern = outsideSctBarrel0_and_insideSctBarrel1;
	
    } else if( rad < 390 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel1;
	
    } else {
    }
      
      
    //////////////////////////////////////////////////////////////////////////////////
    if( vertex_pattern == insideBeamPipe ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	
    }
      
      
    else if( vertex_pattern == insidePixelBarrel1 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel1 ) {
	
      // require nothing for PixelBarrel1
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel2 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      // require nothing for PixelBarrel2
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
      

    else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
	
    else if( vertex_pattern == aroundPixelBarrel3 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      // require nothing for PixelBarrel3
      if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel0 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      // require nothing for SctBarrel0
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
	
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel1 ) {
      if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
      // require nothing for SctBarrel1
      if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
    }
    //////////////////////////////////////////////////////////////////////////////////
      
    return true;
  }
  
  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::patternCheckRun1OuterOnly( const uint32_t& pattern, const Amg::Vector3D& vertex ) {
    //
    // rough guesses for active layers:
    // BeamPipe: 25.0
    // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1
    // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
    //
      
    const double rad  = vertex.perp();
    const double absz = std::abs( vertex.z() );
    
    // vertex area classification
    enum vertexArea {
      insideBeamPipe,
	
      insidePixelBarrel1,
      aroundPixelBarrel1,
	
      outsidePixelBarrel1_and_insidePixelBarrel2,
      aroundPixelBarrel2,
	
      outsidePixelBarrel2_and_insidePixelBarrel3,
      aroundPixelBarrel3,
	
      outsidePixelBarrel3_and_insideSctBarrel0,
      aroundSctBarrel0,
	
      outsideSctBarrel0_and_insideSctBarrel1,
      aroundSctBarrel1,
    };
      
    // Mutually exclusive vertex position pattern
    Int_t vertex_pattern = 0;
    if( rad < 25.00 ) {
      vertex_pattern = insideBeamPipe;
	
    } else if( rad < 47.7 && absz < 400.5 ) {
      vertex_pattern = insidePixelBarrel1;
	
    } else if( rad < 54.4 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel1;
	
    } else if( rad < 85.5 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel1_and_insidePixelBarrel2;
	
    } else if( rad < 92.2 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel2;
	
    } else if( rad < 119.3 && absz < 400.5 ) {
      vertex_pattern = outsidePixelBarrel2_and_insidePixelBarrel3;
	
    } else if( rad < 126.1 && absz < 400.5 ) {
      vertex_pattern = aroundPixelBarrel3;
	
    } else if( rad < 290 && absz < 749.0 ) {
      vertex_pattern = outsidePixelBarrel3_and_insideSctBarrel0;
	
    } else if( rad < 315 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel0;
	
    } else if( rad < 360 && absz < 749.0 ) {
      vertex_pattern = outsideSctBarrel0_and_insideSctBarrel1;
	
    } else if( rad < 390 && absz < 749.0 ) {
      vertex_pattern = aroundSctBarrel1;
	
    } else {
    }
      
      
    //////////////////////////////////////////////////////////////////////////////////
    if( vertex_pattern == insideBeamPipe ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	
    }
      
      
    else if( vertex_pattern == insidePixelBarrel1 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel1 ) {
	
      // require nothing for PixelBarrel1
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundPixelBarrel2 ) {
	
      // require nothing for PixelBarrel2
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
      

    else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
      if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
    }
	
    else if( vertex_pattern == aroundPixelBarrel3 ) {
	
      // require nothing for PixelBarrel3
      if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
    }
      
      
    else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
      if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel0 ) {
	
      // require nothing for SctBarrel0
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
	
      if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
    }
      
      
    else if( vertex_pattern == aroundSctBarrel1 ) {
      // require nothing for SctBarrel1
      if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
    }
    //////////////////////////////////////////////////////////////////////////////////
      
    return true;
  }
  
  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::patternCheck( const uint32_t& pattern, const Amg::Vector3D& vertex ) {
    bool flag = false;
    
    if( m_jp.geoModel == VKalVrtAthena::GeoModel::Run2 ) {
      flag = patternCheckRun2( pattern, vertex );
    } else if ( m_jp.geoModel == VKalVrtAthena::GeoModel::Run1 ) {
      flag = patternCheckRun1( pattern, vertex );
    }
    
    return flag;
  }

  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::patternCheckOuterOnly( const uint32_t& pattern, const Amg::Vector3D& vertex ) {
    bool flag = false;
    
    if( m_jp.geoModel == VKalVrtAthena::GeoModel::Run2 ) {
      flag = patternCheckRun2OuterOnly( pattern, vertex );
    } else if ( m_jp.geoModel == VKalVrtAthena::GeoModel::Run1 ) {
      flag = patternCheckRun1OuterOnly( pattern, vertex );
    }
    
    return flag;
  }


  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::checkTrackHitPatternToVertex( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex )
  {
    
    const uint32_t pattern = trk->hitPattern();
    
    return patternCheck( pattern, vertex );
	
  }
  

  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::checkTrackHitPatternToVertexOuterOnly( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex )
  {
    
    const uint32_t pattern = trk->hitPattern();
    
    return patternCheckOuterOnly( pattern, vertex );
	
  }
  

  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::checkTrackHitPatternToVertexByExtrapolationAssist( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex )
  {
    
    if( m_extrapolatedPatternBank.find( trk ) == m_extrapolatedPatternBank.end() ) {
      
      std::unique_ptr<ExtrapolatedPattern> exPattern_along( extrapolatedPattern( trk, Trk::alongMomentum ) );
      
      m_extrapolatedPatternBank.emplace( trk, std::make_pair( std::move(exPattern_along), nullptr ) );
      
    }
    
    if( vertex.perp() < 31.0 ) {
      double dphi = trk->phi() - vertex.phi();
      while( dphi >  TMath::Pi() ) { dphi -= TMath::TwoPi(); }
      while( dphi < -TMath::Pi() ) { dphi += TMath::TwoPi(); }
      if( cos(dphi) < -0.8 ) return false;
    }
    
    auto& exPattern = m_extrapolatedPatternBank.at( trk );
    
    using LayerCombination = std::vector<int>;
    
    static std::map<LayerCombination, unsigned> layerMap;
    if( layerMap.size() == 0 ) {
      layerMap[ { 1, 0, 0 } ] = Trk::pixelBarrel0;
      layerMap[ { 1, 0, 1 } ] = Trk::pixelBarrel1;
      layerMap[ { 1, 0, 2 } ] = Trk::pixelBarrel2;
      layerMap[ { 1, 0, 3 } ] = Trk::pixelBarrel3;
      
      layerMap[ { 1, 2, 0 } ] = Trk::pixelEndCap0;
      layerMap[ { 1, 2, 1 } ] = Trk::pixelEndCap1;
      layerMap[ { 1, 2, 2 } ] = Trk::pixelEndCap2;
      layerMap[ { 1,-2, 0 } ] = Trk::pixelEndCap0;
      layerMap[ { 1,-2, 1 } ] = Trk::pixelEndCap1;
      layerMap[ { 1,-2, 2 } ] = Trk::pixelEndCap2;
      
      layerMap[ { 2, 0, 0 } ] = Trk::sctBarrel0;
      layerMap[ { 2, 0, 1 } ] = Trk::sctBarrel1;
      layerMap[ { 2, 0, 2 } ] = Trk::sctBarrel2;
      layerMap[ { 2, 0, 3 } ] = Trk::sctBarrel3;

      layerMap[ { 2, 2, 0 } ] = Trk::sctEndCap0;
      layerMap[ { 2, 2, 1 } ] = Trk::sctEndCap1;
      layerMap[ { 2, 2, 2 } ] = Trk::sctEndCap2;
      layerMap[ { 2, 2, 3 } ] = Trk::sctEndCap3;
      layerMap[ { 2, 2, 4 } ] = Trk::sctEndCap4;
      layerMap[ { 2, 2, 5 } ] = Trk::sctEndCap5;
      layerMap[ { 2, 2, 6 } ] = Trk::sctEndCap6;
      layerMap[ { 2, 2, 7 } ] = Trk::sctEndCap7;
      layerMap[ { 2, 2, 8 } ] = Trk::sctEndCap8;
      layerMap[ { 2,-2, 0 } ] = Trk::sctEndCap0;
      layerMap[ { 2,-2, 1 } ] = Trk::sctEndCap1;
      layerMap[ { 2,-2, 2 } ] = Trk::sctEndCap2;
      layerMap[ { 2,-2, 3 } ] = Trk::sctEndCap3;
      layerMap[ { 2,-2, 4 } ] = Trk::sctEndCap4;
      layerMap[ { 2,-2, 5 } ] = Trk::sctEndCap5;
      layerMap[ { 2,-2, 6 } ] = Trk::sctEndCap6;
      layerMap[ { 2,-2, 7 } ] = Trk::sctEndCap7;
      layerMap[ { 2,-2, 8 } ] = Trk::sctEndCap8;
    }
    
    enum { position=0, detector=1, bec=2, layer=3, isActive=4 };
    
    // Labmda!
    auto getDetectorType = [&]( const ExtrapolatedPoint& point ) -> unsigned {
      
      const LayerCombination comb { std::get<detector>( point ), std::get<bec>( point ), std::get<layer>( point ) };
      
      for( auto& pair : layerMap ) {
        if( pair.first == comb ) {
          return pair.second;
        }
      }
      
      return AlgConsts::invalidUnsigned;
    };
    
    uint32_t disabledPattern { 0 };
    
    // Loop over extrapolated points (along direction)
    auto& exPattern_along = *( exPattern.first  );
    
    for( auto itr = exPattern_along.begin(); itr != exPattern_along.end(); ++itr ) {
      if( std::next( itr ) == exPattern_along.end() ) continue;
      
      const auto& point = *itr;
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__ << ": isActive = " << std::get<isActive>( point ) );
      
      if( !std::get<isActive>( point ) ) {
        const auto& detectorType = getDetectorType( point );
        disabledPattern += (1 << detectorType);
      }
    }
    
    uint32_t hitPattern      = trk->hitPattern();
    uint32_t modifiedPattern = disabledPattern | hitPattern;
    
    std::string msg = "Disabled hit pattern: ";
    for( unsigned i=0; i<Trk::numberOfDetectorTypes; i++) {
      msg += Form("%u", ( disabledPattern >> i ) & 1 );
    }
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": " << msg );
    
    msg = "Recorded hit pattern: ";
    for( unsigned i=0; i<Trk::numberOfDetectorTypes; i++) {
      msg += Form("%u", ( hitPattern >> i ) & 1 );
    }
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": " << msg );
    
    return patternCheck( modifiedPattern, vertex );
    
  }
  
  
  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::passedFakeReject( const Amg::Vector3D& FitVertex,
					  const xAOD::TrackParticle *itrk,
					  const xAOD::TrackParticle *jtrk  )
  {
    
    const bool& check_itrk = ( this->*m_patternStrategyFuncs[m_checkPatternStrategy] )( itrk, FitVertex );
    const bool& check_jtrk = ( this->*m_patternStrategyFuncs[m_checkPatternStrategy] )( jtrk, FitVertex );
    
    return ( check_itrk && check_jtrk );
    
  }    
  
} // end of namespace VKalVrtAthena


