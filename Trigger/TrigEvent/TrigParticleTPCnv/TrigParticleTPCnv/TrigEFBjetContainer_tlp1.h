/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigEFBjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBjetContainer_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_TLP1_H
#define TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_TLP1_H

#include "TrigParticleTPCnv/TrigEFBjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBjet_p1.h"

class TrigEFBjetContainer_tlp1 
{
   
 public:
   
  TrigEFBjetContainer_tlp1(){}
  friend class TrigEFBjetContainerCnv_tlp1;

 private:

  std::vector<TrigEFBjetContainer_p1>       m_TrigEFBjetContainers ;
  std::vector<TrigEFBjet_p1>                m_EFBjet               ;
 
};//end of class definitions
 
#endif
