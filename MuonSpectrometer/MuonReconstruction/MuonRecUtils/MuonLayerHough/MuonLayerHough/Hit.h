/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGH_HIT_H
#define MUONHOUGH_HIT_H

#include <functional>
#include <map>
#include "CxxUtils/fpcompare.h"
#include "MuonIdHelpers/MuonStationIndex.h"
namespace Trk {
  class PrepRawData;
}
namespace Muon {
  class TgcClusterObj3D;
}

namespace MuonHough {
  static const int UNINITIALIZED = -99999;
  /// struct containing additional debug information on the hits that is not needed for the actual alg
  /// but very useful for debugging
  class HitDebugInfo {
  public:
    HitDebugInfo();

    HitDebugInfo( int type_, int sector_, Muon::MuonStationIndex::DetectorRegionIndex region_,
                  Muon::MuonStationIndex::LayerIndex layer_, int sublayer_ );
      
    int type;           /// technology type
    int sector;         /// sector
    Muon::MuonStationIndex::DetectorRegionIndex region; /// detector region (endcapA/barrel/endcapC)
    Muon::MuonStationIndex::LayerIndex layer;           /// layer (inner/middle/outer)
    int sublayer;       /// sublayer within layer
    int pdgId;          /// pdgId of the associated truth particle (if any)
    int barcode;        /// barcode of truth particle
    int muonIndex;      /// index of reconstructed muon 
    int clusterSize;    /// cluster size 
    int clusterLayers;  /// number of layers in the cluster
    int isEtaPhi;       /// flag whether confirmed in the other projection
    int trigConfirm;    /// flag whether confirmed by a trigger hit
    int binpos;         /// spacial bin of the hit with the highest value in the hough space 
    int bintheta;       /// angular bin of the hit with the highest value in the hough space 
    float time;         /// measured time 
    float r;            /// drift radius for MDT, strip pitch for strips
    float ph;           /// maximum value in the hough space 
    float phn;          /// maximum value in the hough space in neighbouring sector; dege
    float ph1;          /// maximum value of the hit projected into the first other layer in the hough space 
    float ph2;          /// maximum value of the hit projected into the second other layer in the hough space 
    float rot;          /// angle corresponding to the maximum in the hough space; poin to IP
  };


  /// struct containing all hit information needed for the Hough transform
  class Hit {
  public:
    
    /// constructor, takes ownership of the HitDebugInfo pointer
    Hit( int layer_, float x_, float ymin_, float ymax_, float w_, HitDebugInfo* d_ = 0, 
	 const Trk::PrepRawData* prd_ = 0, const Muon::TgcClusterObj3D* tgc_ = 0 );

    /// destructor
    ~Hit();

    /// copy constructor
    Hit( const Hit& h_);

    /// =operator
    Hit& operator=( const Hit& h_ );

    int   layer;        /// layer identifier (packed word containing technology/sublayer)
    float x;            /// global hit position (x=r in barrel, x=z in endcap)
    float ymin;         /// minimum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
    float ymax;         /// maximum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
    float w;            /// weight of the hit

    /// access to debug information
    const HitDebugInfo* debugInfo() const { return m_debug; } 
    HitDebugInfo* debugInfo() { return m_debug; }

    /// access to assiciated hit, either the prd or the tgc pointer is set in athena
    const Trk::PrepRawData* prd;
    const Muon::TgcClusterObj3D* tgc;

  private:
  
    HitDebugInfo* m_debug;  /// pointer to debug information

    /// copy function for internal use
    void copy( const Hit& hit );
  };

  /// struct containing all hit information needed for the Hough transform
  struct PhiHit {
    
    /// constructor, takes ownership of the HitDebugInfo pointer
    PhiHit( int layer_, float r_, float phimin_, float phimax_, float w_, HitDebugInfo* d_ = 0, 
	    const Trk::PrepRawData* prd_ = 0, const Muon::TgcClusterObj3D* tgc_ = 0 );

    /// destructor
    ~PhiHit();

    /// copy constructor
    PhiHit( const PhiHit& h_);

    /// =operator
    PhiHit& operator=( const PhiHit& h_ );

    int   layer;   /// layer identifier (packed word containing technology/sublayer)
    float r;       /// global hit position (x=r in barrel, x=z in endcap)
    float phimin;  /// minimum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
    float phimax;  /// maximum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
    float w;       /// weight of the hit

    /// access to debug information
    const HitDebugInfo* debugInfo() const { return m_debug; }
    HitDebugInfo* debugInfo() { return m_debug; }

    /// access to assiciated hit, either the prd or the tgc pointer is set in athena
    const Trk::PrepRawData* prd;
    const Muon::TgcClusterObj3D* tgc;

  private:
    HitDebugInfo* m_debug; /// pointer to debug information

    /// copy function for internal use
    void copy( const PhiHit& hit );
  };


  /// struct containing truth or track information
  struct MuonDebugInfo {
    int pdgId;      /// pdgId of the true muon
    int barcode;    /// barcode of the true muon
    int muonIndex;  /// index of the associated track
    float pt;       /// pt 
    float eta;      /// eta
    float phi;      /// phi
    
    // number of reconstructed hits
    int nmdts;      
    int nrpcs;
    int ntgcs;
    int ncscs;
    int nmms;
    int nstgcs;

    // number of true hits
    int ntmdts;
    int ntrpcs;
    int nttgcs;
    int ntcscs;
    int ntmms;
    int ntstgcs;
  };

  /// struct containing truth or segment information
  struct SegDebugInfo {
    float  sposx;
    float  sposy;
    float  sposz;
    float  sdirx;
    float  sdiry;
    float  sdirz;
    int    snPrecHits;
    int    snTrigHits;
    int    sSector;
    int    sChIndex;
  };

  /// struct to sort the hits
  struct SortHitsPerLayer : public std::binary_function<class Hit, class Hit, bool>  {
    bool operator()(const Hit& hit1, const Hit& hit2 ) const {
      return compare(hit1.ymin, hit2.ymin, hit1.layer, hit2.layer, hit1.debugInfo(), hit2.debugInfo() );  
    }

    bool operator()(const PhiHit& hit1, const PhiHit& hit2 ) const {
      return compare(hit1.phimin, hit2.phimin, hit1.layer, hit2.layer, hit1.debugInfo(), hit2.debugInfo() );  
    }

    bool compare( float val1, float val2, int lay1, int lay2, const HitDebugInfo* db1, const HitDebugInfo* db2 ) const {
      if( db1 && db2 ){
	if( db1->sector != db2->sector ) return db1->sector < db2->sector;
	if( db1->region != db2->region ) return db1->region < db2->region;
	if( db1->type != db2->type ) return db1->type < db2->type;
	if( db1->layer != db2->layer ) return db1->layer < db2->layer;
	if( db1->sublayer != db2->sublayer ) return db1->sublayer < db2->sublayer;
      }else{
	if( lay1 != lay2 ) return lay1 < lay2;
      }
      return CxxUtils::fpcompare::less (val1,val2); 
    }

    bool operator()(const Hit* hit1, const Hit* hit2 ) const {
      return operator()(*hit1,*hit2);
    }
    bool operator()(const PhiHit* hit1, const PhiHit* hit2 ) const {
      return operator()(*hit1,*hit2);
    }
  };

}

#endif
