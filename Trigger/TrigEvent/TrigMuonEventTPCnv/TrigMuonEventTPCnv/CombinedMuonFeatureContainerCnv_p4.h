/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : CombinedMuonFeatureContainerCnv_p4
 *
 * @brief transient-persistent converter for CombinedMuonFatureContainer_p4
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureContainerCnv_p4.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_P4_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_P4_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p4.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p4.h"

typedef T_AthenaPoolTPCnvVector< CombinedMuonFeatureContainer, CombinedMuonFeatureContainer_p4, CombinedMuonFeatureCnv_p4 > 
CombinedMuonFeatureContainerCnv_p4;

#endif
