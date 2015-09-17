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

typedef std::map< int, std::map< int, std::vector<const  xAOD::CaloCluster*> > > TopoCluster2DMap;

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
  virtual void       InsertTopoCluster(xAOD::CaloCluster*) = 0;
  virtual void       ClearMap() = 0;
  virtual            TopoCluster2DMap&     RetrieveMap() = 0;
  virtual            egammaTopoClusterMap& DuplicateMap() = 0;

  virtual std::vector<const  xAOD::CaloCluster*> RetrieveTopoClusters(double, double, double, double) = 0;
  virtual std::vector<const  xAOD::CaloCluster*> RetrieveTopoClusters(double, double, double)         = 0;
  virtual std::vector<const  xAOD::CaloCluster*> RetrieveTopoClusters(int, int)                       = 0;

  virtual void DumpMapContents() = 0;

};

inline const InterfaceID& IegammaTopoClusterMap::interfaceID()
{
  return IID_IegammaTopoClusterMap;
}
#endif

