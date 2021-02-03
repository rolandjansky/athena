/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/MultiParticleFilter.cxx
// Description:
//
//   Allows the user to search for Np particles with multiple statuses passing pT and Eta cuts.
//   The particles are required to be listed in the vectors particlePDG[] and particleStatus[].
//   The filter will search for these particles and if Np particles are found which fulfil these requirements and the kinematic cuts, the filter returns success,
//   e.g., setting Np = 2, particlePDG = [23,24] with particleStatus = [23] and ptMinParticle = 10 in the jobOptions, 
//   will accepts events with at least two W/Z bosons with status=23 and pT > 10 GeV. 
//   
//   This filter was tested and used only for Alpgen/Jimmy/Herwig events.
//   User should check if the filter works for other generators.
//
//
// AuthorList:         
//   Orel Gueta    March 2014

// Updates January 2021, Kate Pachal
// This is being extended to include explicit eta_min, eta_max,
// phi_min and phi_max cuts and is tested in Pythia.

// Header for this module:-

#include "GeneratorFilters/MultiParticleFilter.h"

// Other classes used by this class:-
#include <math.h>
#include <algorithm>

// This gives access to isBSM functionality
#include "MCUtils/PIDUtils.h"

using HepMC::GenParticle;

//--------------------------------------------------------------------------
MultiParticleFilter::MultiParticleFilter(const std::string& name, ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator)
  , m_total(0)
  , m_passed(0)
{
//--------------------------------------------------------------------------    
  
    declareProperty("useIsBSM",m_useIsBSM = false);
    declareProperty("particlePDG",m_particlePDG); 
    declareProperty("particleStatus",m_particleStatus); 
    declareProperty("considerStatus",m_considerStatus = true);
    declareProperty("ptMinParticle",m_ptMinParticle = 0.); 
    declareProperty("ptMaxParticle",m_ptMaxParticle = 1e9);
    declareProperty("etaMinParticle",m_etaMinParticle = -1.0);
    declareProperty("etaMaxParticle",m_etaMaxParticle = 999.0);
    declareProperty("useAbsEta",m_useAbsEta = true);
    declareProperty("phiMinParticle",m_phiMinParticle = -999.0);
    declareProperty("phiMaxParticle",m_phiMaxParticle = 999.0);
    declareProperty("Np",m_Np = 1);

}

