/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "photonSuperClusterBuilder.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
//
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
//
#include "FourMomUtils/P4Helpers.h"

#include "egammaInterfaces/IegammaSwTool.h"
#include "egammaMVACalib/IegammaMVATool.h"

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
  AthAlgTool(type, name, parent),
  m_clusterCorrectionTool("egammaSwTool/egammaswtool")
{

  declareProperty("WindowDelEtaCells", m_delEtaCells = 3, 
		  "Number of cells in eta of window from which to add clusters/cells");

  declareProperty("WindowDelPhiCells", m_delPhiCells = 5,
		  "Number of cells in phi of window from which to add clusters/cells");

  declareProperty("EtThresholdCut", m_EtThresholdCut = 1.5*GeV,
		  "The minimum Et required of SEED clusters (not applied to secondaries)");

  declareProperty("EMFracCut", m_emFracCut = 0.5,
		  "The minimum EM fraction required of both seed and secondary clusters");

  declareProperty("SumRemainingCellsInWindow", m_sumRemainingCellsInWindow = false);


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
  
  declareProperty("CorrectClusters", m_correctClusters = false, 
		  "Whether to run cluster corrections");
  
  declareProperty("CalibrateClusters", m_calibrateClusters = true, 
		  "Whether to run cluster calibrations");
  
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ClusterCorrectionTool", m_clusterCorrectionTool);
  
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

  if (m_correctClusters) {
    ATH_CHECK(m_clusterCorrectionTool.retrieve());
  }
  if (m_calibrateClusters) {
    ATH_CHECK(m_MVACalibTool.retrieve());
  }

  return StatusCode::SUCCESS;
}

