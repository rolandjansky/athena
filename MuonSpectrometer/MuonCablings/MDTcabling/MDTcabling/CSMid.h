/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSMID_H
#define CSMID_H

#include <string>
#include <iostream>


#include "AmdcCore/Amdcsimrec.h"

#include "MuonCablingTools/dbline.h"
#include "MDTcabling/t2mugeo.h"

class MDTGeometry;

struct InterStruct{
    uint8_t multilayer;
    uint8_t layer;
    uint8_t tube;
};

class CSMid
{
    private:
    static bool isNewSchema;
    
    uint32_t    m_hash_id;
    uint32_t    m_compact_id;
    std::string m_mnemonic_id;

    bool m_status;

    int m_name;
    int m_StationEta;
    int m_StationPhi;

    int m_eta_tower;
    
    double m_etaMin;
    double m_etaMax;
    double m_phiMin;
    double m_phiMax;
    
    const Chamber* m_mdt_chamber[2];
          int8_t   m_mezanine_stationEta[24];
    const Chamber* m_mezanine_chamber[24];   

    void no_definition(void);

    void clear_cabling(void);
    
    void setEtaPhiDim(void);
    
    double computing_eta(double, double ) const;
    
    
    public:
    CSMid(int StName, int eta, int phi);
    CSMid(uint32_t compact);
    CSMid(std::string mnemonic);
    CSMid(const CSMid& csm);
    CSMid();
    ~CSMid();

    CSMid operator=(const CSMid& csm);

    operator bool  () const;
    bool operator !() const;
    bool operator==(const CSMid& csm) const;

    uint32_t hash_id(void)     const;
    uint32_t compact(void)     const;
    std::string mnemonic(void) const;
    std::string name_str(void) const;
    
    int stationName(void)   const;
    int stationEta(void)    const;
    int stationEta(uint8_t) const;
    int stationPhi(void)    const;
    
    double zMin(void) const;
    double zMax(void) const;
    double rMin(void) const;
    double rMax(void) const;
    
    double etaMin(void) const;
    double etaMax(void) const;
    double phiMin(void) const;
    double phiMax(void) const;


    bool isBarrel(void) const;
    bool isEndcap(void) const;
    bool isAside (void) const;
    bool isCside (void) const;
    bool isLarge (void) const;
    bool isSmall (void) const;
    bool isOverlapping(double,double,double,double) const;

    const Chamber* get_chamber(void) const;
    const Chamber* get_chamber(uint8_t) const;
    const Chamber* get_Chamber(int)     const;
    void  get_chambers(const Chamber* cham[2]) const;
    const TDCgeo*  get_tdc(uint8_t)     const;

    int eta_tower(void) const;

    friend std::ostream& operator<<(std::ostream& stream,const CSMid& csm);
    friend class MDTGeometry;

    private:
    
    static Amdcsimrec* s_amdc;
    
    void set_chamber(const Chamber* mdt_chamber,int stationEta);
    void set_tower(const int eta_tower);
    void set_hash(const uint32_t hash_id);
};

inline std::string
CSMid::name_str() const
{
    int tmp = m_name + 1;
    return s_amdc->GetStationType(tmp);
}


inline const TDCgeo*  CSMid::get_tdc(uint8_t id) const 
{
    if(id>23) return 0;
    return (m_mezanine_chamber[id])? (m_mezanine_chamber[id])->TDC[id] : 0;
}
inline const Chamber*  CSMid::get_chamber(uint8_t id) const 
{
    if(id>23) return 0;
    return m_mezanine_chamber[id];
}
inline const Chamber* CSMid::get_Chamber(int StationEta) const
{
    for (int i=0;i<24;++i)
    {
       if (m_mezanine_stationEta[i]==StationEta) return m_mezanine_chamber[i]; 
    }
    return 0;
}
inline void CSMid::get_chambers(const Chamber* cham[2]) const
{
    cham[0] = m_mdt_chamber[0];
    cham[1] = m_mdt_chamber[1];
}
inline int CSMid::stationEta(uint8_t id) const
{
    if(id>23) return 0;
    return m_mezanine_stationEta[id];
}

inline uint32_t    CSMid::hash_id(void)     const {return m_hash_id;}
inline uint32_t    CSMid::compact(void)     const {return m_compact_id;}
inline std::string CSMid::mnemonic(void)    const {return m_mnemonic_id;}
inline int         CSMid::stationName(void) const {return m_name;}
inline int         CSMid::stationEta(void)  const {return m_StationEta;}
inline int         CSMid::stationPhi(void)  const {return m_StationPhi;}
inline int         CSMid::eta_tower(void)   const {return m_eta_tower;}

inline double CSMid::etaMin(void) const {return m_etaMin;}
inline double CSMid::etaMax(void) const {return m_etaMax;}
inline double CSMid::phiMin(void) const {return m_phiMin;}
inline double CSMid::phiMax(void) const {return m_phiMax;}

inline      CSMid::operator bool  () const {return m_status;}
inline bool CSMid::operator      !() const {return !m_status;}


#endif
