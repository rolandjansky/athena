/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLSUBTRACTIONFACILITATOR_H
#define EFLOWCELLSUBTRACTIONFACILITATOR_H

/********************************************************************

NAME:     eflowCellSubtractionFacilitator.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  3rd February 2009

********************************************************************/

#include "eflowRec/eflowCellSubtractionManager.h"
#include "eflowRec/eflowCellList.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <vector>

class eflowCellSubtractionFacilitator{

 public:

  eflowCellSubtractionFacilitator();

  double subtractCells(eflowCellSubtractionManager& cellSubtractionManager, double trackEnergy, xAOD::CaloCluster* tracksClus,eflowCellList& orderedCells);
  double subtractCells(eflowCellSubtractionManager& cellSubtractionManager, double trackEnergy, std::vector<xAOD::CaloCluster*> tracksClus,eflowCellList& orderedCells);
  bool annFlag() {return m_annFlag;}

 private:
  bool m_annFlag;
  CaloClusterCellLink::iterator getCellIterator(const xAOD::CaloCluster* thisCluster, const CaloCell* thisCell);

};
#endif
