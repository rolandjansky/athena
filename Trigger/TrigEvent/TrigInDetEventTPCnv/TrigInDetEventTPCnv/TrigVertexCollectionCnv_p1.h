/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCollectionCnv_p1
 *
 * @brief transient persistent converter for TrigVertexCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigVertexCollectionCnv_p1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEventTPCnv/TrigVertexCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigVertexCnv_p1.h"

class MsgStream;

class TrigVertexCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigVertexCollection, TrigVertexCollection_p1, ITPConverterFor<TrigVertex> >
{
 
 public:
  TrigVertexCollectionCnv_p1() {}
}; //end of class definitions
 
#endif
