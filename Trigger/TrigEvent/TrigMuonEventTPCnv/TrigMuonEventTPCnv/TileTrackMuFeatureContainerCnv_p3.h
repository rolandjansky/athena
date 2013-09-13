/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileTrackMuContainerCnv_p3
 *
 * @brief transient-persistent converter for TileTrackMuContainer_p3
 *
 * @author Heuijin Lim (hjlim@hep.anl.gov)
 * @Created: Jul. 7. 2008
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_CNV_P3_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< TileTrackMuFeatureContainer, TileTrackMuFeatureContainer_p3, TileTrackMuFeatureCnv_p3 > TileTrackMuFeatureContainerCnv_p3;
 
#endif
