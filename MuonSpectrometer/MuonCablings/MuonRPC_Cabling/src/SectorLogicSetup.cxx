/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "MuonRPC_Cabling/SectorLogicSetup.h"

using namespace MuonRPC_Cabling;
const std::map<std::string, std::string>* MuonRPC_Cabling::SectorLogicSetup::p_trigroads = 0; 
SectorLogicSetup::SectorLogicSetup() : BaseObject(Logic,"Sector Logic Map"),
    m_positive_sector(""),m_negative_sector(""),m_sector_type(0),
    m_online_database(""),m_layout(""),m_cosmic(false)
{
}

SectorLogicSetup::SectorLogicSetup(int type,std::string database,
                                            std::string layout,bool conf) : 
    BaseObject(Logic,"Sector Logic Map"),
    m_positive_sector(""),m_negative_sector(""),m_sector_type(type)
{
    m_online_database = database;
    m_layout = layout;
    m_cosmic = conf;
}

SectorLogicSetup::SectorLogicSetup(const SectorLogicSetup& sec) :
    BaseObject(Logic,"Sector Logic Map")
{
    m_positive_sector = sec.positive_sector();
    m_negative_sector = sec.negative_sector(); 
    m_sector_type = sec.sector_type();
    m_stations = sec.giveStations();

    m_sectors = sec.sectors();

    RPCs    = sec.giveRPC();
    WORs    = sec.giveWOR();
    etaCMAs = sec.giveEtaCMA();
    evenphiCMAs = sec.giveEvenPhiCMA();
    oddphiCMAs  = sec.giveOddPhiCMA();
    
    m_online_database = sec.online_database();
    m_layout = sec.layout();
    m_cosmic = sec.cosmic();
}

SectorLogicSetup&
SectorLogicSetup::operator=(const SectorLogicSetup& sec)
{
    if (this!=&sec) {
      m_positive_sector = sec.positive_sector();
      m_negative_sector = sec.negative_sector();    
      m_sector_type = sec.sector_type();

      m_stations.clear();
      m_stations = sec.giveStations();

      m_sectors.clear();
      m_sectors = sec.sectors();

      RPCs.clear(); 
      RPCs = sec.giveRPC();
      WORs.clear();
      WORs = sec.giveWOR();

      etaCMAs.clear();
      etaCMAs = sec.giveEtaCMA();
      evenphiCMAs.clear();
      evenphiCMAs = sec.giveEvenPhiCMA();
      oddphiCMAs.clear();
      oddphiCMAs = sec.giveOddPhiCMA();
     
      m_online_database = sec.online_database();
      m_layout = sec.layout();
      m_cosmic = sec.cosmic();
    }

    return*this;
}

SectorLogicSetup::~SectorLogicSetup()
{
    m_stations.clear();
    m_sectors.clear();
    RPCs.clear();
    WORs.clear();
    etaCMAs.clear();
    evenphiCMAs.clear();
    oddphiCMAs.clear();
}

void
SectorLogicSetup::no_elements(std::string tech,int stat)
{
    __osstream disp;

    disp << "No " << tech << " elements for Sector Type " << m_sector_type;
    if (stat) disp << ", station " << stat << "!" << std::endl;
    else disp << "!" << std::endl;

    display_error(disp);
}

void
SectorLogicSetup::get_station(CMAinput type,int station)
{
    if(!station) return;
    typedef StationMap::const_iterator cIter;
    std::pair<cIter,cIter> p = m_stations.equal_range(type);
    for(cIter it = p.first;it != p.second;++it)
    {
        if ((*it).second == station) return;
    }
    m_stations.insert(StationMap::value_type(type,station));
}

bool
SectorLogicSetup::setup(void)
{
    //std::cout << "SectorLogicSetup::setup ==> Il settore tipo e` " << sector_type()
    //     << std::endl; 
    //PrintElement(cout,0,"",false);


    typedef StationMap::const_iterator sIter;

    bool RPCsetup = true;
    ////////////////////////////////////////// <-------------- RPCs setup
    for (sIter it = m_stations.begin();it != m_stations.end(); ++it)
    {
        int key  = (*it).second * 100;
        //int size = distance(RPCs.lower_bound(key),RPCs.upper_bound(key+99));
	RPCmap::iterator lower = RPCs.lower_bound(key);
	RPCmap::iterator upper = RPCs.upper_bound(key + 99);
	int size = distance(lower,upper);
	//int ijk_eta = (*lower).second.ijk_etaReadout();
	//int ijk_phi = (*lower).second.ijk_phiReadout();
        if(size)
	{
	    for (RPCmap::iterator rpc = lower; rpc != upper;++rpc)
	        RPCsetup &= (*rpc).second.setup(*this);
	}
	else {no_elements("RPC",(*it).second); RPCsetup = false;}
    }
    bool WORsetup = true;
    ////////////////////////////////////////// <-------------- WORs setup
    for(sIter it = m_stations.begin();it != m_stations.end(); ++it)
    {
        int key  = (*it).second * 100;
	WORmap::iterator lower = WORs.lower_bound(key);
        WORmap::iterator upper = WORs.upper_bound(key + 99);
        int size = distance(lower,upper);
        if(size) 
        {
            for(WORmap::iterator wor = lower;wor != upper;++wor)
	        WORsetup &= (*wor).second.setup(*this);
	}
        else {no_elements("WOR",(*it).second); WORsetup = false;}
    }

    bool EtaCMAsetup = true;
    bool EvenPhiCMAsetup = true;
    bool OddPhiCMAsetup = true;
    ////////////////////////////////////////// <-------------- CMAs setup    
    if(etaCMAs.size()) 
    {
        SectorLogicSetup::EtaCMAmap::iterator it;
        for(it = etaCMAs.begin();it != etaCMAs.end();++it)
	{
	    EtaCMAsetup &= (*it).second.setup(*this);
	    //if((*it).second.inversion()) (*it).first.inversion();
	}

    }
    else {no_elements("EtaCMA",0); EtaCMAsetup = false;}

    if(evenphiCMAs.size()) 
    {
        SectorLogicSetup::EvenPhiCMAmap::iterator it;
        for(it = evenphiCMAs.begin();it != evenphiCMAs.end();++it)
	    EvenPhiCMAsetup &= (*it).second.setup(*this);
    }
    // removed checks on existence of PhiCMAs. Some sectors now cover only 1 side
    //else {no_elements("EvenPhiCMA",0); EvenPhiCMAsetup = false;}

    if(oddphiCMAs.size()) 
    {
        SectorLogicSetup::OddPhiCMAmap::iterator it;
        for(it = oddphiCMAs.begin();it != oddphiCMAs.end();++it)
	    OddPhiCMAsetup &= (*it).second.setup(*this);
    }
    // removed checks on existence of PhiCMAs. Some sectors now cover only 1 side
    //else {no_elements("OddPhiCMA",0); OddPhiCMAsetup = false;}

    return RPCsetup & WORsetup & EtaCMAsetup & EvenPhiCMAsetup &OddPhiCMAsetup;
}

