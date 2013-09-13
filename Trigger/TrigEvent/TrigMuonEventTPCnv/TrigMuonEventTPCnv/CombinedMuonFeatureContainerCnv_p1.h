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
 * $Id: CombinedMuonFeatureContainerCnv_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p1.h"

class CombinedMuonFeatureContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CombinedMuonFeatureContainer, CombinedMuonFeatureContainer_p1, ITPConverterFor<CombinedMuonFeature> >

{
 
 public:
  CombinedMuonFeatureContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
