/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARJJ_H
#define TTBARJJ_H

#include "DataModel/ElementLink.h"
#include "JetEvent/JetCollection.h"

#include "EventKernel/I4Momentum.h"
#include "FourMom/P4PxPyPzE.h"

#include "TopEvent/TTbarJetsBase.h"

#include <vector>
#include <set>

class TTbarJJCnv_p1;

namespace TopRec {

  //================================================================
  //! This represent jets that belong to a hadronically decaying ttbar
  //! pair, without any associations.

  class TTbarJJ : public TTbarJetsBase
  {
    friend class ::TTbarJJCnv_p1;

    P4PxPyPzE m_fourMomentum;

    void update4momentum();

  public:
    //! The ttbar is made out of the given jets in the JetCollection
    TTbarJJ(const JetCollection* jc, const IndexSet& jetNumbers);

    //! 4-momentum of the ttbar pair
    const I4Momentum& fourMom() const { return m_fourMomentum; }

    //! This constructor allows to mix jets from different containers.
    //! (For example one can use different containers for light and b-jets
    //! in order to apply different calibration corrections to b-jets.)
    //! The use of a set guarantees no duplicates are passed.
    TTbarJJ(const JetSet& jets);

    //! Using a vector instead of a set lets to preserve the order of jets.
    TTbarJJ(const JetVector& jets);

    //! default constructor is needed for POOL and dictionaries
    TTbarJJ() {}
  };

} // namespace TopRec

#endif //TTBARJJ_H
