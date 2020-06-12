/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/ParticleJetLabelCommon.h"

namespace ParticleJetTools {

  // the code below is taken from ParticleJetDeltaRLabelTool with
  // minimal modification
  // --------------------------------------------------------------

    // TODO
    // can we do better by only looking at hadrons?
    inline bool isChild
        ( const xAOD::TruthParticle* p
        , const xAOD::TruthParticle* c
        ) {

        if (p->barcode() == c->barcode())
            return false;


        for (size_t iC = 0; iC < p->nChildren(); iC++) {
            const xAOD::TruthParticle* cc = p->child(iC);
	    if(!cc) continue;

            if (cc->barcode() == c->barcode()) {
                return true;
            }

            if (isChild(cc, c)) {
                return true;
            }
        }

        return false;
    }


    void childrenRemoved
        ( const std::vector<const xAOD::TruthParticle*>& parents
        , std::vector<const xAOD::TruthParticle*>& children
        ) {

        // TODO
        // this is probably very inefficient,
        // but it's simple.

        // for instance: if we remove a child from info on one parent,
        // we still loop over the child again for the next parent.

        // also, we're passing around vectors rather than their
        // references.

        // for each of the parents
        for ( size_t ip = 0
            ; ip != parents.size()
            ; ip++ ) {

            const xAOD::TruthParticle* p = parents[ip];
	    if(!p) continue;

            // the current child index
            size_t ic = 0;

            // (x) each of the potential children
            while (ic != children.size()) {

                const xAOD::TruthParticle* c = children[ic];
		if (!c) continue;

                // if citer is (recursively) a child of piter
                // remove it.
                if (isChild(p, c)) {
                    children.erase(children.begin() + ic);
                    // do not increment ic: we just removed a child.
                    continue;

                } else {
                    // increment ic: we did *not* remove a child.
                    ic++;
                    continue;
                }
            }
        }

        return;
    }


  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // End of code copied from ParticleJetDeltaRLabelTool

  void setJetLabels(xAOD::Jet& jet,
                    const PartonCounts& counts,
                    const LabelNames& names) {
    // set truth label for jets above pt threshold
    // hierarchy: b > c > tau > light
    if (counts.b)
      jet.setAttribute<int>(names.singleint, 5);
    else if (counts.c)
      jet.setAttribute<int>(names.singleint, 4);
    else if (counts.tau)
      jet.setAttribute<int>(names.singleint, 15);
    else
      jet.setAttribute<int>(names.singleint, 0);

    if (counts.b) {
      if (counts.b >= 2)
        jet.setAttribute<int>(names.doubleint, 55);

      else if (counts.c)
        jet.setAttribute<int>(names.doubleint, 54);

      else
        jet.setAttribute<int>(names.doubleint, 5);

    } else if (counts.c) {
      if (counts.c >= 2)
        jet.setAttribute<int>(names.doubleint, 44);

      else
        jet.setAttribute<int>(names.doubleint, 4);

    } else if (counts.tau)
      jet.setAttribute<int>(names.doubleint, 15);

    else
      jet.setAttribute<int>(names.doubleint, 0);

  }

}
