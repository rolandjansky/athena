/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HICALOHELPER_H__
#define HIJETREC_HICALOHELPER_H__

#include <string>
#include <vector>
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloCluster.h"


class CaloCell;


namespace HICaloCellHelper{

  float getEtDensity(const CaloCell* theCell, float geoWeight);
  float getAreaEtaPhi(const CaloCell* theCell);
  void updateShape(xAOD::HIEventShapeContainer* shape, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg=false);

  std::string dumpCell(const CaloCell* theCell);
}

#endif
