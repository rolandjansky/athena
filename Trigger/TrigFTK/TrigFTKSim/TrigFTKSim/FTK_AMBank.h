/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_AMBANK_H
#define FTK_AMBANK_H

#include "ftktypes.h"
#include "FTKRoad.h"
#include "FTKSSMap.h"
#include "FTKPattern.h"
#include "FTKSS.h"
#include "FTKHit.h"

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#define MAXSS 100000

#define NEWSSLIST

// alternative ss_patt_lookup implementation using a GCC hashmap (controlled via m_use_lookup_map)
// this is necessary for SCTtrk case, where (int)SS encodes 3 parameters
// note: this makes road_finder ~30% slower, so use only when needed!
#include <ext/hash_map>
using namespace __gnu_cxx;

/** class that define an associative memory */
class FTK_AMBank {
protected:
  static const int m_maxRoads;
  static const int m_ssLookupSize;
  static int m_HBseg;
  
  static int m_WCID; // wildcard ID
  static int m_VetoID; // wildcard ID

   int m_BankID; // id for this bank, used to select hits
   int m_SubID; // sub-region ID, if 0 has no effect
   bool m_CachedBank; // true if the bank is cached, false (default) if not
   FTKSSMap *m_ssmap; // ss convetion map used in this bank
   FTKSSMap *m_ssmap_unused; // ss convetion map used in this bank, for unused layers
   int m_nplanes; // number of the planes for this bank
   int m_npatterns; // number of patterns in this bank
   int **m_patterns; // list of the patterns
   int *m_patternCoverage; // pattern coverage 
   std::map<int,int> *m_sectorCoverage; // sector coverage 
   int m_totalCoverage; // total coverage 

   static const unsigned char m_matchword_countermask;
   static const unsigned char m_matchword_maskshift;
   unsigned int *m_patterns_matchstatus; // a single word representing the match status of a single pattern

   std::unordered_map<int,FTKSS> *m_fired_ssmap; // maps storing the fired SS
   long m_nroads; // number of roads found
   long *m_amout; // id found roads

   int m_do_pattern_stats; // flag for the pattern statistic
   int *m_pattern_stats;

   unsigned int m_HWModeSS; // check the format of the SS-ID: 0 ì is global, 1 is local (tower based)

   bool m_StoreAllSS; // flag to ask to store all the fired SS in the output file

   bool m_useWC;
   std::set<int> **m_WCSS; // the lists with the SS with wildards, set from outside the class
   std::set<int> **m_WCSS2; // the lists with the SS with wildards, set from outside the class
   void applyWildcard();
   
   // cluster/ss information for Naoki's timing simulation
 private:
   std::vector<int> m_nao_nclus;    // number of clusters routed into current region [nplanes]
   std::vector<int> m_nao_nss;      // number of superstrips going into AM [nplanes]
   int m_nao_nroads_am;             // number of roads out of AM
   int m_nao_nroads_rw;             // number of roads after roadwarrior
   std::list<FTKHit>  *m_bad_module_map;
   std::string m_badmap_path;
   std::string m_badmap_path2;

 public:
   const std::vector<int>& naoGetNclus() { return m_nao_nclus; }
   const std::vector<int>& naoGetNss() { return m_nao_nss; }
   int naoGetNroadsAM() const { return m_nao_nroads_am; }
   int naoGetNroadsRW() const { return m_nao_nroads_rw; }

 protected:
   // lookup table between ss and patterns:
   hash_map<int,std::vector<int> > *m_ss_patt_lookup_map;  /*[plane]*/

   // additional parameters
   bool m_require_first; // require a hit in the first layer (usually Pixel B-layer)
   bool m_require_last; // require a hit in the last layer (eg, SCTtrk pseudo-layer)

   std::vector<int> m_fired_patts;

   std::list<FTKRoad> m_roads; // list of found roads
   std::vector< std::unordered_map< int, FTKSS > > m_usedssmap; // map of used SSs
   std::map< int, std::map< int, FTKSS > > m_usedssmap_ignored; // map of used SSs, in used layers

