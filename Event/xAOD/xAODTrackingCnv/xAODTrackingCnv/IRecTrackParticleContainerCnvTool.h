// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRACKINGCNV_IRECTRACKPARTICLECONTAINERCNVTOOL_H
#define XAODTRACKINGCNV_IRECTRACKPARTICLECONTAINERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
//#include "TrkTrack/TrackCollection.h"
#include "TrkValInterfaces/ITrkObserverTool.h"

// Forward declaration(s):
namespace Rec {
class TrackParticleContainer;
}

namespace Trk {
  class ITrackParticleCreatorTool;
}


namespace xAODMaker {

  /// The interface provided by IRecTrackParticleContainerCnvTool
  static const InterfaceID
  IID_IRecTrackParticleContainerCnvTool( "xAODMaker::IRecTrackParticleContainerCnvTool", 1, 0 );
  

  class IRecTrackParticleContainerCnvTool : public virtual IAlgTool {
    
  public:
    /// Function that fills an existing xAOD::TrackParticleContainer
    virtual StatusCode convert( const Rec::TrackParticleContainer* aod,
				xAOD::TrackParticleContainer* xaod ) const = 0;

    /// Function that fills an existing xAOD::TrackParticleContainer and augments track particles
    virtual StatusCode convertAndAugment( const Rec::TrackParticleContainer* aod,
				xAOD::TrackParticleContainer* xaod, const ObservedTrackMap* trk_map ) const = 0;

    virtual StatusCode setParticleCreatorTool(ToolHandle<Trk::ITrackParticleCreatorTool> *tool) = 0;
    
    /// Gaudi interface definition
    static const InterfaceID& interfaceID() {
      return IID_IRecTrackParticleContainerCnvTool;
    }

  };//class definition
    
} // xAODMaker namespace


#endif // XAODTRACKINGCNV_IRECTRACKPARTICLECONTAINERCNVTOOL_H
