/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "MuonRPC_Cabling/WiredOR.h"
#include "MuonRPC_Cabling/SectorLogicSetup.h"

using namespace MuonRPC_Cabling;

WiredOR::WiredOR(int num,int stat,int type,int start,int stop) : 
CablingObject(num,stat,type,"WOR") ,
m_side(Phi),m_start(start),m_stop(stop)
{
    m_even_read_mul.clear();
    m_odd_read_mul.clear();
}


WiredOR::WiredOR(const WiredOR& Wor) :
CablingObject(Wor.number(),Wor.station(),Wor.sector_type(),Wor.name())
{ 
    m_side  = Wor.side();
    m_start = Wor.start();
    m_stop  = Wor.stop();
    m_RPCread       = Wor.RPCread();
    m_readoutCMAs   = Wor.readoutCMAs();
    m_even_read_mul = Wor.even_read_mul();
    m_odd_read_mul  = Wor.odd_read_mul();
}

WiredOR&
WiredOR::operator=(const WiredOR& Wor)
{
    if(this!=&Wor){
      m_side  = Wor.side();
      m_start = Wor.start();
      m_stop  = Wor.stop();
      m_RPCread.clear();
      m_RPCread = Wor.RPCread();
      m_readoutCMAs.clear();
      m_readoutCMAs = Wor.readoutCMAs();
      m_even_read_mul.clear();
      m_even_read_mul = Wor.even_read_mul();
      m_odd_read_mul.clear();
      m_odd_read_mul = Wor.odd_read_mul();
    }
    return*this;
}


WiredOR::~WiredOR()
{
    m_even_read_mul.clear();
    m_odd_read_mul.clear();
    m_RPCread.clear();
    m_readoutCMAs.clear();
}

bool
WiredOR::connect(SectorLogicSetup& setup)
{
    // Check if RPC chambers exist
    for(int i=m_start;i<=m_stop;++i)
    {
        RPCchamber* rpc = setup.find_chamber(this->station(),i);
	
        if(rpc)
	{
            rpc->add_wor(this);
	    m_RPCread.insert(RPClink::value_type(i,rpc));
	}
        else
	{
            this->no_connection_error("RPC",i);
            return false;
	}
    }
    return true;
}


int
WiredOR::give_max_phi_strips(void) const
{
    if(m_side != Phi) return 0;
    RPClink::const_iterator it = m_RPCread.begin();
    int max = 0;
    while(it != m_RPCread.end())
    {
	if(max < (*it).second->phi_strips()) max = (*it).second->phi_strips();
        ++ it;
    }
    return max;
}


int
WiredOR::give_max_eta_strips(void) const
{
    if(m_side != Eta) return 0;
    RPClink::const_iterator it = m_RPCread.begin();
    int max = 0;
    while(it != m_RPCread.end())
    {
	if(max < (*it).second->eta_strips()) max = (*it).second->eta_strips();
	++it;
    }
    return max;
}


const RPCchamber*
WiredOR::connected_rpc(int ord) const
{
    RPClink::const_iterator rpcs = m_RPCread.begin();
    while(rpcs != m_RPCread.end())
    {
	if(!ord) return (*rpcs).second;
        --ord;
	++rpcs;
    }
    return 0;
}
    
void 
WiredOR::add_cma(const CMAparameters* cma)
{
    //cout << *cma;
    m_readoutCMAs.push_back(cma);
    //cout << *this << endl;
}


void 
WiredOR::add_even_read_mul(ReadoutCh& mul)
{
    for(int i=0;i<give_max_phi_strips();++i) m_even_read_mul[i] += mul[i];
}

void 
WiredOR::add_odd_read_mul(ReadoutCh& mul)
{
    for(int i=0;i<give_max_phi_strips();++i) m_odd_read_mul[i] += mul[i];
}

