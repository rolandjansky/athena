/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigEFBphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysContainer_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_TLP1_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_TLP1_H

#include "TrigParticleTPCnv/TrigEFBphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBphys_p1.h"
#include "TrigParticleTPCnv/TrigEFBphys_p2.h"
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p1.h"


class TrigEFBphysContainer_tlp1 
{
   
 public:
   
  TrigEFBphysContainer_tlp1(){}
  friend class TrigEFBphysContainerCnv_tlp1;

 private:

  std::vector<TrigEFBphysContainer_p1>       m_TrigEFBphysContainers ;
  std::vector<TrigEFBphys_p1>                m_EFBphys               ;
  std::vector<TrigEFBphys_p2>                m_EFBphys_p2            ;
  std::vector<Rec::TrackParticleContainer_p1>     m_TrackParticleContainer;
 
};//end of class definitions
 
#endif
