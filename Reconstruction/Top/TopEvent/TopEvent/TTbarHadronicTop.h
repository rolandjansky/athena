/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARHADRONICTOP_H
#define TTBARHADRONICTOP_H

#include "DataModel/ElementLink.h"
#include "JetEvent/Jet.h"

#include "EventKernel/I4Momentum.h"
#include "FourMom/P4PxPyPzE.h"

#include "TopEvent/TTbarJetsBase.h"

#include <vector>
#include <set>

namespace TopRec {

  //================================================================
  //! This represents a hadronic top in a ttbar decay, not a standalone 
  //! top quark.  In other words, this class associates a subset of 
  //! ttbar jets to a top quark.

  class TTbarHadronicTop
  { 
    P4PxPyPzE m_fourMomentum;
    const TTbarJetsBase *m_parent;
    IndexVector m_indexes;

    void computeFourMomentum();

    // When the parent TTbar is copied, our m_parent becomes invalid.
    // Disallow the default copy constructor and assignment to make sure
    // the containing class takes care of that.
    TTbarHadronicTop(const TTbarHadronicTop&);
    TTbarHadronicTop& operator=(const TTbarHadronicTop&);

    // The assignment operator of the containing class needs a way to
    // copy the content of a TTbarHadronicTop.
    friend void TTbarJetsBase::copyHadronicTop(TTbarHadronicTop *dst, const TTbarHadronicTop& src) const;
    
  public:
    //! topJets are indexes of this top quark jets in the TTbar's numbering
    TTbarHadronicTop(const TTbarJetsBase& parent, const IndexSet& topJets);
    TTbarHadronicTop(const TTbarJetsBase& parent, const IndexVector& topJets);
    
    //! default constructor is needed for POOL and dictionaries
    TTbarHadronicTop() {}

    // The ttbar this top is a part of
    const TTbarJetsBase& parent() const { return *m_parent; }

    //! 4-momentum of this top
    const I4Momentum& fourMom() const { return m_fourMomentum; }

    size_type numJets() const { return m_indexes.size(); }
    const Jet& jet(size_type i) const { return m_parent->jet(m_indexes.at(i)); }
    const IndexVector& jetIndexes() const { return m_indexes; }
  };

  //================================================================

} // namespace TopRec

#endif/*TTBARHADRONICTOP_H*/
