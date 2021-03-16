/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGTOOLS_RPCDECODER_H
#define MUONCABLINGTOOLS_RPCDECODER_H

#include "MuonCablingTools/RPCdef.h"
#include <iostream>
#include <sstream>

class RPCdecoder
{
    public:
    enum value_type {Strip_Type,Logic_Sector,LVL1_Station,RPC_Layer};

    private:
    static const int s_value_boundaries[4][2];
    static const char s_value_descriptions[4][15];

    private:
    bool m_fail;

    private:
    unsigned int m_code;

    int m_strip_type;
    int m_logic_sector;
    int m_lvl1_station;
    int m_rpc_layer;
    int m_rpc_z_index;
    int m_strip_number;

    void reset (void);
    void reset_status(void);
    void reset_data(void);

    void set_indexes(unsigned int);
    void set_code(int,int,int,int,int,int);
    void fault_decoding(int,value_type);

    public:
    RPCdecoder();
    RPCdecoder(unsigned int);
    RPCdecoder(int,int,int,int,int,int);
    RPCdecoder(ViewType,int,int,int,int,int);

    RPCdecoder(const RPCdecoder& deco);
    RPCdecoder& operator = (const RPCdecoder& deco);

    ~RPCdecoder()=default;

    public:
    RPCdecoder& operator ()(unsigned int);
    RPCdecoder& operator ()(ViewType,int,int,int,int,int);
    RPCdecoder& operator ()(int,int,int,int,int,int);

    public:
    operator bool() {return !m_fail;}

    public:
    bool OK(int,value_type) const;

    unsigned int code(void) const {return m_code;}
    int strip_type(void)    const {return m_strip_type;}
    int logic_sector(void)  const {return m_logic_sector;}
    int lvl1_station(void)  const {return m_lvl1_station;}
    int rpc_layer(void)     const {return m_rpc_layer;}
    int rpc_z_index(void)   const {return m_rpc_z_index;}
    int strip_number(void)  const {return m_strip_number;}
    int cabling_code(void)  const 
        {return m_strip_number + m_rpc_z_index*100 + m_rpc_layer*10000;}

    ViewType view(void) const;
    HalfType half_barrel(void) const;
    int side2type(ViewType side) const;

    void Print(std::ostream&,bool) const;
};

#endif
