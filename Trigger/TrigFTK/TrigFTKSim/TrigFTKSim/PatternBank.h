/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PATTERNBANK_H
#define PATTERNBANK_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <istream>
#include <iostream>
#include <ostream>
#include <fstream>
#include <cassert>

#include "../TrigFTKSim/ftk_dcap_helper.h"

class PatternDefinition {
 private: 
  int  m_id; // road ID
  int  m_nplanes; // number of the planes
  int *m_ss_id; //[m_nplanes] array of the super-strip ID
  
  int  m_sector_id; // sector id
  float  m_nevents; // number of training events within this pattern
  
  unsigned int *m_hashes; //! if generated store the hashes
  
 public:
  PatternDefinition();
  PatternDefinition(int,int,int *ids=0);
  PatternDefinition(int,int,int,int,int *ids=0);
  PatternDefinition(const PatternDefinition&);
  ~PatternDefinition();
  
  void setID(int v) { m_id = v; }
  int getID() const { return m_id; }
  void setNPlanes(int v);
  int getNPlanes() const { return m_nplanes; }
  void setSSIDs(int *v) { for (int i=0;i<m_nplanes;++i) m_ss_id[i] = v[i]; }  
  void replaceSSIDs(int *v) { m_ss_id = v; } // careful - no check!  
  void setSSID(int il,int id) { m_ss_id[il] = id; }
  int  getSSID(int il) const { return m_ss_id[il]; }
  void setSectorID(int v) { m_sector_id = v; }
  int getSectorID() const { return m_sector_id; }
  void setNEvents(float v) { m_nevents = v; }
  float getNEvents() const { return m_nevents; }
  void addEvent() { m_nevents++; }

  int getNMissing() const { 
    int n_missing=0;
    for(int i=0; i < m_nplanes; i++) {
      if( m_ss_id[i]/100 == -999 ) n_missing++;
    }

    return n_missing; 
  }

  unsigned int getBitMask() const {
    unsigned int bitmask = 0;
    for(int i=0; i < m_nplanes; i++) {
      if( m_ss_id[i]/100 != -999 )
	bitmask |= (1<<i);
    }
    return bitmask;
  }
  
  PatternDefinition& operator=(const PatternDefinition&);
  
  // pattern ordering: first by sector ID, then by each layer
  bool operator<(const PatternDefinition& p) const {
    if(m_sector_id<p.m_sector_id) return true;
    if(m_sector_id>p.m_sector_id) return false;
    for(int i=0; i<m_nplanes; i++) {
      if(m_ss_id[i]<p.m_ss_id[i]) return true;
      if(m_ss_id[i]>p.m_ss_id[i]) return false;
    }
    // in case of equality, we choose "false":
    return false;
  }
  
  void generateHashes();
  unsigned int getHash(int);
  
  void dump(std::ostream&);

  friend std::istream& operator>>(std::istream &,PatternDefinition&);
  friend std::ostream& operator<<(std::ostream &,const PatternDefinition&);
  void Print();

  static bool doStaticRoadWarrior(const PatternDefinition &,
				  const PatternDefinition &,
				  const PatternDefinition &,
				  const PatternDefinition &);
};


class PatternDeletionInfo {
private:
  int m_id;
  int m_bankid;
public:
  PatternDeletionInfo();
};

class KD_Elm {
 private:
  int m_dim; // dimension of the space
  int m_depth; // depth level, 0 is the top level
  int m_splitplane; // coordinate compared at this level
  PatternDefinition m_road; // A pattern
  KD_Elm *m_parent; // parent element
  KD_Elm *m_left; // left sub-tree
  KD_Elm *m_right; // right sub-tree

 public:
  // special comparison operator that sorts patterns by coverage
  struct lessThanNevents {
    bool operator() (const KD_Elm* lhs, const KD_Elm* rhs) const
    {
      // first sort by coverage. higher coverage goes first
      if (lhs->getRoad().getNEvents()>rhs->getRoad().getNEvents()) return true;
      if (lhs->getRoad().getNEvents()<rhs->getRoad().getNEvents()) return false;
      // then sort by sector number. lower number (aka higher-covered sector) goes first 
      if(lhs->getRoad().getSectorID()<rhs->getRoad().getSectorID()) return true;
      if(lhs->getRoad().getSectorID()>rhs->getRoad().getSectorID()) return false;
      // finally, sort by ss id in random order
      for(int i=0; i<lhs->getRoad().getNPlanes(); i++) {
	if(lhs->getRoad().getSSID(i)<rhs->getRoad().getSSID(i)) return true;
	if(lhs->getRoad().getSSID(i)>rhs->getRoad().getSSID(i)) return false;
      }
      // in case of equality, we choose "false":
      return false;
    }
  };

  KD_Elm(const PatternDefinition& aRoad, int depth=0, KD_Elm *parent=0);
  ~KD_Elm();

