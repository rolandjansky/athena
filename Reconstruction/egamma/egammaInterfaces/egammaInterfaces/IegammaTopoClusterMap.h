/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMATOPOCLUSTERMAP_H
#define EGAMMAINTERFACES_IEGAMMATOPOCLUSTERMAP_H

/** @class IEGAMMATOPOCLUSTERMAP
  Interface for the tool Reconstruction/egamma/egammaTools/egammaTopoClusterMap
  @author David Di Valentino david.di.valentino@cern.ch

CREATED  : 21/12/2013
MODIFIED : 
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IAlgTool.h"
// Forward declarations.
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class egammaTopoClusterMap;

static const InterfaceID IID_IegammaTopoClusterMap("IegammaTopoClusterMap", 1, 0);

class IegammaTopoClusterMap : virtual public IAlgTool
{

 public:

  /** @brief Virtual destructor*/
  virtual ~IegammaTopoClusterMap() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize()   = 0;
  virtual StatusCode execute(const  xAOD::CaloClusterContainer*) = 0;

  virtual std::vector<const  xAOD::CaloCluster*> RetrieveTopoClusters(double, double, double, double) const = 0;
  virtual std::vector<const  xAOD::CaloCluster*> RetrieveTopoClusters(double, double, double)      const   = 0;
  virtual std::vector<const  xAOD::CaloCluster*> RetrieveTopoClusters(int, int)                    const   = 0;

};

inline const InterfaceID& IegammaTopoClusterMap::interfaceID()
{
  return IID_IegammaTopoClusterMap;
}
#endif

