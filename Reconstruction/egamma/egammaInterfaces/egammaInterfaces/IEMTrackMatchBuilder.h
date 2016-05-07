/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IEMTrackMatchBuilder.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEMTRACKMATCHBUILDER_H
#define EGAMMAINTERFACES_IEMTRACKMATCHBUILDER_H

/** @class IEMTrackMatchBuilder
  Interface for the Reconstruction/egamma/egammaTools/EMTrackMatchBuilder

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 01/09/2008
MODIFIED :
*/

// INCLUDE HEADER FILES:
#include "egammaInterfaces/IegammaBaseTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h" 

class egammaRec;
static const InterfaceID IID_IEMTrackMatchBuilder("IEMTrackMatchBuilder", 1, 0);

class IEMTrackMatchBuilder : virtual public IegammaBaseTool
{

 public:

  /** @brief Virtual destructor*/
  virtual ~IEMTrackMatchBuilder() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();  
  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method*/
  virtual StatusCode executeRec(egammaRec* eg) = 0;
  /** @brief execute method*/
  virtual StatusCode trackExecute(egammaRec* eg,  const xAOD::TrackParticleContainer * trackPC) = 0;
};

inline const InterfaceID& IEMTrackMatchBuilder::interfaceID()
{
  return IID_IEMTrackMatchBuilder;
}

#endif









