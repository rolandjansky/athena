/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureContainerCnv_p3
 *
 * @brief transient-persistent converter for MuonFatureContainer_p3
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: MuonFeatureContainerCnv_p3.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_P3_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< MuonFeatureContainer, MuonFeatureContainer_p3, MuonFeatureCnv_p3 > MuonFeatureContainerCnv_p3;

#endif
