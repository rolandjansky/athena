/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBPresamplerHVDescriptor.h"

EMBPresamplerHVDescriptor::EMBPresamplerHVDescriptor(const CellPartitioning &etaPartitioning
						     , const CellBinning &phiBinning)
  : m_etaPartitioning(etaPartitioning)
  , m_phiBinning(phiBinning)	 
{
}

EMBPresamplerHVDescriptor::~EMBPresamplerHVDescriptor()
{
}