StatusCode photonSuperClusterBuilder::finalize() {
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////////////////////////////////

StatusCode photonSuperClusterBuilder::execute(){

  // for stats
  int nWindowClusters = 0;
  int nExtraClusters = 0;

  //Retrieve input egammaRec container.
  const EgammaRecContainer *egammaRecs = 0;
  StatusCode sc=evtStore()->retrieve(egammaRecs, m_inputEgammaRecContainerName );
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve "<< m_inputEgammaRecContainerName);
    return StatusCode::FAILURE;
  }

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
    const auto egRec=egammaRecs->at(i);
    const auto egClus = egRec->caloCluster();
    //First some basic seed cuts
    if(isUsed.at(i)){
      continue;      
    }
    double emFrac(0.);
    if (!egClus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
      ATH_MSG_WARNING("NO ENG_FRAC_EM moment available" );
    }
    if (emFrac < m_emFracCut){
      continue;
    }
    if (egClus->et()*emFrac < m_EtThresholdCut){
      continue;
    }

    //Passed preliminary custs
    ATH_MSG_DEBUG("=========================================================");
    ATH_MSG_DEBUG("Creating supercluster egammaRec photon object");
    ATH_MSG_DEBUG("Using cluster Et = " << egClus->et()  << " EMFraction " << emFrac << " EM Et " << egClus->et()*emFrac);
    //
    //So it is used
    isUsed.at(i)=1;     
    //Start accumulating
    std::vector<const xAOD::CaloCluster*>  accumulatedClusters;
    accumulatedClusters.push_back(egClus);

    //////////////////////////////
    //Core Logic goes here
    ATH_MSG_DEBUG("Find secondary clusters");

    const std::vector<std::size_t> secondaryClusters = 
      SearchForSecondaryClusters(i, egammaRecs, isUsed, nWindowClusters, nExtraClusters);


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
    xAOD::CaloCluster* newCluster = CaloClusterStoreHelper::makeCluster(egClus->getCellLinks()->getCellContainer());

    if (!newCluster) {
      ATH_MSG_WARNING("CaloClusterStoreHelper::makeCluster failed.");
      return StatusCode::RECOVERABLE;
    }

    newCluster->setClusterSize(xAOD::CaloCluster::SuperCluster);

    //A vector to keep track of cells we have added (filled by AddEMCellsToCluster)
    std::vector<const CaloCell*> cellsInWindow;

    //Need a vector of element Links to the constituent Cluster
    std::vector< ElementLink< xAOD::CaloClusterContainer > > constituentLinks;

    //Loop over accumulated Clusters, but treat 0 separately to determine 
    const auto acSize = accumulatedClusters.size();
    static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");
    if (acSize == 0) {
      ATH_MSG_ERROR("Missing the seed cluster! Should not happen.");
      return StatusCode::FAILURE;
    }

    //Add the EM cells of the seed cluster
    ATH_CHECK(AddEMCellsToCluster(newCluster,accumulatedClusters[0], cellsInWindow));
    //Set the element Link to the constitents
    if (sisterCluster.isAvailable(*accumulatedClusters[0])) {
      constituentLinks.push_back(sisterCluster(*accumulatedClusters[0]));
    } else{
      ATH_MSG_WARNING("No sister Link available");
    }
    CaloClusterKineHelper::calculateKine(newCluster, true, true);
    
    // set the seed values in the cluster
    const auto seed_eta = newCluster->eta();
    const auto seed_phi = newCluster->phi();
    newCluster->setAltE(newCluster->e());
    newCluster->setAltEta(seed_eta);
    newCluster->setAltPhi(seed_phi);

    // now continue with the remaining clusters
    for (size_t i = 1; i < acSize; i++) {
      //Add te EM cells of the accumulated to the cluster
      ATH_CHECK(AddEMCellsToCluster(newCluster,accumulatedClusters[i], cellsInWindow));
      //Set the element Link to the constitents
      static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");
      if (sisterCluster.isAvailable(*accumulatedClusters[i])) {
	constituentLinks.push_back(sisterCluster(*accumulatedClusters[i]));
      }else{
	ATH_MSG_WARNING("No sister Link available");
      }
    }
    //Set the link from the super cluster to the constituents (accumulated) clusters used. 
    static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
    caloClusterLinks(*newCluster) = constituentLinks;
    ///

    //////////////////////////////////////////////////////////////////
    static const SG::AuxElement::Accessor<int> nWindowClustersAcc ("nWindowClusters");
    nWindowClustersAcc(*newCluster) = nWindowClusters;
    ATH_MSG_DEBUG("Clusters in Window " <<  nWindowClusters);
    static const SG::AuxElement::Accessor<int> nExtraClustersAcc ("nExtraClusters");
    nExtraClustersAcc(*newCluster) = nExtraClusters;
    ATH_MSG_DEBUG("extra  clusters " << nExtraClusters);
    //////////////////////////////////////////////////////////////////

    //Add the remaining cells
    if (m_sumRemainingCellsInWindow) {
      ATH_CHECK(AddRemainingCellsToCluster(newCluster, seed_eta, seed_phi, cellsInWindow));
    }

    ///Calculate the kinematics of the new cluster, after all cells are added
    CaloClusterKineHelper::calculateKine(newCluster, true, true);

    // Apply calibration (if requested) and fill some extra variables
    ATH_CHECK(CalibrateCluster(newCluster, egRec));

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
						      std::vector<bool>& isUsed,
						      int& nWindowClusters,
						      int& nExtraClusters) const
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
      nWindowClusters++;
      addCluster = true;
    } 

    // should do "else if" if we want nWindowCluster and nExtraCluster to not both increment
    if (m_addClustersMatchingVtx && 
	       MatchesVtx(seedVertices, seedVertexType, egRec)) {
      ATH_MSG_DEBUG("conversion vertices match");
      addCluster = true;
      nExtraClusters++;
    } else if (m_addClustersMatchingVtxTracks &&
	       MatchesVtxTrack(seedVertexTracks, egRec)) {
      ATH_MSG_DEBUG("conversion track match");
      addCluster = true;
      nExtraClusters++;
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
							  const xAOD::CaloCluster *ref,
							  std::vector<const CaloCell*>& cellsInWindow) const
{
  if (!self || !ref) {
    ATH_MSG_ERROR("Invalid input in AddEMCellsToCluster");
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
	cellsInWindow.push_back(cell);
      }
      //Add TileGap3. Consider only E4 cell
      if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) {
	if( fabs(cell->eta()) >1.4 && fabs(cell->eta()) < 1.6 ){
	  self->addCell(cell_itr.index(), cell_itr.weight());
	  cellsInWindow.push_back(cell);
	}
      }//TileGap
    }//Loop over cells
    return StatusCode::SUCCESS;
}

