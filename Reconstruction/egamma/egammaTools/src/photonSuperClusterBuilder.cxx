/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "photonSuperClusterBuilder.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
//
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
//
#include "FourMomUtils/P4Helpers.h"

#include <vector>

using CLHEP::GeV;
namespace{
  const float cellEtaSize = 0.025;
  const float cellPhiSize = M_PI/128.;
}

//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
photonSuperClusterBuilder::photonSuperClusterBuilder(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  AthAlgTool(type, name, parent)
{

  declareProperty("WindowDelEtaCells", m_delEtaCells = 3, 
		  "Number of cells in eta of window from which to add clusters/cells");

  declareProperty("WindowDelPhiCells", m_delPhiCells = 5,
		  "Number of cells in phi of window from which to add clusters/cells");

  declareProperty("EtThresholdCut", m_EtThresholdCut = 1.5*GeV,
		  "The minimum Et required of SEED clusters (not applied to secondaries)");

  declareProperty("EMFracCut", m_emFracCut = 0.5,
		  "The minimum EM fraction required of both seed and secondary clusters");

  // declareProperty("InputClusterContainerName",
  // 		  m_inputClusterContainerName = "egammaTopoCluster",
  // 		  "Name of input cluster container");

  declareProperty("InputEgammaRecContainerName", m_inputEgammaRecContainerName = "egammaRecCollection");

  declareProperty("SuperPhotonRecCollectionName", m_photonSuperRecCollectionName = "PhotonSuperRecCollection");

  declareProperty("SuperClusterCollestionName",  m_outputPhotonSuperClusters  = "PhotonSuperClusters");

  declareProperty("AddClustersInWindow", m_addClustersInWindow = true,  
		  "add the topoclusters in window");

  declareProperty("AddClustersMatchingVtx", m_addClustersMatchingVtx = true, 
		  "add the topoclusters matching conversion vertex");
  declareProperty("UseOnlyLeadingVertex", m_useOnlyLeadingVertex = true, 
		  "use only the leading vertex for matching");
  declareProperty("UseOnlySi", m_useOnlySi = true, 
		  "use only vertices/tracks with silicon tracks for adding sec. clusters (Mix not considered Si)");
  declareProperty("AddClustrsMatchingVtxTracks", m_addClustersMatchingVtxTracks = true, 
		  "add the topoclusters matching conversion vertex tracks");
  declareProperty("UseOnlyLeadingTrack", m_useOnlyLeadingTrack = true, 
		  "use only the leading track for matching");


  m_delPhi = m_delPhiCells *cellPhiSize * 0.5;
  m_delEta = m_delEtaCells *cellEtaSize  * 0.5;

  // Declare interface & properties.
  declareInterface<IphotonSuperClusterBuilder>(this);
}

//Destructor.
photonSuperClusterBuilder::~photonSuperClusterBuilder() {}

StatusCode photonSuperClusterBuilder::initialize() {
  ATH_MSG_DEBUG(" Initializing photonSuperClusterBuilder");
  m_delPhi = m_delPhiCells *cellPhiSize * 0.5;
  m_delEta = m_delEtaCells *cellEtaSize  * 0.5;
  return StatusCode::SUCCESS;
}

StatusCode photonSuperClusterBuilder::finalize() {
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////////////////////////////////

StatusCode photonSuperClusterBuilder::execute(){

  //Retrieve input egammaRec container.
  const EgammaRecContainer *egammaRecs = 0;
  StatusCode sc=evtStore()->retrieve(egammaRecs, m_inputEgammaRecContainerName );
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve "<< m_inputEgammaRecContainerName);
    return StatusCode::FAILURE;
  }

  // //Retrieve input clusters.
  // const xAOD::CaloClusterContainer* clusters= 0;
  // sc = evtStore()->retrieve(clusters, m_inputClusterContainerName);
  // if( sc.isFailure() ) {
  //   ATH_MSG_ERROR("No input EM Cluster container found " << m_inputClusterContainerName);
  //   return StatusCode::FAILURE;
  // } else {
  //   ATH_MSG_DEBUG("Retrieved container " << m_inputClusterContainerName << " with size " << clusters->size());
  // }

  //Have to register cluster container in order to properly get cluster links.
  xAOD::CaloClusterContainer *outputClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(), 
											     m_outputPhotonSuperClusters, 
  											     msg());
  if (!outputClusterContainer) {
    ATH_MSG_ERROR("Could not make supercluster container : "<< m_outputPhotonSuperClusters);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Made supercluster container : " << m_outputPhotonSuperClusters);
  }
  //Create the new Photon Super Cluster based EgammaRecContainer
  EgammaRecContainer *newEgammaRecs = new EgammaRecContainer();
  if (evtStore()->record(newEgammaRecs, m_photonSuperRecCollectionName).isFailure()) {
    ATH_MSG_ERROR("Could not record egammaRecContainer " << m_photonSuperRecCollectionName);
    return StatusCode::FAILURE;
  } 
  //Loop over input egammaRec objects, build superclusters.
  std::vector<bool> isUsed (egammaRecs->size(),0);

  for (std::size_t i = 0 ; i < egammaRecs->size();++i) {    
    auto egRec=egammaRecs->at(i);
    //First some basic seed cuts
    if(isUsed.at(i)){
      continue;      
    }
    double emFrac(0.);
    if (!egRec->caloCluster()->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
      ATH_MSG_WARNING("NO ENG_FRAC_EM moment available" );
    }
    if (emFrac < m_emFracCut){
      continue;
    }
    if (egRec->caloCluster()->et()*emFrac < m_EtThresholdCut){
      continue;
    }

    //Passed preliminary custs
    ATH_MSG_DEBUG("=========================================================");
    ATH_MSG_DEBUG("Creating supercluster egammaRec photon object");
    ATH_MSG_DEBUG("Using cluster Et = " << egRec->caloCluster()->et()  << " EMFraction " << emFrac << " EM Et " << egRec->caloCluster()->et()*emFrac);
    //
    //So it is used
    isUsed.at(i)=1;     
    //Start accumulating
    std::vector<const xAOD::CaloCluster*>  accumulatedClusters;
    accumulatedClusters.push_back(egRec->caloCluster());

    //////////////////////////////
    //Core Logic goes here
    //Here one would loop and accumulate satelites inside 3x5 etc, marking also some used ones.
    //
    // Will prb need some extra const methods that take the accumulated and the isused and add/ set to them
    //
    ATH_MSG_DEBUG("Find secondary clusters");

    const std::vector<std::size_t> secondaryClusters = SearchForSecondaryClusters(i, egammaRecs, isUsed);


    for (auto secClus : secondaryClusters) {
      const auto secRec = egammaRecs->at(secClus);
      accumulatedClusters.push_back(secRec->caloCluster());
      // no need to add vertices
    }
    //
    //
    //End of core Logic 

    ///////////////////////////////////////////////////////////////////////////////
    //Create the new cluster 

    ATH_MSG_DEBUG("Set up new Cluster");
    xAOD::CaloCluster* newCluster = CaloClusterStoreHelper::makeCluster(egRec->caloCluster()->getCellLinks()->getCellContainer());
    //Need a vector of element Links to the constituent Cluster
    std::vector< ElementLink< xAOD::CaloClusterContainer > > constituentLinks;
    //Loop over accumulated Clusters
    for (auto clusElt : accumulatedClusters) {
      //Add te EM cells of the accumulated to the cluster
      ATH_CHECK(AddEMCellsToCluster(newCluster,clusElt));
      //Set the element Link to the constitents
      static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");
      if (sisterCluster.isAvailable(*clusElt)) {
	constituentLinks.push_back(sisterCluster(*clusElt));
      }else{
	ATH_MSG_WARNING("No sister Link available");
      }
    }
    //Set the link from the super cluster to the constituents (accumulated) clusters used. 
    static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
    caloClusterLinks(*newCluster) = constituentLinks;
    ///

    ///Calculate the kinematics of the new cluster, after all cells are added
    CaloClusterKineHelper::calculateKine(newCluster, true, true);
    //push back the new photon super cluster 
    outputClusterContainer->push_back(newCluster);

    ///////////////////////////////////////////////////////
    //Now create the new eg Rec 
    egammaRec *newEgRec = new egammaRec(*egRec);

    //Add the cluster link to the super cluster
    ElementLink< xAOD::CaloClusterContainer > clusterLink(newCluster, *outputClusterContainer);
    std::vector< ElementLink<xAOD::CaloClusterContainer> > phCluster {clusterLink};
    newEgRec->setCaloClusters(phCluster);

    // will rerun conversion finding, so conversion vertices are not copied

    //push it back
    newEgammaRecs->push_back(newEgRec);

    ATH_MSG_DEBUG("Finished making photon egammaRec object");
  } //End loop on egammaRecs
  return StatusCode::SUCCESS;
}

