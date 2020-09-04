/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTENDEDSEGMENT_H
#define MUONCALIBEXTENDEDSEGMENT_H

#include "MuonCalibEventBase/MuonCalibSegment.h"

#include "MuonCalibExtraTreeEvent/MuonCalibTrackSummary.h"
#include "MuonCalibExtraUtils/MuonCalibLayerMapping.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibRawHitAssociationMap.h"
#include "CxxUtils/checker_macros.h"

#include <vector>
#include <map>
#include <string>

namespace MuonCalib {
  
  class MuonCalibExtendedTrack;
  
  /**
     @class MuonCalibExtendedSegment

     A segment plus everything one can dream of knowing about it.

     @author niels.van.eldik@cern.ch
  */
  class ATLAS_NOT_THREAD_SAFE MuonCalibExtendedSegment : public MuonCalibSegment {
  public:
    /**  Constructor taking input track. */
    MuonCalibExtendedSegment( const MuonCalibSegment& segment );

    /** access to hit counts */
    const MuonCalibTrackSummary& summary() const { return m_summary; }

    /** access hit identifier map (hits give the MuonFixedId */
    const IdSet& idSet() const { return m_idSet; }

    /** dump all information to string */
    std::string dump() const;

    /** dump track parameters to string */
    std::string dumpPars() const;

    /** dump track summary to string */
    std::string dumpSummary() const;

    /** access to list of tracks the segment was associated to */
    const std::vector<const MuonCalibExtendedTrack*>& associatedTracks() const { return m_associatedTracks; }

    /** add associated track */
    void addTrack( const MuonCalibExtendedTrack* track ) const { m_associatedTracks.push_back(track); }

    /** access raw hit assocation map */ 
    const MuonCalibRawHitAssociationMap& rawHitAssociationMap() const { return m_rawHitAssociationMap; }

  private:

    /** track summary */
    MuonCalibTrackSummary m_summary;

    /** set with Identifiers on segment */
    IdSet m_idSet;

    /** list of tracks to which the segment was associated */
    mutable std::vector<const MuonCalibExtendedTrack*> m_associatedTracks;

    /** raw hit association map */
    MuonCalibRawHitAssociationMap m_rawHitAssociationMap;

  };

}


#endif
