/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaRec/topoEgammaBuilder.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IToolSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

#include "egammaInterfaces/IegammaBaseTool.h" 
#include "ElectronPhotonSelectorTools/IEGammaAmbiguityTool.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IEMBremCollectionBuilder.h"
#include "egammaInterfaces/IEMVertexBuilder.h"
#include "egammaUtils/egammaDuplicateRemoval.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

//Supercluster interfaces.
#include "egammaInterfaces/IegammaTopoClusterCopier.h"
#include "egammaInterfaces/IelectronSuperClusterBuilder.h"
#include "egammaInterfaces/IphotonSuperClusterBuilder.h"

// INCLUDE GAUDI HEADER FILES:
#include <algorithm> 
#include <cmath>

using CLHEP::MeV;
using CLHEP::GeV;

topoEgammaBuilder::topoEgammaBuilder(const std::string& name, 
				     ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_doTrackMatching(true),
  m_doConversions(true),
  m_timingProfile(0){

  //Containers
  declareProperty("ElectronOutputName",
		  m_electronOutputName="ElectronContainer",
		  "Name of Electron Connainer to be created");
  
  declareProperty("PhotonOutputName",
		  m_photonOutputName="PhotonContainer",
		  "Name of Photon Container to be created");

  declareProperty("InputTopoClusterContainerName",
		  m_inputTopoClusterContainerName = "egammaTopoCluster",
		  "Name of input cluster container");


  declareProperty("egammaRecContainer",
		  m_egammaRecContainerName="egammaRecCollection",
		  "Output container for egammaRec objects");

  declareProperty("ElectronSuperClusterRecContainerName",
		  m_electronSuperClusterRecContainerName="ElectronSuperRecCollection",
		  "Input container for electron  Super Cluster  egammaRec objects");

  declareProperty("PhotonSuperClusterRecContainerName",
		  m_photonSuperClusterRecContainerName="PhotonSuperRecCollection",
		  "Input container for electron  Super Cluster  egammaRec objects");

  // Handles of tools
    
  declareProperty("egammaTools", m_egammaTools,
		  "Tools for dressing electrons and photons");
  
  declareProperty("ElectronTools", m_electronTools,
		  "Tools for dressing ONLY electrons");

  declareProperty("PhotonTools", m_photonTools,
		  "Tools for dressing ONLY photons");


  // Tool handles to build superclusters
  declareProperty("TopoClusterCopier",
		  m_egammaTopoClusterCopier,
		  "Tool to build egamma Topo Clusters");

  declareProperty("electronSuperClusterBuilder",
		  m_electronSuperClusterBuilder,
		  "Tool to build superclusters");

  declareProperty("photonSuperClusterBuilder",
		  m_photonSuperClusterBuilder,
		  "Tool to build superclusters");

  // Handle of ambiguity tool
  declareProperty("AmbiguityTool", m_ambiguityTool,
		  "Handle of ambiguity tool");

  // Handle of TrackMatchBuilder
  declareProperty("TrackMatchBuilderTool", m_trackMatchBuilder,
		  "Handle of TrackMatchBuilder");

  // Handle of Conversion Builder
  declareProperty("ConversionBuilderTool",m_conversionBuilder,
		  "Handle of Conversion Builder");

  //Handle of the BremcollectionBuilder tool
  declareProperty("BremCollectionBuilderTool",  
		  m_BremCollectionBuilderTool, 
		  "Handle of the Brem Collection builder tool"); 

  // Handle of vertex builder
  declareProperty("VertexBuilder", m_vertexBuilder, "Handle of VertexBuilder");
  
  // All booleans
  // Boolean to do Brem collection Building
  declareProperty("doBremCollection",m_doBremCollection= true,
		  "Boolean to do Brem collection building");

  // Boolean to do track matching
  declareProperty("doTrackMatching",m_doTrackMatching= true,
		  "Boolean to do track matching (and conversion building)");

  // Boolean to do conversion reconstruction
  declareProperty("doConversions",m_doConversions= true,
		  "Boolean to do conversion building / matching");

  // Boolean to dump content of each object
  declareProperty("Dump",m_dump=false,
		  "Boolean to dump content of each object");
}

// ================================================================
topoEgammaBuilder::~topoEgammaBuilder()
{  
  // destructor
}

// =================================================================
StatusCode topoEgammaBuilder::initialize()
{
  // initialize method

  ATH_MSG_DEBUG("Initializing topoEgammaBuilder");

  //////////////////////////////////////////////////
  //
  CHECK(RetrieveEGammaTopoClusterCopier());
  CHECK(RetrieveElectronSuperClusterBuilder());
  CHECK(RetrievePhotonSuperClusterBuilder());

  //
  //////////////////////////////////////////////////
  // retrieve track match builder
  CHECK( RetrieveEMTrackMatchBuilder() );
  // retrieve conversion builder
  CHECK(  RetrieveEMConversionBuilder() );
  // retrieve tool to build GSF tracks
  CHECK( RetrieveBremCollectionBuilder() );
  // retrieve tool to build ID conversion vertices
  CHECK( RetrieveVertexBuilder() );
  // retrieve ambiguity tool
  CHECK( RetrieveAmbiguityTool() );
  
  ATH_MSG_DEBUG("Retrieving " << m_egammaTools.size() << " tools for egamma objects");
  CHECK( RetrieveTools(m_egammaTools) );
  ATH_MSG_DEBUG("Retrieving " << m_electronTools.size() << " tools for electrons");
  CHECK( RetrieveTools(m_electronTools) );
  ATH_MSG_DEBUG("Retrieving " << m_photonTools.size() << " tools for photons");
  CHECK( RetrieveTools(m_photonTools) );

  // retrieve timing profile
  CHECK( service("ChronoStatSvc",m_timingProfile) );

  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode topoEgammaBuilder::RetrieveTools(ToolHandleArray<IegammaBaseTool>& tools){
  for (const auto tool : tools){
    if ( tool.retrieve().isFailure() ){
      ATH_MSG_FATAL( "Could not get tool: " << tool);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Retrieved Tool " << tool); 
  }
  return StatusCode::SUCCESS;
}
// ====================================================================
StatusCode topoEgammaBuilder::RetrieveEGammaTopoClusterCopier(){
  if (m_egammaTopoClusterCopier.empty()) {
    ATH_MSG_ERROR("Super Cluster Builder tooo  is empty");
    return StatusCode::FAILURE;
  }
  if(m_egammaTopoClusterCopier.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<< m_egammaTopoClusterCopier);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool " << m_egammaTopoClusterCopier); 
  return StatusCode::SUCCESS;  
}
// ====================================================================
StatusCode topoEgammaBuilder::RetrieveElectronSuperClusterBuilder(){
  if (m_electronSuperClusterBuilder.empty()) {
    ATH_MSG_ERROR("Super Cluster Builder tool  is empty");
    return StatusCode::FAILURE;
  }
  if(m_electronSuperClusterBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<< m_electronSuperClusterBuilder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool " << m_electronSuperClusterBuilder); 
  
  return StatusCode::SUCCESS;  

}
// ====================================================================
StatusCode topoEgammaBuilder::RetrievePhotonSuperClusterBuilder(){
  if (m_photonSuperClusterBuilder.empty()) {
    ATH_MSG_ERROR("Super Cluster Builder tool  is empty");
    return StatusCode::FAILURE;
  }
  if(m_photonSuperClusterBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<< m_photonSuperClusterBuilder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool " << m_photonSuperClusterBuilder); 
  
  return StatusCode::SUCCESS;  

}

// ====================================================================
StatusCode topoEgammaBuilder::RetrieveAmbiguityTool(){
  // retrieve Ambiguity tool
  if (m_ambiguityTool.empty()) {
    ATH_MSG_ERROR("EMAmbiguityTool is empty");
    return StatusCode::FAILURE;
  }

  if((m_ambiguityTool.retrieve()).isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_ambiguityTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_ambiguityTool);

  return StatusCode::SUCCESS;
}
// ====================================================================
StatusCode topoEgammaBuilder::RetrieveEMTrackMatchBuilder(){
  // retrieve EMTrackMatchBuilder tool
  if (!m_doTrackMatching) {
    return StatusCode::SUCCESS;
  }

  if (m_trackMatchBuilder.empty()) {
    ATH_MSG_ERROR("EMTrackMatchBuilder is empty, but track matching is enabled");
    return StatusCode::FAILURE;
  } 
  
  if(m_trackMatchBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_trackMatchBuilder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_trackMatchBuilder); 

  return StatusCode::SUCCESS;
}
// ====================================================================
StatusCode topoEgammaBuilder::RetrieveEMConversionBuilder(){

  if (!m_doTrackMatching || !m_doConversions) {
    return StatusCode::SUCCESS;
  }

  if (m_conversionBuilder.empty()) {
    ATH_MSG_ERROR("EMConversionBuilder is empty");
    return StatusCode::FAILURE;
  } 
  
  if(m_conversionBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_conversionBuilder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_conversionBuilder); 
  
  return StatusCode::SUCCESS;
}

StatusCode topoEgammaBuilder::RetrieveBremCollectionBuilder(){
  if (!m_doBremCollection ) {
    return StatusCode::SUCCESS;
  }

  if (m_BremCollectionBuilderTool.empty()) {
    ATH_MSG_ERROR("BremCollectionBuilderTool is empty");
    return StatusCode::FAILURE;
  }

  if(m_BremCollectionBuilderTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_BremCollectionBuilderTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_BremCollectionBuilderTool);  
  
  return StatusCode::SUCCESS;
}
// ====================================================================
StatusCode topoEgammaBuilder::RetrieveVertexBuilder(){
  if (!m_doConversions){
    return StatusCode::SUCCESS;
  }

  if (m_vertexBuilder.empty()) {
    ATH_MSG_ERROR("VertexBuilder is empty");
    return StatusCode::FAILURE;
  }
  
  if(m_vertexBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_vertexBuilder);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_vertexBuilder);
  
  return StatusCode::SUCCESS;
}


// ====================================================================
StatusCode topoEgammaBuilder::finalize()
{
  // finalize method
  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode topoEgammaBuilder::execute()
{
  // athena execute method

  ATH_MSG_DEBUG("Executing topoEgammaBuilder");
  
  // Chrono name for each Tool
  std::string chronoName;

  xAOD::ElectronContainer*    electronContainer    = new xAOD::ElectronContainer();
  xAOD::ElectronAuxContainer* electronAuxContainer = new xAOD::ElectronAuxContainer();
  electronContainer->setStore( electronAuxContainer );
  
  if ( evtStore()->record(electronContainer, m_electronOutputName).isFailure() ||
       evtStore()->record(electronAuxContainer, m_electronOutputName + "Aux.").isFailure())
    {
      ATH_MSG_ERROR("Could not record electron container or its aux container");
      return StatusCode::FAILURE;
    }       

  xAOD::PhotonContainer*    photonContainer    = new xAOD::PhotonContainer();
  xAOD::PhotonAuxContainer* photonAuxContainer = new xAOD::PhotonAuxContainer();
  photonContainer->setStore( photonAuxContainer );
  
  if ( evtStore()->record(photonContainer, m_photonOutputName).isFailure() ||
       evtStore()->record(photonAuxContainer, m_photonOutputName + "Aux.").isFailure()){
    ATH_MSG_ERROR("Could not record photon container or its aux container");
    return StatusCode::FAILURE;
  }       
    
  //First run the egamma Topo Copier that will select copy over cluster of interest to egammaTopoCluster
  chronoName = this->name()+"_"+m_egammaTopoClusterCopier->name();         
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
  //
  CHECK(m_egammaTopoClusterCopier->contExecute());
  //
  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
  
  //Then retrieve them 
  const xAOD::CaloClusterContainer *topoclusters = 0;
  if (evtStore()->retrieve(topoclusters, m_inputTopoClusterContainerName).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve cluster container " << m_inputTopoClusterContainerName);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved input cluster container " << m_inputTopoClusterContainerName);
  }

  //Build the initial egamma Rec objects for all copied Topo Clusters
  EgammaRecContainer* egammaRecs = new EgammaRecContainer();
  if (evtStore()->record(egammaRecs, m_egammaRecContainerName).isFailure()){
    ATH_MSG_ERROR("Could not record egammaRecContainer");
    return StatusCode::FAILURE;
  }
  
  for (unsigned int i(0); i < topoclusters->size(); i++) {
    const ElementLink< xAOD::CaloClusterContainer > clusterLink( *topoclusters, i );
    const std::vector< ElementLink<xAOD::CaloClusterContainer> > ClusterLink {clusterLink};    
    egammaRec *egRec = new egammaRec();
    egRec->setCaloClusters( ClusterLink );
    egammaRecs->push_back( egRec );
  }
  

  //Do the track refitting.
  if (m_doBremCollection){ 
    ATH_MSG_DEBUG("Running BremCollectionBuilder");  
    //
    std::string chronoName = this->name()+"_"+m_BremCollectionBuilderTool->name();         
    if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
    //
    if (m_BremCollectionBuilderTool->contExecute().isFailure()){
      ATH_MSG_ERROR("Problem executing " << m_BremCollectionBuilderTool);
      return StatusCode::FAILURE;  
    }
    //
    if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
  }
  ///Append track Matching information
  chronoName = this->name()+"_"+m_trackMatchBuilder->name()+"_AllClusters";         
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
  for (auto egRec : *egammaRecs) {
    if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
      ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
      return StatusCode::FAILURE;
    }
  }
  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

  ///Append vertex matching /conversion information 
  if (m_doConversions) {
    ATH_MSG_DEBUG("Running VertexBuilder");  
    //
    std::string chronoName = this->name()+"_"+m_vertexBuilder->name()+"_AllClusters";         
    if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
    //
    if (m_vertexBuilder->contExecute().isFailure()){
      ATH_MSG_ERROR("Problem executing " << m_vertexBuilder);
      return StatusCode::FAILURE;  
    }
    //
    if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

    ATH_MSG_DEBUG("Running ConversionBuilder");  
    //
    chronoName = this->name()+"_"+m_conversionBuilder->name()+"_AllClusters";         
    if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
    //
    if (m_conversionBuilder->contExecute().isFailure()){
      ATH_MSG_ERROR("Problem executing " << m_conversionBuilder);
      return StatusCode::FAILURE;  
    }
    //
    if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
  }


  ////////////////////////////////////////////////
  //Now all info should be added  the initial topoClusters
  //Build SuperClusters
  
  //Electron superclusters Builder
  chronoName = this->name()+"_"+m_electronSuperClusterBuilder->name();         
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
  //
  CHECK(m_electronSuperClusterBuilder->execute());
  //
  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

  //Track Match the final electron SuperClusters
  EgammaRecContainer *electronSuperRecs(0);
  if( evtStore()->contains<EgammaRecContainer>( m_electronSuperClusterRecContainerName)) { 
    //Else retrieve them 
    CHECK(evtStore()->retrieve(electronSuperRecs,  m_electronSuperClusterRecContainerName));
    ATH_MSG_DEBUG("Size of "  <<m_electronSuperClusterRecContainerName << " : " << electronSuperRecs->size());

    //Redo track matching given the super cluster
    chronoName = this->name()+"_"+m_trackMatchBuilder->name()+"_FinalClusters";         
    if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
    for (auto egRec : *electronSuperRecs) {
      if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
	ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
	return StatusCode::FAILURE;
      }
    }
    if(m_timingProfile) m_timingProfile->chronoStop(chronoName);    
  }
  ////END OF ELECTRONS

  //Photon superclusters Builder
  chronoName = this->name()+"_"+m_photonSuperClusterBuilder->name();         
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
  //
  CHECK(m_photonSuperClusterBuilder->execute());
  //
  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

  EgammaRecContainer *photonSuperRecs(0);
  if( evtStore()->contains<EgammaRecContainer>( m_photonSuperClusterRecContainerName)) { 
    //Else retrieve them 
    CHECK(evtStore()->retrieve(photonSuperRecs,  m_photonSuperClusterRecContainerName));
    ATH_MSG_DEBUG("Size of "  <<m_photonSuperClusterRecContainerName << " : " << photonSuperRecs->size());

    //Redo conversion matching given the super cluster
    if (m_doConversions) {
      chronoName = this->name()+"_"+m_conversionBuilder->name()+"_FinalClusters";         
      if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
      for (auto egRec : *photonSuperRecs) {
	if (m_conversionBuilder->executeRec(egRec).isFailure()){
	  ATH_MSG_ERROR("Problem executing conversioBuilder on photonSuperRecs");
	  return StatusCode::FAILURE;
	}
      }
      if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
    }
  }
  ///END OF PHOTONS

  //For now naive double loops bases on the seed (constituent at position 0)
  //For ambiguity. Probably we could mark in one pass , for now naive solution
  
  //These should be the CaloCalTopo links for the clustes. the 0 should be the seed (always there) 
  const static SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  
  ATH_MSG_DEBUG("Build  "<< electronSuperRecs->size() << " electron Super Clusters"); 
  ATH_MSG_DEBUG("Build  "<< photonSuperRecs->size() << " photon Super Clusters"); 


  //Build xAOD::Electron objects
  for (const auto& electronRec : *electronSuperRecs) {
    unsigned int author = xAOD::EgammaParameters::AuthorElectron;
    xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::electron;
    for (const auto& photonRec : *photonSuperRecs) {
    //See if the same seed (0 element in the constituents) seed also a photon
      if(caloClusterLinks(*(electronRec->caloCluster())).at(0).index() ==
	 caloClusterLinks(*(photonRec->caloCluster())).at(0).index()){
	//If yes run ambiguity
	ATH_MSG_DEBUG("Running AmbiguityTool for electron");
	
	author = m_ambiguityTool->ambiguityResolve(photonRec->caloCluster(),
						   photonRec->vertex(),
						   electronRec->trackParticle(),
						   type);
	

	break;
      }
    }
    //Fill each electron
    if (author == xAOD::EgammaParameters::AuthorElectron || 
        author == xAOD::EgammaParameters::AuthorAmbiguous){
      ATH_MSG_DEBUG("getElectron");
      if ( !getElectron(electronRec, electronContainer, author,type) )
	return StatusCode::FAILURE;
    }
  }
  ////////////////////////////////
  //Build xAOD::Photon objects.
  for (const auto& photonRec : *photonSuperRecs) {
    unsigned int author = xAOD::EgammaParameters::AuthorPhoton;
    xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::photon;
    //See if the same seed (0 element in the constituents) seed also an electron
    for (const auto& electronRec : *photonSuperRecs) {
      if(caloClusterLinks(*(photonRec->caloCluster())).at(0).index() ==
	 caloClusterLinks(*(electronRec->caloCluster())).at(0).index()){
	//If yes run ambiguity	
	ATH_MSG_DEBUG("Running AmbiguityTool for photon");
  
	author = m_ambiguityTool->ambiguityResolve(photonRec->caloCluster(),
						   photonRec->vertex(),
						   electronRec->trackParticle(),
						   type);
	break;
      }
    }
    //Fill each photon
    if (author == xAOD::EgammaParameters::AuthorPhoton || 
        author == xAOD::EgammaParameters::AuthorAmbiguous){
      ATH_MSG_DEBUG("getPhoton");
      if ( !getPhoton(photonRec, photonContainer, author,type) )
	return StatusCode::FAILURE;
    }
  }
  /////////////////////////////////////
  // Call tools
  for (const auto& tool : m_egammaTools){
    CHECK( CallTool(tool, electronContainer, photonContainer) );
  }
  
  for (const auto& tool : m_electronTools){
    CHECK( CallTool(tool, electronContainer, 0) );
  }
  
  for (const auto& tool : m_photonTools){
    CHECK( CallTool(tool, 0, photonContainer) );
  }

  ATH_MSG_DEBUG("Build  "<< electronContainer->size() << " electrons "); 
  ATH_MSG_DEBUG("Build  "<< photonContainer->size() << " photons"); 

  
  ATH_MSG_DEBUG("execute completed successfully");

  return StatusCode::SUCCESS;
}

// =====================================================
StatusCode topoEgammaBuilder::CallTool(const ToolHandle<IegammaBaseTool>& tool, 
				       xAOD::ElectronContainer *electronContainer /* = 0*/, 
				       xAOD::PhotonContainer *photonContainer /* = 0*/)
{
  

  ATH_MSG_DEBUG("Executing tool on containers: " << tool );
  std::string chronoName = this->name()+"_"+tool->name();         
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
  
  if ( tool->contExecute().isFailure() )
    {
      ATH_MSG_ERROR("Problem executing tool on containers: " << tool);
      return StatusCode::FAILURE;
    }
  
  if (electronContainer){    
    ATH_MSG_DEBUG("Executing tool on electrons: " << tool );
    for (const auto& electron : *electronContainer){
      if (tool->execute(electron).isFailure() ){
	ATH_MSG_ERROR("Problem executing tool on electrons: " << tool);
	return StatusCode::FAILURE;
      }
    }
  }
  
  if (photonContainer){
    ATH_MSG_DEBUG("Executing tool on photons: " << tool );
    for (const auto& photon : *photonContainer){
      if (tool->execute(photon).isFailure() ){
	ATH_MSG_ERROR("Problem executing tool on photons: " << tool);
	return StatusCode::FAILURE;
      }
    }
  }
  
  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
  return StatusCode::SUCCESS;
}
// =====================================================
bool topoEgammaBuilder::getElectron(const egammaRec* egRec, 
				    xAOD::ElectronContainer *electronContainer,
				    const unsigned int author,
				    const uint8_t type)
{  
 
  if (!egRec || !electronContainer) return false;
  
  xAOD::Electron *electron = new xAOD::Electron();
  electronContainer->push_back( electron );
  electron->setAuthor( author );

  const static SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*electron) = type;

  std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
  for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i){
    clusterLinks.push_back( egRec->caloClusterElementLink(i) );
  }
  electron->setCaloClusterLinks( clusterLinks );
  
  std::vector< ElementLink< xAOD::TrackParticleContainer > > trackLinks;
  for (unsigned int i = 0 ; i < egRec->getNumberOfTrackParticles(); ++i){
    trackLinks.push_back( egRec->trackParticleElementLink(i) );
  }
  electron->setTrackParticleLinks( trackLinks );
   
  electron->setCharge(electron->trackParticle()->charge());
  //Set DeltaEta, DeltaPhi , DeltaPhiRescaled
  float deltaEta = static_cast<float>(egRec->deltaEta(0));
  float deltaPhi = static_cast<float>(egRec->deltaPhi(0));
  float deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(0));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta0 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi0 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled0 );

  deltaEta = static_cast<float>(egRec->deltaEta(1));
  deltaPhi = static_cast<float>(egRec->deltaPhi(1));
  deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(1));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta1 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi1 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled1);

  deltaEta = static_cast<float>(egRec->deltaEta(2));
  deltaPhi = static_cast<float>(egRec->deltaPhi(2));
  deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(2));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta2 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi2 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled2);

  deltaEta = static_cast<float>(egRec->deltaEta(3));
  deltaPhi = static_cast<float>(egRec->deltaPhi(3));
  deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(3));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta3 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi3 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled3);

  float deltaPhiLast = static_cast<float>(egRec->deltaPhiLast ());
  electron->setTrackCaloMatchValue(deltaPhiLast,xAOD::EgammaParameters::deltaPhiFromLastMeasurement );
  
  return true;
}

