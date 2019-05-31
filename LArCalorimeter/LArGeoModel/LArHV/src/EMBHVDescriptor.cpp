/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVDescriptor.h"

EMBHVDescriptor::EMBHVDescriptor(const CellBinning &etaBinning
				 , const CellBinning &phiBinning)
  : m_etaBinning(etaBinning)
  , m_phiBinning(phiBinning)	 
{
}

EMBHVDescriptor::~EMBHVDescriptor()
{
}





