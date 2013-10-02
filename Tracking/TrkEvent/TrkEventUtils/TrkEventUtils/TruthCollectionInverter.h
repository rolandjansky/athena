/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file 
 * @brief This file provides @ref TruthInverters.
 */

#ifndef TRUTHCOLLECTIONINVERTER_H
#define TRUTHCOLLECTIONINVERTER_H

#include <map>

#include "TrkEventUtils/InverseTruthMap.h"
#include "TrkEventUtils/TruthCollectionFilter.h"
#include "TrkEventUtils/MatchCompare.h"

namespace Trk {
  /** @defgroup TruthInverters TruthCollection inversion functions */
  /*@{*/

  /**
   * The template function buildInverseTruthMap() fills an
   * InverseTruthMap object with data
   *
   * An inverse of a map is a multimap.  Because we want to get a map
   * instead, information may be lost in this operation.  MatchCompare
   * is a functor deciding which of the matches to keep.  If cmp(a,b) is
   * true, b will be kept, and a discarded.  MatchCompare should define
   * a total ordering (strict weak ordering does not guarantee that
   * results will be reproducible).
   *
   * The input map may contain invalid particle links, therefore we
   * must pre-filter it before the inversion.  The pre-filtering may
   * also include additional matching criteria.  This is all done
   * using a Filter functor, which returns true for accepted matches.
   *
   * @see Trk::InverseTruthMap, MatchCompare.h, TruthCollectionFilter.h
   *
   * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2005
   */
  template<class TruthCollection, class Filter, class MatchCompare>
  void buildInverseTruthMap(const TruthCollection& rec2truth, 
			    InverseTruthMap<TruthCollection> *result,
			    const Filter& filter, 
			    const MatchCompare& cmp);

  /** A fewer argument version with default MatchCompare=MatchCompareByProbability 
   */
  template<class TruthCollection, class Filter>
  void buildInverseTruthMap(const TruthCollection& rec2truth, 
			    InverseTruthMap<TruthCollection> *result,
			    const Filter& filter) 
  {
    buildInverseTruthMap(rec2truth, result, filter, MatchCompareByProbability<TruthCollection>() );
  }
  
  /** A fewer argument version with default MatchCompare and Filter=BasicTruthCollectionFilter 
   */
  template<class TruthCollection>
  void buildInverseTruthMap(const TruthCollection& rec2truth, 
			    InverseTruthMap<TruthCollection> *result)				 
  {
    buildInverseTruthMap(rec2truth, 
			 result, 
			 BasicTruthCollectionFilter<TruthCollection>(), 
			 MatchCompareByProbability<TruthCollection>() 
			 );
  }

  //================================================================
  // Implementation of the "main" inverter.

  template<class TruthCollection, class Filter, class MatchCompare>
  void buildInverseTruthMap(const TruthCollection& rec2truth, 
			    InverseTruthMap<TruthCollection> *result,
			    const Filter& filter, 
			    const MatchCompare& cmp)
  {
    result->clear();
    typedef std::map<const HepMC::GenParticle*, typename TruthCollection::const_iterator> Helper_t;
    Helper_t seen_links;
    for(typename TruthCollection::const_iterator i=rec2truth.begin(); i!=rec2truth.end(); i++) {
      if(filter(i)) {
	const HepMC::GenParticle *particle = i->second.particleLink().cptr();
	typename Helper_t::iterator p = seen_links.find(particle);
	//----------------------------------------------------------------
	if(p == seen_links.end()) { // match to a new GenParticle
	  // Add this match to the output map.
	  (*result)[particle] = i->first ;
	  // and remember we saw the particle
	  seen_links[particle] = i;
	}
	//----------------------------------------------------------------
	else { // another match to the same GenParticle
	  // Need to decide which of the matches to keep.
	  if(cmp(p->second,i)) { // the new match is better
	    // Overwrite value for the key in the output map
	    (*result)[particle] = i->first ;
	    // and in the helper data structure.
	    seen_links[particle] = i;
	  }
	}
      }
    }
  }

  //================================================================

  /*@}*/

} // namespace 

#endif/*TRUTHCOLLECTIONINVERTER_H*/
