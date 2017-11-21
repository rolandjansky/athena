///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

// STL includes
#include <algorithm> 

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinGeantTruthAlg::ThinGeantTruthAlg( const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
::AthAlgorithm( name, pSvcLocator ),
m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
m_doThinning(true),
m_geantOffset(200000),
m_longlived{310,3122,3222,3112,3322,3312},
m_truthParticlesKey("TruthParticles"),
m_truthVerticesKey("TruthVertices"),
m_muonsKey("Muons"),
m_electronsKey("Electrons"),
m_photonsKey("Photons"),
m_nEventsProcessed(0),
m_nParticlesProcessed(0),
m_nVerticesProcessed(0),
m_nParticlesThinned(0),
m_nVerticesThinned(0)
{
   
    declareProperty("ThinningSvc",          m_thinningSvc,
                    "The ThinningSvc instance for a particular output stream" );
    
    declareProperty("ThinGeantTruth", m_doThinning,
                    "Should the Geant truth thinning be run?");
   
    declareProperty("GeantBarcodeOffset", m_geantOffset,
                    "Barcode offset for Geant particles");
 
    declareProperty("LongLivedParticleList", m_longlived,
                    "List of long lifetime particles which are likely to be decayed by Geant but whose children must be kept");

    declareProperty("TruthParticlesKey", m_truthParticlesKey,
                    "StoreGate key for TruthParticle container");
    
    declareProperty("TruthVerticesKey", m_truthVerticesKey,
                    "StoreGate key for TruthVertices container");
    
    declareProperty("MuonsKey", m_truthVerticesKey,
                    "StoreGate key for muons container");

    declareProperty("ElectronsKey", m_truthVerticesKey,
                    "StoreGate key for electrons container");

    declareProperty("PhotonsKey", m_photonsKey,
                    "StoreGate key for photons container");

    declareProperty("EGammaTruthKey", m_egammaTruthKey,
                    "StoreGate key for e-gamma truth container");

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
    ATH_MSG_DEBUG ( " using = " << m_thinningSvc );

    // Is truth thinning required?
    if (!m_doThinning) {
        ATH_MSG_INFO("Geant truth thinning not required");
    } else {
        ATH_MSG_INFO("Geant truth will be thinned");
    }

    // Initialize the counters to zero
    m_nEventsProcessed = 0;
    m_nParticlesProcessed = 0;
    m_nVerticesProcessed = 0;
    m_nParticlesThinned = 0;
    m_nVerticesThinned = 0;
    
    ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
    
    return StatusCode::SUCCESS;
}



StatusCode ThinGeantTruthAlg::finalize()
{
    ATH_MSG_DEBUG ("Finalizing " << name() << "...");
    ATH_MSG_INFO("Processed " << m_nEventsProcessed << " events containing " << m_nParticlesProcessed << " truth particles and " << m_nVerticesProcessed << " truth vertices ");     
    ATH_MSG_INFO("Removed " << m_nParticlesThinned << " Geant truth particles and " << m_nVerticesThinned << " corresponding truth vertices ");
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
    const xAOD::TruthParticleContainer* truthParticles(0);
    const xAOD::TruthVertexContainer* truthVertices(0);
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_truthParticlesKey)) {
        CHECK( evtStore()->retrieve( truthParticles , m_truthParticlesKey ) );
    } else {
        ATH_MSG_FATAL("No TruthParticleContainer with key "+m_truthParticlesKey+" found.");
        return StatusCode::FAILURE;
    }
    if (evtStore()->contains<xAOD::TruthVertexContainer>(m_truthVerticesKey)) {
        CHECK( evtStore()->retrieve( truthVertices , m_truthVerticesKey ) );
    } else {
        ATH_MSG_FATAL("No TruthVertexContainer with key "+m_truthVerticesKey+" found.");
        return StatusCode::FAILURE;
    }

    // Retrieve muons, electrons and photons
    const xAOD::MuonContainer* muons(0);
    if (evtStore()->contains<xAOD::MuonContainer>(m_muonsKey)) {
        CHECK( evtStore()->retrieve( muons , m_muonsKey ) );
    } else {
        ATH_MSG_WARNING("No muon container with key "+m_muonsKey+" found.");
    }
    const xAOD::ElectronContainer* electrons(0);
    if (evtStore()->contains<xAOD::ElectronContainer>(m_electronsKey)) {
        CHECK( evtStore()->retrieve( electrons , m_electronsKey ) );
    } else {
        ATH_MSG_WARNING("No electron container with key "+m_electronsKey+" found.");
    }
    const xAOD::PhotonContainer* photons(0);
    if (evtStore()->contains<xAOD::PhotonContainer>(m_photonsKey)) {
        CHECK( evtStore()->retrieve( photons , m_photonsKey ) );
    } else {
        ATH_MSG_WARNING("No photon container with key "+m_photonsKey+" found.");
    }
   
    // Loop over photons, electrons and muons and get the associated truth particles
    // Retain the associated index number
    std::vector<int> recoParticleTruthIndices;
    if (muons!=nullptr) {
        for (auto muon : *muons) {
            const xAOD::TruthParticle* truthMuon = xAOD::TruthHelpers::getTruthParticle(*muon); 
            if (truthMuon) recoParticleTruthIndices.push_back(truthMuon->index());
        }
    }
    if (electrons!=nullptr) {
        for (auto electron : *electrons) {
            const xAOD::TruthParticle* truthElectron = xAOD::TruthHelpers::getTruthParticle(*electron);
            if (truthElectron) recoParticleTruthIndices.push_back(truthElectron->index());
        }
    }
    if (photons!=nullptr) {
        for (auto photon : *photons) {
            const xAOD::TruthParticle* truthPhoton = xAOD::TruthHelpers::getTruthParticle(*photon);
            if (truthPhoton) recoParticleTruthIndices.push_back(truthPhoton->index());
        } 
    }

    //Set up the indices for the egamma Truth Particles to keep
    std::vector<int> egammaTruthIndices{};
    const xAOD::TruthParticleContainer* egammaTruthParticles(0);
    if (evtStore()->contains<xAOD::TruthParticleContainer>(m_egammaTruthKey)) {
      CHECK( evtStore()->retrieve( egammaTruthParticles , m_egammaTruthKey ) );
    } else {
      ATH_MSG_WARNING("No e-gamma truth container with key "+m_egammaTruthKey+" found.");
    }

    if (egammaTruthParticles!=nullptr) {

      for (auto egTruthParticle : *egammaTruthParticles) {

	static const SG::AuxElement::Accessor<int> accType("truthType");
	if(!accType.isAvailable(*egTruthParticle) || 
	   accType(*egTruthParticle)!=MCTruthPartClassifier::IsoElectron){
	  continue;
	}
	
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

        // Retain particles and their descendants/ancestors associated with the egamma Truth Particles
        if ( std::find(egammaTruthIndices.begin(), egammaTruthIndices.end(), i) != recoParticleTruthIndices.end() ) { 
	  ancestors(particle,particleMask,encounteredBarcodes);
	  encounteredBarcodes.clear();
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
    
    // Apply masks to thinning service
    if (m_thinningSvc->filter(*truthParticles, particleMask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed for truth particles! ");
        return StatusCode::FAILURE;
    }
    if (m_thinningSvc->filter(*truthVertices, vertexMask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed for truth vertices! ");
        return StatusCode::FAILURE;
    }
    
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
        (1000000<abs(pdg) && abs(pdg)<1000040) || // left-handed SUSY
        (2000000<abs(pdg) && abs(pdg)<2000040) || // right-handed SUSY
        abs(pdg)==6000005 || // X5/3
        abs(pdg)==6000006 || // T2/3
        abs(pdg)==6000007 || // B-1/3
        abs(pdg)==6000008 || // Y-4/3
        ( (abs(pdg)>=10000100) && (abs(pdg)<=10001000) ) // multi-charged
    ) isBSM = true;

    if (status1 && isBSM) {return true;} else {return false;}        

}
