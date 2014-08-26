/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertSeedFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////
#ifndef INDETRECTOOLINTERFACES_IMULTISEEDFINDER_H
#define INDETRECTOOLINTERFACES_IMULTISEEDFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/NeutralParticle.h"

/**
 * An abstract base class for implementation of
 * primary vertex seed finders. Concrete implementations
 * scan provided tracks and form track clusters in z0.
 * 
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 */

  
namespace Trk
{
 class Vertex;
 class Track;
 class TrackParticleBase;
}  
  
namespace InDet
{
 static const InterfaceID IID_IMultiPVSeedFinder("IMultiPVSeedFinder", 1, 1);

 class IMultiPVSeedFinder : virtual public IAlgTool 
 {
   public:
      
/**
 * Virtual destructor 
 */
    virtual ~IMultiPVSeedFinder() {}

/** 
 * AlgTool interface methods 
 */
    static const InterfaceID& interfaceID() { return IID_IMultiPVSeedFinder; };
    
/**
 *  Finds track clusters of tracks corresponding to
 *  priamry interaction vertices.  A beam spot information
 *  can be used optionally to reject secondary tracks.
 */
    virtual std::vector< std::vector<const Trk::Track *> > seeds(const std::vector<const Trk::Track*>& tracks )const = 0;

    virtual std::vector< std::vector<const Trk::TrackParticleBase *> > seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const = 0;

    //virtual std::vector< std::vector<const Trk::TrackParameters *> > seeds(const std::vector<const Trk::TrackParameters*>& tracks )const = 0;
    
    virtual std::vector< std::vector<const Trk::TrackParameters *> > seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const = 0;

 };//end of class definitions
} //end of namespace definitions

#endif
