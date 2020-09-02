/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTENDEDTRACK_H
#define MUONCALIBEXTENDEDTRACK_H

#include "MuonCalibExtraUtils/MuonCalibLayerMapping.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrackSummary.h"
#include "MuonCalibExtraTreeEvent/MuonCalibRawHitAssociationMap.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrackOverlap.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <string>
#include <algorithm>

namespace MuonCalib {
  
  class MuonCalibHit_E;
  class MuonCalibExtendedSegment;

  /**
     @class MuonCalibExtendedTrack

     A track plus everything one can dream of knowing about a track.

     @author niels.van.eldik@cern.ch
  */
  class MuonCalibExtendedTrack : public MuonCalibTrack_E {
  public:
    /**  Constructor taking input track. */
    MuonCalibExtendedTrack( const MuonCalibTrack_E& track, int pdgCode = 0, int barCode = 0 );

    /** position of perigee of track */
    const Amg::Vector3D& position() const { return m_pos; }

    /** direction of perigee of track */
    const Amg::Vector3D& direction() const { return m_dir; }

    /** returns trackparameter d0 at IP */
    float d0ip() const;                               

    /** returns trackparameter z0 at IP */
    float z0ip() const;

    /** returns momentum at IP */
    float pip() const;

    /** returns pdgCode */
    int pdgCode() const;

    /** returns barCode */
    int barCode() const;

    /** access to hit counts */
    const MuonCalibTrackSummary& summary() const { return m_summary; }

    /** access to hits per station layer index (mdt) */
    const StationIndexHitsMap& mdtStationIndexHitsMap() const { return m_mdtHitsPerStationIndex; }

    /** access to hits per station layer index (rpc) */
    const StationIndexHitsMap& rpcStationIndexHitsMap() const { return m_rpcHitsPerStationIndex; }

    /** access to hits per station layer index (tgc) */
    const StationIndexHitsMap& tgcStationIndexHitsMap() const { return m_tgcHitsPerStationIndex; }

    /** access to hits per station layer index (csc) */
    const StationIndexHitsMap& cscStationIndexHitsMap() const { return m_cscHitsPerStationIndex; }

    /** dump all information to string */
    std::string dump() const;

    /** dump track parameters to string */
    std::string dumpPars() const;

    /** dump track summary to string */
    std::string dumpSummary() const;

    /** dump track intersects to string */
    std::string dumpIntersects() const;

    /** access to the list of intersected station layers */
    const StationIntersectedLayerMap& intersectedStationLayer() const;

    /** access station layers intersected by track without hits */
    const StationIndexSet& intersectedLayersWithoutHits() const { return m_intersectedLayersWithoutHits; }

    /** access rpc chambers intersected by track without hits */
    const StationIndexSet& intersectedRpcLayersWithoutHits() const { return m_intersectedRpcLayerWithoutHits; }

    /** access tgc chamber layers intersected by track without hits */
    const StationIndexSet& intersectedTgcLayersWithoutHits() const { return m_intersectedTgcLayerWithoutHits; }

    /** access hit identifier map (hits give the MuonFixedId */
    const IdHitMap& idHitMap() const { return m_hitIdMap; }

    /** hit information per station */
    const IdHitsMap& hitsPerChamberMap() const { return m_hitsPerChamber; }

    /** access to list of the segment that are associated to this track */
    const std::vector<const MuonCalibExtendedSegment*>& associatedSegments() const { return m_associatedSegments; }
    
    /** access to list of the tracks that are associated to this track */
    const std::vector<const MuonCalibExtendedTrack*>& associatedTracks() const { return  m_associatedTracks; }

    /** add associated segment */
    void addSegment( const MuonCalibExtendedSegment* seg ) const { m_associatedSegments.push_back(seg); }

    /** add associated track */
    void addAssociatedTrack( const MuonCalibExtendedTrack* track ) const { m_associatedTracks.push_back(track); }

    /** check whether the given segment is associated to this one (pointer based) */
    bool isAssociated( const MuonCalibExtendedSegment* segment ) const;

    /** check whether the given track is associated to this one (pointer based) */
    bool isAssociated( const MuonCalibExtendedTrack* track ) const;

    /** access raw hit assocation map */ 
    const MuonCalibRawHitAssociationMap& rawHitAssociationMap() const { return m_rawHitAssociationMap; }
 
    /** calculate hit overlap between two tracks */
    MuonCalibExtendedTrackOverlap calculateHitOverlap( const MuonCalibExtendedTrack& track ) const;

    /** get associated track for the give author, returns zero if not found */
    const MuonCalibExtendedTrack* getAssociatedTrack( int author ) const;

    /** check whether the track is confirmed by an ID track */
    bool isIDConfirmed() const;

    int m_pdgCode;
    int m_barCode;

  private:
    /** position of perigee parameters */
    Amg::Vector3D  m_pos;

    /** direction of perigee parameters */
    Amg::Vector3D m_dir;

    /** direction of perigee parameters at IP */
    Amg::Vector3D m_dirip;

    /** track summary */
    MuonCalibTrackSummary m_summary;

    /** hit information per station layer index */
    StationIndexHitsMap m_mdtHitsPerStationIndex;
    StationIndexHitsMap m_rpcHitsPerStationIndex;
    StationIndexHitsMap m_tgcHitsPerStationIndex;
    StationIndexHitsMap m_cscHitsPerStationIndex;

