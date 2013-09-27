/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCollectionCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigVertexCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigVertexCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_p1.h"   
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h" 
#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h" 

 
class TrigVertexCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigVertexCollectionCnv_p1, TrigVertexCollection_tlp1 >
{
 
 public:
 
  TrigVertexCollectionCnv_tlp1();
  virtual ~TrigVertexCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigVertexCollection_tlp1 *storage );
  
 protected: 
  TrigVertexCollectionCnv_p1    m_trigVertexCollCnv;
  TrigVertexCnv_p1              m_VertexCnv;
  TrigInDetTrackCnv_p1          m_TrigIDtrackCnv;
  TrigInDetTrackFitParCnv_p1    m_TrigIDtrackFitParCnv;
 
}; //end of class definitions
 

#endif
