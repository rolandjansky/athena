/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "MuonRPC_Cabling/RPCchamber.h"
#include "MuonRPC_Cabling/EtaCMA.h"
#include "MuonRPC_Cabling/WiredOR.h"
#include "MuonRPC_Cabling/SectorLogicSetup.h"

using namespace MuonRPC_Cabling;

RPCchamber::RPCchamber(int num,int stat,int type,std::string cham_name,
                       int stationEta,int doubletR,int doubletZ,
		       int phiReadoutPannels,
                       int eta_sc,int eta_st,int eta_co,//int eta_sp,
	      int ijk_etaReadout,//unsigned int eta_inv0,unsigned int eta_inv1, 
                       int phi_sc,int phi_st,int phi_co,//int phi_sp,
	      int ijk_phiReadout) ://unsigned int phi_inv0,unsigned int phi_inv1) :
CablingObject(num,stat,type,"RPC")
{    
     m_strips_in_Eta_Conn = eta_sc;
     m_strips_in_Phi_Conn = phi_sc;
     m_eta_strips         = eta_st;
     m_eta_connectors     = eta_co;
     m_ijk_etaReadout     = ijk_etaReadout;
     m_phi_strips         = phi_st;
     m_phi_connectors     = phi_co;
     m_ijk_phiReadout     = ijk_phiReadout; 
     m_chamber_name       = cham_name;
     m_eta_strip_global   = 0;
     m_eta_conn_global    = 0;
     
     m_stationEta         = stationEta;
     m_doubletR           = doubletR;
     m_doubletZ           = doubletZ;
     m_phiReadoutPannels  = phiReadoutPannels;

     for(int i = 0 ;i < m_eta_strips; ++i) m_eta_read_mul.push_back(0);
}



RPCchamber::RPCchamber(const RPCchamber& cham) :
CablingObject(cham.number(),cham.station(),cham.sector_type(),cham.name())
{    
     m_strips_in_Eta_Conn = cham.strips_in_Eta_Conn();
     m_strips_in_Phi_Conn = cham.strips_in_Phi_Conn();
     m_eta_strips         = cham.eta_strips();
     m_eta_connectors     = cham.eta_connectors();
     m_ijk_etaReadout     = cham.ijk_etaReadout();
     m_phi_strips         = cham.phi_strips();
     m_phi_connectors     = cham.phi_connectors();
     m_ijk_phiReadout     = cham.ijk_phiReadout();
     m_chamber_name       = cham.chamber_name();
     m_stationEta         = cham.stationEta();
     m_doubletR           = cham.doubletR();
     m_doubletZ           = cham.doubletZ();
     m_phiReadoutPannels  = cham.phiReadoutPannels();
     m_eta_strip_global   = cham.eta_strip_global();
     m_eta_conn_global    = cham.eta_conn_global();

     m_eta_read_mul       = cham.eta_read_mul();

     m_readoutWORs        = cham.readoutWORs();
     m_readoutCMAs        = cham.readoutCMAs();
}

RPCchamber& RPCchamber::operator=(const RPCchamber& cham)
{
     if(this!=&cham){
       m_strips_in_Eta_Conn = cham.strips_in_Eta_Conn();
       m_strips_in_Phi_Conn = cham.strips_in_Phi_Conn();
       m_eta_strips         = cham.eta_strips();
       m_eta_connectors     = cham.eta_connectors();
       m_ijk_etaReadout     = cham.ijk_etaReadout();
       m_phi_strips         = cham.phi_strips();
       m_phi_connectors     = cham.phi_connectors();
       m_ijk_phiReadout     = cham.ijk_phiReadout();
       m_chamber_name       = cham.chamber_name();
       m_stationEta         = cham.stationEta();
       m_doubletR           = cham.doubletR();
       m_doubletZ           = cham.doubletZ();
       m_phiReadoutPannels  = cham.phiReadoutPannels();
       m_eta_strip_global   = cham.eta_strip_global();
       m_eta_conn_global    = cham.eta_conn_global();
       
       m_readoutWORs.clear();
       m_readoutCMAs.clear();
       m_eta_read_mul.clear();

       m_eta_read_mul     = cham.eta_read_mul();
       m_readoutWORs      = cham.readoutWORs();
       m_readoutCMAs      = cham.readoutCMAs();
     }
     return *this;
}


