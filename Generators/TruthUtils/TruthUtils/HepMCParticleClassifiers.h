// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This file is part of MCUtils -- https://bitbucket.org/andybuckley/mcutils
// Copyright (C) 2013-2014 Andy Buckley <andy.buckley@cern.ch>
//
// Embedding of MCUtils code in other projects is permitted provided this
// notice is retained and the MCUtils namespace and include path are changed.
//
#pragma once

/// @file Functions for filtering and classifying HepMC record contents
/// @author Andy Buckley <andy.buckley@cern.ch>
/// @author Nataliia Kondrashova <Nataliia.Kondrashova@cern.ch>

#include "HepMC/GenEvent.h"
#include "boost/function.hpp" //< Replace with std::function when possible
#include <vector>

#include "TruthUtils/HepMCEventUtils.h"
#include "TruthUtils/PIDUtils.h"
// #include "TruthUtils/Utils.h"

// Macros to map HepMC functions to PID:: functions of the same name
/// @todo Can leave return type out of the macro and put that on each line where it's used?
/// @todo Also provide versions taking GenParticle& args? Maybe for HepMC3.
#define HEPMC_TO_PID_BOOLFN(fname) inline bool fname (const HepMC::GenParticle* p) { return PID:: fname (p->pdg_id()); }
#define HEPMC_TO_PID_INTFN(fname) inline int fname (const HepMC::GenParticle* p) { return PID:: fname (p->pdg_id()); }
#define HEPMC_TO_PID_DBLFN(fname) inline double fname (const HepMC::GenParticle* p) { return PID:: fname (p->pdg_id()); }

namespace MC {


  /// @name GenParticle classifier functions
  //@{

  /// Convenient type name for a generic classifier function / function object
  typedef boost::function<bool(const HepMC::GenParticle*)> PClassifier;


  /// Is this particle species charged?
  HEPMC_TO_PID_BOOLFN(isCharged)

  /// Is this particle species neutral?
  HEPMC_TO_PID_BOOLFN(isNeutral)


  /// Is this a neutrino?
  HEPMC_TO_PID_BOOLFN(isNeutrino)

  /// Determine if the PID is that of a charged lepton
  HEPMC_TO_PID_BOOLFN(isChLepton)

  /// Determine if the PID is that of a photon
  HEPMC_TO_PID_BOOLFN(isPhoton)

  /// Determine if the PID is that of an electron or positron
  HEPMC_TO_PID_BOOLFN(isElectron)

  /// Determine if the PID is that of an muon or antimuon
  HEPMC_TO_PID_BOOLFN(isMuon)

  /// Determine if the PID is that of an tau or antitau
  HEPMC_TO_PID_BOOLFN(isTau)

  /// Determine if the PID is that of a hadron
  HEPMC_TO_PID_BOOLFN(isHadron)

  /// Determine if the PID is that of a meson
  HEPMC_TO_PID_BOOLFN(isMeson)

  /// Determine if the PID is that of a baryon
  HEPMC_TO_PID_BOOLFN(isBaryon)

  /// Determine if the PID is that of a quark
  HEPMC_TO_PID_BOOLFN(isQuark)

  /// Determine if the PID is that of a parton (quark or gluon)
  HEPMC_TO_PID_BOOLFN(isParton)



  /// Determine if the PID is that of a W+
  HEPMC_TO_PID_BOOLFN(isWplus)

  /// Determine if the PID is that of a W-
  HEPMC_TO_PID_BOOLFN(isWminus)

  /// Determine if the PID is that of a W+-
  HEPMC_TO_PID_BOOLFN(isW)

  /// Determine if the PID is that of a Z0
  HEPMC_TO_PID_BOOLFN(isZ)

  /// Determine if the PID is that of an SM/lightest SUSY Higgs
  HEPMC_TO_PID_BOOLFN(isHiggs)

  /// Determine if the PID is that of a t/tbar
  HEPMC_TO_PID_BOOLFN(isTop)


  /// Determine if the particle is a heavy flavour hadron or parton
  HEPMC_TO_PID_BOOLFN(isHeavyFlavour)

  /// Determine if the PID is that of a heavy parton (c,b,t)
  HEPMC_TO_PID_BOOLFN(isHeavyParton)

