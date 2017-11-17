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

#include "CxxUtils/make_unique.h"
#include <memory>

using CLHEP::GeV;

//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
electronSuperClusterBuilder::electronSuperClusterBuilder(const std::string& type,
							 const std::string& name,
							 const IInterface* parent) :
  egammaSuperClusterBuilder(type, name, parent),
  m_nWindowClusters(0),
  m_nExtraClusters(0),
  m_nSameTrackClusters(0),
  m_nSimpleBremSearchClusters(0)
{

  ////////////////////////////////////////////////////////////////////////////////
  //Search parameters.
  //Window we search in 
  declareProperty("MaxWindowDelEtaCells", m_maxDelEtaCells = 5);
  declareProperty("MaxWindowDelPhiCells", m_maxDelPhiCells = 12);
  m_maxDelPhi = m_maxDelPhiCells * s_cellPhiSize * 0.5;
  m_maxDelEta = m_maxDelEtaCells * s_cellEtaSize * 0.5;
  //For the Brem matching
  declareProperty("BremExtrapDelEtaCut",  m_bremExtrapMatchDelEta = 0.05);
  declareProperty("BremExtrapDelPhiCut",  m_bremExtrapMatchDelPhi = 0.1);
  declareProperty("BremSearchEOverPCut",  m_secEOverPCut = 1.5);

  //Containers
  declareProperty("InputEgammaRecContainerName", m_inputEgammaRecContainerName = "egammaRecCollection");
  declareProperty("SuperElectronRecCollectionName", m_electronSuperRecCollectionName = "ElectronSuperRecCollection");
  declareProperty("SuperClusterCollestionName",  m_outputElectronSuperClusters  = "ElectronSuperClusters");
  //
  declareProperty("NumberOfReqSiHits", m_numberOfSiHits = 4);

  // Declare interface & properties.
  declareInterface<IelectronSuperClusterBuilder>(this);
}

StatusCode electronSuperClusterBuilder::initialize() {
  ATH_MSG_DEBUG(" Initializing electronSuperClusterBuilder");

  ATH_CHECK(egammaSuperClusterBuilder::initialize());

  m_maxDelPhi = m_maxDelPhiCells * s_cellPhiSize * 0.5;
  m_maxDelEta = m_maxDelEtaCells * s_cellEtaSize * 0.5;

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

  return executeImpl(egammaRecs, newEgammaRecs, outputClusterContainer);
}

StatusCode electronSuperClusterBuilder::hltExecute(const EgammaRecContainer *egammaRecs,
                                                   EgammaRecContainer *newEgammaRecs,
                                                   xAOD::CaloClusterContainer *outputClusterContainer) {

  return executeImpl(egammaRecs, newEgammaRecs, outputClusterContainer);
}

