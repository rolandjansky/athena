// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRACKINGCNV_ITRACKCOLLECTIONCNVTOOL_H
#define XAODTRACKINGCNV_ITRACKCOLLECTIONCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"
// Forward declaration(s):
//class TrackCollection; - no - typedef
#include "TrkValInterfaces/ITrkObserverTool.h"

namespace Trk {
  class ITrackParticleCreatorTool;
}

namespace xAODMaker {

  /// The interface provided by ITrackCollectionCnvTool
  static const InterfaceID
  IID_ITrackCollectionCnvTool( "xAODMaker::ITrackCollectionCnvTool", 1, 0 );
  

  class ITrackCollectionCnvTool : public virtual IAlgTool {
    
  public:
    /// Function that fills an existing xAOD::TrackParticleContainer
    virtual StatusCode convert( const TrackCollection* aod,
				xAOD::TrackParticleContainer* xaod ) const = 0;

    /// Function that fills an existing xAOD::TrackParticleContainer and augments track particles
    virtual StatusCode convertAndAugment( const TrackCollection* aod,
				xAOD::TrackParticleContainer* xaod, const ObservedTrackMap* trk_map ) const = 0;

    virtual StatusCode setParticleCreatorTool(ToolHandle<Trk::ITrackParticleCreatorTool> *tool) = 0;
    
    /// Gaudi interface definition
    static const InterfaceID& interfaceID() {
      return IID_ITrackCollectionCnvTool;
    }

  };//class definition
    
} // xAODMaker namespace


#endif // XAODTRACKINGCNV_ITRACKCOLLECTIONCNVTOOL_H
