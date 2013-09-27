/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto2DContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigHisto2DContainer_tlp1
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGHISTO2DCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainer_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigHisto2DContainerCnv_p1.h"   
 
class TrigHisto2DContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigHisto2DContainerCnv_p1, TrigHisto2DContainer_tlp1 >
{
 
 public:
 
  TrigHisto2DContainerCnv_tlp1();
  virtual ~TrigHisto2DContainerCnv_tlp1() {}
  
  //default methods  
  virtual void  setPStorage( TrigHisto2DContainer_tlp1 *storage );
  
 protected: 
  TrigHisto2DContainerCnv_p1    m_trigHisto2DContainerCnv;
  TrigHisto2DCnv_p1             m_trigHisto2DCnv;
 
}; //end of class definitions
 



#endif
