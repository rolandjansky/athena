/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysContainer_tlp2
 *
 * @brief "top level" persistent partner for TrigEFBphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysContainer_tlp2.h,v 1.1 2010-08-12 demelian Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_TLP2_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_TLP2_H

#include "TrigParticleTPCnv/TrigEFBphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBphys_p3.h"

class TrigEFBphysContainer_tlp2 
{
   
 public:
   
  TrigEFBphysContainer_tlp2(){}
  friend class TrigEFBphysContainerCnv_tlp2;

 private:

  std::vector<TrigEFBphysContainer_p1>       m_TrigEFBphysContainers ;
  std::vector<TrigEFBphys_p3>                m_EFBphys               ;
 
};//end of class definitions
 
#endif
