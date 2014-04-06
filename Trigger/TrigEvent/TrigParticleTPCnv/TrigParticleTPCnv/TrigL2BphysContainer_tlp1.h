/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BphysContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigL2BphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysContainer_tlp1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_TLP1_H
#define TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_TLP1_H

#include "TrigParticleTPCnv/TrigL2BphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p1.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p2.h"
#include "TrigInDetEventTPCnv/TrigVertex_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_p1.h"

class TrigL2BphysContainer_tlp1 
{
   
 public:
   
  TrigL2BphysContainer_tlp1(){}
  friend class TrigL2BphysContainerCnv_tlp1;

 private:

  std::vector<TrigL2BphysContainer_p1>       m_TrigL2BphysContainers ;
  std::vector<TrigL2Bphys_p1>                m_L2Bphys               ;
  std::vector<TrigL2Bphys_p2>                m_L2Bphys_p2            ;
  std::vector<TrigVertex_p1>                 m_trigVertex            ;
  std::vector<TrigInDetTrack_p1>             m_trigInDetTrack        ;
  std::vector<TrigInDetTrackFitPar_p1>       m_trigInDetTrackFitPar  ;
  std::vector<TrigInDetTrackCollection_p1>   m_trigInDetTrackCollection  ;
 
};//end of class definitions
 
#endif
