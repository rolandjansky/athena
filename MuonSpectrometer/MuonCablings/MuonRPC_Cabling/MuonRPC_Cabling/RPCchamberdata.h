/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCHAMBERDATA_H
#define RPCCHAMBERDATA_H

#include <list>
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/RPCdecoder.h"
#include "MuonCablingTools/dbline.h"
#include "MuonRPC_Cabling/RPCchamber.h"

namespace MuonRPC_Cabling {

class RPCchamberdata : BaseObject
{
    private:
    typedef std::list < RPCchamber > RPClist;

    bool m_fail;

    int m_number;
    int m_station;
    int m_type;
    std::string m_name;
    int m_stationEta;
    int m_doubletR;
    int m_doubletZ;
    int m_phiReadoutPannels;
    int m_eta_strips;
    int m_strips_in_Eta_Conn;
    int m_eta_connectors;
    //int m_eta_splitters;
    int m_phi_strips;
    int m_ijk_etaReadout;
    //unsigned int m_eta_inv0;
    //unsigned int m_eta_inv1;
    int m_strips_in_Phi_Conn;
    int m_phi_connectors;
    //int m_phi_splitters;
    int m_ijk_phiReadout;
    //unsigned int m_phi_inv0;
    //unsigned int m_phi_inv1;

    RPClist rpc;

    void reset_data(void);
    bool get_data(DBline&);
    bool confirm_connectors(ViewType);
    bool confirm_ijk(ViewType);

    public:
    RPCchamberdata();
    RPCchamberdata(DBline&,int);
    ~RPCchamberdata();

    RPCchamber* give_rpc(void);

    int station(void) const {return m_station;}

    void Print(std::ostream&,bool) const;
};

template <class X> X& operator<< (X& stream, RPCchamberdata& data)
{
    data.Print(stream,false);
    return stream;
}

}

#endif
