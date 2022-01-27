/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcL1RawData.h"
#include "GaudiKernel/MsgStream.h"

void TgcL1RawData::clear(uint16_t bcTag,
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
    m_type = TYPE_HIPT;
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
  m_type = TYPE_NSL_ROI;
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

// Tracklets ( ROD ) *** For output from TrigT1TGC *** 
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
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

// New Sector logic
// RoI
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                           uint16_t subDetectorId,
                           uint16_t srodId,
                           uint16_t l1Id,
                           uint16_t bcId,
                           bool forward,
                           uint16_t sector,
                           uint16_t innerflag,
                           uint16_t coinflag,
                           bool muplus,
                           uint16_t threshold,
                           uint16_t roi)
{
  clear(bcTag, subDetectorId, srodId, l1Id, bcId);
  m_type = TYPE_NSL_ROI;
  m_forward = forward;
  m_sector = sector;
  m_innerflag = innerflag;
  m_coinflag = coinflag;
  m_muplus = muplus;
  m_threshold = threshold;
  m_roi = roi;
}

// NSW
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                           uint16_t subDetectorId,
                           uint16_t srodId,
                           uint16_t l1Id,
                           uint16_t bcId,
                           bool forward,
                           uint16_t sector,
                           uint16_t nsweta,
                           uint16_t nswphi,
                           uint16_t nswcand,
                           uint16_t nswdtheta,
                           uint16_t nswphires,
                           uint16_t nswlowres,
                           uint16_t nswid)
{
  clear(bcTag, subDetectorId, srodId, l1Id, bcId);
  m_type = TYPE_NSL_NSW;
  m_forward = forward;
  m_sector = sector;
  m_nsweta = nsweta;
  m_nswphi = nswphi;
  m_nswcand = nswcand;
  m_nswdtheta = nswdtheta;
  m_nswphires = nswphires;
  m_nswlowres = nswlowres;
  m_nswid = nswid;
}

// RPC BIS78
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                           uint16_t subDetectorId,
                           uint16_t srodId,
                           uint16_t l1Id,
                           uint16_t bcId,
                           bool forward,
                           uint16_t sector,
                           uint16_t rpceta,
                           uint16_t rpcphi,
                           uint16_t rpcflag,
                           uint16_t rpcdeta,
                           uint16_t rpcdphi)
{
  clear(bcTag, subDetectorId, srodId, l1Id, bcId);
  m_type = TYPE_NSL_RPC;
  m_forward = forward;
  m_sector = sector;
  m_rpceta   = rpceta;
  m_rpcphi   = rpcphi;
  m_rpcflag  = rpcflag;
  m_rpcdeta  = rpcdeta;
  m_rpcdphi = rpcdphi;
}

// EIFI
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                           uint16_t subDetectorId,
                           uint16_t srodId,
                           uint16_t l1Id,
                           uint16_t bcId,
                           bool forward,
                           uint16_t sector,
                           uint16_t ei,
                           uint16_t fi,
                           uint16_t cid)
{
  clear(bcTag, subDetectorId, srodId, l1Id, bcId);
  m_type = TYPE_NSL_EIFI;
  m_forward = forward;
  m_sector = sector;
  m_ei = ei;
  m_fi = fi;
  m_cid = cid;
}

// TMDB
TgcL1RawData::TgcL1RawData(uint16_t bcTag,
                           uint16_t subDetectorId,
                           uint16_t srodId,
                           uint16_t l1Id,
                           uint16_t bcId,
                           bool forward,
                           uint16_t sector,
                           uint16_t mod,
                           uint16_t bcid)
{
  clear(bcTag, subDetectorId, srodId, l1Id, bcId);
  m_type = TYPE_NSL_TMDB;
  m_forward = forward;
  m_sector = sector;
  m_tmdbmod = mod;
  m_tmdbbcid = bcid;
}


void TgcL1RawData::setType(uint16_t type)
{
  switch (type)
    {
    case 3: m_type = TYPE_HIPT; break;
    case 5: m_type = TYPE_NSL_ROI; break;
    case 6: m_type = TYPE_NSL_NSW; break;
    case 7: m_type = TYPE_NSL_RPC; break;
    case 8: m_type = TYPE_NSL_EIFI; break;
    case 9: m_type = TYPE_NSL_TMDB; break;
    default: m_type = TYPE_UNKNOWN; break;
    }
}

std::string TgcL1RawData::typeName(TgcL1RawData::DataType type)
{
  switch (type)
    {
    case TYPE_HIPT:      return "High pT";      break;
    case TYPE_NSL_ROI:   return "TGC RoI"; break;
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
    case TgcL1RawData::TYPE_NSL_ROI:
      sl
        << ", forward=" << data.isForward()
        << ", sector=" << data.sector()
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
