/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureContainerCnv_p3
 *
 * @brief transient-persistent converter for IsoMuonFatureContainer_p3
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureContainerCnv_p3.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_CNV_P3_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< IsoMuonFeatureContainer, IsoMuonFeatureContainer_p3, IsoMuonFeatureCnv_p3 > IsoMuonFeatureContainerCnv_p3;

#endif
