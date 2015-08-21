/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKPattern.h"
#include "common_fcn.h"

#include <boost/unordered_map.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <limits>
#include <vector>

#include "TrigFTKSim/tsp/TSPROOTBankGenerator.h"

using namespace std;

// Plane map pointer, to be set during the initialization
static FTKPlaneMap *pmap(0);
static FTKRegionMap *rmap(0);

// Super-strip map, this object transform an hit in a SS index
static FTKSSMap *ssmap_TSP(0);
static FTKSSMap *ssmap_AM(0);

// Path of Bank
static string bank_path;
static string out_bank_path;

// number of sub regions
static int _iSub;
static int _nSub;

// Number of patterns to be read by this bank
static int bank_maxpatts;
// minimum coverage of the patterns to include in the TSP bank
unsigned mincoverage;
// mode of HW format
static int HWModeID;
// if !=0 ask to remove the original TSP bank
int RemoveTSP(0);

int read_commands();

int main(int argc, char *argv[]) {
	cout << "AM bank generation from TSP bank" << endl;
	FTKSetup &ftkset = FTKSetup::getFTKSetup();

	common_fcn_init(argc, argv);

	// TODO: Implementing a real parser would be nice
	ifstream optionfile;
	streambuf *oldrdbuf = cin.rdbuf();
	if (argc > 1) {
		optionfile.open(argv[1]);
		if (!optionfile) {
			cerr << "*** Error reading input file: " << argv[1] << endl;
			return -1;
		}
		cin.rdbuf(optionfile.rdbuf());
	}

	// call the parser
	if (read_commands() < 0) {
		cerr << "*** Error parsing the options" << endl;
		return -1;
	}
	cin.rdbuf(oldrdbuf);

	/*BankRefiner refiner(ftkset, *ssmap_TSP, *ssmap_AM, bank_path, out_bank_path,bank_maxpatts);
	refiner.convert();*/

	std::vector<FTKSSMap*> ssmaps;
	ssmaps.push_back(ssmap_TSP);
	ssmaps.push_back(ssmap_AM);

	FTKSetup::getFTKSetup().setHWModeSS(HWModeID);

	TSPROOT::TSPROOTBankGenerator generator(&ftkset, ssmaps, bank_path, out_bank_path, rmap->getNPlanes(), bank_maxpatts, mincoverage);
	generator.SetSubregion(_iSub,_nSub);
	generator.SetRemoveTSP(RemoveTSP);
	generator.generate();	  

	return 0;
}

int read_commands() {
	const char prompt[] = "PARSE> ";
	string line;
	while (replace_getline(cin, line) != 0) {
		if (line[0] == '#' || line.size() == 0) continue;

		cout << prompt << line << endl;

		istringstream sline(line);
		string key;
		sline >> key;
		if (key == "SS_FILE_AM") {
			string sval;
			sline >> sval;
			cout << prompt << key << " = " << sval << endl;
			ssmap_AM = new FTKSSMap(rmap, sval.c_str(), false);
		} else if (key == "SS_FILE_TSP") {
			string sval;
			sline >> sval;
			cout << prompt << key << " = " << sval << endl;
			
			// the map on unused layer skip the boundary check by default
			ssmap_TSP = new FTKSSMap(rmap, sval.c_str(), false);
                } else if (key == "CUR_REGION") {
		        int ival;
		        sline >> ival;
		        cout << "TODO. CUR_REGION="<<ival << endl;
                } else if (key == "NSUBREGIONS") {
		        sline >> _nSub;
		        cout << "NSUBREGIONS "<<_nSub << endl;
		} else if (key == "CUR_SUBREGION") {
		        sline >> _iSub;
		        cout << "CUR_SUBREGION="<<_iSub << endl;
                } else if (key == "ENCODE_SUBREGION") {
			int ival;
			sline >> ival;
			cout << "TODO" << endl;
		} else if (key == "BANK") {
			sline >> bank_maxpatts;
			replace_getline(cin, bank_path);
		} else if (key == "OUTBANK"){
			replace_getline(cin, out_bank_path);
		} else if (key == "MINCOVERAGE"){
		  sline >> mincoverage;
		} else if (key == "HWMODEID"){
		  sline >> HWModeID;
		} else if (key == "PMAP_FILE") {
			string sval;
			sline >> sval;
			pmap = new FTKPlaneMap(sval.c_str());
			if (!(*pmap)) {
				cerr << "*** error loading plane map in: " << sval << endl;
				return -1;
			}
		} else if (key == "RMAP_FILE") {
			string sval;
			sline >> sval;
			rmap = new FTKRegionMap(pmap, sval.c_str());
			if (!(*rmap)) {
				cerr << "*** error using map from: " << sval << endl;
				return -1;
			}
		} else if ( key == "REMOVETSP" ) {
		  sline >> RemoveTSP;
		  cout << "RemoveTSP " << RemoveTSP << endl;
		} else {
			cout << prompt << "\"" << key << "\" is not a valid key" << endl;
			continue;
		}
	}

	return 0;
}
