/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CellsInCone.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloUtils/CaloCellList.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/Egamma.h"

void
DerivationFramework::CellsInCone::egammaSelect(
  xAOD::CaloCluster* inputCl,
  const CaloCellContainer* inputcells,
  const CaloDetDescrManager* caloMgr,
  const xAOD::Egamma* eg,
  const double dr)
{

  std::vector<const CaloCell*> cells;
  cells.reserve(100);
  CaloCellList myList(caloMgr, inputcells);

  double egEta = eg->caloCluster()->etaBE(2);
  double egPhi = eg->caloCluster()->phiBE(2);
  std::vector<CaloSampling::CaloSample> samples = {
    CaloSampling::PreSamplerB, CaloSampling::PreSamplerE,
    CaloSampling::EMB1,        CaloSampling::EME1,
    CaloSampling::EMB2,        CaloSampling::EME2,
    CaloSampling::EMB3,        CaloSampling::EME3,
    CaloSampling::TileBar0,    CaloSampling::TileBar1,
    CaloSampling::TileBar2,    CaloSampling::TileExt0,
    CaloSampling::TileExt1,    CaloSampling::TileExt2,
    CaloSampling::TileGap1,    CaloSampling::TileGap2,
    CaloSampling::TileGap3,    CaloSampling::HEC0,
    CaloSampling::HEC1,        CaloSampling::HEC2,
    CaloSampling::HEC3
  };

  for (auto samp : samples) {
    myList.select(egEta, egPhi, dr, samp);
    cells.insert(cells.end(), myList.begin(), myList.end());
  }

  for (const auto* cell : cells) {
    if (!cell || !cell->caloDDE())
      continue;
    int index = inputcells->findIndex(cell->caloDDE()->calo_hash());
    if (index == -1)
      continue;
    inputCl->addCell(index, 1.);
  }
}