  int getSplitPlane() const { return m_splitplane; }
  int addNode(const PatternDefinition& aRoad);
  PatternDefinition& getRoad() { return m_road; }
  const PatternDefinition& getRoad() const { return m_road; }
  void findSimilar(const PatternDefinition&,
                   std::set<KD_Elm*> &);
  void getAllActive(std::set<KD_Elm*,lessThanNevents> &);

};

class PatternBank {
private:
  std::string m_fname;
  ftk_dcap::istream m_ifile;

  std::vector<int> m_pos; //!

  bool m_ready;    // if false, bank wasn't successfully loaded
  bool m_okread;   // if false, bank reading failed

  int m_nplanes; // number of the planes
  int m_npatterns; // number of the patterns
  int m_maxnhits;   // max number of hits in a pattern (used in bank construction)
  int m_maxnmissing; // maximum number of missing hits in the bank

  bool m_usehash; // if true for each pattern the relative hashes are stored
  //bool m_missingHitPatterns; // True if patterns have missing hits
  std::multimap<unsigned int,int> *m_maps; //! hashed map, each map is related to a missing plane

  std::map<int,PatternDefinition> m_pattcache; //! pattern cache [use with large banks]
  std::vector<PatternDefinition> m_preload; //! preload all patterns [use with small or zipped banks]
  //std::map<PatternDefinition,int> m_lookup; //! optional lookup table that maps ss[11] -> ssID
  std::map<unsigned int, KD_Elm*> m_kdlookup; //! Lookup table for patterns with missing hits, indexed by bitmask

  void mergeOverlaps();

public:
  PatternBank(bool usehash=false);
  PatternBank(const char*, bool usehash=false);

  void setFile(const char*);

  void setUseHash(bool v) { m_usehash = v; }
  bool getUseHash() const { return m_usehash; }

  int getNPlanes() const { return m_nplanes; }
  void setNPlanes(int v) { m_nplanes = v; }

  int getNPatterns() const { return m_npatterns; }

  bool Ready() const { return m_ready; }

  // Slower on-demand access using m_pattcache [ O(log(n)) time ]
  // This can't be used with compressed/dcache banks (due to lack of seekg() support)
  void InitOnDemand();    // initialize for use with m_pattcache 
  void cacheLoad(int); // add a pattern definition into the cache
  void cacheClear();   // clear the cache
  int countSimilar(int,PatternDefinition&);
  std::vector<PatternDefinition> getSimilar(int,PatternDefinition&);
  PatternDefinition getPattern(int,bool toCache=false);

  // Fast preloaded access using m_preload [ constant time ]
  // This HAS to be used for compressed/dcache banks
  bool InitPreload(); // initialize for use with m_preload
  bool InitLookup(bool clear=false); // populate m_lookup table out of m_preload
  bool AppendLookup(int npromote=0); // adding patterns directly into m_lookup structure (skipping m_preload step)
  //void Dump();
  // Saves PatternBank into a standard .patt file
  void SaveAs(const char *fname);
  const PatternDefinition& getPatternFast(unsigned int id) const {
    return m_preload[id];
  }


  int getPatternID(const PatternDefinition& p) {
    unsigned int bitmask = p.getBitMask();
    
    if( m_kdlookup.count(bitmask) == 0 )
      return -1;

    std::set<KD_Elm*> sims;
    m_kdlookup[bitmask]->findSimilar(p,sims);
    assert(sims.size() <= 1); // Sanity check

    if( sims.size() )
      return (*sims.begin())->getRoad().getID();

    return -1;
  }

  // Injects a pattern into m_kdlookup
  void addPattern(const PatternDefinition& p) {
    int nmissing = p.getNMissing();
    if( nmissing > m_maxnmissing ) m_maxnmissing = nmissing;
    unsigned int bitmask = p.getBitMask();
    
    if( m_kdlookup.count(bitmask) == 0 )
      m_kdlookup[bitmask] = new KD_Elm(p);
    else {
      // Check if the pattern is already there:
      std::set<KD_Elm*> sims;
      m_kdlookup[bitmask]->findSimilar(p,sims);
      assert(sims.size() <= 1); // Sanity check
      if( sims.size() ) {
	if( (*sims.begin())->getRoad().getSectorID() <= p.getSectorID() ) {
	  // Just increment the one in the tree
	  (*sims.begin())->getRoad().setNEvents((*sims.begin())->getRoad().getNEvents() + p.getNEvents() );
	}
	else {
	  // Need to deactivate the one in the tree and add this one
	  float newCount = p.getNEvents();
	  newCount += (*sims.begin())->getRoad().getNEvents();
	  (*sims.begin())->getRoad().setNEvents(-1);
	  m_kdlookup[bitmask]->addNode(p);
	}
      }
      else
	m_kdlookup[bitmask]->addNode(p);
    }
  }
};

#endif // PATTERNBANK_H
