/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetContainer_tlp2
 *
 * @brief "top level" persistent partner for TrigEFBjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_TLP2_H
#define TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_TLP2_H

#include "TrigParticleTPCnv/TrigEFBjetContainer_p2.h"
#include "TrigParticleTPCnv/TrigEFBjet_p2.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigEFBjetContainer_tlp2 {
   
 public:
   
  TrigEFBjetContainer_tlp2(){}
  friend class TrigEFBjetContainerCnv_tlp2;

 private:

  std::vector<TrigEFBjetContainer_p2> m_TrigEFBjetContainers;
  std::vector<TrigEFBjet_p2>          m_EFBjet;
  std::vector<P4PtEtaPhiM_p1>         m_p4PtEtaPhiM; 
 
};
 
#endif
