/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFT_CIRCLE_CNV_P1_H
#define TRT_DRIFT_CIRCLE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TRT_DriftCircle_p1.h"

#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"

class MsgStream;

class TRT_DriftCircleCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData,
				       InDet::TRT_DriftCircle,
				       InDet::TRT_DriftCircle_p1>
{
public:
  TRT_DriftCircleCnv_p1() : m_rawDataCnv(0) {}
  
  void persToTrans( const InDet::TRT_DriftCircle_p1 *persObj,
		    InDet::TRT_DriftCircle    *transObj,
		    MsgStream                   &log );
  void transToPers( const InDet::TRT_DriftCircle    *transObj,
		    InDet::TRT_DriftCircle_p1 *persObj,
		    MsgStream                   &log );
        
private:
  ITPConverterFor<Trk::PrepRawData>	*m_rawDataCnv;  
};

#endif // TRT_DRIFT_CIRCLE_CNV_P1_H
