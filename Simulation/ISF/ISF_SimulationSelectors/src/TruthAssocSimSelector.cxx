/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthAssocSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// class include
#include "TruthAssocSimSelector.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/HepMCHelper.h"
#include "ISF_Event/TruthBinding.h"

/** Constructor **/
ISF::TruthAssocSimSelector::TruthAssocSimSelector(const std::string& t, const std::string& n, const IInterface* p)
  : BaseSimulationSelector(t,n,p)
  , m_relativesVec()
  , m_relatives()
  , m_relationProp(0)
  , m_relation(HepMC::parents)
{
  declareProperty( "RelativesPDGCode",
                   m_relativesVec,
                   "Only ISFParticles which have specified 'Relation' to the given PDGCode are selected." );

  // as defined in the HepMC manual HepMC::GenVertex::particle iterator
  declareProperty( "Relation",
                   m_relationProp,
                   "Relation to the given PDG codes: 0=parents, 1=family, 2=ancestors, 3=relatives." );

}

/** Destructor **/
ISF::TruthAssocSimSelector::~TruthAssocSimSelector()
{
}

// Athena algtool's Hooks
StatusCode  ISF::TruthAssocSimSelector::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");

  // convert std::vector to std::set for faster searching
  m_relatives.insert( m_relativesVec.begin(), m_relativesVec.end() );

  // convert the 'Relation' Python property into a HepMC data type
  m_relation = HepMCHelper::convertIteratorRange( m_relationProp);

  ATH_MSG_VERBOSE("Initialize successful");
  return StatusCode::SUCCESS;
}


StatusCode  ISF::TruthAssocSimSelector::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");

  ATH_MSG_VERBOSE("Finalize successful");
  return StatusCode::SUCCESS;
}


bool  ISF::TruthAssocSimSelector::passSelectorCuts(const ISFParticle& particle) const
{
  // get the truth binding (as HepMC)
  const TruthBinding* truth = particle.getTruthBinding();
  if (truth)
    {
      // get GenParticle from truth binding
      const HepMC::GenParticle* genParticle = truth->getTruthParticle();
      if (genParticle)
        {
          // test whether any of the pdg codes is found in the genParticle history
          const HepMC::GenParticle* relative = HepMCHelper::findRealtiveWithPDG( *genParticle, m_relation, m_relatives);
          // in case a relative was found
          if (relative)
            {
              // some output
              ATH_MSG_VERBOSE("Particle (eta=" << particle.momentum().eta() << ", "
                              << " phi=" << particle.momentum().phi() << ","
                              << " pdg=" << particle.pdgCode() << ","
                              << " barcode=" << particle.barcode() << ")"
                              << " passes due relative particle"
                              << " (pdg=" << relative->pdg_id() << ","
                              << " barcode=" << relative->barcode() << ")" );
              // selector cuts passed
              return true;
            } // found relative
        } // genParticle present
    }

  // selector cuts not passed
  return false;
}
