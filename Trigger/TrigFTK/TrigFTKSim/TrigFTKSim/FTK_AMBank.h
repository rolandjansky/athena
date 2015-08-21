/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_AMBANK_H
#define FTK_AMBANK_H

#include "ftktypes.h"
#include "FTKPattern.h"
#include "FTKHit.h"
#include "FTK_AMsimulation_base.h"

#include <string>
#include <set>
#include <unordered_set>
#include <cstdint>
#define MAXSS 100000

//#define TESTTBB
#ifdef TESTTBB
#include <tbb/tbb.h>
#endif

#define NEWSSLIST

// alternative ss_patt_lookup implementation using a GCC hashmap (controlled via m_use_lookup_map)
// this is necessary for SCTtrk case, where (int)SS encodes 3 parameters
// note: this makes road_finder ~30% slower, so use only when needed!
#include <ext/hash_map>
using namespace __gnu_cxx;

/** class that define an associative memory */
class FTK_AMBank : public FTK_AMsimulation_base {
protected:
  static const int m_maxRoads;
  static const int m_ssLookupSize;
  static int m_HBseg;
  
  static int m_WCID; // wildcard ID
  static int m_VetoID; // wildcard ID

   bool m_CachedBank; // true if the bank is cached, false (default) if not
   int *m_patterns; // list of the patterns
   int *m_patternCoverage; // pattern coverage 
   std::map<int,int> *m_sectorCoverage; // sector coverage 
   int m_totalCoverage; // total coverage 

   static const unsigned char m_matchword_countermask;
   static const unsigned char m_matchword_maskshift;

   typedef std::uint16_t pattstatus_t;
   pattstatus_t *m_patterns_matchstatus; // a single word representing the match status of a single pattern

   std::unordered_map<int,FTKSS> *m_fired_ssmap; // maps storing the fired SS
   std::unordered_set<int> *m_fired_ss;

   long *m_amout; // id found roads

   int *m_pattern_stats;

   bool m_useWC;
   std::set<int> **m_WCSS; // the lists with the SS with wildards, set from outside the class
   std::set<int> **m_WCSS2; // the lists with the SS with wildards, set from outside the class
   void applyWildcard();
   

private:
   std::list<FTKHit>  *m_bad_module_map;

 public:
   const int * getPatterns() const {return m_patterns; }// list of the patterns
 protected:
   // lookup table between ss and patterns:
   hash_map<int, std::vector<int> > *m_ss_patt_lookup_map;  /*[plane]*/
   int m_lutsepplane;
   bool m_upperindex;

   // additional parameters
   std::vector<int> m_fired_patts;

   std::vector< std::unordered_map< int, FTKSS > > m_usedssmap; // map of used SSs

   int ipatt_step;
   virtual void pattlookup_make_map();  // STL hashmap version
   
   // STL internal structures to manage the hits
   std::vector<FTKHit> *m_stlhit_sort; // an array FTK hit vectors, one per plane
   virtual void sort_hits(const std::vector<FTKHit> &);
   virtual void data_organizer();
   virtual void am_in();
   virtual void am_in_minimal();
   virtual void am_in2();
   virtual void am_output();
   virtual void attach_SS();

   inline int _SSPOS(int ip, int il) const { return ip*(m_nplanes+1)+il; }

#ifdef TESTTBB
   class DataOrganizer {
   private:
     FTK_AMBank *m_amobj;
   public:
     DataOrganizer(FTK_AMBank *);
     void operator()(const tbb::blocked_range<unsigned int>&) const; // distribute the hits into the SS
   };

   friend class DataOrganizer;
#endif

   virtual int informationMatch(FTKRoad *r1,FTKRoad *r2);

   virtual void clear();

   int which_SS(int,int);

   unsigned int getBitmask(int);

   virtual int readBankInit(); 
   virtual int readBankInitEnd(); 

   void setBadSSMap();
   void setBadSSMap2();

public:
   FTK_AMBank(int,int);
   virtual ~FTK_AMBank();

   const int& getNPatterns() const { return m_npatterns; }
   FTKPattern getPattern(int ipatt);

   int getSectorCoverage(int sector){return (*m_sectorCoverage)[sector];};
   int getTotalCoverage(){return m_totalCoverage;};
   
   void setPatternLUTBreak(unsigned il) { m_lutsepplane = il; }
   unsigned int getPatternLUTBrask() { return m_lutsepplane; }

   void setPatternLUTOutIn(bool v) { m_upperindex = v; }
   bool getPatternLUTOutIn() { return m_upperindex;}

   virtual void init();
   virtual void end();

   virtual const std::list<FTKRoad>& getRoads();
   virtual const std::unordered_map<int,FTKSS>& getStrips(int);

   virtual int readASCIIBank(const char*, int maxpatts=-1);
   virtual int readROOTBank(const char*, int maxpatts=-1);
  void setWildcardList(int plane, std::set<int> *deadSSset) 
  { m_WCSS[plane] = deadSSset;}

   int readROOTBankAMPatterns(const char *fname, int maxpatt=-1);
   int readROOTBankBank0(const char *fname, int maxpatt=-1);

   int readROOTBankFTKPatternTree(TFile* file, TString treename, int maxpatt=-1);

   int readROOTBankSectorOrdered(const char *fname, int maxpatt=-1);
   int readROOTBankSectorOrdered(TFile* file, int maxpatt=-1);

   void writeAMBank0(TFile* file,const std::string& TTreename="Bank0", const std::string& TTreeTitle="Level 0 pattern");
   void writeAMBank0(const std::string& filename,const std::string& TTreename="Bank0", const std::string& TTreeTitle="Level 0 pattern");

};
#endif // FTK_AMBANK_H
