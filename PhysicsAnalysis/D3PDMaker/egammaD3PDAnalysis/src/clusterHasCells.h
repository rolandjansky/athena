// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/clusterHasCells.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2012
 * @brief Helper to test if a cluster has cells available.
 */


#ifndef EGAMMAD3PDANALYSIS_CLUSTERHASCELLS_H
#define EGAMMAD3PDANALYSIS_CLUSTERHASCELLS_H


#include "xAODCaloEvent/CaloCluster.h"


namespace D3PD {


bool clusterHasCells (const xAOD::CaloCluster* cl);


}


#endif // not EGAMMAD3PDANALYSIS_CLUSTERHASCELLS_H
