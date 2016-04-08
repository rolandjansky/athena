/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCHAMBER_H
#define RPCCHAMBER_H

#include <list>
#include <vector>
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/RPCdecoder.h"
#include "RPCcablingInterface/CablingObject.h"


namespace MuonRPC_Cabling {

class EtaCMA;
class WiredOR;
class SectorLogicSetup;


class RPCchamber : public CablingObject
{
    private:

    typedef std::vector < int > ReadoutCh;    
    typedef std::list < const EtaCMA* > CMAlist;
    typedef std::list < const WiredOR* > WORlist;

    int m_strips_in_Eta_Conn; // Number of eta strips into connectors
    int m_strips_in_Phi_Conn; // Number of phi strips into connectors

    std::string m_chamber_name;  // RPC chamber name
    int m_stationEta;            // StaionEta as defined into the offline ID
    int m_doubletR;              // DoubletR as defined into the offline ID
    int m_doubletZ;              // DoubletZ as defined into the offline ID
    int m_phiReadoutPannels;     // Phi readout multiplicity

    int m_eta_strips;        // Number of eta strips
    int m_eta_connectors;    // Number of eta connectors
    int m_eta_strip_global;  // Start number of eta strips in global coordinate
    int m_eta_conn_global;   // Start number of eta connector in global coo.
    int m_ijk_etaReadout;    // readout schema of the gas gaps in eta:
                             //     01 = gap 0 in layer 0, gap 1 in layer 1
			     //     10 = gap 0 in layer 1, gap 1 in layer 0
			     
    
    int m_phi_strips;        // Number of phi strips
    int m_phi_connectors;    // Number of phi connectors
    int m_ijk_phiReadout;    // readout schema of the gas gaps in phi:
                             //     01 = gap 0 in layer 0, gap 1 in layer 1
			     //     10 = gap 0 in layer 1, gap 1 in layer 0
			     

    ReadoutCh m_eta_read_mul;


    CMAlist m_readoutCMAs;
    WORlist m_readoutWORs;

    int residual(ViewType,int) const;
    void error(std::string);

    public:
    RPCchamber(int,int,int,std::string,int,int,int,int,
               int,int,int,int,//unsigned int,unsigned int,
	       int,int,int,int);//unsigned int,unsigned int);
    RPCchamber(const RPCchamber&);
    ~RPCchamber();

    RPCchamber& operator=(const RPCchamber&);
    
    void set_eta_st_global(int);
    void set_eta_co_global(int);

    int eta_strips(void)        const {return m_eta_strips;}
    int eta_connectors(void)    const {return m_eta_connectors;}
    int eta_strip_global(void)  const {return m_eta_strip_global;}
    int eta_conn_global(void)   const {return m_eta_conn_global;}
    int phi_strips(void)        const {return m_phi_strips;}

    int phi_connectors(void)    const {return m_phi_connectors;}
    
    int ijk_etaReadout(void)    const {return m_ijk_etaReadout;}
    int ijk_phiReadout(void)    const {return m_ijk_phiReadout;}
    
    std::string chamber_name(void) const {return m_chamber_name;}
    std::string stationName(void)  const {return m_chamber_name.substr(0,3);}
    int stationEta(void)           const {return m_stationEta;}
    int doubletR(void)             const {return m_doubletR;}
    int doubletZ(void)             const {return m_doubletZ;}
    int phiReadoutPannels(void)    const {return m_phiReadoutPannels;}
    
    int strips_in_Eta_Conn(void)   const {return m_strips_in_Eta_Conn;}
    int strips_in_Phi_Conn(void)   const {return m_strips_in_Phi_Conn;}

    const ReadoutCh& eta_read_mul(void)     const {return m_eta_read_mul;}

    const CMAlist& readoutCMAs(void)  const {return m_readoutCMAs;}
    const WORlist& readoutWORs(void)  const {return m_readoutWORs;}
   
    std::string extendedName(int) const;
    bool inversion (int) const;
   
    bool local_strip(ViewType,int,int&) const;
    bool global_strip(ViewType,HalfType,int,int&) const;

    bool local_connector(ViewType,int,int&,int&,int&) const;
    bool global_connector(ViewType,HalfType,int,int&,int&,int&) const;

    bool Gstrip_2_Lnumber(ViewType,int,int&) const;
    bool Gconn_2_Lnumber(ViewType,int,int&,int&) const;
    
    void Print(std::ostream&, bool) const;

    bool check(void);
    bool setup(SectorLogicSetup&);

    void add_cma(const EtaCMA*);
    void add_wor(const WiredOR*);
    void add_eta_channel(int);
};

template <class X> X& operator<<(X& stream, const RPCchamber& cham )
{
    cham.Print(stream,false);
    return stream;
}

}
#endif
