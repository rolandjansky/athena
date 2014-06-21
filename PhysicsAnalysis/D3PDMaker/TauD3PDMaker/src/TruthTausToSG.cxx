/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TruthTausToSG.cxx
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date December, 2009
 * @brief Create collection of true hadronic taus in StoreGate
 */


#include "TruthTausToSG.h"
#include "McParticleEvent/TruthParticleContainer.h"

TruthTausToSG::TruthTausToSG(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator), m_truth_handler()
{
    declareProperty("truth_container",      m_truth_container = "SpclMC" );
    declareProperty("output_container",     m_output_container = "TrueHadronicTaus" );
}


StatusCode TruthTausToSG::initialize()
{
    m_truth_handler.SetProngCount(true);
    return StatusCode::SUCCESS;
}


StatusCode TruthTausToSG::finalize()
{
    return StatusCode::SUCCESS;
}


StatusCode TruthTausToSG::execute()
{
    ATH_MSG_DEBUG( " -- TruthTausToSG::execute() -- " );

    // retrieve truth particles
    const TruthParticleContainer* cont = 0;
    if( evtStore()->retrieve(cont, m_truth_container).isFailure() || !cont )
    {
        ATH_MSG_FATAL( "Failed to retrieve " << m_truth_container );
        return StatusCode::FAILURE;
    }
   
    // get true tau leptons that decay hadronically
    const TruthParticleContainer* true_had_taus = m_truth_handler.getHadronicTruth(cont);

    if( evtStore()->record(true_had_taus, m_output_container).isFailure() ) return StatusCode::FAILURE; 

    /***********************

    // create new TruthParticleContainer with four-vectors to reflect the sum of the true visible decay products
    TruthParticleContainer* true_vis_had_taus = new TruthParticleContainer(SG::OWN_ELEMENTS);
    TruthParticleContainer::const_iterator it  = true_had_taus->begin();
    TruthParticleContainer::const_iterator end = true_had_taus->end();
    for(; it != end; ++it)
    {
        // Get the visible four-vector from TauTools.
        HepLorentzVector vis_hlv = m_truth_handler.getTauVisibleSumTruth(*it);

        // Convert the HepLorentzVector to a HepMC::FourVector because set4Mom must take that.
        // See: https://savannah.cern.ch/bugs/?65255
        HepMC::FourVector vis_4vec = HepMC::FourVector(vis_hlv);

        // Create a new TruthParticle that will have the visible four-vector but the lineage information of the tau lepton.
        // We want to use the visible four-vector for dR truth matching to reconstructed taus.
        TruthParticle* p = new TruthParticle(**it);
        p->set4Mom(vis_4vec);
        true_vis_had_taus->push_back(p);
    }

    if( evtStore()->record(true_vis_had_taus, m_output_container).isFailure() ) return StatusCode::FAILURE;

    delete true_had_taus;

    ************************/
   
    ATH_MSG_DEBUG("TruthTausToSG::execute() SUCCESS");
    return StatusCode::SUCCESS;
}