  /// Determine if the PID is that of a light parton (u,d,s)
  HEPMC_TO_PID_BOOLFN(isLightParton)


  /// Determine if the PID is that of a heavy flavour (b or c) meson
  HEPMC_TO_PID_BOOLFN(isHeavyMeson)

  /// Determine if the PID is that of a heavy flavour (b or c) baryon
  HEPMC_TO_PID_BOOLFN(isHeavyBaryon)

  /// Determine if the PID is that of a heavy flavour (b or c) hadron
  HEPMC_TO_PID_BOOLFN(isHeavyHadron)


  /// Determine if the PID is that of a light flavour (not b or c) meson
  HEPMC_TO_PID_BOOLFN(isLightMeson)

  /// Determine if the PID is that of a light flavour (not b or c) baryon
  HEPMC_TO_PID_BOOLFN(isLightBaryon)

  /// Determine if the PID is that of a light flavour (not b or c) hadron
  HEPMC_TO_PID_BOOLFN(isLightHadron)


  /// Determine if the PID is that of a b-meson.
  HEPMC_TO_PID_BOOLFN(isBottomMeson)

  /// Determine if the PID is that of a b-baryon.
  HEPMC_TO_PID_BOOLFN(isBottomBaryon)

  /// Determine if the PID is that of a b-hadron.
  HEPMC_TO_PID_BOOLFN(isBottomHadron)


  /// @brief Determine if the PID is that of a c-meson.
  ///
  /// Specifically, the _heaviest_ quark is a c: a B_c is a b-meson and NOT a c-meson.
  /// Charmonia (closed charm) are counted as c-mesons here.
  HEPMC_TO_PID_BOOLFN(isCharmMeson)

  /// @brief Determine if the PID is that of a c-baryon.
  ///
  /// Specifically, the _heaviest_ quark is a c: a baryon containing a b & c
  /// is a b-baryon and NOT a c-baryon. To test for the simpler case, just use
  /// a combination of hasCharm() and isBaryon().
  HEPMC_TO_PID_BOOLFN(isCharmBaryon)

  /// Determine if the PID is that of a c-hadron.
  HEPMC_TO_PID_BOOLFN(isCharmHadron)


  /// Determine if the PID is that of a strange meson
  HEPMC_TO_PID_BOOLFN(isStrangeMeson)

  /// Determine if the PID is that of a strange baryon
  HEPMC_TO_PID_BOOLFN(isStrangeBaryon)

  /// Determine if the PID is that of a strange hadron
  HEPMC_TO_PID_BOOLFN(isStrangeHadron)



  /// Is this a pomeron, odderon, or generic reggeon?
  HEPMC_TO_PID_BOOLFN(isReggeon)

  /// Determine if the PID is that of a diquark (used in hadronization models)
  HEPMC_TO_PID_BOOLFN(isDiquark)

  /// Determine if the PID is that of a pentaquark (hypothetical hadron)
  HEPMC_TO_PID_BOOLFN(isPentaquark)

  /// Is this a fundamental SUSY particle?
  HEPMC_TO_PID_BOOLFN(isSUSY)

  /// Is this an R-hadron?
  HEPMC_TO_PID_BOOLFN(isRhadron)

  /// Is this a technicolor particle?
  HEPMC_TO_PID_BOOLFN(isTechnicolor)

  /// Is this an excited (composite) quark or lepton?
  HEPMC_TO_PID_BOOLFN(isExcited)

  /// Is this a Kaluza-Klein excitation?
  HEPMC_TO_PID_BOOLFN(isKK)

  /// Is this a graviton?
  HEPMC_TO_PID_BOOLFN(isGraviton)

  /// Is this a BSM particle (including graviton)?
  HEPMC_TO_PID_BOOLFN(isBSM)



  /// Determine if the PID is in the generator-specific range
  HEPMC_TO_PID_BOOLFN(isGenSpecific)

  /// Determine if the PID is that of an EW scale resonance
  HEPMC_TO_PID_BOOLFN(isResonance)

  /// Check the PID for usability in transport codes like Geant4
  HEPMC_TO_PID_BOOLFN(isTransportable)



  /// Does this particle contain an up quark?
  HEPMC_TO_PID_BOOLFN(hasUp)

