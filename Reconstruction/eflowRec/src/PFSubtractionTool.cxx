#include "eflowRec/PFSubtractionTool.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCaloObjectMaker.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/IEFlowCellEOverPTool.h"
#include "eflowRec/PFClusterFiller.h"
#include "eflowRec/PFSubtractionEnergyRatioCalculator.h"
#include "eflowRec/PFSubtractionStatusSetter.h"
#include "eflowRec/eflowSubtractor.h"
#include "eflowRec/PFTrackFiller.h"
#include "eflowRec/PFCalcRadialEnergyProfiles.h"

using namespace eflowSubtract;

PFSubtractionTool::PFSubtractionTool(const std::string &type, const std::string &name, const IInterface *parent) : base_class(type, name, parent),
                                                                                                                   m_binnedParameters(std::make_unique<eflowEEtaBinnedParameters>())
{
}

PFSubtractionTool::~PFSubtractionTool()
{
}

StatusCode PFSubtractionTool::initialize()
{

  ATH_CHECK(m_theEOverPTool.retrieve());

  ATH_CHECK(m_theEOverPTool->fillBinnedParameters(m_binnedParameters.get()));

  m_trkpos.reset(dynamic_cast<PFMatch::TrackEtaPhiInFixedLayersProvider *>(PFMatch::TrackPositionFactory::Get("EM2EtaPhi").release()));
  if (!m_trkpos.get())
  {
    ATH_MSG_ERROR("Failed to get TrackPositionProvider for cluster preselection!");
    return StatusCode::FAILURE;
  }

  //Retrieve track-cluster matching tools
  ATH_CHECK(m_theMatchingTool.retrieve());
  ATH_CHECK(m_theMatchingToolForPull_015.retrieve());
  ATH_CHECK(m_theMatchingToolForPull_02.retrieve());

  return StatusCode::SUCCESS;
}

void PFSubtractionTool::execute(eflowCaloObjectContainer *theEflowCaloObjectContainer, eflowRecTrackContainer *recTrackContainer, eflowRecClusterContainer *recClusterContainer) const
{

  ATH_MSG_DEBUG("Executing");

  PFData data;
  data.caloObjects = theEflowCaloObjectContainer;
  const PFTrackFiller pfTrackFiller;
  if (!m_recoverSplitShowers) pfTrackFiller.fillTracksToConsider(data, *recTrackContainer);
  else pfTrackFiller.fillTracksToRecover(data);

  const PFClusterFiller pfClusterFiller;
  if (!m_recoverSplitShowers) pfClusterFiller.fillClustersToConsider(data, *recClusterContainer);
  else pfClusterFiller.fillClustersToRecover(data);

  ATH_MSG_DEBUG("This event has " << data.tracks.size() << " tracks " << data.clusters.size() << " clusters ");

  unsigned int numMatches = matchAndCreateEflowCaloObj(data);

  if (msgLvl(MSG::DEBUG)) printAllClusters(*recClusterContainer);

  if (!m_calcEOverP){
    if (!m_recoverSplitShowers) performSubtraction(0,data);
    else performSubtraction(numMatches,data);
  }
  else{
    PFCalcRadialEnergyProfiles pfCalc;
    pfCalc.calculate(data);
  }

}

