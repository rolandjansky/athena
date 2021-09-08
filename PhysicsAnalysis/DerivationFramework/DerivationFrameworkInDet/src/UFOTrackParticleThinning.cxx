/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// UFOTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/UFOTrackParticleThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::UFOTrackParticleThinning::UFOTrackParticleThinning(const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_ntot(0),
m_npass(0),
m_ntotCC(0),
m_npassCC(0),
m_ufoSGKey(""),
m_inDetSGKey("InDetTrackParticles"),
m_jetSGKey("AntiKt10TrackCaloClusterJets"),
m_selectionString(""),
m_PFOSGKey("JetETMiss"),
m_and(false) 
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService"               , m_thinningSvc);
    declareProperty("UFOKey"                        , m_ufoSGKey);
    declareProperty("InDetTrackParticlesKey"        , m_inDetSGKey);
    declareProperty("PFOCollectionSGKey"            , m_PFOSGKey);
    declareProperty("AdditionalPFOKey"              , m_addPFOSGKey);
    declareProperty("JetKey"                        , m_jetSGKey);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd"                      , m_and);
}

// Destructor
DerivationFramework::UFOTrackParticleThinning::~UFOTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::UFOTrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    
    if (m_PFOSGKey=="") {
        ATH_MSG_FATAL("No topocluster collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_PFOSGKey << "as the source collection for topoclusters");}
    
    if (m_jetSGKey=="") {
        ATH_MSG_FATAL("No jet collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_jetSGKey << "as the source collection for UFOs");}
    
    if (m_ufoSGKey=="") {
        ATH_MSG_FATAL("No UFO collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_ufoSGKey << " will be retained in this format with the rest being thinned away");}
        
    // Set up the text-parsing machinery for selectiong the UFO directly according to user cuts
    if (m_selectionString!="") {
      m_proxyLoaders = std::move( std::make_unique<ExpressionParsing::MultipleProxyLoader>());
      m_cleanup.push_back( std::move(std::make_unique<ExpressionParsing::SGxAODProxyLoader>(evtStore())));
      m_proxyLoaders->push_back(m_cleanup.back().get());
      m_cleanup.push_back( std::move(std::make_unique<ExpressionParsing::SGNTUPProxyLoader>(evtStore())));
      m_proxyLoaders->push_back(m_cleanup.back().get());
      m_parser = std::move( std::make_unique<ExpressionParsing::ExpressionParser>(m_proxyLoaders.get()));
      m_parser->loadExpression(m_selectionString);
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::UFOTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotCC <<" calo clusters, "<< m_npassCC<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotUFO <<" UFOs, "<< m_npassUFO<< " were retained ");
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::UFOTrackParticleThinning::doThinning() const
{
    
  // Retrieve main TrackParticle collection
  const xAOD::TrackParticleContainer* importedTrackParticles;
  if (evtStore()->retrieve(importedTrackParticles,m_inDetSGKey).isFailure()) {
        ATH_MSG_ERROR("No TrackParticle collection with name " << m_inDetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
  }
    
  // Retrieve PFO collection if required
  const xAOD::PFOContainer* importedPFONeutral = nullptr;
  ATH_CHECK( evtStore()->retrieve( importedPFONeutral,  m_PFOSGKey + "NeutralParticleFlowObjects") );

  const xAOD::PFOContainer* importedPFOCharged = nullptr;
  ATH_CHECK( evtStore()->retrieve( importedPFOCharged,  m_PFOSGKey+"ChargedParticleFlowObjects" ) );


  // retrieve additional PFO container(s)
  std::vector< const xAOD::PFOContainer* > additionalPFOCharged;
  std::vector< const xAOD::PFOContainer* > additionalPFONeutral;

  for( const std::string& name : m_addPFOSGKey ) {
      const xAOD::PFOContainer* tempPFOCharged = nullptr;
      ATH_CHECK( evtStore()->retrieve( tempPFOCharged, name+"ChargedParticleFlowObjects" ) );
      additionalPFOCharged.push_back(tempPFOCharged);

      const xAOD::PFOContainer* tempPFONeutral = nullptr;
      ATH_CHECK( evtStore()->retrieve( tempPFONeutral, name+"NeutralParticleFlowObjects" ) );
      additionalPFONeutral.push_back(tempPFONeutral);
  }

  // Retrieve main jet collection
  const xAOD::JetContainer* importedJets;
  if (evtStore()->retrieve(importedJets,m_jetSGKey).isFailure()) {
        ATH_MSG_ERROR("No jet collection with name " << m_jetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
  }
  unsigned int nJets(importedJets->size());
  std::vector<const xAOD::Jet*> jetToCheck; jetToCheck.clear();
 
  // Check the event contains tracks
  unsigned int nTracks = importedTrackParticles->size();
  // Check the event contains calo clusters
  const size_t nPFONeutral = importedPFONeutral->size();
  const size_t nPFOCharged = importedPFOCharged->size() ;
  unsigned int nPFOs = nPFOCharged + nPFONeutral;
  if (nPFOs==0 && nTracks==0) return StatusCode::SUCCESS;
    
  // Set up a mask with the same entries as the full TrackParticle collection
  std::vector<bool> maskTracks;
  maskTracks.assign(nTracks,false); // default: don't keep any tracks
  m_ntot += nTracks;
    
  // Set up a mask with the same entries as the full PFO collection(s)
  std::vector< bool > pfomaskNeutral( nPFONeutral, false );
  std::vector< bool > pfomaskCharged( nPFOCharged, false );
  m_ntotCC += nPFOs;
    
  // Retrieve containers
  // ... UFOs
  const xAOD::TrackCaloClusterContainer* importedUFOs(0);
  if (evtStore()->retrieve(importedUFOs,m_ufoSGKey).isFailure()) {
        ATH_MSG_ERROR("No UFO collection with name " << m_ufoSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
  }
  unsigned int nUFOs(importedUFOs->size());
    
  // Set up a mask with the same entries as the full CaloCluster collection
  std::vector<bool> maskUFOs;
  maskUFOs.assign(nUFOs,false); // default: don't keep any tracks
  m_ntotUFO += nUFOs;
    
  // Execute the text parser if requested
  if (m_selectionString!="") {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nJets != nEntries ) {
        	ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used jets??");
            return StatusCode::FAILURE;
        } else {
        	// identify which jets to keep for the thinning check
        	for (unsigned int i=0; i<nJets; ++i) if (entries[i]==1) jetToCheck.push_back((*importedJets)[i]);
        }
  }
    
  if (m_selectionString=="") { // check all jets as user didn't provide a selection string

    for(auto jet : *importedJets){
      for( size_t j = 0; j < jet->numConstituents(); ++j ) {
        auto ufo = jet->constituentLinks().at(j);
        int index = ufo.index();
        maskUFOs[index] = true;
        const xAOD::TrackCaloCluster* ufoO = dynamic_cast<const xAOD::TrackCaloCluster*>(*ufo);
        if(!ufoO) continue;
        if(ufoO->taste()==2){
          index = ufoO->trackParticleLink().index();
           if(index>=0) { 
             maskTracks[index] = true;
          }
        }
        if(ufoO->taste()==0){
          index = dynamic_cast<const xAOD::PFO*>(*ufoO->iparticleLinks().at(0))->track(0)->index();
          if(index>=0) {
            maskTracks[index] = true;
          }
        }
        for (size_t c = 0; c < ufoO->iparticleLinks().size(); ++c) {
          index = ufoO->iparticleLinks().at(c).index();
          if(index<0) continue;
          bool isCharged = std::abs((dynamic_cast<const xAOD::PFO*>( *ufoO->iparticleLinks().at(c)))->charge()) > FLT_MIN;

          // If it's charged, add it to the charged mask
          if(isCharged) pfomaskCharged.at( index ) = true;
          // Otherwise, add it to the neutral mask
          else pfomaskNeutral.at( index ) = true;
        } // for c < ufoO->iparticleLinks().size()
      }
    }
	
  } else {
	
  	for (std::vector<const xAOD::Jet*>::iterator jetIt=jetToCheck.begin(); jetIt!=jetToCheck.end(); ++jetIt) {
      for( size_t j = 0; j < (*jetIt)->numConstituents(); ++j ) {
        auto ufo = (*jetIt)->constituentLinks().at(j);
    	  int index = ufo.index();
	    	maskUFOs[index] = true;
    		const xAOD::TrackCaloCluster* ufoO = dynamic_cast<const xAOD::TrackCaloCluster*>(*ufo);
	    	if(!ufoO) continue;
    		if(ufoO->taste()==2){
	    	  index = ufoO->trackParticleLink().index();
		      if(index>=0) maskTracks[index] = true;
    		}
        if(ufoO->taste()==0){
          index = dynamic_cast<const xAOD::PFO*>(*ufoO->iparticleLinks().at(0))->track(0)->index();
          if(index>=0) {
            maskTracks[index] = true;
          }
        }
        for (size_t c = 0; c < ufoO->iparticleLinks().size(); ++c) {
          index = ufoO->iparticleLinks().at(c).index();
          if(index<0) continue;

          bool isCharged = std::abs((dynamic_cast<const xAOD::PFO*>( *ufoO->iparticleLinks().at(c)))->charge()) > FLT_MIN;
          // If it's charged, add it to the charged mask
          if(isCharged) pfomaskCharged.at( index ) = true;
          // Otherwise, add it to the neutral mask
          else pfomaskNeutral.at( index ) = true;
		    }
	    }
  	}
	
  }

  // Count up the mask contents
  for (unsigned int i=0; i<nTracks; ++i) {
        if (maskTracks[i]) ++m_npass;
  }
    
  for (unsigned int i=0; i<nUFOs; ++i) {
        if (maskUFOs[i]) ++m_npassUFO;
  }
    
  // Execute the thinning service based on the mask. Finish.
  const IThinningSvc::Operator::Type opType =
              ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
  if (m_thinningSvc->filter(*importedTrackParticles, maskTracks, opType).isFailure()) {
            ATH_MSG_ERROR("Application of thinning service failed! ");
            return StatusCode::FAILURE;
  }
  ATH_CHECK( m_thinningSvc->filter( *importedPFONeutral, pfomaskNeutral, opType ) );
  ATH_CHECK( m_thinningSvc->filter( *importedPFOCharged, pfomaskCharged, opType ) );

  for( const xAOD::PFOContainer* cpfo : additionalPFONeutral ) {
          ATH_CHECK( m_thinningSvc->filter( *cpfo, pfomaskNeutral, opType ) );
  }
  for( const xAOD::PFOContainer* cpfo : additionalPFOCharged ) {
          ATH_CHECK( m_thinningSvc->filter( *cpfo, pfomaskCharged, opType ) );
  }

  if (m_thinningSvc->filter(*importedUFOs, maskUFOs, opType).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