RPCchamber::~RPCchamber()
{
    m_readoutWORs.clear();
    m_readoutCMAs.clear();
}

bool
RPCchamber::setup(SectorLogicSetup& setup)
{    
    RPCchamber* rpc = setup.find_chamber(this->station(),1);
    int ijk_eta = rpc->ijk_etaReadout();
    int ijk_phi = rpc->ijk_phiReadout();

    if(ijk_eta != m_ijk_etaReadout)
    {
	error("==> mismatch of ijk_etaReadout with respect to others RPC");
	return false;
    
    }
    
    if(ijk_phi != m_ijk_phiReadout)
    {
	error("==> mismatch of ijk_phiReadout with respect to others RPC");
	return false;

    }
    
    return true; 
}

bool
RPCchamber::check()
{
    
    if(m_readoutCMAs.size() == 0)
    {
	error("==> No readout coverage for this chamber!");
	return false;
    }

    RPCchamber::CMAlist::const_iterator cma = m_readoutCMAs.begin();
    const CMAinput IO = (*cma)->whichCMAinput(this->station());

    int channels = m_eta_strips;
    for(int i = 0; i < channels; ++i)
    { 
        if(!m_eta_read_mul[i]) 
        {
	    error("==> No readout coverage for the full set of ETA strip!"); 
            return false;
        }
        if(m_eta_read_mul[i] > 1 && IO == Pivot)
	{	  
            error("==> Pivot plane ETA strips must be read only once!");
            return false;
	}
        if(m_eta_read_mul[i] > 2)
	{
	    error("==> Confirm plane ETA strips can be read only twice!");
	    return false;
	}
    }

    if(m_readoutWORs.size() > 1)
    {
	error("==> Gives input to more than 1 Wired OR pannel!");
	return false;
    }
    return true; 
}

void
RPCchamber::error(std::string mess)
{
    error_header();

    DISP << mess << std::endl
         << *this;
    DISP_ERROR;
}


bool
RPCchamber::local_strip(ViewType side,int strip_number,int& local_address)const
{
    if(side == Eta)
    {
        if(strip_number >= m_eta_strips) return false;
        if(this->number()) 
        {
            local_address = strip_number + 1;
        }
        else 
        { 
	    int middle  = m_eta_strips/2;
            int address = strip_number - middle;
            local_address = (strip_number < middle)? address : address + 1;
        }
        return true;
    }

    if (side == Phi)
    {
        if(strip_number >= m_phi_strips) return false;
        local_address = strip_number + 1;
    }

    return false;
}

bool
RPCchamber::global_strip(ViewType side,HalfType h_barrel,int strip_number,
int& global_address)const
{
    int local_address;
    if(this->local_strip(side,strip_number,local_address)) 
    {
        if(side == Eta)
        {
	    global_address = local_address + m_eta_strip_global;
            if(this->number())
	    {
                if(h_barrel == Negative) global_address = -global_address;
	    }
            else
	    {
		if(h_barrel == Negative && global_address > 0) return false;
                if(h_barrel == Positive && global_address < 0) return false;
	    }
            return true;
        }

        if (side == Phi)
        {
            return false;
        }
    }

    return false;
}

bool
RPCchamber::local_connector(ViewType side,int strip_number, 
int& local_address,int& low_eta_strips,int& hi_eta_strips) const
{
    int strip_address;
    if(this->local_strip(side,strip_number,strip_address))
    {
        int address    = abs(strip_address) - 1;
        int strip_sign = (abs(strip_address))/strip_address;
        if(side == Eta)
	{
	    local_address  = (address/m_strips_in_Eta_Conn + 1)*strip_sign;
            low_eta_strips = address % m_strips_in_Eta_Conn;
            int left       = m_strips_in_Eta_Conn - low_eta_strips - 1;
            int residual_strips = this->residual(side,strip_number);
            hi_eta_strips  = (left <= residual_strips)? left : residual_strips;
	    //  if(strip_sign < 0) SwapElements(left_strips,right_strips);
            return true;
	}
	if (side == Phi)
	{
	    return false;
	}
    }

    return false;
}

int
RPCchamber::residual(ViewType side,int strip_number) const
{
    if (side == Eta)
    {
        if(this->number())
	{ 
            return (m_eta_strips - 1 - strip_number);
	}
        else
	{
	    int total_strips = m_eta_strips / 2;
            if(strip_number < total_strips) return strip_number;
            else return m_eta_strips - strip_number - 1;
	}
    }
    if (side == Phi)
    {
        return (m_phi_strips - 1 - strip_number);
    }
    return 0;
}

