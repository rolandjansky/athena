/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file
 *
 * This file provides @ref TruthComparison, used with @ref TruthInverters.
 */


#ifndef MATCHCOMPARE_H
#define MATCHCOMPARE_H

namespace Trk {

  /**
   * @defgroup TruthComparison functors to compare different rec->mc matches in a truth map
   * @{
   * 
   * Compare(a,b)==true means that b is better than a.  See @ref
   * TruthInverters.
   */

  /**
   * This functor, used with @ref TruthInverters, selects rec->mc
   * truth map matches with higher associated probabilities.  Requires
   * that the mapped_type of TruthCollection provides probability()
   * and particleLink() calls.
   */
  template<class TruthCollection> 
  struct MatchCompareByProbability {
    bool operator()(typename TruthCollection::const_iterator, typename TruthCollection::const_iterator) const;
  };


  /**
   * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2005 
   * @}
   */


  //================================================================
  // Implementation
  template<class TruthCollection> 
  bool MatchCompareByProbability<TruthCollection>::
  operator()(typename TruthCollection::const_iterator a, typename TruthCollection::const_iterator b) const {
    return 
      (a->second.probability() < b->second.probability() ) || 
      ((a->second.probability() == b->second.probability() ) 
       /* prefer smaller (extended) barcodes --- bias to pick primary events & tracks */
       && (b->second.particleLink() < a->second.particleLink() ));
  }

}

#endif/*MATCHCOMPARE_H*/
