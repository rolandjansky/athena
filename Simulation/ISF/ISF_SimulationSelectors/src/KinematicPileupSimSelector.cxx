/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// KinematicPileupSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "KinematicPileupSimSelector.h"
// HepMC includes
#include "ISF_Event/ISFParticle.h"
// Units includes
#include "GaudiKernel/SystemOfUnits.h"

/** Constructor **/
ISF::KinematicPileupSimSelector::KinematicPileupSimSelector(const std::string& t, const std::string& n, const IInterface* p)
  : ISimulationSelector(t,n,p)
  , KinematicParticleCuts()
{
  declareInterface<ISF::ISimulationSelector>(this);
  declareProperty("MinPosEta",            m_cut_minPosEta  , "Minimum Position Pseudorapidity"    );
  declareProperty("MaxPosEta",            m_cut_maxPosEta  , "Maximum Position Pseudorapidity"    );
  declareProperty("MinMomEta",            m_cut_minMomEta  , "Minimum Momentum Pseudorapidity"    );
  declareProperty("MaxMomEta",            m_cut_maxMomEta  , "Maximum Momentum Pseudorapidity"    );
  declareProperty("MinMom",               m_cut_minMom2    , "Minimum Particle Momentum"          );
  declareProperty("MaxMom",               m_cut_maxMom2    , "Maximum Particle Moemntum"          );
  declareProperty("Charge",               m_cut_charge     , "Particle Charge"                    );
  declareProperty("ParticlePDG",          m_cut_pdg        , "Particle PDG Code"                  );
  declareProperty("PileupBCID",           m_pileupbcid     , "BICDs to be flagged as being pileup");
}

/** Destructor **/
ISF::KinematicPileupSimSelector::~KinematicPileupSimSelector()
{
}

// Athena algtool's Hooks
StatusCode  ISF::KinematicPileupSimSelector::initialize()
{
  ATH_MSG_INFO("Initializing with " << m_pileupbcid.size() <<
               " BCIDs to be accepted");
  for (std::vector<int>::const_iterator itr=m_pileupbcid.begin();
       itr!=m_pileupbcid.end();++itr)
    ATH_MSG_INFO(" - accept BCID " << *itr);

  // compute and store the square of the momentum cuts (faster comparisons)
  if ( !(m_cut_minMom2<0.)) m_cut_minMom2 *= m_cut_minMom2;
  if ( !(m_cut_maxMom2<0.)) m_cut_maxMom2 *= m_cut_maxMom2;

  return StatusCode::SUCCESS;
}

StatusCode  ISF::KinematicPileupSimSelector::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

bool  ISF::KinematicPileupSimSelector::passSelectorCuts(const ISFParticle& particle) const
{
  int bcid = particle.getBCID();

  // test to see if BCID is in list to accept
  bool isPileup = std::find(begin(m_pileupbcid), end(m_pileupbcid), bcid) != end(m_pileupbcid);
  
  if (isPileup)
      return ISF::KinematicParticleCuts::pass(particle);

  return false;
}

