/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TileTrackMuFeatureCnv_p1
 *
 * @brief persistent version of TileTrackMuFeatureCnv
 * 
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 *
 * File and Version Information:
 *  
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_P2_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURECNV_P2_H

#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"

#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"


class MsgStream;

class TileTrackMuFeatureCnv_p2  : public T_AthenaPoolTPCnvBase<TileTrackMuFeature, TileTrackMuFeature_p2>  {
    
  public:
    TileTrackMuFeatureCnv_p2() {}

    virtual void persToTrans(const TileTrackMuFeature_p2* persObj,  TileTrackMuFeature* transObj,   MsgStream &log );
    virtual void transToPers(const TileTrackMuFeature* transObj, TileTrackMuFeature_p2* persObj, MsgStream &log );

 protected:
 
 ElementLinkCnv_p1< ElementLink<TileMuFeatureContainer> >   m_ELink_TileMuCnv;
 ElementLinkCnv_p1< ElementLink<TrigInDetTrackCollection> > m_ELink_IDTkCnv; 

  };


#endif
