/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_CLUSTER_CNV_P1_H
#define PIXEL_CLUSTER_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CscStripPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/CscStripPrepData.h"
#include "CscStripPrepData_p1.h"

#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"

class MsgStream;

class CscStripPrepDataCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::CscStripPrepData, Muon::CscStripPrepData_p1 >
{
    public:
  CscStripPrepDataCnv_p1() : m_prdCnv(0) {}

  void persToTrans( const Muon::CscStripPrepData_p1 *persObj,
                    Muon::CscStripPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::CscStripPrepData    *transObj,
                    Muon::CscStripPrepData_p1 *persObj,
                    MsgStream                &log );

protected:        
  PrepRawDataCnv_p1       *m_prdCnv;
};

#endif // PIXEL_CLUSTER_CNV_P1_H
