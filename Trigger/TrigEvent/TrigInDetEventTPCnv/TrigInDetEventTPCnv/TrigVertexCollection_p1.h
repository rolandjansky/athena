/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCollection_p1
 *
 * @brief persistent partner for TrigVertexCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigVertexCollection_p1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_P1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigVertexCollection_p1 : public std::vector<TPObjRef>
{
 public:    
  TrigVertexCollection_p1(){}

};// end of class definitions
 
 
#endif
