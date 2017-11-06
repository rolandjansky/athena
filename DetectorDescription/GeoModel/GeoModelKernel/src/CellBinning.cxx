/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/CellBinning.h"

CellBinning::CellBinning (double startVal, double endVal, unsigned int nBins, unsigned int firstDivisionNumber)
  :m_start(startVal),m_end(endVal),m_numDivisions(nBins),m_delta((endVal-startVal)/m_numDivisions),m_firstDivisionNumber(firstDivisionNumber)
{
}

