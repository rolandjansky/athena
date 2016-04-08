/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVDescriptor.h"

//##ModelId=478D138C01BE
EMECHVDescriptor::~EMECHVDescriptor()
{
}

//##ModelId=478D138C0214
EMECHVDescriptor::EMECHVDescriptor(const CellPartitioning &etaBinning, const CellBinning &phiBinning, const CellBinning &sectorBinning):
  m_etaBinning(etaBinning),m_phiBinning(phiBinning),m_sectorBinning(sectorBinning)
{
}