bool
SectorLogicSetup::check(void)
{
    typedef StationMap::const_iterator sIter;
    typedef RPCmap::iterator rIter;
    typedef WORmap::iterator wIter;
    std::pair < sIter,sIter > pivot = m_stations.equal_range(Pivot);

    bool RPCcheck = true;
    ////////////////////////////////////////// <-------------- RPCs check
    for(sIter stat = pivot.first;stat != pivot.second;++stat)
    {
        int Lkey = (*stat).second * 100;
        int Ukey = (*stat).second * 100 + 99;
        for(rIter it=RPCs.lower_bound(Lkey);it != RPCs.upper_bound(Ukey);++it)
	{
	    RPCcheck &= (*it).second.check();
	}
    }

    bool WORcheck = true;
    ////////////////////////////////////////// <-------------- WORs check
    for(sIter stat = pivot.first;stat != pivot.second;++stat)
    {
        int Lkey = (*stat).second * 100;
        int Ukey = (*stat).second * 100 + 99;
        for(wIter it=WORs.lower_bound(Lkey);it != WORs.upper_bound(Ukey);++it) 
        {
	  //WORcheck &= (*it).second.check();
	}
    }

    return RPCcheck & WORcheck;
}

RPCchamber*
SectorLogicSetup::find_chamber(int stat, int num)
{
    RPCmap::iterator result = RPCs.find(stat*100 + num);
    if (result != RPCs.end() ) return &(*result).second;
    else return 0;
}

const RPCchamber*
SectorLogicSetup::find_chamber(int stat, int num) const
{
    RPCmap::const_iterator result = RPCs.find(stat*100 + num);
    if (result != RPCs.end() ) return &(*result).second;
    else return 0;
}


WiredOR*
SectorLogicSetup::find_wor(int stat, int num)
{
    WORmap::iterator result = WORs.find(stat*100 + num);
    if (result != WORs.end() ) return &(*result).second;
    else return 0;
}

WiredOR* 
SectorLogicSetup::previousWOR(const WiredOR& wor)
{
    WORmap::iterator result = WORs.find(wor.station()*100 + wor.number());
    WORmap::iterator begin  = WORs.lower_bound(wor.station()*100);
    if(result == WORs.end() || result == begin) return 0;
    --result;
    return &(*result).second;
}

EtaCMA* 
SectorLogicSetup::previousCMA(const EtaCMA& cma)
{
    EtaCMAmap::iterator result = etaCMAs.find(cma.id());
    if(result == etaCMAs.end() || result == etaCMAs.begin()) return 0;
    --result;
    return &(*result).second;
}


EvenPhiCMA* 
SectorLogicSetup::previousCMA(const EvenPhiCMA& cma)
{
    EvenPhiCMAmap::iterator result = evenphiCMAs.find(cma.id());
    if(result == evenphiCMAs.end() || result == evenphiCMAs.begin()) return 0;
    --result;
    return &(*result).second;
}

OddPhiCMA* 
SectorLogicSetup::previousCMA(const OddPhiCMA& cma)
{
    OddPhiCMAmap::iterator result = oddphiCMAs.find(cma.id());
    if(result == oddphiCMAs.end() || result == oddphiCMAs.begin()) return 0;
    --result;
    return &(*result).second;
}

std::list < const EtaCMA* >
SectorLogicSetup::find_eta_CMAs_in_PAD(int pad)
{
    std::list < const EtaCMA* > CMAlist;
    EtaCMAmap::const_iterator it = etaCMAs.begin();
    while(it != etaCMAs.end())
    {
        const EtaCMA* cma = &(*it).second;
	if(cma->id().PAD_index() == pad) CMAlist.push_back(cma);
        ++it;
    }
    return CMAlist;
}


