/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_TGCRAWDATA_H
#define MUONRDO_TGCRAWDATA_H



#include <inttypes.h>
#include <ostream>

class MsgStream;

/**
  An unit object of TGC ROD output.
  This class is supposed to hold Hit- or Coincidence-information.
  For this purpose, two full-constructors have been prepared.

  @ author Tadashi Maeno
*/
class TgcRawData
{
public:
    friend class TgcRawDataCnv_p1;
    friend class TgcRawDataCnv_p2;
    friend class TgcRawDataCnv_p3;
    friend class TgcRawDataCnv_p4;

    enum SlbType
    {
        SLB_TYPE_DOUBLET_WIRE,
        SLB_TYPE_DOUBLET_STRIP,
        SLB_TYPE_TRIPLET_WIRE,
        SLB_TYPE_TRIPLET_STRIP,
        SLB_TYPE_INNER_WIRE,
        SLB_TYPE_INNER_STRIP,
        SLB_TYPE_UNKNOWN
    };

    enum DataType
    {
        TYPE_HIT,
        TYPE_TRACKLET,
        TYPE_HIPT,
        TYPE_SL,
        TYPE_INNER,
        TYPE_INNER_NSW,
        TYPE_INNER_BIS,
        TYPE_INNER_EIFI,
        TYPE_INNER_TMDB,
        TYPE_UNKNOWN
    };

    // Default constructor
    TgcRawData()
    {
      clear(0, 0, 0, 0, 0);
      clear(0, 0, 0, 0, 0, 0, 0);
    }

    // P1 ------------------------
    // Constructor for Hit : 8
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t sswId,
               uint16_t slbId,
               uint16_t l1Id,
               uint16_t bcId,
               uint16_t bitpos);

    // Constructor for Trigger Coincidence : 11
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t sswId,
               uint16_t slbId,
               uint16_t l1Id,
               uint16_t bcId,
               uint16_t type,
               uint16_t index,
               uint16_t pos,
               int16_t delta);

    // P2 ------------------------
    // Hits : 11
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t sswId,
               uint16_t slbId,
               uint16_t l1Id,
               uint16_t bcId,
               SlbType slbType,
               bool adj,
               uint16_t tracklet,
               uint16_t channel);

    // Tracklets : 12
    TgcRawData(uint16_t bcTag,
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
               uint16_t rphi);

    // High pT : 15
    TgcRawData(uint16_t bcTag,
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
               uint16_t inner);

    // Sector logic : 14 -> could be ambiguous with NSW 
    TgcRawData(uint16_t bcTag,
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
               uint16_t roi);

    // New Sector logic
    // RoI : 12
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t l1Id,
               uint16_t bcId,
               bool forward,
               uint16_t sector,
               uint16_t innerflag,
               uint16_t coinflag,
               bool muplus,
               uint16_t threshold,
               uint16_t roi);

    // NSW : 14
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
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
               uint16_t nswid);

    // RPC BIS78 : 12
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t l1Id,
               uint16_t bcId,
               bool forward,
               uint16_t sector,
               uint16_t rpceta,
               uint16_t rpcphi,
               uint16_t rpcflag,
               uint16_t rpcdeta,
               uint16_t rpcdphi);

    // EIFI : 10
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t l1Id,
               uint16_t bcId,
               bool forward,
               uint16_t sector,
               uint16_t ei,
               uint16_t fi,
               uint16_t cid); // chamber ID
    
    // TMDB : 9
    TgcRawData(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t l1Id,
               uint16_t bcId,
               bool forward,
               uint16_t sector,
               uint16_t mod,
               uint16_t bcid);
    
    // Destructor
    virtual ~TgcRawData()
    {
    }

    // return Information Type Hit:Coincidence
    bool isCoincidence() const
    {
        return m_type == TYPE_TRACKLET || m_type == TYPE_HIPT || m_type == TYPE_SL;
    }

    /*********** common accessors
     */
    void setOnlineId(uint16_t sId, uint16_t rId)
    {
        m_subDetectorId = sId;
        m_rodId = rId;
    }

    uint16_t bcTag() const
    {
        return m_bcTag;
    }
    uint16_t l1Id() const
    {
        return m_l1Id;
    }
    uint16_t bcId()  const
    {
        return m_bcId;
    }

    uint16_t subDetectorId() const
    {
        return m_subDetectorId;
    }
    uint16_t rodId() const
    {
        return m_rodId;
    }
    uint16_t sswId() const
    {
        return m_sswId;
    }
    uint16_t slbId() const
    {
        return m_slbId;
    }
    DataType type() const
    {
        return m_type;
    }
    void setType(uint16_t type);
    SlbType slbType() const
    {
        return m_slbType;
    }
    bool isForward() const
    {
        return m_forward;
    }
    uint16_t bitpos() const
    {
        return m_bitpos;
    }
    uint16_t channel() const
    {
        return m_bitpos;
    }
    uint16_t tracklet() const
    {
        return m_tracklet;
    }
    bool isAdjacent() const
    {
        return m_adjacent;
    }
    uint16_t index() const
    {
        return m_index;
    }
    uint16_t position() const
    {
        return m_pos;
    }
    int16_t delta() const
    {
        return m_delta;
    }
    uint16_t inner() const
    {
        return m_inner;
    }
    uint16_t segment() const
    {
        return m_segment;
    }
    uint16_t subMatrix() const
    {
        return m_subMatrix;
    }
    uint16_t sector() const
    {
        return m_sector;
    }
    uint16_t chip() const
    {
        return m_chip;
    }
    bool isHipt() const
    {
        return m_hipt;
    }
    uint16_t hitId() const
    {
        return m_hitId;
    }
    uint16_t hsub() const
    {
        return m_hsub;
    }
    bool isStrip() const
    {
        return m_strip;
    }
    bool cand3plus() const
    {
        return m_cand3plus;
    }
    bool isMuplus() const
    {
        return m_muplus;
    }
    uint16_t threshold() const
    {
        return m_threshold;
    }
    bool isOverlap() const
    {
        return m_overlap;
    }
    bool isVeto() const
    {
        return m_veto;
    }
    uint16_t roi() const
    {
        return m_roi;
    }
    uint16_t innerflag() const
    {
        return m_innerflag;
    }
    uint16_t coinflag() const
    {
        return m_coinflag;
    }
    uint16_t nsweta() const
    {
        return m_nsweta;
    }
    uint16_t nswphi() const
    {
        return m_nswphi;
    }
    uint16_t nswsl() const
    {
        return m_nswsl;
    }
    uint16_t nswcand() const
    {
        return m_nswcand;
    }
    uint16_t nswdtheta() const
    {
        return m_nswdtheta;
    }
    uint16_t nswphires() const
    {
        return m_nswphires;
    }
    uint16_t nswlowres() const
    {
        return m_nswlowres;
    }
    uint16_t nswid() const
    {
        return m_nswid;
    }
    uint16_t rpceta() const
    {
        return m_rpceta;
    }
    uint16_t rpcphi() const
    {
        return m_rpcphi;
    }
    uint16_t rpcflag() const
    {
        return m_rpcflag;
    }
    uint16_t rpcdeta() const
    {
        return m_rpcdeta;
    }
    uint16_t rpcdphi() const
    {
        return m_rpcdphi;
    }
    uint16_t ei() const
    {
        return m_ei;
    }
    uint16_t fi() const
    {
        return m_fi;
    }
    uint16_t cid() const
    {
        return m_cid;
    }
    uint16_t tmdbmod() const
    {
        return m_tmdbmod;
    }
    uint16_t tmdbbcid() const
    {
        return m_tmdbbcid;
    }

    static std::string typeName(DataType type);

    std::string typeName() const
    {
        return typeName(m_type);
    }

    static std::string slbTypeName(SlbType type);

    std::string slbTypeName() const
    {
        return slbTypeName(m_slbType);
    }

