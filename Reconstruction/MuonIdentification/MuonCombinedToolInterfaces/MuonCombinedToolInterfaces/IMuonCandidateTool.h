/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCandidateTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONCOMBINED_IMUONCANDIDATETOOL_H
#define MUONCOMBINED_IMUONCANDIDATETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"

namespace MuonCombined {

  static const InterfaceID IID_IMuonCandidateTool("MuonCombined::IMuonCandidateTool", 1, 0);

  /** @class IMuonCandidateTool
      @brief interface for tools building MuonCandidateCollection from a TrackParticleCollection of spectrometer tracks
 
      @author Niels van Eldik
   */

  class IMuonCandidateTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IMuonCandidateTool interface: build a MuonCandidateCollection from a TrackCollection of spectrometer tracks */
    virtual void create( const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection, TrackCollection& outputTracks ) = 0;

  virtual void cleanUp() const = 0; 

  };

  inline const InterfaceID& IMuonCandidateTool::interfaceID() { 
    return IID_IMuonCandidateTool; 
  }

} // end of namespace

#endif 
