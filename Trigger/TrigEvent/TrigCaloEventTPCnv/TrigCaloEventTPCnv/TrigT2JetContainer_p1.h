/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetContainer_p1
 *
 * @brief persistent partner for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2JetContainer_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigT2JetContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigT2JetContainer_p1(){}
     
};// end of class definitions
 
 
#endif
