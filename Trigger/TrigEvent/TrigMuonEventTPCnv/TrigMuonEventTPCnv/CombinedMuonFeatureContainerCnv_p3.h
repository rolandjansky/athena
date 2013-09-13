/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : CombinedMuonFeatureContainerCnv_p3
 *
 * @brief transient-persistent converter for CombinedMuonFatureContainer_p3
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureContainerCnv_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_P3_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< CombinedMuonFeatureContainer, CombinedMuonFeatureContainer_p3, CombinedMuonFeatureCnv_p3 > 
CombinedMuonFeatureContainerCnv_p3;

#endif