unsigned int PFSubtractionTool::matchAndCreateEflowCaloObj(PFData &data) const{

  //Counts up how many tracks found at least 1 calorimeter cluster matched to it.
  unsigned int nMatches(0);

  /* Cache the original number of eflowCaloObjects, if there were any */
  const unsigned int nCaloObj = data.caloObjects->size();
  const EventContext &ctx = Gaudi::Hive::currentContext();

  /* loop tracks in data.tracks and do matching */
  for (auto thisEfRecTrack : data.tracks)
  {
    /** No point to do anything if e/p reference bin does not exist */
    if (!thisEfRecTrack->hasBin()) {
      std::unique_ptr<eflowCaloObject> thisEflowCaloObject = std::make_unique<eflowCaloObject>();
      thisEflowCaloObject->addTrack(thisEfRecTrack);
      data.caloObjects->push_back(std::move(thisEflowCaloObject));
      continue;
    }

    if (msgLvl(MSG::DEBUG))
    {
      const xAOD::TrackParticle *track = thisEfRecTrack->getTrack();
      ATH_MSG_DEBUG("Matching track with e,pt, eta and phi " << track->e() << ", " << track->pt() << ", " << track->eta() << " and " << track->phi());
    }

    std::vector<eflowTrackClusterLink*> bestClusters;

    if (!m_recoverSplitShowers){
      /** Add cluster matches needed for pull calculation (in eflowCaloObject::simulateShowers) which is used to determine whether to run the charged shower subtraction or not.
      / Clusters in both a cone of 0.15 and 0.2 are needed for this.
      / The clusters in a cone of 0.2 are also used as the matched cluster list for recover split showers mode.    
      **/
      std::vector<std::pair<eflowRecCluster *, float>> bestClusters_02 = m_theMatchingToolForPull_02->doMatches(thisEfRecTrack, data.clusters, -1);
      for (auto &matchpair : bestClusters_02)
      {
        eflowRecCluster *theCluster = matchpair.first;
        float distancesq = matchpair.second;
        eflowTrackClusterLink *trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack, theCluster, ctx);
        if (distancesq < 0.15 * 0.15)
        {
          // Narrower cone is a subset of the selected clusters
          // Distance returned is deltaR^2
          thisEfRecTrack->addAlternativeClusterMatch(trackClusterLink, "cone_015");
        }
        thisEfRecTrack->addAlternativeClusterMatch(trackClusterLink, "cone_02");
      }//loop over bestClusters_02

      //This matching scheme is used to match the calorimeter cluster(s) to be used in the charged showers subtraction for this track.
      std::vector<std::pair<eflowRecCluster *, float>> matchedClusters = m_theMatchingTool->doMatches(thisEfRecTrack, data.clusters,m_nClusterMatchesToUse);    
      for (auto thePair : matchedClusters) bestClusters.push_back(eflowTrackClusterLink::getInstance(thisEfRecTrack, thePair.first, ctx));     
    }
    else {
      const std::vector<eflowTrackClusterLink*>* matchedClusters_02 = thisEfRecTrack->getAlternativeClusterMatches("cone_02");
      if (!matchedClusters_02) continue;
      else bestClusters = *matchedClusters_02;
    }

    if (bestClusters.empty()) continue;

    if (msgLvl(MSG::DEBUG))
    {
      for (auto thisClusterLink : bestClusters ) {
        xAOD::CaloCluster* thisCluster = thisClusterLink->getCluster()->getCluster();
        ATH_MSG_DEBUG("Matched this track to cluster with e,pt, eta and phi " << thisCluster->e() << ", " << thisCluster->pt() << ", " << thisCluster->eta() << " and " << thisCluster->phi());
      }
    }

    nMatches++;

    //loop over the matched calorimeter clusters and associate tracks and clusters to each other as needed.
    for (auto *trkClusLink : bestClusters){

      eflowRecCluster *thisEFRecCluster = trkClusLink->getCluster();

      if (m_recoverSplitShowers){
        // Look up whether this cluster is intended for recovery
        if (std::find(data.clusters.begin(), data.clusters.end(), trkClusLink->getCluster()) == data.clusters.end()) continue;       
      }

      eflowTrackClusterLink *trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack, thisEFRecCluster, ctx);
      thisEfRecTrack->addClusterMatch(trackClusterLink);
      thisEFRecCluster->addTrackMatch(trackClusterLink);
    }

  }

  /* Create 3 types eflowCaloObjects: track-only, cluster-only, track-cluster-link */
  std::vector<eflowRecCluster *> clusters(data.clusters.begin(), data.clusters.end());
  if (m_recoverSplitShowers) std::sort(clusters.begin(), clusters.end(), eflowRecCluster::SortDescendingPt());
  unsigned int nCaloObjects = eflowCaloObjectMaker::makeTrkCluCaloObjects(data.tracks, clusters, data.caloObjects);
  ATH_MSG_DEBUG("Created  " << nCaloObjects << " eflowCaloObjects.");
  if (msgLvl(MSG::DEBUG)){
    for (auto thisEFlowCaloObject : *(data.caloObjects)){
      ATH_MSG_DEBUG("This eflowCaloObject has " << thisEFlowCaloObject->nTracks() << " tracks and " << thisEFlowCaloObject->nClusters() << " clusters ");
      for (unsigned int count = 0; count < thisEFlowCaloObject->nTracks(); count++){
        const xAOD::TrackParticle* thisTrack = thisEFlowCaloObject->efRecTrack(count)->getTrack();
        ATH_MSG_DEBUG("Have track with e, pt, eta and phi of " << thisTrack->e() << ", " << thisTrack->pt() << ", " << thisTrack->eta() << " and " << thisTrack->phi());
      }
      for (unsigned int count = 0; count < thisEFlowCaloObject->nClusters(); count++){
        const xAOD::CaloCluster* thisCluster = thisEFlowCaloObject->efRecCluster(count)->getCluster();
        ATH_MSG_DEBUG("Have cluster with e, pt, eta and phi of " << thisCluster->e() << ", " << thisCluster->pt() << ", " << thisCluster->eta() << " and " << thisCluster->phi());
      }
    }
  }

  const double gaussianRadius = 0.032;
  const double gaussianRadiusError = 1.0e-3;
  const double maximumRadiusSigma = 3.0;

  eflowLayerIntegrator integrator(gaussianRadius, gaussianRadiusError, maximumRadiusSigma, m_isHLLHC);

  /** Start loop from nCaloObj, which should be zero on a first pass */
  if (!m_recoverSplitShowers && 0 != nCaloObj) ATH_MSG_WARNING("Not in Split Showers Mode and already have " << nCaloObj << " eflowCaloObjects");

  //For each eflowCaloObject we calculate the expected energy deposit in the calorimeter and cell ordering for subtraction.  
  for (unsigned int iCalo = nCaloObj; iCalo < data.caloObjects->size(); ++iCalo) {  
    eflowCaloObject* thisEflowCaloObject = data.caloObjects->at(iCalo);
    thisEflowCaloObject->simulateShower(&integrator, m_binnedParameters.get(), true);        
  }

  if (!m_recoverSplitShowers) return nMatches;
  else return nCaloObj;
}