    /** hit information per station */
    IdHitsMap m_hitsPerChamber;

    /** hit information per station */
    IdHitMap m_hitIdMap;

    /** map with all station layers intersected by track */
    StationIntersectedLayerMap m_intersectedLayers;

    /** set with all station layers intersected by the track without hits */
    StationIndexSet m_intersectedLayersWithoutHits;

    /** set with all rpc chamber identifiers of layers intersected by the track without hits */
    StationIndexSet m_intersectedRpcLayerWithoutHits;

    /** set with all tgc chamber identifiers of layers intersected by the track without hits */
    StationIndexSet m_intersectedTgcLayerWithoutHits;

    /** list of segments associated with this track */
    mutable std::vector<const MuonCalibExtendedSegment*> m_associatedSegments; 

    /** list of tracks associated with this track */
    mutable std::vector<const MuonCalibExtendedTrack*> m_associatedTracks; 

    /** raw hit association map */
    MuonCalibRawHitAssociationMap m_rawHitAssociationMap;
  };


  inline bool MuonCalibExtendedTrack::isAssociated( const MuonCalibExtendedSegment* segment ) const {
    std::vector<const MuonCalibExtendedSegment*>::const_iterator pos = std::find(m_associatedSegments.begin(),m_associatedSegments.end(),segment);
    if( pos != m_associatedSegments.end() ) return true;
    return false;
  }

  inline bool MuonCalibExtendedTrack::isAssociated( const MuonCalibExtendedTrack* track ) const {
    std::vector<const MuonCalibExtendedTrack*>::const_iterator pos = std::find(m_associatedTracks.begin(),m_associatedTracks.end(),track);
    if( pos != m_associatedTracks.end() ) return true;
    return false;
  }
  struct FindAuthor {
    FindAuthor(int author) : m_author(author) {}
    bool operator()(const MuonCalibExtendedTrack* track) const {
      if( track->author() == m_author ) return true;
      return false;
    }
    int m_author;
  };
  struct SameFixedId {
    SameFixedId(MuonFixedId id) : m_id(id) {}
    bool operator()(const MuonCalibHit_E* hit) const {
      if( hit->identify() == m_id ) return true;
      return false;
    }
    MuonFixedId m_id;
  };

  inline const MuonCalibExtendedTrack* MuonCalibExtendedTrack::getAssociatedTrack( int author ) const {
    std::vector<const MuonCalibExtendedTrack*>::const_iterator pos = std::find_if(m_associatedTracks.begin(),m_associatedTracks.end(),FindAuthor(author));
    if( pos != m_associatedTracks.end() ) return *pos;
    return 0;    
  }
  inline float MuonCalibExtendedTrack::d0ip() const {
    if( author() == 0 ){
      const MuonCalibExtendedTrack* ipTrack = getAssociatedTrack(10);
      if( ipTrack ) {
        return ipTrack->d0();
      } else {
        ipTrack = getAssociatedTrack(1);
        if( ipTrack ) return ipTrack->d0();
      }
    }
    return MuonCalibTrack_E::d0();
  }

  inline float MuonCalibExtendedTrack::z0ip() const {
    if( author() == 0 ){
      const MuonCalibExtendedTrack* ipTrack = getAssociatedTrack(10);
      if( ipTrack ) {
        return ipTrack->z0();
      } else { 
        ipTrack = getAssociatedTrack(1);
        if( ipTrack ) return ipTrack->z0();
      }
    }
    return MuonCalibTrack_E::z0();
  }

  inline float MuonCalibExtendedTrack::pip() const {
    if( author() == 0 ){
      const MuonCalibExtendedTrack* ipTrack = getAssociatedTrack(10);
      if( ipTrack ) {
        return ipTrack->p();
      } else { 
        ipTrack = getAssociatedTrack(1);
        if( ipTrack ) return ipTrack->p();
      }
    }
    return MuonCalibTrack_E::p();
  }

  inline int MuonCalibExtendedTrack::pdgCode() const { return m_pdgCode;}

  inline int MuonCalibExtendedTrack::barCode() const { return m_barCode;}


  inline bool MuonCalibExtendedTrack::isIDConfirmed() const {
    std::set<int> idAuthors;
    idAuthors.insert(20);
    idAuthors.insert(25);
    idAuthors.insert(30);
    idAuthors.insert(35);
    idAuthors.insert(40);
    idAuthors.insert(41);
    idAuthors.insert(42);

    std::vector<const MuonCalibExtendedTrack*>::const_iterator it = m_associatedTracks.begin();
    std::vector<const MuonCalibExtendedTrack*>::const_iterator it_end = m_associatedTracks.end();
    for( ;it!=it_end;++it ){
      if( idAuthors.count((*it)->author()) ) return true;
      if( (*it)->author() == 140 ) {
        const MuonCalibExtendedTrack* mbTrack = (*it)->getAssociatedTrack(100);
        if( mbTrack ){
          if( mbTrack->ndof() != (*it)->ndof() ) return true;
          else {
            MsgStream log(Athena::getMessageSvc(),"MuonCalibExtendedTrack");
            log<<MSG::WARNING<<"STACO track without IDn"<<endmsg;
          }
        }
      }
    }
    return false;
  }

}


#endif
