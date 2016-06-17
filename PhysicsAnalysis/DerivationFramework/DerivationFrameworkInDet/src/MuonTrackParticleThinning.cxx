/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MuonTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This is a trivial example of an implementation of a thinning tool
// which removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkInDet/MuonTrackParticleThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::MuonTrackParticleThinning::MuonTrackParticleThinning(const std::string& t,
                                                                          const std::string& n,
                                                                          const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_ntot(0),
m_npass(0),
m_muonSGKey(""),
m_inDetSGKey("InDetTrackParticles"),
m_selectionString(""),
m_coneSize(-1.0),
m_and(false),
m_parser(0)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("MuonKey", m_muonSGKey);
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ConeSize", m_coneSize);
    declareProperty("ApplyAnd", m_and);
}

// Destructor
DerivationFramework::MuonTrackParticleThinning::~MuonTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::MuonTrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    if (m_muonSGKey=="") {
        ATH_MSG_FATAL("No muon collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_muonSGKey << " will be retained in this format with the rest being thinned away");}
   
    // Set up the text-parsing machinery for selectiong the muon directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MuonTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    if (m_selectionString!="") {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::MuonTrackParticleThinning::doThinning() const
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
    mask.assign(nTracks,false); // default: don't keep any tracks
    m_ntot += nTracks;
    
    // Retrieve containers
    // ... muons
    const xAOD::MuonContainer* importedMuons(0);
    if (evtStore()->retrieve(importedMuons,m_muonSGKey).isFailure()) {
        ATH_MSG_ERROR("No muon collection with name " << m_muonSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nMuons(importedMuons->size());
    std::vector<const xAOD::Muon*> muToCheck; muToCheck.clear();
    // Execute the text parser if requested
    if (m_selectionString!="") {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nMuons != nEntries ) {
        	ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used muons??");
            return StatusCode::FAILURE;
        } else {
        	// identify which muons to keep for the thinning check
        	for (unsigned int i=0; i<nMuons; ++i) if (entries[i]==1) muToCheck.push_back((*importedMuons)[i]);
        }
    }

    DerivationFramework::TracksInCone tInC;    
    if (m_selectionString=="") { // check all muons as user didn't provide a selection string
	    for (xAOD::MuonContainer::const_iterator muIt=importedMuons->begin(); muIt!=importedMuons->end(); ++muIt) {
	      if ((*muIt)->inDetTrackParticleLink().isValid()) {
		//This line prevents SiliconAssociatedForwardMuon from being used unless we're skimming InDetForwardTrackParticles
		//since their track links point to this container while all others point to InDetTrackParticles
		if ((*muIt)->muonType()==xAOD::Muon::SiliconAssociatedForwardMuon &&  m_inDetSGKey != "InDetForwardTrackParticles")
		  {
		    ATH_MSG_DEBUG("Skipping Forward Muon since we are not skimming InDetForwardParticles");
		  }
		else{
		  ATH_MSG_DEBUG("Simming Muon tracks in " << m_inDetSGKey << " "<< (*muIt)->muonType());
		  int index = (*muIt)->inDetTrackParticleLink().index();
		  mask[index] = true;
		}
	      }
	        if (m_coneSize>0.0) tInC.select(*muIt,m_coneSize,importedTrackParticles,mask); // check tracks in a cone around the muon if req'd
	      }
    } else { // check only muons passing user selection string
        for (std::vector<const xAOD::Muon*>::iterator muIt = muToCheck.begin(); muIt!=muToCheck.end(); ++muIt) {
	    if ((*muIt)->inDetTrackParticleLink().isValid()) {
	      
	      if ((*muIt)->muonType()==xAOD::Muon::SiliconAssociatedForwardMuon &&  m_inDetSGKey != "InDetForwardTrackParticles")
		{
		  ATH_MSG_DEBUG("Skipping Forward Muon since we are not skimming InDetForwardParticles");
		}
	      else{
		ATH_MSG_DEBUG("Simming Muon tracks in " << m_inDetSGKey << " "<< (*muIt)->muonType());
		int index = (*muIt)->inDetTrackParticleLink().index();
		mask[index] = true;
	      }
	    } 	
	    if (m_coneSize>0.0) tInC.select(*muIt,m_coneSize,importedTrackParticles,mask); // check tracks in a cone around the muon if req'd	
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