  /// Does this particle contain a down quark?
  HEPMC_TO_PID_BOOLFN(hasDown)

  /// Does this particle contain a strange quark?
  HEPMC_TO_PID_BOOLFN(hasStrange)

  /// Does this particle contain a charm quark?
  HEPMC_TO_PID_BOOLFN(hasCharm)

  /// Does this particle contain a bottom quark?
  HEPMC_TO_PID_BOOLFN(hasBottom)

  /// Does this particle contain a top quark?
  HEPMC_TO_PID_BOOLFN(hasTop)



  /// jSpin returns 2J+1, where J is the total spin
  HEPMC_TO_PID_INTFN(jSpin)

  /// sSpin returns 2S+1, where S is the spin
  HEPMC_TO_PID_INTFN(sSpin)

  /// lSpin returns 2L+1, where L is the orbital angular momentum
  HEPMC_TO_PID_INTFN(lSpin)


  /// Return 3 times the charge (3 x quark charge is an int)
  HEPMC_TO_PID_INTFN(threeCharge)

  /// Return the charge
  HEPMC_TO_PID_DBLFN(charge)



  /// @brief Is this particle the first of its type in the 'decay' chain?
  ///
  /// i.e. it has no parents with the same PID
  inline bool isFirstReplica(const HepMC::GenParticle* p) {
    if (!p->production_vertex()) return true;
    bool isfirst = true;
    BOOST_FOREACH (const HepMC::GenParticle* m, const_parents(p->production_vertex())) {
      if (m->pdg_id() == p->pdg_id()) {
        isfirst = false;
        break;
      }
    }
    return isfirst;
  }

  /// @brief Is this particle the last of its type in the 'decay' chain?
  ///
  /// i.e. it has no daughters with the same PID
  inline bool isLastReplica(const HepMC::GenParticle* p) {
    if (!p->end_vertex()) return true;
    bool islast = true;
    BOOST_FOREACH (const HepMC::GenParticle* d, const_children(p->end_vertex())) {
      if (d->pdg_id() == p->pdg_id()) {
        islast = false;
        break;
      }
    }
    return islast;
  }


  /// @todo Add isPrompt (with what definition?)


  /// A function object with state of the status code to be tested for
  /// @todo Generalise to allow comparison to multiple statuses
  struct HasStatus {
    HasStatus(int status) : status(status) { }
    bool operator()(const HepMC::GenParticle* p) {
      return p->status() == status;
    }
    int status;
  };

  /// A function object with state of the PID codes to be tested for
  struct HasPID {
    HasPID(long pid) { pids.push_back(pid); }
    HasPID(const std::vector<long>& pids) : pids(pids) { }
    bool operator()(const HepMC::GenParticle* p) {
      return std::find(pids.begin(), pids.end(), p->pdg_id()) != pids.end();
    }
    std::vector<long> pids;
  };

  /// A function object with state of the PID codes to be tested for (including antiparticles)
  struct HasAbsPID {
    HasAbsPID(long pid) { pids.push_back(pid); }
    HasAbsPID(const std::vector<long>& pids) : pids(pids) { }
    bool operator()(const HepMC::GenParticle* p) {
      return (std::find(pids.begin(), pids.end(), p->pdg_id()) != pids.end()) ||
        (std::find(pids.begin(), pids.end(), -p->pdg_id()) != pids.end());
    }
    std::vector<long> pids;
  };


  /// @todo Specify and handle units
  inline bool ptLess(const HepMC::GenParticle* p, double ptmax) {
    return p->momentum().perp() < ptmax;
  }
  /// @todo Specify and handle units
  struct PtLess {
    PtLess(double ptmax) : ptmax(ptmax) { }
    bool operator()(const HepMC::GenParticle* p) {
      return ptLess(p, ptmax);
    }
    double ptmax;
  };


  /// @todo Specify and handle units
  inline bool ptGtr(const HepMC::GenParticle* p, double ptmin) {
    return p->momentum().perp() >= ptmin;
  }
  /// @todo Specify and handle units
  struct PtGtr {
    PtGtr(double ptmin) : ptmin(ptmin) { }
    bool operator()(const HepMC::GenParticle* p) {
      return ptGtr(p, ptmin);
    }
    double ptmin;
  };


