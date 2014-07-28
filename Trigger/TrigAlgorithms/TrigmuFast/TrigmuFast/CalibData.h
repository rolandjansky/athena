/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBDATA_H
#define CALIBDATA_H

#include <stdint.h>
#include <fstream>
#include <list>
#include <vector>

#ifndef LVL2_MUON_CALIBRATION_STANDALONE

#include "TrigmuFast/TgcCalibBSData.h"

#else

#include "TgcCalibBSData.h"

#endif


namespace LVL2CALIBRATION {

enum DataTags{
    EVTTAG = 0xabba,
    RPCTAG = 0xacca,
    MDTTAG = 0xadda,
    TGCTAG = 0xaeea,
    CSCTAG = 0xaffa,
    TRLTAG = 0xabb1fede
};

enum FragSize{
    EV0HDRSIZE = 36,
    EV1HDRSIZE = 56,
    RPCHDRSIZE = 20,
    MDTHDRSIZE = 12,
    TGCHDRSIZE = 20,
    CSCHDRSIZE = 12,
    TRLSIZE    = 8,
    WRDSIZE    = 4
};



class CalibData {
    
    protected:
    virtual bool checkIntegrity(uint8_t*) const = 0;
    
    public:
    CalibData();
    virtual ~CalibData();
    
    
    virtual uint16_t size() const = 0;
    virtual int dumpWords(uint8_t*,uint16_t&) const = 0;

    friend std::ostream  &operator<<(std::ostream& ,const CalibData*);
};





class MdtCalibData : public CalibData {
    
    private:
    uint32_t m_id;
    uint16_t m_leadingCoarse;
    uint16_t m_leadingFine;
    uint16_t m_trailingCoarse;
    uint16_t m_trailingFine;
    uint16_t m_width;
    float    m_phi;
    
    
    public:
    MdtCalibData();
    MdtCalibData(uint32_t id,uint16_t leadingCoarse,uint16_t leadingFine,
                            uint16_t trailingCoarse,uint16_t trailingFine,
		            uint16_t width,float phi);
    MdtCalibData(const MdtCalibData&);
    MdtCalibData(uint8_t*);

    // access methods
    uint32_t id() const;
    uint16_t leadingCoarse() const;
    uint16_t leadingFine() const;
    uint16_t trailingCoarse() const;
    uint16_t trailingFine() const; 
    uint16_t width() const;
    float phi() const;
    
    void getOfflineId(int&,int&,int&,int&,int&,int&) const;
    
    // inherited methods
    bool checkIntegrity(uint8_t*) const;
    
    uint16_t size() const;
    int dumpWords(uint8_t*,uint16_t&) const;
    
    friend std::ostream  &operator<<(std::ostream& ,const MdtCalibData&);
}; 

inline uint32_t MdtCalibData::id(void) const { return m_id; }
inline uint16_t MdtCalibData::leadingCoarse(void) const { return m_leadingCoarse; }
inline uint16_t MdtCalibData::leadingFine(void) const { return m_leadingFine; }
inline uint16_t MdtCalibData::trailingCoarse(void) const { return m_trailingCoarse; }
inline uint16_t MdtCalibData::trailingFine(void) const { return m_trailingFine; }
inline uint16_t MdtCalibData::width(void) const { return m_width; }
inline float    MdtCalibData::phi(void) const { return m_phi; }

inline uint16_t MdtCalibData::size(void) const { return 4*WRDSIZE; }




class RpcCalibData : public CalibData {

    private:
    uint16_t m_onlineId;
    uint16_t m_fel1Id;
    uint16_t m_febcId;
    uint16_t m_crc;
    std::list<uint16_t> m_hits;
    
    public:
    RpcCalibData();
    RpcCalibData(uint16_t onlineId, uint16_t fel1Id, uint16_t febcId,
                 uint16_t crc);
    RpcCalibData(const RpcCalibData&);
    RpcCalibData(uint8_t*);

    void getHit(uint16_t bcid, uint16_t time, uint16_t ijk, uint16_t channel);
    void getHit(uint16_t bcid, uint16_t time, uint16_t ijk, uint16_t ovl, uint16_t thr);
    bool giveHit (uint16_t  num, uint16_t& bcid, uint16_t&time, 
                  uint16_t& ijk, uint16_t& channel, uint16_t& ovl, 
		  uint16_t& th) const; 

    // access methods
    uint16_t onlineId() const;
    uint16_t fel1Id() const;
    uint16_t febcId() const;
    uint16_t crc() const;
    uint16_t hitNum() const; 
    const std::list<uint16_t>& hits() const;
    
    // inherited methods
    bool checkIntegrity(uint8_t*) const;
    
    uint16_t size() const;
    int dumpWords(uint8_t*,uint16_t&) const;
    
