/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFT_CIRCLE_CNV_P2_H
#define TRT_DRIFT_CIRCLE_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TRT_DriftCircle_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TRT_DriftCircleCnv_p2
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData,
				       InDet::TRT_DriftCircle,
				       InDet::TRT_DriftCircle_p2> 
{
public:
  TRT_DriftCircleCnv_p2()  {}

  InDet::TRT_DriftCircle
  createTRT_DriftCircle( const InDet::TRT_DriftCircle_p2 *persObj,
                         Identifier clusId,
                         std::vector<Identifier>&& rdoList,
                         const InDetDD::TRT_BaseElement* detEl,
                         MsgStream &log);

  void persToTrans( const InDet::TRT_DriftCircle_p2 *persObj,
		    InDet::TRT_DriftCircle    *transObj,
		    MsgStream                   &log );
  void transToPers( const InDet::TRT_DriftCircle    *transObj,
		    InDet::TRT_DriftCircle_p2 *persObj,
		    MsgStream                   &log );
        

};

#endif // TRT_DRIFT_CIRCLE_CNV_P2_H
