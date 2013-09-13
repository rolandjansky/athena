/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileTrackMuFeatureContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TileTrackMuFeatureContainer_tlp1
 *
 * @author Heuijin Lim (hjlim@hep.anl.gov)
 * @Created: Jul. 7. 2008
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_p1.h"   
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p1.h"   
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p2.h"
 
class TileTrackMuFeatureContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TileTrackMuFeatureContainerCnv_p1, TileTrackMuFeatureContainer_tlp1 >
{
 
 public:
 
  TileTrackMuFeatureContainerCnv_tlp1();
  virtual ~TileTrackMuFeatureContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TileTrackMuFeatureContainer_tlp1 *storage );
 
 protected: 
  TileTrackMuFeatureContainerCnv_p1    m_TileTrackMuFeatureContainerCnv;
  TileTrackMuFeatureCnv_p1             m_TileTrackMuFeatureCnv;
  TileTrackMuFeatureCnv_p2	       m_TileTrackMuFeatureCnv_p2; 	
}; //end of class definitions


#endif
