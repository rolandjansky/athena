/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BadChanService.h"
#include "LArBadChannelParser.h"
#include "LArBadChanBitPacking.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

int main() {
	
	typedef BadChanService::BadChanInfo        BadChanInfo;
	typedef BadChanInfo::BadChanVec            BadChanVec;
	
	string name="";
	cout << "Input filename: " <<endl;
	cin >> name;

	LArBadChannelParser myParser(name);
	if(!myParser.fileStatusGood()) cout << "File no good!" << endl;
	vector<LArBadChannelParser::BadChannelEntry> res = myParser.parseAll();
	if( res.size()==0) cout << "Empty result!" << endl;
	
	BadChanVec bcv;
	LArBadChanBitPacking packing;
	for (vector<LArBadChannelParser::BadChannelEntry>::const_iterator i = res.begin();
		 i != res.end(); ++i) {
		cout << i->channel << " "
		<< i->status.packedData() 
		<< " " << packing.stringStatus( i->status)
		<< endl;
		
		bcv.push_back( BadChanVec::value_type( i->channel, i->status));
	}
	
	BadChanInfo& bci = BadChanService::info();
	bci.set(bcv);

	cout << endl << "BCI has size " << bci.size() << endl;
	for (BadChanInfo::const_iterator i=bci.begin(); i!=bci.end(); ++i) {
		cout << i->first << " " << packing.stringStatus( i->second) << endl;
	}
	
	for (vector<LArBadChannelParser::BadChannelEntry>::const_iterator i = res.begin();
		 i != res.end(); ++i) {
		LArBadChannel bc = bci.status(i->channel);
		cout << i->channel << " "
		<< bc.packedData() 
		<< " " << packing.stringStatus( bc)
		<< endl;
	}

	
}