void PFSubtractionTool::performSubtraction(const unsigned int& startingPoint,PFData &data) const{
  unsigned int nEFCaloObs = data.caloObjects->size();
  for (unsigned int iCalo = startingPoint; iCalo < nEFCaloObs; ++iCalo) {
    eflowCaloObject* thisEflowCaloObject = data.caloObjects->at(iCalo);
    this->performSubtraction(*thisEflowCaloObject);
  }
}

void PFSubtractionTool::performSubtraction(eflowCaloObject& thisEflowCaloObject) const{

  ATH_MSG_DEBUG("In performSubtraction");

  //These are used to set whether a track was subtracted and also to calculate how much energy was subtracted
  PFSubtractionStatusSetter pfSubtractionStatusSetter;
  PFSubtractionEnergyRatioCalculator pfSubtractionEnergyRatioCalculator;
  if (msgLevel(MSG::DEBUG))
  {
    pfSubtractionStatusSetter.setLevel(MSG::DEBUG);
    pfSubtractionEnergyRatioCalculator.setLevel(MSG::DEBUG);
  }

  unsigned int nClusters = thisEflowCaloObject.nClusters();
  unsigned int nTrackMatches = thisEflowCaloObject.nTracks();

  ATH_MSG_DEBUG("Have got an eflowCaloObject with " << nClusters << " clusters and " << nTrackMatches << " track matches");

  if (msgLevel(MSG::DEBUG)){
    for (unsigned int iTrack = 0; iTrack < nTrackMatches; ++iTrack){
       eflowRecTrack* thisTrack = thisEflowCaloObject.efRecTrack(iTrack);
       ATH_MSG_DEBUG("eflowCaloObject has track with E, pt and eta " << thisTrack->getTrack()->e() << ", " << thisTrack->getTrack()->pt() << " and " << thisTrack->getTrack()->eta());
    }
  }
  
  //To keep historical behaviour when in recover split showers mode allow tracks with no cluster matches to proceed.
  if (!m_recoverSplitShowers && nClusters < 1) return;  

  //Need at least one track in this eflowCaloObject to continue.
  if (nTrackMatches < 1) return;

  double expectedEnergy = thisEflowCaloObject.getExpectedEnergy();
  double clusterEnergy = thisEflowCaloObject.getClusterEnergy();
  double expectedSigma = sqrt(thisEflowCaloObject.getExpectedVariance());

  /* Check e/p, if on first pass - return if e/p not consistent with expected e/p */
  if (!m_recoverSplitShowers){
    if (isEOverPFail(expectedEnergy, expectedSigma, clusterEnergy)) return;
  }
  
  const std::vector<std::pair<eflowTrackClusterLink *, std::pair<float, float>>> &matchedTrackList = thisEflowCaloObject.efRecLink();

  ATH_MSG_DEBUG("Matched Track List has size " << matchedTrackList.size());

  if (msgLevel(MSG::DEBUG))
  {
    for (unsigned int iTrack = 0; iTrack < nTrackMatches; ++iTrack)
    {
      const xAOD::TrackParticle *thisTrack = thisEflowCaloObject.efRecTrack(iTrack)->getTrack();      
      ATH_MSG_DEBUG("eflowCaloObject has track match with E, pt and eta " << thisTrack->e() << ", " << thisTrack->pt() << " and " << thisTrack->eta());
    }
  }

  ATH_MSG_DEBUG("About to perform subtraction for this eflowCaloObject");

  bool wasAnnihilated = false;

  //First deal with non-split showers mode
  if (!m_recoverSplitShowers){
    /* Check if we can annihilate right away - true if matched cluster has only the expected energy deposit */
    if (canAnnihilate(expectedEnergy, expectedSigma, clusterEnergy)){

      wasAnnihilated = true;

      std::vector<std::pair<xAOD::CaloCluster *, bool>> clusterList;
      std::map<xAOD::CaloCluster *, double> clusterEnergyMap;
      unsigned nCluster = thisEflowCaloObject.nClusters();
      for (unsigned iCluster = 0; iCluster < nCluster; ++iCluster){
        clusterList.emplace_back(thisEflowCaloObject.efRecCluster(iCluster)->getCluster(), false);
      }

      ATH_MSG_DEBUG("We are going to annihilate. ExpectedEnergy, expectedSigma and clusterEnergy are " << expectedEnergy << ", " << expectedSigma << " and " << clusterEnergy);
      if (msgLevel(MSG::DEBUG))
        for (auto thisPair : clusterList)
          ATH_MSG_DEBUG("Annihilating cluster with E and eta " << thisPair.first->e() << " and " << thisPair.first->eta());

      pfSubtractionStatusSetter.markAllTracksAnnihStatus(thisEflowCaloObject);
      Subtractor::annihilateClusters(clusterList);

      if (msgLevel(MSG::DEBUG))
        for (auto thisPair : clusterList)
          ATH_MSG_DEBUG("Have Annihilated cluster with E and eta " << thisPair.first->e() << " and " << thisPair.first->eta());
      
      /* Flag all tracks in this system as subtracted */
      for (unsigned iTrack = 0; iTrack < thisEflowCaloObject.nTracks(); ++iTrack){
        eflowRecTrack *thisEfRecTrack = (matchedTrackList[iTrack].first)->getTrack();
        if (!thisEfRecTrack->isSubtracted()) thisEfRecTrack->setSubtracted();
      }

    }//if can annihilate this track-cluster systems matched cluster
  }//split shower recovery mode or regular mode where above annihilation was not triggered
  if (m_recoverSplitShowers || !wasAnnihilated){

    for (unsigned iTrack = 0; iTrack < thisEflowCaloObject.nTracks(); ++iTrack){

      eflowRecTrack *thisEfRecTrack = thisEflowCaloObject.efRecTrack(iTrack);

      ATH_MSG_DEBUG("About to subtract track with e, pt, eta and phi of " << thisEfRecTrack->getTrack()->e() << ", " << thisEfRecTrack->getTrack()->pt() << ", " << thisEfRecTrack->getTrack()->eta() << " and "
                                                                       << thisEfRecTrack->getTrack()->eta());
      
      if (!thisEfRecTrack->hasBin()) continue;

      ATH_MSG_DEBUG("Have bin for this eflowCaloObject");

      if (thisEfRecTrack->isInDenseEnvironment() && !m_recoverSplitShowers) continue;

      ATH_MSG_DEBUG("Am not in dense environment for this eflowCaloObject");

      /* Get matched cluster via Links */
      std::vector<eflowRecCluster *> matchedClusters;
      std::vector<eflowTrackClusterLink *> links = thisEfRecTrack->getClusterMatches();
      for (auto *thisEFlowTrackClusterLink : links)
        matchedClusters.push_back(thisEFlowTrackClusterLink->getCluster());
      if (m_recoverSplitShowers) std::sort(matchedClusters.begin(), matchedClusters.end(), eflowRecCluster::SortDescendingPt());

      if (msgLvl(MSG::DEBUG)){
        for (auto *thisClus : matchedClusters)
          ATH_MSG_DEBUG("Haved matched cluster " << thisClus->getCluster()->index() << " with e,pt, eta and phi of " << thisClus->getCluster()->e() << ", " << thisClus->getCluster()->pt() << ", " << thisClus->getCluster()->eta() << " and " << thisClus->getCluster()->phi() << " will be subtracted");
      }

      /* Do subtraction */
      std::vector<std::pair<xAOD::CaloCluster *, bool>> clusterSubtractionList;
      clusterSubtractionList.reserve(matchedClusters.size());
      std::map<xAOD::CaloCluster *, double> clusterEnergyMap;
      for (auto *thisEFlowRecCluster : matchedClusters){
        xAOD::CaloCluster *thisCluster = thisEFlowRecCluster->getCluster();
        clusterSubtractionList.emplace_back(thisCluster, false);
        clusterEnergyMap[thisCluster] = thisCluster->e();
      }

      ATH_MSG_DEBUG("Have filled clusterSubtractionList for this eflowCaloObject");

      unsigned int trackIndex = thisEfRecTrack->getTrack()->index();

      //Previously we only checked this in recover split showers, but makes sense to check it in both passes.
      auto sumClusEnergy = [](double accumulator, std::pair<xAOD::CaloCluster *, bool> thisPair){ return accumulator += thisPair.first->e();};
      double totalClusterEnergy = std::accumulate(clusterSubtractionList.begin(),clusterSubtractionList.end(),0.0,sumClusEnergy);      

      /* Check if we can annihilate right away - true if matched cluster has only the expected energy deposit */
      if(canAnnihilate(thisEfRecTrack->getEExpect(),sqrt(thisEfRecTrack->getVarEExpect()),totalClusterEnergy)){
        
        if (msgLevel(MSG::DEBUG))
          for (auto thisPair : clusterSubtractionList)
            ATH_MSG_DEBUG("Annihilating cluster with E and eta " << thisPair.first->e() << " and " << thisPair.first->eta());

        Subtractor::annihilateClusters(clusterSubtractionList);
        //Now we should mark all of these clusters as being subtracted
        //Now need to mark which clusters were modified in the subtraction procedure
        std::vector<std::pair<float, float>> clusterSubtractedEnergyRatios;
        pfSubtractionEnergyRatioCalculator.calculateSubtractedEnergyRatiosForAnnih(clusterSubtractionList, clusterEnergyMap, clusterSubtractedEnergyRatios);
        pfSubtractionStatusSetter.markSubtractionStatus(clusterSubtractionList, clusterSubtractedEnergyRatios, thisEflowCaloObject, trackIndex);
      }
      else
      {

        /* Subtract the track from all matched clusters */
        const bool debugToggle = msgLvl(MSG::DEBUG);
        Subtractor::subtractTracksFromClusters(thisEfRecTrack, clusterSubtractionList, debugToggle);

        //recalculate total cluster energy from the clusters afer subtraction
        totalClusterEnergy = std::accumulate(clusterSubtractionList.begin(),clusterSubtractionList.end(),0.0,sumClusEnergy);        

        /* Annihilate the cluster(s) if the remnant is small (i.e. below k*sigma) */
        if (canAnnihilate(0.0,sqrt(thisEfRecTrack->getVarEExpect()), totalClusterEnergy)){

          if (msgLevel(MSG::DEBUG))
          for (auto thisPair : clusterSubtractionList)
            ATH_MSG_DEBUG("Annihilating remnant cluster with E and eta " << thisPair.first->e() << " and " << thisPair.first->eta());
          Subtractor::annihilateClusters(clusterSubtractionList);
          //Now we should mark all of these clusters as being subtracted
          std::vector<std::pair<float, float>> clusterSubtractedEnergyRatios;
          pfSubtractionEnergyRatioCalculator.calculateSubtractedEnergyRatiosForAnnih(clusterSubtractionList, clusterEnergyMap, clusterSubtractedEnergyRatios);
          pfSubtractionStatusSetter.markSubtractionStatus(clusterSubtractionList, clusterSubtractedEnergyRatios, thisEflowCaloObject, trackIndex);
        }//if remove the remnant after cell by cell subtraction
        else
        { 
          std::vector<std::pair<float, float>> clusterSubtractedEnergyRatios;
          pfSubtractionEnergyRatioCalculator.calculateSubtractedEnergyRatios(clusterSubtractionList, clusterEnergyMap, clusterSubtractedEnergyRatios);
          pfSubtractionStatusSetter.markSubtractionStatus(clusterSubtractionList, clusterSubtractedEnergyRatios, thisEflowCaloObject, trackIndex);
        }//if don't remove the remnant after cell by cell subtraction

      }//if not annihilating, and instead subtracting cell by cell

      ATH_MSG_DEBUG("Have subtracted charged shower for this eflowRecTrack");

      /* Flag tracks as subtracted */
      if (!thisEfRecTrack->isSubtracted()) thisEfRecTrack->setSubtracted();

    }//loop over tracks in eflowCaloObject
  }//cell by cell subtraction

}