StatusCode electronSuperClusterBuilder::executeImpl(const EgammaRecContainer *egammaRecs,
                                                    EgammaRecContainer *newEgammaRecs,
                                                    xAOD::CaloClusterContainer *outputClusterContainer) {
  //Reserve a vector to keep track of what is used
  std::vector<bool> isUsed (egammaRecs->size(),0);
  //Loop over input egammaRec objects, build superclusters.
  for (std::size_t i = 0 ; i < egammaRecs->size();++i) {    
    //Used to revert status of topos 
    //in case we fail to make a supercluser.
    std::vector<bool> isUsedRevert(isUsed);
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
    //Require minimum energy for supercluster seeding.
    if (egRec->caloCluster()->et()*emFrac < m_EtThresholdCut){
      continue;
    }
    //Get the associated tracks
    if (egRec->getNumberOfTrackParticles()==0) {
      continue;
    }
    //
    uint8_t trkPixelHits(0);
    uint8_t uint8_value(0);
    //
    //use both Pixel and Dead Pixel ala the EMTrackMatchBuilder + offline cuts
    if (egRec->trackParticle(0)->summaryValue(uint8_value,  xAOD::numberOfPixelDeadSensors)){
      trkPixelHits+=uint8_value;
    }
    if (egRec->trackParticle(0)->summaryValue(uint8_value,  xAOD::numberOfPixelHits)){
      trkPixelHits+=uint8_value;
    }    
    if (!trkPixelHits){
      continue;
    }
    //Check if it is TRT standalone
    if (xAOD::EgammaHelpers::numberOfSiHits(egRec->trackParticle(0)) < m_numberOfSiHits){
      continue;
    }
    //
    //Counters to keep tracks why we added the clusters
    m_nWindowClusters=0;
    m_nExtraClusters=0;
    m_nSameTrackClusters=0;
    m_nSimpleBremSearchClusters=0;
    //
    //=========================================================================================
    ATH_MSG_DEBUG("=====================================================================");
    ATH_MSG_DEBUG("Creating supercluster egammaRec electron object");
    ATH_MSG_DEBUG("Using cluster Et = " << egRec->caloCluster()->et() << " eta " << egRec->caloCluster()->eta() 
		  << " phi "<< egRec->caloCluster()->phi()  << " EMFraction " << emFrac << " EM Et " 
		  << egRec->caloCluster()->et()*emFrac << " pixel hits " << static_cast<unsigned int> (trkPixelHits));    
    //Mark as used
    isUsed.at(i)=true;     
    //
    //Add the seed as the 1st entry in the secondaries list 
    ATH_MSG_DEBUG("Push back the existing egRec caloCluster");
    std::vector<const xAOD::CaloCluster*> accumulatedClusters;
    accumulatedClusters.push_back(egRec->caloCluster());
    //
    //Find Secondary Clusters
    ATH_MSG_DEBUG("Find secondary clusters");
    const std::vector<std::size_t>  secondaryIndices = SearchForSecondaryClusters(i, egammaRecs, isUsed);
    //
    //Append possible accumulated  clusters.
    ATH_MSG_DEBUG("Add secondary clusters");
    for(const auto& secIndex : secondaryIndices){
      const auto secRec = egammaRecs->at(secIndex);
      accumulatedClusters.push_back(secRec->caloCluster());
    }
    //
    ATH_MSG_DEBUG("Total clusters " << accumulatedClusters.size());
    //
    //Take the full list of cluster , now stored in the secondaries and add their cells together
    xAOD::CaloCluster *newClus = CreateNewCluster(accumulatedClusters,xAOD::EgammaParameters::electron);
    //
    if (!newClus) {
      ATH_MSG_DEBUG("Creating a new cluster failed - skipping it");
      //Revert status of constituent clusters.
      isUsed = isUsedRevert;
      continue;
    }
    //
    ATH_MSG_DEBUG("extra clusters: " << m_nExtraClusters);
    ATH_MSG_DEBUG("extra clusters: " << m_nWindowClusters);
    ATH_MSG_DEBUG("Same Track clusters: " << m_nSameTrackClusters);
    ATH_MSG_DEBUG("Simple Brem Search clusters:  " << m_nSimpleBremSearchClusters);
    //
    //////////////////////////////////////////////////////////////////
    //Push back the new cluster into the output container.
    outputClusterContainer->push_back(newClus);
    ElementLink< xAOD::CaloClusterContainer > clusterLink(newClus, *outputClusterContainer);
    std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink};
    //
    ////////////////////////////////////////////////////////////////////
    //Make egammaRec object, and push it back into output container.
    egammaRec *newEgRec = new egammaRec(*egRec);
    if (newEgRec) {
      newEgRec->setCaloClusters  (elClusters);
      newEgammaRecs->push_back(newEgRec);
      ATH_MSG_DEBUG("Made new egammaRec object");
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

    //Now the actual checks
    //Check if clusters are nearby enough to form the "topo-seeded window.'
    bool matchesInWindow(MatchesInWindow(seedCluster,clus));
    if (matchesInWindow){
      ATH_MSG_DEBUG("Cluster  with Et : " << clus->et()<< " matched in window");
      ++ m_nWindowClusters;
    }    

    //Satellite brem cluster search for clusters
    //outside the 3x5 window.
    bool passesSimpleBremSearch(false);
    bool matchSameTrack        (false);

    float seedSecdEta(fabs(seedCluster->eta() - clus->eta()));
    float seedSecdPhi(fabs(P4Helpers::deltaPhi(seedCluster->phi(), clus->phi())));

    if (!matchesInWindow 
	&& seedSecdEta<m_maxDelEta 
	&& seedSecdPhi<m_maxDelPhi) {      
      matchSameTrack = MatchSameTrack(seedEgammaRec,egRec);
      
      if(matchSameTrack) {
	++m_nSameTrackClusters;
	++m_nExtraClusters;
      } else {
	if (egRec->trackParticle()) {
	  float qoverp    = egRec->trackParticle()->qOverP();
	  float seedEOverP(egRec->caloCluster()->e() / fabs(1./qoverp));
	  
	  static const SG::AuxElement::Accessor<float> pgExtrapEta ("perigeeExtrapEta");
	  static const SG::AuxElement::Accessor<float> pgExtrapPhi ("perigeeExtrapPhi");
	  float perigeeExtrapEta (pgExtrapEta(*egRec->trackParticle()));
	  float perigeeExtrapPhi (pgExtrapPhi(*egRec->trackParticle()));
	  if (perigeeExtrapEta>-999. && perigeeExtrapPhi>-999.)
	    passesSimpleBremSearch = PassesSimpleBremSearch(seedCluster,
							    clus,
							    perigeeExtrapEta,
							    perigeeExtrapPhi,
							    seedEOverP);
	  if (passesSimpleBremSearch) {
	    ++m_nSimpleBremSearchClusters;
	    ++m_nExtraClusters;
	  }
	}
      }
    }
    
    //Add it to the list of secondary clusters if it matches.
    if (matchesInWindow || passesSimpleBremSearch || matchSameTrack) {
      secondaryClusters.push_back(i); 
      isUsed.at(i)=1;
    }
  }

  ATH_MSG_DEBUG("Found: " << secondaryClusters.size()<< " secondaries");
  return secondaryClusters;
}

