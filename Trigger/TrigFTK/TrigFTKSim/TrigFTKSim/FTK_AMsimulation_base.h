/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_AMBANK_BASE_H
#define FTK_AMBANK_BASE_H

#include <list>
#include <vector>
#include <map>
#include <unordered_map>

#include <sys/time.h>
#include <sys/resource.h>

#include "FTKSSMap.h"
#include "FTKRoad.h"
#include "FTKSS.h"

/* class FTK_AMsimulation_base
 *
 * base class to perform an emulation of the FTK pattern recognition
 *  (simulation of the Associated Memory hardware)
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

class FTK_AMsimulation_base {
 public:
   FTK_AMsimulation_base(int id, int subid);
   virtual ~FTK_AMsimulation_base();

   // steering parameter, to do pattern counting or not
   void setPatternStats(int val) { m_do_pattern_stats = val; }
   int getPatternStats() const { return m_do_pattern_stats; }

   // steering whether getStrips() returns all SS or only those connected 
   //    with roads
   void setStoreAllSS(bool flag) { m_StoreAllSS = flag; }
   bool getStoreAllSS() { return m_StoreAllSS; }

   // steering parameter to save *really* all roads or only those
   //   above the threshold of hits/layer
   //   (not all types of simulation implement this feature)
   void setSaveAllRoads(int lvl) { m_SaveAllRoads = lvl; }
   int getSaveAllRoads() const { return m_SaveAllRoads; }

   // get/set number of planes
   virtual void setNPlanes(int nPlanes);
   int getNPlanes() const { return m_nplanes; }

   // if set, a hit in the innermost layer is required to form a road
   void setRequireFirst(bool v) { m_require_first = v; }
   bool getRequireFirst() const { return m_require_first; }
    
   // if set, a hit in the outermost layer is required to form a road
   void setRequireLast(bool v) { m_require_last = v; }
   bool getRequireLast() const { return m_require_last; }

   // set (coarse) superstrip geometry
   void setSSMap(FTKSSMap *map) { m_ssmap = map; }
   FTKSSMap* getSSMap() const { return m_ssmap; }

   // set (coarse) superstrip geometry for hits passed to passHitsUnused()
   void setSSMapUnused(FTKSSMap *map) { m_ssmap_unused = map; }
   FTKSSMap* getSSMapUnused() const { return m_ssmap_unused; }

   // set path for reading wildcards
   void setBadSSMapPath(std::string s ){ m_badmap_path = s;  }
   const char *getBadSSMapPath(void) const { return m_badmap_path.c_str(); }

   // set another path for reading wildcards
   void setBadSSMapPath2(std::string s ){ m_badmap_path2 = s;  }
   const char *getBadSSMapPath2(void) const { return m_badmap_path2.c_str(); }

   // choose alternative pattern finding algorithm
   //  (not all types of simulation implement this feature)
   void setUseMinimalAMIN(bool v) { m_useMinimalAMIN = v; }
   bool getUseMinimalAMIN() const { return m_useMinimalAMIN; }

   // control road counter
   void clearNRoads() { m_nroads=0; m_nroads_complete = 0; m_nroads_misspix = 0; m_nroads_misssct = 0;}
   void setNRoads(long nroad) { m_nroads=nroad; }
   void setNRoads_complete(long nroad) { m_nroads_complete=nroad; }
   void setNRoads_misspix(long nroad) { m_nroads_misspix=nroad; }
   void setNRoads_misssct(long nroad) { m_nroads_misssct=nroad; }
   void countNRoads(long delta=1) { m_nroads+=delta; }
   void countNRoads_complete(long delta=1) { m_nroads_complete+=delta; }
   void countNRoads_misspix(long delta=1) { m_nroads_misspix+=delta; }
   void countNRoads_misssct(long delta=1) { m_nroads_misssct+=delta; }
   long getNRoads() const { return m_nroads; }
   long getNRoads_complete() const { return m_nroads_complete; }
   long getNRoads_misspix() const { return m_nroads_misspix; }
   long getNRoads_misssct() const { return m_nroads_misssct; }

   // bank and subregion identifier
   int getBankID() const { return m_BankID; }
   int getSubID() const { return m_SubID; }

   // clear "nao" statistics
   void naoClear(void);
   // cluster/ss information for Naoki's timing simulation
   const std::vector<int>& naoGetNclus() { return m_nao_nclus; }
   const std::vector<int>& naoGetNclus_road() { return m_nao_nclus_road; }
   const std::vector<int>& naoGetNss() { return m_nao_nss; }
   void naoClusPushBack(int i) { m_nao_nclus.push_back(i); }
   void naoClusRoadAdd(unsigned int iplane, int n=1) { if (iplane < m_nao_nclus_road.size()) m_nao_nclus_road[iplane] += n;};

   void naoSSPushBack(int i) { m_nao_nss.push_back(i); }
   int naoGetNroadsAM() const { return m_nao_nroads_am; }
   int naoGetNroadsAMComplete() const { return m_nao_nroads_am_complete; }
   int naoGetNroadsAMMissPix() const { return m_nao_nroads_am_misspix; }
   int naoGetNroadsAMMissSCT() const { return m_nao_nroads_am_misssct; }
   int naoGetNroadsRW() const { return m_nao_nroads_rw; }
   void naoSetNroadsAM(int i) { m_nao_nroads_am=i; }
   void naoSetNroadsAMComplete(int i) { m_nao_nroads_am_complete=i; }
   void naoSetNroadsAMMissPix(int i) { m_nao_nroads_am_misspix=i; }
   void naoSetNroadsAMMissSCT(int i) { m_nao_nroads_am_misssct=i; }
   void naoSetNroadsRW(int i) { m_nao_nroads_rw=i; }

   // count events and number of roads
   float getTotRoads() const { return m_stat_totroads; }
   float getTotEvents() const { return m_stat_nevents; }
   void addTotStat(long nRoads);

   //=============================================================
   // purely virtual methods
   //
   // return the superstrips after road finding
   //   result is steered by setStoreAllSS(bool)
   virtual const std::unordered_map<int,FTKSS>& getStrips(int)=0;
   //
   // return list of the roads found by this simulation
   virtual const std::list<FTKRoad>& getRoads()=0;
   //
   // decide whether two roads are identical
   //   this method is used by the default road_warrior() method below
   virtual int informationMatch(FTKRoad *r1,FTKRoad *r2)=0; // needed
   //
   // initialize data structures for this event (called by pass_hits)
   virtual void clear(void)=0;
   //
   // sort hits into FTK internal structures (called by pass_hits)
   virtual void sort_hits(const std::vector<FTKHit> &)=0;
   //
   // simulate the data organizer (called by pass_hits)
   virtual void data_organizer(void)=0;
   //
   // simulate the AM input (called by pass_hits)
   virtual void am_in(void)=0;
   //
   // simulate the AM output (called by pass_hits)
   virtual void am_output(void)=0;
   //
   // virtual methods, defaulting to "do nothing"
   //   read pattern banks in various formats
   virtual int readDBBank(const char *path, int nPattern, int level);
   virtual int readASCIIBank(const char*, int maxpatts=-1);
   virtual int readROOTBank(const char*, int maxpatts=-1);
   virtual int readROOTBankCache(const char*);

   // virtual with default methods
   //
   // clear/initialize  global stats
   //   called by athena at beginning of job
   //   default method clears global event and road counters
   virtual void init();
   //
   // finalize global stats, clean up
   //   called by athena at end of job
   //   (default is to do nothing)
   virtual void end();
   //
   // run simulation by passing a list of FTKHit
   //   calls other virtual methods:
   virtual int passHits(const std::vector<FTKHit> &);
   //
   // remove duplicate roads from the list of roads
   //   to be called by pass_hits()
   //   uses the method informationMatch()
   virtual void road_warrior();
   //
   // methods dealing with hits which are passed to this class
   //   but are not used for the pattern recognition (different layer?)
   virtual const std::map< int, std::map<int,FTKSS> >& getIgnoredStrips();
   virtual int passHitsUnused(const std::vector<FTKHit> &);
   //
   // utility to print the list of roads
   void printRoads(std::list<FTKRoad> const &roads,int sectorNumber) const;

 protected:
   std::list<FTKRoad> m_roads; // list of found roads

   int m_BankID; // id for this bank, used to select hits
   int m_SubID; // sub-region ID, if 0 has no effect
   int m_npatterns; // number of patterns in this bank
   int m_do_pattern_stats; // flag for the pattern statistic
   bool m_StoreAllSS; // flag to ask to store all the fired SS in the output file
   int m_SaveAllRoads; // if >0 try to save as much road as possible for debug
   int m_nplanes; // number of the planes for this bank
   bool m_require_first; // require a hit in the first layer (usually Pixel B-layer)
   bool m_require_last; // require a hit in the last layer (eg, SCTtrk pseudo-layer)
   FTKSSMap *m_ssmap; // ss conversion map used in this bank
   FTKSSMap *m_ssmap_unused; // ss convetion map used in this bank, for unused layers
   // cluster/ss information for Naoki's timing simulation
   std::vector<int> m_nao_nclus;    // number of clusters routed into current region [nplanes]
   std::vector<int> m_nao_nclus_road;    // number of clusters routed into current region [nplanes] IN a road, allowing for duplicates
   std::vector<int> m_nao_nss;      // number of superstrips going into AM [nplanes]
   int m_nao_nroads_am;             // number of roads out of AM
   int m_nao_nroads_am_complete;             // number of roads out of AM, no missing hits
   int m_nao_nroads_am_misspix;             // number of roads out of AM, missing at least one hit in pixel layer
   int m_nao_nroads_am_misssct;             // number of roads out of AM, missing at least one hit in sct layer
   int m_nao_nroads_rw;             // number of roads after roadwarrior

   float m_stat_totroads; // count all the found roads to keep statistical information
   int m_stat_nevents; // number of processed events
   long m_nroads; // number of roads found
   long m_nroads_complete; // number of roads that are complete
   long m_nroads_misspix; // number of roads missing at least one pixel hit
   long m_nroads_misssct; // number of roads missing at least one silicon hit

   std::string m_badmap_path;
   std::string m_badmap_path2;

   std::map< int, std::map< int, FTKSS > > m_usedssmap_ignored; // map of used SSs, in used layers

   bool m_useMinimalAMIN; // if true the minimal amin is used, no LUT required

};

#endif
