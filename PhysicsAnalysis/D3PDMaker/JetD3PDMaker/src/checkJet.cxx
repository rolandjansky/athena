/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/checkJet.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Check for pathological jets.
 */


#include "checkJet.h"
#include "JetEvent/Jet.h"


namespace D3PD {


/**
 * @brief Check for bad jets.
 * @param jet Jet to check.
 *
 * The reconstruction can make infinitely-recursive jets, than contain
 * themselves as constituents.  If we try to do anything on those jets
 * that accesses the consituents, we'll crash.  This function tests for
 * this pathology, allowing us to skip those jets.
 */
bool checkJet (const Jet& jet)
{
  bool bad = false;
  Jet::const_iterator beg = jet.begin();
  Jet::const_iterator end = jet.end();
  for (; beg != end; ++beg) {
    const INavigable4Momentum* in4m = *beg;
    const Jet* jet2 = dynamic_cast<const Jet*>(in4m);
    if (jet2) {
      if (jet2 == &jet) bad = true;
      Jet::const_iterator beg2 = jet2->begin();
      Jet::const_iterator end2 = jet2->end();
      for (; beg2 != end2; ++beg2) {
        const INavigable4Momentum* in4m2 = *beg2;
        const Jet* jet3 = dynamic_cast<const Jet*>(in4m2);
        if (jet3 == &jet || jet3 == jet2) bad = true;
      }
    }
  }

  return !bad;
}


} // namespace D3PD
