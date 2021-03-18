/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// GenericTruthThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all truth particles/vertices which do not pass a user-defined cut

#include "DerivationFrameworkMCTruth/GenericTruthThinning.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::GenericTruthThinning::GenericTruthThinning(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
base_class(t,n,p),
m_ntotvtx(0),
m_ntotpart(0),
m_npassvtx(0),
m_npasspart(0),
m_partString(""),
//m_vtxString(""),
m_preserveDescendants(false),
m_preserveGeneratorDescendants(false),
m_preserveAncestors(false),
m_tauHandling(true),
m_geantOffset(200000)
{
    declareProperty("VerticesKey", m_verticesKey);
    declareProperty("EventsKey", m_eventsKey);
    declareProperty("ParticleSelectionString", m_partString);
    //declareProperty("VertexSelectionString", m_vtxString);
    declareProperty("PreserveDescendants", m_preserveDescendants);
    declareProperty("PreserveGeneratorDescendants", m_preserveGeneratorDescendants);
    declareProperty("PreserveAncestors", m_preserveAncestors);
    declareProperty("TauHandling", m_tauHandling);
    declareProperty("SimBarcodeOffset", m_geantOffset); 
}

// Destructor
DerivationFramework::GenericTruthThinning::~GenericTruthThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::GenericTruthThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK( m_particlesKey.initialize (m_streamName) );
    ATH_CHECK( m_verticesKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_particlesKey.key() << " and "<< m_verticesKey.key() << " as the source collections for truth thinning");
    
    if (m_partString==""/* && m_vtxString==""*/) {
        ATH_MSG_FATAL("No selection string provided either for vertices or particles!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Truth thinning selection strings: " << m_partString /*<< " " << m_vtxString*/);}
   
    if (m_preserveDescendants && m_preserveGeneratorDescendants) {
	ATH_MSG_FATAL("You are asking to keep both all descendants, and only those from the event generator. Please check your job options.");
        return StatusCode::FAILURE;
    }
    // Set up the text-parsing machinery for thinning the truth directly according to user cuts
    if (!m_partString.empty()) {
       ATH_CHECK( initializeParser(m_partString) );
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::GenericTruthThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntotvtx <<" truth vertices, "<< m_npassvtx << " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotpart <<" truth particles, "<< m_npasspart << " were retained ");
    ATH_CHECK(finalizeParser());
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::GenericTruthThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    
    // Retrieve truth collections
    SG::ThinningHandle<xAOD::TruthParticleContainer> importedTruthParticles
      (m_particlesKey, ctx);
    SG::ThinningHandle<xAOD::TruthVertexContainer> importedTruthVertices
      (m_verticesKey, ctx);

    const xAOD::TruthEventContainer* importedTruthEvents;
    if (evtStore()->retrieve(importedTruthEvents,m_eventsKey).isFailure()) {
        ATH_MSG_ERROR("No TruthEventContainer with name " << m_eventsKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }


    
    // Set up a mask with the same entries as the full collections
    unsigned int nParticles = importedTruthParticles->size();
    unsigned int nVertices = importedTruthVertices->size();
    std::vector<bool> partMask, vertMask;
    partMask.assign(nParticles,false); // default: don't keep any truth items
    vertMask.assign(nVertices,false);
    m_ntotvtx += nVertices; m_ntotpart += nParticles;
   
    // Locate the signal process vertices and save them
    // Do we need to do this for pile-up as well??
    std::vector<const xAOD::TruthVertex*> signalProcessVertices;
    for (const xAOD::TruthEvent* evt : *importedTruthEvents) {
        const xAOD::TruthVertex* vtx = evt->signalProcessVertex();
        signalProcessVertices.push_back(vtx);
    }
    for (unsigned int vertexCounter = 0; vertexCounter < nVertices; ++vertexCounter) {
        for (const xAOD::TruthVertex* spVertex : signalProcessVertices) {
                if ( (*importedTruthVertices)[vertexCounter] == spVertex) vertMask[vertexCounter]=true;
        }          
    } 
 
    // Execute the text parsers and update the mask
    if (m_partString!="") {
    	std::vector<int> entries =  m_parser->evaluateAsVector();
    	unsigned int nEntries = entries.size();
    	// check the sizes are compatible
    	if (nParticles != nEntries ) {
    	    ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used TruthParticles?");
            return StatusCode::FAILURE;
    	} else {
            // set mask
            for (unsigned int i=0; i<nParticles; ++i) if (entries[i]==1) partMask[i]=true;
    	}
    }

    // Special treatment of taus such that only the last one in the chain is kept 
    // Needs another run over the particle collection
    if (m_tauHandling) {
        DerivationFramework::DecayGraphHelper tauDecayHelper;
        for (unsigned int i=0; i<nParticles; ++i) {
            const xAOD::TruthParticle* particle = (*importedTruthParticles)[i];
            if ( abs(particle->pdgId())==15 ) { // This is a tau
                bool last(true);
                std::vector<int> tauDecayProducts; // all decay products of the tau
                std::unordered_set<int> tauDecayEncounteredBarcodes; // loop checking
                tauDecayHelper.descendants(particle,tauDecayProducts,tauDecayEncounteredBarcodes); // recursive 
                for (unsigned int tauDecIt=0; tauDecIt<tauDecayProducts.size(); ++tauDecIt) {
                    if (abs(tauDecayProducts[tauDecIt])==15) { // any taus in the decay products?
                        last = false;
                        break;
                    }
                }
                if (!last) partMask[i]=false;
            } // end of code for tau
        } // end of loop over particles for tau checking  
    } // end of tau handling option

    // If user requested preservation of descendants/ancestors:
    // - loop over the masks and work out which particles need to be descended/ascended from
    // - do the recursive loop
    // - update the masks including the descendants/ancestors
    // To ensure graph completeness, this  over-rides anything set by the special treatment 
    // of taus in the section above 
    DerivationFramework::DecayGraphHelper decayHelper(m_geantOffset);
    std::unordered_set<int> encounteredBarcodes; // to enable loop handling
    if (m_preserveDescendants || m_preserveGeneratorDescendants  || m_preserveAncestors) {
        for (unsigned int i=0; i<nParticles; ++i) {
            bool toKeep = partMask[i];
            if (!toKeep) continue;
            const xAOD::TruthParticle* particle = (*importedTruthParticles)[i];
            encounteredBarcodes.clear();
            if (m_preserveDescendants) decayHelper.descendants(particle,partMask,vertMask,encounteredBarcodes,true);
            encounteredBarcodes.clear();
            if (m_preserveGeneratorDescendants) decayHelper.descendants(particle,partMask,vertMask,encounteredBarcodes,false);
            encounteredBarcodes.clear();			
	    if (m_preserveAncestors) decayHelper.ancestors(particle,partMask,vertMask,encounteredBarcodes);
	    encounteredBarcodes.clear();	
        }
    }
    //for (unsigned int i=0; i<nVertices; ++i) {
    //    bool toKeep = vertMask[i];
    //    if (!toKeep) continue;
    //    const xAOD::TruthVertex* vertex = (*importedTruthVertices)[i];
    //    decayHelper.descend(vertex,partMask,vertMask);
    //}
   
    // Count the masks
    m_npasspart += std::count (partMask.begin(), partMask.end(), true);
    m_npassvtx  += std::count (vertMask.begin(), vertMask.end(), true);
    
    // Execute the thinning service based on the mask. Finish.
    importedTruthParticles.keep (partMask);
    importedTruthVertices.keep  (vertMask);
    
    return StatusCode::SUCCESS;
}

