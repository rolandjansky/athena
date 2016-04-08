/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcRawData.h"
#include "GaudiKernel/MsgStream.h"

void TgcRawData::clear(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t rodId,
                       uint16_t sswId,
                       uint16_t slbId,
                       uint16_t l1Id,
                       uint16_t bcId)
{
    m_bcTag = bcTag;
    m_subDetectorId = subDetectorId;
    m_rodId = rodId;
    m_sswId = sswId;
    m_slbId = slbId;
    m_l1Id = l1Id;
    m_bcId = bcId;
    m_slbType = SLB_TYPE_UNKNOWN;
    m_bitpos = 0;
    m_tracklet = 0;
    m_adjacent = false;
    m_type = TYPE_HIT;
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

// P1 ------------------------
// Constructor for Hit
TgcRawData::TgcRawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t rodId,
                       uint16_t sswId,
                       uint16_t slbId,
                       uint16_t l1Id,
                       uint16_t bcId,
                       uint16_t bitpos)
{
    clear(bcTag, subDetectorId, rodId, sswId, slbId, l1Id, bcId);
    m_bitpos = bitpos;
}

// Constructor for Trigger Coincidence
TgcRawData::TgcRawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t rodId,
                       uint16_t sswId,
                       uint16_t slbId,
                       uint16_t l1Id,
                       uint16_t bcId,
                       uint16_t type,
                       uint16_t index,
                       uint16_t pos,
                       int16_t delta)
{
    clear(bcTag, subDetectorId, rodId, sswId, slbId, l1Id, bcId);
    setType(type);
    m_index = index;
    m_pos = pos;
    m_delta = delta;
}

// P2 ------------------------
// Hits
TgcRawData::TgcRawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t rodId,
                       uint16_t sswId,
                       uint16_t slbId,
                       uint16_t l1Id,
                       uint16_t bcId,
                       SlbType slbType,
                       bool adj,
                       uint16_t tracklet,
                       uint16_t channel)
{
    clear(bcTag, subDetectorId, rodId, sswId, slbId, l1Id, bcId);
    m_slbType = slbType;
    m_adjacent = adj;
    m_tracklet = tracklet;
    m_bitpos = channel;
}

// Tracklets
TgcRawData::TgcRawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t rodId,
                       uint16_t sswId,
                       uint16_t slbId,
                       uint16_t l1Id,
                       uint16_t bcId,
                       SlbType slbType,
                       int16_t delta,
                       uint16_t seg,
                       uint16_t sub,
                       uint16_t rphi)
{
    clear(bcTag, subDetectorId, rodId, sswId, slbId, l1Id, bcId);
    m_type = TYPE_TRACKLET;
    m_slbType = slbType;
    m_delta = delta;
    m_segment = seg;
    m_subMatrix = sub;
    m_pos = rphi;
}

// High pT
TgcRawData::TgcRawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t rodId,
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
    clear(bcTag, subDetectorId, rodId, 0, 0, l1Id, bcId);
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
TgcRawData::TgcRawData(uint16_t bcTag,
                       uint16_t subDetectorId,
                       uint16_t rodId,
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
    clear(bcTag, subDetectorId, rodId, 0, 0, l1Id, bcId);
    m_type = TYPE_SL;
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

void TgcRawData::setType(uint16_t type)
{
    switch (type)
    {
    case 0: m_type = TYPE_TRACKLET; m_slbType = SLB_TYPE_DOUBLET_WIRE; break;
    case 1: m_type = TYPE_TRACKLET; m_slbType = SLB_TYPE_TRIPLET_WIRE; break;
    case 2: m_type = TYPE_TRACKLET; m_slbType = SLB_TYPE_TRIPLET_STRIP; break;
    case 3: m_type = TYPE_HIPT; break;
    case 4: m_type = TYPE_SL; break;
    case 10: m_type = TYPE_HIT; break;
    default: m_type = TYPE_UNKNOWN; break;
    }
}

std::string TgcRawData::typeName(TgcRawData::DataType type)
{
    switch (type)
    {
    case TYPE_HIT:       return "Hit";          break;
    case TYPE_TRACKLET:  return "Tracklet";     break;
    case TYPE_HIPT:      return "High pT";      break;
    case TYPE_SL:        return "Sector Logic"; break;
    default:             return "";
    }
}
std::string TgcRawData::slbTypeName(TgcRawData::SlbType type)
{
    switch (type)
    {
    case SLB_TYPE_DOUBLET_WIRE:  return "Doublet Wire";  break;
    case SLB_TYPE_DOUBLET_STRIP: return "Doublet Strip"; break;
    case SLB_TYPE_TRIPLET_WIRE:  return "Triplet Wire";  break;
    case SLB_TYPE_TRIPLET_STRIP: return "Triplet Strip"; break;
    case SLB_TYPE_INNER_WIRE:    return "Inner Wire";    break;
    case SLB_TYPE_INNER_STRIP:   return "Inner Strip";   break;
    default:                     return "";
    }
}

template <class stream>
stream& dump(stream& sl, const TgcRawData& data)
{
    sl << "TgcRawData " << data.typeName()
    << ", bcTag=" << data.bcTag()
    << ", l1Id=" << data.l1Id()
    << ", bcId=" << data.bcId()
    << ", subDetectorId=" << data.subDetectorId()
    << ", rodId=" << data.rodId()
    << ", sswId=" << data.sswId()
    << ", slbId=" << data.slbId();
    switch (data.type())
    {
    case TgcRawData::TYPE_HIT:
        sl << ", tracklet=" << data.tracklet()
        << ", adjacent=" << data.isAdjacent()
        << ", slbType=" << data.slbTypeName()
        << ", bitpos=" << data.bitpos();
        break;
    case TgcRawData::TYPE_TRACKLET:
        sl << ", slbType=" << data.slbTypeName()
        << ", delta=" << data.delta()
        << ", segment=" << data.segment()
        << ", subMatrix=" << data.subMatrix()
        << ", position=" << data.position();
        break;
    case TgcRawData::TYPE_HIPT:
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
    case TgcRawData::TYPE_SL:
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

MsgStream& operator<<(MsgStream& sl, const TgcRawData& data)
{
    return dump(sl, data);
}

std::ostream& operator<<(std::ostream& sl, const TgcRawData& data)
{
    return dump(sl, data);
}

