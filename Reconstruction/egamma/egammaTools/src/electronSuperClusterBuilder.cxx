/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "electronSuperClusterBuilder.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
//
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h" 
//
//extrapolation
#include "egammaInterfaces/IEMExtrapolationTools.h"

//
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "FourMomUtils/P4Helpers.h"

#include "egammaInterfaces/IegammaSwTool.h"
#include "egammaMVACalib/IegammaMVATool.h"

using CLHEP::GeV;
namespace{
  const float cellEtaSize = 0.025;
  const float cellPhiSize = M_PI/128.;
}
//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
electronSuperClusterBuilder::electronSuperClusterBuilder(const std::string& type,
							 const std::string& name,
							 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_extrapolationTool("EMExtrapolationTools"),
  m_clusterCorrectionTool("egammaSwTool/egammaswtool")
{
  declareProperty("WindowDelEtaCells", m_delEtaCells = 3);

  declareProperty("WindowDelPhiCells", m_delPhiCells = 5);
  
  declareProperty("UseBremFinder", m_useBremFinder=false);

  declareProperty("EtThresholdCut", m_EtThresholdCut = 1.0*GeV);

  declareProperty("PUThresholdCut", m_secThresholdCut = 400.);

  declareProperty("EMFracCut", m_emFracCut = 0.5);

  declareProperty("SecondaryEMFracCut", m_secondaryEmFracCut = 0.9);

  declareProperty("TrackOverlapDelEta", m_trackOverlapDelEta = 0.075);

  declareProperty("TrackOverlapDelPhi", m_trackOverlapDelPhi = 0.15);

  declareProperty("SumRemainingCellsInWindow", m_sumRemainingCellsInWindow = false);

  declareProperty("ExtrapolationTool", m_extrapolationTool, "Handle of the extrapolation tool");

  //declareProperty("InputClusterContainerName", m_inputClusterContainerName = "egammaTopoCluster", "Name of input cluster container");

  declareProperty("InputEgammaRecContainerName", m_inputEgammaRecContainerName = "egammaRecCollection");

  //declareProperty("TrackParticleContainerName", m_trackParticleContainerName = "GSFTrackParticles");

  declareProperty("SuperElectronRecCollectionName", m_electronSuperRecCollectionName = "ElectronSuperRecCollection");

  declareProperty("SuperClusterCollestionName",  m_outputElectronSuperClusters  = "ElectronSuperClusters");

  declareProperty("NumberOfReqSiHits", m_numberOfSiHits = 4);

  declareProperty("CorrectClusters", m_correctClusters = false, 
		  "Whether to run cluster corrections");
  
  declareProperty("CalibrateClusters", m_calibrateClusters = true, 
		  "Whether to run cluster calibrations");
  
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ClusterCorrectionTool", m_clusterCorrectionTool);

  m_delPhi = m_delPhiCells *cellPhiSize * 0.5;
  m_delEta = m_delEtaCells *cellEtaSize * 0.5;

  // Declare interface & properties.
  declareInterface<IelectronSuperClusterBuilder>(this);
}

//Destructor.
electronSuperClusterBuilder::~electronSuperClusterBuilder() {}

StatusCode electronSuperClusterBuilder::initialize() {
  ATH_MSG_DEBUG(" Initializing electronSuperClusterBuilder");
  ATH_MSG_DEBUG("Using  window of size: " << m_delEtaCells << " x " << m_delPhiCells);
  if(m_extrapolationTool.retrieve().isFailure()){
    ATH_MSG_ERROR("initialize: Cannot retrieve extrapolationTool " << m_extrapolationTool);
    return StatusCode::FAILURE;
  }

  if (m_correctClusters) {
    ATH_CHECK(m_clusterCorrectionTool.retrieve());
  }
  if (m_calibrateClusters) {
    ATH_CHECK(m_MVACalibTool.retrieve());
  }

  m_delPhi = m_delPhiCells *cellPhiSize * 0.5;
  m_delEta = m_delEtaCells *cellEtaSize * 0.5;
  return StatusCode::SUCCESS;
}

StatusCode electronSuperClusterBuilder::finalize() {
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////////////////////////////////

StatusCode electronSuperClusterBuilder::execute(){

  //-------------------------------------------------------------------------------------------------------
  //Register cluster container.
  xAOD::CaloClusterContainer *outputClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(), 
											     m_outputElectronSuperClusters, 
  											     msg());
  if (!outputClusterContainer) {
    ATH_MSG_ERROR("Could not make supercluster container : "<< m_outputElectronSuperClusters);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Made supercluster container : " << m_outputElectronSuperClusters);
  }
  //Create new EgammaRecContainer
  EgammaRecContainer *newEgammaRecs = new EgammaRecContainer();
  if (evtStore()->record(newEgammaRecs, m_electronSuperRecCollectionName).isFailure()) {
    ATH_MSG_ERROR("Could not record egammaRecContainer " << m_electronSuperRecCollectionName);
    return StatusCode::FAILURE;
  }
  //-------------------------------------------------------------------------------------------------------
  //Retrieve input egammaRec container.
  const EgammaRecContainer *egammaRecs = 0;
  StatusCode sc=evtStore()->retrieve(egammaRecs, m_inputEgammaRecContainerName);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve "<< m_inputEgammaRecContainerName);
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved "<< m_inputEgammaRecContainerName);

  //Reserve a vector to keep track of what is used
  std::vector<bool> isUsed (egammaRecs->size(),0);
  //Loop over input egammaRec objects, build superclusters.
  for (std::size_t i = 0 ; i < egammaRecs->size();++i) {    
    auto egRec=egammaRecs->at(i);
    if(isUsed.at(i)){
      continue;      
    }
    /////////////////////////////////////////////////////////////////
    // Cut on emfrac here
    double emFrac(0.);
    if (!egRec->caloCluster()->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
      ATH_MSG_WARNING("NO ENG_FRAC_EM moment available" );
    }
    if (emFrac < m_emFracCut) {
      continue;
    }
    //Require minimum energy for supercluster seeding.
    if (egRec->caloCluster()->et()*emFrac < m_EtThresholdCut){
      continue;
    }
    //Basic cut on energy.
    if (egRec->caloCluster()->e()*emFrac < m_secThresholdCut) {
      continue;
    }
    //Get the associated tracks
    if (egRec->getNumberOfTrackParticles()==0) {
      continue;
    }
    uint8_t trkPixelHits(0);
    egRec->trackParticle(0)->summaryValue(trkPixelHits,xAOD::numberOfPixelHits);
    if (!trkPixelHits){
      continue;
    }
    if (xAOD::EgammaHelpers::numberOfSiHits(egRec->trackParticle(0)) < m_numberOfSiHits){
      continue;
    }
    //Counters to keep tracks why we added the clusters
    m_nWindowClusters    = 0;
    m_nExtraClusters = 0;
    m_nBremPointClusters=0;
    m_nSameTrackClusters=0;
    //
    //=========================================================================================
    ATH_MSG_DEBUG("=====================================================================");
    ATH_MSG_DEBUG("Creating supercluster egammaRec electron object");
    ATH_MSG_DEBUG("Using cluster Et = " << egRec->caloCluster()->et() << " eta " << egRec->caloCluster()->eta() << " phi "<< egRec->caloCluster()->phi()  << 
		  " EMFraction " << emFrac << " EM Et " << egRec->caloCluster()->et()*emFrac << " pixel hits " << static_cast<unsigned int> (trkPixelHits));    
    //Mark as used
    isUsed.at(i)=1;     
    //
    //Add the seed as the 1st entry in the secondaries list 
    ATH_MSG_DEBUG("Push back the existing egRec caloCluster");
    std::vector<const xAOD::CaloCluster*> accumulatedClusters;
    accumulatedClusters.push_back(egRec->caloCluster());
    //Add its tracks
    std::vector< ElementLink<xAOD::TrackParticleContainer> > elTracks;
    for (unsigned int i = 0; i < egRec->getNumberOfTrackParticles(); ++i){
      elTracks.push_back(egRec->trackParticleElementLink(i));
    }
    //================
    ATH_MSG_DEBUG("Find the Brem Extrapolation");
    CHECK(GetBremExtrapolations(egRec));
    //===========

    //Find Secondary Clusters
    ATH_MSG_DEBUG("Find secondary clusters");
    const std::vector<std::size_t>  secondaryIndices = SearchForSecondaryClusters(i, egammaRecs, isUsed);
    //
    //Append possible accumulated  clusters.
    ATH_MSG_DEBUG("Add secondary clusters");
    for(const auto& secIndex : secondaryIndices){
      const auto secRec = egammaRecs->at(secIndex);
      accumulatedClusters.push_back(secRec->caloCluster());
      //Append also the corresponding tracks
      for (unsigned int i = 0; i < secRec->getNumberOfTrackParticles(); ++i){
	elTracks.push_back(secRec->trackParticleElementLink(i));
      }
    }
    ATH_MSG_DEBUG("Total clusters " << accumulatedClusters.size());
    
    //Take the full list of cluster , now stored in the secondaries and add their cells together
    xAOD::CaloCluster *newClus = AddTopoClusters(accumulatedClusters);
    if (newClus) {

      // Decorate the supercluster with a vector of ElementLinks to the constituent clusters.
      static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
      std::vector< ElementLink< xAOD::CaloClusterContainer > > clusLinks;
      static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");
      
      for ( const auto& clusElt : accumulatedClusters ) {
	if (sisterCluster.isAvailable(*clusElt) ) {
	  ElementLink< xAOD::CaloClusterContainer > clusLink = sisterCluster(*clusElt);
	  clusLinks.push_back(clusLink);
	}else{
	  ATH_MSG_WARNING("No sister Link available");
	}
      }
      caloClusterLinks(*newClus) = clusLinks;
      //////////////////////////////////////////////////////////////////
      //C.A still not sure if needed , but if yes we need them also for photons, otherwise can not be used globally
      //Can we decide soon as this is kind of EDM
      static const SG::AuxElement::Accessor<int> nWindowClusters ("nWindowClusters");
      nWindowClusters(*newClus) = m_nWindowClusters;
      ATH_MSG_DEBUG("Clusters in Window " <<  m_nWindowClusters);
      static const SG::AuxElement::Accessor<int> nExtrapClusters ("nExtraClusters");
      nExtrapClusters(*newClus) = m_nExtraClusters;
      ATH_MSG_DEBUG("extra  clusters " << m_nExtraClusters);
      ATH_MSG_DEBUG("Brem Point  clusters " << m_nBremPointClusters);
      ATH_MSG_DEBUG("Same Track  clusters " << m_nSameTrackClusters);
      //////////////////////////////////////////////////////////////////
      //Push back the new cluster into the output container.
      outputClusterContainer->push_back(newClus);
      ElementLink< xAOD::CaloClusterContainer > clusterLink(newClus, *outputClusterContainer);
      std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink};

      // Apply calibration (if requested) and fill some extra variables
      ATH_CHECK(CalibrateCluster(newClus, egRec));

      ////////////////////////////////////////////////////////////////////
      //Make egammaRec object, and push it back into output container.
      egammaRec *newEgRec = new egammaRec(*egRec);
      if (newEgRec) {
	newEgRec->setCaloClusters  (elClusters);
	newEgRec->setTrackParticles(elTracks);
	newEgammaRecs->push_back(newEgRec);
	ATH_MSG_DEBUG("Made new egammaRec object");
      }
    }
  } //End loop on egammaRecs
  return StatusCode::SUCCESS;
}

