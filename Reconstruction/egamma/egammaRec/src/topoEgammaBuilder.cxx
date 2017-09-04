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
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

#include "egammaInterfaces/IegammaBaseTool.h" 
#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"
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

namespace{
  class smallChrono{
  public:
    smallChrono(IChronoStatSvc* timingProfile, const std::string& name): 
      m_time(timingProfile),
      m_name(name){
      if(m_time){
	m_time->chronoStart(m_name);
      }
    }
    ~smallChrono(){
      if(m_time){
	m_time->chronoStop(m_name);
      }
    }
  private:
    IChronoStatSvc* m_time;
    const std::string m_name;
  };
}


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

  // Boolean to do the conversion vertex collection Building
  declareProperty("doVertexCollection",m_doVertexCollection= true,
		  "Boolean to do conversion vertex collection building");

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
  //
  // retrieve EMConversionBuilder tool
  //  
  if (!m_doConversions) {
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

// ====================================================================
StatusCode topoEgammaBuilder::RetrieveBremCollectionBuilder(){
  //
  // retrieve bremfitter tool
  //
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
  //
  // retrieve vertex builder for ID conversions
  //
  if (!m_doVertexCollection){ 
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
StatusCode topoEgammaBuilder::finalize(){
  // finalize method
  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode topoEgammaBuilder::execute(){
  // athena execute method

  ATH_MSG_DEBUG("Executing topoEgammaBuilder");
  
  // Chrono name for each Tool
  std::string chronoName;

  xAOD::ElectronContainer*    electronContainer    = new xAOD::ElectronContainer();
  xAOD::ElectronAuxContainer* electronAuxContainer = new xAOD::ElectronAuxContainer();
  electronContainer->setStore( electronAuxContainer );
  
  if ( evtStore()->record(electronContainer, m_electronOutputName).isFailure() ||
       evtStore()->record(electronAuxContainer, m_electronOutputName + "Aux.").isFailure()){
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
  {
    smallChrono timer(m_timingProfile,this->name()+"_"+m_egammaTopoClusterCopier->name());
    CHECK(m_egammaTopoClusterCopier->contExecute());
  }
  
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
  
  for (size_t i(0); i < topoclusters->size(); i++) {
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
    smallChrono timer(m_timingProfile, this->name()+"_"+m_BremCollectionBuilderTool->name());
    if (m_BremCollectionBuilderTool->contExecute().isFailure()){
      ATH_MSG_ERROR("Problem executing " << m_BremCollectionBuilderTool);
      return StatusCode::FAILURE;  
    }
  }
  ///Append track Matching information
  if (m_doTrackMatching){
    smallChrono timer(m_timingProfile, this->name()+"_"+m_trackMatchBuilder->name()+"_AllClusters");
    for (auto egRec : *egammaRecs) {
      if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
	ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
	return StatusCode::FAILURE;
      }
    }
  }
  ///Append vertex matching /conversion information 
  if (m_doVertexCollection){ 
    ATH_MSG_DEBUG("Running VertexBuilder");  
    //
    smallChrono timer(m_timingProfile, this->name()+"_"+m_vertexBuilder->name()+"_AllClusters");
    if (m_vertexBuilder->contExecute().isFailure()){
      ATH_MSG_ERROR("Problem executing " << m_vertexBuilder);
      return StatusCode::FAILURE;  
    }
  }
  //Do the conversion matching
  if (m_doConversions){
    ATH_MSG_DEBUG("Running ConversionBuilder");  
    smallChrono timer(m_timingProfile, this->name()+"_"+m_conversionBuilder->name()+"_AllClusters");
    if (m_conversionBuilder->contExecute().isFailure()){
      ATH_MSG_ERROR("Problem executing " << m_conversionBuilder);
      return StatusCode::FAILURE;  
    }
  }
  //
  ////////////////////////////////////////////////
  //Now all info should be added  the initial topoClusters build SuperClusters
  //Electron superclusters Builder
  {
    smallChrono timer(m_timingProfile, this->name()+"_"+m_electronSuperClusterBuilder->name());
    CHECK(m_electronSuperClusterBuilder->execute());
  }

  //Track Match the final electron SuperClusters
  EgammaRecContainer *electronSuperRecs(0);
  if( evtStore()->contains<EgammaRecContainer>( m_electronSuperClusterRecContainerName)) { 
    CHECK(evtStore()->retrieve(electronSuperRecs,  m_electronSuperClusterRecContainerName));
    ATH_MSG_DEBUG("Size of "  <<m_electronSuperClusterRecContainerName << " : " << electronSuperRecs->size());
    
    if (m_doTrackMatching){
      smallChrono timer(m_timingProfile,this->name()+"_"+m_trackMatchBuilder->name()+"_FinalClusters");
      for (auto egRec : *electronSuperRecs) {
	if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
	  ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
	  return StatusCode::FAILURE;
	}
      }      
    }
  }
  //Photon superclusters Builder
  {
  smallChrono timer(m_timingProfile,this->name()+"_"+m_photonSuperClusterBuilder->name());  
  CHECK(m_photonSuperClusterBuilder->execute());
  }
  
  EgammaRecContainer *photonSuperRecs(0);
  if( evtStore()->contains<EgammaRecContainer>( m_photonSuperClusterRecContainerName)) { 
    //Else retrieve them 
    CHECK(evtStore()->retrieve(photonSuperRecs,  m_photonSuperClusterRecContainerName));
    ATH_MSG_DEBUG("Size of "  <<m_photonSuperClusterRecContainerName << " : " << photonSuperRecs->size());

    //Redo conversion matching given the super cluster
    if (m_doConversions) {
      smallChrono timer(m_timingProfile,this->name()+"_"+m_conversionBuilder->name()+"_FinalClusters");
      for (auto egRec : *photonSuperRecs) {
	if (m_conversionBuilder->executeRec(egRec).isFailure()){
	  ATH_MSG_ERROR("Problem executing conversioBuilder on photonSuperRecs");
	  return StatusCode::FAILURE;
	}
      }
    }
  }
  //
  //
  //For now naive double loops bases on the seed (constituent at position 0)
  //For ambiguity. Probably we could mark in one pass , for now naive solution
  //These should be the CaloCalTopo links for the clustes. the 0 should be the seed (always there) 
  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  
  ATH_MSG_DEBUG("Build  "<< electronSuperRecs->size() << " electron Super Clusters"); 
  ATH_MSG_DEBUG("Build  "<< photonSuperRecs->size() << " photon Super Clusters"); 
  //
  //Look at the constituents , for ambiguity resolution, for now based on the seed only
  //We could add secondaries cluster in this logic.
  //Also probably we could factor some common code.
  //-----------------------------------------------------------------
  //Build xAOD::Electron objects
  for (const auto& electronRec : *electronSuperRecs) {
    
    unsigned int author = xAOD::EgammaParameters::AuthorElectron;
    xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::electron;
    
    for (const auto& photonRec : *photonSuperRecs) {
      
      //See if the same seed (0 element in the constituents) seed also a photon
      if(caloClusterLinks(*(electronRec->caloCluster())).at(0)==
	 caloClusterLinks(*(photonRec->caloCluster())).at(0)){
	ATH_MSG_DEBUG("Running AmbiguityTool for electron");
	
	author = m_ambiguityTool->ambiguityResolve(electronRec->caloCluster(),
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
      if ( !getElectron(electronRec, electronContainer, author,type) ){
	return StatusCode::FAILURE;
      }
    }
  }

  //-----------------------------------------------------------------
  //Build xAOD::Photon objects.
  for (const auto& photonRec : *photonSuperRecs) {
    unsigned int author = xAOD::EgammaParameters::AuthorPhoton;
    xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::photon;
    
    //See if the same seed (0 element in the constituents) seed also an electron
    for (const auto& electronRec : *electronSuperRecs) {
      
    if(caloClusterLinks(*(photonRec->caloCluster())).at(0) ==
       caloClusterLinks(*(electronRec->caloCluster())).at(0)){
      ATH_MSG_DEBUG("Running AmbiguityTool for photon");
      
      author = m_ambiguityTool->ambiguityResolve(electronRec->caloCluster(),
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
      if ( !getPhoton(photonRec, photonContainer, author,type) ){
	return StatusCode::FAILURE;
      }
    }
  }
  
  //-----------------------------------------------------------------
  // Call tools
  for (auto& tool : m_egammaTools){
    CHECK( CallTool(tool, electronContainer, photonContainer) );
  }
  
  for (auto& tool : m_electronTools){
    CHECK( CallTool(tool, electronContainer, 0) );
  }
  
  for (auto& tool : m_photonTools){
    CHECK( CallTool(tool, 0, photonContainer) );
  }
  
  //---------------------------------------
  //Do the ambiguity Links
  //-----------------------------------------------------------------
  CHECK(doAmbiguityLinks (electronContainer,photonContainer));
  
  ATH_MSG_DEBUG("Build  "<< electronContainer->size() << " electrons "); 
  ATH_MSG_DEBUG("Build  "<< photonContainer->size() << " photons"); 
  ATH_MSG_DEBUG("execute completed successfully");
  
  return StatusCode::SUCCESS;
}

StatusCode topoEgammaBuilder::doAmbiguityLinks(xAOD::ElectronContainer *electronContainer, 
					       xAOD::PhotonContainer *photonContainer){
  
  ///Needs the same logic as the ambiguity after building the objects (make sure they are all valid)
  static const SG::AuxElement::Accessor<ElementLink<xAOD::EgammaContainer> > ELink ("ambiguityLink");
  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  ElementLink<xAOD::EgammaContainer> dummylink;
  for (size_t photonIndex=0; photonIndex < photonContainer->size() ; ++photonIndex) {    
    
    xAOD::Photon* photon = photonContainer->at(photonIndex); 
    ELink(*photon)=dummylink;
    
    if(photon->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
      continue;
    }      

    for (size_t electronIndex=0; electronIndex < electronContainer->size() ; ++electronIndex) {
      
      xAOD::Electron* electron = electronContainer->at(electronIndex); 
      if(electron->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
	continue;
      }
      
      if(caloClusterLinks(*(electron->caloCluster())).at(0) ==
	 caloClusterLinks(*(photon->caloCluster())).at(0)){
	ElementLink<xAOD::EgammaContainer> link (*electronContainer,electronIndex);
	ELink(*photon)=link;
	break;
      }
    }
  }
  for (size_t electronIndex=0; electronIndex < electronContainer->size() ; ++electronIndex) {    

    xAOD::Electron* electron = electronContainer->at(electronIndex); 
    ELink(*electron)=dummylink;
    if(electron->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
      continue;
    }      
    for (size_t photonIndex=0; photonIndex < photonContainer->size() ; ++photonIndex) {
      
      xAOD::Photon* photon = photonContainer->at(photonIndex); 
      if(photon->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
	continue;
      }
      
      if(caloClusterLinks(*(electron->caloCluster())).at(0) ==
	 caloClusterLinks(*(photon->caloCluster())).at(0)){
	ElementLink<xAOD::EgammaContainer> link (*photonContainer,photonIndex);
	ELink(*electron)=link;
	break;
      }
    }
  }
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------

// =====================================================
StatusCode topoEgammaBuilder::CallTool(ToolHandle<IegammaBaseTool>& tool, 
				       xAOD::ElectronContainer *electronContainer /* = 0*/, 
				       xAOD::PhotonContainer *photonContainer /* = 0*/){
  

  ATH_MSG_DEBUG("Executing tool on containers: " << tool );  
  smallChrono timer(m_timingProfile,this->name()+"_"+tool->name());  
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
  return StatusCode::SUCCESS;
}
// =====================================================
bool topoEgammaBuilder::getElectron(const egammaRec* egRec, 
				    xAOD::ElectronContainer *electronContainer,
				    const unsigned int author,
				    const uint8_t type){  
  
  if (!egRec || !electronContainer) return false;
  
  xAOD::Electron *electron = new xAOD::Electron();
  electronContainer->push_back( electron );
  electron->setAuthor( author );

  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*electron) = type;

  std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfClusters(); ++i){
    clusterLinks.push_back( egRec->caloClusterElementLink(i) );
  }
  electron->setCaloClusterLinks( clusterLinks );
  
  std::vector< ElementLink< xAOD::TrackParticleContainer > > trackLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfTrackParticles(); ++i){
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
				  const uint8_t type){
  if (!egRec || !photonContainer) return false;
  
  xAOD::Photon *photon = new xAOD::Photon();
  photonContainer->push_back( photon );
  photon->setAuthor( author );
  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*photon)=type;

  // Transfer the links to the clusters
  std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfClusters(); ++i){
    clusterLinks.push_back( egRec->caloClusterElementLink(i) );
  }
  photon->setCaloClusterLinks( clusterLinks );

  // Transfer the links to the vertices  
  std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfVertices(); ++i){
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


