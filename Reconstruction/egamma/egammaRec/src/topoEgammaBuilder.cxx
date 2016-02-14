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

#include "ElectronPhotonSelectorTools/IEGammaAmbiguityTool.h"
#include "egammaInterfaces/IegammaBaseTool.h" 
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IEMBremCollectionBuilder.h"
#include "egammaInterfaces/IEMVertexBuilder.h"

#include "egammaUtils/egammaDuplicateRemoval.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "StoreGate/StoreGateSvc.h"

//Supercluster interfaces.
#include "egammaInterfaces/IegammaTopoClusterMap.h"
#include "egammaInterfaces/IegammaSuperClusterBuilder.h"
#include "egammaInterfaces/IegammaClusterOverlapMarker.h"

// INCLUDE GAUDI HEADER FILES:
#include <algorithm> 
#include <math.h>

using CLHEP::MeV;
using CLHEP::GeV;

topoEgammaBuilder::topoEgammaBuilder(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_tracksName("InnerDetectorTrackParticles"),
  m_doTrackMatching(true),
  m_doConversions(true),
  m_timingProfile(0)
{

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //Supercluster-specific tools.

  // Tool to build topocluster map
  declareProperty("egammaTopoClusterMapToolName",
		  m_egammaTopoClusterMapToolName="egammaTopoClusterMap/emsofteTopoclustermap",
		  "Tool to build topocluster map");

  // Tool to build superclusters
  declareProperty("SuperClusterBuilder",
		  m_egammaSuperClusterBuilder,
		  "Tool to build superclusters");

  declareProperty("OverlapMarker",
		  m_egammaClusterOverlapMarker,
		  "Tool to do overlap marking");


  ////////////////////////////////////////////////////////////////////////////////////////////////
  
  // The following properties are specified at run-time
  // (declared in jobOptions file)
  
  declareProperty("ElectronOutputName",
		  m_electronOutputName="ElectronContainer",
		  "Name of Electron Connainer to be created");
  
  declareProperty("PhotonOutputName",
		  m_photonOutputName="PhotonContainer",
		  "Name of Photon Container to be created");

  declareProperty("InputClusterContainerName",
		  m_inputClusterContainerName="LArClusterEM",
		  "Input cluster container for egamma objects");


  declareProperty("TopoSeededClusterContainerName",
		  m_topoSeededClusterContainerName="EMTopoCluster430",
		  "Input topo-seeded cluster container for egamma objects");

  declareProperty("egammaRecContainer",
		  m_egammaRecContainerName="egammaRecCollection",
		  "Output container for egammaRec objects");

  declareProperty("egammaTools", m_egammaTools,
    "Tools for dressing electrons and photons");
  
  declareProperty("ElectronTools", m_electronTools,
    "Tools for dressing ONLY electrons");

  declareProperty("PhotonTools", m_photonTools,
    "Tools for dressing ONLY photons");

  //
  // Handles of tools
  //

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
  
  //
  // All booleans
  //

  // Boolean to do Brem collection Building
  declareProperty("doBremCollection",m_doBremCollection= true,
		  "Boolean to do Brem collection building");

  // Boolean to do track matching
  declareProperty("doTrackMatching",m_doTrackMatching= true,
		  "Boolean to do track matching (and conversion building)");

  // Boolean to do conversion reconstruction
  declareProperty("doConversions",m_doConversions= true,
		  "Boolean to do conversion building / matching");

  // Boolean to do topo-seeded photons
  declareProperty("doTopoSeededPhotons",m_doTopoSeededPhotons= true,
		  "Boolean to do topo-seeded photons");


  // Boolean to dump content of each object
  declareProperty("Dump",m_dump=false,
		  "Boolean to dump content of each object");
  //
  // Other properties.
  //
  // Threshold for discarding pathological clusters.
  declareProperty ("clusterEnergyCut", m_clusterEnergyCut = 10 * MeV,
                   "Discard clusters with energies below this "
                   "after corrections.");
  
  declareProperty("minDeltaEta", m_minDeltaEta = 0.05,
                  "Minimum deltaEta to check if clusters overlap");
  
  declareProperty("minDeltaPhi", m_minDeltaPhi = 0.1,
                  "Minimum deltaPhi to check if clusters overlap");

  declareProperty("minEtTopo", m_minEtTopo = 1.5 * GeV,
                  "Minimum transverse energy to accept topo-seeded clusters");

  declareProperty("maxEtTopo", m_maxEtTopo = 8 * GeV,
                  "Minimum transverse energy to accept topo-seeded clusters");
  
}

// ================================================================
topoEgammaBuilder::~topoEgammaBuilder()
{  
  //
  // destructor
  //
}

// =================================================================
StatusCode topoEgammaBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_INFO("Initializing topoEgammaBuilder");

  //////////////////////////////////////////////////
  //
  CHECK(RetrieveegammaTopoClusterMap());
  CHECK(RetrieveegammaSuperClusterBuilder());
  CHECK(RetrieveegammaClusterOverlapMarker());
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
  
  ATH_MSG_INFO("Retrieving " << m_egammaTools.size() << " tools for egamma objects");
  CHECK( RetrieveTools(m_egammaTools) );
  
  ATH_MSG_INFO("Retrieving " << m_electronTools.size() << " tools for electrons");
  CHECK( RetrieveTools(m_electronTools) );
  
  ATH_MSG_INFO("Retrieving " << m_photonTools.size() << " tools for photons");
  CHECK( RetrieveTools(m_photonTools) );

  // retrieve timing profile
  CHECK( service("ChronoStatSvc",m_timingProfile) );

  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode topoEgammaBuilder::RetrieveTools(ToolHandleArray<IegammaBaseTool>& tools)
{
  for (const auto& tool : tools)
  {
    if ( tool.retrieve().isFailure() )
    {
      
      ATH_MSG_FATAL( "Could not get tool: " << tool );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode topoEgammaBuilder::RetrieveAmbiguityTool()
{
  // retrieve Ambiguity tool
  if (m_ambiguityTool.empty()) {
    ATH_MSG_ERROR("EGammaAmbiguityTool is empty");
    return StatusCode::FAILURE;
  }

  if((m_ambiguityTool.retrieve()).isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_ambiguityTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_INFO("Retrieved Tool "<<m_ambiguityTool);

  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode topoEgammaBuilder::RetrieveEMTrackMatchBuilder()
{
  //
  // retrieve EMTrackMatchBuilder tool
  //

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
  else ATH_MSG_INFO("Retrieved Tool "<<m_trackMatchBuilder); 

  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode topoEgammaBuilder::RetrieveEMConversionBuilder()
{
  //
  // retrieve EMConversionBuilder tool
  //

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
  else ATH_MSG_INFO("Retrieved Tool "<<m_conversionBuilder); 
  
  return StatusCode::SUCCESS;
}

//C.A Topo Brem based Rec
// ====================================================================
StatusCode topoEgammaBuilder::RetrieveBremCollectionBuilder()
{
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
  else ATH_MSG_INFO("Retrieved Tool "<<m_BremCollectionBuilderTool);  
  
  return StatusCode::SUCCESS;
}
 // ====================================================================
StatusCode topoEgammaBuilder::RetrieveVertexBuilder()
{
  //
  // retrieve vertex builder for ID conversions
  //
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
  else ATH_MSG_INFO("Retrieved Tool "<<m_vertexBuilder);
  
  return StatusCode::SUCCESS;
}


// ====================================================================
StatusCode topoEgammaBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode topoEgammaBuilder::execute()
{
  //
  // athena execute method
  //
  
  ATH_MSG_DEBUG("Executing topoEgammaBuilder");
  
  // Chrono name for each Tool
  std::string chronoName;

  //Make supercluster electron container.
  xAOD::ElectronContainer*    electronContainer    = new xAOD::ElectronContainer();
  xAOD::ElectronAuxContainer* electronAuxContainer = new xAOD::ElectronAuxContainer();
  electronContainer->setStore( electronAuxContainer );
  
  if ( evtStore()->record(electronContainer, m_electronOutputName).isFailure() ||
       evtStore()->record(electronAuxContainer, m_electronOutputName + "Aux.").isFailure())
  {
    ATH_MSG_ERROR("Could not record electron container or its aux container");
    return StatusCode::FAILURE;
  }       
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // NEW SUPERCLUSTER-RELATED CODE.

  //Get topoclusters.
  const xAOD::CaloClusterContainer *topoclusters;
  if (evtStore()->retrieve(topoclusters, "EMTopoCluster430").isFailure()) {
    ATH_MSG_ERROR("Could not retrieve cluster container EMTopoCluster430");
    return StatusCode::FAILURE;
  }

  //Set & sort the topocluster map.
  //NOTE: Seed egammaRec container will be made here now, too.
  ATH_MSG_DEBUG("Making topocluster map");
  CHECK(ExecTopoClusterMap(topoclusters));

  EgammaRecContainer *egammaSeedRecs = 0;
  CHECK(evtStore()->retrieve(egammaSeedRecs, "TopoTrackClusterMatches"));

  //Do track refitting.
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

  // Track matching for topoclusters.
  for (auto egRec : *egammaSeedRecs){
    if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
      ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
      return StatusCode::FAILURE;
    }
  }

  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

  //Build superclusters & retrieve resultant egammaRec container.
  CHECK(m_egammaSuperClusterBuilder->execute());

  EgammaRecContainer *egammaSuperRecs = 0;
  CHECK(evtStore()->retrieve(egammaSuperRecs,  "egammaSuperRecCollection"));

  ATH_MSG_DEBUG("Size of egammaSuperRecs: " << egammaSuperRecs->size());

  //Build egammaRec objects for superclusters.
  for (auto egRec : *egammaSuperRecs) {
    if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
      ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
      return StatusCode::FAILURE;
    }
  }
  
  //Build xAOD::Electron objects from track-matched superclusters.
  for (const auto& egRec : *egammaSuperRecs) {

    ATH_MSG_INFO("Number of clusters in egRec: " << egRec->getNumberOfClusters());
    
    //Sanity check that we don't find any electron authors to be photons.
    unsigned int author = m_ambiguityTool->ambiguityResolve(egRec->caloCluster(),
                                                            egRec->vertex(),
                                                            egRec->trackParticle());

    if (author == xAOD::EgammaParameters::AuthorPhoton) {
      ATH_MSG_INFO("WARNING: Supercluster electron called photon by ambiguity tool??");
      //return StatusCode::FAILURE;
    }
    
    if (author == xAOD::EgammaParameters::AuthorAmbiguous) {
      ATH_MSG_INFO("WARNING: Supercluster electron called ambiguous by ambiguity tool??");
      //return StatusCode::FAILURE;
    }

    if (!getElectron(egRec, electronContainer, author))
      return StatusCode::FAILURE;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////

  //SLIDING WINDOW PHOTON ROUTINES.
  // create egammaRecContainer, PhotonContainer and register them
  EgammaRecContainer* egammaRecs = new EgammaRecContainer();
  if (evtStore()->record(egammaRecs, m_egammaRecContainerName).isFailure())
    {
      ATH_MSG_ERROR("Could not record egammaRecContainer");
      return StatusCode::FAILURE;
    }

  //Make output SW photon containers.  
  xAOD::PhotonContainer*    photonContainer    = new xAOD::PhotonContainer();
  xAOD::PhotonAuxContainer* photonAuxContainer = new xAOD::PhotonAuxContainer();
  photonContainer->setStore( photonAuxContainer );
  
  if ( evtStore()->record(photonContainer, m_photonOutputName).isFailure() ||
       evtStore()->record(photonAuxContainer, m_photonOutputName + "Aux.").isFailure())
  {
    ATH_MSG_ERROR("Could not record photon container or its aux container");
    return StatusCode::FAILURE;
  }       

  // Loop over SW clusters to create photon egammaRec objects.
  ATH_MSG_DEBUG("Creating egammaRec objects");
  const xAOD::CaloClusterContainer *clusters;
  if (evtStore()->retrieve(clusters, m_inputClusterContainerName).isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve cluster container " << m_inputClusterContainerName);
      return StatusCode::FAILURE;
    }

  //Building SW egammaRecs, converted photons.
  for (unsigned int i = 0; i < clusters->size(); ++i)
  {
    const ElementLink< xAOD::CaloClusterContainer > clusterLink( *clusters, i );
    const std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink};    
    egammaRec *egRec = new egammaRec();
    egRec->setCaloClusters( elClusters );
    egammaRecs->push_back( egRec );
  }

  if (m_doConversions) {

    ATH_MSG_DEBUG("Running VertexBuilder");  
    //
    std::string chronoName = this->name()+"_"+m_vertexBuilder->name();         
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
    chronoName = this->name()+"_"+m_conversionBuilder->name();         
    if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
    //
    if (m_conversionBuilder->contExecute().isFailure()){
      ATH_MSG_ERROR("Problem executing " << m_conversionBuilder);
      return StatusCode::FAILURE;  
    }
    //
    if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
  }

  //Track matching for SW clusters. Not necessary anymore?
  // if (m_doTrackMatching){   
  //   ATH_MSG_DEBUG("Running TrackMatchBuilder");  
  //   //
  //   std::string chronoName = this->name()+"_"+m_trackMatchBuilder->name();         
  //   if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
  //   //
  //   for (auto egRec : *egammaRecs){
  //     if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
  //       ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
  //       return StatusCode::FAILURE;
  //     }
  //   }
  // }
    
  //Change in behaviour: No SW electrons built, mark photons overlapping
  //with superclusters as conflicting.
  for (const auto& egRec : *egammaRecs) {

    ATH_MSG_DEBUG("Running AmbiguityTool");
    unsigned int author = m_ambiguityTool->ambiguityResolve(egRec->caloCluster(),
                                                            egRec->vertex(),
                                                            egRec->trackParticle());
    
    if (author == xAOD::EgammaParameters::AuthorPhoton     || 
	author == xAOD::EgammaParameters::AuthorAmbiguous)
      {
	ATH_MSG_DEBUG("getPhoton");
	if ( !getPhoton(egRec, photonContainer, author) )
	  return StatusCode::FAILURE;
      }
  }


  //////////////////////////////////////////////////////////////////////////
  // Modification to algorithm to make input to 'addTopoSeededPhotons' be
  // a container of SW + my supercluster electrons.
  //////////////////////////////////////////////////////////////////////////

  ATH_MSG_INFO("Adding topo-seeded photons ...");
  
  if (m_doTopoSeededPhotons)
    CHECK( addTopoSeededPhotons(photonContainer, clusters, egammaSuperRecs) );

  ATH_MSG_INFO("Done.");

  //////////////////////////////////////////////////////////////////////////
  // Check supercluster overlap with photons here.
  //////////////////////////////////////////////////////////////////////////

  //Check if photon clusters overlap with any supercluster electrons.
  for (const auto &photon : *photonContainer) {
    CHECK(m_egammaClusterOverlapMarker->execute(photon->caloCluster()));
  }
  
  // Call tools
  for (const auto& tool : m_egammaTools)
    {
      CHECK( CallTool(tool, electronContainer, photonContainer) );
    }
  
  for (const auto& tool : m_electronTools)
    {
      CHECK( CallTool(tool, electronContainer, 0) );
    }

  for (const auto& tool : m_photonTools)
    {
      CHECK( CallTool(tool, 0, photonContainer) );
    }

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
  
  if (electronContainer)
  {    
    ATH_MSG_DEBUG("Executing tool on electrons: " << tool );
    for (const auto& electron : *electronContainer)
    {
      if (tool->execute(electron).isFailure() )
      {
        ATH_MSG_ERROR("Problem executing tool on electrons: " << tool);
        return StatusCode::FAILURE;
      }
    }
  }
  
  if (photonContainer)
  {
    ATH_MSG_DEBUG("Executing tool on photons: " << tool );
    for (const auto& photon : *photonContainer)
    {
      if (tool->execute(photon).isFailure() )
      {
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
                                unsigned int author)
{  
  if (!egRec || !electronContainer) return false;
  
  xAOD::Electron *electron = new xAOD::Electron();
  electronContainer->push_back( electron );
  electron->setAuthor( author );
  
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
                              unsigned int author)
{
  if (!egRec || !photonContainer) return false;
  
  xAOD::Photon *photon = new xAOD::Photon();
  photonContainer->push_back( photon );
  photon->setAuthor( author );

  // Transfer the links to the clusters
  std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
  for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i)
    clusterLinks.push_back( egRec->caloClusterElementLink(i) );
  photon->setCaloClusterLinks( clusterLinks );

  // Transfer the links to the vertices  
  std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks;
  for (unsigned int i = 0 ; i < egRec->getNumberOfVertices(); ++i)
    vertexLinks.push_back( egRec->vertexElementLink(i) );
  photon->setVertexLinks( vertexLinks );
  
  // Transfer deltaEta/Phi info
  float deltaEta = egRec->deltaEtaVtx(), deltaPhi = egRec->deltaPhiVtx();
  if (!photon->setVertexCaloMatchValue( deltaEta,
                                        xAOD::EgammaParameters::convMatchDeltaEta1) )
  {
    ATH_MSG_WARNING("Could not transfer deltaEta to photon");
    return false;
  }
  
  if (!photon->setVertexCaloMatchValue( deltaPhi,
                                        xAOD::EgammaParameters::convMatchDeltaPhi1) )
  {
    ATH_MSG_WARNING("Could not transfer deltaPhi to photon");
    return false;
  }
    
  return true;
}

// =====================================================
StatusCode topoEgammaBuilder::addTopoSeededPhotons(xAOD::PhotonContainer            *photonContainer,
						   const xAOD::CaloClusterContainer *clusters,
						   const EgammaRecContainer         *egammaSuperRecs)
						   
{

  bool useSuperRecs(false);
  if (egammaSuperRecs)
    useSuperRecs = true;
  
  // Retrieve the cluster container
  const xAOD::CaloClusterContainer *topoSeededClusters = 0;
  CHECK( evtStore()->retrieve(topoSeededClusters, m_topoSeededClusterContainerName) );
  ATH_MSG_DEBUG("Number of photons (before topo-clusters): " << photonContainer->size() );
  ATH_MSG_DEBUG("Number of topo-seeded clusters: " << topoSeededClusters->size() );
  
  // Create photon objects for the clusters that pass minimum Et and eta requirements and
  // do not overlap with egamma clusters
  typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;
  
  std::vector<const xAOD::CaloCluster* > candidateTopoClusters;
  for (const auto topoCluster : *topoSeededClusters){

    if (topoCluster->et() < m_minEtTopo || 
        topoCluster->et() > m_maxEtTopo ||
        fabs(topoCluster->etaBE(2)) > 2.47 || 
        clustersOverlap(topoCluster, clusters))
      continue;

    if (useSuperRecs && clustersOverlap(topoCluster, egammaSuperRecs))
      continue;
 
    candidateTopoClusters.push_back(topoCluster);
    
  }

  std::vector<const xAOD::CaloCluster* > finalTopoClusters= egammaDuplicateRemoval::getClusterDuplicateRemoval(candidateTopoClusters);

  for (const auto topoCluster : finalTopoClusters){

  // ATH_MSG_DEBUG("Creating topo-seeded photon, Et: " << topoCluster->et() <<
  // 		" eta: " << topoCluster->eta() << " phi: " << topoCluster->phi());

    xAOD::Photon *photon = new xAOD::Photon();
    photonContainer->push_back(photon);
    photon->setAuthor( xAOD::EgammaParameters::AuthorCaloTopo35 );
    
    ClusterLink_t link(topoCluster, *topoSeededClusters );
    photon->setCaloClusterLinks( std::vector< ClusterLink_t>{ link } );
  }
  ATH_MSG_DEBUG("Number of photons (after topo-clusters): " << photonContainer->size() );
  
  return StatusCode::SUCCESS;
}

// =====================================================
bool topoEgammaBuilder::clustersOverlap(const xAOD::CaloCluster *refCluster, 
				       const xAOD::CaloClusterContainer *clusters)
{
  if (!refCluster || !clusters) return false;
  CaloPhiRange phiHelper;
  
  for (const auto cluster: *clusters)
  {
    if (fabs(refCluster->eta() - cluster->eta()) < m_minDeltaEta &&
        fabs(phiHelper.diff(refCluster->phi(), cluster->phi())) < m_minDeltaPhi)
      return true;
  }
  return false;
}

bool topoEgammaBuilder::clustersOverlap(const xAOD::CaloCluster  *refCluster, 
					const EgammaRecContainer *egammaSuperRecs)
{
  if (!refCluster || !egammaSuperRecs) return false;
  CaloPhiRange phiHelper;
  
  for (const auto egRec: *egammaSuperRecs) {

    //Get list of clusters from element links.
    for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i) {
      const xAOD::CaloCluster *cluster(*(egRec->caloClusterElementLink(i)));
    
      if (fabs(refCluster->eta() - cluster->eta()) < m_minDeltaEta &&
	  fabs(phiHelper.diff(refCluster->phi(), cluster->phi())) < m_minDeltaPhi)
	return true;
    }
  }

  return false;
  
}

//  LocalWords:  newClus egc getCellLinks

//////////////////////////////////////////////////////////////////////
StatusCode topoEgammaBuilder::RetrieveegammaSuperClusterBuilder()
{
 
  // create egammaTopoClusterTrackMatchTool Tool:
  //m_egammaSuperClusterBuilder=ToolHandle<IegammaSuperClusterBuilder>(m_egammaSuperClusterBuilderName); // Builds default instance

  if(m_egammaSuperClusterBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<< m_egammaSuperClusterBuilder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_INFO("Retrieved Tool " << m_egammaSuperClusterBuilder); 
  
  return StatusCode::SUCCESS;
  

}

//////////////////////////////////////////////////////////////////////
StatusCode topoEgammaBuilder::RetrieveegammaClusterOverlapMarker()
{
 
  // create egammaTopoClusterTrackMatchTool Tool:
  //m_egammaClusterOverlapMarker=ToolHandle<IegammaClusterOverlapMarker>(m_egammaClusterOverlapMarkerName);

  if(m_egammaClusterOverlapMarker.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<< m_egammaClusterOverlapMarker);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_INFO("Retrieved Tool " << m_egammaClusterOverlapMarker); 
  
  return StatusCode::SUCCESS;
  

}

//////////////////////////////////////////////////////////////////////
StatusCode topoEgammaBuilder::RetrieveegammaTopoClusterMap()
{

  if (m_egammaTopoClusterMapToolName=="") {
    ATH_MSG_INFO("Map is disabled  " 
     << m_egammaTopoClusterMapToolName);
    return StatusCode::FAILURE;
  } 
 
  // create EMShowerBuilder Tool:
  m_egammaTopoClusterMapTool=ToolHandle<IegammaTopoClusterMap>(m_egammaTopoClusterMapToolName);
  if(m_egammaTopoClusterMapTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_egammaTopoClusterMapTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_INFO("Retrieved Tool " << m_egammaTopoClusterMapTool); 
  
  return StatusCode::SUCCESS;
  

}

/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode topoEgammaBuilder::ExecTopoClusterMap(const xAOD::CaloClusterContainer* topoclusters) 
{

  // protection in case tool is not available
  // return success as algorithm may be able to run without it 
  // in degraded mode
  if (m_egammaTopoClusterMapToolName=="") {
    return StatusCode::SUCCESS;
  }

  // setup chrono for this tool
  std::string chronoName=this->name()+"_"+m_egammaTopoClusterMapTool->name() ;
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);

  // execute the tool
  StatusCode sc = m_egammaTopoClusterMapTool->execute(topoclusters);
  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("failure returned by topocluster map" );
  }
  
  return sc;

}
