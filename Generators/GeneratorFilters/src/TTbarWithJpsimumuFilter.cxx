/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "GeneratorFilters/TTbarWithJpsimumuFilter.h"

#include "GaudiKernel/MsgStream.h"

//--------------------------------------------------------------------------
TTbarWithJpsimumuFilter::TTbarWithJpsimumuFilter(const std::string& fname,
        ISvcLocator* pSvcLocator)
    : GenFilter(fname,pSvcLocator)

{
    declareProperty("SelectJpsi",m_selectJpsi=true);
    declareProperty("JpsipTMinCut",m_JpsiPtMinCut=0.); /// MeV
    declareProperty("JpsietaMaxCut",m_JpsiEtaMaxCut=5.);
}

//--------------------------------------------------------------------------
TTbarWithJpsimumuFilter::~TTbarWithJpsimumuFilter() {
    /////
}

//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterInitialize() {
    ATH_MSG_INFO("Initialized");
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterFinalize() {
    ATH_MSG_INFO(" Events out of " << m_nPass+m_nFail << " passed the filter");
    return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode TTbarWithJpsimumuFilter::filterEvent() {
    //---------------------------------------------------------------------------

    bool pass = false;
    bool isjpsi = false;

    McEventCollection::const_iterator itr;
    for (itr = events_const()->begin(); itr != events_const()->end(); ++itr) {

        const HepMC::GenEvent* genEvt = (*itr);

        // Loop over all truth particles in the event
        // ===========================================
        for(auto part: *genEvt) {
            if(HepMC::barcode(part) > 200000) break;
            if ( !passJpsiSelection(part) ) continue;
            isjpsi=true;

        } /// loop on particles

    } // loop on events (only one at evgen - no PU)

    if (m_selectJpsi && isjpsi) pass = true;
    
    setFilterPassed(pass);
    return StatusCode::SUCCESS;
}

// ========================================================
bool TTbarWithJpsimumuFilter::isLeptonDecay(HepMC::ConstGenParticlePtr part, int type) const {
    auto end = part->end_vertex();
    if(!end) return true;
    int partbarcode = HepMC::barcode(part);
#ifdef HEPMC3
    for (auto p: end->particles_out()) {
        if (partbarcode > HepMC::barcode(p)) continue;
        if (std::abs(p->pdg_id()) !=  type ) return false;
    }
#else
    HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::children);
    for(; firstChild!=endChild; ++firstChild) {
        if( partbarcode > (*firstChild)->barcode() ) continue; /// protection for sherpa
        int childtype = std::abs((*firstChild)->pdg_id());
        if( childtype != type ) {
            return false;
        }
    }
#endif
    return true;
}

// ========================================================
bool TTbarWithJpsimumuFilter::passJpsiSelection(HepMC::ConstGenParticlePtr part) const {

    const HepMC::FourVector& p4 = part->momentum();
    double pt = p4.perp();
    double eta = std::abs(p4.eta());

    if (pt < m_JpsiPtMinCut) return false;
    if (eta > m_JpsiEtaMaxCut) return false;

    return true;

}
