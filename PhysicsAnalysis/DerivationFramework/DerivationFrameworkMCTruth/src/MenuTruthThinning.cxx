/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MenuTruthThinning.cxx
// Translated into xAOD by James Catmore (James.Catmore@cern.ch)
// from orignal D3PD code by Snyder, Marshall, Boonekamp et al:
// TruthD3PDAnalysis/src/TruthParticleFilterTool.cxx
// Provides user with a menu of options for thinning truth particles
// from xAOD. Tool sets up a mask which is then passed to the
// thinning service.
// DOES NOT PRESERVE GRAPH INTEGRITY

#include "DerivationFrameworkMCTruth/MenuTruthThinning.h"
//#include "DerivationFrameworkMCTruth/xPlotterUtils.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
//#include "EventKernel/PdtPdg.h"
#include "AthenaKernel/errorcheck.h"
#include "HepPID/ParticleIDMethods.hh"
#include "GaudiKernel/SystemOfUnits.h"
#include <vector>
#include <string>

using Gaudi::Units::GeV;

namespace {

    const int PARTONPDGMAX = 43;
    const int NPPDGMIN = 1000000;
    const int NPPDGMAX = 8999999;
    const int PHOTOSMIN = 10000;

} // anonymous namespace

// Constructor
DerivationFramework::MenuTruthThinning::MenuTruthThinning(const std::string& t,
                                                          const std::string& n,
                                                          const IInterface* p ) :
AthAlgTool(t,n,p),
m_particlesKey("TruthParticles"),
m_verticesKey("TruthVertices"),
m_eventsKey("TruthEvents"),
m_writeFirstN(-1),
m_totpart(0),
m_removedpart(0),
m_geantOffset(200000),
m_thinningSvc("ThinningSvc",n)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    // The menu
    declareProperty ("ParticlesKey",
                     m_particlesKey = "TruthParticles",
                     "TruthParticle container name");

    declareProperty ("VerticesKey",
                     m_verticesKey = "TruthVertices",
                     "TruthVertex container name");

    declareProperty ("EventsKey",
                     m_eventsKey = "TruthEvents",
                     "TruthEvent container name");

    declareProperty ("WritePartons",
                     m_writePartons = true,
                     "Keep partons?");

    declareProperty ("WriteHadrons",
                     m_writeHadrons = false,
                     "Keep hadrons?");

    declareProperty ("WriteBHadrons",
                     m_writeBHadrons = true,
                     "Keep b-hadrons?");

    declareProperty ("WriteCHadrons",
                     m_writeCHadrons = true,
                     "Keep c-hadrons?");

    declareProperty ("WriteGeant",
                     m_writeGeant = false,
                     "Keep Geant particles?");

    declareProperty ("GeantPhotonPtThresh",
                     m_geantPhotonPtThresh = 0.5*GeV,
                     "Write Geant photons with Pt above this threshold.  "
                     "Set to < 0 to not write any.");

    declareProperty ("WriteTauHad",
                     m_writeTauHad = false,
                     "Keep hadronic taus?");

    declareProperty ("PartonPtThresh",
                     m_partonPtThresh = -1,
                     "Write partons with Pt above this threshold.");

    declareProperty ("WriteBSM",
                     m_writeBSM = false,
                     "Keep BSM particles?");

    declareProperty ("WriteBosons",
                     m_writeBosons = false,
                     "Keep bosons?");

    declareProperty ("PhotonPtThresh",
                     m_photonPtCut = 3000.,
                     "Photon pt cut with WriteBosons");

    declareProperty ("WriteBSMProducts",
                     m_writeBSMProducts = false,
                     "Keep BSM particle decay products?");

    declareProperty ("WriteBosonProducts",
                     m_writeBosonProducts = false,
                     "Keep boson decay products?");

    declareProperty ("WriteTopAndDecays",
                     m_writeTopAndDecays = false,
                     "Keep top partons and immediate decay products?");

    declareProperty("WriteEverything",
                    m_writeEverything = false,
                    "Keep absolutely everything (overrides all other flags)");

    declareProperty("WriteAllLeptons",
                    m_writeAllLeptons = false,
                    "Keep absolutely all leptons");

    declareProperty("WriteLeptonsNotFromHadrons",
                    m_writeLeptonsNotFromHadrons = false,
                    "Keep leptons not from hadron decays");

    declareProperty("WriteAllStable",
                    m_writeAllStable = false,
                    "Keep all stable particles");

    declareProperty("WriteStatus3",
                    m_writeStatus3 = false,
                    "Save all particles with status code 3");

    declareProperty("WriteFirstN",
                    m_writeFirstN = -1,
                    "Keep first N particles in record");

    declareProperty("PreserveDescendants",
                    m_preserveDescendants = false,
                    "Preserve descendants of retained particles");

    declareProperty("PreserveGeneratorDescendants",
                    m_preserveGeneratorDescendants = false,
                    "Preserve descendants of retained particles excluding Geant particles");

    declareProperty("PreserveAncestors",
                    m_preserveAncestors = false,
                    "Preserve ancestors of retained particles");

    declareProperty ("PreserveParentsSiblingsChildren",
                     m_preserveImmediate = false,
                     "Preserve the parents, siblings and children of retained particles");

    declareProperty ("PreserveHadronizationVertices",
                     m_preserveHadVtx = false,
                     "Preserve hadronization vertices with parents/children.");

    declareProperty("SimBarcodeOffset",
                    m_geantOffset = 200000,
                    "Barcode offset for simulation particles");

    declareProperty ("WritettHFHadrons",
                     m_writettHFHadrons = false,
                     "Keep tt+HF hadrons?");

    declareProperty ("PDGIDsToKeep",
                     m_pdgIdsToKeep={},
                     "List of PDG IDs to always keep");

}