SectorLogicSetup::EvenPhiCMAmap::const_iterator
SectorLogicSetup::find_evenphiCMA(int PAD,int Ixx) const
{
    EvenPhiCMAmap::const_iterator it = evenphiCMAs.begin();
    while(it != evenphiCMAs.end())
    {
        const EvenPhiCMA* cma = &(*it).second;
	if(cma->id().PAD_index() == PAD && 
	   cma->id().Ixx_index() == Ixx) return it;
        ++it;
    }
    return evenphiCMAs.end();
}


SectorLogicSetup::OddPhiCMAmap::const_iterator
SectorLogicSetup::find_oddphiCMA(int PAD,int Ixx) const
{
    OddPhiCMAmap::const_iterator it = oddphiCMAs.begin();
    while(it != oddphiCMAs.end())
    {
        const OddPhiCMA* cma = &(*it).second;
	if(cma->id().PAD_index() == PAD && 
	   cma->id().Ixx_index() == Ixx) return it;
        ++it;
    }
    return oddphiCMAs.end();
}


SectorLogicSetup::EtaCMAmap::const_iterator
SectorLogicSetup::find_etaCMA(int PAD,int Ixx) const
{
    EtaCMAmap::const_iterator it = etaCMAs.begin();
    while(it != etaCMAs.end())
    {
        const EtaCMA* cma = &(*it).second;
	if(cma->id().PAD_index() == PAD && 
	   cma->id().Ixx_index() == Ixx) return it;
        ++it;
    }
    return etaCMAs.end();
}

SectorLogicSetup::EvenPhiCMAmap::iterator
SectorLogicSetup::find_evenphiCMA(int PAD,int Ixx)
{
    EvenPhiCMAmap::iterator it = evenphiCMAs.begin();
    while(it != evenphiCMAs.end())
    {
        const EvenPhiCMA* cma = &(*it).second;
	if(cma->id().PAD_index() == PAD && 
	   cma->id().Ixx_index() == Ixx) return it;
        ++it;
    }
    return evenphiCMAs.end();
}


SectorLogicSetup::OddPhiCMAmap::iterator
SectorLogicSetup::find_oddphiCMA(int PAD,int Ixx)
{
    OddPhiCMAmap::iterator it = oddphiCMAs.begin();
    while(it != oddphiCMAs.end())
    {
        const OddPhiCMA* cma = &(*it).second;
	if(cma->id().PAD_index() == PAD && 
	   cma->id().Ixx_index() == Ixx) return it;
        ++it;
    }
    return oddphiCMAs.end();
}


SectorLogicSetup::EtaCMAmap::iterator
SectorLogicSetup::find_etaCMA(int PAD,int Ixx)
{
    EtaCMAmap::iterator it = etaCMAs.begin();
    while(it != etaCMAs.end())
    {
        const EtaCMA* cma = &(*it).second;
	if(cma->id().PAD_index() == PAD && 
	   cma->id().Ixx_index() == Ixx) return it;
        ++it;
    }
    return etaCMAs.end();
}

bool 
SectorLogicSetup::global_strip_add(ViewType side,HalfType h_barrel,int station,
int rpc_index, int strip_number, int& global_address) const
{
    RPCmap::const_iterator result = RPCs.find(station*100 + rpc_index);
    if(result == RPCs.end()) return false;
    return (*result).second.global_strip(side,h_barrel,strip_number,
                                         global_address);
}

bool
SectorLogicSetup::local_strip_add(ViewType side,int station, 
                  int global_address, int& rpc_index, int& strip_number) const
{
    if(side == Eta)
    {
        RPCmap::const_iterator it = RPCs.lower_bound(station*100);
	while(it != RPCs.upper_bound(station*100 + 99))
	{
	    if((*it).second.Gstrip_2_Lnumber(side,global_address,strip_number))
	    {
		rpc_index = (*it).second.number();
		return true;
	    }
            ++it;
	}
    }
    if(side == Phi)
    {
	return false;
    }

    return false;
}

bool 
SectorLogicSetup::global_conn_add (ViewType side, HalfType h_barrel,
              int station,int rpc_index,int strip_number, int& global_address, 
              int& low_eta_strips, int& hi_eta_strips) const
{
    RPCmap::const_iterator result = RPCs.find(station*100 + rpc_index);
    if(result == RPCs.end()) return false;
    return (*result).second.global_connector(side,h_barrel,strip_number,
                                  global_address,low_eta_strips,hi_eta_strips);
}


bool
SectorLogicSetup::local_conn_add(ViewType side,int station, int global_address,
int& c_number,int& rpc_index, int& strip_number) const
{
    if(side == Eta)
    {
        RPCmap::const_iterator it = RPCs.lower_bound(station*100);
	while(it != RPCs.upper_bound(station*100 + 99))
	{
	    if((*it).second.Gconn_2_Lnumber(side,global_address,c_number,
                                          strip_number))
	    {
		rpc_index = (*it).second.number();
		return true;
	    }
            ++it;
	}
    }
    if(side == Phi)
    {
	return false;
    }

    return false;
}

