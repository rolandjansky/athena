/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TileTrackMuFeatureCnv_p3
 *
 * @brief persistent version of TileTrackMuFeatureCnv
 * 
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 *
 * File and Version Information:
 *  
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_P3_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_P3_H

#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"

#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p3.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"


class MsgStream;

class TileTrackMuFeatureCnv_p3  : public T_AthenaPoolTPCnvBase<TileTrackMuFeature, TileTrackMuFeature_p3>  {
    
  public:
    TileTrackMuFeatureCnv_p3() {}

    virtual void persToTrans(const TileTrackMuFeature_p3* persObj,  TileTrackMuFeature* transObj,   MsgStream &log );
    virtual void transToPers(const TileTrackMuFeature* transObj, TileTrackMuFeature_p3* persObj, MsgStream &log );

 protected:
 
 ElementLinkCnv_p3< ElementLink<TileMuFeatureContainer> >   m_ELink_TileMuCnv;
 ElementLinkCnv_p3< ElementLink<TrigInDetTrackCollection> > m_ELink_IDTkCnv; 

  };


#endif
