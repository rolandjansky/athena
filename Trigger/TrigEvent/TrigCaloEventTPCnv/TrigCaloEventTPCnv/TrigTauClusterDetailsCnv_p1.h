/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsCnv_p1
 *
 * @brief transient persistent converter for TrigTauClusterDetails
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCNV_P1_H

#include "TrigCaloEventTPCnv/TrigTauClusterDetails_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigTauClusterDetailsCnv_p1 : public T_AthenaPoolTPCnvBase<TrigTauClusterDetails, TrigTauClusterDetails_p1>
{
 public:
  
  TrigTauClusterDetailsCnv_p1() {}
  
  virtual void persToTrans( const TrigTauClusterDetails_p1 *persObj,
			    TrigTauClusterDetails    *transObj,
			    MsgStream            &log );
  virtual void transToPers( const TrigTauClusterDetails    *transObj,
			    TrigTauClusterDetails_p1 *persObj,
			    MsgStream            &log );

};


#endif
