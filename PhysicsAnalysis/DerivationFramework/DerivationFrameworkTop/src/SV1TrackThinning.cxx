/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SV1TrackThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkTop/SV1TrackThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBTagging/BTagging.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::SV1TrackThinning::SV1TrackThinning(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p),
    m_thinningSvc("ThinningSvc",n),
    m_ntot(0),
    m_npass(0),
    m_jetSGKey(""),
    m_inDetSGKey("InDetTrackParticles"),
    m_selectionString(""),
    m_and(false),
    m_parser(0)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("JetKey", m_jetSGKey);
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd", m_and);
}

// Destructor
DerivationFramework::SV1TrackThinning::~SV1TrackThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::SV1TrackThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    if (m_jetSGKey=="") {
        ATH_MSG_FATAL("No jet collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_jetSGKey << " will be retained in this format with the rest being thinned away");}

    // Set up the text-parsing machinery for selectiong the jet directly according to user cuts
    if (m_selectionString!="") {
        ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
        proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
        proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
        m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
        m_parser->loadExpression(m_selectionString);
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SV1TrackThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::SV1TrackThinning::doThinning() const
{

    // Retrieve main TrackParticle collection
    const xAOD::TrackParticleContainer* importedTrackParticles;
    if (evtStore()->retrieve(importedTrackParticles,m_inDetSGKey).isFailure()) {
        ATH_MSG_ERROR("No TrackParticle collection with name " << m_inDetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    if (nTracks==0) return StatusCode::SUCCESS;

    // Set up a mask with the same entries as the full TrackParticle collection
    std::vector<bool> mask;
    mask.assign(nTracks, false); // default: don't keep any tracks
    m_ntot += nTracks;

    const xAOD::JetContainer* importedJets(0);
    if (evtStore()->retrieve(importedJets,m_jetSGKey).isFailure()) {
        ATH_MSG_ERROR("No jet collection with name " << m_jetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    unsigned int nJets(importedJets->size());
    std::vector<const xAOD::Jet*> jetToCheck; jetToCheck.clear();

    // Execute the text parser if requested
    if (m_selectionString!="") {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nJets != nEntries ) {
            ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used jets??");
            return StatusCode::FAILURE;
        } else {
            // identify which jets to keep for the thinning check
            for (unsigned int i=0; i<nJets; ++i) if (entries[i]==1) jetToCheck.push_back((*importedJets)[i]);
        }
    }

    // Retrieve containers
    // ... jets
    if (m_selectionString=="") { // check all jets as user didn't provide a selection string
        for (xAOD::JetContainer::const_iterator jetIt=importedJets->begin(); jetIt!=importedJets->end(); ++jetIt) {
            const xAOD::BTagging* btag = (*jetIt)->btagging();
            const std::vector< ElementLink<xAOD::TrackParticleContainer> >& SV1_trackParticleLinks =
                btag->SV1_TrackParticleLinks();


            for (const ElementLink<xAOD::TrackParticleContainer> &tp : SV1_trackParticleLinks) {
                int index = (*tp)->index();
                mask[index] = true;
            }
        }
    } else { // check only jets passing user selection string
        for (std::vector<const xAOD::Jet*>::iterator jetIt = jetToCheck.begin(); jetIt!=jetToCheck.end(); ++jetIt) {
            const xAOD::BTagging* btag = (*jetIt)->btagging();
            const std::vector< ElementLink<xAOD::TrackParticleContainer> >& SV1_trackParticleLinks =
                btag->SV1_TrackParticleLinks();


            for (const ElementLink<xAOD::TrackParticleContainer> &tp : SV1_trackParticleLinks) {
                int index = (*tp)->index();
                mask[index] = true;
            }
        }
    }

    // Count up the mask contents
    for (unsigned int i=0; i<nTracks; ++i) {
        if (mask[i]) ++m_npass;
    }

    // Execute the thinning service based on the mask. Finish.
    if (m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::And).isFailure()) {
            ATH_MSG_ERROR("Application of thinning service failed! ");
            return StatusCode::FAILURE;
        }
    }
    if (!m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::Or).isFailure()) {
            ATH_MSG_ERROR("Application of thinning service failed! ");
            return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;
}

