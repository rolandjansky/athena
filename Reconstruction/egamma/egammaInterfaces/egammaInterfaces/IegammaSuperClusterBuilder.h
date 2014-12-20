/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMASUPERCLUSTERBUILDER_H
#define EGAMMAINTERFACES_IEGAMMASUPERCLUSTERBUILDER_H

/** @class IEGAMMASUPERCLUSTERTRACKMATCH
  Interface for the tool Reconstruction/egamma/egammaTools/egammaSuperClusterTrackMatch

  @author David Di Valentino david.di.valentino@cern.ch

CREATED  : 21/12/2013
MODIFIED : 
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IAlgTool.h"
#include <vector>

// Forward declarations.
//class egamma;
//class CaloCluster;

//class CaloClusterContainer;

/* namespace Rec */
/* { */
/*   class TrackParticleContainer; */
/*   class TrackParticle; */
/* } */

// Forward declarations.

class SuperCaloCluster;
class egammaTopoClusterMap;

static const InterfaceID IID_IegammaSuperClusterBuilder("IegammaSuperClusterBuilder", 1, 0);

class IegammaSuperClusterBuilder : virtual public IAlgTool
{

 public:

  /** @brief Virtual destructor*/
  virtual ~IegammaSuperClusterBuilder() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize()   = 0;
  virtual StatusCode execute()    = 0;

};

inline const InterfaceID& IegammaSuperClusterBuilder::interfaceID()
{
  return IID_IegammaSuperClusterBuilder;
}

#endif
