/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HICALOHELPER_H__
#define __HICALOHELPER_H__

#include <string>
#include <vector>
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloCluster.h"


class CaloCell;


namespace HICaloCellHelper{

  float GetEtDensity(const CaloCell* theCell, float geoWeight);
  float GetAreaEtaPhi(const CaloCell* theCell);
  void UpdateShape(xAOD::HIEventShapeContainer* shape, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg=false);  


  std::string DumpCell(const CaloCell* theCell);


  
}


#endif

