/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARJJTOP_H
#define TTBARJJTOP_H

#include "DataModel/ElementLink.h"
#include "JetEvent/JetCollection.h"

#include "TopEvent/TTbarJJ.h"
#include "TopEvent/TTbarHadronicTop.h"

#include <vector>
#include <set>

namespace TopRec {
  //================================================================
  //! All hadronic TTbar with top quarks reconstructed

  class TTbarJJTop : public TTbarJJ
  {
    TTbarHadronicTop m_htop1;
    TTbarHadronicTop m_htop2;

    static IndexSet getSetCompliment(size_type max, const IndexSet& s);
    static IndexSet getSetCompliment(size_type max, const IndexVector& s);
  public:

    //! topJets are indexes of top quark's jets in the TTbar's numbering.
    //! The rest of the jets in the parent are assigned to the other top quark.
    TTbarJJTop(const TTbarJJ& parent, const IndexSet& topJets);
    TTbarJJTop(const TTbarJJ& parent, const IndexVector& topJets);

    //! access to the tops
    const TTbarHadronicTop& top1() const { return m_htop1; }
    const TTbarHadronicTop& top2() const { return m_htop2; }

    // These two are needed to make sure the parent pointer
    // in the hadronic ttbars is set correctly.
    TTbarJJTop(const TTbarJJTop&);
    TTbarJJTop& operator=(const TTbarJJTop&);

    //! default constructor is needed for POOL and dictionaries
    TTbarJJTop() {}
  };

  //================================================================

} // namespace TopRec

#endif //TTBARJJTOP_H
