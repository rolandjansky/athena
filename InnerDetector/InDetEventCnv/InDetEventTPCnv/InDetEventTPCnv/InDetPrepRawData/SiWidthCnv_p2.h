/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_WIDTH_CNV_P2_H
#define SI_WIDTH_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   SiWidthCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SiWidth.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidth_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

class MsgStream;

namespace InDet
{

    class SiWidthCnv_p2
       : public T_AthenaPoolTPCnvBase<InDet::SiWidth, InDet::SiWidth_p2>
    {
    public:
      SiWidthCnv_p2()  {}

      void persToTrans( const InDet::SiWidth_p2 *, InDet::SiWidth *, MsgStream & );
      void transToPers( const InDet::SiWidth *, InDet::SiWidth_p2 *, MsgStream & );

    private:
      LocalPositionCnv_p1	m_localPosCnv;
            
    };
}
#endif // SI_WIDTH_CNV_P1_H
