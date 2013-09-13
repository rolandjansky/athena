/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonClusterFeatureContainerCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonClusterFatureContainer_p1
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch>  - U. of Washington
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeatureContainerCnv_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/TrigMuonClusterFeatureContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_p1.h"

class TrigMuonClusterFeatureContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigMuonClusterFeatureContainer, TrigMuonClusterFeatureContainer_p1, ITPConverterFor<TrigMuonClusterFeature> >

{
 
 public:
  TrigMuonClusterFeatureContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
