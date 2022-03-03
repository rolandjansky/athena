/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_CELLSINCONE_H
#define DERIVATIONFRAMEWORK_CELLSINCONE_H

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaFwd.h"
class CaloCellContainer;
class CaloDetDescrManager;
namespace DerivationFramework {
namespace CellsInCone {
void
egammaSelect(xAOD::CaloCluster* inputCl,
             const CaloCellContainer* inputcells,
             const CaloDetDescrManager* caloMgr,
             const xAOD::Egamma* eg,
             const double dr);
}
}
#endif