const CMAparameters::CMAlist
SectorLogicSetup::give_CMAs(const int sector,const ViewType side,
                            const int station,const int cabling_code) const
{
    CMAparameters::CMAlist list;
    if(side == Eta)
    {
        EtaCMAmap::const_iterator it = etaCMAs.begin();
        while(it != etaCMAs.end())
	{
	    const CMAinput IO = (*it).second.whichCMAinput(station);
            const CMAparameters* cma = (*it).second.test(IO,cabling_code);
	    if(cma) list.push_back(cma);
	    ++it;
	}
    }
    else if (side == Phi)
    {
        if(sector%2)
	{//odd
            OddPhiCMAmap::const_iterator it = oddphiCMAs.begin();
            while(it != oddphiCMAs.end())
	    {
	        const CMAinput IO = (*it).second.whichCMAinput(station);
                const CMAparameters* cma = (*it).second.test(IO,cabling_code);
	        if(cma) list.push_back(cma);
	        ++it;
	    }            
	}
        else
	{//even
            EvenPhiCMAmap::const_iterator it = evenphiCMAs.begin();
            while(it != evenphiCMAs.end())
	    {
	        const CMAinput IO = (*it).second.whichCMAinput(station);
                const CMAparameters* cma = (*it).second.test(IO,cabling_code);
	        if(cma) list.push_back(cma);
	        ++it;
	    } 
	}
    }
    return list;
}

bool
SectorLogicSetup::give_RoI_borders(CMAidentity ETA,CMAidentity PHI,
                                   unsigned int& firstEtaCode,
                                   unsigned int& lastEtaCode,
                                   unsigned int& firstPhiCode,
                                   unsigned int& lastPhiCode) const
{
    EtaCMAmap::const_iterator etaCMA =
                                   find_etaCMA(ETA.PAD_index(),ETA.Ixx_index());
    if(etaCMA == etaCMAs.end()) return false;
    
    firstEtaCode = (*etaCMA).second.first_pivot_code();
    lastEtaCode  = (*etaCMA).second.last_pivot_code();
    
    if(PHI.coverage() == OddSectors)
    {
        OddPhiCMAmap::const_iterator phiCMA =
	                        find_oddphiCMA(PHI.PAD_index(),PHI.Ixx_index());
	if(phiCMA == oddphiCMAs.end()) return false;
        firstPhiCode = (*phiCMA).second.first_pivot_code();
        lastPhiCode  = (*phiCMA).second.last_pivot_code();	
    }
    else if (PHI.coverage() == EvenSectors)
    {
        EvenPhiCMAmap::const_iterator phiCMA =
	                       find_evenphiCMA(PHI.PAD_index(),PHI.Ixx_index());
	if(phiCMA == evenphiCMAs.end()) return false;
        firstPhiCode = (*phiCMA).second.first_pivot_code();
        lastPhiCode  = (*phiCMA).second.last_pivot_code();	
    }
    else return false;

    if (firstEtaCode==lastEtaCode) return false;
    
    return true;
}


bool
SectorLogicSetup::give_LowPt_borders(CMAidentity ETA,CMAidentity PHI,
                                     unsigned int& firstEtaCode,
                                     unsigned int& lastEtaCode,
                                     unsigned int& firstPhiCode,
                                     unsigned int& lastPhiCode) const
{
    EtaCMAmap::const_iterator etaCMA =
                                   find_etaCMA(ETA.PAD_index(),ETA.Ixx_index());
    if(etaCMA == etaCMAs.end()) return false;

    firstEtaCode = (*etaCMA).second.first_lowPt_code();
    lastEtaCode  = (*etaCMA).second.last_lowPt_code();
    
    if(PHI.coverage() == OddSectors)
    {
        OddPhiCMAmap::const_iterator phiCMA =
	                        find_oddphiCMA(PHI.PAD_index(),PHI.Ixx_index());
	if(phiCMA == oddphiCMAs.end()) return false;
        firstPhiCode = (*phiCMA).second.first_lowPt_code();
        lastPhiCode  = (*phiCMA).second.last_lowPt_code();	
    }
    else if (PHI.coverage() == EvenSectors)
    {
        EvenPhiCMAmap::const_iterator phiCMA =
	                       find_evenphiCMA(PHI.PAD_index(),PHI.Ixx_index());
	if(phiCMA == evenphiCMAs.end()) return false;
        firstPhiCode = (*phiCMA).second.first_lowPt_code();
        lastPhiCode  = (*phiCMA).second.last_lowPt_code();	
    }
    else return false;
    
    return true;
}

bool
SectorLogicSetup::give_HighPt_borders(CMAidentity ETA,CMAidentity PHI,
                                      unsigned int& firstEtaCode,
                                      unsigned int& lastEtaCode,
                                      unsigned int& firstPhiCode,
                                      unsigned int& lastPhiCode) const
{
    EtaCMAmap::const_iterator etaCMA =
                                   find_etaCMA(ETA.PAD_index(),ETA.Ixx_index());
    if(etaCMA == etaCMAs.end()) return false;

    firstEtaCode = (*etaCMA).second.first_highPt_code();
    lastEtaCode  = (*etaCMA).second.last_highPt_code();
    
    if(PHI.coverage() == OddSectors)
    {
        OddPhiCMAmap::const_iterator phiCMA =
	                        find_oddphiCMA(PHI.PAD_index(),PHI.Ixx_index());
	if(phiCMA == oddphiCMAs.end()) return false;
        firstPhiCode = (*phiCMA).second.first_highPt_code();
        lastPhiCode  = (*phiCMA).second.last_highPt_code();	
    }
    else if (PHI.coverage() == EvenSectors)
    {
        EvenPhiCMAmap::const_iterator phiCMA =
	                       find_evenphiCMA(PHI.PAD_index(),PHI.Ixx_index());
	if(phiCMA == evenphiCMAs.end()) return false;
        firstPhiCode = (*phiCMA).second.first_highPt_code();
        lastPhiCode  = (*phiCMA).second.last_highPt_code();	
    }
    else return false;
    
    return true;
}

