/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODMuon/versions/MuonSegment_v1.h"

namespace xAOD {

  MuonSegment_v1::MuonSegment_v1()
    : SG::AuxElement() {    
  }

  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, x )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, y )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, z )

  void MuonSegment_v1::setPosition(float x, float y, float z) {
    static const Accessor< float > acc1( "x" );
    acc1( *this ) = x;
    static const Accessor< float > acc2( "y" );
    acc2( *this ) = y;
    static const Accessor< float > acc3( "z" );
    acc3( *this ) = z;
  }
  
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, px )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, py )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, pz )

  void MuonSegment_v1::setDirection(float px, float py, float pz) {
    static const Accessor< float > acc1( "px" );
    acc1( *this ) = px;
    static const Accessor< float > acc2( "py" );
    acc2( *this ) = py;
    static const Accessor< float > acc3( "pz" );
    acc3( *this ) = pz;
  }

  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, t0      )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, t0error )

  void MuonSegment_v1::setT0Error(float t0, float t0error) {
    static const Accessor< float > acc1( "t0" );
    acc1( *this ) = t0;  
    static const Accessor< float > acc2( "t0error" );
    acc2( *this ) = t0error;   
  }

  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, chiSquared )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, float, numberDoF  )

  void MuonSegment_v1::setFitQuality(float chiSquared, float numberDoF) {
    static const Accessor< float > acc1( "chiSquared" );
    acc1( *this ) = chiSquared;  
    static const Accessor< float > acc2( "numberDoF" );
    acc2( *this ) = numberDoF;   
  }

  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, int, sector )
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( MuonSegment_v1, int, Muon::MuonStationIndex::ChIndex, chamberIndex )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, int, etaIndex )
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( MuonSegment_v1, int, Muon::MuonStationIndex::TechnologyIndex, technology )

  void MuonSegment_v1::setIdentifier(int sector, Muon::MuonStationIndex::ChIndex chamberIndex, int etaIndex, Muon::MuonStationIndex::TechnologyIndex technology) {
    static const Accessor< int   > acc1( "sector" );
    acc1( *this ) = sector;
    static const Accessor< int   > acc2( "chamberIndex" );
    acc2( *this ) = static_cast<int>(chamberIndex);
    static const Accessor< int   > acc3( "etaIndex" );
    acc3( *this ) = etaIndex;
    static const Accessor< int   > acc4( "technology" );
    acc4( *this ) = static_cast<int>(technology);
  }

  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, int  , nPrecisionHits )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, int  , nPhiLayers     )
  AUXSTORE_PRIMITIVE_GETTER( MuonSegment_v1, int  , nTrigEtaLayers )

  void MuonSegment_v1::setNHits(int nPrecisionHits, int nPhiLayers, int nTrigEtaLayers) {
    static const Accessor< int   > acc1( "nPrecisionHits" );
    acc1( *this ) = nPrecisionHits;
    static const Accessor< int   > acc2( "nPhiLayers" );
    acc2( *this ) = nPhiLayers;
    static const Accessor< int   > acc3( "nTrigEtaLayers" );
    acc3( *this ) = nTrigEtaLayers;
  }

  #ifndef XAOD_ANALYSIS    
    AUXSTORE_OBJECT_SETTER_AND_GETTER( MuonSegment_v1, ElementLink< ::Trk::SegmentCollection > , muonSegment, setMuonSegment )
  #endif // not XAOD_ANALYSIS

} // namespace xAOD
