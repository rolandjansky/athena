/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETContainer_p1
 *
 * @brief persistent partner for TrigMissingETContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMissingETContainer_p1.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCONTAINER_P1_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCONTAINER_P1_H


#include "AthenaPoolUtilities/TPObjRef.h"  
#include <vector>
  
class TrigMissingETContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigMissingETContainer_p1(){}
     
};// end of class definitions
 
 
#endif
