/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileTrackMuFeatureCnv_tlp1
 *
 * @brief "top level" persistent partner for TileTrackMuFeatureCnv_
 *
 * @author  Heuijin Lim <hjlim@hep.anl.gov>
 *
 * File and Version Information:
 * 
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_TLP1_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEventTPCnv/TileTrackMuFeature_tlp1.h"

#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"

class TileTrackMuFeatureCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TileTrackMuFeatureCnv_p1, TileTrackMuFeature_tlp1>  {

 public:
  TileTrackMuFeatureCnv_tlp1();
  virtual ~TileTrackMuFeatureCnv_tlp1() {}

  virtual void setPStorage(TileTrackMuFeature_tlp1 *storage);

  
 protected:

  TileMuFeatureCnv_p1            m_TileMuCnv; 
  TrigInDetTrackCnv_p1           m_IDTkCnv;
  TrigInDetTrackFitParCnv_p1     m_IDTkFitParCnv;
};

#endif