   int ipatt_step;
   void pattlookup_make_map();  // STL hashmap version
   
   // STL internal structures to manage the hits
   std::vector<FTKHit> *m_stlhit_sort; // an array FTK hit vectors, one per plane
   
   // replicate old functions and structure to populate a single AM bank
   ftk::hitlist_elm_t **hit_sort; // a list of hits for each plane
   void sort_hits(const std::vector<FTKHit> &);
   int m_nhits_hf; // number of filtered hits
   ftk::hitlist_elm_t *m_hf_out; // hit filter list
   int *m_perplane_cnt; // counter used in readout_hits() to count the hits in a plane
   void readout_hits();
   long int *m_nhits_rout; //
   ftk::hitlist_elm_t **m_rout_out; // one list per plane
   void routing();
   virtual void am_in();
   virtual void am_output();
   virtual void attach_SS();

   int informationMatch(int,int);
   void road_warrior();

   void clear();

   int which_SS(int,int);

   unsigned int getBitmask(int);

   virtual int readBankInit(); 
   virtual int readBankInitEnd(); 

   void setBadSSMap();
   void setBadSSMap2();

   int m_SaveAllRoads; // if >0 try to save as much road as possible for debug

   float m_stat_totroads; // count all the found roads to keep statistical information
   int m_stat_nevents; // number of processed events
public:
   FTK_AMBank(int,int);
   virtual ~FTK_AMBank();

   const int& getNPatterns() { return m_npatterns; }
   FTKPattern getPattern(int ipatt);

   int getSectorCoverage(int sector){return (*m_sectorCoverage)[sector];};
   int getTotalCoverage(){return m_totalCoverage;};
   
   void setRequireFirst(bool v) { m_require_first = v; }
   bool getRequireFirst() const { return m_require_first; }
   
   void setRequireLast(bool v) { m_require_last = v; }
   bool getRequireLast() const { return m_require_last; }
   
   void setSSMap(FTKSSMap *map) { m_ssmap = map; }

   void setSSMapUnused(FTKSSMap *map) { m_ssmap_unused = map; }

   void setPatternStats(int val) { m_do_pattern_stats = val; }
   int getPatternStats() const { return m_do_pattern_stats; }

   int getBankID() const { return m_BankID; }
   int getSubID() const { return m_SubID; }

   int getNPlanes() const { return m_nplanes; }

   virtual void init();
   virtual void end();

   virtual int passHits(const std::vector<FTKHit> &);
   virtual int passHitsUnused(const std::vector<FTKHit> &);

   const std::list<FTKRoad>& getRoads();
   const std::unordered_map<int,FTKSS>& getStrips(int);

   const std::map< int, std::map<int,FTKSS> >& getIgnoredStrips()
     { return m_usedssmap_ignored; }

   int readASCIIBank(const char*, int maxpatts=-1);
   virtual int readROOTBank(const char*, int maxpatts=-1);
   virtual int readDBBank(const char*, int, int) {return -1;}
   virtual int readROOTBankCache(const char*);
   
   void setSaveAllRoads(int lvl) { m_SaveAllRoads = lvl; }
   int getSaveAllRoads() const { return m_SaveAllRoads; }

  void setWildcardList(int plane, std::set<int> *deadSSset) 
  { m_WCSS[plane] = deadSSset;}

  void setBadSSMapPath(std::string s ){ m_badmap_path = s;  }
  void setBadSSMapPath2(std::string s ){ m_badmap_path2 = s;  }

  void setHWModeSS(unsigned int val) { m_HWModeSS = val; }
  const unsigned int &getHWModeSS() { return m_HWModeSS; }

  void setStoreAllSS(bool flag) { m_StoreAllSS = flag; }
  bool getStoreAllSS() { return m_StoreAllSS; }

  float getTotRoads() const { return m_stat_totroads; }
  float getTotEvents() const { return m_stat_nevents; }
};
#endif // FTK_AMBANK_H
