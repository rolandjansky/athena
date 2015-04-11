/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HICaloCellHelper.h"
#include "CaloEvent/CaloCell.h"
#include "CxxUtils/prefetch.h"
#include "HIEventUtils/HIEventDefs.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <set>
#include <TMath.h>

float HICaloCellHelper::GetAreaEtaPhi(const CaloCell* theCell)
{
  float deta=theCell->caloDDE()->deta();
  float dphi=theCell->caloDDE()->dphi();
  return std::abs(deta*dphi);
}

float HICaloCellHelper::GetEtDensity(const CaloCell* theCell, float geoWeight)
{
  float density=theCell->et();
  float area=HICaloCellHelper::GetAreaEtaPhi(theCell);
  if(area<1e-9) area=0.2*TMath::Pi()/geoWeight;
  density/=area;
  return density;
}

std::string HICaloCellHelper::DumpCell(const CaloCell* theCell) 
{
  std::stringstream ss;
  ss << "CELLINFO:" 
     << std::setw(10) << (CaloSampling::CaloSample) theCell->caloDDE()->getSampling()
     << std::setw(20) << theCell->ID().get_compact()
     << std::setw(10) << theCell->et()
     << std::setw(10) << theCell->caloDDE()->eta()
     << std::setw(10) << theCell->caloDDE()->phi()
     << std::setw(15) << theCell->caloDDE()->deta()
     << std::setw(15) << theCell->caloDDE()->dphi()
     << std::setw(15) << HICaloCellHelper::GetAreaEtaPhi(theCell);
  return ss.str();
}
