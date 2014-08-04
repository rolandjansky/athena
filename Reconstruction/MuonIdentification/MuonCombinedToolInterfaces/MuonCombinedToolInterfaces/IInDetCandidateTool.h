/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IInDetCandidateTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONCOMBINED_IINDETCANDIDATETOOL_H
#define MUONCOMBINED_IINDETCANDIDATETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

namespace MuonCombined {

  static const InterfaceID IID_IInDetCandidateTool("MuonCombined::IInDetCandidateTool", 1, 0);

  /** @class IInDetCandidateTool
      @brief interface for tools building InDetCandidateCollection from a TrackParticleContainer of ID tracks
 
      @author Niels van Eldik
   */

  class IInDetCandidateTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IInDetCandidateTool interface: build an InDetCandidateCollection from a InDet TrackParticleContainer */
    virtual void create( const xAOD::TrackParticleContainer& indetTrackParticles, InDetCandidateCollection& outputContainer ) const = 0;

  };

  inline const InterfaceID& IInDetCandidateTool::interfaceID() { 
    return IID_IInDetCandidateTool; 
  }

} // end of namespace

#endif 
