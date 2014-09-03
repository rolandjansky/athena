/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOCLUSTERCORRECTION_CALOCOMPUTESWCELLWEIGHTS_H
#define CALOCLUSTERCORRECTION_CALOCOMPUTESWCELLWEIGHTS_H
/********************************************************************

NAME:     CaloComputeSWcellWeights.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  D. Lelas
CREATED:  Dec. 02, 2005

PURPOSE:  - Compute simple cell weights for Sliding Window
          clusters, to solve double-counting of cells' energies
          for cells shared by two or more clusters.

          This tool does not update the cluster kinematics,
          only the cell weights.  It should be followed by
          a pass of CaloFillRectangularCluster with fill_cluster
          set to False.

********************************************************************/

// INCLUDE HEADER FILES:
#include "CaloRec/CaloClusterCollectionProcessor.h"

class CaloComputeSWcellWeights : public AthAlgTool, virtual public CaloClusterCollectionProcessor
{

 public:

  // Constructor
  CaloComputeSWcellWeights(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  using CaloClusterCollectionProcessor::execute; // xxx temporary
  StatusCode execute(xAOD::CaloClusterContainer*  clusColl);

 private:

  CaloComputeSWcellWeights();

  // Not used, but required to use this with CaloRunClusterCorrections.
  int m_order;
};

#endif
