/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATSEGMENT_H
#define MUPATSEGMENT_H

#include "MuonTrackFindingEvent/MuPatCandidateBase.h"

#include "Identifier/Identifier.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "TrkParameters/TrackParameters.h"

#include <set>

namespace Muon {

  class MuPatTrack;

  /** 
      @brief segment candidate object. 
      The purpose of the segment candidate is three folded:
      - provide the generic MuPatCandidateBase interface for tracks
      - keep track of tracks the segment is accociated to
      - cache additional information that cannot be stored on the track 
      
      The following information is cached:
      - the segment quality integer calculated by the MuonSegmentSelectionTool
      - the MuonSegmentQuality
      - a AtaPlane that represents the segment and allows extrapolation
      - the index of the segment in the order list of segments in the track steering cache
      - an integer (usedInFit) showing on how many track candidates the segment is included
      - the identifier of the chamber with most hits on the segment
      - a string summarizing the segment that can be used for dumping it to screen
      - a chamber index
      - a station index
      - a flag indicating whether the segment is in the barrel or endcap
      - a flag indicating whether the segment contains MDT hits
   */

  class MuPatSegment : public MuPatCandidateBase {
  public:
    MuPatSegment();

    ~MuPatSegment();

    MuPatSegment( const MuPatSegment& info );

    MuPatSegment& operator=( const MuPatSegment& info );

    int                         quality;
    const MuonSegmentQuality*   segQuality;
    const MuonSegment*          segment;
    const Trk::AtaPlane*        segPars;
    int                         segmentIndex; //!< index of segment within station
    int                         usedInFit;
    Identifier                  chid;
    std::string                 name;
    MuonStationIndex::ChIndex   chIndex;
    MuonStationIndex::StIndex   stIndex;
    bool                        isEndcap;
    bool                        isMdt; //!< true for MDT, false for CSC

    //** @brief Mboy Specific */
    struct MboyInfo{

      int    m_ISC0    ;//! Mboy Nbering
      int    m_IWDRAT1 ;//! Mboy Nbering
      int    m_IWDRAT2 ;//! Mboy Nbering
      
      int    m_JFDRAT  ; //! Octant Number
  
      double m_FFDRAT  ; //! Phi   angle
      double m_GGDRAT  ; //! Gamma angle
      
      double m_EZDRAT  ; //! Error associated to CSC clusters

      double m_SSDRAT1 ; //! S in SZT system for the upper layer
      double m_ZZDRAT1 ; //! Z in SZT system for the upper layer
      double m_TTDRAT1 ; //! T in SZT system for the upper layer
  
      double m_SSDRAT2 ; //! S in SZT system for the lower layer
      double m_ZZDRAT2 ; //! Z in SZT system for the lower layer
      double m_TTDRAT2 ; //! T in SZT system for the lower layer
      
    }; 

    MboyInfo * mboyInfo;

    /** @brief returns first track parameters */
    const Trk::TrackParameters& entryPars() const;

    /** @brief reset the maximum number of objects counter */
    static void resetMaxNumberOfInstantiations();

    /** @brief maximum number of objects of this type in memory */
    static unsigned int maxNumberOfInstantiations();

    /** current number of objects of this type in memory */
    static unsigned int numberOfInstantiations();

    /** @brief reset the copy constructor counter */
    static void resetNumberOfCopies();

    /** @brief number of times the copy constructor was called since last reset */
    static unsigned int numberOfCopies();

    /** @brief add a new track to the segment */
    void addTrack( MuPatTrack* );

    /** @brief remove a track from the segment */
    void removeTrack( MuPatTrack* );

    /** access to the tracks the segment is associated with */
    const std::set<MuPatTrack*>& tracks() const { return m_associatedTracks; }

  private:

    /** @brief Keeping track of number of object instances */
    static void addInstance();

    /** @brief Keeping track of number of object instances */
    static void removeInstance();

    static unsigned int s_numberOfInstantiations;    //<! current number of objects of this type in memory
    static unsigned int s_maxNumberOfInstantiations; //<! maximum number of objects of this type in memory
    static unsigned int s_numberOfCopies;            //<! number of times the copy constructor was called since last reset

    std::set<MuPatTrack*> m_associatedTracks; //<! list of associated tracks

  };

  //
  // static inline functions implementations
  //
  inline unsigned int MuPatSegment::maxNumberOfInstantiations() {
    return s_maxNumberOfInstantiations;
  }

  inline unsigned int MuPatSegment::numberOfCopies() {
    return s_numberOfCopies;
  }

  inline void MuPatSegment::addInstance() {
    ++s_numberOfInstantiations;
    if ( s_numberOfInstantiations > s_maxNumberOfInstantiations ) {
      s_maxNumberOfInstantiations = s_numberOfInstantiations;
    }
  }

  inline void MuPatSegment::removeInstance() {
    if ( s_numberOfInstantiations > 0 ) --s_numberOfInstantiations;
  }

  //
  // inline member functions implementations
  //
  inline const Trk::TrackParameters& MuPatSegment::entryPars() const {
    return *segPars;
  }
  
  struct SortSegInfoByR {
    bool operator()( const MuPatSegment* c1, const MuPatSegment* c2 ){  
      return operator()(*c1,*c2);
    } 
    bool operator()( const MuPatSegment& c1, const MuPatSegment& c2 ){  
      return c1.segment->globalPosition().perp() < c2.segment->globalPosition().perp();
    } 
  };
  struct SortSegInfoByZ {
    bool operator()( const MuPatSegment* c1, const MuPatSegment* c2 ){  
      return operator()(*c1,*c2);
    } 
    bool operator()( const MuPatSegment& c1, const MuPatSegment& c2 ){  
      return fabs(c1.segment->globalPosition().z()) < fabs(c2.segment->globalPosition().z());
    } 
  };
  struct SortSegInfoByQuality {
    bool operator()( const MuPatSegment* c1, const MuPatSegment* c2 ){  
      return operator()(*c1,*c2);
    } 
    bool operator()( const MuPatSegment& c1, const MuPatSegment& c2 ){  
      return c1.quality > c2.quality;
    } 
  };

}

#endif