bool PFSubtractionTool::isEOverPFail(double expectedEnergy, double sigma, double clusterEnergy) const
{
  if ((expectedEnergy == 0) && (clusterEnergy > 0)) return false;
  return clusterEnergy < expectedEnergy - m_consistencySigmaCut * sigma;
}

bool PFSubtractionTool::canAnnihilate(double expectedEnergy, double sigma, double clusterEnergy) const
{
  return clusterEnergy - expectedEnergy < m_subtractionSigmaCut * sigma;
}

std::string PFSubtractionTool::printTrack(const xAOD::TrackParticle* track) {
  std::stringstream result;
  result << " track with E, eta and phi "<< track->e() << ", " << track->eta() << " and " << track->phi();
  return result.str();
}

std::string PFSubtractionTool::printCluster(const xAOD::CaloCluster* cluster) {
  std::stringstream result;
  result << " cluster with E, eta and phi of " << cluster->e() << ", " << cluster->eta() << " and " << cluster->phi();
  return result.str();
}

void PFSubtractionTool::printAllClusters(const eflowRecClusterContainer& recClusterContainer) const {

  for ( auto thisEFRecCluster : recClusterContainer){
    if (thisEFRecCluster->getTrackMatches().empty()) {
      ATH_MSG_DEBUG("Isolated" << printCluster(thisEFRecCluster->getCluster()));
    } else {
      ATH_MSG_DEBUG("Matched" << printCluster(thisEFRecCluster->getCluster()));
      std::vector<eflowTrackClusterLink*> theTrackLinks = thisEFRecCluster->getTrackMatches();
      for ( auto thisTrack : theTrackLinks){
       ATH_MSG_DEBUG("Matched" << printTrack(thisTrack->getTrack()->getTrack()));
      }
    }
  }
}


StatusCode PFSubtractionTool::finalize() { return StatusCode::SUCCESS; }
