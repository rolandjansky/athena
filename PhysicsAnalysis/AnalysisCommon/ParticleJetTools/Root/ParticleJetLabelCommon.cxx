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
                    const Particles& particles,
                    const LabelNames& names) {

    // we also want to save the maximum pt of the labeling partons
    auto maxPt = [](const auto& container) -> float {
      if (container.size() == 0) return NAN;
      auto itr = std::max_element(container.begin(), container.end(),
                                  [](auto* p1, auto* p2) {
                                    return p1->pt() < p2->pt();
                                  });
      return (*itr)->pt();

    };

    // set truth label for jets above pt threshold
    // hierarchy: b > c > tau > light
    if (particles.b.size()) {
      jet.setAttribute<int>(names.singleint, 5);
      jet.setAttribute<float>(names.pt, maxPt(particles.b));
    } else if (particles.c.size()) {
      jet.setAttribute<int>(names.singleint, 4);
      jet.setAttribute<float>(names.pt, maxPt(particles.c));
    } else if (particles.tau.size()) {
      jet.setAttribute<int>(names.singleint, 15);
      jet.setAttribute<float>(names.pt, maxPt(particles.tau));
    } else {
      jet.setAttribute<int>(names.singleint, 0);
      jet.setAttribute<float>(names.pt, NAN);
    }

    if (particles.b.size()) {
      if (particles.b.size() >= 2)
        jet.setAttribute<int>(names.doubleint, 55);

      else if (particles.c.size())
        jet.setAttribute<int>(names.doubleint, 54);

      else
        jet.setAttribute<int>(names.doubleint, 5);

    } else if (particles.c.size()) {
      if (particles.c.size() >= 2)
        jet.setAttribute<int>(names.doubleint, 44);

      else
        jet.setAttribute<int>(names.doubleint, 4);

    } else if (particles.tau.size())
      jet.setAttribute<int>(names.doubleint, 15);

    else
      jet.setAttribute<int>(names.doubleint, 0);

  }

}
