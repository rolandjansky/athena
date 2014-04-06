/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigTau
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauCnv_tlp2.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCNV_TLP2_H
#define TRIGPARTICLETPCNV_TRIGTAUCNV_TLP2_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigTau_tlp2.h"
#include "TrigParticleTPCnv/TrigTauCnv_p2.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

class TrigTauCnv_tlp2 : public AthenaPoolTopLevelTPConverter<TrigTauCnv_p2, TrigTau_tlp2>
{

 public:

  TrigTauCnv_tlp2();
  virtual ~TrigTauCnv_tlp2() {}

  virtual void setPStorage( TrigTau_tlp2 *storage );

 protected:

  //tp converters for ALL types used in TrigTau
  TrigTauCnv_p2      m_trigTauCnv;
  P4PtEtaPhiMCnv_p1  m_p4PtEtaPhiMCnv;

};


#endif
