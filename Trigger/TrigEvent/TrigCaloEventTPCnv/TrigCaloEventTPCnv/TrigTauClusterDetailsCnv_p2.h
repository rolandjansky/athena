/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsCnv_p2
 *
 * @brief "top level" transient-persistent converter for TrigTauClusterDetails
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
**********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCNV_P2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_p2.h"

class MsgStream;

class TrigTauClusterDetailsCnv_p2  : public T_AthenaPoolTPCnvBase<TrigTauClusterDetails, TrigTauClusterDetails_p2>  {
    
  public:
    TrigTauClusterDetailsCnv_p2() {}

    virtual void persToTrans(const TrigTauClusterDetails_p2* persObj,  TrigTauClusterDetails* transObj,   MsgStream &log );
    virtual void transToPers(const TrigTauClusterDetails* transObj, TrigTauClusterDetails_p2* persObj, MsgStream &log );

 protected:

};
#endif
