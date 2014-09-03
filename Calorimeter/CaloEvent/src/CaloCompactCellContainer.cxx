/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloCompactCellContainer.cxx,v 1.2 2004-08-05 22:15:01 menke Exp $
//
// Description: see CaloCompactCellContainer.h
//
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#include "CaloEvent/CaloCompactCellContainer.h"

CaloCompactCell CaloCompactCellContainer::getCompactCell
( const int & iCell, 
  const int & nBytesPerCell) const
{
  // the size of value_type must be an integral multiple
  // of the size of CaloCompactCell::value_type 
  const int nRatio = sizeof(value_type)/sizeof(CaloCompactCell::value_type);

  // the number of words (in terms of the CaloCompactCell::value_type) 
  // needed to store one compact cell
  const int nWords = nBytesPerCell/sizeof(CaloCompactCell::value_type);
  
  // the compace data for one cell
  std::vector<CaloCompactCell::value_type> cData(nWords);

  for (int iWord=0;iWord<nWords;iWord++) {
    // the vector element to use for this cell
    int iPos = m_compactData[0] + (int)((iCell*nWords+iWord)/nRatio);
    
    // the relative position inside the vector element to use for this cell
    int iMod = ((iCell*nWords+iWord)%nRatio)<<(8/nRatio);

    // shift the current vector element down and project out 16 bits
    cData[iWord] = (CaloCompactCell::value_type) (((m_compactData[iPos])>>iMod) 
						  & CaloCompactCell::WORDMASK);
    
  }
  
  CaloCompactCell cCell(cData);
  return cCell;
}

const std::vector<CaloCompactCellContainer::value_type> CaloCompactCellContainer::getHeader() const
{
  std::vector<value_type> theHeader(m_compactData[0]);
  for(int i=0;i<m_compactData[0];i++)
    theHeader[i] = m_compactData[i];
  return theHeader;
}

void CaloCompactCellContainer::resize (const unsigned int & totalSize)
{
  m_compactData.resize(totalSize);
}

void CaloCompactCellContainer::setHeader
(const std::vector<CaloCompactCellContainer::value_type> & theHeader) 
{
  for(unsigned int i=0;i<theHeader.size();i++)
    m_compactData[i] = theHeader[i];
}

void CaloCompactCellContainer::setCompactCell
(const CaloCompactCell &theCompactCell,
 const int & iCell,
 const int & headerLength) {
  
  // the size of value_type must be an integral multiple
  // of the size of CaloCompactCell::value_type 
  const int nRatio = sizeof(value_type)/sizeof(CaloCompactCell::value_type);

  // the compace data for one cell
  std::vector<CaloCompactCell::value_type> cData(theCompactCell.getData());

  // the number of words (in terms of the CaloCompactCell::value_type) 
  // needed to store one compact cell
  const int nWords = cData.size();
  
  for (int iWord=0;iWord<nWords;iWord++) {
    // the vector element to use for this cell
    int iPos = headerLength + (int)((iCell*nWords+iWord)/nRatio);
    
    // the relative position inside the vector element to use for this cell
    int iMod = ((iCell*nWords+iWord)%nRatio)<<(8/nRatio);

    if ( iMod == 0 ) 
      m_compactData[iPos] = 0;
    // shift the 16 bits to the correct position and put them into the
    // current vector element
    m_compactData[iPos] = m_compactData[iPos] | (cData[iWord]<<iMod);
  }
}
