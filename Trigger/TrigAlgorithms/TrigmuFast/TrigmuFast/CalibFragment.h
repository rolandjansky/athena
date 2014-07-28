/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBFRAGMENT_H
#define CALIBFRAGMENT_H

#include <stdint.h>
#include <fstream>
#include <list>
#include "TrigmuFast/CalibData.h"


namespace LVL2CALIBRATION {


class MdtCalibFragment : public CalibData {
    
    private:
    std::list<MdtCalibData> m_data;
    
    
    public:
    MdtCalibFragment();
    MdtCalibFragment(const MdtCalibFragment&);
    MdtCalibFragment(uint8_t*);


    // access methods
    std::list<MdtCalibData> data() const;
    MdtCalibFragment& operator<<(const MdtCalibData&);
    
        
    // inherited methods
    bool checkIntegrity(uint8_t*) const;
    
    uint16_t size() const;
    int dumpWords(uint8_t*,uint16_t&) const;
    
    friend std::ostream  &operator<<(std::ostream& ,const MdtCalibFragment&);
}; 

inline std::list<MdtCalibData> MdtCalibFragment::data(void) const { return m_data; }
inline uint16_t MdtCalibFragment::size(void) const {
    uint16_t size = MDTHDRSIZE+TRLSIZE;
    MdtCalibData tmp;
    size += m_data.size() * tmp.size();
    return size;
}



class RpcCalibFragment : public CalibData {
    
    private:
    uint16_t m_subsystemId;
    uint16_t m_sectorId;
    uint16_t m_padId;
    uint16_t m_status;
    uint16_t m_error_code;
    
    std::list<RpcCalibData> m_data;
    
    
    public:
    RpcCalibFragment();
    RpcCalibFragment(uint16_t subsystemId,uint16_t sectorId,uint16_t padId,
                     uint16_t status, uint16_t error_code);
    RpcCalibFragment(const RpcCalibFragment&);
    RpcCalibFragment(uint8_t*);


    // access methods
    uint16_t subsystemId() const;
    uint16_t sectorId() const;
    uint16_t padId() const;
    uint16_t status() const;
    uint16_t error_code () const;
    std::list<RpcCalibData> data() const;
    
    RpcCalibFragment& operator<<(const RpcCalibData&);
    
        
    // inherited methods
    bool checkIntegrity(uint8_t*) const;
    
    uint16_t size() const;
    int dumpWords(uint8_t*,uint16_t&) const;
    
    friend std::ostream  &operator<<(std::ostream& ,const RpcCalibFragment&);
}; 

inline uint16_t RpcCalibFragment::subsystemId(void) const {return m_subsystemId;}
inline uint16_t RpcCalibFragment::sectorId(void) const {return m_sectorId;}
inline uint16_t RpcCalibFragment::padId(void) const {return m_padId;}
inline uint16_t RpcCalibFragment::status(void) const {return m_status;}
inline uint16_t RpcCalibFragment::error_code(void) const {return m_error_code;}
inline std::list<RpcCalibData> RpcCalibFragment::data(void) const { return m_data; }
inline uint16_t RpcCalibFragment::size(void) const {
    uint16_t size = RPCHDRSIZE+TRLSIZE;
    std::list<RpcCalibData>::const_iterator it = m_data.begin();
    while (it != m_data.end() ) {
        size += (*it).size();
	++it;
    }
    return size;
}

class TgcCalibFragment : public CalibData {
    
    private:
    uint16_t m_systemId;
    uint16_t m_subsystemId;
    uint16_t m_sectorId;
    uint16_t m_RoInumber;
    
    std::list<TgcCalibData> m_data;
    
    
    public:
    TgcCalibFragment();
    TgcCalibFragment(uint16_t systemId, uint16_t subsystemId, uint16_t sectorId,
                     uint16_t roiNumber);
    TgcCalibFragment(const TgcCalibFragment&);
    TgcCalibFragment(uint8_t*);


    // access methods
    uint16_t systemId() const;
    uint16_t subsystemId() const;
    uint16_t sectorId() const;
    uint16_t roiNumber() const;

    std::list<TgcCalibData> data() const;
    
    TgcCalibFragment& operator<<(const TgcCalibData&);
    
        
    // inherited methods
    bool checkIntegrity(uint8_t*) const;
    
    uint16_t size() const;
    int dumpWords(uint8_t*,uint16_t&) const;
    
    friend std::ostream  &operator<<(std::ostream& ,const TgcCalibFragment&);
}; 

inline uint16_t TgcCalibFragment::systemId(void) const {return m_systemId;}
inline uint16_t TgcCalibFragment::subsystemId(void) const {return m_subsystemId;}
inline uint16_t TgcCalibFragment::sectorId(void) const {return m_sectorId;}
inline uint16_t TgcCalibFragment::roiNumber(void) const {return m_RoInumber;}
inline std::list<TgcCalibData> TgcCalibFragment::data(void) const { return m_data; }

inline uint16_t TgcCalibFragment::size(void) const {
    uint16_t size = TGCHDRSIZE+TRLSIZE;
    std::list<TgcCalibData>::const_iterator it = m_data.begin();
    while (it != m_data.end() ) {
        size += (*it).size();
	++it;
    }
    return size;
}
}  // end of namespace LVL2CALIBRATION
#endif
