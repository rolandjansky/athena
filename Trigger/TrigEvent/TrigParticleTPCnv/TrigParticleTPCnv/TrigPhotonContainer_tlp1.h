/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainer_tlp1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_TLP1_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_TLP1_H

#include "TrigParticleTPCnv/TrigPhotonContainer_p1.h"
#include "TrigParticleTPCnv/TrigPhoton_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigPhotonContainer_tlp1 
{
   
 public:
   
  TrigPhotonContainer_tlp1(){}
  friend class TrigPhotonContainerCnv_tlp1;

 private:

  std::vector<TrigPhotonContainer_p1>       m_TrigPhotonContainers ;
  std::vector<TrigPhoton_p1>                m_Photon               ;
  std::vector<TrigEMCluster_p1>             m_EMcluster            ;
  std::vector<TrigCaloCluster_p1>           m_CaloCluster          ;
  std::vector<P4PtEtaPhiM_p1>               m_P4PtEtaPhiM          ;

};//end of class definitions
 
#endif