// Destructor
DerivationFramework::MenuTruthThinning::~MenuTruthThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::MenuTruthThinning::initialize()
{
    if (m_preserveDescendants && m_preserveGeneratorDescendants) {
        ATH_MSG_FATAL("You are asking to keep both all descendants, and only those from the event generator. Please check your job options.");
        return StatusCode::FAILURE;
    }
    if (m_preserveImmediate && (m_preserveDescendants || m_preserveGeneratorDescendants || m_preserveAncestors)) {
        ATH_MSG_FATAL("You are asking to preserve only parents/children/siblings of retained states, and also more distant relatives. Please check your job options.");
        return StatusCode::FAILURE;
    }
    m_totpart = 0;
    m_removedpart = 0;
    m_eventCount = 0;
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MenuTruthThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Total of " << m_totpart << " truth particles");
    ATH_MSG_INFO("Removed "<< m_removedpart << " truth particles");
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::MenuTruthThinning::doThinning() const
{
    // Retrieve the truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles;
    if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticleContainer with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    m_totpart += importedTruthParticles->size();
    const xAOD::TruthVertexContainer* importedTruthVertices;
    if (evtStore()->retrieve(importedTruthVertices,m_verticesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthVertexContainer with name " << m_verticesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    const xAOD::TruthEventContainer* importedTruthEvents;
    if (evtStore()->retrieve(importedTruthEvents,m_eventsKey).isFailure()) {
        ATH_MSG_ERROR("No TruthEventContainer with name " << m_eventsKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    // Print events
    //if( m_eventCount < 20 ){
    //    printxAODTruth(m_eventCount, importedTruthParticles);
    //}
    ++m_eventCount;

    // Set up a mask with the same number of entries as the full TruthParticle collection
    std::vector<bool> particleMask, vertexMask;
    int nTruthParticles = importedTruthParticles->size();
    int nTruthVertices = importedTruthVertices->size();
    particleMask.assign(nTruthParticles,true); // default: keep all particles
    vertexMask.assign(nTruthVertices,false); // throw all vertices: to be discussed

    // Locate the signal process vertices and save them
    // Do we need to do this for pile-up as well??
    std::vector<const xAOD::TruthVertex*> signalProcessVertices;
    for (const xAOD::TruthEvent* evt : *importedTruthEvents) {
        const xAOD::TruthVertex* vtx = evt->signalProcessVertex();
        signalProcessVertices.push_back(vtx);
    }
    for (int vertexCounter = 0; vertexCounter < nTruthVertices; ++vertexCounter) {
        for (const xAOD::TruthVertex* spVertex : signalProcessVertices) {
            if ( (*importedTruthVertices)[vertexCounter] == spVertex) vertexMask[vertexCounter]=true;
        }
    }

    // Standard particle loop
    for (int particleCounter = 0; particleCounter < nTruthParticles; ++particleCounter) {
        const xAOD::TruthParticle* particle = (*importedTruthParticles)[particleCounter];
        // Keep first N particles
        if ( (particleCounter > m_writeFirstN) && (!isAccepted(particle)) ){
            // Particle removal - note mask is true by default
            particleMask[particleCounter] = false;
            ++m_removedpart;
        }
    }

    // If user requested preservation of descendants/ancestors:
    // - loop over the masks and work out which particles need to be descended/ascended from
    // - do the recursive loop
    // - update the masks including the descendants/ancestors
    // To ensure graph completeness, this  over-rides anything set by the special treatment
    // of taus in the section above
    DerivationFramework::DecayGraphHelper decayHelper(m_geantOffset);
    std::unordered_set<int> encounteredBarcodes; // For loop handling
    if (m_preserveDescendants || m_preserveGeneratorDescendants || m_preserveAncestors) {
        for (int i=0; i<nTruthParticles; ++i) {
            bool toKeep = particleMask[i];
            if (!toKeep) continue;
            const xAOD::TruthParticle* particle = (*importedTruthParticles)[i];
            encounteredBarcodes.clear();
            if (m_preserveDescendants) decayHelper.descendants(particle,particleMask,vertexMask,encounteredBarcodes,true);
            encounteredBarcodes.clear();
            if (m_preserveGeneratorDescendants) decayHelper.descendants(particle,particleMask,vertexMask,encounteredBarcodes,false);
            encounteredBarcodes.clear();
            if (m_preserveAncestors) decayHelper.ancestors(particle,particleMask,vertexMask,encounteredBarcodes);
            encounteredBarcodes.clear();
        }
    }
    // User only wants to keep parents, siblings and children of retained states
    // Much simpler case - no recursion so no need for barcodes etc
    if (m_preserveImmediate) {
        // Make a copy of the particle mask to avoid changes being propagated
        // down the chain
        std::vector<bool> particleMaskCopy = particleMask;
        for (int i=0; i<nTruthParticles; ++i) {
            bool toKeep = particleMask[i]; // still loop over the orginal list, not the copy
            if (!toKeep) continue;
            const xAOD::TruthParticle* particle = (*importedTruthParticles)[i];
            decayHelper.immediateRelatives(particle,particleMaskCopy,vertexMask,
                                           m_preserveHadVtx); // but only update the copy
        }
        particleMask = particleMaskCopy; // Now update the original list in one go
    }

    // Execute the thinning service based on the mask. Finish.
    if (m_thinningSvc->filter(*importedTruthParticles, particleMask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed for truth particles! ");
        return StatusCode::FAILURE;
    }
    if (m_thinningSvc->filter(*importedTruthVertices, vertexMask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed for truth vertices! ");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

// Test to see if we want to keep a particle
bool DerivationFramework::MenuTruthThinning::isAccepted(const xAOD::TruthParticle* p) const
{
    bool ok = false;

    int pdg_id = std::abs(p->pdgId());
    int barcode = p->barcode();

    if (barcode > m_geantOffset && !m_writeGeant && !m_writeEverything && !ok) {
        if (! (pdg_id == 22/*PDG::gamma*/ &&
               m_geantPhotonPtThresh >= 0 &&
               p->pt() > m_geantPhotonPtThresh) )
            return false;
    }

    // Do we want to save everything?
    if (m_writeEverything)
        ok = true;

    // Save status code 3 particles
    if (m_writeStatus3 && p->status()==3)
        ok = true;

    // OK if we select partons and are at beginning of event record
    if( m_writePartons &&
       (pdg_id <= PARTONPDGMAX || (pdg_id >= NPPDGMIN && pdg_id <= NPPDGMAX) ) &&
       (m_partonPtThresh<0 || p->pt()>m_partonPtThresh) )
        ok = true;

    //  OK if we should select hadrons and are in hadron range
    // JRC: cut changed from PHOTOSMIN to m_geantOffset
    if( m_writeHadrons && HepPID::isHadron (pdg_id) && barcode < m_geantOffset )
        ok = true;

    // OK if we should select b hadrons and are in hadron range
    // JRC: cut changed from PHOTOSMIN to m_geantOffset
    if( m_writeBHadrons &&  barcode < m_geantOffset && HepPID::isHadron (pdg_id) && HepPID::hasBottom (pdg_id) )
        ok= true;

    // OK if we should select c hadrons and are in hadron range
    // JRC: cut changed from PHOTOSMIN to m_geantOffset
    if( m_writeCHadrons &&  barcode < m_geantOffset && HepPID::isHadron (pdg_id) && HepPID::hasCharm (pdg_id) )
        ok= true;

    // PHOTOS range: check whether photons come from parton range or
    // hadron range
    int motherPDGID = 999999999;
    if( barcode > PHOTOSMIN && barcode < m_geantOffset &&
       p->hasProdVtx() )
    {
        const xAOD::TruthVertex* vprod = p->prodVtx();
        if (vprod->nIncomingParticles() > 0) {
            const xAOD::TruthParticle* mother = vprod->incomingParticle(0);
            if (mother) motherPDGID = mother->pdgId();
        }
        if( m_writePartons && !HepPID::isHadron( motherPDGID ) )
            ok = true;
        if( m_writeHadrons && HepPID::isHadron( motherPDGID ) )
            ok = true;
    }

    // OK if we should select G4 particles and are in G4 range
    if( m_writeGeant && barcode > m_geantOffset )
        ok = true;

    if(isLeptonFromTau(p))
        ok = true;

    if(isFsrFromLepton(p))
        ok = true;

    // Hadronic tau decays
    if(m_writeTauHad){
        m_barcode_trace.clear();
        if (isFromTau(p))
            ok = true;
    }

    // Bosons
    if(m_writeBosons && isBoson(p))
        ok = true;

    // BSM particles
    if(m_writeBSM && isBSM(p))
        ok = true;

    // tt+HF hadrons
    if (m_writettHFHadrons && isttHFHadron(p))
        ok = true;

    // Top quarks
    if(m_writeTopAndDecays && pdg_id==6)
        ok = true;

    // All leptons
    if(m_writeAllLeptons && (pdg_id>10 && pdg_id<19)) // Include 4th generation...
        ok = true;

    // All stable
    if (m_writeAllStable && p->status()==1 && barcode<m_geantOffset)
        ok = true;

    // All leptons not from hadron decays
    if(!ok && m_writeLeptonsNotFromHadrons && (pdg_id>10 && pdg_id<19) && p->status()==1) {// Include 4th generation...
        ok = !(matchHadronIncTau(p) || matchQuarkIncTau(p) || isOrphanIncTau(p));
    }

    if ((m_writeBSMProducts || m_writeBosonProducts || m_writeTopAndDecays) && p->hasProdVtx()){ // Either way we have to go through parents
        const xAOD::TruthVertex* prodVtx = p->prodVtx();
        unsigned int nIncoming = prodVtx->nIncomingParticles();
        for(unsigned int itr=0; itr<nIncoming; ++itr) {
            const xAOD::TruthParticle* incomingParticle = prodVtx->incomingParticle(itr);
            if (!incomingParticle) continue;
            if ((m_writeBSMProducts && isBSM( (incomingParticle) )) ||
                (m_writeBosonProducts && isBoson( (incomingParticle) )) ||
                (m_writeTopAndDecays && abs(incomingParticle->pdgId())==6) ){
                ok = true;
                break;
            }
        }
    }

    // All explicitly requested PDG IDs
    for (const auto id : m_pdgIdsToKeep){
      if (pdg_id==id) ok=true;
    } // Loop over PDG IDs

    return ok;
}

bool DerivationFramework::MenuTruthThinning::matchHadronIncTau(const xAOD::TruthParticle* part) const {
    //check if part descends from a hadron, possibly passing through a tau decay
    //ported from TopFiducial
    std::vector<int> hadrons = std::vector<int>{111};
    std::vector<int> taus = std::vector<int>{15,-15};
    return matchGenParticle(part, hadrons, taus, true);
}

bool DerivationFramework::MenuTruthThinning::matchQuarkIncTau(const xAOD::TruthParticle* part) const {
    //check if part descends from a hadron, possibly passing through a tau decay
    //ported from TopFiducial
    std::vector<int> quarks = std::vector<int>{1,6};
    std::vector<int> taus = std::vector<int>{15,-15};
    return matchGenParticle(part, quarks, taus, true);
}

bool DerivationFramework::MenuTruthThinning::isOrphanIncTau(const xAOD::TruthParticle* part) const {
    //check if part descends from nothing, possibly passing through a tau decay
    //(indicating a broken truth record)
    //ported from TopFiducial

    int pdgId = part->pdgId();

    if (!(part->hasProdVtx())) return true;

    const xAOD::TruthVertex* prodVtx = part->prodVtx();
    unsigned int nIncoming = prodVtx->nIncomingParticles();
    unsigned int itrParent = 0;

    // Simple loop catch
    if (prodVtx==part->decayVtx()) return false;

    //0 incoming-> orphan
    if(nIncoming == 0) return true;

    unsigned int nParents = 0;
    while(nParents==0 && itrParent<nIncoming) {
        const xAOD::TruthParticle* incomingParticle = prodVtx->incomingParticle(itrParent);

        // If the parent is itself migrate up the generator record
        if(incomingParticle->pdgId() == pdgId) {
            if(!isOrphanIncTau(incomingParticle)) nParents++;
        }
        else if(abs(incomingParticle->pdgId()) == 15) {
            if(!isOrphanIncTau(incomingParticle)) nParents++;
        }
        else {
            nParents++;
        }
        ++itrParent;
    }
    return (nParents==0);
}

bool DerivationFramework::MenuTruthThinning::matchGenParticle(const xAOD::TruthParticle* part, std::vector<int> &targetPdgIds,
                                                              std::vector<int> &intermediatePdgIds, bool targetsAreRange) const {
    //check if part descends from a particle, possibly passing through an intermediate decay
    //ported from TopFiducial

    int pdgId = part->pdgId();

    bool found = false;

    // Iterators for the target pdg or the intermediate pdg ids
    std::vector<int>::const_iterator itrPdgId, itrPdgIdEnd;

    if (!(part->hasProdVtx())) return false;

    // Loop over the parents
    const xAOD::TruthVertex* prodVtx = part->prodVtx();
    unsigned int nIncoming = prodVtx->nIncomingParticles();

    // Simple loop catch
    if (prodVtx==part->decayVtx()) return false;

    unsigned int itrParent = 0;
    while(!found && itrParent<nIncoming) {
        const xAOD::TruthParticle* incomingParticle = prodVtx->incomingParticle(itrParent);

        if(!targetsAreRange) {
            // If the parent is one of the particles in the target pdg list
            itrPdgId = targetPdgIds.begin();
            itrPdgIdEnd = targetPdgIds.end();
            for(;itrPdgId != itrPdgIdEnd; ++itrPdgId) {
                if(incomingParticle->pdgId() == (*itrPdgId)) return true;
            }
        }
        else {
            int absPdgId = abs(incomingParticle->pdgId());

            // If the parent is within the range given in the target pdg list
            if(targetPdgIds.size() == 1) {
                if(absPdgId >= targetPdgIds.at(0)) return true;
            }
            else if(targetPdgIds.size() >= 2) {
                if(absPdgId >= targetPdgIds.at(0) &&
                   absPdgId <= targetPdgIds.at(1)) return true;
            }
        }

        // If the parent is itself migrate up the generator record
        if(incomingParticle->pdgId() == pdgId) {
            found = matchGenParticle(incomingParticle, targetPdgIds, intermediatePdgIds, targetsAreRange);
        }
        else {
            // If the parent is in the intermediatePdgIds list migrate up the generator record
            itrPdgId = intermediatePdgIds.begin();
            itrPdgIdEnd = intermediatePdgIds.end();
            bool foundIntermediate = false;
            while(!foundIntermediate && itrPdgId != itrPdgIdEnd) {
                if(incomingParticle->pdgId() == (*itrPdgId)) foundIntermediate = true;
                else ++itrPdgId;
            }
            if(foundIntermediate) {
                found = matchGenParticle(incomingParticle, targetPdgIds, intermediatePdgIds, targetsAreRange);
            }
        }

        ++itrParent;
    }
    return found;
}

bool DerivationFramework::MenuTruthThinning::isLeptonFromTau(const xAOD::TruthParticle* part) const{

    int pdg = part->pdgId();

    if(abs(pdg) != 11 &&
       abs(pdg) != 12 &&
       abs(pdg) != 13 &&
       abs(pdg) != 14 &&
       abs(pdg) != 15 &&
       abs(pdg) != 16) return false; // all leptons including tau.

    const xAOD::TruthVertex* prod = part->prodVtx();
    if(!prod) return false; // no parent.

    // Simple loop catch
    if (prod==part->decayVtx()) return false;

    // Loop over the parents of this particle.
    unsigned int nIncoming = prod->nIncomingParticles();
    for(unsigned int itr = 0; itr<nIncoming; ++itr){
        int parentId = prod->incomingParticle(itr)->pdgId();
        if(abs(parentId) == 15) {
            ATH_MSG_DEBUG("Particle with pdgId = " << pdg << ", matched to tau");
            return true; // Has tau parent
        }

        if(parentId == pdg) { // Same particle just a different MC status
            // Go up the generator record until a tau is found or not.
            // Note that this requires a connected *lepton* chain, while calling
            //  isFromTau would allow leptons from hadrons from taus
            if(isLeptonFromTau(prod->incomingParticle(itr))) {
                return true;
            }
        }
    }

    return false;
}

bool DerivationFramework::MenuTruthThinning::isFromTau(const xAOD::TruthParticle* part) const {

    int pdg = part->pdgId();

    const xAOD::TruthVertex* prod = part->prodVtx();
    if(!prod) return false; // no parent.

    // Simple loop catch
    if (prod==part->decayVtx()) return false;

    // More complex loop catch
    std::unordered_set<int>::const_iterator foundVtx = m_barcode_trace.find( prod->barcode() );
    //if ( find(m_barcode_trace.begin(),m_barcode_trace.end(),prod->barcode()) != m_barcode_trace.end()){
    if( foundVtx != m_barcode_trace.end() ) {
        ATH_MSG_DEBUG( "Found a loop (a la Sherpa sample).  Backing out." );
        return false;
    }
    m_barcode_trace.insert(prod->barcode());

    // Loop over the parents of this particle.
    unsigned int nIncoming = prod->nIncomingParticles();
    for (unsigned int pitr = 0; pitr<nIncoming; ++pitr){
        const xAOD::TruthParticle* itrParent = prod->incomingParticle(pitr);
        if (!itrParent) continue;
        if (pitr>2) break; // No point in trying - this vertex does not have a quantum meaning...

        int parentId = itrParent->pdgId();
        if(abs(parentId) == 15) {
            // Check if one of the children of this parent was a tau - if it is, then it is
            //   photon radiation, and we already cover that under FSR
            bool has_fsr = false;
            if ( itrParent->hasDecayVtx() ){
                unsigned int nChildren = itrParent->decayVtx()->nOutgoingParticles();
                for (unsigned int citr = 0; citr<nChildren; ++citr) {
                    const xAOD::TruthParticle* itrChild = itrParent->decayVtx()->outgoingParticle(citr);
                    if (!itrChild) continue;
                    if (abs(itrChild->pdgId())==15){
                        has_fsr = true;
                        break;
                    } // Caught FSR check
                } // loop over immediate children
            } // Parent has an end vertex
            if (has_fsr) return false;
            ATH_MSG_DEBUG("Particle with pdgId = " << pdg << ", matched to tau");
            return true; // Has tau parent
        }

        // Go up the generator record until a tau is found or not.
        if(isFromTau(itrParent)) {
            return true;
        }
    }

    return false;
}

bool DerivationFramework::MenuTruthThinning::isBSM(const xAOD::TruthParticle* part) const{

    int pdg = part->pdgId();

    if ( (31<abs(pdg) && abs(pdg)<38) || // BSM Higgs / W' / Z' / etc
        abs(pdg)==39 ||
        abs(pdg)==41 ||
        abs(pdg)==42 ||
        abs(pdg)== 7 || // 4th gen beauty
        abs(pdg)== 8 || // 4th gen top
        (600 < abs(pdg) && abs(pdg) < 607) || // scalar leptoquarks
        (1000000<abs(pdg) && abs(pdg)<1000040) || // left-handed SUSY
        (2000000<abs(pdg) && abs(pdg)<2000040) || // right-handed SUSY
        abs(pdg)==6000005 || // X5/3
        abs(pdg)==6000006 || // T2/3
        abs(pdg)==6000007 || // B-1/3
        abs(pdg)==6000008 || // Y-4/3
        ( (abs(pdg)>=10000100) && (abs(pdg)<=10001000) ) // multi-charged
        )
        return true;

    return false;
}


bool DerivationFramework::MenuTruthThinning::isttHFHadron(const xAOD::TruthParticle* part) const{

    int ttHFClassification=6;

    if (part->isAvailable<int>("TopHadronOriginFlag")){
        ttHFClassification = part->auxdata< int >( "TopHadronOriginFlag" );
    }
    else{
        return false;
    }

    if (ttHFClassification < 6 ) // useful Hadrons
        return true;

    return false;
}

bool DerivationFramework::MenuTruthThinning::isBoson(const xAOD::TruthParticle* part) const{

    int pdg = part->pdgId();

    if(abs(pdg) != 22  &&
       abs(pdg) != 23 &&
       abs(pdg) != 24 &&
       abs(pdg) != 25 ) return false;

    if(abs(pdg)==22 && part->pt()<m_photonPtCut) return false;

    return true;
}

bool DerivationFramework::MenuTruthThinning::isFsrFromLepton(const xAOD::TruthParticle* part) const {
    int pdg = part->pdgId();
    if(abs(pdg) != 22) return false; // photon
    if(part->barcode() >= m_geantOffset) return false; // Geant photon
    const xAOD::TruthVertex* prod = part->prodVtx();
    if(!prod) return false; // no parent.
    // Simple loop check
    if (prod==part->decayVtx()) return false;
    // Loop over the parents of this particle.
    unsigned int nIncoming = prod->nIncomingParticles();
    for(unsigned int pitr=0; pitr<nIncoming; ++pitr){
        const xAOD::TruthParticle* itrParent = prod->incomingParticle(pitr);
        int parentId = itrParent->pdgId();
        if(abs(parentId) == 11 ||
           abs(parentId) == 13 ||
           abs(parentId) == 15) {
            ATH_MSG_DEBUG("Photon with barcode " << part->barcode() << " matched to particle with pdgId = " << parentId );
            return true; // Has lepton parent
        }
        if(parentId == pdg) { // Same particle just a different MC status
            // Go up the generator record until a tau is found or not.
            if(isFsrFromLepton(itrParent)) return true;
        }
    }
    return false;
}