const std::vector<std::size_t> electronSuperClusterBuilder::SearchForSecondaryClusters(const std::size_t electronIndex,
										       const EgammaRecContainer* egammaRecs,
										       std::vector<bool>& isUsed){
  
  std::vector<std::size_t>  secondaryClusters;
  if (!egammaRecs) {
    ATH_MSG_DEBUG("egammaRec container is null! Returning an empty vector ...");
    return secondaryClusters;
  }
  const auto seedEgammaRec =egammaRecs->at(electronIndex);
  const xAOD::CaloCluster *seedCluster = seedEgammaRec->caloCluster();
  
  for (std::size_t i = 0 ; i < egammaRecs->size();++i) {    
    //if already used continue
    if(isUsed.at(i)){
      continue;
    }
    //if not retrieve it
    const auto egRec = egammaRecs->at(i);
    const xAOD::CaloCluster *clus = egRec->caloCluster();
    //Basic cuts 
    double emFrac(0.);
    if (!clus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
      ATH_MSG_WARNING("NO ENG_FRAC_EM moment available" );
    }
    if (emFrac < m_emFracCut){
      continue;
    }
    if (clus->e()*emFrac < m_secThresholdCut) {
      continue;
    }
    //Now the actual checks
    //Check if clusters are nearby enough to form the "topo-seeded window.'
    bool matchesInWindow(MatchesInWindow(seedCluster,clus));
    if (matchesInWindow){
      ATH_MSG_DEBUG("Cluster  with Et : " << clus->et()<< " matched in window");
      ++ m_nWindowClusters;
    }    
    //Lastly, check if we have any other egRec with the same track as best.
    bool matchSameTrack (MatchSameTrack(seedEgammaRec,egRec));
    if(matchSameTrack){
      ATH_MSG_DEBUG("Cluster  with Et : " << clus->et()<< " eta " << clus->eta()<<" phi " << clus->phi() << " matches same track");
      ++m_nSameTrackClusters;
      ++m_nExtraClusters;
    }
    //Now go through to see if any match the brem point extrapolations.
    bool overlapsBremPoint(OverlapsABremPoint(clus) && emFrac > m_secondaryEmFracCut);
    if(overlapsBremPoint) {
      ATH_MSG_DEBUG("Cluster  with Et : " << clus->et()<< " overlaps brem point");
      ++m_nBremPointClusters;
      ++m_nExtraClusters;
    }
    //Add it to the list of secondary clusters if it matches.
    if (matchesInWindow || overlapsBremPoint || matchSameTrack) {
      secondaryClusters.push_back(i); 
      isUsed.at(i)=1;
    }
  }
  ATH_MSG_DEBUG("Found: " << secondaryClusters.size()<< " secondaries");
  return secondaryClusters;
}

