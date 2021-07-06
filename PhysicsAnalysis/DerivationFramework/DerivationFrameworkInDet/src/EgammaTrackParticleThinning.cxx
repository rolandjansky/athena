/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EgammaTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)

#include "DerivationFrameworkInDet/EgammaTrackParticleThinning.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::EgammaTrackParticleThinning::EgammaTrackParticleThinning(const std::string& t,
                                                                              const std::string& n,
                                                                              const IInterface* p ) :
base_class(t,n,p)
{}

// Destructor
DerivationFramework::EgammaTrackParticleThinning::~EgammaTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::EgammaTrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK( m_egammaKey.initialize());
    ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_inDetSGKey.key() << "as the source collection for inner detector track particles");

    ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_egammaKey.key() << " will be retained in this format with the rest being thinned away");

    ATH_CHECK( m_gsfSGKey.initialize (m_streamName) );
    ATH_MSG_INFO("GSF track particles associated with objects in " << m_gsfSGKey << " will be retained in this format with the rest being thinned away");
    
    // Set up the text-parsing machinery for selectiong the photon directly according to user cuts
    if (!m_selectionString.empty()) {
       ATH_CHECK(initializeParser(m_selectionString));
    }
    
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EgammaTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, of which "<< m_npass<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotGSF <<" GSF tracks, of which "<< m_nGSFPass << " were retained ");
    ATH_CHECK( finalizeParser() );
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::EgammaTrackParticleThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();

    SG::ThinningHandle<xAOD::TrackParticleContainer> importedTrackParticles
      (m_inDetSGKey, ctx);
    SG::ThinningHandle<xAOD::TrackParticleContainer> importedGSFTrackParticles
      (m_gsfSGKey, ctx);
    
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
    SG::ReadHandle<xAOD::EgammaContainer> importedEgamma(m_egammaKey,ctx);
    if (!importedEgamma.isValid()) {
        ATH_MSG_ERROR("No e-gamma collection with name " << m_egammaKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nEgammas(importedEgamma->size());
    if (nEgammas==0) return StatusCode::SUCCESS;
    std::vector<const xAOD::Egamma*> egToCheck; egToCheck.clear();
    
    // Execute the text parsers if requested
    if (!m_selectionString.empty()) {
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
    const xAOD::ElectronContainer* testElectrons = dynamic_cast<const xAOD::ElectronContainer*>(importedEgamma.cptr());
    const xAOD::PhotonContainer* testPhotons = dynamic_cast<const xAOD::PhotonContainer*>(importedEgamma.cptr());
    bool isElectrons(false), isPhotons(false);
    if (testElectrons) isElectrons = true;
    if (testPhotons) isPhotons = true;

    // Set elements in the mask to true if there is a corresponding ElementLink from a reconstructed object
    if (m_selectionString.empty()) { // check all objects as user didn't provide a selection string
        if (isElectrons) setElectronMasks(mask,gsfMask,importedEgamma.cptr(),importedTrackParticles.cptr(),importedGSFTrackParticles.cptr(),m_bestMatchOnly);
        if (isPhotons) setPhotonMasks(mask,gsfMask,importedEgamma.cptr(),importedTrackParticles.cptr(),importedGSFTrackParticles.cptr(),m_bestMatchOnly);
    } else { // check only photons passing user selection string
        if (isElectrons) setElectronMasks(mask,gsfMask,egToCheck,importedTrackParticles.cptr(),importedGSFTrackParticles.cptr(),m_bestMatchOnly);
        if (isPhotons) setPhotonMasks(mask,gsfMask,egToCheck,importedTrackParticles.cptr(),importedGSFTrackParticles.cptr(),m_bestMatchOnly);
    }

    // Count up the mask contents
    {
       unsigned int n_pass=0;
       for (unsigned int i=0; i<nTracks; ++i) {
          if (mask[i]) ++n_pass;
       }
    m_npass += n_pass;
    }
    {
       unsigned int n_gsf_pass=0;
       for (unsigned int i=0; i<nGSF; ++i) {
          if (gsfMask[i]) ++n_gsf_pass;
       }
       m_nGSFPass += n_gsf_pass;
    }
    
    
    // Execute the thinning service based on the mask. Finish.
    importedTrackParticles.keep (mask);
    importedGSFTrackParticles.keep (gsfMask);
    
    return StatusCode::SUCCESS;
}

void DerivationFramework::EgammaTrackParticleThinning::setPhotonMasks(std::vector<bool>& mask,
                                                                      std::vector<bool>& gsfMask,
                                                                      const xAOD::EgammaContainer* egammas,
                                                                      const xAOD::TrackParticleContainer* tps,
                                                                      const xAOD::TrackParticleContainer* gsfs,
                                                                      const bool bestMatchOnly) const {
   
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
                                                                      const xAOD::TrackParticleContainer* tps,
                                                                      const xAOD::TrackParticleContainer* gsfs,
                                                                      const bool bestMatchOnly) const {
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
                                                                        const xAOD::EgammaContainer* egammas,
                                                                        const xAOD::TrackParticleContainer* tps,
                                                                        const xAOD::TrackParticleContainer* gsfs,
                                                                        const bool bestMatchOnly) const{
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
                                                                        const xAOD::TrackParticleContainer* tps,
                                                                        const xAOD::TrackParticleContainer* gsfs,
                                                                        const bool bestMatchOnly) const {
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