bool 
SectorLogicSetup::give_LowPt_layout(CMAidentity ID,
                          unsigned short int& start_pivot_ch,
			  unsigned int& start_pivot_code,
			  unsigned short int& stop_pivot_ch,
			  unsigned int& stop_pivot_code,
			  unsigned short int& start_confirm_ch,
			  unsigned int& start_confirm_code,
			  unsigned short int& stop_confirm_ch,
			  unsigned int& stop_confirm_code) const
{
    if(ID.type() == Eta)
    {
        EtaCMAmap::const_iterator etaCMA =
                                   find_etaCMA(ID.PAD_index(),ID.Ixx_index());
        if(etaCMA == etaCMAs.end()) return false;
    
        start_pivot_code = (*etaCMA).second.first_pivot_code();
        stop_pivot_code  = (*etaCMA).second.last_pivot_code();
	start_confirm_code = (*etaCMA).second.first_lowPt_code();
        stop_confirm_code  = (*etaCMA).second.last_lowPt_code();
	
	start_pivot_ch = (*etaCMA).second.first_pivot_channel();
        stop_pivot_ch  = (*etaCMA).second.last_pivot_channel();
	start_confirm_ch = (*etaCMA).second.first_lowPt_channel();
        stop_confirm_ch  = (*etaCMA).second.last_lowPt_channel();
    }
    else 
    {
        if (ID.coverage() == OddSectors)
	{
	    OddPhiCMAmap::const_iterator phiCMA =
	                        find_oddphiCMA(ID.PAD_index(),ID.Ixx_index());
	    if(phiCMA == oddphiCMAs.end()) return false;
	    
            start_pivot_code = (*phiCMA).second.first_pivot_code();
            stop_pivot_code  = (*phiCMA).second.last_pivot_code();
	    start_confirm_code = (*phiCMA).second.first_lowPt_code();
            stop_confirm_code  = (*phiCMA).second.last_lowPt_code();
	    
	    start_pivot_ch = (*phiCMA).second.first_pivot_channel();
            stop_pivot_ch  = (*phiCMA).second.last_pivot_channel();
	    start_confirm_ch = (*phiCMA).second.first_lowPt_channel();
            stop_confirm_ch  = (*phiCMA).second.last_lowPt_channel();
	}
	else
	{
	    EvenPhiCMAmap::const_iterator phiCMA =
	                       find_evenphiCMA(ID.PAD_index(),ID.Ixx_index());
	    if(phiCMA == evenphiCMAs.end()) return false;
            start_pivot_code = (*phiCMA).second.first_pivot_code();
            stop_pivot_code  = (*phiCMA).second.last_pivot_code();
	    start_confirm_code = (*phiCMA).second.first_lowPt_code();
            stop_confirm_code  = (*phiCMA).second.last_lowPt_code();
	    
	    start_pivot_ch = (*phiCMA).second.first_pivot_channel();
            stop_pivot_ch  = (*phiCMA).second.last_pivot_channel();
	    start_confirm_ch = (*phiCMA).second.first_lowPt_channel();
            stop_confirm_ch  = (*phiCMA).second.last_lowPt_channel();
	}
    }
    
    if (start_confirm_ch == 999 ||  stop_confirm_ch==999) return false;
    return true;
}


bool 
SectorLogicSetup::give_HighPt_layout(CMAidentity ID,
                          unsigned short int& start_pivot_ch,
			  unsigned int& start_pivot_code,
			  unsigned short int& stop_pivot_ch,
			  unsigned int& stop_pivot_code,
			  unsigned short int& start_confirm_ch,
			  unsigned int& start_confirm_code,
			  unsigned short int& stop_confirm_ch,
			  unsigned int& stop_confirm_code) const
{
    if(ID.type() == Eta)
    {
        EtaCMAmap::const_iterator etaCMA =
                                   find_etaCMA(ID.PAD_index(),ID.Ixx_index());
        if(etaCMA == etaCMAs.end()) return false;
    
        start_pivot_code = (*etaCMA).second.first_pivot_code();
        stop_pivot_code  = (*etaCMA).second.last_pivot_code();
	start_confirm_code = (*etaCMA).second.first_highPt_code();
        stop_confirm_code  = (*etaCMA).second.last_highPt_code();
	
	start_pivot_ch = (*etaCMA).second.first_pivot_channel();
        stop_pivot_ch  = (*etaCMA).second.last_pivot_channel();
	start_confirm_ch = (*etaCMA).second.first_highPt_channel();
        stop_confirm_ch  = (*etaCMA).second.last_highPt_channel();
    }
    else 
    {
        if (ID.coverage() == OddSectors)
	{
	    OddPhiCMAmap::const_iterator phiCMA =
	                        find_oddphiCMA(ID.PAD_index(),ID.Ixx_index());
	    if(phiCMA == oddphiCMAs.end()) return false;
	    
            start_pivot_code = (*phiCMA).second.first_pivot_code();
            stop_pivot_code  = (*phiCMA).second.last_pivot_code();
	    start_confirm_code = (*phiCMA).second.first_highPt_code();
            stop_confirm_code  = (*phiCMA).second.last_highPt_code();
	    
	    start_pivot_ch = (*phiCMA).second.first_pivot_channel();
            stop_pivot_ch  = (*phiCMA).second.last_pivot_channel();
	    start_confirm_ch = (*phiCMA).second.first_highPt_channel();
            stop_confirm_ch  = (*phiCMA).second.last_highPt_channel();
	}
	else
	{
	    EvenPhiCMAmap::const_iterator phiCMA =
	                       find_evenphiCMA(ID.PAD_index(),ID.Ixx_index());
	    if(phiCMA == evenphiCMAs.end()) return false;
            start_pivot_code = (*phiCMA).second.first_pivot_code();
            stop_pivot_code  = (*phiCMA).second.last_pivot_code();
	    start_confirm_code = (*phiCMA).second.first_highPt_code();
            stop_confirm_code  = (*phiCMA).second.last_highPt_code();
	    
	    start_pivot_ch = (*phiCMA).second.first_pivot_channel();
            stop_pivot_ch  = (*phiCMA).second.last_pivot_channel();
	    start_confirm_ch = (*phiCMA).second.first_highPt_channel();
            stop_confirm_ch  = (*phiCMA).second.last_highPt_channel();
	}
    }
    
    if (start_confirm_ch == 999 ||  stop_confirm_ch==999) return false;
    return true;
}