std::vector<std::size_t> 
photonSuperClusterBuilder::SearchForSecondaryClusters(std::size_t photonInd,
						      const EgammaRecContainer *egammaRecs,
						      std::vector<bool>& isUsed) const
{

  std::vector<std::size_t> secondaryClusters;
  if (!egammaRecs) {
    ATH_MSG_WARNING("photon egammaRec container is null! Returning an empty vector ...");
    return secondaryClusters;
  }

  const auto seedPhoton = egammaRecs->at(photonInd);
  const auto seedCaloClus = seedPhoton->caloCluster();
  if (!seedCaloClus) {
    ATH_MSG_WARNING("The seed egammaRec does not have a cluster");
    return secondaryClusters;
  }

  // let's determine some things about the seed
  std::vector<const xAOD::Vertex*> seedVertices;
  std::vector<xAOD::EgammaParameters::ConversionType> seedVertexType;
  std::vector<const xAOD::TrackParticle*> seedVertexTracks; // tracks from conversion vertex
  
  auto numVertices = seedPhoton->getNumberOfVertices();
  if (m_useOnlyLeadingVertex && numVertices > 0) {
    numVertices = 1;
  }

  for (std::size_t vx = 0; vx < numVertices; ++vx) {
    const auto vertex = seedPhoton->vertex(vx);
    const auto convType = xAOD::EgammaHelpers::conversionType(vertex);
    seedVertices.push_back(vertex);
    seedVertexType.push_back(convType);
    const bool addTracks = !m_useOnlySi || 
      convType == xAOD::EgammaParameters::singleSi ||
      convType == xAOD::EgammaParameters::doubleSi;
    if (addTracks) {
      for (unsigned int tp=0; tp < vertex->nTrackParticles(); ++tp){
	seedVertexTracks.push_back(vertex->trackParticle(tp));
      }
    }
  }

  // Now loop over the potential secondary clusters
  // Note: start with index after seed cluster: depends on sorted nature of egammaRecs
  for (std::size_t i = photonInd + 1; i < egammaRecs->size(); ++i) {
    
    //First some basic seed cuts
    if(isUsed.at(i)){
      continue;      
    }
    
    const auto egRec = egammaRecs->at(i);
    const auto caloClus = egRec->caloCluster();
    if (!caloClus) {
      ATH_MSG_WARNING("The potentially secondary egammaRec does not have a cluster");
      continue;
    } 

    double emFrac(0.);
    if (!caloClus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
      ATH_MSG_WARNING("NO ENG_FRAC_EM moment available" );
    }
    if (emFrac < m_emFracCut){
      continue;
    }

    // Now perform a number of tests to see if the cluster should be added
    bool addCluster = false;
    if (m_addClustersInWindow && 
	MatchesInWindow(seedCaloClus, caloClus)) {
      ATH_MSG_DEBUG("Cluster  with Et : " << caloClus->et()<< " matched in window");
      addCluster = true;
    } else if (m_addClustersMatchingVtx && 
	       MatchesVtx(seedVertices, seedVertexType, egRec)) {
      ATH_MSG_DEBUG("conversion vertices match");
      addCluster = true;
    } else if (m_addClustersMatchingVtxTracks &&
	       MatchesVtxTrack(seedVertexTracks, egRec)) {
      ATH_MSG_DEBUG("conversion track match");
      addCluster = true;
    }

    if (addCluster) {
      secondaryClusters.push_back(i);
      isUsed[i] = true;
    }      
  }
  ATH_MSG_DEBUG("Found: " << secondaryClusters.size()<< " secondaries");
  return secondaryClusters;
}

