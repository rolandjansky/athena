/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronCnv_p2
 *
 * @brief transient persistent converter for TrigElectron
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectron_p2.h,v 1.2 2009-06-14 14:23:00 jgoncalo Exp $ 
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCNV_P2_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCNV_P2_H

// transient class
#include "TrigParticle/TrigElectron.h"
// persistent class
#include "TrigParticleTPCnv/TrigElectron_p2.h"
// base-class converter
// base class
#include "FourMom/P4PtEtaPhiM.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
// converter for ElementLinks
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
// converter for track and cluster objects (pointed to through ElementLinks)
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
// T/P converter base class
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigElectronCnv_p2 : public T_AthenaPoolTPCnvBase<TrigElectron, TrigElectron_p2>
{
public:

  TrigElectronCnv_p2() : m_p4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigElectron_p2* persObj, 
			   TrigElectron* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigElectron* transObj, 
			   TrigElectron_p2* persObj, 
			   MsgStream &log);
 protected:

  // to persistify ElementLinks to cluster and track 
  ElementLinkCnv_p1< ElementLink<TrigEMClusterContainer> >   m_ELinkEMclusterCnv ;
  ElementLinkCnv_p1< ElementLink<TrigInDetTrackCollection> > m_ELinkIDtrackCnv   ;

  // 4-momentum base class
  P4PtEtaPhiMCnv_p1* m_p4PtEtaPhiMCnv ;

};


#endif
