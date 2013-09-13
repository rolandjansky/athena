/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileTrackMuFeatureCnv_p1 
 *
 * @brief persistent version of TileTrackMuFeatureCnv
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 *
 * File and Version Information:
 *  
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_P1_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"

class MsgStream;

class TileTrackMuFeatureCnv_p1  : public T_AthenaPoolTPCnvBase<TileTrackMuFeature, TileTrackMuFeature_p1>  {
    
  public:
    TileTrackMuFeatureCnv_p1() : m_TileMuCnv(0), m_IDTkCnv(0) {}

    virtual void persToTrans(const TileTrackMuFeature_p1* persObj,  TileTrackMuFeature* transObj,   MsgStream &log );
    virtual void transToPers(const TileTrackMuFeature* transObj, TileTrackMuFeature_p1* persObj, MsgStream &log );

 protected:
    TileMuFeatureCnv_p1*    m_TileMuCnv;
    TrigInDetTrackCnv_p1*   m_IDTkCnv;

  };


#endif
