/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <fstream>

#include "RPCcablingInterface/CMAprogram.h"

#include "RPCcablingSim/EtaCMA.h"
#include "RPCcablingSim/SectorLogicSetup.h"

#ifndef LVL1_STANDALONE
#include "PathResolver/PathResolver.h"
#endif

using namespace RPCcablingSim;

EtaCMA::EtaCMA(int num,int stat,int type,CMAcoverage coverage,
              int eta,int phi,int PAD,int Ixx,
	      int pivot_station,int lowPt_station,int highPt_station,       
              int start_ch,int start_st,int stop_ch,int stop_st) :
              CMAparameters(num,stat,type,coverage,eta,phi,PAD,Ixx,
                            pivot_station,lowPt_station,highPt_station,
                            start_ch,start_st,stop_ch,stop_st,Eta) 
{
    // Set the memory for storing the cabling map
    m_pivot_rpc_read  = 1;
    m_lowPt_rpc_read  = 1;
    m_highPt_rpc_read = 1;

    m_conf_type = CMAparameters::Simulation;

    create_pivot_map(m_pivot_rpc_read);
    create_lowPt_map(m_lowPt_rpc_read);
    create_highPt_map( m_highPt_rpc_read);
} 

EtaCMA::EtaCMA(int num,int stat,int type,int eta, int phi,
	      int lowPt_start_co,int lowPt_stop_co,int lowPt_number_co,
              int highPt_start_co,int highPt_stop_co,int highPt_number_co) :
                  CMAparameters(num,stat,type,eta,phi,
                      lowPt_start_co,lowPt_stop_co,lowPt_number_co,
                      highPt_start_co,highPt_stop_co,highPt_number_co)
{
    // Set the memory for storing the cabling map

    m_pivot_rpc_read  = 1;
    m_lowPt_rpc_read  = 1;
    m_highPt_rpc_read = 1;

    m_conf_type = CMAparameters::Simulation;

    create_pivot_map(m_pivot_rpc_read);
    create_lowPt_map(m_lowPt_rpc_read);
    create_highPt_map( m_highPt_rpc_read);
}

EtaCMA::EtaCMA(const EtaCMA& cma) : 
CMAparameters(static_cast<const CMAparameters&>(cma))
{
    m_conf_type = CMAparameters::Simulation;
    m_pivot_RPCs = cma.pivot_RPCs();
    m_lowPt_RPCs = cma.lowPt_RPCs();
    m_highPt_RPCs = cma.highPt_RPCs();
}


EtaCMA::~EtaCMA()
{
    m_pivot_RPCs.clear();
    m_lowPt_RPCs.clear();
    m_highPt_RPCs.clear();
}

EtaCMA&
EtaCMA::operator=(const EtaCMA& cma)
{
    static_cast<CMAparameters&>(*this)=static_cast<const CMAparameters&>(cma);
    m_pivot_RPCs.clear();
    m_pivot_RPCs = cma.pivot_RPCs();
    m_lowPt_RPCs.clear();
    m_lowPt_RPCs = cma.lowPt_RPCs();
    m_highPt_RPCs.clear();
    m_highPt_RPCs = cma.highPt_RPCs();
    return *this;
}

