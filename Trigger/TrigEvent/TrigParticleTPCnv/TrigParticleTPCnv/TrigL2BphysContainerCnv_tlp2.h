/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BphysContainerCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigL2BphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysContainerCnv_tlp2.h,v 1.1 2010-08-11 demelian Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_CNV_TLP2_H
#define  TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_CNV_TLP2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_p1.h"   
#include "TrigParticleTPCnv/TrigL2BphysCnv_p3.h" 
#include "TrigParticleTPCnv/TrigL2BphysContainer_tlp2.h"

class TrigL2BphysContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrigL2BphysContainerCnv_p1, TrigL2BphysContainer_tlp2 >
{
 
 public:
 
  TrigL2BphysContainerCnv_tlp2();
  virtual ~TrigL2BphysContainerCnv_tlp2() {}
 
  //default methods  
  virtual void  setPStorage( TrigL2BphysContainer_tlp2 *storage );
  
 protected: 
  TrigL2BphysContainerCnv_p1    m_trigL2BphysContCnv      ;
  TrigL2BphysCnv_p3             m_L2BphysCnv              ;
}; //end of class definitions
 

#endif
