/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigTauContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauContainer_tlp1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCONTAINER_TLP1_H
#define TRIGPARTICLETPCNV_TRIGTAUCONTAINER_TLP1_H

#include "TrigParticleTPCnv/TrigTauContainer_p1.h"
#include "TrigParticleTPCnv/TrigTau_p2.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigTauContainer_tlp1 
{
   
 public:
   
  TrigTauContainer_tlp1(){}
  friend class TrigTauContainerCnv_tlp1;

 private:

  std::vector<TrigTauContainer_p1>       m_TrigTauContainers ;
  std::vector<TrigTau_p2>                m_Tau               ;
  std::vector<P4PtEtaPhiM_p1>               m_P4PtEtaPhiM          ;

};//end of class definitions
 
#endif