bool
EtaCMA::cable_CMA_channels(HalfType side)
{
    if(pivot_station())  //Check and connect strips with Pivot matrix channels
    {
        for(int i=pivot_start_ch();i<=pivot_stop_ch();++i)
        {
            int strip_number = (m_active_pivot_chs)? 1 : pivot_start_st();
            RPClink::iterator found = m_pivot_RPCs.find(i);
            RPCchamber* rpc = (*found).second;
            int final_strip = rpc->eta_strips();
            if ( i == pivot_stop_ch() ) final_strip = pivot_stop_st();
            do
            {
	        if(m_active_pivot_chs == pivot_channels) 
                {
                    noMoreChannels("Pivot");
                    return false;
	        }
		
		int st_effective = strip_number - 1; 
		if(!i && side == Negative) st_effective = 
		                           abs(st_effective-final_strip) -1;
		
		if(rpc->ijk_etaReadout() == 1)
		{
                m_pivot[0][0][m_active_pivot_chs]=i*100 + st_effective;
                m_pivot[0][1][m_active_pivot_chs]=10000 + i*100 + st_effective;
		}else
		{
		m_pivot[0][1][m_active_pivot_chs]=i*100 + st_effective;
                m_pivot[0][0][m_active_pivot_chs]=10000 + i*100 + st_effective;
		}
                rpc->add_eta_channel(st_effective);
	        ++m_active_pivot_chs;
            }while(++strip_number <= final_strip);
        }
	// Set first and last connectors code
	int code = m_pivot_station*100000 + 2*100000000;
	m_first_pivot_code = code + m_pivot[0][0][0];
	m_last_pivot_code  = code + m_pivot[0][0][m_active_pivot_chs-1];
    }
    if(lowPt_station() && lowPt_number_co() != -1)
    {
	for(int i=lowPt_start_ch();i<=lowPt_stop_ch();++i)
	{
            int strip_number = (m_active_lowPt_chs)? 1 : lowPt_start_st();
            RPClink::iterator found = m_lowPt_RPCs.find(i);
            RPCchamber* rpc = (*found).second;
            int final_strip = rpc->eta_strips();
            if ( i == lowPt_stop_ch() ) final_strip = lowPt_stop_st();
            do
            {   
		if(m_active_lowPt_chs == confirm_channels) 
                {
                    noMoreChannels("Low Pt");
                    return false;
	        }
		
		int st_effective = strip_number - 1; 
		if(!i && side == Negative) st_effective = 
		                           abs(st_effective-final_strip) -1;
		
		if(rpc->ijk_etaReadout() == 1)
		{
                m_lowPt[0][0][m_active_lowPt_chs]=i*100 + st_effective;
                m_lowPt[0][1][m_active_lowPt_chs]=10000 + i*100 + st_effective;
		}else
		{
		m_lowPt[0][1][m_active_lowPt_chs]=i*100 + st_effective;
                m_lowPt[0][0][m_active_lowPt_chs]=10000 + i*100 + st_effective;
		}
                rpc->add_eta_channel(st_effective);
	        ++m_active_lowPt_chs;
            }while(++strip_number <= final_strip);
	}
	// Set first and last connectors code	
	int code = m_lowPt_station*100000 + 2*100000000;
	m_first_lowPt_code = code + m_lowPt[0][0][0];
	m_last_lowPt_code  = code + m_lowPt[0][0][m_active_lowPt_chs-1];
    }
    if(highPt_station() && highPt_number_co() != -1)
    {
	for(int i=highPt_start_ch();i<=highPt_stop_ch();++i)
	{
            int strip_number = (m_active_highPt_chs)? 1 : highPt_start_st();
            RPClink::iterator found = m_highPt_RPCs.find(i);
            RPCchamber* rpc = (*found).second;
            int final_strip = rpc->eta_strips();
            if ( i == highPt_stop_ch() ) final_strip = highPt_stop_st();
            do
            {
	        if(m_active_highPt_chs == confirm_channels) 
                {
                    noMoreChannels("High Pt");
                    return false;
	        }
		
		int st_effective = strip_number - 1; 
		if(!i && side == Negative) st_effective = 
		                           abs(st_effective-final_strip) -1;
		
		if(rpc->ijk_etaReadout() == 1)
		{
                m_highPt[0][0][m_active_highPt_chs]=i*100 + st_effective;
                m_highPt[0][1][m_active_highPt_chs]=10000+i*100 + st_effective;
		} else
		{
		m_highPt[0][1][m_active_highPt_chs]=i*100 + st_effective;
                m_highPt[0][0][m_active_highPt_chs]=10000+i*100 + st_effective;
		}
                rpc->add_eta_channel(st_effective);
	        ++m_active_highPt_chs;
            }while(++strip_number <= final_strip);
	}
	// Set first and last connectors code
	int code = m_highPt_station*100000 + 2*100000000;
	m_first_highPt_code = code + m_highPt[0][0][0];
	m_last_highPt_code  = code + m_highPt[0][0][m_active_highPt_chs-1];	
    }
    return true;
}


