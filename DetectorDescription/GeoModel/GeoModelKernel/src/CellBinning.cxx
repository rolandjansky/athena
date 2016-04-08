/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/CellBinning.h"

CellBinning::CellBinning (double startVal, double endVal, unsigned int nBins, unsigned int firstDivisionNumber)
  //## begin CellBinning::CellBinning%445A13F30367.hasinit preserve=no
  //## end CellBinning::CellBinning%445A13F30367.hasinit
  //## begin CellBinning::CellBinning%445A13F30367.initialization preserve=yes
  :m_start(startVal),m_end(endVal),m_numDivisions(nBins),m_delta((endVal-startVal)/m_numDivisions),m_firstDivisionNumber(firstDivisionNumber)
  //## end CellBinning::CellBinning%445A13F30367.initialization
{
  //## begin CellBinning::CellBinning%445A13F30367.body preserve=yes
  //## end CellBinning::CellBinning%445A13F30367.body
}


// Additional Declarations
  //## begin CellBinning%445A130F034A.declarations preserve=yes
  //## end CellBinning%445A130F034A.declarations

//## begin module%445A130F034A.epilog preserve=yes
//## end module%445A130F034A.epilog
