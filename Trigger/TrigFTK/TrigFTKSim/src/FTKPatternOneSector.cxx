/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* package FTKPatternOneSector
 *
 * Store all patterns of one sector in memory
 * duplicate patterns are added up (increase coverage)
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

#include "TrigFTKSim/FTKPatternOneSector.h"
#include <TDirectory.h>
#include <TList.h>
#include <TLeaf.h>
#include <TObjString.h>
#include <stdlib.h>

using namespace std;

//======================== class FTKPatternOneSector =================

void FTKPatternOneSector::AddPattern(FTKPatternWithCoverage const &p) {
   // insert pattern if needed and increase coverage
   fPatterns[p.GetHitPattern()] += p.GetCoverage();
}

FTKPatternOneSectorOrdered *FTKPatternOneSector::OrderPatterns
(FTKPatternOrderByCoverage const &compare) const {
   // return ordered patterns, using "compare" rule
   return new FTKPatternOneSectorOrdered(fPatterns,compare);
}

//======================== class FTKPatternOneSectorOrdered  =========

FTKPatternOneSectorOrdered::FTKPatternOneSectorOrdered
(FTKPatternCoverageMap_t const &patterns,
 FTKPatternOrderByCoverage const &compare) : fPatterns(compare) {
   for(FTKPatternCoverageMap_t::const_iterator i=patterns.begin();
       i!=patterns.end();i++) {
      if(compare.IsSelected(i))
         fPatterns.insert(i);
   }
}

int FTKPatternOneSectorOrdered::GetSummedCoverage() const {
   int r=0;
   for(Ptr_t i=Begin();i!=End();i++) {
      r += GetCoverage(i);
   }
   return r;
}

//======================== class FTKPatternOrderByCoverage =========

bool FTKPatternOrderByCoverage::operator()
   (FTKPatternCoverageMap_t::const_iterator const &a,
    FTKPatternCoverageMap_t::const_iterator const &b) const {
   // order by coverage (higher coverage first)
   if((*a).second>(*b).second) return true;
   if((*a).second<(*b).second) return false;
   // for same coverage, order by hit numbers (smallest hits first)
   return fHitPatternCompare((*a).first,(*b).first);
}
