/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainer_tlp2
 *
 * @brief "top level" persistent partner for TrigL2BjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_TLP2_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_TLP2_H

#include "TrigParticleTPCnv/TrigL2BjetContainer_p2.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p2.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigL2BjetContainer_tlp2 {
   
 public:
   
  TrigL2BjetContainer_tlp2(){}
  friend class TrigL2BjetContainerCnv_tlp2;

 private:

  std::vector<TrigL2BjetContainer_p2> m_TrigL2BjetContainers;
  std::vector<TrigL2Bjet_p2>          m_L2Bjet;
  std::vector<P4PtEtaPhiM_p1>         m_p4PtEtaPhiM; 

};
 
#endif
