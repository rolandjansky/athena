/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMAINTERFACES_IEGAMMACALOCLUSTERSELECTOR_H
#define EGAMMAINTERFACES_IEGAMMACALOCLUSTERSELECTOR_H

#include "GaudiKernel/IAlgTool.h"

#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloDetDescrManager;
/**
 * @class IegammaCaloClusterSelector
 * @brief used to select clusters for seeding
 **/
class IegammaCaloClusterSelector : virtual public IAlgTool {
public: 
  DeclareInterfaceID(IegammaCaloClusterSelector, 1, 0);

  virtual bool passSelection(const xAOD::CaloCluster* cluster,const CaloDetDescrManager& cmgr) const = 0;

  virtual ~IegammaCaloClusterSelector() override {}
}; 

#endif // RECONSTRUCTION/EGAMMA/EGAMMAINTERFACES_IEGAMMACALOCLUSTERSELECTOR_H
