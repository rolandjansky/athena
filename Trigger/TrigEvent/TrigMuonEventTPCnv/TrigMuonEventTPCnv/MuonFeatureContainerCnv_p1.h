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
 * $Id: MuonFeatureContainerCnv_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p2.h"

class MuonFeatureContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< MuonFeatureContainer, MuonFeatureContainer_p1, ITPConverterFor<MuonFeature> >

{
 
 public:
  MuonFeatureContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
