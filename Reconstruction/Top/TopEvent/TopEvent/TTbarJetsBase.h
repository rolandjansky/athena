/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARJETSBASE_H
#define TTBARJETSBASE_H

#include "DataModel/ElementLink.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

#include <vector>
#include <set>

class TTbarJetsBaseCnv_p1;

namespace TopRec {
  class TTbarHadronicTop;

  typedef std::set<unsigned> IndexSet;
  typedef std::vector<unsigned> IndexVector;

  typedef std::set<ElementLink<JetCollection> > JetSet;
  typedef std::vector<ElementLink<JetCollection> > JetVector;

  typedef IndexVector::size_type size_type;
  
  //================================================================
  class TTbarJetsBase {
  public:
    //! The ttbar is made out of the given jets in the JetCollection
    TTbarJetsBase(const JetCollection* jc, const IndexSet& jetNumbers);

    //! default constructor is needed for POOL and dictionaries
    TTbarJetsBase() {}

    size_type numJets() const { return m_jets.size(); }

    const Jet& jet(size_type i) const { return **m_jets.at(i); }
    ElementLink<JetCollection> jetLink(size_type i) const { return m_jets.at(i); }

    const JetVector& jets() const { return m_jets; }

    //! This constructor allows to mix jets from different containers.
    //! (For example one can use different containers for light and b-jets
    //! in order to apply different calibration corrections to b-jets.)
    //! The use of a set guarantees no duplicates are passed.
    TTbarJetsBase(const JetSet& jets);

    //! Using a vector instead of a set lets to preserve the order of jets.
    TTbarJetsBase(const JetVector& jets);

  protected:

    // The assignment operator of a class containing TTbarHadronicTop 
    // needs a way to copy the content of that member while keeping the
    // correct parent.
    void copyHadronicTop(TTbarHadronicTop *dst, const TTbarHadronicTop& src) const;
    friend class TTbarHadronicTop;

  private:
    JetVector m_jets;
    friend class ::TTbarJetsBaseCnv_p1;
  };

} // namespace TopRec

#endif/*TTBARJETSBASE_H*/
