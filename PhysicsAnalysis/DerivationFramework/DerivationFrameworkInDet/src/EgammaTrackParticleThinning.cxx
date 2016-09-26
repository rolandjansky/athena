/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EgammaTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)

#include "DerivationFrameworkInDet/EgammaTrackParticleThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::EgammaTrackParticleThinning::EgammaTrackParticleThinning(const std::string& t,
                                                                              const std::string& n,
                                                                              const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_ntot(0),
m_ntotGSF(0),
m_npass(0),
m_nGSFPass(0),
m_sgKey(""),
m_inDetSGKey("InDetTrackParticles"),
m_gsfSGKey("GSFTrackParticles"),
m_selectionString(""),
m_bestMatchOnly(true),
m_coneSize(-1.0),
m_and(false),
m_parser(0)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("SGKey", m_sgKey);
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey);
    declareProperty("GSFTrackParticlesKey", m_gsfSGKey);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("BestMatchOnly", m_bestMatchOnly);
    declareProperty("ConeSize", m_coneSize);
    declareProperty("ApplyAnd", m_and); 
}

// Destructor
DerivationFramework::EgammaTrackParticleThinning::~EgammaTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::EgammaTrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    if (m_sgKey=="") {
        ATH_MSG_FATAL("No e-gamma collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_sgKey << " will be retained in this format with the rest being thinned away");}
    if (m_gsfSGKey=="") {
        ATH_MSG_FATAL("No GSF track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("GSF track particles associated with objects in " << m_gsfSGKey << " will be retained in this format with the rest being thinned away");}
    
    // Set up the text-parsing machinery for selectiong the photon directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    if (m_selectionString!="") {
            m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
            m_parser->loadExpression(m_selectionString);
        }
    }
    
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EgammaTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, of which "<< m_npass<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotGSF <<" GSF tracks, of which "<< m_nGSFPass << " were retained ");
    if (m_selectionString!="") {
        delete m_parser;
        m_parser = 0;
    }
    
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::EgammaTrackParticleThinning::doThinning() const
{
    
    // Retrieve main TrackParticle collection
    const xAOD::TrackParticleContainer* importedTrackParticles;
    if (evtStore()->retrieve(importedTrackParticles,m_inDetSGKey).isFailure()) {
        ATH_MSG_ERROR("No TrackParticle collection with name " << m_inDetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    // Retrieve GSF TrackParticle collection if required
    const xAOD::TrackParticleContainer* importedGSFTrackParticles;
    if (evtStore()->retrieve(importedGSFTrackParticles,m_gsfSGKey).isFailure()) {
        ATH_MSG_ERROR("No GSF TrackParticle collection with name " << m_gsfSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    unsigned int nGSF = importedGSFTrackParticles->size();
    if (nTracks==0 || nGSF==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full TrackParticle collection(s)
    std::vector<bool> mask, gsfMask;
    mask.assign(nTracks,false); // default: don't keep any tracks
    gsfMask.assign(nGSF,false);
    m_ntot += nTracks;
    m_ntotGSF += nGSF;
    
    // Retrieve e-gamma container
    const xAOD::EgammaContainer* importedEgamma(0);
    if (evtStore()->retrieve(importedEgamma,m_sgKey).isFailure()) {
        ATH_MSG_ERROR("No e-gamma collection with name " << m_sgKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nEgammas(importedEgamma->size());
    if (nEgammas==0) return StatusCode::SUCCESS;
    std::vector<const xAOD::Egamma*> egToCheck; egToCheck.clear();
    
    // Execute the text parsers if requested
    if (m_selectionString!="") {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nEgammas!= nEntries ) {
        	ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used e-gamma objects??");
            return StatusCode::FAILURE;
        } else {
        	// identify which e-gammas to keep for the thinning check
        	for (unsigned int i=0; i<nEgammas; ++i) if (entries[i]==1) egToCheck.push_back((*importedEgamma)[i]);
        }
    }
    
    // Are we dealing with electrons or photons?
    const xAOD::ElectronContainer* testElectrons = dynamic_cast<const xAOD::ElectronContainer*>(importedEgamma);
    const xAOD::PhotonContainer* testPhotons = dynamic_cast<const xAOD::PhotonContainer*>(importedEgamma);
    bool isElectrons(false), isPhotons(false);
    if (testElectrons) isElectrons = true;
    if (testPhotons) isPhotons = true;

    // Set elements in the mask to true if there is a corresponding ElementLink from a reconstructed object
    if (m_selectionString=="") { // check all objects as user didn't provide a selection string
        if (isElectrons) setElectronMasks(mask,gsfMask,importedEgamma,importedTrackParticles,importedGSFTrackParticles,m_bestMatchOnly);
        if (isPhotons) setPhotonMasks(mask,gsfMask,importedEgamma,importedTrackParticles,importedGSFTrackParticles,m_bestMatchOnly);
    } else { // check only photons passing user selection string
        if (isElectrons) setElectronMasks(mask,gsfMask,egToCheck,importedTrackParticles,importedGSFTrackParticles,m_bestMatchOnly);
        if (isPhotons) setPhotonMasks(mask,gsfMask,egToCheck,importedTrackParticles,importedGSFTrackParticles,m_bestMatchOnly);
    }
    
    // Count up the mask contents
    for (unsigned int i=0; i<nTracks; ++i) {
        if (mask[i]) ++m_npass;
    }
    for (unsigned int i=0; i<nGSF; ++i) {
        if (gsfMask[i]) ++m_nGSFPass;
    }
    
    
    // Execute the thinning service based on the mask. Finish.
    if (m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::And).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
	if (m_thinningSvc->filter(*importedGSFTrackParticles, gsfMask, IThinningSvc::Operator::And).isFailure()) {
        	ATH_MSG_ERROR("Application of thinning service failed! ");
        	return StatusCode::FAILURE;
    	}
    }
    if (!m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::Or).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
        if (m_thinningSvc->filter(*importedGSFTrackParticles, gsfMask, IThinningSvc::Operator::Or).isFailure()) {
        	ATH_MSG_ERROR("Application of thinning service failed! ");
        	return StatusCode::FAILURE;
    	}
    }
    
    return StatusCode::SUCCESS;
}

void DerivationFramework::EgammaTrackParticleThinning::setPhotonMasks(std::vector<bool>& mask,
                                                                      std::vector<bool>& gsfMask,
                                                                      const xAOD::EgammaContainer*& egammas,
                                                                      const xAOD::TrackParticleContainer*& tps,
                                                                      const xAOD::TrackParticleContainer*& gsfs,
                                                                      const bool& bestMatchOnly) const {
   
    DerivationFramework::TracksInCone trIC;
    for (xAOD::EgammaContainer::const_iterator egIt=egammas->begin(); egIt!=egammas->end(); ++egIt) {
        const xAOD::Photon* photon = dynamic_cast<const xAOD::Photon*>(*egIt);
	if(!photon){
	  ATH_MSG_ERROR("Did not get a photon object in EgammaTrackParticleThinning::setPhotonMasks");
	  return;
	}
        if (m_coneSize>0.0) {
		trIC.select(photon,m_coneSize,tps,mask); // check InDet tracks in a cone around the e-gammas
	}	
        std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks= photon->vertexLinks();
        unsigned int nLinks = vertexLinks.size();
        if (nLinks==0) continue;
        if (bestMatchOnly) nLinks = 1;
        for (unsigned int i=0; i<nLinks; ++i) {
	    if (!(vertexLinks[i])) continue;
            if (!(vertexLinks[i]).isValid() ) continue;
            const xAOD::Vertex* vx = *(vertexLinks[i]);
            if(!vx) continue;
            auto trackParticleLinks = vx->trackParticleLinks();
            for ( auto link : trackParticleLinks){
                if( !link.isValid() ) continue;
                gsfMask[link.index() ] = true;
                const ElementLink< xAOD::TrackParticleContainer > origTrackLink =
                (*gsfs)[link.index()]->auxdata< ElementLink< xAOD::TrackParticleContainer > >("originalTrackParticle");
                int inDetIndex = origTrackLink.index();
                mask[inDetIndex] = true;
            }
        }
    }
    return;
}

void DerivationFramework::EgammaTrackParticleThinning::setPhotonMasks(std::vector<bool>& mask,
                                                                      std::vector<bool>& gsfMask,
                                                                      std::vector<const xAOD::Egamma*>& egammas,
                                                                      const xAOD::TrackParticleContainer*& tps,
                                                                      const xAOD::TrackParticleContainer*& gsfs,
                                                                      const bool& bestMatchOnly) const {
    DerivationFramework::TracksInCone trIC; 
    for (std::vector<const xAOD::Egamma*>::iterator egIt=egammas.begin(); egIt!=egammas.end(); ++egIt) {
        const xAOD::Photon* photon = dynamic_cast<const xAOD::Photon*>(*egIt);
	if(!photon){
	  ATH_MSG_ERROR("Did not get a photon object in EgammaTrackParticleThinning::setPhotonMasks");
	  return;
	}
        if (m_coneSize>0.0){ trIC.select(photon,m_coneSize,tps,mask);} // check InDet tracks in a cone around the e-gammas
        std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks= photon->vertexLinks();
        unsigned int nLinks = vertexLinks.size();
        if (nLinks == 0) {continue;}
        if (bestMatchOnly) nLinks = 1;
        for (unsigned int i=0; i<nLinks; ++i) {
            if( ! (vertexLinks[i]).isValid() ) continue;
            const xAOD::Vertex* vx = *(vertexLinks[i]);
            if(!vx) continue;
            auto trackParticleLinks = vx->trackParticleLinks();
            for ( auto link : trackParticleLinks){
                if( !link.isValid() ) continue;
                gsfMask[link.index() ] = true;
                const ElementLink< xAOD::TrackParticleContainer > origTrackLink =
                (*gsfs)[link.index()]->auxdata< ElementLink< xAOD::TrackParticleContainer > >("originalTrackParticle");
                int inDetIndex = origTrackLink.index();
                mask[inDetIndex] = true;
            }
        }
    }
    return;
}

void DerivationFramework::EgammaTrackParticleThinning::setElectronMasks(std::vector<bool>& mask,
                                                                        std::vector<bool>& gsfMask,
                                                                        const xAOD::EgammaContainer*& egammas,
                                                                        const xAOD::TrackParticleContainer*& tps,
                                                                        const xAOD::TrackParticleContainer*& gsfs,
                                                                        const bool& bestMatchOnly) const{
    DerivationFramework::TracksInCone trIC;
    for (xAOD::EgammaContainer::const_iterator egIt=egammas->begin(); egIt!=egammas->end(); ++egIt) {
        const xAOD::Electron* electron = dynamic_cast<const xAOD::Electron*>(*egIt);
	if(!electron){
	  ATH_MSG_ERROR("Did not get an electron object in EgammaTrackParticleThinning::setElectronMasks");
	  return;
	}
	if (m_coneSize>0.0) trIC.select(electron,m_coneSize,tps,mask); // check InDet tracks in a cone around the e-gammas
        unsigned int nGSFLinks = 1;
        if (!bestMatchOnly) nGSFLinks = electron->nTrackParticles();
        for (unsigned int i=0; i<nGSFLinks; ++i) {
	    if (!(electron->trackParticleLink(i).isValid())) continue;
            int gsfIndex = electron->trackParticleLink(i).index();
            gsfMask[gsfIndex] = true;
            const ElementLink< xAOD::TrackParticleContainer > origTrackLink =
            (*gsfs)[gsfIndex]->auxdata< ElementLink< xAOD::TrackParticleContainer > >("originalTrackParticle");
	    if (origTrackLink.isValid()) {
            	int inDetIndex = origTrackLink.index();
            	mask[inDetIndex] = true;
	    }	
	}
    }
    return;
}

void DerivationFramework::EgammaTrackParticleThinning::setElectronMasks(std::vector<bool>& mask,
                                                                        std::vector<bool>& gsfMask,
                                                                        std::vector<const xAOD::Egamma*>& egammas,
                                                                        const xAOD::TrackParticleContainer*& tps,
                                                                        const xAOD::TrackParticleContainer*& gsfs,
                                                                        const bool& bestMatchOnly) const {
    DerivationFramework::TracksInCone trIC;
    for (std::vector<const xAOD::Egamma*>::iterator egIt=egammas.begin(); egIt!=egammas.end(); ++egIt) {
        unsigned int nGSFLinks = 1;
        const xAOD::Electron* electron = dynamic_cast<const xAOD::Electron*>(*egIt);
	if(!electron){
	  ATH_MSG_ERROR("Did not get an electron object in EgammaTrackParticleThinning::setElectronMasks");
	  return;
	}
	if (m_coneSize>0.0) trIC.select(electron,m_coneSize,tps,mask); // check InDet tracks in a cone around the e-gammas
        if (!bestMatchOnly) nGSFLinks = electron->nTrackParticles();
        for (unsigned int i=0; i<nGSFLinks; ++i) {
	    if (!(electron->trackParticleLink(i).isValid())) continue;
            int gsfIndex = electron->trackParticleLink(i).index();
            gsfMask[gsfIndex] = true;
            const ElementLink< xAOD::TrackParticleContainer > origTrackLink =
            (*gsfs)[gsfIndex]->auxdata< ElementLink< xAOD::TrackParticleContainer > >("originalTrackParticle");
	    if (origTrackLink.isValid()) {
            	int inDetIndex = origTrackLink.index();
            	mask[inDetIndex] = true;
	    }	
	 }
    }
    return;
}




