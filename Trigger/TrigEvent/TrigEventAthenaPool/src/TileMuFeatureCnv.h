/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TileMuFeatureCnv 
 *
 * @brief transient persistent converter for TileMuFeatureCnv
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 *
 * File and Version Information:
 * $Id: TileMuFeatureCnv.h,v 1.2 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/

#ifndef TRIGEVENTATHENAPOOL_TileMuFeatureCnv_H
#define TRIGEVENTATHENAPOOL_TileMuFeatureCnv_H

#include "TrigMuonEvent/TileMuFeature.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TileMuFeature_p1;
class TileMuFeatureCnv_p1;


class MsgStream;

// typedef to the latest persistent version
typedef TileMuFeature_p1  TileMuFeature_PERS;

typedef T_AthenaPoolCustomCnv<TileMuFeature, TileMuFeature_PERS > TileMuFeatureCnvBase; 

class TileMuFeatureCnv : public TileMuFeatureCnvBase {

  friend class CnvFactory<TileMuFeatureCnv>;

 protected:
  TileMuFeatureCnv(ISvcLocator *svcloc);
  ~TileMuFeatureCnv();

  virtual TileMuFeature_PERS  *createPersistent(TileMuFeature *transObj);
  virtual TileMuFeature       *createTransient();
};


#endif