StatusCode photonSuperClusterBuilder::AddRemainingCellsToCluster(xAOD::CaloCluster *myCluster,
								 float seed_eta, float seed_phi,
								 std::vector<const CaloCell*>& cellsInWindow) const
{
  ATH_MSG_DEBUG("Add Remaining cells in window");
  if (!myCluster) {
    ATH_MSG_ERROR("Invalid input in AddRemainingCellsToCluster");
    return StatusCode::FAILURE;
  }
  std::vector<const CaloCell*> cells;
  cells.reserve(100);
  const CaloCellContainer* inputcells=myCluster->getCellLinks()->getCellContainer();
  CaloCellList myList(inputcells);
  const std::vector<CaloSampling::CaloSample> samples = {CaloSampling::PreSamplerB,
							 CaloSampling::PreSamplerE,
							 CaloSampling::EMB1,
							 CaloSampling::EME1,
							 CaloSampling::EMB2,
							 CaloSampling::EME2,
							 CaloSampling::EMB3,
							 CaloSampling::EME3,
							 CaloSampling::TileGap3};
  
  for ( auto samp : samples ) {
    myList.select(seed_eta, seed_phi, m_delEta, m_delPhi,samp);
    cells.insert(cells.end(), myList.begin(), myList.end());
  }
  for ( auto cell : cells ) {
    if( !cell || !cell->caloDDE() ) {continue;}
    int index = inputcells->findIndex(cell->caloDDE()->calo_hash());
    if( index == -1 ) {continue;}
    //Check if it's already used.
    if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling() && 
	(std::abs(cell->eta()) <= 1.4 || std::abs(cell->eta()) >= 1.6)) {
      continue; // only take them in the gap
    }
    if (std::find(cellsInWindow.begin(),cellsInWindow.end(),cell) != cellsInWindow.end()) {
      continue;
    }
    //Adding with weight '1' now -- don't a priori have weight of cell,
    myCluster->addCell(index, 1.);
  }
  return StatusCode::SUCCESS;
}

StatusCode photonSuperClusterBuilder::CalibrateCluster(xAOD::CaloCluster* newCluster,
						       const egammaRec* egRec)
{

  // first do the corrections
  if (m_correctClusters) {
    ATH_CHECK(m_clusterCorrectionTool->execute(newCluster));
  }
  ATH_CHECK(fillPositionsInCalo(newCluster));
  
  //Fill the raw state before doing any calibrations
  newCluster->setRawE(newCluster->e());
  newCluster->setRawEta(newCluster->eta());
  newCluster->setRawPhi(newCluster->phi());

  // This is where we calibrate the cluster
  if (m_calibrateClusters) {
    auto egType = (egRec->getNumberOfVertices() > 0) ? 
      xAOD::EgammaParameters::convertedPhoton :
      xAOD::EgammaParameters::unconvertedPhoton;
    ATH_CHECK(m_MVACalibTool->execute(newCluster,egRec,egType));
  }

  return StatusCode::SUCCESS;
}


// ==========================================================================
StatusCode photonSuperClusterBuilder::fillPositionsInCalo(xAOD::CaloCluster* cluster) 
{

  bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
  CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
  // eta and phi of the cluster in the calorimeter frame
  double eta, phi;
  m_caloCellDetPos.getDetPosition(sample, cluster->eta(), cluster->phi(), eta, phi); 

  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME,phi);

  //  eta in the second sampling
  m_caloCellDetPos.getDetPosition(sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME,phi);
  //  eta in the first sampling 
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  m_caloCellDetPos.getDetPosition(sample, cluster->etaBE(1), cluster->phiBE(1),eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME,phi);

  return StatusCode::SUCCESS;

}
