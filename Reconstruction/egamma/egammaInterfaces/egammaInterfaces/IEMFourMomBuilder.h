/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMFOURMOMBUILDER_H
#define EGAMMAINTERFACES_IEMFOURMOMBUILDER_H

/** @class IEMFourMomBuilder
  Interface for the Reconstruction/egamma/egammaTools/EMFourMomBuilder

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 01/09/2008
MODIFIED : May,2009 (remove an argument from fourMomExecute)
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
// Forward declarations
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h" 

// INCLUDE HEADER FILES:
#include "egammaInterfaces/IegammaBaseTool.h"

// Forward declarations
//class egamma;
//class egDetail;
//class egDetailContainer;

static const InterfaceID IID_IEMFourMomBuilder("IEMFourMomBuilder", 1, 0);

class IEMFourMomBuilder : virtual public IegammaBaseTool
{

 public:

  /** @brief Virtual destructor*/
  virtual ~IEMFourMomBuilder() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method */
  virtual StatusCode execute(const EventContext& ctx, xAOD::Egamma* eg) const = 0;
};

inline const InterfaceID& IEMFourMomBuilder::interfaceID()
{
  return IID_IEMFourMomBuilder;
}

#endif









