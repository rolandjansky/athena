/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/JetManager.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"


using namespace Trig;

#if !defined( XAOD_STANDALONE ) && !defined( XAOD_ANALYSIS )
ToolHandle< Analysis::IBTagTool > *JetManager::m_bTagTool = nullptr;
ToolHandle< Analysis::IBTagTrackAssociation > *JetManager::m_bTagTrackAssocTool = nullptr;
ToolHandle< Analysis::IBTagSecVertexing > *JetManager::m_bTagSecVtxTool = nullptr;
#endif

//**********************************************************************

JetManager::JetManager(std::string name,ToolHandle<Trig::TrigDecisionTool>& trigDec,std::string chain)
  : AsgTool(name), m_chain(chain), 
    m_containers( "","" ), m_keys( "","","","" ),
    m_uses4x4(false),
    m_trigDec(trigDec) 
{
  m_jet_Containers = std::unique_ptr< xAOD::JetContainer >( new xAOD::JetContainer() );
  std::unique_ptr< xAOD::AuxContainerBase > jet_Containers_Aux( new xAOD::AuxContainerBase() );
  m_jet_Containers->setStore( jet_Containers_Aux.release() );

  m_jetRoI_Containers = std::unique_ptr< xAOD::JetRoIContainer >( new xAOD::JetRoIContainer() );
  std::unique_ptr< xAOD::AuxContainerBase > jetRoI_Containers_Aux( new xAOD::AuxContainerBase() );
  m_jetRoI_Containers->setStore( jetRoI_Containers_Aux.release() );

  m_btagging_Containers = std::unique_ptr< xAOD::BTaggingContainer >( new xAOD::BTaggingContainer() );
  std::unique_ptr< xAOD::AuxContainerBase > btagging_Containers_Aux( new xAOD::AuxContainerBase() );
  m_btagging_Containers->setStore( btagging_Containers_Aux.release() );

  m_primaryVertex_Containers = std::unique_ptr< xAOD::VertexContainer >( new xAOD::VertexContainer() );
  std::unique_ptr< xAOD::AuxContainerBase > primaryVertex_Containers_Aux( new xAOD::AuxContainerBase() );
  m_primaryVertex_Containers->setStore( primaryVertex_Containers_Aux.release() );
}
JetManager::~JetManager() {}


void JetManager::setContainers(std::string jet_container,std::string bTag_container) {
  std::get< EventElement::JET >( m_containers )  = jet_container;
  std::get< EventElement::BTAG >( m_containers ) = bTag_container;
}
void JetManager::setKeys(std::string jet_Key,std::string primaryVertex_key,std::string trackParticle_key) {
  std::get< EventElement::JET >( m_keys )          = jet_Key;
  std::get< EventElement::PRIM_VTX >( m_keys )     = primaryVertex_key;
  std::get< EventElement::TRK_PARTICLE >( m_keys ) = trackParticle_key;
}

void JetManager::use4x4( bool set4x4 ) { m_uses4x4 = set4x4; }

std::string JetManager::chainName() const { return m_chain; }
std::string JetManager::jetContainerName() const { return std::get< EventElement::JET >(m_containers); }
std::string JetManager::btaggingContainerName() const { return std::get<EventElement::BTAG>(m_containers); }
std::string JetManager::jetKeyName() const { return std::get< EventElement::JET >( m_keys ); }
std::string JetManager::btaggingKeyName() const { return std::get< EventElement::BTAG >( m_keys ); }
std::string JetManager::primaryVertexKeyName() const { return std::get< EventElement::PRIM_VTX >( m_keys ); }
std::string JetManager::trackParticleKeyName() const { return std::get< EventElement::TRK_PARTICLE >( m_keys ); }

