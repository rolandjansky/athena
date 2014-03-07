/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARHADRONICTOPB_H
#define TTBARHADRONICTOPB_H

#include "DataModel/ElementLink.h"
#include "JetEvent/Jet.h"

#include "EventKernel/I4Momentum.h"
#include "FourMom/P4PxPyPzE.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TopEvent/TTbarHadronicTop.h"

#include <vector>
#include <set>

namespace TopRec {
  class TTbarHadronicTopBAccessor;

  //================================================================
  //! This represents a hadronic top in a ttbar decay, not a standalone 
  //! top quark.  In other words, this class associates a subset of 
  //! ttbar jets to a top quark.
  //! The b-jets must be explicitly identified.
  //! It is assumed that all the other jets are from the W

  class TTbarHadronicTopB  : public TTbarHadronicTop {
    friend class TTbarHadronicTopBAccessor;

    IndexVector m_indexesW;
    P4PxPyPzE m_W;
    size_type m_indexB;

    //!use by the 2 constructors 
    void makeW();
    void computeWfourMom();

    // Forbid copying or m_parent would become wrong.
    TTbarHadronicTopB(const TTbarHadronicTopB&);
    TTbarHadronicTopB& operator=(const TTbarHadronicTopB&);

  public:
    //! topJets are indexes of this top quark jets in the TTbar's numbering
    //! the b-jet index must be specified
    TTbarHadronicTopB(const TTbarJetsBase& base, const IndexSet& topJets, const size_type& indexB);
    
    //!Specify hadronic and leptonic b-jets. 
    //!All the jets expect the leptonic b-jet are associated to the hadronic top
    TTbarHadronicTopB(const TTbarJetsBase& base, const size_type& indexB, const size_type& indexLB);

    //! default constructor is needed for POOL and dictionaries
    TTbarHadronicTopB() {}

    size_type numJetsW() const { return m_indexesW.size(); }
    const IndexVector& jetIndexesW() const { return m_indexesW; }
    const Jet& jetW(size_type i) const {return parent().jet(m_indexesW.at(i)); }
    const size_type& jetindexB() const { return m_indexB; } 
    const Jet& jetB() const {return parent().jet(m_indexB); }
    const I4Momentum& W() const { return m_W; } 
    
  };

  //================================================================

} // namespace TopRec

#endif/*TTBARHADRONICTOPB_H*/
