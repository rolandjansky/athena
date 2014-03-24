/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEXTRAPOLATEMUONTOIPTOOL_H
#define IEXTRAPOLATEMUONTOIPTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"


static const InterfaceID IID_IExtrapolateMuonToIPTool("IExtrapolateMuonToIPTool", 1, 0);

class IExtrapolateMuonToIPTool : virtual public IAlgTool 
{      
 public:
  /** access to tool interface */
  static const InterfaceID& interfaceID() {return IID_IExtrapolateMuonToIPTool;}

  /** @brief extrapolate all tracks in the track collection to the IP
      @param   muonTracks the track collection with muon tracks to be extrapolated to the IP
      @return  TrackCollection with the tracks at the IP, ownership is passed to the caller
  */
  virtual TrackCollection* backExtrapolate(const TrackCollection& muonTracks) const = 0;

  /** @brief extrapolate a muon track the IP, will return 0 if the back extrapolation fails
      @param   muonTrack the moun inpu track
      @return  Track at the IP, ownership is passed to the caller, return zero if back extrapolation failed
  */
  virtual Trk::Track* backExtrapolate(const Trk::Track& muonTrack) const = 0; 
};

#endif // IEXTRAPOLATEMUONTOIPTOOL_H

