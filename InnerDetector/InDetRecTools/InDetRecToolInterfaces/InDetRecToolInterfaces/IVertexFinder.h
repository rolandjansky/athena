/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          IVertexFinder.h  -  Description
                             -------------------
    begin   : 14-02-2006
    authors : Andreas Wildauer (CERN PH-ATC)
            : Giacinto Piacquadio (Freiburg University), Kirill Prokofiev
 (Sheffield University) email   : andreas.wildauer@cern.ch
              giacinto.piacquadio@physik.uni-freiburg.de
    changes : David Shope <david.richard.shope@cern.ch> (2016-04-26)

                EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex

                findVertex will now always return an xAOD::VertexContainer,
                even when using a TrackCollection or a
 TrackParticleBaseCollection as input.

            : Christos Anastopoulos Athena MT
 ***************************************************************************/

/** Vertex Finder Tool.

    Abstract class which provides the possibility to implement more than one
    vertex finder tool (every vertex finder tool inherits from this class).

    It was implemented to create a new primary vertex finding algorithm,
    different from the one already used by InDetPriVxFinderTool (by AW and FK),
    the InDetAdaptivePriVxFinderTool...
*/

#ifndef InDetRecToolInterfaces_IVertexFinder_H
#define InDetRecToolInterfaces_IVertexFinder_H
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkTrack/TrackCollection.h"

// xAOD includes
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexFwd.h"

namespace InDet {
static const InterfaceID IID_IVertexFinder("IVertexFinder", 1, 0);
class IVertexFinder : virtual public IAlgTool
{
public:
  virtual ~IVertexFinder(){};
  static const InterfaceID& interfaceID() { return IID_IVertexFinder; }

  /*
   * For MT we have 2 sets , one with EventContext and one without
   * Implementation really need to overload only one
   * The clients can call either
   */

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const EventContext& ctx, const TrackCollection* trackTES) const
  {

    (void)(ctx); // We do not use ctx
    return findVertex(trackTES);
  }

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const EventContext& ctx,
             const Trk::TrackParticleBaseCollection* trackTES) const
  {
    (void)(ctx); // We do not use ctx
    return findVertex(trackTES);
  }

  /** Find vertex from xAOD::TrackParticleContainer.
   * @param EventContext
   * @param trackParticles input track container
   * @return a pair of newly created container and auxiliary store
   */
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const EventContext& ctx,
             const xAOD::TrackParticleContainer* trackParticles) const
  {
    (void)(ctx); // We do not use ctx
    return findVertex(trackParticles);
  }

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const TrackCollection* trackTES) const
  {
    return findVertex(Gaudi::Hive::currentContext(), trackTES);
  }

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const Trk::TrackParticleBaseCollection* trackTES) const
  {
    return findVertex(Gaudi::Hive::currentContext(), trackTES);
  }
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const xAOD::TrackParticleContainer* trackParticles) const
  {
    return findVertex(Gaudi::Hive::currentContext(), trackParticles);
  }
};
}
#endif