bool
EtaCMA::connect(SectorLogicSetup& setup)
{
    if(pivot_station())  //Check and connect Pivot chambers
    {
        for(int i=pivot_start_ch();i<=pivot_stop_ch();++i)
        {   
            RPCchamber* rpc = setup.find_chamber(m_pivot_station,i);
            if(rpc)
	    {
	        rpc->add_cma(this);
	        m_pivot_RPCs.insert(RPClink::value_type(i,rpc));
	    }
            else 
            {
                this->no_connection_error("RPC",i);
                return false;
            }
        }
    }

    if(lowPt_station() && lowPt_number_co() != -1)
    {   // Check and connect Low Pt chambers
        if(got_confirm_cabling(setup,lowPt_station()))
	{
	    ++m_lowPt_start_st;   // RPC strips starts from 1!
            for(int i = m_lowPt_start_ch; i <= m_lowPt_stop_ch;++i)
            {   
                RPCchamber* rpc = setup.find_chamber(lowPt_station(),i);
                if(rpc)
	        {
	            rpc->add_cma(this);
	            m_lowPt_RPCs.insert(RPClink::value_type(i,rpc));
	        }
                else 
                {
                    this->no_connection_error("RPC",i);
                    return false;
                }
                if(i == m_lowPt_stop_ch) m_lowPt_stop_st += 
                                                     rpc->strips_in_Eta_Conn();
            }
	}
        else
	{
            no_confirm_error(lowPt_station());
            return false;
	}
    }
   
    if(highPt_station() && highPt_number_co() != -1)
    {   // Check and connect High Pt chambers
        if(got_confirm_cabling(setup,highPt_station()))
	{
	    ++m_highPt_start_st;   // RPC strips starts from 1!
            for(int i = m_highPt_start_ch;i <= m_highPt_stop_ch;++i)
            {   
                RPCchamber* rpc = setup.find_chamber(highPt_station(),i);
                if(rpc)
	        {
	            rpc->add_cma(this);
	            m_highPt_RPCs.insert(RPClink::value_type(i,rpc));
	        }
                else 
                {
                    this->no_connection_error("RPC",i);
                    return false;
                }
                if(i == m_highPt_stop_ch) m_highPt_stop_st += 
                                                     rpc->strips_in_Eta_Conn();
            }
        }
	else
	{
            no_confirm_error(highPt_station());
            return false;
	}
    }
    return true;
}

bool
EtaCMA::got_confirm_cabling(SectorLogicSetup& setup,int stat)
{
  int l;
  if(stat == lowPt_station())
  {
      bool result = setup.local_conn_add
          (Eta,stat,lowPt_start_co(),l,m_lowPt_start_ch,m_lowPt_start_st) &&
             setup.local_conn_add
	  (Eta,stat,lowPt_stop_co(),l,m_lowPt_stop_ch,m_lowPt_stop_st);
      if ((!m_lowPt_start_ch || !m_lowPt_stop_ch) && setup.side() == NoHalf)
      {
           DISP << "Sector Type must belong to a specific side when"
	        << " RPC chamber is in between eta 0" << std::endl;
	   DISP_ERROR;
	   return false;
      }
      return result;
  }
  else if (stat == highPt_station())
  {
      bool result = setup.local_conn_add
          (Eta,stat,highPt_start_co(),l,m_highPt_start_ch,m_highPt_start_st) &&
             setup.local_conn_add
	  (Eta,stat,highPt_stop_co(),l,m_highPt_stop_ch,m_highPt_stop_st);
      if ( (!m_highPt_start_ch || !m_highPt_stop_ch)&& setup.side() == NoHalf)
      {
           DISP << "Sector Type must belong to a specific side when"
	        << " RPC chamber is in between eta 0" << std::endl;
	   DISP_ERROR;
	   return false;
      }
      return result;
  
  
  }
  else 
  {
      DISP << "Station n. " << stat 
           << " don't give input to CMA confirm planes!" << std::endl;
      DISP_ERROR;
      return false;
  }
}

