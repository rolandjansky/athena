/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRACKCLEANER_H
#define MUON_IMUONTRACKCLEANER_H
 
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonTrackCleaner("Muon::IMuonTrackCleaner",1,0);

namespace Trk {
  class Track;
}

namespace Muon {

  /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */  
  class IMuonTrackCleaner : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    /** @brief clean a track, returns a pointer to a new track if successfull.
	If the input track is does not require cleaning a pointer the the initial track is return in which case the 
	user should not delete the old track!
	The caller should ensure the track gets deleted. */
    virtual Trk::Track* clean( Trk::Track& track ) const = 0;
    
    /** @brief clean a track, returns a pointer to a new track if successfull.
	If the input track is does not require cleaning a pointer the the initial track is return in which case the 
	user should not delete the old track! The cleaning will not clean if all the chambers in the exclusions list 
	are marked as to be deleted.
	The caller should ensure the track gets deleted. */
    virtual Trk::Track* clean( Trk::Track& track, const std::set<Identifier>& chamberRemovalExclusionList ) const = 0;

  };
  
  inline const InterfaceID& IMuonTrackCleaner::interfaceID()
  {
    return IID_IMuonTrackCleaner;
  }

} // end of name space

#endif 