const CMAparameters* 
SectorLogicSetup::give_CMA(CMAidentity CMA) const
{
    if(CMA.type() == Eta)
    {
        EtaCMAmap::const_iterator etaCMA =
                                  find_etaCMA(CMA.PAD_index(),CMA.Ixx_index());
        return (etaCMA!=etaCMAs.end())? &((*etaCMA).second) : 0;
    }
    if(CMA.type() == Phi)
    {
        if(CMA.coverage() == EvenSectors)
	{
            EvenPhiCMAmap::const_iterator phiCMA =
	                      find_evenphiCMA(CMA.PAD_index(),CMA.Ixx_index());
            return (phiCMA!=evenphiCMAs.end())? &((*phiCMA).second) : 0;
	}
        if(CMA.coverage() == OddSectors)
	{        
            OddPhiCMAmap::const_iterator phiCMA =
	                       find_oddphiCMA(CMA.PAD_index(),CMA.Ixx_index());
            return (phiCMA!=oddphiCMAs.end())? &((*phiCMA)).second : 0;
	}
    }

    return 0;
}


bool
SectorLogicSetup::correct(CMAidentity CMA,L1RPCcabCorrection type,
			  CMAinput it,unsigned int ly,
			  unsigned short int Channel1,
			  unsigned short int Channel2,
			  short int number)
{
    if(CMA.type() == Eta)
    {
        EtaCMAmap::iterator etaCMA =
                                  find_etaCMA(CMA.PAD_index(),CMA.Ixx_index());
        return (*etaCMA).second.correct(type,it,ly,Channel1,Channel2,number);
    }
    if(CMA.type() == Phi)
    {
        if(CMA.coverage() == EvenSectors)
	{
            EvenPhiCMAmap::iterator phiCMA =
	                      find_evenphiCMA(CMA.PAD_index(),CMA.Ixx_index());
	    return (*phiCMA).second.correct(type,it,ly,Channel1,Channel2,
	                                    number);
	}
        if(CMA.coverage() == OddSectors)
	{        
            OddPhiCMAmap::iterator phiCMA =
	                       find_oddphiCMA(CMA.PAD_index(),CMA.Ixx_index());
	    return (*phiCMA).second.correct(type,it,ly,Channel1,Channel2,
                                            number);
	}
    }
    return false;
}


std::list<unsigned int> 
SectorLogicSetup::give_strip_code(CMAidentity CMA,int logic_sector,
				  unsigned short int lh,
                                  unsigned short int ijk,
			          unsigned short int Channel) const
{
    std::list<unsigned int> StripCodes;

    if(CMA.type() == Eta)
    {
        EtaCMAmap::const_iterator etaCMA =
                                  find_etaCMA(CMA.PAD_index(),CMA.Ixx_index());
        
	if(etaCMA==etaCMAs.end()) return StripCodes;
	(*etaCMA).second.give_strip_code(logic_sector,lh,ijk,Channel,
                                         StripCodes);
    }
    if(CMA.type() == Phi)
    {
        if(CMA.coverage() == EvenSectors)
	{
            EvenPhiCMAmap::const_iterator phiCMA =
	                      find_evenphiCMA(CMA.PAD_index(),CMA.Ixx_index());
	    if(phiCMA==evenphiCMAs.end()) return StripCodes;
	    (*phiCMA).second.give_strip_code(logic_sector,lh,ijk,Channel,
                                             StripCodes);
	}
        if(CMA.coverage() == OddSectors)
	{        
            OddPhiCMAmap::const_iterator phiCMA =
	                       find_oddphiCMA(CMA.PAD_index(),CMA.Ixx_index());
	    if(phiCMA==oddphiCMAs.end()) return StripCodes;
	    (*phiCMA).second.give_strip_code(logic_sector,lh,ijk,Channel,
                                             StripCodes);
	}
    }

    return StripCodes;
}

SectorLogicSetup&
SectorLogicSetup::operator<<(int sector)
{    
    m_sectors.push_back(sector);

    __osstream str; 
    if (sector < 32 )
    {
        str << std::setw(3) << sector << std::ends;
	m_negative_sector += str.str();
    }
    else
    {
        str << std::setw(3) << sector - 32 << std::ends;
	m_positive_sector += str.str();
    }

    return *this;
}

