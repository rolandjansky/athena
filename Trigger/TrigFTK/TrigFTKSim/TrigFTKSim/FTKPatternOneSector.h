/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef H_FTKPatternOneSector
#define H_FTKPatternOneSector

/* package FTKPatternOneSector
 *
 * Store all patterns of one sector in memory
 * duplicate patterns are added up (increase coverage)
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 * types/classes
 *    FTKPatternCoverageMap_t
 *       STL container holding hit patterns
 *        the number of identical hit patterns is the "coverage"
 *    FTKPatternOrderByCoverage
 *       ordering rule for iterators pointing to a FTKPatternCoverageMap_t
 *       also defines a selection method to discard patterns below 
 *       a certain coverage
 *    FTKPatternOneSectorOrdered
 *       a container which uses iterators pointing to a FTKPatternCoverageMap_t
 *       but selects and orders them depending on theircoverage data
 *    FTKPatternOneSector
 *       wrapper for a container of type FTKPatternCoverageMap_t
 *       and the sector number
 */

#include <iostream>
#include <map>
#include <set>
#include "FTKPatternWithCoverage.h"

typedef std::map<FTKHitPattern,int,FTKHitPatternCompare>
FTKPatternCoverageMap_t;

class FTKPatternOrderByCoverage {
public:
   explicit FTKPatternOrderByCoverage(int minCoverage)
      : fMinCoverage(minCoverage) { }
   virtual bool operator()(FTKPatternCoverageMap_t::const_iterator const &a,
                           FTKPatternCoverageMap_t::const_iterator const &b)
      const;
   bool IsSelected(FTKPatternCoverageMap_t::const_iterator const &a) const {
      return (*a).second>=fMinCoverage;
   }
protected:
   FTKHitPatternCompare fHitPatternCompare;
   int fMinCoverage;
};

class FTKPatternOneSectorOrdered {
public:
   FTKPatternOneSectorOrdered(FTKPatternCoverageMap_t const &patterns,
                              FTKPatternOrderByCoverage const &compare);
   typedef std::set<FTKPatternCoverageMap_t::const_iterator,
                    FTKPatternOrderByCoverage> OrderedPatternSet_t;
   typedef OrderedPatternSet_t::const_iterator Ptr_t;
   inline Ptr_t Begin(void) const { return fPatterns.begin(); }
   inline Ptr_t End(void) const { return fPatterns.end(); }
   inline FTKHitPattern const &GetHitPattern(Ptr_t const &p) const {\
      return (**p).first;
   }
   inline int GetCoverage(Ptr_t const &p) const { return (**p).second; }
   inline int GetNumberOfPatterns(void) const { return fPatterns.size(); }
   int GetSummedCoverage(void) const;
protected:
   OrderedPatternSet_t fPatterns;
};

class FTKPatternOneSector {
public:
   explicit FTKPatternOneSector(int sector) : fSector(sector) { }  // create new empty sector
   void AddPattern(FTKPatternWithCoverage const &p);

   inline int GetSector(void) const { return fSector; } // get sector number
   inline int GetNumberOfPatterns(void) const { return fPatterns.size(); }

   FTKPatternOneSectorOrdered *OrderPatterns
   (FTKPatternOrderByCoverage const &compare) const;

   typedef FTKPatternCoverageMap_t::const_iterator Ptr_t;
   inline Ptr_t Begin(void) const { return fPatterns.begin(); }
   inline Ptr_t End(void) const { return fPatterns.end(); }
   inline FTKHitPattern const &GetHitPattern(Ptr_t const &p) const {\
      return (*p).first;
   }
   inline int GetCoverage(Ptr_t const &p) const { return (*p).second; }

 protected:
   int fSector;
   FTKPatternCoverageMap_t fPatterns;
};

#endif
