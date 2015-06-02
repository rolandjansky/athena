/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLESORT_H
#define CALOCELLESORT_H
//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloCellESort.h,v 1.1 2005-06-14 20:33:49 menke Exp $
//
// Description: E sorting for CaloCell 
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#include "CaloEvent/CaloCell.h"

namespace CaloCellESort{
// comparison,  order Cell by E
class compare
 {

  public:
  inline   compare() {} ;
  inline   bool operator () (const CaloCell* c1, 
			     const CaloCell* c2)
  {
    return c1->e() > c2->e() ;
  }  
  
 };
}

#endif // CALOCELLESORT_H