unsigned int JetManager::jetSize() const { return m_jet_Containers->size(); }
unsigned int JetManager::jetRoISize() const { return m_jetRoI_Containers->size(); }
unsigned int JetManager::btaggingSize() const { return m_btagging_Containers->size(); }
unsigned int JetManager::primaryVertexSize() const { return m_primaryVertex_Containers->size(); }
unsigned int JetManager::trackParticleSize() const { return m_trackParticle_Containers.size(); }

StatusCode JetManager::retrieveByNavigation() {
  clear();
  ATH_MSG_DEBUG( "Retrieving via Navigation ..." );

  Trig::FeatureContainer features = m_trigDec->features(m_chain);
  const std::vector< Trig::Combination >& combinations = features.getCombinations();

  for ( Trig::Combination combo : combinations ) {
    getFromCombo( m_jet_Containers            ,combo ,std::get< EventElement::JET >( m_keys ) );
    getFromCombo( m_btagging_Containers       ,combo ,std::get< EventElement::BTAG >( m_keys ) );
    getFromCombo( m_primaryVertex_Containers  ,combo ,std::get< EventElement::PRIM_VTX >( m_keys ) );
    getTPfromCombo( m_trackParticle_Containers,combo ,std::get< EventElement::TRK_PARTICLE >( m_keys ) );
  }

  jetCopy( m_jet_Containers );
  return StatusCode::SUCCESS;
}

template<typename T> bool JetManager::getFromCombo(std::unique_ptr< DataVector< T > > &output,const Trig::Combination& combo,std::string key) {
  const DataVector< T > *tmpContainer = nullptr;
  const std::vector< Trig::Feature< DataVector<T> > > tmpFeatures = combo.containerFeature< DataVector<T> >(key.c_str());
  
  if (tmpFeatures.size())
    tmpContainer = tmpFeatures[0].cptr();
  if (tmpContainer == nullptr)
    return false;
  
  T* toBeAdded = new T();
  output->push_back( toBeAdded );
  *toBeAdded = *tmpContainer->at(0);

  return true;
}

bool JetManager::getTPfromCombo(std::vector< std::unique_ptr< xAOD::TrackParticleContainer > >& tpContainers,const Trig::Combination& combo,std::string key) {
  std::unique_ptr< xAOD::TrackParticleContainer > trackParticles( new xAOD::TrackParticleContainer() );
  std::unique_ptr< xAOD::AuxContainerBase > trackParticlesAux( new xAOD::AuxContainerBase );
  trackParticles->setStore( trackParticlesAux.release() );

  const xAOD::TrackParticleContainer *tmpTrackContainer = nullptr;
  const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > trackParticleContainerFeatures = combo.containerFeature<xAOD::TrackParticleContainer>( key.c_str() );

  if (trackParticleContainerFeatures.size())
    tmpTrackContainer = trackParticleContainerFeatures[0].cptr();
  if (tmpTrackContainer == nullptr)
    return false;

  for ( const xAOD::TrackParticle* el : *tmpTrackContainer ) {
    xAOD::TrackParticle *particle = new xAOD::TrackParticle();
    trackParticles->push_back( particle );
    *particle = *el;
  }

  tpContainers.push_back( std::move( trackParticles ) );
  return true;
}

void JetManager::jetCopy( std::unique_ptr< xAOD::JetContainer >& container ) {
  for ( const xAOD::Jet *theJet : *container.get() ) 
    m_outputJets.push_back( std::unique_ptr< TrigBtagEmulationJet >( new TrigBtagEmulationJet( msg(),theJet ) ) );
}
void JetManager::jetCopy( std::unique_ptr< xAOD::JetRoIContainer >& container ) {
  for ( const xAOD::JetRoI *theJetRoI : *container.get() ) 
    m_outputJets.push_back( std::unique_ptr< TrigBtagEmulationJet >( new TrigBtagEmulationJet( msg(),theJetRoI,m_uses4x4 ) ) );
}

