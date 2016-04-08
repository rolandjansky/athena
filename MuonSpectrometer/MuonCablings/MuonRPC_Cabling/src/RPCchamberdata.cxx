/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "MuonRPC_Cabling/RPCchamberdata.h"

using namespace MuonRPC_Cabling;

RPCchamberdata::RPCchamberdata() : BaseObject(Logic,"RPC Chameber Data")
{
    m_type = -1;
    m_station = -1;
    m_strips_in_Eta_Conn = -1;
    m_strips_in_Phi_Conn = -1;
    reset_data();
}

RPCchamberdata::RPCchamberdata(DBline& data, int type) :
    BaseObject(Logic,"RPC Chamber Data")
{
    int chams;
    m_type = type;
    m_station = -1;
    m_strips_in_Eta_Conn = -1;
    m_strips_in_Phi_Conn = -1;
    reset_data();

    m_fail = false;
    if(!(data("station") >> m_station)) return;
    if(!(data("made of") >> chams >> "chamber. Strips in connectors:")) return;
    if(!(data("eta") >> m_strips_in_Eta_Conn)) return;
    if(!(data("phi") >> m_strips_in_Phi_Conn)) return;

    //--m_station;

    (++data)("{");
    do
    {
        if(get_data(data))
	{
           RPCchamber cham(m_number,m_station,m_type,m_name,
	   m_stationEta,m_doubletR,m_doubletZ,m_phiReadoutPannels,
           m_strips_in_Eta_Conn,m_eta_strips,m_eta_connectors,//m_eta_splitters,
	   m_ijk_etaReadout,//m_eta_inv0,m_eta_inv1,
           m_strips_in_Phi_Conn,m_phi_strips,m_phi_connectors,//m_phi_splitters,
	   m_ijk_phiReadout);//m_phi_inv0,m_phi_inv1);
           rpc.push_back(cham);
	}
        data++;
    }while(!data("}"));
}

RPCchamberdata::~RPCchamberdata()
{
    rpc.clear();
}

void
RPCchamberdata::reset_data()
{
    m_number = -1;
    m_name = "";
    m_eta_strips = -1;
    m_eta_connectors = -1;
    //m_eta_splitters = -1;
    m_ijk_etaReadout = -1;
    //m_eta_inv0 = 0xfff;
    //m_eta_inv1 = 0xfff;
    m_phi_strips = -1;
    m_phi_connectors = -1;
    //m_phi_splitters = -1;
    m_ijk_phiReadout = -1;
    //m_phi_inv0 = 0xfff;
    //m_phi_inv1 = 0xfff;
    m_stationEta = -1;
    m_doubletR = -1;
    m_doubletZ = -1;
    m_phiReadoutPannels = -1;

    m_fail = true;
}

bool
RPCchamberdata::confirm_connectors(ViewType side)
{
    int strips     = (side == Phi)? m_phi_strips : m_eta_strips;

    if(side == Phi) m_phi_connectors = strips / m_strips_in_Phi_Conn;
    else     m_eta_connectors = strips / m_strips_in_Eta_Conn;

    int connectors = (side == Phi)? m_phi_connectors : m_eta_connectors;
    int strips_in_conn = (side == Phi)? m_strips_in_Phi_Conn : m_strips_in_Eta_Conn;
    float str = (float)strips/(float)connectors;
    std::string view = (side == Phi)? "phi" : "eta";

    __osstream disp;

    if(str > strips_in_conn)
    {
        disp << "RPCdata error in configuration for Sector Type " << m_type
             << ", station " << m_station << ", RPC number " << m_number<<std::endl
             << " " << view << " strips into connectors must be less than "
	     << strips_in_conn << " (instead are "
             << std::setprecision(2)  << str << ")" << std::endl;
        display_error(disp);
        return false;
    }
    if(m_number == 0 && ((m_eta_strips%2) || m_eta_connectors%2))
    {
        disp << "RPCdata error in configuration for Sector Type " << m_type
             << ", station " <<  m_station << ", RPC number " <<m_number<<std::endl
             << " " << view << " strips and/or connectors must be "
             << "multiple of 2 " << " (eta_strips " << m_eta_strips
             << ", eta_conn " << m_eta_connectors << ")" << std::endl;
        display_error(disp);
        return false;
    }
    if(strips_in_conn * connectors != strips)
    {
        disp << "RPCdata error in configuration for Sector Type " << m_type
             << ", station " <<  m_station << ", RPC number " <<m_number<<std::endl
             << " strips into " << view << " connectors are "<<std::setprecision(2)
             << str << " instead of " << strips_in_conn << std::endl;
        display_error(disp);
	return false;
    }
    return true;
}


bool
RPCchamberdata::confirm_ijk(ViewType side)
{

    int ijk = (side == Phi)? m_ijk_phiReadout : m_ijk_etaReadout;
    std::string view = (side == Phi)? "phi" : "eta";

    __osstream disp;

    if(ijk != 1 && ijk != 10)
    {
        disp << "RPCdata error in configuration for Sector Type " << m_type
             << ", station " << m_station << ", RPC number " << m_number<<std::endl
             << " " << view << " ijk readout must be 01 or 10; "
	     << " on the contrary it is " << std::setw(2) << std::setfill('0')
	     << ijk << std::setfill(' ') << std::endl;
        display_error(disp);
        return false;
    }

    return true;
}


bool
RPCchamberdata::get_data(DBline& data)
{
    reset_data();
    if(data("cham") >> m_number >> m_name >> m_stationEta >> m_doubletR 
                    >> m_doubletZ >> m_phiReadoutPannels
		    >> "Eview" >> m_eta_strips //>> m_eta_connectors 
		    //>> m_eta_splitters 
		    >> m_ijk_etaReadout //>> m_eta_inv0 
		    //>> m_eta_inv1	   
                    >> "Pview" >> m_phi_strips //>> m_phi_connectors 
		    //>> m_phi_splitters 
		    >> m_ijk_phiReadout) //>> m_phi_inv0 
		    //>> m_phi_inv1)
    {
        m_fail = false;
        if(!confirm_connectors(Eta) || !confirm_connectors(Phi)) m_fail = true;
	if(!confirm_ijk(Eta) || !confirm_ijk(Phi)) m_fail = true;
    }

    return !m_fail;
}

RPCchamber*
RPCchamberdata::give_rpc(void)
{
    if(rpc.size())
    {
        RPCchamber* cham = new RPCchamber(rpc.front());
	rpc.pop_front();
	return cham;
    }
    return 0;
}

void
RPCchamberdata::Print(std::ostream& stream, bool detail) const
{
    stream << "RPC data of station n. " << m_station;
    stream << " belonging to sector type " << m_type << std::endl;
    stream << "It contains " << rpc.size();
    stream << " RPC chambers:" << std::endl;
    std::list < RPCchamber >::const_iterator it;
    for(it = rpc.begin();it!=rpc.end();++it) 
        stream << ShowRequest<RPCchamber>(*it,detail); 
}
