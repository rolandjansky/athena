/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronContainer_tlp1
 *
 * @brief "top level" persistent partner for TrigElectronContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigElectronContainer_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINER_TLP1_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINER_TLP1_H

#include "TrigParticleTPCnv/TrigElectronContainer_p1.h"
#include "TrigParticleTPCnv/TrigElectron_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigElectronContainer_tlp1
{
 public:
  
  TrigElectronContainer_tlp1() {}
  friend class TrigElectronContainerCnv_tlp1;
  
 private:
  
  std::vector< TrigElectronContainer_p1 >   m_trigElectronContainerVec;
  std::vector< TrigElectron_p1 >            m_trigElectronVec;
  std::vector< P4PtEtaPhiM_p1 >             m_p4PtEtaPhiMVec;

};

#endif
