/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WIREDOR_H
#define WIREDOR_H

#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include "MuonCablingTools/ShowRequest.h"
#include "RPCcablingInterface/CablingObject.h"
#include "RPCcablingInterface/CMAidentity.h"

#include "MuonRPC_Cabling/RPCchamber.h"

class CMAparameters;


namespace MuonRPC_Cabling {

class SectorLogicSetup;

class WiredOR : public CablingObject
{
    public:
    
    typedef std::map < int, const RPCchamber*, std::less <int> > RPClink;
    
    
    private:
    ViewType m_side;          // strip type put in wired OR
    int  m_start;             // first RPC chamber to which wired strips belong
    int  m_stop;              // last RPC chamber to which wired strips belong

    typedef std::vector < int > ReadoutCh;
    typedef std::list < const CMAparameters* > CMAlist; 

    ReadoutCh m_even_read_mul;
    ReadoutCh m_odd_read_mul;  

    RPClink m_RPCread;
    CMAlist m_readoutCMAs;

    bool connect(SectorLogicSetup&);

    public:

    WiredOR();
    WiredOR(int,int,int,int,int);
    WiredOR(const WiredOR&);
    ~WiredOR();

    WiredOR& operator=(const WiredOR&);
    
    ViewType side(void) const {return m_side;}
    int start(void)     const {return m_start;}
    int stop(void)      const {return m_stop;}
    const ReadoutCh&  even_read_mul(void) const {return m_even_read_mul;}
    const ReadoutCh&  odd_read_mul(void)  const {return m_odd_read_mul;} 
    const RPClink& RPCread(void)        const {return m_RPCread;}
    const CMAlist& readoutCMAs(void)    const {return m_readoutCMAs;}

    const RPCchamber* connected_rpc(int) const;

    bool check(void);
    bool setup(SectorLogicSetup&);

    void add_cma(const CMAparameters*);
    void add_even_read_mul(ReadoutCh&);
    void add_odd_read_mul(ReadoutCh&);

    int give_max_phi_strips(void) const;
    int give_max_eta_strips(void) const;
    int RPCacquired(void) const {return m_RPCread.size();}

    void Print(std::ostream&,bool) const;

    void two_obj_error_message(std::string,WiredOR*);
    void error(std::string);
};

template <class X> X& operator<<(X& stream, const WiredOR& Wor)
{
    Wor.Print(stream,false);
    return stream;
}

}
#endif
