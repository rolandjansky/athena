/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigTauCnv
 *
 * @brief transient persistent converter for TrigTau
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauCnv.h,v 1.5 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCNV_H

#include "TrigParticle/TrigTau.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigTau_tlp2;

// typedef to the latest persistent version
typedef TrigTau_tlp2  TrigTau_PERS;

class MsgStream;

class TrigTauCnv_impl;
class TrigTauCnv : public T_AthenaPoolCustomCnv<TrigTau, TrigTau_PERS > 
{

  friend class CnvFactory<TrigTauCnv>;

 protected:
  
  TrigTauCnv(ISvcLocator* svcloc);
  ~TrigTauCnv();

  TrigTau_PERS*  createPersistent(TrigTau* transCont);

  TrigTau*       createTransient ();

 private:
  TrigTauCnv_impl *m_impl;
};


#endif