bool
WiredOR::setup(SectorLogicSetup& setup)
{
    WiredOR* prev = setup.previousWOR(*this);
    if(prev)
    {
        if(!(this->start() == prev->stop() + 1))
	{
            this->two_obj_error_message("boundary inconsistence",prev); 
            return false;
	}
    }
 
    if( !this->connect(setup) ) return false;

    int ch = (m_side == Eta)? give_max_eta_strips() : give_max_phi_strips();
    m_even_read_mul.resize(ch);
    m_odd_read_mul.resize(ch);
    for (int i = 0; i<ch; ++i) {m_even_read_mul.push_back(0);}
    for (int i = 0; i<ch; ++i) {m_odd_read_mul.push_back(0);}
    return true;
}


bool
WiredOR::check(void)
{
    
    WiredOR::CMAlist::const_iterator cma = m_readoutCMAs.begin();
    const CMAinput IO = (*cma)->whichCMAinput(this->station());


    int ch = (m_side == Eta)? give_max_eta_strips() : give_max_phi_strips();
    for(int i = 0; i < ch; ++i) 
    {
        if(!m_even_read_mul[i]) 
        {
	  error("==> No readout coverage for the full set of even PHI strip!");
          return false;
	}
        if(!m_odd_read_mul[i])
        {
	  error("==> No readout coverage for the full set of odd PHI strip!");
          return false;
	}
        if(m_even_read_mul[i] > 1 && IO == Pivot)
	{
            error("==> Pivot plane even PHI strips must be read only once!");
            return false;
	}
        if(m_odd_read_mul[i] > 1 && IO == Pivot)
	{
            error("==> Pivot plane odd PHI strips must be read only once!");
            return false;
	}        
        if(m_even_read_mul[i] > 2)
	{
          error("==> Confirm plane even PHI strips can be read only 3 times!");
          return false;
	}
        if(m_odd_read_mul[i] > 1 && IO == Pivot)
	{
          error("==> Confirm plane odd PHI strips can be read only 3 times");
          return false;
	}
    }

    return true;
}


void
WiredOR::Print(std::ostream& stream,bool detail) const
{
    stream << " wired OR n. " << std::setw(2) << number();
    stream << "  (stat " << station() << ")";
    stream << "   connects RPC chamber n. " << std::setw(2) << start();
    stream << " to RPC chamber n. " << std::setw(2) << stop() << std::endl;

    if(detail)
    {
        stream << " It reads " << RPCacquired()
               << " RPC phi pannel:" << std::endl;
        RPClink::const_iterator rpc = m_RPCread.begin();
        while(rpc != m_RPCread.end()) 
        {
            stream << *(*rpc).second; 
            ++rpc;
        }

        stream << " It gives input to " << m_readoutCMAs.size()
               << " Phi CMA:" << std::endl;
        CMAlist::const_iterator cma = m_readoutCMAs.begin();
        while(cma != m_readoutCMAs.end()) 
        {
            stream << *(*cma); 
            ++cma;
        }
        int ch = give_max_phi_strips();
        stream << " Phi even sector readout multiplicity:" << std::endl;
        stream << "1   5    10   15   20   25   30   35   40   "
               << "45   50   55   60   65   70   75" << std::endl;
        stream << "|   |    |    |    |    |    |    |    |    " 
               << "|    |    |    |    |    |    |" << std::endl;
        for(int i = 0;i < ch;++i) stream << m_even_read_mul[i];
        stream << std::endl;
        stream << " Phi odd sector readout multiplicity:" << std::endl;
        stream << "1   5    10   15   20   25   30   35   40   "
               << "45   50   55   60   65   70   75" << std::endl;
        stream << "|   |    |    |    |    |    |    |    |    " 
               << "|    |    |    |    |    |    |" << std::endl;
        for(int i = 0;i < ch;++i) stream << m_odd_read_mul[i];
        stream << std::endl;
        stream << "========================================"
               << "=======================================" << std::endl;
    }
}

void
WiredOR::two_obj_error_message(std::string msg,WiredOR* wor)
{    
    this->error_header();

    DISP << "  " << msg << " between " << name() << " n. " << number()
         << " and " << wor->name() << " n. " << wor->number() << std::endl
         << *this
         << *wor;
    DISP_ERROR;
}

void
WiredOR::error(std::string msg)
{
    this->error_header();

    DISP << msg << std::endl
         << *this;
    DISP_ERROR;
}
