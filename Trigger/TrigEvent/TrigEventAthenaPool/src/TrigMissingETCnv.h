/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigMissingETCnv
 *
 * @brief transient persistent converter for TrigMissingET
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Diego Casadei    <Diego.Casadei@cern.ch>    - NYU
 *
 * File and Version Information:
 * $Id: TrigMissingETCnv.h,v 1.4 2009-04-02 20:51:02 salvator Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGMISSINGETCNV_H
#define TRIGEVENTATHENAPOOL_TRIGMISSINGETCNV_H

#include "TrigMissingEtEvent/TrigMissingET.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"
class TrigMissingET_p2;

// typedef to the latest persistent version

/* typedef TrigMissingET_p1  TrigMissingET_PERS; */
typedef TrigMissingET_p2  TrigMissingET_PERS;

typedef T_AthenaPoolCustomCnv<TrigMissingET, TrigMissingET_PERS > TrigMissingETCnvBase; 

class TrigMissingETCnv : public TrigMissingETCnvBase {

  friend class CnvFactory<TrigMissingETCnv>;

 protected:
  TrigMissingETCnv(ISvcLocator *svcloc);
  ~TrigMissingETCnv();

  virtual TrigMissingET_PERS  *createPersistent(TrigMissingET *transObj);
  virtual TrigMissingET       *createTransient();
};


#endif
