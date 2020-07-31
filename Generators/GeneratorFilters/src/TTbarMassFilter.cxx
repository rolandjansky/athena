/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarMassFilter.h"
#include "AtlasHepMC/SimpleVector.h"
#include <algorithm>

TTbarMassFilter::TTbarMassFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("TopPairMassLowThreshold",  m_massRangeLowThr  = 300000.);
  declareProperty("TopPairMassHighThreshold", m_massRangeHighThr = 500000.);
}


StatusCode TTbarMassFilter::filterEvent() {
  int  top      = 0;
  int  topbar       = 0;
  bool isLastTop    = false;
  bool isFirstTop = false;
  double topPairInvariantMass = 0.;
  std::vector<HepMC::ConstGenParticlePtr> tops;
  std::vector<HepMC::ConstGenVertexPtr>   top_vtxs;

  for (McEventCollection::const_iterator itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for ( auto mcpart: *genEvt) {
      // Reset the flag. it become true if the top with a final ('last') status is found
      isLastTop = false;
      // Reset the flag. becomes true if the top with the initial ('first') status is found
      isFirstTop = false;

      // Work only with tops
      if (std::abs(mcpart->pdg_id()) == 6) {
        // Assume that this is the 'last' top
        isLastTop=true;
        auto decayVtx = mcpart->end_vertex();
        // Unusual case...
        if (!decayVtx) {
          ATH_MSG_WARNING("top particle with a status "<<mcpart->status()<<" has no valid decay vertex. ");
          ATH_MSG_WARNING("It looks like a Pythia history particle if it has a status=3. Skip this particle ");
          continue;
        }

        // Find out whether this is the top particle with final statuscode, so, just before its decay.
        /// @todo How generator-portable is this status code assumption?
        for (auto child_mcpart: decayVtx->particles_out() ) {
          if (std::abs(child_mcpart->pdg_id()) == 6) {
            // This is not a 'last' top: break the loop over the children, and do nothing with this top particle
            isLastTop = false;
            break;
          }
        }

        // Store the 'last' top
        if (isLastTop) {
          ATH_MSG_DEBUG("Top particle with a status " << mcpart->status() << " is found and stored");
          tops.push_back(mcpart);
        }

        // Find and store production vertices for the 'last' tops. This will help to easily couple top and anti-top
        // pairs later if there are 2 pairs of them. It's a very rare case but needs to be properly handled.
        if (isLastTop) {
          // Investigate whether this top particle is the 'first' one.
          isFirstTop = false;
          // Retrieve the production vertex of the current 'last' top particle
          auto prodVtx = mcpart->production_vertex();
          if (!prodVtx) {
            ATH_MSG_WARNING("Top particle with a status " << mcpart->status() << " has no valid production vertex");
            //save null pointer for consistency
            top_vtxs.push_back(nullptr);
          } else {
            // Loop until the 'first' top particle  production vertex is not reached
            while (!isFirstTop && prodVtx) {
              // Loop over the top mother particles
              for (auto  mother_mcpart: prodVtx->particles_in()) {
                // One of the mother particles is still top quark. needed to go up in the hierarchy
                if (mother_mcpart->pdg_id() == 6) {
                  isFirstTop = false;
                  prodVtx = mother_mcpart->production_vertex();
                  if (!prodVtx) {
                    ATH_MSG_WARNING("mother particle is still a top with a status " << mcpart->status() << ", but has no valid production vertex");
                    top_vtxs.push_back(nullptr);
                  }
                  break;
                } else {
                  // The current (in loop over the mother particles) mother particle is not a top.
                  // maybe the found top particle is the initial one? set the flag true. it will be
                  // set false if one of the other mother particles is top quark.
                  isFirstTop = true;
                }
              }
            }

            //Store the production vertex for the given top particle. it's a production vertex
            //  of the initial ('first') top particle for the current final ('last') top.
            if (isFirstTop) {
              top_vtxs.push_back(prodVtx);
            }
          }
        }

        // Investigate how many top and anti-tops are found and make a decision
        // to continue or stop looping over GenParticles in the event
        if (isLastTop) {
          // One more 'last' top particle was stored into the vector --> increment the number of top or anti-top particles.
          if (tops[tops.size()-1]->pdg_id() == 6) top++; else topbar++;

          // One top and one ati-top 'last' particles are found.  Stop looping
          // over the particles
          if (top==1 && topbar==1) break;

          // This should not happen. But depends on a style how a given
          // generator records the particles.  In very rare cases there are more
          // than 1 top-pairs created. If occasionally the same signed tops from
          // the different couples are ordered first in the generator record,
          // then the situation below is possible
          if (top > 1 || topbar > 1) ATH_MSG_INFO("More than one top-pair exist in the event.");

          // Both top-pairs are found. don't expect more than that.  So, it's
          // time to break the loop over particles.  this fairly assumes that
          // there are no more top-pairs
          if (top==2 && topbar==2) break;
        }
      }
    }
#else
    HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin();
    for (; pitr!=genEvt->particles_end(); ++pitr) {
      // Reset the flag. it become true if the top with a final ('last') status is found
      isLastTop = false;
      // Reset the flag. becomes true if the top with the initial ('first') status is found
      isFirstTop = false;
      HepMC::GenParticle* mcpart = (*pitr);

      // Work only with tops
      if (std::abs(mcpart->pdg_id()) == 6) {
        // Assume that this is the 'last' top
        isLastTop=true;
        HepMC::GenVertex* decayVtx = mcpart->end_vertex();
        // Unusual case...
        if (!decayVtx) {
          ATH_MSG_WARNING("top particle with a status "<<mcpart->status()<<" has no valid decay vertex. ");
          ATH_MSG_WARNING("It looks like a Pythia history particle if it has a status=3. Skip this particle ");
          continue;
        }

        // Find out whether this is the top particle with final statuscode, so, just before its decay.
        /// @todo How generator-portable is this status code assumption?

        HepMC::GenVertex::particles_out_const_iterator child_mcpartItr  = decayVtx->particles_out_const_begin();
        HepMC::GenVertex::particles_out_const_iterator child_mcpartItrE = decayVtx->particles_out_const_end();
        for (; child_mcpartItr!=child_mcpartItrE; ++child_mcpartItr) {
          HepMC::GenParticle* child_mcpart = (*child_mcpartItr);
          if (std::abs(child_mcpart->pdg_id()) == 6) {
            // This is not a 'last' top: break the loop over the children, and do nothing with this top particle
            isLastTop = false;
            break;
          }
        }

        // Store the 'last' top
        if (isLastTop) {
          ATH_MSG_DEBUG("Top particle with a status " << mcpart->status() << " is found and stored");
          tops.push_back(mcpart);
        }

        // Find and store production vertices for the 'last' tops. This will help to easily couple top and anti-top
        // pairs later if there are 2 pairs of them. It's a very rare case but needs to be properly handled.
        if (isLastTop) {
          // Investigate whether this top particle is the 'first' one.
          isFirstTop = false;
          // Retrieve the production vertex of the current 'last' top particle
          HepMC::GenVertex* prodVtx = mcpart->production_vertex();
          if (!prodVtx) {
            ATH_MSG_WARNING("Top particle with a status " << mcpart->status() << " has no valid production vertex");
            //save null pointer for consistency
            top_vtxs.push_back(NULL);
          } else {
            // Loop until the 'first' top particle  production vertex is not reached
            while (!isFirstTop && prodVtx) {
              // Loop over the top mother particles
              HepMC::GenVertex::particles_in_const_iterator mother_mcpartItr  = prodVtx->particles_in_const_begin();
              HepMC::GenVertex::particles_in_const_iterator mother_mcpartItrE = prodVtx->particles_in_const_end();
              for (; mother_mcpartItr != mother_mcpartItrE; ++mother_mcpartItr) {
                HepMC::GenParticle* mother_mcpart = (*mother_mcpartItr);
                // One of the mother particles is still top quark. needed to go up in the hierarchy
                if (mother_mcpart->pdg_id() == 6) {
                  isFirstTop = false;
                  prodVtx = mother_mcpart->production_vertex();
                  if (!prodVtx) {
                    ATH_MSG_WARNING("mother particle is still a top with a status " << mcpart->status() << ", but has no valid production vertex");
                    top_vtxs.push_back(NULL);
                  }
                  break;
                } else {
                  // The current (in loop over the mother particles) mother particle is not a top.
                  // maybe the found top particle is the initial one? set the flag true. it will be
                  // set false if one of the other mother particles is top quark.
                  isFirstTop = true;
                }
              }
            }

            //Store the production vertex for the given top particle. it's a production vertex
            //  of the initial ('first') top particle for the current final ('last') top.
            if (isFirstTop) {
              top_vtxs.push_back(prodVtx);
            }
          }
        }

        // Investigate how many top and anti-tops are found and make a decision
        // to continue or stop looping over GenParticles in the event
        if (isLastTop) {
          // One more 'last' top particle was stored into the vector --> increment the number of top or anti-top particles.
          if (tops[tops.size()-1]->pdg_id() == 6) top++; else topbar++;

          // One top and one ati-top 'last' particles are found.  Stop looping
          // over the particles
          if (top==1 && topbar==1) break;

          // This should not happen. But depends on a style how a given
          // generator records the particles.  In very rare cases there are more
          // than 1 top-pairs created. If occasionally the same signed tops from
          // the different couples are ordered first in the generator record,
          // then the situation below is possible
          if (top > 1 || topbar > 1) ATH_MSG_INFO("More than one top-pair exist in the event.");

          // Both top-pairs are found. don't expect more than that.  So, it's
          // time to break the loop over particles.  this fairly assumes that
          // there are no more top-pairs
          if (top==2 && topbar==2) break;
        }

      }
    }
#endif
  }

  // Main case: there is one top-pair in the event
  if (tops.size()==2) {
    HepMC::FourVector topSumMomentum(0.,0.,0.,0.);
    topSumMomentum.set(tops[0]->momentum().px()+tops[1]->momentum().px(),
                       tops[0]->momentum().py()+tops[1]->momentum().py(),
                       tops[0]->momentum().pz()+tops[1]->momentum().pz(),
                       tops[0]->momentum().e() +tops[1]->momentum().e());
    topPairInvariantMass = topSumMomentum.m();
  }

  // Very rare case: there are 2 top-pairs found in the event.
  // One pair comes from the hard scattering and another from additional gluon
  // splitting. Using particle production vertices one needs to correctly couple
  // the tops to anti-tops
  else if (tops.size() == 4 && top_vtxs.size() == 4) {
    HepMC::FourVector topSumMomentum_1(0.,0.,0.,0.);
    HepMC::FourVector topSumMomentum_2(0.,0.,0.,0.);
    double topPairInvMass_1=0.;
    double topPairInvMass_2=0.;

    // There must be only two different vertecies out of the four
    auto prodVtx = top_vtxs[0];
    int top_11 = 0;
    int top_12 = -1;
    int top_21 = -1;
    int top_22 = -1;
    for (size_t i = 1; i < top_vtxs.size(); ++i) {
      if (top_vtxs[i] == prodVtx) top_12=i;
      else {
        if (top_21 < 0) top_21 = i;
        else top_22 = i;
      }
    }

    // Check that the first top particle shares a production vertex with another top
    if (top_12 < 0) {
      ATH_MSG_ERROR("First top particle doesn't share the production vertex to any other top particles. Event failed the filter");
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }

    // Check that the second top-pair relly has the same production vertex
    if (top_vtxs[top_21] != top_vtxs[top_22]) {
      ATH_MSG_ERROR("Production vertex for the second top-pair particles is not the same. Event failed the filter");
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }

    // First couple. check the coupled top quark's signs. They must be opposite.
    if (tops[top_11]->pdg_id() == tops[top_12]->pdg_id()) ATH_MSG_ERROR("first top-pair particles have the same charge");
    else {
      // The right coupling seems to be achieved. Calculate the invariant mass of the couple
      topSumMomentum_1.set(tops[top_11]->momentum().px()+tops[top_12]->momentum().px(),
                           tops[top_11]->momentum().py()+tops[top_12]->momentum().py(),
                           tops[top_11]->momentum().pz()+tops[top_12]->momentum().pz(),
                           tops[top_11]->momentum().e() +tops[top_12]->momentum().e());
      topPairInvMass_1 = topSumMomentum_1.m();
    }

    // Second couple. check the coupled top quark's signs. They must be opposite.
    if (tops[top_21]->pdg_id() == tops[top_21]->pdg_id()) ATH_MSG_ERROR("second top-pair particles have the same charge");
    else {
      // The right coupling seems to be achieved. Calculate the invariant mass of the couple
      topSumMomentum_2.set(tops[top_21]->momentum().px()+tops[top_22]->momentum().px(),
                           tops[top_21]->momentum().py()+tops[top_22]->momentum().py(),
                           tops[top_21]->momentum().pz()+tops[top_22]->momentum().pz(),
                           tops[top_21]->momentum().e() +tops[top_22]->momentum().e());
      topPairInvMass_2 = topSumMomentum_2.m();
    }

    // Choose the top-pair with the largest invariant mass
    topPairInvariantMass = std::max(topPairInvMass_1, topPairInvMass_2);
  }

  // Something wrong with the number of tops in the event or with the stored vertices....
  else ATH_MSG_ERROR("Size of the collected final top particles vector " << tops.size() <<
                     " doesn't match to the size of there production vertices vector size " << top_vtxs.size() <<
                     ". Event fails the filter");

  // Filter is expected to run on the ttbar sample. This means there should be 1
  // or very rarely 2 top-pairs in the truth.  If this is not the case then
  // something wrong must have a place in this event or in the filter and has to
  // be investigated.
  if (tops.size() != 2 && tops.size() != 4) {
    ATH_MSG_WARNING("1 or 2 top-pairs are expected in the event. But there are " << tops.size() << " final-status top particles.");
  }

  // Proper top-pair invariant mass is calculated. Check if it is in the range of the interest and accept the event if so.
  setFilterPassed(topPairInvariantMass > m_massRangeLowThr && topPairInvariantMass <= m_massRangeHighThr);
  ATH_MSG_DEBUG("The top-pair invariant mass is " << topPairInvariantMass << " CLHEP::MeV");
  return StatusCode::SUCCESS;
}
