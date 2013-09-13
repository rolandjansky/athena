/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainerCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonEFInfoContainer_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p1.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

class MsgStream;

class TrigMuonEFInfoContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigMuonEFInfoContainer, TrigMuonEFInfoContainer_p1, ITPConverterFor< TrigMuonEFInfo> >

{

 public:
  TrigMuonEFInfoContainerCnv_p1() {}

}; //end of class definitions

#endif
