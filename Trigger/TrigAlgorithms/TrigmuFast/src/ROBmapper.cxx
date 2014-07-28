/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>


#include "TrigmuFast/ROBmapper.h"
#include "MuonCablingTools/dbline.h"

using namespace MUON;

ROBmapper::ROBmapper(std::string filename) : m_robMap(0), is_atlas(false)
{
    std::ifstream mapfile(filename.c_str());
    
    if(!mapfile)
    {            
        std::cout << "Cannot open file " << filename.c_str()
	          << " in the working directory" << std::endl;
	return;
    }
    
    
    DBline data(mapfile);
    
    data + 3;
    
    std::string type;
    int maxSubsytem;
    int maxLogicSector;
    int maxRoI;
    
    if( !(data("type:") >> type) )                     return; ++data;
    if( !(data("maxSubsystem:") >> maxSubsytem) )      return; ++data;
    if( !(data("maxLogicSector:") >> maxLogicSector) ) return; ++data;
    if( !(data("maxRoI:") >> maxRoI) )                 return; ++data;
    
    is_atlas = (type=="ATLAS")? true : false;
    
    //std::cout << "maxSubsytem="    << maxSubsytem    << "  "
    //          << "maxLogicSector=" << maxLogicSector << "  "
    //	      << "maxRoI="         << maxRoI         << "  " << std::endl;
    if(maxSubsytem != 2 || maxLogicSector != 32) return;
    
    m_robMap = new uint32_t [maxRoI][3][32][2];
    for(int i=0;i<maxRoI;++i)
        for(int j=0;j<3;++j)
	    for(int k=0;k<maxLogicSector;++k)
	        for(int l=0;l<maxSubsytem;++l) m_robMap[i][j][k][l] = 0;
    
    while(data+4)
    {
        unsigned int subsystem;
	unsigned int sector;
	unsigned int roi;
	unsigned int robnum;
        while(data >> subsystem >> "|" >> sector >> "|" >> roi >> "|"
	           >> robnum >> "|")
        {
	    //std::cout << "Subsytem="    << subsystem    << "  "
            //          << "sector="      << sector       << "  "
	    //          << "roi="         << roi          << " :"
            //          << " robnum="     << robnum       << " ,";
	    unsigned int robid;
	    do
	    {
	        data >> hex() >> robid >> dec();
		//std::cout << " " << std::hex << robid << std::dec;
		if(is_atlas) 
		    m_robMap[roi][--robnum][sector][subsystem] = robid;
		else
		    m_robMap[roi-1][--robnum][sector][subsystem] = robid;
	    }while (robnum);
	    //std::cout << std::endl;
	    ++data;
	}
    }
    
}

ROBmapper::~ROBmapper()
{
    if(m_robMap) delete[] m_robMap;
}


void
ROBmapper::lookup(unsigned int subsystem, unsigned int sector,unsigned int roi,
                  std::vector<uint32_t>& ids) const
{
    if(!m_robMap) return;
    
    uint32_t rob1 = 0x0;
    uint32_t rob2 = 0x0;
    uint32_t rob3 = 0x0;
     
    if(is_atlas)
    { 
        rob1 = m_robMap[roi][0][sector][subsystem];
        rob2 = m_robMap[roi][1][sector][subsystem];
	rob3 = m_robMap[roi][2][sector][subsystem];
    } else
    {
        rob1 = m_robMap[roi-1][0][sector][subsystem];
        rob2 = m_robMap[roi-1][1][sector][subsystem];
	rob3 = m_robMap[roi-1][2][sector][subsystem];
    }
    
    if (rob1 != 0) ids.push_back(rob1);
    if (rob2 != 0) ids.push_back(rob2);
    if (rob3 != 0) ids.push_back(rob3);
    
    if (!m_robMap) return;
}