bool
EtaCMA::setup(SectorLogicSetup& setup)
{
    // Connect the CMA with RPC chambers
    if( !this->connect(setup) ) return false;

    // Check boundary of CMA channels
    EtaCMA* prev = setup.previousCMA(*this);
    if(prev && pivot_station())
    {
        if (this->pivot_start_ch() == prev->pivot_stop_ch())
	{
	    if(!(this->pivot_start_st() == prev->pivot_stop_st() + 1))
	    {
                this->two_obj_error_message("strips mismatch",prev);
                return false;
	    }
	}
        else if (!(this->pivot_start_ch() == prev->pivot_stop_ch() + 1))
	{
            this->two_obj_error_message("chambers mismatch",prev);
	    return false;
	}
        else
	{
	    if(!prev->end_at_RPC_Z_boundary()|| !begin_at_RPC_Z_boundary())
	    {
               this->two_obj_error_message("boundary mismatch",prev);
	       return false;
	    }
        }
    }

    // Build the cabling map
    if( !this->cable_CMA_channels(setup.side()) ) return false;
    
    std::string CMAdir;
    if (setup.layout()=="P03")
    {
	CMAdir = "P03CMAprograms.data";
    } else if (setup.layout()=="H8")
    {
	CMAdir = "P03CMAprograms.data";
    } else if (setup.layout()=="900GeV")
    {
        CMAdir = "900GeVCMAprograms.data";
    } else
    {
	CMAdir = "CMAprograms.data";
    }
    
    // Read the program file if exist
    SectorLogicSetup::SECTORlist sectors = setup.sectors();
    SectorLogicSetup::SECTORlist::const_iterator it = sectors.begin();

    std::ifstream CMAprogLow;
    
    while (!CMAprogLow.is_open() && it != sectors.end())
    {
        char name[200];
#if (__GNUC__) && (__GNUC__ > 2) 
        // put your gcc 3.2 specific code here
        __osstream namestr;
#ifdef LVL1_STANDALONE
        namestr << "./" << CMAdir << "/MATsec" << *it << "lh" << 0 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog"<< std::ends;
#else
        std::string dir;
        dir = PathResolver::find_directory (CMAdir, "DATAPATH");
        namestr << dir << "/MATsec" << *it << "lh" << 0 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog"<< std::ends;
#endif
	//cout << "namestr=" << namestr.str() << endl;
	namestr.str().copy(name,namestr.str().length(),0);
	name[namestr.str().length()] = 0;
	//cout << "name=" << name << endl;
#else
        // put your gcc 2.95 specific code here
        __osstream namestr (name,200);
#ifdef LVL1_STANDALONE
        namestr << "./" << CMAdir << "/MATsec" << *it << "lh" << 0 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog" << std::ends;
#else
        std::string dir;
        dir = PathResolver::find_directory (CMAdir, "DATAPATH");
        namestr << dir << "/MATsec" << *it << "lh" << 0 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog"<< std::ends;
#endif

#endif
            
	CMAprogLow.open(name);
        //CMAprogLow.open(name);
        ++it;
	// namestr.freeze(0);
        namestr.clear();
    }

    if (CMAprogLow.is_open()) 
    {
        //cout << "programma Low Aperto" << endl;
        CMAprogram* program = new CMAprogram(CMAprogLow);
        if(program->check()) m_lowPt_program = program;
        else delete program;
	CMAprogLow.close();
	if (setup.lowpt_1031()) {
	    m_lowPt_program->copy_over(0,1);
	    m_lowPt_program->open_threshold(0);
	}
    }


    it = sectors.begin();
    //cout << "sto leggendo il file high" << endl;
    std::ifstream CMAprogHigh;
    while (!CMAprogHigh.is_open() && it != sectors.end())
    {
        char name[200];
#if (__GNUC__) && (__GNUC__ > 2) 
        // put your gcc 3.2 specific code here
        __osstream namestr;
#ifdef LVL1_STANDALONE
        namestr << "./" << CMAdir << "/MATsec" << *it << "lh" << 1 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog" << std::ends;
#else
        std::string dir;
        dir = PathResolver::find_directory (CMAdir, "DATAPATH");
        namestr << dir << "/MATsec" << *it << "lh" << 1 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog" << std::ends;
#endif
	namestr.str().copy(name,namestr.str().length(),0);
	name[namestr.str().length()] = 0;
#else
        // put your gcc 2.95 specific code here
        __osstream namestr (name,200);
#ifdef LVL1_STANDALONE
        namestr << "./" << CMAdir << "/MATsec" << *it << "lh" << 1 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog" << std::ends;
#else
        std::string dir;
        dir = PathResolver::find_directory (CMAdir, "DATAPATH");
        namestr << dir << "/MATsec" << *it << "lh" << 1 << "pj" <<0 
                << "add" << id().eta_index() << "_" << id().phi_index() 
                << ".prog" << std::ends;
#endif

#endif
        CMAprogHigh.open(name);
        ++it;
	// namestr.freeze(0);
        namestr.clear();
    }   

    if (CMAprogHigh.is_open()) 
    {
        CMAprogram* program = new CMAprogram(CMAprogHigh);
        if(program->check()) m_highPt_program = program;
	else {delete program;}
	CMAprogHigh.close();
	//cout << "programma High Aperto" << endl;
    }

    return true;
}

bool
EtaCMA::end_at_RPC_Z_boundary(void) const
{
    RPClink::const_iterator found = m_pivot_RPCs.find(pivot_stop_ch());
    if((*found).second->eta_strips() == pivot_stop_st()) return true;
    return false;
}

bool
EtaCMA::begin_at_RPC_Z_boundary(void) const
{
    if(pivot_start_st() == 1) return true;
    return false;
}