StatusCode JetManager::retagCopy(bool useNavigation,bool tagOffline,bool tagOnline) {
  if ( m_jet_Containers->size() == 0 ) return StatusCode::SUCCESS;

  for (unsigned int i(0); i<m_outputJets.size(); i++) {
    std::unique_ptr< TrigBtagEmulationJet >& out = m_outputJets.at(i);
    const xAOD::BTagging *btag = m_btagging_Containers->at(i);

    const double mv2c10 = Trig::retrieveAuxDataConst< double >( btag,"MV2c10_discriminant",-99. );
    const double mv2c20 = Trig::retrieveAuxDataConst< double >( btag,"MV2c20_discriminant",-99. );

    double ip3dsv1 = btag->SV1plusIP3D_discriminant();
    double combw = (btag->IP3D_pb()/btag->IP3D_pu()) * (btag->SV1_pb()/btag->SV1_pu());
    double comb = 50;
    if(combw/(1+combw)<1) comb=-1.0*TMath::Log10(1-(combw/(1+combw)));

    if (!useNavigation || !tagOffline) {
      out->weights( "MV2c10",mv2c10 );
      out->weights( "MV2c20",mv2c20 );
    }

    if (!useNavigation || !tagOnline) {
      out->weights( "IP3DSV1",ip3dsv1 );
      out->weights( "COMB"   ,comb );
    }
  }
  return StatusCode::SUCCESS;
}

bool JetManager::clear() {
  m_jet_Containers->clear();
  m_jetRoI_Containers->clear();
  m_primaryVertex_Containers->clear();
  m_trackParticle_Containers.clear();
  m_btagging_Containers->clear();

  m_outputJets.clear();

  return true;
}

