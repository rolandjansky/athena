/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef FIX_HEPMC
#define FIX_HEPMC

#include "GeneratorModules/GenBase.h"


/// @brief A "fix-up" algorithm to correct weird event records
///
/// FixHepMC is run in evgen production, *after* the chain of input,
/// shower, hadronization, and afterburner generators, but before
/// the TestHepMC check algorithm, any analysis algs, and the POOL
/// event stream output. Its job is to "normalise" strange event
/// features to allow greater consistency of MC event record use in
/// ATLAS.
///
/// Examples of its features include removing dummy "padding" particles
/// from event records, stripping parton and EW boson intermediates
/// from hadron/tau decay chains, and ensuring that every event has at
/// least one entry in its event weight vector.
///
class FixHepMC : public GenBase {
public:

  FixHepMC(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode execute();
  StatusCode finalize();


private:

  /// @name Classifiers for identifying particles to be removed
  //@{
  bool isPID0(HepMC::ConstGenParticlePtr p);
  bool isNonTransportableInDecayChain(HepMC::ConstGenParticlePtr p);
  bool isLoop(HepMC::ConstGenParticlePtr p);
  //@}

  /// @name Config properties
  //@{
  bool m_killLoops;   // Kill loops?
  bool m_killPDG0;    // Kill PDG0 particles?
  bool m_cleanDecays; // Clean decays?
  bool m_loopByBC;    // Detect loops by barcode
  //@}

  /// @name Cleaned-particle counters
  //@{
  long m_loopKilled;
  long m_pdg0Killed;
  long m_decayCleaned;
  long m_totalSeen;
  //@}

};


#endif

#endif
