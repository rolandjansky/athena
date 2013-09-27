/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto1DContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigHisto1DContainer_tlp1
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGHISTO1DCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainer_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigHisto1DContainerCnv_p1.h"   
 
class TrigHisto1DContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigHisto1DContainerCnv_p1, TrigHisto1DContainer_tlp1 >
{
 
 public:
 
  TrigHisto1DContainerCnv_tlp1();
  virtual ~TrigHisto1DContainerCnv_tlp1() {}
  
  //default methods  
  virtual void  setPStorage( TrigHisto1DContainer_tlp1 *storage );
  
 protected: 
  TrigHisto1DContainerCnv_p1    m_trigHisto1DContainerCnv;
  TrigHisto1DCnv_p1             m_trigHisto1DCnv;
 
}; //end of class definitions
 


#endif
