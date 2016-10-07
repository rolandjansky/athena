/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinematicSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "KinematicSimSelector.h"

/** Constructor **/
ISF::KinematicSimSelector::KinematicSimSelector(const std::string& t, const std::string& n, const IInterface* p)
  : ISimulationSelector(t,n,p)
  , KinematicParticleCuts()
{
  declareInterface<ISF::ISimulationSelector>(this);

  declareProperty("MinPosEta",            m_cut_minPosEta  , "Minimum Position Pseudorapidity" );
  declareProperty("MaxPosEta",            m_cut_maxPosEta  , "Maximum Position Pseudorapidity" );
  declareProperty("MinMomEta",            m_cut_minMomEta  , "Minimum Momentum Pseudorapidity" );
  declareProperty("MaxMomEta",            m_cut_maxMomEta  , "Maximum Momentum Pseudorapidity" );
  declareProperty("MinMom",               m_cut_minMom2    , "Minimum Particle Momentum"       );
  declareProperty("MaxMom",               m_cut_maxMom2    , "Maximum Particle Moemntum"       );
  declareProperty("Charge",               m_cut_charge     , "Particle Charge"                 );
  declareProperty("ParticlePDG",          m_cut_pdg        , "Particle PDG Code"               );
}

/** Destructor **/
ISF::KinematicSimSelector::~KinematicSimSelector()
{
}

// Athena algtool's Hooks
StatusCode  ISF::KinematicSimSelector::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");

  // compute and store the square of the momentum cuts (faster comparisons)
  if ( !(m_cut_minMom2<0.)) m_cut_minMom2 *= m_cut_minMom2;
  if ( !(m_cut_maxMom2<0.)) m_cut_maxMom2 *= m_cut_maxMom2;

  return StatusCode::SUCCESS;
}


StatusCode  ISF::KinematicSimSelector::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

bool  ISF::KinematicSimSelector::passSelectorCuts(const ISFParticle& particle) const
{
  return ISF::KinematicParticleCuts::pass(particle);
}
