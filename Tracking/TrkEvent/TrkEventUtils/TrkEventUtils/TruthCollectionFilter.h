/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file 
 *
 * This file provides @ref TruthFilters that can be used with the
 * @ref TruthInverters.
 */


#ifndef TRUTHCOLLECTIONFILTER_H
#define TRUTHCOLLECTIONFILTER_H

namespace Trk {

  /**
   * @defgroup TruthFilters truth map filtering functors
   * @{
   * 
   * A filter for the truth map is a predicate on a
   * TruthCollection::const_iterator that returns true for the accepted
   * matches.
   */
  

  /** BasicTruthCollectionFilter accepts all valid map entries, 
   * as determined by the isValid() call on the HepMcParticleLink.
   * See @ref TruthInverters.
   * Requirements on the TruthCollection: the mapped_type of the
   * TruthCollection should provides a particleLink() call returning a
   * const HepMcParticleLink&.
   */
  template<class TruthCollection>
  class BasicTruthCollectionFilter {
  public:
    bool operator()(typename TruthCollection::const_iterator) const;
  };

  template<class OStream, class TruthCollection>
  OStream& operator<<(OStream& os, const BasicTruthCollectionFilter<TruthCollection>& f);


  /** This functor accepts valid entries that have associated
   *  probability>=probabilityCut.  
   *  See @ref TruthInverters.
   *  Requires that the mapped_type of
   *  the TruthCollection provides a probability() call, in addition
   *  to the BasicTruthCollectionFilter's requirements.
   */
  template<class TruthCollection>
  class PCutTruthCollectionFilter {
  public:
    PCutTruthCollectionFilter(double probabilityCut);
    bool operator()(typename TruthCollection::const_iterator) const;
    double probabilityCut() const { return m_probabilityCut; }
  private:
    BasicTruthCollectionFilter<TruthCollection> m_basicFilter;
    double m_probabilityCut;
  };

  template<class OStream, class TruthCollection>
  OStream& operator<<(OStream& os, const PCutTruthCollectionFilter<TruthCollection>& f);


  /** Works like PCutTruthCollectionFilter, but also gathers some stats
   * about the collections. See @ref TruthInverters.
   * Same requirements on TruthCollection as for
   * PCutTruthCollectionFilter.
   */
  template<class TruthCollection>
  class StatTruthCollectionFilter {
  public:
    StatTruthCollectionFilter(double probabilityCut);
    bool operator()(typename TruthCollection::const_iterator) const;

    double probabilityCut() const { return m_probabilityCut; }

    /** To get stats on the number of invalid TrackTruth entries in the
     * truth collection. 
     */
    unsigned numTruthTotal() const { return m_numTruthTotal; }
    unsigned numTruthInvalid() const { return m_numTruthInvalid; }
    unsigned numTruthNoVtx() const { return m_numTruthNoVtx; }
  private:
    BasicTruthCollectionFilter<TruthCollection> m_basicFilter;
    double m_probabilityCut;
    mutable unsigned m_numTruthTotal;
    mutable unsigned m_numTruthInvalid;
    mutable unsigned m_numTruthNoVtx;
  };

  template<class OStream, class TruthCollection>
  OStream& operator<<(OStream& os, const StatTruthCollectionFilter<TruthCollection>& f);

  /**
   * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2005 
   * @}
   */
}

//================================================================
#include "TrkEventUtils/TruthCollectionFilter.icc"

#endif/*TRUTHCOLLECTIONFILTER_H*/