StatusCode JetManager::retagOffline() {
  if ( m_jet_Containers->size() == 0 ) return StatusCode::SUCCESS;

#if !defined( XAOD_STANDALONE ) && !defined( XAOD_ANALYSIS )
  xAOD::VertexContainer::iterator pv  = m_primaryVertex_Containers->begin();
  std::vector<std::unique_ptr<DataVector<xAOD::TrackParticle_v1> > >::iterator tp = m_trackParticle_Containers.begin();
  std::vector< std::unique_ptr< TrigBtagEmulationJet > >::iterator out = m_outputJets.begin();

  for ( xAOD::Jet *jet : *m_jet_Containers.get() ) {
    // Create container for copied jets
    std::unique_ptr< xAOD::JetContainer > output_jets( new xAOD::JetContainer(SG::OWN_ELEMENTS) );
    std::unique_ptr< xAOD::JetAuxContainer > output_jetsAux( new xAOD::JetAuxContainer );
    output_jets->setStore( output_jetsAux.release() );

    // Create container for new BTagging info
    std::unique_ptr< xAOD::BTaggingContainer > output_btags( new xAOD::BTaggingContainer(SG::OWN_ELEMENTS) );
    std::unique_ptr< xAOD::BTaggingAuxContainer > output_btagsAux( new xAOD::BTaggingAuxContainer() );
    output_btags->setStore( output_btagsAux.release() );

    // Create container for SV results
    std::unique_ptr< xAOD::VertexContainer > output_svs( new xAOD::VertexContainer(SG::OWN_ELEMENTS) );
    std::unique_ptr< xAOD::VertexAuxContainer > output_svsAux( new xAOD::VertexAuxContainer() );
    output_svs->setStore( output_svsAux.release() );

    // Create container for JF results
    std::unique_ptr< xAOD::BTagVertexContainer > output_jfs( new xAOD::BTagVertexContainer(SG::OWN_ELEMENTS) );
    std::unique_ptr< xAOD::BTagVertexAuxContainer > output_jfsAux( new xAOD::BTagVertexAuxContainer() );
    output_jfs->setStore( output_jfsAux.release() );

    // Create jet copy
    xAOD::Jet* output_jet = new xAOD::Jet;
    output_jets->push_back(output_jet);
    *output_jet=*jet;

    // Create new BTagging object
    xAOD::BTagging* output_btag = new xAOD::BTagging;
    output_btags->push_back(output_btag);

    // Link the BTagging objects to the corresponding Jet
    ElementLink<xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement( *output_btags.get(), output_btag );
    output_jet->setBTaggingLink( linkBTagger );

    // Get PV
    const xAOD::Vertex* primaryVertex = *pv;

    // Perform track association
    StatusCode sc;
    if (!m_bTagTrackAssocTool->empty()) {
      std::vector<xAOD::Jet*> jetsList;
      jetsList.push_back(output_jet);

      // ==========================================
      // There is need of some work in order to accomodate the new track to jet associator interface
      // Make a copy container
      std::unique_ptr< xAOD::TrackParticleContainer > toBeSaved( new xAOD::TrackParticleContainer() );
      std::unique_ptr< xAOD::AuxContainerBase > toBeSavedAux( new xAOD::AuxContainerBase() );
      toBeSaved->setStore( toBeSavedAux.release() );

      for ( const xAOD::TrackParticle *particle : *tp->get() ) {
	xAOD::TrackParticle *newParticle = new xAOD::TrackParticle();
	toBeSaved->push_back(newParticle);
	*newParticle = *particle;
      }

      // Create Element Link
      std::vector< ElementLink< xAOD::IParticleContainer > > toBeUsed;  
      for ( const auto& particle : *toBeSaved.get() ) { 
	ElementLink< xAOD::IParticleContainer > cellLink; 
	cellLink.toContainedElement( *toBeSaved.get(),particle ); 
	toBeUsed.push_back( cellLink );   
      }

      // Save this copy
      if ( evtStore()->contains< xAOD::TrackParticleContainer >("TrigBtagEmulationTool_RetaggingTracks") )
	CHECK( evtStore()->overwrite( toBeSaved.release(),"TrigBtagEmulationTool_RetaggingTracks" ) );
      else 
	CHECK( evtStore()->record( toBeSaved.release(),"TrigBtagEmulationTool_RetaggingTracks" ) );

      output_jet->auxdata< std::vector< ElementLink< xAOD::IParticleContainer > > >("MatchedTracks") = toBeUsed;      
      // ==========================================

      sc = (*m_bTagTrackAssocTool)->BTagTrackAssociation_exec(&jetsList);

    }
    else {
      ATH_MSG_WARNING( "#BTAG# Empty track association tool" );
    }
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "#BTAG# Failed to associate tracks to jet" );
      return sc;
    }

    // Execute secondary vertexing
    sc = (*m_bTagSecVtxTool)->BTagSecVtx_exec(*output_jet, output_btag, output_svs.get(), output_jfs.get(), primaryVertex);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "#BTAG# Failed to reconstruct sec vtx" );
      return sc;
    }

    // Tag jet
    sc = (*m_bTagTool)->tagJet(*output_jet, output_btag, primaryVertex);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "#BTAG# Failed in taggers call" );
      return sc;
    }

    // Get weights
    double mv2c10=-1000;
    double mv2c20=-1000;
    output_btag->variable<double>("MV2c10","discriminant",mv2c10);
    output_btag->variable<double>("MV2c20","discriminant",mv2c20);

    // Save weights
    (*out)->weights( "MV2c10",mv2c10 );    
    (*out)->weights( "MV2c20",mv2c20 );

    // Increment
    pv++; tp++; out++;
  }
#endif

  return StatusCode::SUCCESS;
}

StatusCode JetManager::retagOnline() { return StatusCode::SUCCESS; }
std::vector< std::unique_ptr< TrigBtagEmulationJet > > JetManager::getJets() { 

  std::vector< std::unique_ptr< TrigBtagEmulationJet > > output;
  for ( std::unique_ptr< TrigBtagEmulationJet >& el : m_outputJets )
    output.push_back( std::unique_ptr< TrigBtagEmulationJet >( new TrigBtagEmulationJet(*el) ) );

  return output;
}


