/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigL2BjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BjetContainerCnv_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_TLP1_H
#define  TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigL2BjetContainer_tlp1.h"
 
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_p1.h"   
 
class TrigL2BjetContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigL2BjetContainerCnv_p1, TrigL2BjetContainer_tlp1 >
{
 
 public:
 
  TrigL2BjetContainerCnv_tlp1();
  virtual ~TrigL2BjetContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigL2BjetContainer_tlp1 *storage );
  
 protected: 
  TrigL2BjetContainerCnv_p1    m_trigL2BjetContCnv  ;
  TrigL2BjetCnv_p1             m_L2BjetCnv          ;
   
}; //end of class definitions
 


#endif
