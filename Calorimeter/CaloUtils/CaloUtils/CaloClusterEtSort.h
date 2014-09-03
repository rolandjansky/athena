/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERETSORT_H
#define CALOCLUSTERETSORT_H
//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloClusterEtSort.h,v 1.2 2008-09-29 12:44:57 menke Exp $
//
// Description: Et sorting for CaloCluster 
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#include "xAODCaloEvent/CaloCluster.h"

namespace CaloClusterEtSort{
// comparison,  order cluster by Et
class compare
 {

  public:
  inline   compare() {} ;
  inline   bool operator () (const xAOD::CaloCluster* c1, 
			     const xAOD::CaloCluster* c2)
  {
    // assign to volatile double in order to avoid excess precission
    // in FP unit on x386 machines
    volatile double et1(c1->et());
    volatile double et2(c2->et());

    return et1 > et2 ;
  }  
  
 };
}

#endif // CALOCLUSTERETSORT_H