  inline bool inEtaRange(const HepMC::GenParticle* p, double etamin, double etamax) {
    return in_range(eta(p->momentum()), etamin, etamax);
  }
  struct InEtaRange {
    InEtaRange(double etamin, double etamax) : etamin(etamin), etamax(etamax) { }
    bool operator()(const HepMC::GenParticle* p) {
      return inEtaRange(p, etamin, etamax);
    }
    double etamin, etamax;
  };
  struct NotInEtaRange {
    NotInEtaRange(double etamin, double etamax) : etamin(etamin), etamax(etamax) { }
    bool operator()(const HepMC::GenParticle* p) {
      return !inEtaRange(p, etamin, etamax);
    }
    double etamin, etamax;
  };


  inline bool inRapRange(const HepMC::GenParticle* p, double ymin, double ymax) {
    return in_range(rap(p->momentum()), ymin, ymax);
  }
  struct InRapRange {
    InRapRange(double ymin, double ymax) : ymin(ymin), ymax(ymax) { }
    bool operator()(const HepMC::GenParticle* p) {
      return inRapRange(p, ymin, ymax);
    }
    double ymin, ymax;
  };
  struct NotInRapRange {
    NotInRapRange(double ymin, double ymax) : ymin(ymin), ymax(ymax) { }
    bool operator()(const HepMC::GenParticle* p) {
      return !inRapRange(p, ymin, ymax);
    }
    double ymin, ymax;
  };

  //@}



  /// @name Non-PID-based particle classifiers
  //@{

  /// Determine whether the particle is stable
  inline bool isStable(const HepMC::GenParticle* p) {
    return p->status() == 1;
  }

  /// Determine whether the particle is physical but decayed
  inline bool isDecayed(const HepMC::GenParticle* p) {
    return p->status() == 2;
  }

  /// @brief Determine whether the particle is outside the physical status code range
  ///
  /// If the answer is "true", use with extreme, generator-specific care!
  inline bool hasNonStandardStatus(const HepMC::GenParticle* p) { return !in_range(p->status(), 1, 5); }

  /// Is this a physically reliable final state particle? (as opposed to a debugging entry)
  inline bool isPhysical(const HepMC::GenParticle* p) {
    return isStable(p) || isDecayed(p);
  }

  /// Is this a physically reliable hadron? (as opposed to a debugging entry)
  inline bool isPhysicalHadron(const HepMC::GenParticle* p) {
    return isHadron(p) && isPhysical(p);
  }
  /// Determine if the particle is a decayed hadron
  inline bool isDecayedHadron(const HepMC::GenParticle* p) {
    return isDecayed(p) && isHadron(p);
  }

  /// Determine if the particle is a decayed tau
  inline bool isDecayedTau(const HepMC::GenParticle* p) {
    return isDecayed(p) && isTau(p);
  }

  //@}



  /// @name Classifiers for particles based on their relationships to others
  //@{

  /// @brief Determine whether the given particle has an ancestor that matches the given classifier
  inline bool fromAncestorWith(const HepMC::GenParticle* p, const PClassifier& c) {
    /// @todo *sigh* Missing const methods in HepMC...
    HepMC::GenVertex* gv = const_cast<HepMC::GenVertex*>(p->production_vertex());
    if (gv == NULL) return false;
    /// @todo HepMC needs const versions of these iterator methods :-/
    for (HepMC::GenVertex::particle_iterator pi = gv->particles_begin(HepMC::ancestors); pi != gv->particles_end(HepMC::ancestors); ++pi) {
      if (c(*pi)) return true;
    }
    return false;
  }

  /// @brief Determine whether the given particle has a descendant that matches the given classifier
  inline bool hasDescendantWith(const HepMC::GenParticle* p, const PClassifier& c) {
    /// @todo *sigh* Missing const methods in HepMC...
    HepMC::GenVertex* gv = const_cast<HepMC::GenVertex*>(p->end_vertex());
    if (gv == NULL) return false;
    /// @todo HepMC needs const versions of these iterator methods :-/
    for (HepMC::GenVertex::particle_iterator pi = gv->particles_begin(HepMC::descendants); pi != gv->particles_end(HepMC::descendants); ++pi) {
      if (c(*pi)) return true;
    }
    return false;
  }