void
SectorLogicSetup::PrintElement (std::ostream& stream,int station,std::string ele,
                                int obj,bool detail) const
{
    int klw = station * 100;
    int khg = station * 100 + 99;

    if(station == 0) khg = 10*100 + 99;

    bool all     = (ele == name() || ele == "")? true : false;
    bool printed = false;

    bool nRPC = RPCs.size();
    bool nWOR = WORs.size();
    bool nEtaCMA     = etaCMAs.size();
    bool nEvenPhiCMA = evenphiCMAs.size();
    bool nOddPhiCMA  = oddphiCMAs.size();

    stream << name() << " type n. " << sector_type() << std::endl;
    stream << "It applies to negative logic sector n. ";
    stream << negative_sector() << std::endl;
    stream << "                       logic sector n. ";
    stream << positive_sector() << std::endl;


    if(nRPC && (ele == (*RPCs.begin()).second.name() || all ))
    {
        printed = true;
        RPCmap::const_iterator low  = RPCs.lower_bound(klw);
        RPCmap::const_iterator high = RPCs.upper_bound(khg);
        stream << "It contains " << distance(low,high);
        stream << " RPCs chamber on station " << station << std::endl;

        for(RPCmap::const_iterator it = low; it != high; ++it)
	{
	    if(!obj) (*it).second.Print(stream,detail);
	    else if(obj==(*it).second.number())
	        (*it).second.Print(stream,detail);
	}
    }

    if(nWOR && (ele == (*WORs.begin()).second.name() || all ))
    {
        printed = true;
        WORmap::const_iterator low  = WORs.lower_bound(klw);
        WORmap::const_iterator high = WORs.upper_bound(khg);
        stream << "It contains " << distance(low,high);
        stream << " WORs links on station " << station << std::endl;

        for(WORmap::const_iterator it = low; it != high; ++it)
	{	    
	    if(!obj) (*it).second.Print(stream,detail);
	    else if (obj==(*it).second.number())
	        (*it).second.Print(stream,detail);
	}
    }

    if(nEtaCMA&&(ele==(*etaCMAs.begin()).second.name() || all ))
    {
        printed = true;
        stream << "It contains " << etaCMAs.size() << " eta CMA:" << std::endl;
        EtaCMAmap::const_iterator ei;	
        for(ei=etaCMAs.begin(); ei != etaCMAs.end(); ++ei)
	{
            if(!obj) (*ei).second.Print(stream,detail);
	    else if (obj==(*ei).second.number())
	        (*ei).second.Print(stream,detail);
	}
    }

    if(nEvenPhiCMA && (ele==(*evenphiCMAs.begin()).second.name() || all))
    {
        printed = true;
        stream << "It contains ";
        stream << evenphiCMAs.size() << " even phi CMA:" << std::endl;
        EvenPhiCMAmap::const_iterator ev;

        for(ev=evenphiCMAs.begin(); ev != evenphiCMAs.end();++ev)
	{
            if(!obj) (*ev).second.Print(stream,detail);
	    else if (obj==(*ev).second.number())
	        (*ev).second.Print(stream,detail);
	}
     }

     if(nOddPhiCMA && (ele==(*oddphiCMAs.begin()).second.name() || all))
     {
        printed = true;
        stream << "It contains ";
        stream << oddphiCMAs.size() << " odd phi CMA:" << std::endl;
        OddPhiCMAmap::const_iterator od;

        for(od=oddphiCMAs.begin(); od != oddphiCMAs.end();++od)
	{
            if(!obj) (*od).second.Print(stream,detail);
	    else if (obj==(*od).second.number())
	        (*od).second.Print(stream,detail);
	}
    }

    if(ele == "CMA" )
    {   
        printed = true;

        stream << "It contains " << etaCMAs.size() << " eta CMA:" << std::endl;
        EtaCMAmap::const_iterator ei;
	stream << "nome=" << (*etaCMAs.begin()).second.name() << std::endl;
        for(ei = etaCMAs.begin(); ei != etaCMAs.end(); ++ei)
	{
            if(!obj) (*ei).second.Print(stream,detail);
	    else if (obj==(*ei).second.number())
	        (*ei).second.Print(stream,detail);
	}

        stream << "plus ";
        stream << evenphiCMAs.size() << " even phi CMA:" << std::endl;
        EvenPhiCMAmap::const_iterator ev;
        for(ev = evenphiCMAs.begin(); ev != evenphiCMAs.end(); ++ev)
	{
            if(!obj) (*ev).second.Print(stream,detail);
	    else if (obj==(*ev).second.number())
	        (*ev).second.Print(stream,detail);
	}

        stream << "plus ";
        stream << oddphiCMAs.size() << " odd phi CMA:" << std::endl;
        OddPhiCMAmap::const_iterator od;
        for(od = oddphiCMAs.begin(); od != oddphiCMAs.end(); ++od)
	{
            if(!obj) (*od).second.Print(stream,detail);
	    else if (obj==(*od).second.number())
	        (*od).second.Print(stream,detail);
	}

    } 
    
    if(!printed)
    {
        stream << std::endl;
	stream << "No " << ele << " elements are present into this map!"
               << std::endl;
    }
}


std::ostream& operator<<(std::ostream& stream,const SectorLogicSetup& setup)
{
    setup.PrintElement(stream,0,setup.name(),0,false);
    return stream;
}


