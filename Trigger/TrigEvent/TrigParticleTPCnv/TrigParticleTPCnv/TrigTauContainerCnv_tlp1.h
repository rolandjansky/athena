/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigTauContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauContainerCnv_tlp1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGTAUCONTAINER_CNV_TLP1_H
#define  TRIGPARTICLETPCNV_TRIGTAUCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigTauContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigTauContainerCnv_p1.h"   

#include "TrigParticleTPCnv/TrigTauCnv_p2.h"   
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h" 

 
class TrigTauContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigTauContainerCnv_p1, TrigTauContainer_tlp1 >
{
 
 public:
 
  TrigTauContainerCnv_tlp1();
  virtual ~TrigTauContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigTauContainer_tlp1 *storage );
  
 protected: 
  TrigTauContainerCnv_p1    m_trigTauContCnv;
  TrigTauCnv_p2             m_TauCnv;
  P4PtEtaPhiMCnv_p1            m_P4PtEtaPhiMCnv;
 
}; //end of class definitions
 

#endif
