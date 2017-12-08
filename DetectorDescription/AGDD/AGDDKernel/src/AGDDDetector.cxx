/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"

void AGDDDetector::SetAddressAndPosition(AGDDDetectorPositioner *p) 
{
	AddDetectorPosition(p);
	SetDetectorAddress(p);
}
	
std::vector<AGDDDetectorPositioner*>& AGDDDetector::GetDetectorPositioners() 
{
	return detectorPositions;
}
		
void AGDDDetector::AddDetectorPosition(AGDDDetectorPositioner* p) 
{
	detectorPositions.push_back(p);
}

