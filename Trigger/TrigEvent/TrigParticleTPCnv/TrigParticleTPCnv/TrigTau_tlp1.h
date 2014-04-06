/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTau_tlp1
 *
 * @brief "top level" persistent partner for TrigTau
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTau_tlp1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAU_TLP1_H
#define TRIGPARTICLETPCNV_TRIGTAU_TLP1_H

#include "TrigParticleTPCnv/TrigTau_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigTau_tlp1
{
 public:
  
  TrigTau_tlp1() {}
  friend class TrigTauCnv_tlp1;
  
 private:
  
  std::vector< TrigTau_p1 >                m_trigTau;
  std::vector< P4PtEtaPhiM_p1 >            m_p4PtEtaPhiM;

};

#endif
