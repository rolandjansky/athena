/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Guido Volpi - guido.volpi@cern.ch
 */

#ifndef TSPROOTBANKGENERATOR_H
#define TSPROOTBANKGENERATOR_H

#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKPatternBySector.h"

#include <TFile.h>
#include <TTree.h>

#include <boost/unordered_map.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

namespace TSPROOT{

class TSPPatternReadException : public std::runtime_error{
public:
	TSPPatternReadException(const std::string& description) : runtime_error(description){}
};

/**
 * Non working, non optimized pre alpha version
 */
class TSPROOTBankGenerator{
public:

  TSPROOTBankGenerator(const char *);
  
  TSPROOTBankGenerator(FTKSetup* setup, const std::vector<FTKSSMap*> & ssMaps, const std::string& inputBank, const std::string& outBank, unsigned planes = 7, int maxPatterns = -1,int mincoverage=1);
  ~TSPROOTBankGenerator();

	void generate();
	void showstats(int ipatt, int npatterns) const;
        void writeSSMapsToFile();  //!< write maps to root file
	void SetRemoveTSP(int remtsp) { m_RemoveTSP = remtsp; }
	int GetRemoveTSP() const { return m_RemoveTSP; }
        int GetCurrentSubregion() const { return m_iSub; }; //!< Get current subregion
        int GetNSubregion() const { return m_nSub; }; //!< Get current subregion
        void SetSubregion(int iSub, int nSub) {m_iSub = iSub; m_nSub=nSub; }; //!< Get current subregion
private:

	void readNextPattern(FTKPattern& pattern, int patternID);
	void generateChildren(int bankID, int planes);
	int addAMPattern(int bankID, int childID, const std::string& patternKey, TTree *ambank);

	typedef boost::unordered_map<std::string, long> PatternMap;

	int m_RemoveTSP; // ask to remove the orioginal TSP patterns

	FTKSetup* m_setup;
	std::vector<FTKSSMap*> m_ssMaps;
	ftk_dcap::istream m_inputBank;
	int m_maxPatterns;
	int m_planes;
	int m_mincoverage; // minimum coverage for the TSP patterns

	TFile *m_bankfile;	

	long m_patternID;
        long m_npatterns;
        int m_iSub; //!< current subregion
        int m_nSub; //!< number of subregions
        int m_nplanes;
	std::stringstream m_patternStringID;
	PatternMap m_patternMap;
	std::vector<int> m_ssPattern;
        FTKPatternBySectorForestReader* m_preader;
};

}
#endif
