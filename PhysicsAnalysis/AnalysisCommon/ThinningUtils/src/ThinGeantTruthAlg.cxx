///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ThinGeantTruthAlg.cxx
// Author: James Catmore <James.Catmore@cern.ch>
// based on similar code by Karsten Koeneke <karsten.koeneke@cern.ch>
// Uses thinning service to remove unwanted xAOD truth particles that
// can't be dropped earlier in the simulation chain.
// Not intended for use in derivations!
// - Keep all truth particles with barcode <200 000
// - Keep all truth particles associated with reco photons, electrons 
//   and muons, and their ancestors.
// - Drop any vertices that, after the above thinning, have neither 
// incoming nor outgoing particles
// Unlike other algs in this package, no tool is used to select the
// objects for thinning - everything is done in this one class.
// Expression evaluation is also not used.
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ThinGeantTruthAlg.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
// STL includes
#include <algorithm> 

// FrameWork includes
#include "StoreGate/ThinningHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"

//Standard includes
#include <cstdlib>

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinGeantTruthAlg::ThinGeantTruthAlg( const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
::AthAlgorithm( name, pSvcLocator ),
m_doThinning(true),
m_geantOffset(200000),
m_longlived{310,3122,3222,3112,3322,3312},
m_nEventsProcessed{0},
m_nParticlesProcessed{0},
m_nVerticesProcessed{0},
m_nParticlesThinned{0},
m_nVerticesThinned{0}
{
   
    declareProperty("ThinGeantTruth", m_doThinning,
                    "Should the Geant truth thinning be run?");
    declareProperty("GeantBarcodeOffset", m_geantOffset,
                    "Barcode offset for Geant particles");
    declareProperty("LongLivedParticleList", m_longlived,
                    "List of long lifetime particles which are likely to be decayed by Geant but whose children must be kept");

}

// Destructor
///////////////
ThinGeantTruthAlg::~ThinGeantTruthAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinGeantTruthAlg::initialize()
{
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    
    // Print out the used configuration
    ATH_MSG_DEBUG ( " using = " << m_streamName );

    // Is truth thinning required?
    if (!m_doThinning) {
        ATH_MSG_INFO("Geant truth thinning not required");
    } else {
        ATH_MSG_INFO("Geant truth will be thinned");
    }

    if (m_doThinning && m_streamName.empty()) {
      ATH_MSG_ERROR ("StreamName property was not initialized.");
      return StatusCode::FAILURE;
    }

    ATH_CHECK(m_truthParticlesKey.initialize(m_streamName, m_doThinning));
    ATH_CHECK(m_truthVerticesKey.initialize(m_streamName, m_doThinning));
    ATH_CHECK(m_electronsKey.initialize(m_doThinning));
    ATH_CHECK(m_photonsKey.initialize(m_doThinning));
    ATH_CHECK(m_muonsKey.initialize(m_doThinning));
    ATH_CHECK(m_egammaTruthKey.initialize(m_doThinning));
 
    // Initialize the counters to zero
   ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
    return StatusCode::SUCCESS;
}


StatusCode ThinGeantTruthAlg::finalize()
{
    ATH_MSG_DEBUG ("Finalizing " << name() << "...");
    ATH_MSG_INFO("Processed " << m_nEventsProcessed << " events containing " << m_nParticlesProcessed 
                 << " truth particles and " << m_nVerticesProcessed << " truth vertices ");     
    ATH_MSG_INFO("Removed " << m_nParticlesThinned << " Geant truth particles and " 
                 << m_nVerticesThinned << " corresponding truth vertices ");
    return StatusCode::SUCCESS;
}

StatusCode ThinGeantTruthAlg::execute()
{
    // Increase the event counter
    ++m_nEventsProcessed;
    
    // Is truth thinning required?
    if (!m_doThinning) {
        return StatusCode::SUCCESS;
    } 
   
    // Retrieve truth and vertex containers
    SG::ThinningHandle<xAOD::TruthParticleContainer> truthParticles(m_truthParticlesKey);
    SG::ThinningHandle<xAOD::TruthVertexContainer> truthVertices(m_truthVerticesKey);
    if(!truthParticles.isValid()){
        ATH_MSG_FATAL("No TruthParticleContainer with key "+m_truthParticlesKey.key()+" found.");
        return StatusCode::FAILURE;
    }
    if(!truthVertices.isValid()){
        ATH_MSG_FATAL("No TruthVertexContainer with key "+m_truthVerticesKey.key()+" found.");
        return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::MuonContainer> muons(m_muonsKey);
    // Retrieve muons, electrons and photons
    if(!muons.isValid()){
        ATH_MSG_WARNING("No muon container with key "+m_muonsKey.key()+" found.");
    }
    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronsKey);   
    if(!electrons.isValid()){
        ATH_MSG_WARNING("No electron container with key "+m_electronsKey.key()+" found.");
    }
    SG::ReadHandle<xAOD::PhotonContainer> photons(m_photonsKey);   
    if(!photons.isValid()){
        ATH_MSG_WARNING("No photon container with key "+m_photonsKey.key()+" found.");
    }
   
    // Loop over photons, electrons and muons and get the associated truth particles
    // Retain the associated index number
    std::vector<int> recoParticleTruthIndices;
    for (const xAOD::Muon* muon : *muons) {
      const xAOD::TruthParticle* truthMuon = xAOD::TruthHelpers::getTruthParticle(*muon); 
      if (truthMuon) {recoParticleTruthIndices.push_back(truthMuon->index());}
    }
    for (const xAOD::Electron* electron : *electrons) {
      const xAOD::TruthParticle* truthElectron = xAOD::TruthHelpers::getTruthParticle(*electron);
      if (truthElectron) {recoParticleTruthIndices.push_back(truthElectron->index());}
    }
    for (const xAOD::Photon* photon : *photons) {
      const xAOD::TruthParticle* truthPhoton = xAOD::TruthHelpers::getTruthParticle(*photon);
      if (truthPhoton) {recoParticleTruthIndices.push_back(truthPhoton->index());}
    } 

    //Set up the indices for the egamma Truth Particles to keep
    SG::ReadHandle<xAOD::TruthParticleContainer> egammaTruthParticles(m_egammaTruthKey);   
    if(!egammaTruthParticles.isValid()){
      ATH_MSG_WARNING("No e-gamma truth container with key "+m_egammaTruthKey.key()+" found.");
    }

    std::vector<int> egammaTruthIndices{};
    for (const xAOD::TruthParticle* egTruthParticle : *egammaTruthParticles) {

      static const SG::AuxElement::ConstAccessor<int> accType("truthType");

      if(!accType.isAvailable(*egTruthParticle) || 
         accType(*egTruthParticle)!=MCTruthPartClassifier::IsoElectron || 
         std::abs(egTruthParticle->eta()) > 2.525){
        continue;
        }
      //Only central isolated true electrons	
      typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;
      static SG::AuxElement::ConstAccessor<TruthLink_t> linkToTruth("truthParticleLink");
      if (!linkToTruth.isAvailable(*egTruthParticle)) {
        continue;
      }

      const TruthLink_t& truthegamma = linkToTruth(*egTruthParticle);
      if (!truthegamma.isValid()) {
        continue;
      } 
      egammaTruthIndices.push_back( (*truthegamma)->index());
    }
    // Set up masks
    
    std::vector<bool> particleMask, vertexMask;
    int nTruthParticles = truthParticles->size();
    int nTruthVertices = truthVertices->size();
    m_nParticlesProcessed += nTruthParticles;
    m_nVerticesProcessed += nTruthVertices;
    particleMask.assign(nTruthParticles,false);
    vertexMask.assign(nTruthVertices,false);
    
    // Vector of pairs keeping track of how many incoming/outgoing particles each vertex has
    std::vector<std::pair<int, int> > vertexLinksCounts;
    for (auto vertex : *truthVertices) {
        std::pair<int , int> tmpPair;
        tmpPair.first = vertex->nIncomingParticles();
        tmpPair.second = vertex->nOutgoingParticles();
        vertexLinksCounts.push_back(tmpPair);
    }
    
    // Loop over truth particles and update mask
    std::unordered_set<int> encounteredBarcodes; // for loop protection
    for (int i=0; i<nTruthParticles; ++i) {
        encounteredBarcodes.clear();
        const xAOD::TruthParticle* particle = (*truthParticles)[i];
        // Retain status 1 BSM particles and descendants
        if (isStatus1BSMParticle(particle)) {
            descendants(particle,particleMask,encounteredBarcodes);
            encounteredBarcodes.clear();
        }
        // Retain children of longer-lived generator particles
        if (particle->status()==1) { 
            int pdgId = abs(particle->pdgId());
            if ( std::find(m_longlived.begin(), m_longlived.end(), pdgId) != m_longlived.end() ) {
                const xAOD::TruthVertex* decayVtx(0);
                if (particle->hasDecayVtx()) {decayVtx = particle->decayVtx();}
                int nChildren = 0;
                if (decayVtx) nChildren = decayVtx->nOutgoingParticles();
                for (int i=0; i<nChildren; ++i) {
                    particleMask[decayVtx->outgoingParticle(i)->index()] = true;    
                }        
            }
        }  

        // Retain particles and their descendants/ancestors associated with the reconstructed objects
        if ( std::find(recoParticleTruthIndices.begin(), recoParticleTruthIndices.end(), i) != recoParticleTruthIndices.end() ) { 
            if (abs(particle->barcode()) > m_geantOffset) { // only need to do this for Geant particles since non-Geant are kept anyway 
                ancestors(particle,particleMask,encounteredBarcodes);
                encounteredBarcodes.clear();
                descendants(particle,particleMask,encounteredBarcodes);
                encounteredBarcodes.clear();
            }
        }

        // Retain particles and their descendants  associated with the egamma Truth Particles
        if ( std::find(egammaTruthIndices.begin(), egammaTruthIndices.end(), i) != egammaTruthIndices.end() ) { 
          descendants(particle,particleMask,encounteredBarcodes);
          encounteredBarcodes.clear();
        }

        if (abs(particle->barcode()) < m_geantOffset) {
            particleMask[i] = true;
        }         
    }

    // Loop over the mask and update vertex association counters
    for (int i=0; i<nTruthParticles; ++i) {
        if (particleMask[i] == false) {
            ++m_nParticlesThinned;
            const xAOD::TruthParticle* particle = (*truthParticles)[i];
            if (particle->hasProdVtx()) {
                auto prodVertex = particle->prodVtx();
                --vertexLinksCounts[prodVertex->index()].second;
            }
            if (particle->hasDecayVtx()) {
                auto decayVertex = particle->decayVtx();
                --vertexLinksCounts[decayVertex->index()].first;
            }             
        }
    } 

    // Loop over truth vertices and update mask
    // Those for which all incoming and outgoing particles are to be thinned, will be thinned as well
    for (int i=0; i<nTruthVertices; ++i) {
        if (vertexLinksCounts[i].first!=0 || vertexLinksCounts[i].second!=0) {
            vertexMask[i] = true;
        } else {++m_nVerticesThinned;}
    }
    // Apply masks to thinning
    truthParticles.keep (particleMask);
    truthVertices.keep (vertexMask);
    
    return StatusCode::SUCCESS;
}



// Inline methods
//
// ==============================
// ancestors 
// ==============================
// Updates particle mask such that particle and all ancestors are retained
void ThinGeantTruthAlg::ancestors(const xAOD::TruthParticle* pHead,
                                  std::vector<bool> &particleMask,
                                  std::unordered_set<int> &encounteredBarcodes ) {
           
    // Check that this barcode hasn't been seen before (e.g. we are in a loop)
    std::unordered_set<int>::const_iterator found = encounteredBarcodes.find(pHead->barcode());
    if (found!=encounteredBarcodes.end()) return;
    encounteredBarcodes.insert(pHead->barcode());
   
    // Save particle position in the mask
    int headIndex = pHead->index();
    particleMask[headIndex] = true;
   
    // Get the production vertex
    const xAOD::TruthVertex* prodVtx(0);
    if (pHead->hasProdVtx()) {prodVtx = pHead->prodVtx();}
    else {return;}
   
    // Get children particles and self-call
    int nParents = prodVtx->nIncomingParticles();
    for (int i=0; i<nParents; ++i) ancestors(prodVtx->incomingParticle(i),particleMask,encounteredBarcodes);
    return;
}


// ==============================
// descendants 
// ==============================
// Updates particle mask such that particle and all descendants are retained
void ThinGeantTruthAlg::descendants(const xAOD::TruthParticle* pHead,
                                    std::vector<bool> &particleMask,
                                    std::unordered_set<int> &encounteredBarcodes ) {
    // Check that this barcode hasn't been seen before (e.g. we are in a loop)
    std::unordered_set<int>::const_iterator found = encounteredBarcodes.find(pHead->barcode());
    if (found!=encounteredBarcodes.end()) return;
    encounteredBarcodes.insert(pHead->barcode());
    
    // Save the particle position in the mask
    int headIndex = pHead->index();
    particleMask[headIndex] = true;
    
    // Get the decay vertex
    const xAOD::TruthVertex* decayVtx(0);
    if (pHead->hasDecayVtx()) {decayVtx = pHead->decayVtx();}
    else {return;}
    
    // Get children particles and self-call
    int  nChildren = decayVtx->nOutgoingParticles();
    for (int i=0; i<nChildren; ++i) {
        descendants(decayVtx->outgoingParticle(i),particleMask,encounteredBarcodes);
    }
 
    return;
}

// ==============================
// isStatus1BSMParticle 
// ==============================
// Returns true if a particle is BSM and stable

bool ThinGeantTruthAlg::isStatus1BSMParticle(const xAOD::TruthParticle* part) const{
   
    int pdg = part->pdgId();
    bool status1 = (part->status()==1);
    bool isBSM(false);   

    if ( (31<abs(pdg) && abs(pdg)<38) || // BSM Higgs / W' / Z' / etc
        abs(pdg)==39 ||
        abs(pdg)==41 ||
        abs(pdg)==42 ||
        abs(pdg)== 7 || // 4th gen beauty
        abs(pdg)== 8 || // 4th gen top
        (600 < abs(pdg) && abs(pdg) < 607) || // scalar leptoquarks
        (1000000<abs(pdg) && abs(pdg)<2000000) || // left-handed SUSY (including R-Hadrons)
        (2000000<abs(pdg) && abs(pdg)<3000000) || // right-handed SUSY (including R-Hadrons)
        abs(pdg)==6000005 || // X5/3
        abs(pdg)==6000006 || // T2/3
        abs(pdg)==6000007 || // B-1/3
        abs(pdg)==6000008 || // Y-4/3
        ( (abs(pdg)>=10000100) && (abs(pdg)<=10001000) ) // multi-charged
    ) isBSM = true;

    if (status1 && isBSM) {return true;} else {return false;}        

}
