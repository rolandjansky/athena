/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthCollectionMaker.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all truth particles/vertices which do not pass a user-defined cut

#include "DerivationFrameworkMCTruth/TruthCollectionMaker.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
//#include "xAODTruth/TruthVertexContainer.h"
#include <vector>
#include <string>
#include <numeric>

// Constructor
DerivationFramework::TruthCollectionMaker::TruthCollectionMaker(const std::string& t,
                                                                const std::string& n,
                                                                const IInterface* p)
//m_do_compress = true: removes particles with the same pdgId in a decay chain (but keeps first and last)
//m_do_sherpa = true: checks if there are truth W bosons in the current record.  If not, tries to combine W daughters to create one.
//Disclaimer: m_do_sherpa currently only works for W+jets.  It will not work for Z+jets for dibosons (coming soon).
  : AthAlgTool(t,n,p)
  , m_ntotpart(0)
  , m_npasspart(0)
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ParticlesKey", m_particlesKey="TruthParticles");
    //declareProperty("VerticesKey", m_verticesKey);
    declareProperty("NewCollectionName", m_collectionName="");
    declareProperty("ParticleSelectionString", m_partString="");
    declareProperty("Do_Compress",m_do_compress=false);
    declareProperty("Do_Sherpa",m_do_sherpa=false);
    declareProperty("KeepNavigationInfo",m_keep_navigation_info=true);
}

