/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"
#include "TrigFTKSim/FTKPattern.h"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <list>
#include <map>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {
  /* The arguments are now assumed to be the pattern file (ROOT format)
     and the number of TSP pattern to read.
     In the number of options grows consider moving to getopt (google it)
     or an input file (if really many) */
  FTKSetup &ftksetup = FTKSetup::getFTKSetup();

  cout << "Reading TSP ROOT bank: " << argv[1] << endl;
  TFile *m_file = TFile::Open(argv[1]);
  if (!m_file) {
    FTKSetup::PrintMessage(ftk::sevr,"Error opening the ROOT bank");
    return -1;
  }

  int maxpatt = atoi(argv[2]);

  /* this is the pointer to the TTree containing the AM bank, 
     if a real TSP/DC simulation is performed this points to Bank1,
     if the TSP bank is used as test could be Bank0 */
  TTree *AMBank(0x0);

  // this maps will contain the map of used AM pattern and the relative TSP patterns
  map< int, AMSelection > AMPatternMap;
  bool hasBuiltAM(false); // it becomes true if an AM is built: best tsp or minimum coverage
  ftksetup.usageStat("Create AM Bank using the ROOT file");

  /** AM bank is created building grouping the fatherID field of a given 
      number of TSP patterns, the number of TSP patterns is decided by the
      user */
  FTKSetup::PrintMessage(ftk::info,"Bank from Best TSP patterns");
  // Get the pointer to the TSP bank TTree and the TTree with the relations
  AMBank = (TTree*) m_file->Get("Bank1");
  TTree *TSPBank = (TTree*) m_file->Get("Bank0");
  TTree *TSPRelation = (TTree*) m_file->Get("TSPRelation0");

  // get the maxium number of TSP patterns
  Long64_t m_npatternsTSP = TSPBank->GetEntries();

  // prepare to read the patterns
  FTKPattern *tsppatt = new FTKPattern();
  TSPBank->SetBranchAddress("Pattern",&tsppatt);
  // prepare to read the AM parent IDs
  Int_t AMID;
  TSPRelation->SetBranchAddress("AMID",&AMID);

  // get the first element to complete the information on the bank
  TSPBank->GetEntry(0);
  int m_nplanes = tsppatt->getNPlanes();


  // disable alla the useless branches
  TSPRelation->SetBranchAddress("HalfPlaneMask",0);

  // set number of TSP patterns to read following the maxpatt value
  if (m_npatternsTSP<maxpatt || maxpatt<0) {
    cout << "Maximum number of TSP patterns to be read: " << m_npatternsTSP << " (asked " << maxpatt << ')' << endl;
    maxpatt = m_npatternsTSP;
  }
  cout << "Found # " << m_npatternsTSP << " TSP patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

  // m_patternsTSP is set to the maximum number or patterns
  if (maxpatt<m_npatternsTSP) m_npatternsTSP = maxpatt;

  // set m_npatterns to an impossible value the AM generation loop to stop
  Long64_t m_npatterns = -1;

  int nAMPatterns(0); // number of AM patterns loaded
  for (int ipatt=0;ipatt!=m_npatternsTSP;++ipatt) { // loop over TSP patterns
    // get the ID of this TSP pattern
    TSPRelation->GetEntry(ipatt);
    TSPBank->GetEntry(ipatt);

    map<int,AMSelection>::iterator iAM = AMPatternMap.find(AMID);
    if (iAM==AMPatternMap.end()) { 

	  // new AM pattern
	  AMPatternMap[AMID] = AMSelection(AMID,ipatt,tsppatt->getCoverage());
	  nAMPatterns+=1;
	  
    }
  }

  // order the AM bank by coverage
  list<AMSelection> AMPatternList;
  map< int, AMSelection >::const_iterator iAMmap = AMPatternMap.begin();
  for (;iAMmap!=AMPatternMap.end();++iAMmap) {
    AMPatternList.push_back((*iAMmap).second);
  }
  AMPatternMap.clear(); // remove the map used to store the AM patterns
  AMPatternList.sort(); // use the sorting metho of the list

  return 0;
}
