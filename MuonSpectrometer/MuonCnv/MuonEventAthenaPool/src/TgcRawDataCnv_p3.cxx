/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/TgcRawData.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "TgcRawDataCnv_p3.h"

void TgcRawDataCnv_p3::persToTrans(const TgcRawData_p3* persObj,
                                   TgcRawData* transObj,
                                   MsgStream &/*log*/)
{
    transObj->m_bcTag         = persObj->m_bcTag;
    transObj->m_subDetectorId = persObj->m_subDetectorId;
    transObj->m_rodId         = persObj->m_rodId;
    transObj->m_sswId         = persObj->m_sswId;
    transObj->m_slbId         = persObj->m_slbId;
    transObj->m_l1Id          = persObj->m_l1Id;
    transObj->m_bcId          = persObj->m_bcId;
    transObj->m_slbType       = (TgcRawData::SlbType)(persObj->m_slbType);
    transObj->m_bitpos        = persObj->m_bitpos;
    transObj->m_tracklet      = persObj->m_tracklet;
    transObj->m_adjacent      = persObj->m_adjacent;
    transObj->m_type          = (TgcRawData::DataType)(persObj->m_type);
    transObj->m_forward       = persObj->m_forward;
    transObj->m_index         = persObj->m_index;
    transObj->m_pos           = persObj->m_pos;
    transObj->m_delta         = persObj->m_delta;
    transObj->m_segment       = persObj->m_segment;
    transObj->m_subMatrix     = persObj->m_subMatrix;
    transObj->m_sector        = persObj->m_sector;
    transObj->m_chip          = persObj->m_chip;
    transObj->m_hipt          = persObj->m_hipt;
    transObj->m_hitId         = persObj->m_hitId;
    transObj->m_hsub          = persObj->m_hsub;
    transObj->m_strip         = persObj->m_strip;
    transObj->m_tile          = persObj->m_tile;
    transObj->m_cand3plus     = persObj->m_cand3plus;
    transObj->m_muplus        = persObj->m_muplus;
    transObj->m_threshold     = persObj->m_threshold;
    transObj->m_overlap       = persObj->m_overlap;
    transObj->m_veto          = persObj->m_veto;
    transObj->m_roi           = persObj->m_roi;
}

void TgcRawDataCnv_p3::transToPers(const TgcRawData* transObj,
                                   TgcRawData_p3* persObj,
                                   MsgStream &) 
{
    persObj->m_bcTag         = transObj->m_bcTag;
    persObj->m_subDetectorId = transObj->m_subDetectorId;
    persObj->m_rodId         = transObj->m_rodId;
    persObj->m_sswId         = transObj->m_sswId;
    persObj->m_slbId         = transObj->m_slbId;
    persObj->m_l1Id          = transObj->m_l1Id;
    persObj->m_bcId          = transObj->m_bcId;
    persObj->m_slbType       = (uint16_t)(transObj->m_slbType);
    persObj->m_bitpos        = transObj->m_bitpos;
    persObj->m_tracklet      = transObj->m_tracklet;
    persObj->m_adjacent      = transObj->m_adjacent;
    persObj->m_type          = (uint16_t)(transObj->m_type);
    persObj->m_forward       = transObj->m_forward;
    persObj->m_index         = transObj->m_index;
    persObj->m_pos           = transObj->m_pos;
    persObj->m_delta         = transObj->m_delta;
    persObj->m_segment       = transObj->m_segment;
    persObj->m_subMatrix     = transObj->m_subMatrix;
    persObj->m_sector        = transObj->m_sector;
    persObj->m_chip          = transObj->m_chip;
    persObj->m_hipt          = transObj->m_hipt;
    persObj->m_hitId         = transObj->m_hitId;
    persObj->m_hsub          = transObj->m_hsub;
    persObj->m_strip         = transObj->m_strip;
    persObj->m_tile          = transObj->m_tile;
    persObj->m_cand3plus     = transObj->m_cand3plus;
    persObj->m_muplus        = transObj->m_muplus;
    persObj->m_threshold     = transObj->m_threshold;
    persObj->m_overlap       = transObj->m_overlap;
    persObj->m_veto          = transObj->m_veto;
    persObj->m_roi           = transObj->m_roi;
}