private:
    void clear(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t l1Id,
               uint16_t bcId);
    void clear(uint16_t bcTag,
               uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t sswId,
               uint16_t slbId,
               uint16_t l1Id,
               uint16_t bcId);

    /********** common attributes
     */

    // BC tag (Central, Next, Previous)
    uint16_t m_bcTag = 0U;

    // online Identifiers
    uint16_t m_subDetectorId = 0U;
    uint16_t m_rodId = 0U;
    uint16_t m_sswId = 0U;
    uint16_t m_slbId = 0U;

    // BCID and L1ID on SLB
    uint16_t m_l1Id = 0U;
    uint16_t m_bcId = 0U;

    SlbType m_slbType{SLB_TYPE_UNKNOWN};

    /********** Hit attributes
     */

    // bitmap position [40..199]
    uint16_t m_bitpos = 0U;

    // Index of assocaitated coincidence (if any) [0..63]
    uint16_t m_tracklet = 0U;

    bool m_adjacent = false;

    /********** Coincidence attributes
     */

    //  Coincidence type (L-Pt, H-Pt, SL)
    DataType m_type;
    bool m_forward = false;

    // Coincidence index
    uint16_t m_index = 0U;

    // Coincidence Data
    uint16_t m_pos = 0U;
    int16_t m_delta = 0;
    uint16_t m_segment = 0U;
    uint16_t m_subMatrix = 0U;

    // HipT data
    uint16_t m_sector = 0U;
    uint16_t m_chip = 0U;
    bool m_hipt = false;
    uint16_t m_hitId = 0U;
    uint16_t m_hsub = 0U;
    bool m_strip = false;
    uint16_t m_inner = 0U;

    // Sector Logic
    bool m_cand3plus = false;
    bool m_muplus = false;
    uint16_t m_threshold = 0U;
    bool m_overlap = false;
    bool m_veto = false;
    uint16_t m_roi = 0U;

    // New Sector Logic
    // RoI
    uint16_t m_innerflag = 0U;
    uint16_t m_coinflag = 0U;

    // NSW
    uint16_t m_nsweta = 0U;
    uint16_t m_nswphi = 0U;
    uint16_t m_nswsl = 0U;
    uint16_t m_nswcand = 0U;
    uint16_t m_nswdtheta = 0U;
    uint16_t m_nswphires = 0U;
    uint16_t m_nswlowres = 0U;
    uint16_t m_nswid = 0U;

    // RPC BIS78
    uint16_t m_rpceta = 0U;
    uint16_t m_rpcphi = 0U;
    uint16_t m_rpcflag = 0U;
    uint16_t m_rpcdeta = 0U;
    uint16_t m_rpcdphi = 0U;

    // EIFI
    uint16_t m_ei = 0U;
    uint16_t m_fi = 0U;
    uint16_t m_cid = 0U;

    // TMDB
    uint16_t m_tmdbmod = 0U;
    uint16_t m_tmdbbcid = 0U;
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator<<(MsgStream& sl, const TgcRawData& coll);

/**Overload of << operator for std::ostream for debug output*/
std::ostream& operator<<(std::ostream& sl, const TgcRawData& coll);

#endif

