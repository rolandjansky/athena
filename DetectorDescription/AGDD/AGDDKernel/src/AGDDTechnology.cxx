/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDTechnology.h"
#include "AGDDKernel/AGDDDetectorStore.h"

//namespace AGDDModel {

std::string AGDDTechnology::GetName() const
{
	return m_name;
}

AGDDTechnology::AGDDTechnology(const std::string& s,
                               AGDDDetectorStore& ds): m_name(s)
{
    ds.RegisterTechnology(this);
}

AGDDTechnology::~AGDDTechnology()
{
}
//} // namespace AGDDModel
