/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "RPCcablingInterface/RDOindex.h"
#include "MuonCablingTools/RPCdecoder.h"

using namespace std;


RDOindex::RDOindex(unsigned int PAD,unsigned int code) : m_ROBid(0),
                                                         m_RODid(0),
                                                         m_side(0),
                                                         m_SLid(0),
                                                         m_RXid(0),
                             m_PADid(static_cast<unsigned short int>(PAD)),
                                                         m_lvl1_code(code),
							 m_stationName(0),
							 m_stationEta(0),
							 m_stationPhi(0),
							 m_doubletR(0),
							 m_doubletZ(0),
							 m_doubletPhi(0),
							 m_hash(0),
                                                         m_status(false)
{
    set_indexes();
}


RDOindex::RDOindex(unsigned int PAD,unsigned int code,std::string Name,
                   int sEta,int sPhi,int dR,int dZ,int dP) :
		                                         m_ROBid(0),
                                                         m_RODid(0),
                                                         m_side(0),
                                                         m_SLid(0),
                                                         m_RXid(0),
                             m_PADid(static_cast<unsigned short int>(PAD)),
                                                         m_lvl1_code(code),
							 m_stationName(0),
							 m_stationEta(sEta),
							 m_stationPhi(sPhi),
							 m_doubletR(dR),
							 m_doubletZ(dZ),
							 m_doubletPhi(dP),
							 m_hash(0),
                                                         m_status(false)
{
#ifndef LVL1_STANDALONE
    if(s_rpcHelper) m_stationName = s_rpcHelper->stationNameIndex(Name);
#endif
    set_indexes();
}

RDOindex::RDOindex(const RDOindex& index)
{
    m_ROBid     = index.ROBid();
    m_RODid     = index.RODid();
    m_side      = index.side();
    m_SLid      = index.SLid();
    m_RXid      = index.RXid();
    m_PADid     = index.PADid();
    m_lvl1_code = index.lvl1_code();
    
    m_stationName = index.stationName();
    m_stationEta  = index.stationEta();
    m_stationPhi  = index.stationPhi();
    m_doubletR    = index.doubletR();
    m_doubletZ    = index.doubletZ();
    m_doubletPhi  = index.doubletPhi();
    
    m_hash = index.hash();
    
    m_status    = index.status();
}
    
void 
RDOindex::set_indexes(void)
{
    RPCdecoder decode(m_lvl1_code);
    if(decode)
    {
	unsigned int sector = decode.logic_sector();
	m_RXid   = (sector%32)%2;
	m_SLid   = sector%32;
	m_side   = (sector<32)? 0x66 : 0x65;
	m_RODid  = (sector%32)/2;
	m_ROBid  = (sector%32)/2;
        m_status = true;
    }
}


RDOindex::~RDOindex() {}


RDOindex::operator bool()
{
    return m_status;
}

bool
RDOindex::operator !()
{
    return !m_status;
}

RDOindex&
RDOindex::operator=(const RDOindex& index)
{
    if (this!=&index) {
      m_ROBid     = index.ROBid();
      m_RODid     = index.RODid();
      m_side      = index.side();
      m_SLid      = index.SLid();
      m_RXid      = index.RXid();
      m_PADid     = index.PADid();
      m_lvl1_code = index.lvl1_code();

      m_stationName = index.stationName();
      m_stationEta  = index.stationEta();
      m_stationPhi  = index.stationPhi();
      m_doubletR    = index.doubletR();
      m_doubletZ    = index.doubletZ();
      m_doubletPhi  = index.doubletPhi();

      m_hash = index.hash();

      m_status    = index.status();
    }
    return *this;
}

void 
RDOindex::set_hash(unsigned int h) {
    m_hash = h;
}

#ifndef LVL1_STANDALONE

//const RPCGeometry* RDOindex::s_rpcgeo    = 0;
const RpcIdHelper* RDOindex::s_rpcHelper = 0;


