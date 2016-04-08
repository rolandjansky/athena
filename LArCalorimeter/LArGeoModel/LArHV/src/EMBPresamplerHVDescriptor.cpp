/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBPresamplerHVDescriptor.h"

EMBPresamplerHVDescriptor::~EMBPresamplerHVDescriptor()
{
}


EMBPresamplerHVDescriptor::EMBPresamplerHVDescriptor(const CellPartitioning &etaPartitioning, const CellBinning &phiBinning)
:m_etaPartitioning(etaPartitioning),m_phiBinning(phiBinning)	 
{
}