xAOD::CaloCluster *electronSuperClusterBuilder::AddTopoClusters(const std::vector <const xAOD::CaloCluster*> &clusters){
  //Reset the vector to keep track of added cell indices.
  m_cellsin3x5.clear();
  if( clusters.size()  ==  0 )  return 0;
  ////////////////////////////////////////////////////////////////////////
  // making Cluster
  ATH_MSG_DEBUG("Adding Topo Clusters Together");
  xAOD::CaloCluster* myCluster = CaloClusterStoreHelper::makeCluster(clusters.at(0)->getCellLinks()->getCellContainer());
  if (myCluster) { 
    myCluster->setClusterSize(xAOD::CaloCluster::SuperCluster);
    //Use only the EM cells to calculate the energy.
    AddEMCellsToCluster(myCluster, clusters.at(0));
    CaloClusterKineHelper::calculateKine(myCluster, true, true);
  }else {
    ATH_MSG_ERROR("Null Cluster in AddTopoCluster");
    return 0;
  }
  ATH_MSG_DEBUG("Raw energy of original seed cluster: " << clusters.at(0)->rawE());
  ATH_MSG_DEBUG("Energy of EM-only part of the seed cluster: "      << myCluster->e());

  // Change this to use Alt* values in cluster  
  // ////////////////////////////////////////////////////////////////////////
  // //C.A still not sure if needed , but if yes we need them also for photons
  // //Can we decide soon as this is kind of EDM
  // static const SG::AuxElement::Accessor<float> seedE  ("seedSuperClusterE");
  // static const SG::AuxElement::Accessor<float> seedEta  ("seedSuperClusterEta");
  // static const SG::AuxElement::Accessor<float> seedPhi  ("seedSuperClusterPhi");
  // float seed_eta(myCluster->eta()), seed_phi(myCluster->phi());
  // seedE(*myCluster)     = myCluster->e();
  // seedEta(*myCluster)     = seed_eta;
  // seedPhi(*myCluster)     = seed_phi;
  const auto seed_eta = myCluster->eta();
  const auto seed_phi = myCluster->phi();
  myCluster->setAltE(myCluster->e());
  myCluster->setAltEta(seed_eta);
  myCluster->setAltPhi(seed_phi);

  //Skip the first one which already is added
  std::vector<const xAOD::CaloCluster*>::const_iterator  matchedClusters    = clusters.begin()+1;
  std::vector<const xAOD::CaloCluster*>::const_iterator  matchedClustersEnd = clusters.end();
  ///
  //Add the remaining topoclusters to form supercluster.
  ATH_MSG_DEBUG("Will sum together " << clusters.size() << " clusters to form supercluster");
  for (; matchedClusters!=matchedClustersEnd; ++matchedClusters){
    AddEMCellsToCluster(myCluster, *matchedClusters);
  }

  //Update cluster kinematics. 
  // JM: Is it necessary to do this here? Also done after summing remaining cells
  CaloClusterKineHelper::calculateKine(myCluster,true,true);
  if (clusters.size() > 1) {
    ATH_MSG_DEBUG("Cluster energy after adding satellites: " << myCluster->e());
  }
  ///////////////////////////////////////////////////////
  // If requested, sum also any cells that are in
  // fixed 3x5 window around seed, but WEREN'T added  the first time around.
  if (m_sumRemainingCellsInWindow) {
    ATH_MSG_DEBUG("Add Remaining cells in 3x5 window");
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
      if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling() && 
	  (std::abs(cell->eta()) <= 1.4 || std::abs(cell->eta()) >= 1.6)) {
	continue; // only take them in the gap
      }
      //Check if it's already used.
      if (std::find(m_cellsin3x5.begin(),m_cellsin3x5.end(),cell) != m_cellsin3x5.end()) {
	continue;
      }
      //Adding with weight '1' now -- don't a priori have weight of cell,
      myCluster->addCell(index, 1.);
    }
  }
  //Update cluster kinematics.
  CaloClusterKineHelper::calculateKine(myCluster,true,true);
  ATH_MSG_DEBUG("Made final supercluster with energy " << myCluster->e());
  return myCluster;
}