bool photonSuperClusterBuilder::MatchesInWindow(const xAOD::CaloCluster *ref,
						const xAOD::CaloCluster *clus) const
{
  //Get the differences in 2nd layer eta, phi of the
  //satellite and seed for convenience.
  float dEta(fabs(ref->eta()-clus->eta()));
  float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
  return (dEta < m_delEta && dPhi < m_delPhi);
}

bool photonSuperClusterBuilder::MatchesVtx(const std::vector<const xAOD::Vertex*>& seedVertices,
					   const std::vector<xAOD::EgammaParameters::ConversionType>& seedVertexType,
					   const egammaRec *egRec) const
{

  auto numTestVertices = egRec->getNumberOfVertices();
  if (m_useOnlyLeadingVertex && numTestVertices > 0) {
    numTestVertices = 1;
  }
  
  for (size_t seedVx = 0; seedVx < seedVertices.size(); ++seedVx) {
    if (!m_useOnlySi || 
	seedVertexType[seedVx] == xAOD::EgammaParameters::singleSi ||
	seedVertexType[seedVx] == xAOD::EgammaParameters::doubleSi) {
      
      for (size_t testVx = 0; testVx < numTestVertices; ++testVx) {
	if (seedVertices[seedVx] == egRec->vertex(testVx)) {
	  return true;
	}
      }
    }
  }
  return false;
}

