/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTSPBANK_H
#define FTKTSPBANK_H

#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/tsp/TSPMap.h"
#include "TrigFTKSim/tsp/TSPLevel.h"
#include "TrigFTKSim/tsp/FTKAMSplit.h"
#include <TFile.h>

#include <string>
#include <vector>

class FTKTSPBank : public FTK_AMBank{
private:
  TSPLevel *m_TSPProcessor; // the object could contain a nested TSP level

  TFile *m_file; // ROOT bank format

  int m_SimulateTSP; // the flag is 1 if the TSP is emulated, default
  int m_npatternsTSP; // number of TSP patterns
  int m_TSPMinCoverage; // minimum coverage of the TSP patterns
  int m_setAMSize; // if 1 or 2 the number of patterns to load is at AM level, not TSP level

  // decide splitting strategy for AM patterns with DC bits
  // default=0: no splitting
  // >0       : split AM patterns and optimize DC usage
  int m_AMSplit;

  // max number of AM patterns after split, default=-1: no limits
  int m_maxAMAfterSplit; 

  //minimum threshold for DVol/DNPatt for split; default=0
  int m_minDVolOverDNPatt;
  
  int m_DCMatchMethod; // define the was the DC match

  FTKSSMap *m_ssmap_tsp; // this SS map describe the detector at TSP level

  std::map<int,FTKSS> *m_splitted_ssmap; // maps storing the fired SS after the TSP splitting
  std::string m_cachepath; // path of the cache file

  std::vector<int> m_PatternDBID; // vector of the ID used by a pattern in the DB

  bool m_makecache; // if true save the local bank as cache

protected:
  virtual void data_organizer();
  void am_in_dc();
  virtual void am_in();
  virtual void am_output();
  virtual void attach_SS();
  virtual void pattlookup_make_map();  // STL hashmap version

  void filterPattern(int);

  void dumpCache();
public:
  FTKTSPBank(int,int);
  virtual ~FTKTSPBank();

  virtual void end();

  virtual int readROOTBank(const char*, int maxpatts=-1);
  int readROOTBankCache(const char *);

  void setSimulateTSP(int flag) { m_SimulateTSP = flag; }
  const int& getSimulateTSP() { return m_SimulateTSP; }

  void setTSPMinCoverage(int val) { m_TSPMinCoverage = val; }
  const int& getTSPMinCoverage() { return m_TSPMinCoverage; }

  void setAMSize(int val) { m_setAMSize = val; }
  const int& getAMSize() { return m_setAMSize; }

  const int& getNPatternsTSP() { return m_npatternsTSP; }

  void setSSMapTSP(FTKSSMap *ssmap) { m_ssmap_tsp = ssmap; }
  const FTKSSMap *getSSMapTSP() { return m_ssmap_tsp; }

  void setMakeCache(bool v) { m_makecache = v; }
  bool getMakeCache() const { return m_makecache; }

  void setCachePath(const char *);
  const std::string& getCachePath() const { return m_cachepath; }
  
  void setAMSplit(int val){m_AMSplit = val;}
  const int &getAMSplit(){return m_AMSplit;}

  void setDCMatchMethod(int v) { m_DCMatchMethod = v; }
  const int& getDCMatchMethod() const { return m_DCMatchMethod; }

  void setMaxAMAfterSplit( int maxAMAfterSplit){ m_maxAMAfterSplit=maxAMAfterSplit; }
  const int& getMaxAMAfterSplit() const { return m_maxAMAfterSplit; }

  void setMinDVolOverDNPatt(int minDVolOverDNPatt){m_minDVolOverDNPatt = minDVolOverDNPatt;}
  const int& getMinDVolOverDNPatt() const { return m_minDVolOverDNPatt;}



  virtual const std::list<FTKRoad>& getRoads();
};


class AMSelection {
private:
    int m_AMID;
    unsigned m_Coverage;
    std::vector<int> m_TSPID;
    std::vector<UInt_t> m_HalfPlaneMask;
    std::vector<unsigned> m_TSPCoverage;
    std::vector< std::vector<int> > m_DeltaVolumeOverDeltaAM;

public:
    AMSelection();
    AMSelection(int amid, int tspid, unsigned coverage, unsigned int hbmask = 0);
    void addTSP(int, unsigned, unsigned int hbmask = 0);

    int getAMID() const{ return m_AMID; }

    const std::vector<int> &getTSPIDs() const{return m_TSPID;}

    const std::vector<UInt_t> &getHalfPlaneMask() const{return m_HalfPlaneMask;}

    bool operator<(const AMSelection &) const;

    const std::vector<unsigned> &getTSPCoverage() const{return m_TSPCoverage;}

    void addTSPCoverage(int TSPCoverage){m_TSPCoverage.push_back(TSPCoverage);}

};

#endif
