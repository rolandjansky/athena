/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronCnv_p1
 *
 * @brief transient persistent converter for TrigElectron
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigElectronCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCNV_P1_H

#include "TrigParticle/TrigElectron.h"
#include "TrigParticleTPCnv/TrigElectron_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigElectronCnv_p1 : public T_AthenaPoolTPCnvBase<TrigElectron, TrigElectron_p1>
{
public:

  TrigElectronCnv_p1() : m_p4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigElectron_p1* persObj, 
			   TrigElectron* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigElectron* transObj, 
			   TrigElectron_p1* persObj, 
			   MsgStream &log);
 protected:

  P4PtEtaPhiMCnv_p1*  m_p4PtEtaPhiMCnv;

};


#endif