bool electronSuperClusterBuilder::MatchSameTrack(const egammaRec *seed,
						 const egammaRec *sec) const{
  bool matchesSameTrack(false);
  if (seed && sec) {
    const xAOD::TrackParticle *seedTrack = seed->trackParticle();
    const xAOD::TrackParticle *secTrack  = sec ->trackParticle();
    if (seedTrack && secTrack) {
      //Check that the tracks are the same.
      if (seedTrack==secTrack) {
	ATH_MSG_DEBUG("Track match! Clusters matched to same track.");
	matchesSameTrack = true;
      } else {
	ATH_MSG_DEBUG(" Track does not  match seed " <<seedTrack->pt() << " secondary "<< secTrack->pt());
      }
    }
  }
  if (matchesSameTrack){
    ATH_MSG_DEBUG("Clusters match the same track!");
  }
  return matchesSameTrack;
}

bool electronSuperClusterBuilder::PassesSimpleBremSearch(const xAOD::CaloCluster *seed,
							 const xAOD::CaloCluster *sec,
							 float perigeeExtrapEta,
							 float perigeeExtrapPhi,
							 float seedEOverP) const
{
  if (seedEOverP > m_secEOverPCut){
    return false;
  }

  float perigeeExtrapClusDelEta = fabs(seed->eta() - perigeeExtrapEta);
  float perigeeExtrapClusDelPhi = fabs(P4Helpers::deltaPhi(seed->phi(), perigeeExtrapPhi));
  float perigeeExtrapSecClusDelEta = fabs(sec->eta() - perigeeExtrapEta);
  float perigeeExtrapSecClusDelPhi = fabs(P4Helpers::deltaPhi(sec->phi(), perigeeExtrapPhi));

  // determine window size, barrel or EC
  auto searchWindowEta = m_searchWindowEtaEndcap;
  auto searchWindowPhi = m_searchWindowPhiEndcap;
  if (xAOD::EgammaHelpers::isBarrel(seed)) {
    searchWindowEta = m_searchWindowEtaBarrel;
    searchWindowPhi = m_searchWindowPhiBarrel;
  }

  if (perigeeExtrapClusDelEta > searchWindowEta && perigeeExtrapClusDelPhi > searchWindowPhi && 
      perigeeExtrapSecClusDelEta < m_bremExtrapMatchDelEta && 
      perigeeExtrapSecClusDelPhi < m_bremExtrapMatchDelPhi) {
    return true;
  }

  return false;
}
