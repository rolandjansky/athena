/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/clusterHasCells.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2012
 * @brief Helper to test if a cluster has cells available.
 */


#include "clusterHasCells.h"


namespace D3PD {


bool clusterHasCells (const xAOD::CaloCluster* cl)
{
  if (!cl) return false;
  if (!cl->getCellLinks()) return false;
  if (!cl->getCellLinks()->getCellContainer()) return false;
  if (!cl->size()) return false;
  return true;
}


} // namespace D3PD
