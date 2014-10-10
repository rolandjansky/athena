/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimHelpers/DetectorGeometryHelper.h"
#include <iostream>

DetectorMap DetectorGeometryHelper::theMap;

DetectorGeometryHelper::DetectorGeometryHelper(std::string n, DetectorType t)
{
	theMap[n]=t;
}

DetectorType DetectorGeometryHelper::GeometryType(std::string n)
{
	if (theMap.find(n) != theMap.end()) return theMap[n];
	else return Standard;
}

void DetectorGeometryHelper::SetDetectorType(std::string n, DetectorType t)
{
	theMap[n]=t;
}
