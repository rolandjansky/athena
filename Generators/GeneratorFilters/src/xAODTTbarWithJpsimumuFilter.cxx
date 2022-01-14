/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "GeneratorFilters/xAODTTbarWithJpsimumuFilter.h"

#include "GaudiKernel/MsgStream.h"

//--------------------------------------------------------------------------
xAODTTbarWithJpsimumuFilter::xAODTTbarWithJpsimumuFilter(const std::string &fname,
                                                         ISvcLocator *pSvcLocator)
    : GenFilter(fname, pSvcLocator)

{
    declareProperty("SelectJpsi", m_selectJpsi = true);
    declareProperty("JpsipTMinCut", m_JpsiPtMinCut = 0.); /// MeV
    declareProperty("JpsietaMaxCut", m_JpsiEtaMaxCut = 5.);
}

//--------------------------------------------------------------------------
xAODTTbarWithJpsimumuFilter::~xAODTTbarWithJpsimumuFilter()
{
    /////
}

//---------------------------------------------------------------------------
StatusCode xAODTTbarWithJpsimumuFilter::filterInitialize()
{
    ATH_MSG_INFO("Initialized");
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode xAODTTbarWithJpsimumuFilter::filterFinalize()
{
    ATH_MSG_INFO(" Events out of " << m_nPass + m_nFail << " passed the filter");
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode xAODTTbarWithJpsimumuFilter::filterEvent()
{
    //---------------------------------------------------------------------------

    bool pass = false;
    bool isjpsi = false;
    // Retrieve full TruthEventContainer container
    const xAOD::TruthEventContainer *xTruthEventContainer = NULL;
    if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name "
                      << "TruthEvents"
                      << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    for (xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr)
    {
        unsigned int nPart = (*itr)->nTruthParticles();
        // Loop over all truth particles in the event
        // ===========================================
        for (unsigned int iPart = 0; iPart < nPart; ++iPart)
        {
            const xAOD::TruthParticle *pitr = (*itr)->truthParticle(iPart);

            if (pitr->barcode() > 200000)
                break;
            int pdgid = abs(pitr->pdgId());
            // don't loose time checking all if one found
            if (pdgid == 443)
            {
                if (!isLeptonDecay(pitr, 13))
                    continue;
            }
            else
            {
                continue;
            }
            if (!passJpsiSelection(pitr))
                continue;
            isjpsi = true;

        } /// loop on particles

    } // loop on events (only one at evgen - no PU)

    if (m_selectJpsi && isjpsi)
        pass = true;

    setFilterPassed(pass);
    return StatusCode::SUCCESS;
}

// ========================================================
bool xAODTTbarWithJpsimumuFilter::isLeptonDecay(const xAOD::TruthParticle *part, int type) const
{
    auto end = part->decayVtx();
    if (!end)
        return true;
    int partbarcode = part->barcode();
    for (size_t thisChild_id = 0; thisChild_id < end->nOutgoingParticles(); thisChild_id++)
    {
        auto p = end->outgoingParticle(thisChild_id);
        if (partbarcode > p->barcode())
            continue;
        if (std::abs(p->pdgId()) != type)
            return false;
    }
    return true;
}

// ========================================================
bool xAODTTbarWithJpsimumuFilter::passJpsiSelection(const xAOD::TruthParticle *part) const
{
    double pt = part->pt();
    double eta = std::abs(part->eta());

    if (pt < m_JpsiPtMinCut)
        return false;
    if (eta > m_JpsiEtaMaxCut)
        return false;

    return true;
}
