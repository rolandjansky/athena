/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/EventContext.h"

// Forward declarations
class CaloCellContainer;
class CaloDetDescrManager;

static const InterfaceID IID_IEMShowerBuilder("IEMShowerBuilder", 1, 0);

class IEMShowerBuilder : virtual public IAlgTool
{
 public:

  /** @brief Virtual destructor*/
  virtual ~IEMShowerBuilder() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method with event context to retrieve the cells collection*/
  virtual StatusCode execute(const EventContext& ctx,
                             const CaloDetDescrManager& cmgr,
                             xAOD::Egamma*) const = 0;
  /** @Execute passing the cells explicitly*/
  virtual StatusCode executeWithCells(const CaloCellContainer* cellcoll,
                                      const CaloDetDescrManager& cmgr,
                                      xAOD::Egamma*) const = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& IEMShowerBuilder::interfaceID(){
  return IID_IEMShowerBuilder;
}

#endif









