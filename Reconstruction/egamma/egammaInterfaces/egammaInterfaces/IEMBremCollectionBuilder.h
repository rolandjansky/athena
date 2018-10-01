/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IBREMCOLLECTIONBUILDER_H
#define IBREMCOLLECTIONBUILDER_H

/**
   @class IEMBremCollectionBuilder
   Interface for the Reconstruction/egamma/egammaTools/EMBremCollectionBuilder

   CREATED: Sep 2011
   MODIFIED:
*/

#include "egammaInterfaces/IegammaBaseTool.h"

class egamma;

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrkTrack/TrackCollection.h"


static const InterfaceID IID_IEMBremCollectionBuilder("IEMBremCollectionBuilder",1,0);

class IEMBremCollectionBuilder : virtual public IAlgTool
{

 public:

  /**Virtual Destructor*/
  virtual ~IEMBremCollectionBuilder() {};
  /**AlgTool interface methods*/
  static const InterfaceID& interfaceID();
  /**Gaudi algorithm hooks*/
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;
  virtual StatusCode contExecute() = 0;
  virtual StatusCode hltExecute(const xAOD::CaloClusterContainer *clusters, const xAOD::TrackParticleContainer *inputTracks,
				TrackCollection *refitTracks, xAOD::TrackParticleContainer *refitTrackParticles) = 0;

};

inline const InterfaceID& IEMBremCollectionBuilder::interfaceID()
{
  return IID_IEMBremCollectionBuilder;
}

#endif
