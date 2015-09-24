/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_WIDTH_CNV_P1_H
#define SI_WIDTH_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SiWidthCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SiWidth.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidth_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

class MsgStream;

class SiWidthCnv_p1
   : public T_AthenaPoolTPCnvBase<InDet::SiWidth, InDet::SiWidth_p1>
{
public:
  SiWidthCnv_p1() : m_localPosCnv(0) {}

  void persToTrans( const InDet::SiWidth_p1 *, InDet::SiWidth *, MsgStream & );
  void transToPers( const InDet::SiWidth *, InDet::SiWidth_p1 *, MsgStream & );

private:
  LocalPositionCnv_p1	*m_localPosCnv;
        
};

#endif // SI_WIDTH_CNV_P1_H
