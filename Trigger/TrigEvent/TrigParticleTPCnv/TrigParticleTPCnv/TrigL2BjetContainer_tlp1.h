/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigL2BjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BjetContainer_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_TLP1_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_TLP1_H

#include "TrigParticleTPCnv/TrigL2BjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p1.h"

class TrigL2BjetContainer_tlp1 
{
   
 public:
   
  TrigL2BjetContainer_tlp1(){}
  friend class TrigL2BjetContainerCnv_tlp1;

 private:

  std::vector<TrigL2BjetContainer_p1>       m_TrigL2BjetContainers ;
  std::vector<TrigL2Bjet_p1>                m_L2Bjet               ;
 
};//end of class definitions
 
#endif
