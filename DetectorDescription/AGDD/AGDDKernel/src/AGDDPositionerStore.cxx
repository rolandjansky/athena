/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDPositioner.h"
#include <iostream>

AGDDPositionerStore::AGDDPositionerStore()
{
}

void AGDDPositionerStore::RegisterPositioner(AGDDPositioner *v)
{
	(*this).push_back(v);
}

AGDDPositioner* AGDDPositionerStore::GetPositioner(unsigned int i)
{
	if (i<size())
		return (*this)[i];
	else
	{
		std::cout << " Positioner "<<i<<" not found: returning 0"<<std::endl;
		return 0;
	}
}