JetManager& JetManager::merge(const std::unique_ptr< JetManager >& other) { return merge( other->m_jet_Containers ); }
JetManager& JetManager::merge(const std::unique_ptr< JetManager >& other,double minPt,double maxPt) { return merge( other->m_jet_Containers,minPt,maxPt ); }
JetManager& JetManager::merge( std::unique_ptr< xAOD::JetContainer >& jets, double minPt, double maxPt) {
  for ( const xAOD::Jet* jet : *jets.get() ) {
    std::unique_ptr< TrigBtagEmulationJet > backupJet( new TrigBtagEmulationJet( msg(),jet ) );
    backupJet->weights( "MV2c10" ,-1 );
    backupJet->weights( "MV2c20" ,-1 );
    backupJet->weights( "IP3DSV1",-1000 );
    backupJet->weights( "COMB"   ,-1000 );

    bool isUnique = true;
    for (unsigned int index(0); index < m_outputJets.size(); index++)
      if ( m_outputJets.at(index)->pt() == backupJet->pt() &&
	   m_outputJets.at(index)->eta() == backupJet->eta() &&
	   m_outputJets.at(index)->phi() == backupJet->phi() ) 
	{isUnique = false; break;}

    if (!isUnique) continue;
    if (backupJet->pt() < minPt) continue;

    if ( maxPt == 0 ) m_outputJets.push_back( std::move( backupJet ) );
    else if ( backupJet->pt() < maxPt ) m_outputJets.push_back( std::move( backupJet ) );

  }
  
  return *this;
}


// ********************************************** //

StatusCode JetManager::retrieveJetContainer() {
  // In case we want LVL1 Jet we shoud use jet RoIs
  const xAOD::JetRoIContainer *theJetRoIContainer = nullptr;
  const xAOD::JetContainer *theJetContainer = nullptr;
  // Get Jet Objects
  if ( std::get< EventElement::JET >( m_containers ) == "LVL1JetRoIs" ) { 
    // We are retrieving xAOD::JetRoI object
    if ( evtStore()->retrieve(theJetRoIContainer, std::get< EventElement::JET >( m_containers) ).isFailure() ||
	 theJetRoIContainer == nullptr ) {
      ATH_MSG_ERROR( "CANNOT RETRIEVE JET-ROI CONTAINER 'LVL1JetRoIs' " );
      return StatusCode::FAILURE;
    }
    
    for ( const xAOD::JetRoI *jetRoI : *theJetRoIContainer ) {
      bool unique = true;
      for ( const xAOD::JetRoI *storedJetRoI : *m_jetRoI_Containers.get() ) 
	if ( jetRoI->et4x4() == storedJetRoI->et4x4() &&
	     jetRoI->et8x8() == storedJetRoI->et8x8() &&
	     jetRoI->eta() == storedJetRoI->eta() &&
	     jetRoI->phi() == storedJetRoI->phi() ) 
	  unique = false;

      if (unique) { 
	xAOD::JetRoI *toBeAdded = new xAOD::JetRoI();
	m_jetRoI_Containers->push_back( toBeAdded );
	*toBeAdded = *jetRoI;
      }
    }

    jetCopy( m_jetRoI_Containers );
  } else { 
    // Check if the input chain fired. Little protection against raw jets container : in this case we do not have to check the input chain
    if ( std::get< EventElement::JET >( m_containers) != "HLT_xAOD__JetContainer_a4tcemsubjesISFS" && 
	 std::get< EventElement::JET >( m_containers) != "HLT_xAOD__JetContainer_a4tcemsubjesFS" && 
	 not m_trigDec->isPassed( m_chain ) ) return StatusCode::SUCCESS;
    const std::string ilContainer = std::get< EventElement::JET >( m_containers);

    // We are retrieving xAOD::Jet object
    if ( evtStore()->retrieve(theJetContainer, std::get< EventElement::JET >( m_containers) ).isFailure() ||
	 theJetContainer == nullptr ) {
      ATH_MSG_ERROR( "CANNOT RETRIEVE JET CONTAINER '" << std::get< EventElement::JET >( m_containers ) << "' " );
      return StatusCode::FAILURE;
    }

    for ( const xAOD::Jet *jet : *theJetContainer ) {
      bool unique = true;
      for ( const xAOD::Jet *storedJet : *m_jet_Containers.get() )
	if ( jet->p4().Et() == storedJet->p4().Et() &&
	     jet->eta() == storedJet->eta() &&
	     jet->phi() == storedJet->phi() ) 
	  unique = false;

      if ( unique ) {
	xAOD::Jet *toBeAdded = new xAOD::Jet();
	m_jet_Containers->push_back( toBeAdded );
	*toBeAdded = *jet;
      }
    }

    jetCopy( m_jet_Containers );
  }
  
  return StatusCode::SUCCESS;
}


