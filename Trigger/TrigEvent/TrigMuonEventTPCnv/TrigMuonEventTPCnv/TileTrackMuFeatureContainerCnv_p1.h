/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileTrackMuContainerCnv_p1
 *
 * @brief transient-persistent converter for TileTrackMuContainer_p1
 *
 * @author Heuijin Lim (hjlim@hep.anl.gov)
 * @Created: Jul. 7. 2008
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p1.h"

class TileTrackMuFeatureContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TileTrackMuFeatureContainer, TileTrackMuFeatureContainer_p1, ITPConverterFor<TileTrackMuFeature> >

{
 
 public:
  TileTrackMuFeatureContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
