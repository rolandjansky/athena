/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSPRIVTXFINDERINTERFACES_IACTSPRIVTXFINDERTOOL_H
#define ACTSPRIVTXFINDERINTERFACES_IACTSPRIVTXFINDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
//#include "ActsGeometry/ActsGeometryContext.h"


#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"

class IActsPriVtxFinderTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IActsPriVtxFinderTool, 1, 0);

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  	 findVertex(const TrackCollection* trackTES, const EventContext& ctx) const = 0;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  	 findVertex(const Trk::TrackParticleBaseCollection* trackTES, const EventContext& ctx) const = 0;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  	 findVertex(const xAOD::TrackParticleContainer* trackParticles, const EventContext& ctx) const = 0;
  
};

#endif
