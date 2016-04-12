/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloEvent/CaloClusterCellLink.h"

CaloClusterCellLink::~CaloClusterCellLink() {}

CaloClusterCellLink::CaloClusterCellLink() {}
		      
//0-size vector for dummy iterator
const CaloClusterCellLink::linkAndWeightCollType CaloClusterCellLink::m_dummyIndicesAndWeights(0);

// Dummy iterator to be returned by begin/end if no cell-links are present
const CaloClusterCellLink::const_iterator CaloClusterCellLink::dummyIt(nullptr, CaloClusterCellLink::m_dummyIndicesAndWeights.end());



CaloClusterCellLink::CaloClusterCellLink(const CaloCellContainer* cellCont) :
  m_cellCont(cellCont)
{}


CaloClusterCellLink::CaloClusterCellLink(const DataLink<CaloCellContainer>& cellCont) :
  m_cellCont(cellCont)
{}


CaloClusterCellLink::iterator CaloClusterCellLink::removeCell(CaloClusterCellLink::iterator cellItr) {
  linkAndWeightCollType::iterator toDelete=cellItr.m_it;
  auto newVecIt=m_indicesAndWeights.erase(toDelete);
  return iterator(cellItr.m_ccc,newVecIt);
  
}


bool CaloClusterCellLink::removeCell(const CaloCell* ptrToDelete) {
  bool success=false;
  CaloClusterCellLink::iterator it=this->begin();
  CaloClusterCellLink::iterator it_e=this->end();
  for(;it!=it_e;++it) {
    const CaloCell* pCell=(*it);
    if (pCell==ptrToDelete) { //Pointer comparison!
      this->removeCell(it);
      success=true;
      break;
    }//end cell found
  }//end for loop
  return success;
}


/**@brief Prepare for writing.
 */
void CaloClusterCellLink::toPersistent()
{
  m_cellCont.toPersistent();
}