bool
RPCchamber::global_connector(ViewType side,HalfType h_barrel,int strip_number,
int& global_address,int& left_strips,int& right_strips) const
{
    int l_address;
    if(local_connector(side,strip_number,l_address,left_strips,right_strips))
    {
        if(side == Eta) 
        {
	    global_address = l_address + m_eta_conn_global;            
            if(this->number())
	    {
	        if(h_barrel == Negative) global_address = -global_address;
	    }
            else
	    {
		if(h_barrel == Negative && global_address > 0) return false;
                if(h_barrel == Positive && global_address < 0) return false;
	    }
            return true;
        }
        if (side == Phi)
        {
            return false;
        }
    }

    return false;
}

bool
RPCchamber::Gstrip_2_Lnumber(ViewType side,int global_address,
			     int& strip_number) const
{
    if(side == Eta)
    {
        if(this->number())
	{
	    strip_number = abs(global_address) - m_eta_strip_global - 1;
	    if(strip_number <= m_eta_strips - 1) return true;
	}
        else
	{
            strip_number=(global_address>0)? global_address+m_eta_strips/2 - 1:
	                                     global_address+m_eta_strips/2;
	    if(strip_number <= m_eta_strips - 1 && strip_number>=0) return true;
	}

    }
    if(side == Phi)
    {
        return false;
    }

  return false;
}

bool
RPCchamber::Gconn_2_Lnumber(ViewType side,int global_address,int&local_address,
			     int& strip_number) const
{
    if(side == Eta)
    {
        if(this->number())
	{
            local_address = abs(global_address) - m_eta_conn_global - 1;
	    if(local_address <= m_eta_connectors - 1)
            {
                strip_number = local_address * m_strips_in_Eta_Conn;
                return true;
	    }
	}
        else
	{
            local_address = (global_address>0)? 
                                       global_address + m_eta_connectors/2 - 1:
	                               global_address + m_eta_connectors/2;
	    if(local_address <= m_eta_connectors -1 && local_address >= 0)
            {
                strip_number = local_address * m_strips_in_Eta_Conn;
                return true;
	    }
	}

    }

    if(side == Phi)
    {
        return false;
    }

  return false;
}

void
RPCchamber::set_eta_st_global(int gl)
{
    m_eta_strip_global = gl;
}

void
RPCchamber::set_eta_co_global(int gl)
{
    m_eta_conn_global = gl;
}

void
RPCchamber::Print(std::ostream& stream, bool detail) const
{
    stream << " chamber n. " << std::setw(2) << number() << " "; 
    stream << chamber_name();
    if(chamber_name().length() == 5) stream << " ";
    stream << "  " << std::setw(2) << stationEta() << std::setw(2) << doubletR();
    stream << std::setw(2) << doubletZ() << "  ";
    stream << "  (stat " << station() << ")";
    stream << " :  eta -> " << std::setw(2) << eta_strips() << " "; 
    stream << std::setw(2) << eta_connectors() << " ";
    stream << std::setw(2) << std::setfill('0') << ijk_etaReadout() << std::setfill(' ');
    stream << "    phi -> " << std::setw(2) << phi_strips() << " "; 
    stream << std::setw(2) << phi_connectors() << " ";
    stream << std::setw(2) << std::setfill('0') << ijk_phiReadout() << std::setfill(' '); 
    stream << std::endl;
    if(detail)
    {
        stream << " global strips offset -( " << m_eta_strip_global
               << " )-   global connectors offset -( "
               << m_eta_conn_global << " )-" << std::endl; 
        stream << " It gives input to " << m_readoutCMAs.size() 
               << " Eta CMA:" << std::endl;
        CMAlist::const_iterator cma = m_readoutCMAs.begin();
        while(cma != m_readoutCMAs.end()) 
        {
            stream << *(*cma); 
            ++cma;
        }
        stream << " Eta Readout multiplicity:" << std::endl;
        stream << "                          "
               << " 1   5    10   15   20   25   30   35   40" << std::endl;
        stream << "                          "
               << " |   |    |    |    |    |    |    |    |" << std::endl;
        stream << "                           ";
        for(int i=0;i<eta_strips();++i)stream<<m_eta_read_mul[i];
        stream << std::endl;
        stream << " It gives input to " << m_readoutWORs.size() 
               << " WiredOR pannel:" << std::endl;
        RPCchamber::WORlist::const_iterator wor = m_readoutWORs.begin();
        while(wor != m_readoutWORs.end()) 
        {
            stream << *(*wor); 
            ++wor;
        }

        stream << "========================================"
               << "=======================================" << std::endl;
      }
}

