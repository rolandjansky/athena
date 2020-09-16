/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8B_i/CheckCloningFactor.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"

#include <set>

CheckCloningFactor::CheckCloningFactor(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name, pSvcLocator)
{
    // Set users' request
    declareProperty("McEventKey", m_key="GEN_EVENT");
    declareProperty("Tolerance", m_tolerance=0.000001);
    m_eventCount = 0;
}

StatusCode CheckCloningFactor::initialize(){
    
    for (int i=0; i<=2; ++i) bqKinematics[i].clear();
    m_eventCount = 0;
    
    return StatusCode::SUCCESS;
    
}


StatusCode CheckCloningFactor::execute() {
    
    ++m_eventCount;
    
    // Read Data from Transient Store
    const McEventCollection* mcCollptr;
    if (evtStore()->retrieve(mcCollptr, m_key).isFailure()) {
        ATH_MSG_ERROR("Could not retrieve McEventCollection");
        return StatusCode::FAILURE;
    }
    
    // Loop over all events in McEventCollection
    McEventCollection::const_iterator itr;
    std::vector<HepMC::ConstGenParticlePtr> bQuarks;
    for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
        // Loop over all particles in the event and find the b-quarks
        const HepMC::GenEvent* genEvt = (*itr);
        for (auto pitr: *genEvt) {
	    int p_id = pitr->pdg_id();
	    int p_stat = pitr->status();
            if ( (std::abs(p_id)==5) && (p_stat == 62 || p_stat == 63) ) {
                bQuarks.push_back(pitr);
            }
        }
    }
    //Unfortunately it is not clear what should happen in the case of absence of b quarks. To prevent bad things we return false.
    if ( bQuarks.size()==0) return StatusCode::FAILURE;
    // Loop over the b-quarks and find the one with higest pt
    std::vector<HepMC::ConstGenParticlePtr>::iterator recordedBQuarkIt;
    double lastPt(-1.0);
    for (auto bItr = bQuarks.begin(); bItr!=bQuarks.end(); ++bItr) {
        double thisPt=(*bItr)->momentum().perp();
        if (thisPt>lastPt) { recordedBQuarkIt=bItr; }
    }
    auto BQMom=(*recordedBQuarkIt)->momentum();
    double b_pt = BQMom.perp();
    double b_rapid = BQMom.pseudoRapidity();
    double b_phi = BQMom.phi();
    
    unsigned int nAccumulatedEvents = bqKinematics[0].size();
    bool isUnique(true);
    for (unsigned int i=0; i<nAccumulatedEvents; ++i) {
        double pt = (bqKinematics[0])[i];
        double rap = (bqKinematics[1])[i];
        double phi = (bqKinematics[2])[i];
        if ((std::abs(pt-b_pt)<m_tolerance) &&
            (std::abs(rap-b_rapid)<m_tolerance) &&
            (std::abs(phi-b_phi)<m_tolerance) ) isUnique=false;
        if (!isUnique) break;
    }
    
    if (isUnique) {
        bqKinematics[0].push_back(b_pt);
        bqKinematics[1].push_back(b_rapid);
        bqKinematics[2].push_back(b_phi);
    }
        
    return StatusCode::SUCCESS;
}


StatusCode CheckCloningFactor::finalize() {

    int uniqueEvents = bqKinematics[0].size();
    ATH_MSG_INFO("Total number of accepted events = " << m_eventCount);
    ATH_MSG_INFO("Total number of unique events = " << uniqueEvents);
    ATH_MSG_INFO("Ratio = " << (double)m_eventCount / (double)uniqueEvents);
    
    return StatusCode::SUCCESS;
}
