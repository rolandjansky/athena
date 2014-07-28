/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBEVENT_H
#define CALIBEVENT_H

#include "TrigmuFast/CalibFragment.h"
#include "stdint.h"
#include <list>

namespace LVL2CALIBRATION {

class CalibEvent : public CalibData {

    private:
    uint16_t m_version;
    uint32_t m_run_number;
    uint32_t m_lvl1_id;
    uint32_t m_timestamp;
    uint32_t m_ip_address;
    uint16_t m_l2pu_id;
    
    uint32_t m_mrod_trigger;
    uint32_t m_mrod_data[4];
    
    char m_name[100];
    
    
    float m_eta;
    float m_phi;
    float m_pt;
    
    MdtCalibFragment* m_mdt;
    RpcCalibFragment* m_rpc;
    TgcCalibFragment* m_tgc;
    
    //various implementations of encoding/decoding methods
    uint16_t getVersion(uint8_t*) const;
    bool checkIntegrity_v0(uint8_t*) const;
    bool checkIntegrity_v1(uint8_t*) const;
    void decode_v0(uint8_t* data);
    void decode_v1(uint8_t* data);
    int encode_v0(uint8_t*,uint16_t&) const;
    int encode_v1(uint8_t*,uint16_t&) const;
    
    
    public:
    CalibEvent();
    CalibEvent(float,float,float);
    //CalibEvent(uint16_t,uint32_t,uint32_t,uint32_t,uint16_t,std::string);
    CalibEvent(uint16_t, uint32_t, uint32_t, uint16_t, uint32_t, uint32_t[4], 
               std::string, float, float, float);
    CalibEvent(const CalibEvent&);
    CalibEvent(uint8_t*);
    ~CalibEvent();

    
    //access method
    uint16_t    version(void) const;
    uint32_t    run_number(void) const;
    uint32_t    lvl1_id(void) const;
    uint32_t    timestamp(void) const;
    std::string timestamp_str(void) const;
    uint16_t    l2pu_id(void) const;
    uint32_t    ip_address(void) const;
    std::string ip_address_str(void) const;
    uint32_t    mrod_trigger(void) const;
    uint32_t    mrod_data(int) const;
    
    uint8_t     name_size(void) const;
    
    std::string name() const;
    
    float eta() const;
    float phi() const;
    float pt () const;
    
    MdtCalibFragment* mdt() const;
    RpcCalibFragment* rpc() const;
    TgcCalibFragment* tgc() const;
    
    void operator<<(const MdtCalibFragment&);
    void operator<<(const RpcCalibFragment&);
    void operator<<(const TgcCalibFragment&);
    
    
    // inherited methods
    bool checkIntegrity(uint8_t*) const;
    
    uint16_t size() const;
    uint16_t payload_size() const;
    int dumpWords(uint8_t*,uint16_t&) const;
    int dataPayload(uint8_t*,uint16_t&) const;
    
    friend std::ostream  &operator<< (std::ostream& , const CalibEvent&);

};

inline uint16_t CalibEvent::version(void) const      {return m_version;}
inline uint32_t CalibEvent::run_number(void) const   {return m_run_number;}
inline uint32_t CalibEvent::lvl1_id(void) const      {return m_lvl1_id;}
inline uint32_t CalibEvent::timestamp(void) const    {return m_timestamp;}
inline uint16_t CalibEvent::l2pu_id(void) const      {return m_l2pu_id;}
inline uint32_t CalibEvent::ip_address(void) const   {return m_ip_address;}
inline uint32_t CalibEvent::mrod_trigger(void) const {return m_mrod_trigger;}
inline uint32_t CalibEvent::mrod_data(int i) const   {
                         if(i<4&&i>=0) return m_mrod_data[i]; else return 0;}

inline uint8_t CalibEvent::name_size(void) const {
    uint16_t size = std::string(m_name).length() + 1;
    return size + ((size%4)?(4-size%4):0);
}
inline std::string CalibEvent::name(void) const {return std::string(m_name);}

inline float CalibEvent::eta(void) const { return m_eta; }
inline float CalibEvent::phi(void) const { return m_phi; }
inline float CalibEvent::pt (void) const { return m_pt; }
inline MdtCalibFragment* CalibEvent::mdt(void) const { return m_mdt; }
inline RpcCalibFragment* CalibEvent::rpc(void) const { return m_rpc; }
inline TgcCalibFragment* CalibEvent::tgc(void) const { return m_tgc; }
 
}  // end of LVL2CALIBRATION namespace


#endif