// Destructor
DerivationFramework::TruthCollectionMaker::~TruthCollectionMaker() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthCollectionMaker::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    
    if (m_particlesKey=="" /*|| m_verticesKey==""*/) {
        ATH_MSG_FATAL("No truth particle collection provided to use as a basis for new collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_particlesKey << " as the source collections for new truth collections");}
    
    if (m_collectionName=="") {
        ATH_MSG_FATAL("No key provided for the new truth particle collection");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("New truth particle collection key: " << m_collectionName );}
    
    if (m_partString=="") {
        ATH_MSG_FATAL("No selection string provided");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Truth particle selection string: " << m_partString );}
    
    // Set up the text-parsing machinery for thinning the truth directly according to user cuts
    if ( m_partString!="") {
        ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
        proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
        proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
        if (m_partString!="") {
            m_partParser = new ExpressionParsing::ExpressionParser(proxyLoaders);
            m_partParser->loadExpression(m_partString);
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthCollectionMaker::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    //ATH_MSG_INFO("Processed "<< m_ntotvtx <<" truth vertices, "<< m_npassvtx << " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotpart <<" truth particles, "<< m_npasspart << " were retained ");
    if (m_partString!="") {
        delete m_partParser;
        m_partParser = 0;
    }
    return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthCollectionMaker::addBranches() const
{
    
    // Retrieve truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles;
    if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    /*const xAOD::TruthVertexContainer* importedTruthVertices;
     if (evtStore()->retrieve(importedTruthVertices,m_verticesKey).isFailure()) {
     ATH_MSG_ERROR("No TruthVertex collection with name " << m_verticesKey << " found in StoreGate!");
     return StatusCode::FAILURE;
     }*/
    
    // Create the new containers
    xAOD::TruthParticleContainer* newParticleCollection = new xAOD::TruthParticleContainer();
    CHECK( evtStore()->record( newParticleCollection, m_collectionName ) );
    xAOD::TruthParticleAuxContainer* newParticleAuxCollection = new xAOD::TruthParticleAuxContainer();
    CHECK( evtStore()->record( newParticleAuxCollection, m_collectionName + "Aux." ) );
    newParticleCollection->setStore( newParticleAuxCollection );
    ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " <<  m_collectionName);
    
    // Set up a mask with the same entries as the full collections
    unsigned int nParticles = importedTruthParticles->size();
    //unsigned int nVertices = importedTruthVertices->size();
    m_ntotpart += nParticles;
    
    // Set up decorators
    const static SG::AuxElement::Decorator< ElementLink<xAOD::TruthParticleContainer> > linkDecorator("originalTruthParticle");
    const static SG::AuxElement::Decorator< unsigned int > originDecorator("classifierParticleOrigin");
    const static SG::AuxElement::Decorator< unsigned int > typeDecorator("classifierParticleType");
    const static SG::AuxElement::Decorator< unsigned int > outcomeDecorator("classifierParticleOutCome");
    const static SG::AuxElement::Decorator< int > motherIDDecorator("motherID");
    const static SG::AuxElement::Decorator< int > daughterIDDecorator("daughterID");
    const static SG::AuxElement::Decorator< int > HadronOriginDecorator("TopHadronOriginFlag");
    
    // Execute the text parsers and update the mask
    if (m_partString!="") {
        std::vector<int> entries =  m_partParser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nParticles != nEntries ) {
            ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used TruthParticles?");
            return StatusCode::FAILURE;
        } else {
            // add relevant particles to new collection
            
            //---------------
            //This is some code to *add* new particles. Probably a good idea to break this off as a sub-function, but I'll let James C decide where that should go.
            //---------------
            
            //Let's check if we want to build W/Z bosons
            bool SherpaW = false;
            bool SherpaZ = false;
            if (m_partString.find("24") != std::string::npos && m_do_sherpa) {
                SherpaW = true;
            }
            if (m_partString.find("23") != std::string::npos && m_do_sherpa) {
                SherpaZ = true;
            }
            if (SherpaW or SherpaZ){
                if (std::accumulate(entries.begin(),entries.end(),0) > 0){ //We actually have some W and Z bosons in there.
                    SherpaW = false;
                    SherpaZ = false;
                }
            }
            
            if (SherpaW){ //In principle, we should add a block for Z's and dibosons as well - it will look quite similar to this one.
                //auto xTruthParticle = make_particle(&*importedTruthParticles);
                //newParticleCollection->push_back( xTruthParticle );
                //return StatusCode::SUCCESS;
            }
            for (unsigned int i=0; i<nParticles; ++i) {
                ElementLink<xAOD::TruthParticleContainer> eltp(*importedTruthParticles,i);
                if (entries[i]==1) {
                    //In TRUTH3, we want to remove all particles but the first and last in a decay chain.  This is off in TRUTH1.  The first and last particles in the decay chain are decorated as such.
                    
                    const xAOD::TruthParticle* theParticle = (*importedTruthParticles)[i];
                    if (m_do_compress){
                        bool same_as_mother = false;
                        bool same_as_daughter = false;
                        if (theParticle->hasProdVtx()){
                            if ((theParticle->prodVtx()->nIncomingParticles() > 0) && (theParticle->prodVtx()->incomingParticle(0)!=nullptr)) {
                                if (theParticle->prodVtx()->incomingParticle(0)->pdgId() == theParticle->pdgId()){
                                    same_as_mother = true;
                                }
                            }
                        }
                        if (theParticle->hasDecayVtx()){
                            if ((theParticle->decayVtx()->nOutgoingParticles() > 0) && (theParticle->decayVtx()->outgoingParticle(0)!=nullptr)) {
                                if (theParticle->decayVtx()->outgoingParticle(0)->pdgId() == theParticle->pdgId()){
                                    same_as_daughter = true;
                                }
                            }
                        }
                        if (same_as_mother && same_as_daughter){
                            entries[i]=0;
                            continue;
                        }
                    }
                    xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
                    newParticleCollection->push_back( xTruthParticle );
                    if (m_keep_navigation_info){
                        if (theParticle->hasProdVtx()) {
                            if ((theParticle->prodVtx()->nIncomingParticles() > 0) && (theParticle->prodVtx()->incomingParticle(0)!=nullptr)) {
                                motherIDDecorator(*xTruthParticle) = theParticle->prodVtx()->incomingParticle(0)->pdgId();
                            } else {motherIDDecorator(*xTruthParticle) = 0;}
                        } else {motherIDDecorator(*xTruthParticle) = 0;} 
                        if (theParticle->hasDecayVtx()) {
                            if ((theParticle->decayVtx()->nOutgoingParticles() > 0) && (theParticle->decayVtx()->outgoingParticle(0)!=nullptr)) {
                                daughterIDDecorator(*xTruthParticle) = theParticle->decayVtx()->outgoingParticle(0)->pdgId();
                            } else {daughterIDDecorator(*xTruthParticle) = 0;}
                        } else {daughterIDDecorator(*xTruthParticle) = 0;}
                    }
                    // Fill with numerical content
                    xTruthParticle->setPdgId(theParticle->pdgId());
                    xTruthParticle->setBarcode(theParticle->barcode());
                    xTruthParticle->setStatus(theParticle->status());
                    xTruthParticle->setM(theParticle->m());
                    xTruthParticle->setPx(theParticle->px());
                    xTruthParticle->setPy(theParticle->py());
                    xTruthParticle->setPz(theParticle->pz());
                    xTruthParticle->setE(theParticle->e());
                    // Copy over the decorations if they are available
                    if (theParticle->isAvailable<unsigned int>("classifierParticleType")) {
                        typeDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleType" );
                    } else {typeDecorator(*xTruthParticle) = 0;}
                    if (theParticle->isAvailable<unsigned int>("classifierParticleOrigin")) {
                        originDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleOrigin" );
                    } else {originDecorator(*xTruthParticle) = 0;}
                    if (theParticle->isAvailable<unsigned int>("classifierParticleOutCome")) {
                        outcomeDecorator(*xTruthParticle) = theParticle->auxdata< unsigned int >( "classifierParticleOutCome" );
                    } else {outcomeDecorator(*xTruthParticle) = 0;}
                    if (m_collectionName=="TruthHFHadrons"){
                        if (theParticle->isAvailable<int>("TopHadronOriginFlag")) {
                            HadronOriginDecorator(*xTruthParticle) = theParticle->auxdata< int >( "TopHadronOriginFlag" );
                        } else {HadronOriginDecorator(*xTruthParticle) = 0;}
                    }
                    if(m_keep_navigation_info) linkDecorator(*xTruthParticle) = eltp;
                }
            }
        }
        // Count the mask
        for (unsigned int i=0; i<nParticles; ++i) if (entries[i]) ++m_npasspart;
    }
    
    return StatusCode::SUCCESS;
}


/*
 xAOD::TruthParticle* DerivationFramework::TruthCollectionMaker::make_particle(const xAOD::TruthParticleContainer* importedTruthParticles){
 
 xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
 unsigned int nParticles = importedTruthParticles->size();
 
 bool sherpa_foundone = false;
 for (unsigned int i=0; i<nParticles; ++i) {
 xAOD::TruthParticle* theParticle = const_cast<xAOD::TruthParticle*>( (*importedTruthParticles)[i] );
 if (!theParticle->isChLepton()) continue;
 for (unsigned int j=0; j<nParticles; ++j) {
 xAOD::TruthParticle* theParticle2 = const_cast<xAOD::TruthParticle*>( (*importedTruthParticles)[j] );
 if (!theParticle2->isNeutrino()) continue;
 if (abs(theParticle->pdgId()+theParticle2->pdgId())!=1) continue;
 if (!(theParticle->prodVtx()->incomingParticle(0)->isQuark() || theParticle->prodVtx()->incomingParticle(0)->pdgId()==21)) continue;
 if (!(theParticle2->prodVtx()->incomingParticle(0)->isQuark() || theParticle2->prodVtx()->incomingParticle(0)->pdgId()==21)) continue;
 
 xTruthParticle->setBarcode(-1);
 xTruthParticle->setPdgId(theParticle->threeCharge()*8); //3*8 = 24.                                                                                        
 xTruthParticle->setStatus(-1);
 xTruthParticle->setM((theParticle->p4()+theParticle2->p4()).M());
 xTruthParticle->setPx((theParticle->p4()+theParticle2->p4()).Px());
 xTruthParticle->setPy((theParticle->p4()+theParticle2->p4()).Py());
 xTruthParticle->setPz((theParticle->p4()+theParticle2->p4()).Pz());
 xTruthParticle->setE((theParticle->p4()+theParticle2->p4()).E());
 
 sherpa_foundone = true;
 break; //Just take the first one we find.                                                                                                                  
 }
 
 if (sherpa_foundone) break;
 }
 
 return xTruthParticle;
 
 }
 */
