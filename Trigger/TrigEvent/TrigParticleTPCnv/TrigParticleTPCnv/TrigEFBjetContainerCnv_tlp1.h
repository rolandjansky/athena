/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigEFBjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBjetContainerCnv_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_TLP1_H
#define  TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigEFBjetContainer_tlp1.h"
 
#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_p1.h"   
 
class TrigEFBjetContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigEFBjetContainerCnv_p1, TrigEFBjetContainer_tlp1 >
{
 
 public:
 
  TrigEFBjetContainerCnv_tlp1();
  virtual ~TrigEFBjetContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigEFBjetContainer_tlp1 *storage );
  
 protected: 
  TrigEFBjetContainerCnv_p1    m_trigEFBjetContCnv  ;
  TrigEFBjetCnv_p1             m_EFBjetCnv          ;
   
}; //end of class definitions
 



#endif
