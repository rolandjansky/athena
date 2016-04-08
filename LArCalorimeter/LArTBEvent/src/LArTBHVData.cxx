/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArTBEvent/LArTBHVData.h"

LArTBHVData::LArTBHVData(int moduleNumber, short detectorType, short unit,
			 int nHVch, std::vector<float> HVdata)
{    
  m_moduleNumber    = moduleNumber;
  m_detectorType    = detectorType;
  m_unit            = unit;
  m_nHVch           = nHVch;
  m_HVdata          = HVdata; 
}

LArTBHVData::~LArTBHVData() { }

LArTBHVData::operator std::string() const
{
  std::string error_message = "Unable to get LArTBHVData string" ;
  return error_message ;
}
