/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloComputeSWcellWeights.cxx
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

*********************************************************************/

// include header files
#include "CaloComputeSWcellWeights.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterContainer.h"
//#include "CaloDetDescr/CaloDetDescrElement.h"
//#include "CaloDetDescr/CaloDetDescrManager.h"
//#include "CaloIdentifier/CaloCell_ID.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------

CaloComputeSWcellWeights::CaloComputeSWcellWeights(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
  : AthAlgTool(type, name, parent)
{ 
}

// Make Corrections for SW cell weights 
StatusCode
CaloComputeSWcellWeights::execute(const EventContext& /*ctx*/,
                                  xAOD::CaloClusterContainer*  clusColl) const
{
 

  ATH_MSG_DEBUG( "Executing CaloComputeSWcellWeights" << endmsg) ; 
  
  std::map<IdentifierHash,double> WeightMap; //FIXME: use a vector!

  for (const xAOD::CaloCluster* cluster : *clusColl) {
    for (const CaloCell* cell : *cluster) {
      IdentifierHash myHashId = cell->caloDDE()->calo_hash();
      WeightMap[myHashId] += cell->e();
    }
  }   
  
  for (xAOD::CaloCluster* cluster : *clusColl) {
    xAOD::CaloCluster::cell_iterator cellIter    = cluster->cell_begin();
    xAOD::CaloCluster::cell_iterator cellIterEnd = cluster->cell_end();
    for( ;cellIter!=cellIterEnd;cellIter++) {
      const CaloCell* cell = *cellIter;
      IdentifierHash myHashId =  cell->caloDDE()->calo_hash();
      double WEIGHT=(cell->e())/WeightMap[myHashId];
      cellIter.reweight(WEIGHT);
    }
    CaloClusterKineHelper::calculateKine(cluster); //FIXME .. maybe unnecessary
  }  

  return StatusCode::SUCCESS;
}





