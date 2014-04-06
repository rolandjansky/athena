/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainer_tlp2
 *
 * @brief "top level" persistent partner for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainer_tlp2.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_TLP2_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_TLP2_H

#include "TrigParticleTPCnv/TrigPhotonContainer_p2.h"
#include "TrigParticleTPCnv/TrigPhoton_p2.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigPhotonContainer_tlp2 
{
   
 public:
   
  TrigPhotonContainer_tlp2(){}
  friend class TrigPhotonContainerCnv_tlp2;

 private:

  std::vector<TrigPhotonContainer_p2>       m_TrigPhotonContainers ;
  std::vector<TrigPhoton_p2>                m_Photon               ;
  std::vector<P4PtEtaPhiM_p1>               m_P4PtEtaPhiM          ;

};//end of class definitions
 
#endif
