/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RDOINDEX_H
#define RDOINDEX_H

// Mapping of the PAD into Receiver(RX), Sector Logic(SL), ROD and ROB.
// The PAD is identified inside the SL by a progressive number. The mapping
// between SL and RX is one on one, the same between RDO and ROB. Two SL go
// into one ROD

//                              MAP for the Sector Logic
//
// phi
//  ^          eta < 0, side C (0x66)             eta > 0, side A (0x65)
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=31,   ROD=ROB=15    |   RX=1,   SL=31,   ROD=ROB=15    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=30,   ROD=ROB=15    |   RX=0,   SL=30,   ROD=ROB=15    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=29,   ROD=ROB=14    |   RX=1,   SL=29,   ROD=ROB=14    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=28,   ROD=ROB=14    |   RX=0,   SL=28,   ROD=ROB=14    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=27,   ROD=ROB=13    |   RX=1,   SL=27,   ROD=ROB=13    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=26,   ROD=ROB=13    |   RX=0,   SL=26,   ROD=ROB=13    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=25,   ROD=ROB=12    |   RX=1,   SL=25,   ROD=ROB=12    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=24,   ROD=ROB=12    |   RX=0,   SL=24,   ROD=ROB=12    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=23,   ROD=ROB=11    |   RX=1,   SL=23,   ROD=ROB=11    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=22,   ROD=ROB=11    |   RX=0,   SL=22,   ROD=ROB=11    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=21,   ROD=ROB=10    |   RX=1,   SL=21,   ROD=ROB=10    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=20,   ROD=ROB=10    |   RX=0,   SL=20,   ROD=ROB=10    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=19,   ROD=ROB=09    |   RX=1,   SL=19,   ROD=ROB=09    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=18,   ROD=ROB=09    |   RX=0,   SL=18,   ROD=ROB=09    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=17,   ROD=ROB=08    |   RX=1,   SL=17,   ROD=ROB=08    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=16,   ROD=ROB=08    |   RX=0,   SL=16,   ROD=ROB=08    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=15,   ROD=ROB=07    |   RX=1,   SL=15,   ROD=ROB=07    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=14,   ROD=ROB=07    |   RX=0,   SL=14,   ROD=ROB=07    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=13,   ROD=ROB=06    |   RX=1,   SL=13,   ROD=ROB=06    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=12,   ROD=ROB=06    |   RX=0,   SL=12,   ROD=ROB=06    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=11,   ROD=ROB=05    |   RX=1,   SL=11,   ROD=ROB=05    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=10,   ROD=ROB=05    |   RX=0,   SL=10,   ROD=ROB=05    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=09,   ROD=ROB=04    |   RX=1,   SL=09,   ROD=ROB=04    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=08,   ROD=ROB=04    |   RX=0,   SL=08,   ROD=ROB=04    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=07,   ROD=ROB=03    |   RX=1,   SL=07,   ROD=ROB=03    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=06,   ROD=ROB=03    |   RX=0,   SL=06,   ROD=ROB=03    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=05,   ROD=ROB=02    |   RX=1,   SL=05,   ROD=ROB=02    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=04,   ROD=ROB=02    |   RX=0,   SL=04,   ROD=ROB=02    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=03,   ROD=ROB=01    |   RX=1,   SL=03,   ROD=ROB=01    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=02,   ROD=ROB=01    |   RX=0,   SL=02,   ROD=ROB=01    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=1,   SL=01,   ROD=ROB=00    |   RX=1,   SL=01,   ROD=ROB=00    |
//  |   +----------------------------------+----------------------------------+
//  |   |   RX=0,   SL=00,   ROD=ROB=00    |   RX=0,   SL=00,   ROD=ROB=00    |
//  +   +----------------------------------+----------------------------------+

#include "MuonIdHelpers/RpcIdHelper.h"

class RDOindex {
private:
    unsigned short int m_ROBid{0};  // hardware Id for the ROB
    unsigned short int m_RODid{0};  // hardware Id for the ROD
    unsigned short int m_side{0};   // hardware Id for the Sector Logic Side
    unsigned short int m_SLid{0};   // hardware Id for the Sector Logic
    unsigned short int m_RXid{0};   // hardware Id for the Receiver
    unsigned short int m_PADid{0};  // hardware Id for the PAD

    unsigned long int m_lvl1_code{0};  // Identification of the first RPC strip
                                       // on eta projection read by this PAD

    int m_stationName{0};  // StationName index according to offline Ids
    int m_stationEta{0};   // StationEta index according to offline Ids
    int m_stationPhi{0};   // StationPhi index according to offline Ids
    int m_doubletR{0};     // DoubletR index according to offline Ids
    int m_doubletZ{0};     // DoubletZ index according to offline Ids
    int m_doubletPhi{0};   // DoubletPhi index according to offline Ids

    unsigned int m_hash{0};  // hash id to be used for allocation into the container

    bool m_status{false};  // boolean flag for checking the map integrity

    void set_indexes(void);

public:
    RDOindex(unsigned int, unsigned int,
             const RpcIdHelper& helper);
    RDOindex(unsigned int, unsigned int, const std::string&, int, int, int, int, int,
             const RpcIdHelper& helper);

    RDOindex(const RDOindex&) = default;
    ~RDOindex() = default;

    RDOindex& operator=(const RDOindex&) = delete;
    operator bool() const;
    bool operator!() const;

    void set_hash(unsigned int h);

    unsigned short int ROBid(void) const { return m_ROBid; }
    unsigned short int RODid(void) const { return m_RODid; }
    unsigned short int side(void) const { return m_side; }

    unsigned short int SLid(void) const { return m_SLid; }
    unsigned short int RXid(void) const { return m_RXid; }
    unsigned short int PADid(void) const { return m_PADid; }

    unsigned long int lvl1_code(void) const { return m_lvl1_code; }

    int stationName(void) const { return m_stationName; }
    int stationEta(void) const { return m_stationEta; }
    int stationPhi(void) const { return m_stationPhi; }
    int doubletR(void) const { return m_doubletR; }
    int doubletZ(void) const { return m_doubletZ; }
    int doubletPhi(void) const { return m_doubletPhi; }

    unsigned int hash(void) const { return m_hash; }

    bool status(void) const { return m_status; }

public:
    void offline_indexes(int& name, int& eta, int& phi, int& doublet_r, int& doublet_z, int& doublet_phi, int& gas_gap, int& measures_phi,
                         int& strip) const;

    void pad_identifier(Identifier& id) const;

private:
    const RpcIdHelper* m_rpcIdHelper;

    friend std::ostream& operator<<(std::ostream&, const RDOindex&);
};

#endif