//--------------------------------------------------------------------------
 MultiParticleFilter::~MultiParticleFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode MultiParticleFilter::filterInitialize() {
//---------------------------------------------------------------------------
 
    ATH_MSG_INFO("MultiParticleFilter INITIALISING");

    // Particle ID can be either a list of PDG IDs or else
    // just any BSM particle.
    if (m_useIsBSM && m_particlePDG.size()!=0) {
        ATH_MSG_ERROR("Either set useIsBSM or give a list of particle PDG IDs to consider, but not both!");
        return StatusCode::FAILURE;
    } else if ((!m_useIsBSM) && m_particlePDG.size()==0) {
        ATH_MSG_ERROR("You must specify some particles to look for, either via useIsBSM or by explicitly giving a list of PDG IDs!");
        return StatusCode::FAILURE;
    }

    if(m_particleStatus.size()==0 && m_considerStatus==true) {
        ATH_MSG_ERROR("particleStatus[] not set - if this was intentional, please turn considerStatus to false to confirm.");
        return StatusCode::FAILURE;
    }

    for(unsigned int i=0; i<m_particlePDG.size(); i++) 
        ATH_MSG_INFO("particlePDG["<<i<<"] = "<<m_particlePDG.at(i));
    if (m_considerStatus)
        for(unsigned int i=0; i<m_particleStatus.size(); i++) 
            ATH_MSG_INFO("particleStatus["<<i<<"] = " << m_particleStatus.at(i));
    
    ATH_MSG_INFO("ptMinParticle = " << m_ptMinParticle);
    ATH_MSG_INFO("ptMaxParticle = " << m_ptMaxParticle);
    if (m_etaMaxParticle <= m_etaMinParticle) {
        ATH_MSG_ERROR("Maximum eta must be greater than minimum eta!");
        return StatusCode::FAILURE;        
    }
    ATH_MSG_INFO("etaMinParticle = " << m_etaMinParticle);
    ATH_MSG_INFO("etaMaxParticle = " << m_etaMaxParticle);
    if (m_useAbsEta) ATH_MSG_INFO("This eta range WILL be symmetrised to cover both positive and negative detector regions.");
    else ATH_MSG_INFO("This eta range will NOT be symmetrised and will cover only positive or negative detector regions, as specified.");
    ATH_MSG_INFO("phiMinParticle = " << m_phiMinParticle);
    ATH_MSG_INFO("phiMaxParticle = " << m_phiMaxParticle);
    ATH_MSG_INFO("Np = " << m_Np);
    
    return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------------
StatusCode MultiParticleFilter::filterFinalize() {
//---------------------------------------------------------------------------
    ATH_MSG_INFO("Total efficiency: " << 100.*double(m_passed)/double(m_total) << "%");
    return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode MultiParticleFilter::filterEvent() {
//---------------------------------------------------------------------------

    // Loop over all events in McEventCollection

    McEventCollection::const_iterator itr;
    for (itr = events_const()->begin(); itr != events_const()->end(); ++itr) {

        m_total++; // Bookkeeping
        unsigned int Np(0);

        // Loop over all particles in the event
        const HepMC::GenEvent* genEvt = (*itr);
        for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr){
                          
            // Either accept BSM particles, if that flag is set, or else
            // compare this particle to the passed list of PDG IDs.
            bool passedPDG = false;
            if (m_useIsBSM && MCUtils::PID::isBSM(abs((*pitr)->pdg_id()))) passedPDG = true;
            else {
                for(unsigned int i=0; i<m_particlePDG.size(); i++) 
                    if(abs((*pitr)->pdg_id()) == m_particlePDG[i]) passedPDG = true;
            }            

            // Save time - don't bother
            if (!passedPDG) continue;

            // Check status against listed options, if requested
            bool passedStatus = false;
            if (m_considerStatus) {
                for(unsigned int i=0; i<m_particleStatus.size(); i++) {
                    if(abs((*pitr)->status()) == m_particleStatus[i]) passedStatus=true;
                }
            } else passedStatus = true;

            // If status isn't 1, only consider this particle to be of interest
            // if its child is not another particle of the same type
            // (i.e. it didn't just radiate a gluon or something)
            bool notSelfDecay = true;
            if (abs((*pitr)->status())!=1 && (*pitr)->end_vertex()) {
                HepMC::GenVertex::particle_iterator child = (*pitr)->end_vertex()->particles_begin(HepMC::children);
                HepMC::GenVertex::particle_iterator childE = (*pitr)->end_vertex()->particles_end(HepMC::children);
                for (; child != childE; ++child) {
                    if ( (*child)->pdg_id() == (*pitr)->pdg_id() && (*child)->barcode()!=(*pitr)->barcode() && (*child)->barcode() < 100000) {
                    notSelfDecay = false;
                    break;
                    }
                }
            }

            // This particle is one we're interested in - check pT, eta, and phi requirements.
            // Only consider it a success if it passes ALL of them.
            if(passedPDG && passedStatus && notSelfDecay) {

                bool success = true;

                // pT selection
                if ((*pitr)->momentum().perp() < m_ptMinParticle || (*pitr)->momentum().perp() > m_ptMaxParticle) success = false;

                // eta selection
                if (m_useAbsEta) {
                    // Absolute values on the range as well in case the user requested a negative eta range
                    // and also asked it to be symmetrised.
                    if (fabs((*pitr)->momentum().eta()) < std::min(fabs(m_etaMinParticle),fabs(m_etaMaxParticle)) || fabs((*pitr)->momentum().eta()) > std::max(fabs(m_etaMaxParticle),fabs(m_etaMinParticle))) success = false;
                } else {
                  if ((*pitr)->momentum().eta() < m_etaMinParticle || (*pitr)->momentum().eta() > m_etaMaxParticle) success = false;
                }

                // phi selection
                if ((*pitr)->momentum().phi() < m_phiMinParticle || (*pitr)->momentum().phi() > m_phiMaxParticle) success = false;

	            if (success) Np++; // Found a particle passing all the cuts.

            } // End of passedPDG & passedStatus condition

        } // End of loop over particles

        // Test if we found sufficient passing particles and return in that case.
        if (Np >= m_Np){
            setFilterPassed(true);
            m_passed++;
            return StatusCode::SUCCESS;
        }

    }
    
    // if we got here, we failed
    setFilterPassed(false);
    return StatusCode::SUCCESS;

}


