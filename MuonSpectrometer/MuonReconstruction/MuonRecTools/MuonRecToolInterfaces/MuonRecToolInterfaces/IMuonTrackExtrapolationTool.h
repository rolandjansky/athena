/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONTRACKEXTRAPOLATIONTOOL_H
#define IMUONTRACKEXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"

static const InterfaceID IID_IMuonTrackExtrapolationTool("Muon::IMuonTrackExtrapolationTool",1,0);

namespace Muon {
  
  /** Interface class for tools extrapolating muon tracks to a location in the atlas detector.
      The destination is defined in the concrete tool implementation. 
  */
  class IMuonTrackExtrapolationTool : virtual public IAlgTool 
  {      
    public:
      static const InterfaceID& interfaceID();

      /** @brief extrapolates a muon track to a destination and returns a new track expressed at the destination.
	  @param track input track
	  @return a pointer to the extrapolated track, zero if extrapolation failed.
	   	  The ownership of the track is passed to the client calling the tool.
      */
      virtual Trk::Track* extrapolate( const Trk::Track& track ) const = 0;

      /** @brief extrapolates a muon track collection to a destination and returns a new track expressed at the destination.
	  @param tracks input track collection
	  @return a pointer to the extrapolated track collection, zero if extrapolation failed.
	   	  The ownership of the collection is passed to the client calling the tool.
      */
      virtual TrackCollection* extrapolate( const TrackCollection& tracks ) const = 0;
  };
  
  inline const InterfaceID& IMuonTrackExtrapolationTool::interfaceID()
  {
    return IID_IMuonTrackExtrapolationTool;
  }
} // end of name space

#endif // IMuonTrackExtrapolationTool_H

