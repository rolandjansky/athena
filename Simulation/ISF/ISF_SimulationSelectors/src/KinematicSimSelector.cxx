/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinematicSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "KinematicSimSelector.h"

#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"


/** Constructor **/
ISF::KinematicSimSelector::KinematicSimSelector(const std::string& t, const std::string& n, const IInterface* p)
  : BaseSimulationSelector(t,n,p)
  , KinematicParticleCuts()
  , m_partPropSvc("PartPropSvc", name())
{
  declareProperty("MinPosEta",            m_cut_minPosEta  , "Minimum Position Pseudorapidity" );
  declareProperty("MaxPosEta",            m_cut_maxPosEta  , "Maximum Position Pseudorapidity" );
  declareProperty("MinMomEta",            m_cut_minMomEta  , "Minimum Momentum Pseudorapidity" );
  declareProperty("MaxMomEta",            m_cut_maxMomEta  , "Maximum Momentum Pseudorapidity" );
  declareProperty("MinMom",               m_cut_minMom2    , "Minimum Particle Momentum"       );
  declareProperty("MaxMom",               m_cut_maxMom2    , "Maximum Particle Momentum"       );
  declareProperty("MaxEkin",              m_cut_maxEkin    , "Maximum Particle Kinetic Energy" );
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
  ATH_CHECK(m_partPropSvc.retrieve()); 

   HepPDT::ParticleDataTable* particleDataTable; 
   particleDataTable = (HepPDT::ParticleDataTable*) m_partPropSvc->PDT();

   if(particleDataTable == 0) 
   {
    ATH_MSG_ERROR("PDG table not found");
    return StatusCode::FAILURE;
    }

  const HepPDT::ParticleData* data = particleDataTable->particle(HepPDT::ParticleID(abs(m_cut_pdg))); 

  double mass = 0; 
  if(data) mass = data->mass().value(); 


  // compute and store the square of the momentum cuts (faster comparisons)
  if ( !(m_cut_minMom2<0.)) m_cut_minMom2 *= m_cut_minMom2;
  if ( !(m_cut_maxMom2<0.)) m_cut_maxMom2 *= m_cut_maxMom2;

  // if use kinetic energy 
  if(!(m_cut_maxEkin < 0.)) m_cut_maxMom2 = m_cut_maxEkin * (m_cut_maxEkin + 2 * mass);

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
