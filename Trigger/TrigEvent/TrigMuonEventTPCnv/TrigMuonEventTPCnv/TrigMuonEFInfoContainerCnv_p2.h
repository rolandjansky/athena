/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainerCnv_p2
 *
 * @brief transient-persistent converter for TrigMuonEFInfoContainer_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p2.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

class MsgStream;

class TrigMuonEFInfoContainerCnv_p2 : public T_AthenaPoolTPPtrVectorCnv< TrigMuonEFInfoContainer, TrigMuonEFInfoContainer_p2, ITPConverterFor< TrigMuonEFInfo> >{

 public:
  TrigMuonEFInfoContainerCnv_p2() {}

};

#endif
