/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCollectionCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigVertexCollection
 *
 * @author Dmitry Emeliyanov
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_CNV_TLP2_H
#define  TRIGINDETEVENTTPCNV_TRIGVERTEXCOLLECTION_CNV_TLP2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

 
#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_p1.h"   
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p3.h" 
#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p3.h" 
#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp2.h"
 
class TrigVertexCollectionCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrigVertexCollectionCnv_p1, TrigVertexCollection_tlp2 >
{
 
 public:
 
  TrigVertexCollectionCnv_tlp2();
  virtual ~TrigVertexCollectionCnv_tlp2() {}
 
  //default methods  
  virtual void  setPStorage( TrigVertexCollection_tlp2 *storage );
  
 protected: 
  TrigVertexCollectionCnv_p1    m_trigVertexCollCnv;
  TrigVertexCnv_p2              m_VertexCnv;
  TrigInDetTrackCnv_p3          m_TrigIDtrackCnv;
  TrigInDetTrackFitParCnv_p3    m_TrigIDtrackFitParCnv;
 
}; //end of class definitions
 

#endif
