/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StationSelector_H
#define StationSelector_H

#include <string>
#include <vector>
#include <map>

#include "MuonGeoModel/Station.h"

namespace MuonGM {

class StationSelector {
public:
	typedef std::map<std::string,Station* ,std::less<std::string> >::const_iterator StationIterator;
	StationSelector(std::string filename);
	StationSelector(std::vector<std::string> s);
	StationIterator begin();
	StationIterator end();
	static void SetSelectionType(int t);
private: 
	const StationIterator iterator;
	std::vector<std::string> selector;
	std::map<std::string,Station* ,std::less<std::string> > theMap;
	bool select(StationIterator it);
	static int m_selectType;
};

} // namespace MuonGM

#endif
