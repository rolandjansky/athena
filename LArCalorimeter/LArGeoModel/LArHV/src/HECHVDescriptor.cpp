/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVDescriptor.h"
#include "LArHV/EMBHVDescriptor.h"

//##ModelId=47A079350327
HECHVDescriptor::HECHVDescriptor(const CellBinning &phiBinning):m_phiBinning(phiBinning)
{
}

//##ModelId=47A079350321
const CellBinning &HECHVDescriptor::getPhiBinning() const
{
  return m_phiBinning;
}

//##ModelId=47A07935031D
HECHVDescriptor::~HECHVDescriptor()
{
}

