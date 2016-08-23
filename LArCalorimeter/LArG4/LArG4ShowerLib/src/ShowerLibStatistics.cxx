/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * FCALDistEnergyShowerLibStatistics.cpp
 *
 *  Created on: Feb 24, 2012
 *      Author: sedov
 */

#include "LArG4ShowerLib/ShowerLibStatistics.h"
#include <sstream>
#include "G4Track.hh"

namespace ShowerLib {

ShowerLibStatistics::ShowerLibStatistics(const std::map<int,std::string>& bincaptions,
                                         const std::map<int,int>& binsizes) : m_counter(0),
                                                                                                                          m_binstrings(bincaptions),
                                                                                                                          m_binsizes(binsizes)
{
	for(std::map<int,int>::const_iterator it = m_binsizes.begin(); it != m_binsizes.end(); it++)
		  m_calls[(*it).first] = 0;
}

void ShowerLibStatistics::recordShowerLibUse(int binkey)
{
	  m_counter++;
	  m_calls[binkey] += 1;
}

const std::string ShowerLibStatistics::statistics()
{
	std::stringstream out;
	out << std::fixed << std::setprecision(2);
    out << "Number of bins: " << m_binsizes.size() << std::endl;
    out << "Bin Statistics: " << std::endl;
    float maxuse = 0;
    float minuse = -1;
    int totnum = 0;
    for(std::map<int,int>::const_iterator it = m_binsizes.begin(); it != m_binsizes.end();++it) {
  	  totnum += (*it).second;
  	  int curcalls = m_calls.at((*it).first);
  	  int curlen = (*it).second;
	  std::string name = m_binstrings.at((*it).first);
  	  int use = 0;
  	  if (curlen >0) {
  		  use = 100 * curcalls/curlen;
  	  }
  	  if (maxuse < use) maxuse = use;
  	  if (minuse == -1) minuse = use;
  	  else if (minuse > use) minuse = use;
  	  
  	  out << name << " size " << std::setw(7) << curlen << " / calls " << std::setw(9) << curcalls << " ("<< std::setw(6) << use << "%)" << std::endl;
    }

    if (totnum == 0) { //i don't believe there ever will be a situation when we'll ask for a statistics for an empty lib, but for coverity to be quiet let it be here
    	return out.str();
    }

    out << "Total:               size " << std::setw(6) << totnum << " / calls " << std::setw(9) << m_counter << " (min " << minuse << "% av " << (100 * m_counter/totnum) << "% max " << maxuse << "%)" << std::endl;

    return out.str();
}

} /* namespace ShowerLib */
