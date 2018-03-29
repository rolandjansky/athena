/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TCCTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/TCCTrackParticleThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TCCTrackParticleThinning::TCCTrackParticleThinning(const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_ntot(0),
m_npass(0),
m_ntotCC(0),
m_npassCC(0),
m_tccSGKey(""),
m_inDetSGKey("InDetTrackParticles"),
m_calCTCSGKey("CaloCalTopoClusters"),
m_oCalCTCSGKey("LCOriginTopoClusters"),
m_jetSGKey("AntiKt10TrackCaloClusterJets"),
// m_selectionString(""),
m_and(false),
m_thinO(false)
// ,m_parser(0)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService"               , m_thinningSvc);
    declareProperty("TCCKey"                        , m_tccSGKey);
    declareProperty("InDetTrackParticlesKey"        , m_inDetSGKey);
    declareProperty("CaloCalTopoClustersKey"        , m_calCTCSGKey);
    declareProperty("OriginCaloCalTopoClustersKey"  , m_oCalCTCSGKey);
    declareProperty("JetKey"                        , m_jetSGKey);
//     declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd"                      , m_and);
    declareProperty("ThinOriginCorrectedClusters"   , m_thinO);
}

// Destructor
DerivationFramework::TCCTrackParticleThinning::~TCCTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TCCTrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    
    if (m_calCTCSGKey=="") {
        ATH_MSG_FATAL("No topocluster collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_calCTCSGKey << "as the source collection for topoclusters");}
    
    if(m_thinO && m_oCalCTCSGKey=="") {
        ATH_MSG_FATAL("No origin corrected topocluster collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_oCalCTCSGKey << "as the source collection for origin corrected topoclusters");}
    
    if (m_jetSGKey=="") {
        ATH_MSG_FATAL("No jet collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_jetSGKey << "as the source collection for TCCs");}
    
    if (m_tccSGKey=="") {
        ATH_MSG_FATAL("No TCC collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_tccSGKey << " will be retained in this format with the rest being thinned away");}
        
    // Set up the text-parsing machinery for selectiong the TCC directly according to user cuts
//     if (m_selectionString!="") {
// 	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
// 	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
// 	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
// 	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
// 	    m_parser->loadExpression(m_selectionString);
//     }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TCCTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotCC <<" calo clusters, "<< m_npassCC<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotTCC <<" TCCs, "<< m_npassTCC<< " were retained ");
//     if (m_selectionString!="") {
//         delete m_parser;
//         m_parser = 0;
//     }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TCCTrackParticleThinning::doThinning() const
{
    
    // Retrieve main TrackParticle collection
    const xAOD::TrackParticleContainer* importedTrackParticles;
    if (evtStore()->retrieve(importedTrackParticles,m_inDetSGKey).isFailure()) {
        ATH_MSG_ERROR("No TrackParticle collection with name " << m_inDetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // Retrieve main CaloCluster collection
    const xAOD::CaloClusterContainer* importedCaloClusters;
    if (evtStore()->retrieve(importedCaloClusters,m_calCTCSGKey).isFailure()) {
        ATH_MSG_ERROR("No CaloCluster collection with name " << m_calCTCSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // Retrieve origin corrected CaloCluster collection
    const xAOD::CaloClusterContainer* importedOriginCaloClusters;
    if (m_thinO && evtStore()->retrieve(importedOriginCaloClusters,m_oCalCTCSGKey).isFailure()) {
        ATH_MSG_ERROR("No CaloCluster collection with name " << m_oCalCTCSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // Retrieve main jet collection
    const xAOD::JetContainer* importedJets;
    if (evtStore()->retrieve(importedJets,m_jetSGKey).isFailure()) {
        ATH_MSG_ERROR("No jet collection with name " << m_jetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    // Check the event contains calo clusters
    unsigned int nCaloClusters = importedCaloClusters->size();
    if (nCaloClusters==0 && nTracks==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full TrackParticle collection
    std::vector<bool> maskTracks;
    maskTracks.assign(nTracks,false); // default: don't keep any tracks
    m_ntot += nTracks;
    
    // Set up a mask with the same entries as the full CaloCluster collection
    std::vector<bool> maskClusters;
    maskClusters.assign(nCaloClusters,false); // default: don't keep any tracks
    m_ntotCC += nCaloClusters;
    
    // Retrieve containers
    // ... TCCs
    const xAOD::TrackCaloClusterContainer* importedTCCs(0);
    if (evtStore()->retrieve(importedTCCs,m_tccSGKey).isFailure()) {
        ATH_MSG_ERROR("No TCC collection with name " << m_tccSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nTCCs(importedTCCs->size());
    
    // Set up a mask with the same entries as the full CaloCluster collection
    std::vector<bool> maskTCCs;
    maskTCCs.assign(nTCCs,false); // default: don't keep any tracks
    m_ntotTCC += nTCCs;
    
    for(auto jet : *importedJets){
        for( size_t j = 0; j < jet->numConstituents(); ++j ) {
            auto tcc = jet->constituentLinks().at(j);
            int index = tcc.index();
            maskTCCs[index] = true;
            const xAOD::TrackCaloCluster* tccO = dynamic_cast<const xAOD::TrackCaloCluster*>(*tcc);
            if(!tccO) continue;
            if(tccO->taste()!=1){
                    index = tccO->trackParticleLink().index();
                    maskTracks[index] = true;
            }
            if(tccO->taste()!=0){
                for (size_t c = 0; c < tccO->caloClusterLinks().size(); ++c) {
                    index = tccO->caloClusterLinks().at(c).index();
                    maskClusters[index] = true;
                }
            }
        }
    }
    
    // Count up the mask contents
    for (unsigned int i=0; i<nTracks; ++i) {
        if (maskTracks[i]) ++m_npass;
    }
    
    for (unsigned int i=0; i<nCaloClusters; ++i) {
        if (maskClusters[i]) ++m_npassCC;
    }
    
    for (unsigned int i=0; i<nTCCs; ++i) {
        if (maskTCCs[i]) ++m_npassTCC;
    }
    
    // Execute the thinning service based on the mask. Finish.
    if (m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, maskTracks, IThinningSvc::Operator::And).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
        if (m_thinningSvc->filter(*importedCaloClusters, maskClusters, IThinningSvc::Operator::And).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
        if (m_thinO && m_thinningSvc->filter(*importedOriginCaloClusters, maskClusters, IThinningSvc::Operator::And).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
        if (m_thinningSvc->filter(*importedTCCs, maskTCCs, IThinningSvc::Operator::And).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
    }
    if (!m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, maskTracks, IThinningSvc::Operator::Or).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
        if (m_thinningSvc->filter(*importedCaloClusters, maskClusters, IThinningSvc::Operator::Or).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
        if (m_thinO && m_thinningSvc->filter(*importedOriginCaloClusters, maskClusters, IThinningSvc::Operator::Or).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
        if (m_thinningSvc->filter(*importedTCCs, maskTCCs, IThinningSvc::Operator::Or).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;
}