StatusCode JetManager::retrieveByContainer() {
  clear();
  ATH_MSG_DEBUG( "Retrieving via Container ... " );

  // Check what container has been specified
  if ( std::get< EventElement::JET >( m_containers ).empty() ) {
    ATH_MSG_ERROR( "INPUT JET CONTAINER HAS NOT BEEN SPECIFIED. CANNOT RETRIEVE VIA CONTAINER." );
    return StatusCode::FAILURE;
  }
  else if ( std::get< EventElement::BTAG >( m_containers ).empty() ) 
    return  retrieveJetContainer() ;

  // If Both jet abd btagging containers have been defined, we need to match them together
  // Check if 'chain' has fired. In case not, do nothing and return.
  if (!m_trigDec->isPassed(m_chain)) 
    return StatusCode::SUCCESS;


  // Retrieve Jet container
  const xAOD::JetContainer *theJetContainer = nullptr;
  if( evtStore()->retrieve(theJetContainer,std::get< EventElement::JET >(m_containers)).isFailure() || theJetContainer == nullptr) {
    ATH_MSG_ERROR( "CANNOT RETRIEVE JET CONTAINER '" << std::get< EventElement::JET >(m_containers) << "'" );
    return StatusCode::FAILURE;
  }
  // Retrieve Btagging container
  const xAOD::BTaggingContainer *theBTaggingContainer = nullptr;
  if ( evtStore()->retrieve(theBTaggingContainer,std::get< EventElement::BTAG >(m_containers)).isFailure() || theBTaggingContainer == nullptr ) {
    ATH_MSG_ERROR( "CANNOT RETRIEVE B-TAGGING CONTAINER '" << std::get< EventElement::BTAG >(m_containers) << "'" );
    return StatusCode::FAILURE;
  }


  // Loop on BTagging objects
  for (const xAOD::BTagging *bjet : *theBTaggingContainer) {
    // Check if BTagging object is of the required type 
    bool selectOffline  = (m_chain.find("off")!=std::string::npos);
    const double mv2c10 = Trig::retrieveAuxDataConst< double >( bjet,"MV2c10_discriminant",0. );
    const double mv2c20 = Trig::retrieveAuxDataConst< double >( bjet,"MV2c20_discriminant",0. );
    if ((selectOffline && mv2c20==0 && mv2c10==0) || (!selectOffline && (mv2c20!=0 || mv2c10!=0)) ) continue;

    // Get address of linked Jet and type of link  
    bool goodLink=false;
    const xAOD::Jet* btaggedJet = nullptr;
    // First Method for retrieving jet-btag link 
    try {
      if ( bjet->isAvailable<std::vector<ElementLink<xAOD::JetContainer> > >("BTagBtagToJetAssociator") ) {
	const auto &jetLink = Trig::retrieveAuxDataConst< std::vector< ElementLink< xAOD::JetContainer > > >( bjet,"BTagBtagToJetAssociator" );
	if ( jetLink.size() > 0 && jetLink.at(0).isValid() ) {
	    btaggedJet = *jetLink.at(0);
	    goodLink=true;
	}
      }
    } catch (...) { }
    // Second Method for retrieving jet-btag link 
    try {
      if ( bjet->isAvailable<std::vector<ElementLink<xAOD::IParticleContainer> > >("BTagBtagToJetAssociator")) {
	const auto &jetLink = Trig::retrieveAuxDataConst< std::vector< ElementLink< xAOD::IParticleContainer > > >( bjet,"BTagBtagToJetAssociator" );
	if ( jetLink.size() > 0 && jetLink.at(0).isValid() ) {
	    btaggedJet = static_cast<const xAOD::Jet*>( *jetLink.at(0) );
	    goodLink=true;
	}
      }
    } catch (...) { }


    if (goodLink) {
      // Check if the linked Jet is in the requested container
      // Link is not sotred for GSC jets, but it is for SPLIT jets. Thus we will need to make a GSC-SPLIT jet dR matching first.
      bool isGSCchain = std::get< EventElement::JET >( m_containers ).find("GSC")!=std::string::npos;

      // First check if the btagged jet is present in the jets retrieved by the container
      const xAOD::Jet *matchedJet = nullptr;

      bool isJetPresent = false;
      for ( const xAOD::Jet *theJet : *theJetContainer ) {
	
	if ( ( !isGSCchain && btaggedJet == theJet ) || // For non-GSC chains check if the btagged and container jets are the same 
	     ( isGSCchain && matchedSPLITjet( btaggedJet,theJet ) ) ) { // For GSC chains check if the container jets satisfy the dR matching with the btagged jet
	  matchedJet = theJet;
	  isJetPresent = true;
	  break;
	}

      }
      
      // Check if the linked Jet has already been found
      bool isJetUnique = true;
      for ( const xAOD::Jet *j : *m_jet_Containers.get() )
	if( matchedJet->p4().Et() == j->p4().Et() &&
	    matchedJet->eta() == j->eta() &&
	    matchedJet->phi() == j->phi() ) 
	  isJetUnique = false;

      // Save Jet and BTagging objects if jet is found and unique
      if (isJetPresent && isJetUnique) {
	xAOD::Jet *toBeAdded_Jet = new xAOD::Jet();
	m_jet_Containers->push_back( toBeAdded_Jet );
	*toBeAdded_Jet = *matchedJet;

	xAOD::BTagging *toBeAdded_Btag = new xAOD::BTagging();
	m_btagging_Containers->push_back( toBeAdded_Btag );
	*toBeAdded_Btag = *bjet;
      }
    }

  }
  
  jetCopy( m_jet_Containers );
  return StatusCode::SUCCESS;
}

bool JetManager::matchedSPLITjet(const xAOD::Jet* splitJet,const xAOD::Jet* gscJet) {
  double Deta = fabs( splitJet->eta() - gscJet->eta() );
  double Dphi = fabs( TVector2::Phi_mpi_pi( splitJet->phi() - gscJet->phi() ) );

  double dR = sqrt( pow(Deta,2) + pow(Dphi,2) );
  return dR < 0.05;
}

//**********************************************************************

template<typename T,typename U>
T Trig::retrieveAuxData( const U * storable,const std::string& name, T defaultValue ) {
  if ( storable->template isAvailable<T>( name.c_str() ) )
    return storable->template auxdata<T>( name.c_str() );
  return defaultValue;
}

template<typename T,typename U>
const T Trig::retrieveAuxDataConst( const U * storable,const std::string& name, const T defaultValue ) {
  if ( storable->template isAvailable<T>( name.c_str() ) )
    return storable->template auxdataConst<T>( name.c_str() );
  return defaultValue;
}

//**********************************************************************   

