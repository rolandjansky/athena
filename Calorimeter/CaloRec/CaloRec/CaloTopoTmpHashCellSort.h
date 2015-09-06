/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTOPOTMPHASHCELLSORT_H
#define CALOTOPOTMPHASHCELLSORT_H
//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoTmpHashCellSort.h,v 1.2 2008-08-28 05:15:05 ssnyder Exp $
//
// Description: ration (could be S/N, or E/V) sorting for CaloTopoTmpHashCells
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#include "CaloRec/CaloTopoTmpHashCell.h"

namespace CaloTopoTmpHashCellSort{
// comparison,  order seed cells by E/V
template <class T>
class compare
 {

  public:
  inline   compare() {} ;
  inline   bool operator () (const CaloTopoTmpHashCell<T>& c1, 
			     const CaloTopoTmpHashCell<T>& c2)
  {
    return c1.getCaloTopoTmpClusterCell()->getSignedRatio() > c2.getCaloTopoTmpClusterCell()->getSignedRatio();
  }  
  
 };

template <class T>
class compareAbs
 {

  public:
  inline   compareAbs() {} ;
  inline   bool operator () (const CaloTopoTmpHashCell<T>& c1, 
			     const CaloTopoTmpHashCell<T>& c2)
  {
    return fabs(c1.getCaloTopoTmpClusterCell()->getSignedRatio()) > fabs(c2.getCaloTopoTmpClusterCell()->getSignedRatio());
  }  
 };
}
#endif // CALOTOPOTMPHASHCELLSORT_H

