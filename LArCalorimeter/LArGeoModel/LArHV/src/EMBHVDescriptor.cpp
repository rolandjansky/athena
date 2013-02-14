/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVDescriptor.h"

//##ModelId=4755BF390264
EMBHVDescriptor::~EMBHVDescriptor()
{
}


//##ModelId=4755BF39021B
EMBHVDescriptor::EMBHVDescriptor(const CellBinning &etaBinning, const CellBinning &phiBinning)
:etaBinning(etaBinning),phiBinning(phiBinning)	 
{
}





