/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : MuonFeatureCnv
 *
 * @brief transient persistent converter for MuonFeature
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeatureCnv.h,v 1.3 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/

#ifndef TRIGEVENTATHENAPOOL_MUONFEATURECNV_H
#define TRIGEVENTATHENAPOOL_MUONFEATURECNV_H

#include "TrigMuonEvent/MuonFeature.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MuonFeature_p2;

class MsgStream;

// typedef to the latest top level persistent version
// typedef MuonFeature_p1  MuonFeature_PERS;
typedef MuonFeature_p2  MuonFeature_PERS;

typedef T_AthenaPoolCustomCnv<MuonFeature, MuonFeature_PERS > MuonFeatureCnvBase; 

class MuonFeatureCnv : public MuonFeatureCnvBase {

  friend class CnvFactory<MuonFeatureCnv>;

 protected:
  MuonFeatureCnv(ISvcLocator *svcloc);
  ~MuonFeatureCnv();

  virtual MuonFeature_PERS  *createPersistent(MuonFeature *transObj);
  virtual MuonFeature       *createTransient();
};


#endif