// =====================================================
bool topoEgammaBuilder::getPhoton(const egammaRec* egRec,
				  xAOD::PhotonContainer *photonContainer,
				  const unsigned int author,
				  const uint8_t type)
{
  if (!egRec || !photonContainer) return false;
  
  xAOD::Photon *photon = new xAOD::Photon();
  photonContainer->push_back( photon );
  photon->setAuthor( author );
  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*photon)=type;

  // Transfer the links to the clusters
  std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
  for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i){
    clusterLinks.push_back( egRec->caloClusterElementLink(i) );
  }
  photon->setCaloClusterLinks( clusterLinks );

  // Transfer the links to the vertices  
  std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks;
  for (unsigned int i = 0 ; i < egRec->getNumberOfVertices(); ++i){
    vertexLinks.push_back( egRec->vertexElementLink(i) );
  }
  photon->setVertexLinks( vertexLinks );
  
  // Transfer deltaEta/Phi info
  float deltaEta = egRec->deltaEtaVtx(), deltaPhi = egRec->deltaPhiVtx();
  if (!photon->setVertexCaloMatchValue( deltaEta,
                                        xAOD::EgammaParameters::convMatchDeltaEta1) ){
    ATH_MSG_WARNING("Could not transfer deltaEta to photon");
    return false;
  }
  
  if (!photon->setVertexCaloMatchValue( deltaPhi,
                                        xAOD::EgammaParameters::convMatchDeltaPhi1) ){
      ATH_MSG_WARNING("Could not transfer deltaPhi to photon");
      return false;
  }
  return true;
}


