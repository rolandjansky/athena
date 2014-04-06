/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BphysContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigL2BphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysContainerCnv_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_CNV_TLP1_H
#define  TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_p1.h"   
#include "TrigInDetEventTPCnv/TrigVertexCnv_p1.h" 
#include "TrigParticleTPCnv/TrigL2BphysCnv_p1.h" 
#include "TrigParticleTPCnv/TrigL2BphysCnv_p2.h" 
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h" 
#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h" 
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_p1.h" 
 
#include "TrigParticleTPCnv/TrigL2BphysContainer_tlp1.h"

class TrigL2BphysContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigL2BphysContainerCnv_p1, TrigL2BphysContainer_tlp1 >
{
 
 public:
 
  TrigL2BphysContainerCnv_tlp1();
  virtual ~TrigL2BphysContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigL2BphysContainer_tlp1 *storage );
  
 protected: 
  TrigL2BphysContainerCnv_p1    m_trigL2BphysContCnv      ;
  TrigL2BphysCnv_p1             m_L2BphysCnv              ;
  TrigL2BphysCnv_p2             m_L2BphysCnv_p2           ;
  TrigVertexCnv_p1              m_VertexCnv               ;
  TrigInDetTrackCnv_p1          m_trigInDetTrackCnv       ;
  TrigInDetTrackFitParCnv_p1    m_trigInDetTrackFitParCnv ;
  TrigInDetTrackCollectionCnv_p1    m_trigInDetTrackCollectionCnv ;
  
}; //end of class definitions
 

#endif
