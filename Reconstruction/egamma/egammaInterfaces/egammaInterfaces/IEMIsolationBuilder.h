/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMISOLATIONBUILDER_H
#define EGAMMAINTERFACES_IEMISOLATIONBUILDER_H

/** @class IEMIsolationBuilder
  Interface for the Reconstruction/egamma/egammaTools/EMIsolationBuilder

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 06/10/2014
MODIFIED :
*/

// INCLUDE HEADER FILES:
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

// Forward declarations
class CaloCellContainer;

static const InterfaceID IID_IEMIsolationBuilder("IEMIsolationBuilder", 1, 0);

class IEMIsolationBuilder : virtual public IegammaBaseTool
{
 public:

  /** @brief Virtual destructor*/
  virtual ~IEMIsolationBuilder() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method*/
  virtual StatusCode execute(xAOD::Egamma*) = 0;
  /** @brief execute method*/
  virtual  StatusCode recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll,
				  const xAOD::TrackParticleContainer* aTrackParticleContainer = 0) = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& IEMIsolationBuilder::interfaceID(){
  return IID_IEMIsolationBuilder;
}

#endif









