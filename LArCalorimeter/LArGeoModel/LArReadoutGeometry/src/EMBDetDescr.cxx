/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/EMBDetDescr.h"
#include "LArReadoutGeometry/EMBBasicReadoutNumbers.h"
#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "GeoModelKernel/CellBinning.h"
struct Boundary {
  double min;
  double max;
  unsigned int nDiv;
  unsigned int firstDiv;
} etaBoundariesBarrel [4][2] = {{{0.000, 1.525,  61,0},{0.000, 0.000,  0,0}},    // 
				{{0.003125, 1.400, 447,1},{1.400, 1.475,  3,0}},    // 
				{{0.000, 1.400,  56,0},{1.400, 1.475,  1,0}},    // 
				{{0.000, 1.350,  27,0},{0.000, 0.000,  0,0}}};   // 


 

EMBDetDescr::EMBDetDescr (const EMBDetectorManager *detManager, unsigned int sampling, unsigned int region, const CellBinning &phiBinning)
  :manager(detManager),samplingIndex(sampling),regionIndex(region),phiBinning(phiBinning),etaBinning(etaBoundariesBarrel[sampling][region].min,
												     etaBoundariesBarrel[sampling][region].max,
												     etaBoundariesBarrel[sampling][region].nDiv,
												     etaBoundariesBarrel[sampling][region].firstDiv)
{
  
  for (unsigned int i=0;i<etaBinning.getFirstDivisionNumber()+etaBinning.getNumDivisions();i++) {
    if (i<etaBinning.getFirstDivisionNumber()) {
      offset.push_back(0);
      halfLength.push_back(0);
    }
    else {
      double front, back;
      switch (sampling) {
      case 0:
	front = manager->getBasicReadoutNumbers()->getPresamplerRadius();
	back  = manager->getBasicReadoutNumbers()->getRinAc();
	break;
      case 1:
	front  = manager->getBasicReadoutNumbers()->getRinAc();
	back   = region==0 ? manager->getBasicReadoutNumbers()->getEMBSamplingSepInnerRMax(i) : manager->getBasicReadoutNumbers()->getEMBSamplingSepInnerRMax(447);
	break;
      case 2:
	front   = region==0 ? manager->getBasicReadoutNumbers()->getEMBSamplingSepInnerRMax(std::max((unsigned int) 0,8*i+7)) : manager->getBasicReadoutNumbers()->getEMBSamplingSepInnerRMax(447);
	back  = manager->getBasicReadoutNumbers()->getRMX23(std::min((unsigned int) 52,i));
	break;
      case 3:
	front  = (manager->getBasicReadoutNumbers()->getRMX23(std::min((unsigned int)52,2*i))+manager->getBasicReadoutNumbers()->getRMX23(std::min((unsigned int)52,2*i+1)))/2.0;
	back  = manager->getBasicReadoutNumbers()->getRoutAc();
	break;
      default:
	throw std::range_error("Attempt to construct EMEB DetDescr with illegal sampling index");
	
      }
      offset.push_back((front+back)/2.);
      halfLength.push_back((back-front)/2.);
    }
  }
}


EMBDetDescr::~EMBDetDescr()
{
}


