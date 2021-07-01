/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcL1RawData.h"
#include "GaudiKernel/MsgStream.h"

void TgcL1RawData::clear(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t srodId,
                       uint16_t l1Id,
                       uint16_t bcId)
{
    m_bcTag = bcTag;
    m_subDetectorId = subDetectorId;
    m_srodId = srodId;
    m_l1Id = l1Id;
    m_bcId = bcId;
    m_bitpos = 0;
    m_tracklet = 0;
    m_adjacent = false;
    m_type = TYPE_NSL;
    m_forward = false;
    m_index = 0;
    m_pos = 0;
    m_delta = 0;
    m_inner = 0;
    m_segment = 0;
    m_subMatrix = 0;
    m_sector = 0;
    m_chip = 0;
    m_hipt = false;
    m_hitId = 0;
    m_hsub = 0;
    m_strip = false;
    m_cand3plus = false;
    m_muplus = false;
    m_threshold = 0;
    m_overlap = false;
    m_veto = false;
    m_roi = 0;
}

// Constructor for Trigger Coincidence
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t srodId,
                       uint16_t l1Id,
                       uint16_t bcId,
                       uint16_t type,
                       uint16_t index,
                       uint16_t pos,
                       int16_t delta)
{
    clear(bcTag, subDetectorId, srodId,  l1Id, bcId);
    setType(type);
    m_index = index;
    m_pos = pos;
    m_delta = delta;
}

// High pT
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t srodId,
                       uint16_t l1Id,
                       uint16_t bcId,
                       bool strip,
                       bool forward,
                       uint16_t sector,
                       uint16_t chip,
                       uint16_t index,
                       bool hipt,
                       uint16_t hitId,
                       uint16_t sub,
                       int16_t delta,
                       uint16_t inner
                       )
{
    clear(bcTag, subDetectorId, srodId, l1Id, bcId);
    m_type = TYPE_HIPT;
    m_strip = strip;
    m_forward = forward;
    m_sector = sector;
    m_chip = chip;
    m_index = index;
    m_hipt = hipt;
    m_hitId = hitId;
    m_hsub = sub;
    m_delta = delta;
    m_inner = inner;
}

// Sector logic
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t srodId,
                       uint16_t l1Id,
                       uint16_t bcId,
                       bool cand3plus,
                       bool forward,
                       uint16_t sector,
                       uint16_t index,
                       bool muplus,
                       uint16_t threshold,
                       bool overlap,
                       bool veto,
                       uint16_t roi)
{
    clear(bcTag, subDetectorId, srodId, l1Id, bcId);
    m_type = TYPE_NSL;
    m_cand3plus = cand3plus;
    m_forward = forward;
    m_sector = sector;
    m_index = index;
    m_muplus = muplus;
    m_threshold = threshold;
    m_overlap = overlap;
    m_veto = veto;
    m_roi = roi;
}

void TgcL1RawData::setType(uint16_t type)
{
    switch (type)
    {
    case 3: m_type = TYPE_HIPT; break;
    case 5: m_type = TYPE_NSL; break;
    default: m_type = TYPE_UNKNOWN; break;
    }
}

std::string TgcL1RawData::typeName(TgcL1RawData::DataType type)
{
    switch (type)
    {
    case TYPE_HIPT:      return "High pT";      break;
    case TYPE_NSL:       return "New Sector Logic"; break;
    default:             return "";
    }
}

template <class stream>
stream& dump(stream& sl, const TgcL1RawData& data)
{
    sl << "TgcL1RawData " << data.typeName()
       << ", bcTag=" << data.bcTag()
       << ", l1Id=" << data.l1Id()
       << ", bcId=" << data.bcId()
       << ", subDetectorId=" << data.subDetectorId()
       << ", srodId=" << data.srodId();
    switch (data.type())
    {
    case TgcL1RawData::TYPE_HIPT:
        sl << ", strip=" << data.isStrip()
        << ", forward=" << data.isForward()
        << ", sector=" << data.sector()
        << ", chip=" << data.chip()
        << ", cand=" << data.index()
        << ", hipt=" << data.isHipt()
        << ", hitId=" << data.hitId()
        << ", hsub=" << data.hsub()
        << ", delta=" << data.delta()
        << ", inner=" << data.inner();
        break;
    case TgcL1RawData::TYPE_NSL:
        sl << ", cand3plus=" << data.cand3plus()
        << ", forward=" << data.isForward()
        << ", sector=" << data.sector()
        << ", cand=" << data.index()
        << ", sign=" << data.isMuplus()
        << ", threshold=" << data.threshold()
        << ", overlap=" << data.isOverlap()
        << ", veto=" << data.isVeto()
        << ", roi=" << data.roi();
        break;
    default:
        break;
    }
    return sl;
}

MsgStream& operator<<(MsgStream& sl, const TgcL1RawData& data)
{
    return dump(sl, data);
}

std::ostream& operator<<(std::ostream& sl, const TgcL1RawData& data)
{
    return dump(sl, data);
}
