/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileTrackMuFeatureContainer_tlp1
 *
 * @brief "top-level" persistent partner for TileTrackMuFeatureContainer
 *
 * @author Heuijin Lim (hjlim@hep.anl.gov)
 * @Created: Jul. 7. 2008
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p2.h"

#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"

class TileTrackMuFeatureContainer_tlp1 
{
   
 public:
   
  TileTrackMuFeatureContainer_tlp1(){}
 
  std::vector<TileTrackMuFeatureContainer_p1>       m_TileTrackMuFeatureContainerVec;
  std::vector<TileTrackMuFeature_p1>                m_TileTrackMuFeatureVec;
  std::vector<TileTrackMuFeature_p2>		    m_TileTrackMuFeatureVec_p2;     
};//end of class definitions
 
#endif
