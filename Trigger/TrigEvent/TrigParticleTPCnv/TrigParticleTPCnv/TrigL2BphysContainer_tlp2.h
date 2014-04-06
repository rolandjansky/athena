/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BphysContainer_tlp2
 *
 * @brief "top level" persistent partner for TrigL2BphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysContainer_tlp1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_TLP2_H
#define TRIGPARTICLETPCNV_TRIGL2BPHYSCONTAINER_TLP2_H

#include "TrigParticleTPCnv/TrigL2BphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p3.h"

class TrigL2BphysContainer_tlp2 
{
   
 public:
   
  TrigL2BphysContainer_tlp2(){}
  friend class TrigL2BphysContainerCnv_tlp2;

 private:

  std::vector<TrigL2BphysContainer_p1>       m_TrigL2BphysContainers ;
  std::vector<TrigL2Bphys_p3>                m_L2Bphys               ;
 
};//end of class definitions
 
#endif