void
RDOindex::setRpcIdHelper(const RpcIdHelper* helper) {
    s_rpcHelper = helper;
}


void 
RDOindex::offline_indexes(int& name, int& eta, int& phi,
                         int& doublet_r, int& doublet_z, int& doublet_phi,
                         int& gas_gap, int& measures_phi, int& strip) const
{
    if(m_status)
    {
        //RPCgeometrySvc*  geoSvc;
        //ISvcLocator* svcLoc = Gaudi::svcLocator( );
        //svcLoc->service( "RPCgeometrySvc", geoSvc );

        if(m_stationPhi)
	{
	    name         = m_stationName;
	    eta          = m_stationEta;
	    phi          = m_stationPhi;
	    doublet_r    = m_doubletR;
	    doublet_z    = m_doubletZ;
	    doublet_phi  = m_doubletPhi;
	    gas_gap      = 1;
	    measures_phi = 0;
	    strip        = 1;
	} //else {
//            int Jtyp;

//            s_rpcgeo->give_strip_index(m_lvl1_code,Jtyp,eta,phi,doublet_r,
//                                   doublet_z,doublet_phi,gas_gap,measures_phi,
//                                   strip);

//            name = Jtyp - 1;
            //name = muon_station_name_index(Jtyp -1);
//	}
    }
}

void
RDOindex::pad_identifier(Identifier& id ) const
{
    if(m_status)
    {   
        int name = 0;
        //int Jtyp;
        int eta  = 0;
	int phi  = 0;
	int doublet_r = 0;
        int doublet_z = 0;
	int doublet_phi = 0;
	// int gas_gap = 0;
	// int measures_phi = 0;
        // int strip = 0;
	
        if(m_stationPhi)
	{
	    name         = m_stationName;
	    eta          = m_stationEta;
	    phi          = m_stationPhi;
	    doublet_r    = m_doubletR;
	    doublet_z    = m_doubletZ;
	    doublet_phi  = m_doubletPhi;
	    // gas_gap      = 1;
	    // measures_phi = 0;
	    // strip        = 1;
	} //else {	
//            s_rpcgeo->give_strip_index(m_lvl1_code,Jtyp,eta,phi,doublet_r,
//                                   doublet_z,doublet_phi,gas_gap,measures_phi,
//                                   strip);
//	    name = Jtyp - 1;
//	}   

        if(s_rpcHelper!=0)
	id = s_rpcHelper->padID(name,eta,phi,doublet_r,doublet_z,doublet_phi);
	//id = s_rpcHelper->padID(1,2,3,4,5,6);
    }
}

#endif

ostream& operator<<(ostream& stream,const RDOindex& rdo)
{
    std::stringstream tmp_stream;
#ifndef LVL1_STANDALONE

    int name; 
    int eta; 
    int phi;
    int doublet_r;
    int doublet_z;
    int doublet_phi;
    int gas_gap;
    int measures_phi;
    int strip;
    
    rdo.offline_indexes(name,eta,phi,doublet_r,doublet_z,doublet_phi,
                        gas_gap,measures_phi,strip);   
    
    tmp_stream << "RPC PAD /" << hex << showbase << rdo.side() << "/"
               << rdo.SLid() << "/" << rdo.PADid() << "   mapped on offline Id /"
	       << dec << name << "/" << eta << "/" << phi << "/" << doublet_r
	       << "/" << doublet_z << "/" << doublet_phi << "/" << gas_gap
	       << "/" << measures_phi << "/" << strip 
	       << " .... hashId = " << rdo.hash() << endl;

#else    
    std::string side = (rdo.side() == 0x66)? "Negative" : "Positive";
    tmp_stream << "/side=" << side << "/ROB=ROD=" << setw(2) << rdo.ROBid() 
               << "/SL=RX=" << setw(2) << rdo.SLid()
               << "/PAD="  << setw(2) << rdo.PADid() << "/" << endl;
#endif

    stream << tmp_stream.str();

    return stream;
}
