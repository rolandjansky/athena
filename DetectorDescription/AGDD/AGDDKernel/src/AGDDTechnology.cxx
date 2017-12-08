/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDTechnology.h"
#include "AGDDKernel/AGDDDetectorStore.h"

//namespace AGDDModel {

std::string AGDDTechnology::GetName()
{
	return m_name;
}

AGDDTechnology::AGDDTechnology(std::string s): m_name(s)
{
	AGDDDetectorStore *ds=AGDDDetectorStore::GetDetectorStore();
    ds->RegisterTechnology(this);
}

AGDDTechnology::~AGDDTechnology()
{
}
//} // namespace AGDDModel