bool
SectorLogicSetup::operator+=(RPCchamberdata& data)
{
    while(RPCchamber* cham = data.give_rpc())
    {
        int key = data.station()*100;
        std::pair < RPCmap::iterator, bool> ins = 
            RPCs.insert(RPCmap::value_type(key + cham->number(),*cham));

	RPCmap::iterator lower = RPCs.lower_bound(key);
	RPCmap::iterator upper = RPCs.upper_bound(key+99);
        RPCmap::iterator current = (ins.first);

        if (ins.second)
        {
            if (current != lower ) --current; 
            while (current != upper )
	    {
	        int div       = ((*current).second.number())? 1 : 2;
	        int eta_st    = (*current).second.eta_strips()/div;
                int eta_st_of = (*current).second.eta_strip_global()/div;
                int eta_co    = (*current).second.eta_connectors()/div;
                int eta_co_of = (*current).second.eta_conn_global()/div;
	        
		current++;
		if (current != RPCs.end())
                {
		    (*current).second.set_eta_st_global(eta_st + eta_st_of);
                    (*current).second.set_eta_co_global(eta_co + eta_co_of);
	        }
	    }
        } 
        else
        {
	    DISP <<"Error in inserting chamber:" << std::endl
                 << *cham << std::endl << "in "; 
            PrintElement(m_message->message(),data.station(),"RPC",0,false);
            DISP_ERROR;
            delete cham;
            return ins.second;
        }
        delete cham;
    }
    return true;
}


bool
SectorLogicSetup::operator+=(WiredORdata& data)
{   
    while(WiredOR* Wor = data.give_wor())
    {
        int key = data.station() * 100;
        std::pair < WORmap::iterator, bool> ins = 
        WORs.insert(WORmap::value_type(key + Wor->number(),*Wor));

        if(!ins.second)
        {
	    DISP << "Error in inserting WOR:" << std::endl
                 << *Wor << std::endl << "in ";
            PrintElement(m_message->message(),data.station(),"WOR",0,false);
            DISP_ERROR;
            delete Wor;
            return ins.second;
        }
        delete Wor;
    }
    return true;
}

bool
SectorLogicSetup::operator+=(CMApivotdata& data)
{
    while(EtaCMA* CMA = data.give_eta_cma())
    {
        std::pair < EtaCMAmap::iterator, bool> ins = 
        etaCMAs.insert(EtaCMAmap::value_type(CMA->id(),*CMA));
        if(!ins.second)
        {
	    std::cout << CMA->id() << std::endl;
	    EtaCMAmap::iterator inserted = etaCMAs.find(CMA->id());
            
            int cma_start = CMA->pivot_start_ch();
            int ins_start = (*inserted).second.pivot_start_ch();
            if (cma_start*ins_start > 0) 
	    {
	        DISP << "Error in inserting Eta CMA:" << std::endl
                     << *CMA << std::endl << "in "; 
                PrintElement(m_message->message(),0,CMA->name(),0,false);
                DISP_ERROR;
                delete CMA;
                return ins.second;
	    }
            (*inserted).second += *CMA;
        }

        get_station(LowPt,CMA->lowPt_station());
        get_station(Pivot,CMA->pivot_station());
        get_station(HighPt,CMA->highPt_station());

        delete CMA;
    }

    while(EvenPhiCMA* CMA = data.give_evenphi_cma())
    {
        std::pair < EvenPhiCMAmap::iterator, bool> ins = 
        evenphiCMAs.insert(EvenPhiCMAmap::value_type(CMA->id(),*CMA));
        if(!ins.second)
        {
	        DISP << "Error in inserting even Phi CMA:" << std::endl
                     << *CMA << std::endl << "in ";
                PrintElement(m_message->message(),0,CMA->name(),0,false);
                DISP_ERROR;
                delete CMA;
                return ins.second;
        }
        delete CMA;
    }

    while(OddPhiCMA* CMA = data.give_oddphi_cma())
    {
        std::pair < OddPhiCMAmap::iterator, bool> ins = 
        oddphiCMAs.insert(OddPhiCMAmap::value_type(CMA->id(),*CMA));
        if(!ins.second)
        {
	        DISP << "Error in inserting odd Phi CMA:" << std::endl
                     << *CMA << std::endl << "in ";
                PrintElement(m_message->message(),0,CMA->name(),0,false);
                DISP_ERROR;
                delete CMA;
                return ins.second;
        }
        delete CMA;
    }

    return true;
}


bool
SectorLogicSetup::operator+=(CMAcablingdata& data)
{
    while(EtaCMA* CMA = data.give_eta_cma())
    {
        std::pair < EtaCMAmap::iterator, bool> ins = 
        etaCMAs.insert(EtaCMAmap::value_type(CMA->id(),*CMA));
        if(!ins.second)
        {
	    EtaCMAmap::iterator inserted = etaCMAs.find(CMA->id());
            
            int cma_start = CMA->pivot_start_ch();
            int ins_start = (*inserted).second.pivot_start_ch();
            if (cma_start*ins_start > 0) 
	    {
	        DISP << "Error in inserting Eta CMA:" << std::endl
                     << *CMA << std::endl << "in "; 
                PrintElement(m_message->message(),0,"CMA",0,false);
                DISP_ERROR;
                delete CMA;
                return ins.second;
	    }
            (*inserted).second += *CMA;
        }
        delete CMA;
    }
    return true;
}

HalfType
SectorLogicSetup::side() const
{
    if (m_negative_sector != "" && m_positive_sector == "") return Negative;
    if (m_negative_sector == "" && m_positive_sector != "") return Positive;
    return NoHalf;
}


void
SectorLogicSetup::SetPtoTrigRoads(const std::map<std::string, std::string>* RPC_trigroads) 
{
  p_trigroads=RPC_trigroads;
}