    friend std::ostream  &operator<<(std::ostream& ,const RpcCalibData&);

};

inline uint16_t RpcCalibData::onlineId(void) const { return m_onlineId; }
inline uint16_t RpcCalibData::fel1Id(void) const { return m_fel1Id; }
inline uint16_t RpcCalibData::febcId(void) const { return m_febcId; }
inline uint16_t RpcCalibData::crc(void) const { return m_crc; }
inline uint16_t RpcCalibData::hitNum(void) const {return m_hits.size();}
inline const std::list<uint16_t>& RpcCalibData::hits(void) const { return m_hits; }
inline uint16_t RpcCalibData::size(void) const {
                                      return (2+hitNum()/2+hitNum()%2)*WRDSIZE;}




class TgcCalibData : public CalibData {

    private:
    std::vector<TGC_BYTESTREAM_FRAGMENTCOUNT> m_counters;
    
    std::list<uint32_t> m_readout_hit;
    std::list<uint32_t> m_readout_triplet_strip;
    std::list<uint32_t> m_readout_tracklet;
    std::list<uint32_t> m_readout_hipt;
    std::list<uint32_t> m_readout_sl;
        
    void initCounters();
    
    
    public:
    TgcCalibData();
    TgcCalibData(uint16_t);
    TgcCalibData(const TgcCalibData&);
    TgcCalibData(uint8_t*);

    TgcCalibData& operator<<(TGC_BYTESTREAM_READOUTHIT);
    TgcCalibData& operator<<(TGC_BYTESTREAM_READOUTTRIPLETSTRIP);
    TgcCalibData& operator<<(TGC_BYTESTREAM_READOUTTRACKLET);
    TgcCalibData& operator<<(TGC_BYTESTREAM_HIPT);
    TgcCalibData& operator<<(TGC_BYTESTREAM_SL);
    
    void getHit(const TGC_BYTESTREAM_READOUTHIT,
                const TGC_BYTESTREAM_READOUTTRIPLETSTRIP,
		const TGC_BYTESTREAM_READOUTTRACKLET,
		const TGC_BYTESTREAM_HIPT,
		const TGC_BYTESTREAM_SL);


    // access methods
    const std::list<uint32_t>& readout_hit() const;
    const std::list<uint32_t>& readout_triplet_strip() const;
    const std::list<uint32_t>& readout_tracklet() const;
    const std::list<uint32_t>& readout_hipt() const;
    const std::list<uint32_t>& readout_sl() const;
    uint32_t preassigned_size() const;
    
    uint16_t hit_counter() const;
    
    const std::vector<TGC_BYTESTREAM_FRAGMENTCOUNT>& counters() const;
        
    std::list<TGC_BYTESTREAM_READOUTHIT>          readoutHit () const;
    std::list<TGC_BYTESTREAM_READOUTTRIPLETSTRIP> readoutTripletStrip() const;
    std::list<TGC_BYTESTREAM_READOUTTRACKLET>     readoutTracklet() const;
    std::list<TGC_BYTESTREAM_HIPT>                readoutHipt() const;
    std::list<TGC_BYTESTREAM_SL>                  readoutSL() const;
    
    // inherited methods
    bool checkIntegrity(uint8_t*) const;
    
    uint16_t size() const;
    uint16_t counter_size() const;
    
    int dumpWords(uint8_t*,uint16_t&) const;
    
    friend std::ostream  &operator<<(std::ostream& ,const TgcCalibData&);

};


inline const std::list<uint32_t>&  TgcCalibData::readout_hit(void) const {return m_readout_hit;}
inline const std::list<uint32_t>&  TgcCalibData::readout_triplet_strip(void) const {return m_readout_triplet_strip;}
inline const std::list<uint32_t>&  TgcCalibData::readout_tracklet(void) const {return m_readout_tracklet;}
inline const std::list<uint32_t>&  TgcCalibData::readout_hipt(void) const {return m_readout_hipt;}
inline const std::list<uint32_t>&  TgcCalibData::readout_sl(void) const {return m_readout_sl;}

inline const std::vector<TGC_BYTESTREAM_FRAGMENTCOUNT>& TgcCalibData::counters(void) const {return m_counters;}


inline uint16_t TgcCalibData::counter_size(void) const 
{
    uint16_t counter_size = 0x0;
    for (unsigned int i=0;i<7;++i)
    {
        counter_size += (m_counters[i].count!=0)? WRDSIZE : 0x0;
    }
    return counter_size;
}

inline uint16_t TgcCalibData::size(void) const 
{   
    uint16_t data_size = 0x0;
    for (unsigned int i=0;i<7;++i)
    {
        data_size += m_counters[i].count * WRDSIZE;
    }
    return this->counter_size() + data_size;
}

inline uint16_t TgcCalibData::hit_counter(void) const 
{
    uint16_t counter = 0x0;
    for (unsigned int i=0;i<7;++i)
    {
        counter += m_counters[i].count;
    }
    return counter;
}

}  // end of namespace LVL2CALIBRATION
#endif
