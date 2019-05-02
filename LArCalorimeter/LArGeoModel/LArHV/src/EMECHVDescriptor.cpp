/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVDescriptor.h"

EMECHVDescriptor::EMECHVDescriptor(const CellPartitioning &etaBinning
				   , const CellBinning &phiBinning
				   , const CellBinning &sectorBinning)
  : m_etaBinning(etaBinning)
  , m_phiBinning(phiBinning)
  , m_sectorBinning(sectorBinning)
{
}

EMECHVDescriptor::~EMECHVDescriptor()
{
}