  /// Determine if the particle is a hadronically decayed tau
  inline bool isHadronicTau(const HepMC::GenParticle* p) {
    return isDecayedTau(p) && hasDescendantWith(p, isHadron);
  }
  /// Determine if the particle is a leptonically decayed tau
  inline bool isLeptonicTau(const HepMC::GenParticle* p) {
    return isDecayedTau(p) && !hasDescendantWith(p, isHadron);
  }


  /// @brief Helper function used in detecting hadron/tau decay daughters
  ///
  /// @todo Use C++11 lambda when available
  inline bool _isDecayedHadronOrTau(const HepMC::GenParticle* p) {
    return isDecayed(p) && (isTau(p) || isHadron(p));
  }
  /// @brief Determine whether the given particle is from a hadron or tau decay
  ///
  /// Specifically, walk up the ancestor chain until a status 2 hadron or tau is found, if at all.
  inline bool fromDecay(const HepMC::GenParticle* p) {
    return fromAncestorWith(p, _isDecayedHadronOrTau);
  }


  /// @brief Determine whether the given particle is from a hadron decay
  ///
  /// Specifically, walk up the ancestor chain until a status 2 hadron is found, if at all.
  inline bool fromHadronDecay(const HepMC::GenParticle* p) {
    return fromAncestorWith(p, isDecayedHadron);
  }

  /// @brief Determine whether the given particle is from a tau decay
  ///
  /// Specifically, walk up the ancestor chain until a status 2 tau is found, if at all.
  inline bool fromTauDecay(const HepMC::GenParticle* p) {
    return fromAncestorWith(p, isDecayedTau);
  }

  /// @brief Determine whether the given particle is from a hadronic tau decay
  ///
  /// Specifically, walk up the ancestor chain until a status 2 tau is found, if at all.
  inline bool fromHadronicTauDecay(const HepMC::GenParticle* p) {
    return fromAncestorWith(p, isHadronicTau);
  }


  /// Helper function used in detecting HF hadron/tau decay daughters
  /// @todo Use a C++11 inline lambda when available
  inline bool _isDecayedHFHadronOrTau(const HepMC::GenParticle* p) {
    // if (isDecayed(p) && (abs(p->pdg_id()) == PID::TAU || isHeavyHadron(p)))
    //   std::cout << "HFTau: " << p->barcode() << ", " << p->pdg_id() << p->status() << std::endl;
    return isDecayed(p) && (isTau(p) || isHeavyHadron(p));
  }
  /// @brief Determine whether the given particle is from a HF hadron or tau decay
  ///
  /// Specifically, walk up the ancestor chain until a status 2 c/b hadron or tau is found, if at all.
  inline bool fromTauOrHFDecay(const HepMC::GenParticle* p) {
    return fromAncestorWith(p, _isDecayedHFHadronOrTau);
  }


  /// @todo Add more explanatory variants for e.g. isBremPhoton... this one's not obviously of general usefulness
  ///
  // inline bool isBremsstrahlung(const HepMC::GenParticle* p) {
  //   if (abs(p->pdg_id()) != ELECTRON) return false;
  //   if (!p->end_vertex()) return false;
  //   if (p->end_vertex()->particles_out_size() != 2) return isNotBrem;
  //   // Ensure that the children are a corresponding e+/e- and a photon
  //   int mult = 1;
  //   /// @todo Ripe for clean-up with new accessor functions!
  //   HepMC::GenVertex::particle_iterator child = p->end_vertex()->particles_begin(HepMC::children);
  //   HepMC::GenVertex::particle_iterator childE = p->end_vertex()->particles_end(HepMC::children);
  //   for (; child != childE; ++child) {
  //     /// @todo Make sure that the electron/positron child matches p
  //     if (abs((*child)->pdg_id()) != 11 && (*child)->pdg_id() != PHOTON) return false;
  //     mult *= (*child)->pdg_id();
  //   }
  //   if (abs(mult) != 11*22) return false;
  //   return true;
  // }

  /// @todo Also an isConversion()/isConverted() type of function? Should be used on the photon or electrons?

  //@}


}
