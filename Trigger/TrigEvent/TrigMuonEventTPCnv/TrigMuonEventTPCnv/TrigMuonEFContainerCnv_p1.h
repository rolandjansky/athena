/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFContainerCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonContainer_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEFContainerCnv_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p1.h"

class MsgStream;
 
class TrigMuonEFContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigMuonEFContainer, TrigMuonEFContainer_p1, TrigMuonEFCnv_p1 >

{
 
 public:
  TrigMuonEFContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
