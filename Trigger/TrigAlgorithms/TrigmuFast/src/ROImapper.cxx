/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>

#include <math.h>

#include "TrigmuFast/ROImapper.h"
#include "MuonCablingTools/dbline.h"



using namespace MUON;

ROImapper::ROImapper(std::string filename) : m_roiMap(0)
{
    std::ifstream mapfile(filename.c_str());
    
    if(!mapfile)
    {            
	return;
    }
    
    
    DBline data(mapfile);
    while (data)
    {
        ROImap map;
	
        if ( data("SYS")    >> map.SYS &&
	     data("SUBSYS") >> map.SUBSYS &&
	     data("SEC")    >> map.SEC &&
	     data("sys")    >> map.sys &&
	     data("subsys") >> map.subsys &&
	     data("sec")    >> map.sec &&
	     data("phi")    >> map.phi )
        {
	    data("TH")     >> map.th;
	    m_roiMap.push_back(map);
	}
	++data;
    }
    
    mapfile.close();
}

ROImapper::~ROImapper()
{
    m_roiMap.clear();
}

void
ROImapper::remap(unsigned short int& system, unsigned short int& subsystem,
                 unsigned short int& sector, float& eta, float& phi,
		 int th) const
{
    std::list<ROImap>::const_iterator it;
    for(it=m_roiMap.begin();it!=m_roiMap.end();++it)
    {
        ROImap map = *it;
        if(map.SYS==system && map.SUBSYS==subsystem && map.SEC==sector &&
	   map.th != th )
	{
	    system    = map.sys;
	    subsystem = map.subsys;
	    sector    = map.sec;
	    phi       = map.phi;
	    eta       = (map.subsys==0)? -fabs(eta) : fabs(eta);
	    return;
	}
    }
}

std::ostream& MUON::operator<< (std::ostream& stream,const MUON::ROImapper& map)
{
    stream << "Dumping the content of the RoI remapper" << std::endl;
    const std::list<ROImap> remap = map.roiMap();
    std::list<ROImap>::const_iterator it;
    for(it=remap.begin();it!=remap.end();++it)
    {
        stream << "SYS "      << (*it).SYS
	       << ", SUBYSY " << (*it).SUBSYS
	       << ", SEC "    << (*it).SEC
	       << ", TH "     << (*it).th
	       << " ==> SYS " << (*it).sys
	       << ", SUBSYS " << (*it).subsys
	       << ", SEC "    << (*it).sec << std::endl;
    }
    
    return stream;
}
