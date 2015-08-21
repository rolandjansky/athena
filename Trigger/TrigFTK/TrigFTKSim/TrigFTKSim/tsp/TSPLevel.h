/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TSPLEVEL_H_
#define TSPLEVEL_H_

#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/FTKSS.h"
#include "TrigFTKSim/tsp/TSPMap.h"

#include <TFile.h>

#include <vector>
#include <map>

class FTKTSPBank;
class AMSelection;

class TSPLevel{
public:
	///Constructor for AM - TSP
  TSPLevel(TFile* file, int bankID, FTKTSPBank *AMbank,
           const std::list<AMSelection>  &amlist, unsigned threshold,
           FTKSSMap* ssTopMap, FTKSSMap* ssBottomMap,
           std::map<int, FTKSS> *ssmaps, bool onlydc=false);
  TSPLevel(int bankID, int, unsigned , FTKSSMap* ssTopMap,
           FTKSSMap* ssBottomMap,
           std::map<int, FTKSS> *ssmaps, bool onlydc=false);

  void simulate(FTKRoad& road);

  const std::vector<std::vector<int> >& getSubPatternID() const { return m_subPatternID; }

  const std::vector<std::vector<int> >& getHBMask() const { return m_hbmask; }
  unsigned int getHBMask(int, int) const;

  std::map<int,FTKSS>* getSplittedSSMap() const { return m_splitted_ssmap; }
  const std::vector<int>& getDCMask() const { return m_dcmask; }
  unsigned int getDCMask(int, int) const;

  const std::vector< std::vector<unsigned int> >& getSubSSMasks() const { return m_subssmasks; }
  const TSPMap& getTSPMap() const { return m_tspmap; }
  const int& getNPatterns() const { return m_npatterns; }

  void fillPatternInfo(int, FTKPattern&, bool writeTSP);
  int getPatternInfo(int, const FTKPattern*, bool readTSP);
  int getPatternInfo(int, int, int/*, bool*/);
  void clearExtraInfo();

private:
  int m_nplanes;
  int m_bankID;
  int m_threshold;
  int m_nroads;
  int m_npatterns; // Number of TSP patterns loaded

  bool m_dcOnly; // then true the TSP feature is not prepared, while only the don't care fields are

  std::vector<std::vector<int> > m_subPatternID; // IDs of the sub-patterns
  std::vector<std::vector<int> > m_hbmask; // mask of the half-layer representazion
  std::vector<int> m_dcmask; // vector of DC mask
  std::vector< std::vector<unsigned int> > m_subssmasks; // masks of the sub-SS in each layer
  std::map<int,FTKSS> *m_splitted_ssmap; // pointer to the map of splitted map, usable by the TSP

  TSPMap m_tspmap; // map storing the information to navigate trought the TSP

  int evaluateSubSSMask(int, int, int, int);

  // runtime variables used while the file is read
  int run_ipatt;
  int run_fatherID;
  int run_dcmask;
  int run_hbmask;
  std::vector<int> run_subssmasks;
  std::vector<int> run_TSPPatternIDs;
  std::vector<int> run_hbmasks;
  void storeAMInfo();
};

#endif /* TSPLEVEL_H_ */
