/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMSHOWERBUILDER_H
#define EGAMMAINTERFACES_IEMSHOWERBUILDER_H

/** @class IEMShowerBuilder
  Interface for the Reconstruction/egamma/egammaRec/EMShowerBuilder

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 01/09/2008
MODIFIED :
*/

// INCLUDE HEADER FILES:
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h" 
#include "xAODTracking/VertexContainerFwd.h" 

// Forward declarations
class CaloCellContainer;

static const InterfaceID IID_IEMShowerBuilder("IEMShowerBuilder", 1, 0);

class IEMShowerBuilder : virtual public IegammaBaseTool
{
 public:

  /** @brief Virtual destructor*/
  virtual ~IEMShowerBuilder() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method*/
  virtual StatusCode execute(xAOD::Egamma*) = 0;
  /** @brief execute method*/
  virtual  StatusCode recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll) = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& IEMShowerBuilder::interfaceID(){
  return IID_IEMShowerBuilder;
}

#endif









