/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysContainerCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigEFBphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysContainerCnv_tlp2.h,v 1.1 2010-08-12 demelian Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_CNV_TLP2_H
#define  TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_CNV_TLP2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

 
#include "TrigParticleTPCnv/TrigEFBphysContainerCnv_p1.h"   
#include "TrigParticleTPCnv/TrigEFBphysCnv_p3.h" 
 
#include "TrigParticleTPCnv/TrigEFBphysContainer_tlp2.h"

class TrigEFBphysContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrigEFBphysContainerCnv_p1, TrigEFBphysContainer_tlp2 >
{
 
 public:
 
  TrigEFBphysContainerCnv_tlp2();
  virtual ~TrigEFBphysContainerCnv_tlp2() {}
 
  //default methods  
  virtual void  setPStorage( TrigEFBphysContainer_tlp2 *storage );
  
 protected: 
  TrigEFBphysContainerCnv_p1    m_trigEFBphysContCnv      ;
  TrigEFBphysCnv_p3             m_EFBphysCnv          ;
      
}; //end of class definitions
 


#endif
