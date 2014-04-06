/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigEFBphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysContainerCnv_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_CNV_TLP1_H
#define  TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

 
#include "TrigParticleTPCnv/TrigEFBphysContainerCnv_p1.h"   
#include "TrigParticleTPCnv/TrigEFBphysCnv_p1.h" 
#include "TrigParticleTPCnv/TrigEFBphysCnv_p2.h" 
#include "TrackParticleTPCnv/Particle/TrackParticleContainerCnv_p1.h"
 
#include "TrigParticleTPCnv/TrigEFBphysContainer_tlp1.h"

class TrigEFBphysContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigEFBphysContainerCnv_p1, TrigEFBphysContainer_tlp1 >
{
 
 public:
 
  TrigEFBphysContainerCnv_tlp1();
  virtual ~TrigEFBphysContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigEFBphysContainer_tlp1 *storage );
  
 protected: 
  TrigEFBphysCnv_p1             m_EFBphysCnv          ;
  TrigEFBphysCnv_p2             m_EFBphysCnv_p2       ;
  TrackParticleContainerCnv_p1  m_TrackParticleContainerCnv;
      
}; //end of class definitions
 


#endif
