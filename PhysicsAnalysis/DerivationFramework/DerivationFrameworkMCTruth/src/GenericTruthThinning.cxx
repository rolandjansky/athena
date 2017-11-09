/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// GenericTruthThinning.cxx
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all truth particles/vertices which do not pass a user-defined cut

#include "DerivationFrameworkMCTruth/GenericTruthThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::GenericTruthThinning::GenericTruthThinning(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_ntotvtx(0),
m_ntotpart(0),
m_npassvtx(0),
m_npasspart(0),
m_particlesKey("TruthParticles"),
m_verticesKey("TruthVertices"),
m_eventsKey("TruthEvents"),
m_partString(""),
//m_vtxString(""),
m_preserveDescendants(false),
m_preserveGeneratorDescendants(false),
m_preserveAncestors(false),
m_tauHandling(true),
m_geantOffset(200000)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("ParticlesKey", m_particlesKey);
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
    if (m_particlesKey=="" || m_verticesKey=="") {
        ATH_MSG_FATAL("No truth vertex/particle collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_particlesKey << " and "<< m_verticesKey << " as the source collections for truth thinning");}
    
    if (m_partString==""/* && m_vtxString==""*/) {
        ATH_MSG_FATAL("No selection string provided either for vertices or particles!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Truth thinning selection strings: " << m_partString /*<< " " << m_vtxString*/);}
   
    if (m_preserveDescendants && m_preserveGeneratorDescendants) {
        ATH_MSG_FATAL("You are asking to keep both all descendants, and only those from the event generator. Please check your job options.");
        return StatusCode::FAILURE;
    }
    // Set up the text-parsing machinery for thinning the truth directly according to user cuts
    if (/*m_vtxString!="" ||*/ m_partString!="") {
            ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
            proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
            proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
            if (m_partString!="") {
                m_partParser = new ExpressionParsing::ExpressionParser(proxyLoaders);
                    m_partParser->loadExpression(m_partString);
            }
            /*if (m_vtxString!="") {
                m_vertParser = new ExpressionParsing::ExpressionParser(proxyLoaders);
                m_vertParser->loadExpression(m_vtxString);
            }*/
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::GenericTruthThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntotvtx <<" truth vertices, "<< m_npassvtx << " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotpart <<" truth particles, "<< m_npasspart << " were retained ");
    if (m_partString!="") {
        delete m_partParser;
        m_partParser = 0;
    }
    /*if (m_vtxString!="") {
        delete m_vertParser;
        m_vertParser = 0;
    }*/
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::GenericTruthThinning::doThinning() const
{
    
    // Retrieve truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles;
    if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    const xAOD::TruthVertexContainer* importedTruthVertices;
    if (evtStore()->retrieve(importedTruthVertices,m_verticesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthVertex collection with name " << m_verticesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
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
            std::vector<int> entries =  m_partParser->evaluateAsVector();
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
    /*if (m_vertParser) {
        std::vector<int> entries =  m_vertParser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nVertices != nEntries ) {
                ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used TruthVertices?");
            return StatusCode::FAILURE;
        } else {
            // set mask
            for (unsigned int i=0; i<nVertices; ++i) if (entries[i]==1) vertMask[i]=true;
        }
    } */  

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
    for (unsigned int i=0; i<nParticles; ++i) {
        if (partMask[i]) ++m_npasspart;
    }
    for (unsigned int i=0; i<nVertices; ++i) {
        if (vertMask[i]) ++m_npassvtx;
    }
    
    // Execute the thinning service based on the mask. Finish.
    if (m_thinningSvc->filter(*importedTruthParticles, partMask, IThinningSvc::Operator::Or).isFailure()) {
               ATH_MSG_FATAL("Application of thinning service failed! ");
               return StatusCode::FAILURE;
    }
    if (m_thinningSvc->filter(*importedTruthVertices, vertMask, IThinningSvc::Operator::Or).isFailure()) {
            ATH_MSG_FATAL("Application of thinning service failed! ");
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}

