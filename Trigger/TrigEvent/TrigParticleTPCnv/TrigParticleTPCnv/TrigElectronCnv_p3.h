/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronCnv_p3
 *
 * @brief transient persistent converter for TrigElectron
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectronCnv_p3.h 358100 2011-04-12 12:29:54Z salvator $ 
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCNV_P3_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCNV_P3_H

// transient class
#include "TrigParticle/TrigElectron.h"
// persistent class
#include "TrigParticleTPCnv/TrigElectron_p3.h"
// base-class converter
// base class
#include "FourMom/P4PtEtaPhiM.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
// converter for ElementLinks
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
// converter for track and cluster objects (pointed to through ElementLinks)
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
// T/P converter base class
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigElectronCnv_p3 : public T_AthenaPoolTPCnvBase<TrigElectron, TrigElectron_p3>
{
public:

  TrigElectronCnv_p3() {}

  virtual void persToTrans(const TrigElectron_p3* persObj, 
			   TrigElectron* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigElectron* transObj, 
			   TrigElectron_p3* persObj, 
			   MsgStream &log);
 protected:

  // to persistify ElementLinks to cluster and track 
  ElementLinkCnv_p3< ElementLink<TrigEMClusterContainer> >   m_ELinkEMclusterCnv ;
  ElementLinkCnv_p3< ElementLink<TrigInDetTrackCollection> > m_ELinkIDtrackCnv   ;

  // 4-momentum base class
  P4PtEtaPhiMCnv_p1 m_p4PtEtaPhiMCnv ;

};


#endif