bool electronSuperClusterBuilder::MatchesInWindow(const xAOD::CaloCluster *ref,
						  const xAOD::CaloCluster *clus) const{
  //Get the differences in eta, phi of the
  //satellite and seed 
  float dEta(fabs(ref->eta()-clus->eta()));
  float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
  return (dEta < m_delEta && dPhi < m_delPhi);
}

StatusCode electronSuperClusterBuilder::GetBremExtrapolations(const egammaRec *egRec) {
  if (!m_useBremFinder) {
    ATH_MSG_DEBUG("Not using brem point extrapolation!");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Running GetBremExtrapolations ...");
    
  //Some variables to store % energy retained, plus extrapolated eta, phi.
  m_bpRetainedEnFrac.clear();
  m_bpExtrapEta.clear();
  m_bpExtrapPhi.clear();
  //Use extrapolator to find impact point on calorimeter.
  //Want to search around this region for secondaries, i.e. if it's within
  //a valid range in (dEta, dPhi).
  //Get track particle.
  const xAOD::TrackParticle *elTrack = egRec->trackParticle();
  if (!elTrack) {
    ATH_MSG_DEBUG("No track particle associated to the egammaRec");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Perigee eta, phi of seed track: (" << elTrack->eta() << ", " << elTrack->phi() << ")");
  }
  std::vector<const Trk::TrackStateOnSurface*> bremPts;
  static const SG::AuxElement::Accessor < std::vector<const Trk::TrackStateOnSurface*> > bremPoints("bremPoints");	  
  if (bremPoints.isAvailable(*elTrack)){
    bremPts =bremPoints(*elTrack) ;
  }
      
  for (unsigned int ib(0); ib < bremPts.size(); ++ib) {

    const Trk::EstimatedBremOnTrack *estimatedBremOnTrack(0);
    const Trk::TrackParameters      *bpTrackParams(0);
    if (!bremPts[ib]) {
      ATH_MSG_ERROR("Brem point vector has a null element???");
      return StatusCode::FAILURE;
    }	
    if (bremPts[ib]->materialEffectsOnTrack()) {
      estimatedBremOnTrack = (Trk::EstimatedBremOnTrack*)(bremPts[ib]->materialEffectsOnTrack());
    } else {
      ATH_MSG_DEBUG("No material effects!!!");
    }	
    if (estimatedBremOnTrack && bremPts[ib]->trackParameters() != 0) {

      float extrapEta(-999.), extrapPhi(-999.);
      bpTrackParams = bremPts[ib]->trackParameters();
      ATH_MSG_DEBUG("Track eta, phi at surface: (" << bpTrackParams->eta() << ", " << bpTrackParams->parameters()[Trk::phi0] << ")");
      const Trk::PerigeeSurface*  pSurface = new Trk::PerigeeSurface(bpTrackParams->position());
      const Trk::TrackParameters* pTrkPar  = pSurface->createTrackParameters( bpTrackParams->position(), bpTrackParams->momentum().unit()*1.e9, +1, 0);
      ATH_MSG_DEBUG("Perigee surface track eta, phi: (" << pTrkPar->eta() << ", " << pTrkPar->parameters()[Trk::phi0] << ")");
      //Do the straight-line extrapolation.	  
      bool hitEM2 = m_extrapolationTool->getEtaPhiAtCalo(pTrkPar,&extrapEta,&extrapPhi);
      if (hitEM2) {
	ATH_MSG_DEBUG("Extrap. eta, phi from brem surface: (" << extrapEta << ", " << extrapPhi << ")");
      } else {
	ATH_MSG_DEBUG("Extrapolation to EM2 failed!");
      }
      //Save info on % brem loss and extrapolated parameters.
      m_bpRetainedEnFrac.push_back(estimatedBremOnTrack->retainedEnFraction());
      if (hitEM2) {
	m_bpExtrapEta.push_back(extrapEta);
	m_bpExtrapPhi.push_back(extrapPhi);
      } else {
	m_bpExtrapEta.push_back(-999.);
	m_bpExtrapPhi.push_back(-999.);
      }
    } else {
      ATH_MSG_DEBUG("Cast to Trk::EstimatedBremOnTrack failed?");
    }
  }
  return StatusCode::SUCCESS;
}

bool electronSuperClusterBuilder::OverlapsABremPoint(const xAOD::CaloCluster *clus){
  bool  overlapsABremPoint(false);
  //A better way to do this would probably be to get the overlapping cluster
  //with minimum eta, phi.
  for (unsigned int ib(0); ib < m_bpExtrapEta.size(); ib++) {
    if (m_bpExtrapEta[ib] == -999. || m_bpExtrapPhi[ib] == -999.) {
      //ATH_MSG_DEBUG("Eta, phi of extrap. point not found, skipping this one ...");
      continue;
    }
    bool matchEta(false); 
    bool matchPhi(false); 
    if (clus->etaBE(2) > -100. && clus->phiBE(2) > -100.) {
      matchEta = fabs(m_bpExtrapEta[ib] - clus->etaBE(2)) < 0.1;
      matchPhi = fabs(P4Helpers::deltaPhi(m_bpExtrapPhi[ib], clus->phiBE(2))) < 0.15;
    } else {
      matchEta = fabs(m_bpExtrapEta[ib] - clus->eta()) < 0.1;
      matchPhi = fabs(P4Helpers::deltaPhi(m_bpExtrapPhi[ib], clus->phi())) < 0.15;
    }
    overlapsABremPoint = matchEta && matchPhi;
    if (overlapsABremPoint) {
      // retainedEnFrac = m_bpRetainedEnFrac[ib];
      ATH_MSG_DEBUG("Extrapolated brem point overlaps a potential secondary!");
      ATH_MSG_DEBUG("Extrap. eta: " << m_bpExtrapEta[ib] << ", cluster eta:" << clus->etaBE(2));
      ATH_MSG_DEBUG("Extrap. phi: " << m_bpExtrapPhi[ib] << ", cluster phi:" << clus->phiBE(2));
      ATH_MSG_DEBUG("dEta: " << fabs(m_bpExtrapEta[ib]-clus->etaBE(2)) << ", dPhi: " << fabs(P4Helpers::deltaPhi(m_bpExtrapPhi[ib], clus->phiBE(2))));
      //Should remove the brem point so we don't repeatedly use it.
      m_bpExtrapEta.erase(m_bpExtrapEta.begin()+ib);
      m_bpExtrapPhi.erase(m_bpExtrapPhi.begin()+ib);
      m_bpRetainedEnFrac.erase(m_bpRetainedEnFrac.begin()+ib);
      break;
    }else {
      ATH_MSG_VERBOSE("Extrapolated brem point missed a potential secondary");
      ATH_MSG_VERBOSE("Extrap. eta: " << m_bpExtrapEta[ib] << ", cluster eta:" << clus->etaBE(2));
      ATH_MSG_VERBOSE("Extrap. phi: " << m_bpExtrapPhi[ib] << ", cluster phi:" << clus->phiBE(2));
      ATH_MSG_VERBOSE("dEta: " << fabs(m_bpExtrapEta[ib]-clus->etaBE(2)) << ", dPhi: " << fabs(P4Helpers::deltaPhi(m_bpExtrapPhi[ib], clus->phiBE(2))));
    }
  } 
  return overlapsABremPoint;   
}

bool electronSuperClusterBuilder::MatchSameTrack(const egammaRec *seed,
						 const egammaRec *sec) const{
  bool matchesSameTrack(false);
  if (seed && sec) {

    float dEta(fabs(seed->caloCluster()->eta() - sec->caloCluster()->eta()));
    float dPhi(fabs(P4Helpers::deltaPhi(seed->caloCluster()->phi(), sec->caloCluster()->phi())));
      
    if (dEta > m_trackOverlapDelEta || dPhi > m_trackOverlapDelPhi){
      return false;
    }

    const xAOD::TrackParticle *seedTrack = seed->trackParticle();
    const xAOD::TrackParticle *secTrack  = sec ->trackParticle();
    if (seedTrack && secTrack) {
      //Check that the tracks are the same.
      if (seedTrack==secTrack) {
	ATH_MSG_DEBUG(Form("Track match! Clusters matched to same track."));
	matchesSameTrack = true;
      } else {
	ATH_MSG_DEBUG(Form("Track don't match: %f vs %f",seedTrack->pt(),secTrack->pt()));
      }
    }
  }
  if (matchesSameTrack){
    ATH_MSG_DEBUG("Clusters match the same track!");
  }
  return matchesSameTrack;
}

//Handles cases where we want to sum a cluster's cells,
//or add cells from another cluster.
void electronSuperClusterBuilder::AddEMCellsToCluster(xAOD::CaloCluster*       self,
						      const xAOD::CaloCluster* ref){
  if (!self || !ref) {
    ATH_MSG_DEBUG("Can't find clusters!");
    return;
  }
  xAOD::CaloCluster::const_cell_iterator cell_itr = ref->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = ref->end();
  //Need to check that the cell belongs to the EM calorimeter,
  for (; cell_itr != cell_end; ++cell_itr) { 
    const CaloCell* cell = *cell_itr; 
    if (!cell){
      continue;
    }
    //Add all LAR EM
    if (cell->caloDDE()->getSubCalo() == CaloCell_ID::LAREM) {
      self->addCell(cell_itr.index(), cell_itr.weight());
      m_cellsin3x5.push_back((*cell_itr));
    }
    //Add TileGap3. Consider only E4 cell
    if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) {
      if( fabs(cell->eta()) >1.4 && fabs(cell->eta()) < 1.6 ){
	self->addCell(cell_itr.index(), cell_itr.weight());
	m_cellsin3x5.push_back((*cell_itr));
      }
    }//TileGap
  }//Loop over cells
}

StatusCode electronSuperClusterBuilder::CalibrateCluster(xAOD::CaloCluster* newCluster,
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
    ATH_CHECK(m_MVACalibTool->execute(newCluster,egRec,xAOD::EgammaParameters::electron));
  }

  return StatusCode::SUCCESS;
}


// ==========================================================================
StatusCode electronSuperClusterBuilder::fillPositionsInCalo(xAOD::CaloCluster* cluster) 
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
