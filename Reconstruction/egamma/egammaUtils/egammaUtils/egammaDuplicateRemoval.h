/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAUTILS_EGAMMADUPLICATEREMOVAL_H
#define EGAMMAUTILS_EGAMMADUPLICATEREMOVAL_H


/// @brief egammaDuplicateRemoval
/// @brief Remove duplicate using cluster 

/// @author  T. Cuhadar Donszelmann
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

/********************************************************************
NAME:     
PACKAGE:  offline/Reconstruction/egammaUtils
AUTHORS:  
CREATED:  Nov 2011
PURPOSE:   removal
UPDATED:
********************************************************************/

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <vector>


namespace egammaDuplicateRemoval {
  std::vector<const xAOD::CaloCluster *>  getClusterDuplicateRemoval(std::vector<const xAOD::CaloCluster*> input_Ptr, double DETA=0.05, double DPHI=0.1);
}

#endif
