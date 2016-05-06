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

#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellList.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <vector>

class eflowCellSubtractionFacilitator{

 public:

  eflowCellSubtractionFacilitator();

  double subtractCells(eflowRingSubtractionManager& ringSubtractionManager, double trackEnergy, xAOD::CaloCluster* tracksClus, eflowCellList& orderedCells);
  double subtractCells(eflowRingSubtractionManager& ringSubtractionManager, double trackEnergy, std::vector<xAOD::CaloCluster*> tracksClus, eflowCellList& orderedCells);
  bool annFlag() {return m_annFlag;}
  void setAnnFlag() { m_annFlag = true; }

 private:
  bool m_annFlag;
  CaloClusterCellLink::iterator getCellIterator(const xAOD::CaloCluster* thisCluster, const CaloCell* thisCell);
  void updateClusterKinematics(std::vector<xAOD::CaloCluster*>& tracksClusters);
  void updateClusterKinematics(xAOD::CaloCluster*);

  double getTotalEnergy(std::vector<xAOD::CaloCluster*> tracksClusters);

  double getRingsEnergy(const std::vector<xAOD::CaloCluster*>& tracksClusters, CellIt beginRing,
                        CellIt endRing);
  void subtractPartialRings(const std::vector<xAOD::CaloCluster*>& tracksClusters, CellIt beginRing,
                            CellIt endRing, double targetRingEnergy, double eRing);
  void subtractFullRings(const std::vector<xAOD::CaloCluster*>& tracksClusters, CellIt beginRing,
                         CellIt endRing);
  bool subtractRings(eflowRingSubtractionManager& ringSubtractionManager,
                     const std::pair<eflowCaloENUM, short>& ring, double & eSubtracted,
                     const double eExpect, eflowCellList& orderedCells,
                     std::vector<xAOD::CaloCluster*>& tracksClusters);

  bool subtractCaloCell(double & eSubtracted, const double eExpect, xAOD::CaloCluster* cluster, CaloCell* cell);
  void annihilateClusters(std::vector<xAOD::CaloCluster*>& tracksClusters);
  bool subtractReorderedCells(const std::vector<xAOD::CaloCluster*>& tracksClusters,
                            double eSubtracted, const double eExpect, eflowCellList& orderedCells);
};
#endif