void
RPCchamber::add_cma(const EtaCMA* cma)
{
    m_readoutCMAs.push_back(cma);
}

void
RPCchamber::add_wor(const WiredOR* wor)
{
    //cout << *wor;
    m_readoutWORs.push_back(wor);
    //cout << *this << std::endl;
}

void
RPCchamber::add_eta_channel(int channel)
{
    m_eta_read_mul[channel] += 1;
}

std::string
RPCchamber::extendedName (int sector) const
{
    std::string side;
    if (sector <=31) side = "C";
    else             side = "A"; 
    
    std::string sd;
    
    switch (stationName()[2])
    {
        case 'L' : sd = (sector%2)? "HV" : "RO";
	break;
	
        case 'E' : sd = (sector%2)? "HV" : "RO";
	break;
	
	case 'R' : sd = (sector%2)? "HV" : "RO";
	break;
	
	case 'M' : sd = (sector%2)? "HV" : "RO";
	break;
	
	case 'S' : sd = (sector%2)? "RO" : "HV";
	break;
	
	case 'F' : sd = (sector%2)? "RO" : "HV";
	break;
	
	case 'G' : sd = (sector%2)? "RO" : "HV";
	break;
	
	default: return ""; 
    }
    
    __osstream out;
    
    int physicsSector = (((sector+1)%32)/2 + 1)%16;
    if (!physicsSector) physicsSector = 16;
    
    if( stationName().c_str()[1] == 'O'  && 
        (stationName().c_str()[2] == 'F' || stationName().c_str()[2] == 'G') )
    {
	int etaIdx = (stationName().c_str()[2]=='F')? (stationEta()-1)*2+1:
	                                              (stationEta())*2;
	    
	out << stationName() << etaIdx << side << std::setw(2) << std::setfill('0')
            << physicsSector << "-" << sd << "-" << doubletZ();
    } 
    else
    {
        out << stationName() << stationEta() << side << std::setw(2) << std::setfill('0')
	    << physicsSector << "-" << sd << "-" << doubletZ();
    }
    
    std::string outstr;
    outstr = out.str();

    return outstr;
}

bool
RPCchamber::inversion (int sector) const
{
    switch (stationName()[2])
    {
        case 'L' :
	if (sector%2)  return (sector<=31)? true  : false;   //HV
	else           return (sector<=31)? false : true;   //RO
	break;
	
        case 'E' :
	if (sector%2)  return (sector<=31)? true  : false;   //HV
	else           return (sector<=31)? false : true;   //RO
	break;
	
	case 'R' :
	if (sector%2)  return (sector<=31)? false : false;   //HV 
	else           return (sector<=31)? false : false;   //RO
	break;
	
	case 'M' :
	if (sector%2)  return (sector<=31)? false : false;   //HV 
	else           return (sector<=31)? false : false;   //RO
	break;
	
	case 'S' :
	if (sector%2)  return (sector<=31)? true  : false;  //RO
	else           return (sector<=31)? false : true;   //HV
	break;
	
	case 'F' :
	if(stationName()[1]=='O' &&
	   (sector==25 || sector==26 || sector==57 || sector==58) )
        {
	    if (sector%2)  return (sector<=31)? false : true;    //RO
	    else           return (sector<=31)? true  : false;   //HV
	}
	
	if (sector%2)  return (sector<=31)? true  : false;   //RO
	else           return (sector<=31)? false : true;    //HV
	break;
	
	case 'G' :
	if(stationName()[1]=='O' && stationName()[3] != '8' &&
	   (sector==25 || sector==26 || sector==57 || sector==58) )
        {
	    if (sector%2)  return (sector<=31)? false : true;    //RO
	    else           return (sector<=31)? true  : false;   //HV
	}
	
	if (sector%2)  return (sector<=31)? true  : false;   //RO
	else           return (sector<=31)? false : true;    //HV
	break;
	
	default: return false; 
    }
}

