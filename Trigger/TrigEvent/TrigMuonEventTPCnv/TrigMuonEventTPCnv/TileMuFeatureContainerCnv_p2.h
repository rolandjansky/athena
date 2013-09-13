/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeatureContainerCnv_p2
 *
 * @brief transient-persistent converter for TileMuFeatureContainer_p2
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Jul. 3. 2008 
 * 
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_CNV_P2_H
#define  TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_CNV_P2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p2.h"   
 
typedef T_AthenaPoolTPCnvVector< TileMuFeatureContainer, TileMuFeatureContainer_p2, TileMuFeatureCnv_p2 > TileMuFeatureContainerCnv_p2;

#endif