bool photonSuperClusterBuilder::MatchesVtxTrack(const std::vector<const xAOD::TrackParticle*>& seedVertexTracks,
						const egammaRec *egRec) const
{
  auto numTestTracks = egRec->getNumberOfTrackParticles();
  if (m_useOnlyLeadingTrack && numTestTracks > 0) {
    numTestTracks = 1;
  }
  for (size_t seedTk = 0; seedTk < seedVertexTracks.size(); ++seedTk) {
    // selected tracks alread are just Si if we are only looking at Si tracks
    for (size_t testTk = 0; testTk < numTestTracks; ++testTk) {
      if (seedVertexTracks[seedTk] == egRec->trackParticle(testTk)) {
	return true;
      }
    }
  }
  return false;
}

StatusCode photonSuperClusterBuilder::AddEMCellsToCluster(xAOD::CaloCluster *self,
							  const xAOD::CaloCluster *ref)
{
  if (!self || !ref) {
    ATH_MSG_DEBUG("Invalid input in AddEMCellsToCluster");
    return StatusCode::FAILURE;
  }
  xAOD::CaloCluster::const_cell_iterator cell_itr = ref->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = ref->end();
  //Need to check that the cell belongs to the EM calorimeter,
    //Need to check that the cell belongs to the EM calorimeter,
    for (; cell_itr != cell_end; ++cell_itr) { 
      const CaloCell* cell = *cell_itr; 
      if (!cell){
	continue;
      }
      //Add all LAR EM
      if (cell->caloDDE()->getSubCalo() == CaloCell_ID::LAREM) {
	self->addCell(cell_itr.index(), cell_itr.weight());
      }
      //Add TileGap3. Consider only E4 cell
      if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) {
	if( fabs(cell->eta()) >1.4 && fabs(cell->eta()) < 1.6 ){
	  self->addCell(cell_itr.index(), cell_itr.weight());
	}
      }//TileGap
    }//Loop over cells
    return StatusCode::SUCCESS;
}
