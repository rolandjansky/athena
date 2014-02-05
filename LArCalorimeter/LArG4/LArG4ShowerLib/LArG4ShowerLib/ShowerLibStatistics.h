/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * FCALDistEnergyShowerLibStatistics.h
 *
 *  Created on: Feb 24, 2012
 *      Author: sedov
 */

#ifndef SHOWERLIBSTATISTICS_H_
#define SHOWERLIBSTATISTICS_H_

#include <map>
#include <string>

namespace ShowerLib {

class ShowerLibStatistics {
public:
	ShowerLibStatistics(const std::map<int,std::string> bincaptions, const std::map<int,int> binsizes);
	void recordShowerLibUse(int binkey);
	const std::string statistics();
private:

	int counter;
	std::map<int,int> calls;
	std::map<int,std::string> binstrings;
	std::map<int,int> binsizes;

};


} /* namespace ShowerLib */
#endif /* SHOWERLIBSTATISTICS_H_ */
