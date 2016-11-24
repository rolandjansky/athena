/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcRawData.h"
#include "GaudiKernel/MsgStream.h"
#include "TgcRawDataCnv_p2.h"

void TgcRawDataCnv_p2::persToTrans(const TgcRawData_p2* persObj,
                                   TgcRawData* transObj,
                                   MsgStream &/*log*/)
{
    
	transObj-> m_bcTag = persObj->m_bcTag;
	transObj-> m_subDetectorId = persObj->m_subDetectorId;
	transObj-> m_rodId = persObj->m_rodId;
	transObj-> m_l1Id = persObj->m_l1Id;
	transObj-> m_bcId = persObj->m_bcId;
	transObj-> m_strip = persObj->m_strip;
	transObj-> m_forward = persObj->m_forward;
	transObj-> m_sector = persObj->m_sector;
	transObj-> m_chip = persObj->m_chip;
	transObj-> m_index = persObj->m_index;
	transObj-> m_hipt = persObj->m_hipt;
	transObj-> m_hitId = persObj->m_hitId;
	transObj-> m_hsub = persObj->m_hsub;
	transObj-> m_delta = persObj->m_delta;
	transObj-> m_inner = 0;
	// m_inner
	// *transObj = TgcRawData (persObj->m_bcTag,
	//                             persObj->m_subDetectorId,
	//                             persObj->m_rodId,
	//                             persObj->m_l1Id,
	//                             persObj->m_bcId,
	//                             persObj->m_strip,
	//                             persObj->m_forward,
	//                             persObj->m_sector,
	//                             persObj->m_chip,
	//                             persObj->m_index,
	//                             persObj->m_hipt,
	//                             persObj->m_hitId,
	//                             persObj->m_hsub,
	//                             persObj->m_delta,
	//                             0);

    transObj->m_sswId         = persObj->m_sswId;
    transObj->m_slbId         = persObj->m_slbId;
    transObj->m_slbType       = (TgcRawData::SlbType)(persObj->m_slbType);
    transObj->m_bitpos        = persObj->m_bitpos;
    transObj->m_tracklet      = persObj->m_tracklet;
    transObj->m_adjacent      = persObj->m_adjacent;
    transObj->m_type          = (TgcRawData::DataType)(persObj->m_type);
    transObj->m_pos           = persObj->m_pos;
    transObj->m_segment       = persObj->m_segment;
    transObj->m_subMatrix     = persObj->m_subMatrix;
    transObj->m_cand3plus     = persObj->m_cand3plus;
    transObj->m_muplus        = persObj->m_muplus;
    transObj->m_threshold     = persObj->m_threshold;
    transObj->m_overlap       = persObj->m_overlap;
    transObj->m_roi           = persObj->m_roi;
}

void TgcRawDataCnv_p2::transToPers(const TgcRawData* transObj,
                                   TgcRawData_p2* persObj,
                                   MsgStream &) 
{
    persObj->m_bcTag         = transObj->bcTag();
    persObj->m_subDetectorId = transObj->subDetectorId();
    persObj->m_rodId         = transObj->rodId();
    persObj->m_sswId         = transObj->sswId();
    persObj->m_slbId         = transObj->slbId();
    persObj->m_l1Id          = transObj->l1Id();
    persObj->m_bcId          = transObj->bcId();
    persObj->m_slbType       = (uint16_t)(transObj->slbType());
    persObj->m_bitpos        = transObj->bitpos();
    persObj->m_tracklet      = transObj->tracklet();
    persObj->m_adjacent      = transObj->isAdjacent();
    persObj->m_type          = (uint16_t)(transObj->type());
    persObj->m_forward       = transObj->isForward();
    persObj->m_index         = transObj->index();
    persObj->m_pos           = transObj->position();
    persObj->m_delta         = transObj->delta();
    persObj->m_segment       = transObj->segment();
    persObj->m_subMatrix     = transObj->subMatrix();
    persObj->m_sector        = transObj->sector();
    persObj->m_chip          = transObj->chip();
    persObj->m_hipt          = transObj->isHipt();
    persObj->m_hitId         = transObj->hitId();
    persObj->m_hsub          = transObj->hsub();
    persObj->m_strip         = transObj->isStrip();
    persObj->m_cand3plus     = transObj->cand3plus();
    persObj->m_muplus        = transObj->isMuplus();
    persObj->m_threshold     = transObj->threshold();
    persObj->m_overlap       = transObj->isOverlap();
    persObj->m_roi           = transObj->roi();
}

