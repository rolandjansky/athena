/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCountsCollectionCnv_p1
 *
 * @brief transient persistent converter for TrigVertexCountsCollection
 *
 * @author W.Bell@cern.ch  <w.bell@physics.gla.ac.uk>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGT2MBTSBITSCOLLECTION_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGT2MBTSBITSCOLLECTION_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigVertexCounts.h"
#include "TrigInDetEvent/TrigVertexCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCollection_p1.h"

class TrigVertexCountsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv<TrigVertexCountsCollection, TrigVertexCountsCollection_p1, ITPConverterFor<TrigVertexCounts> >
{
 
 public:
  TrigVertexCountsCollectionCnv_p1() {}

}; //end of class definitions

#endif
