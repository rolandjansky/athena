/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTHitsNoiseMonAlg.h"
#include "SCT_NameFormatter.h"

#include "AthContainers/DataVector.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "StoreGate/ReadHandle.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"

#include <string>

using namespace SCT_Monitoring;

namespace {
  static const std::string abbreviations[N_REGIONS] = {
    "ECm", "", "ECp"
  };

  enum
  Pattern {
    IXX=0, XIX, XXI
  };
  
  // is the timebin in the desired pattern?
  bool timeBinInPattern(const int tbin, const Pattern xxx) {
    switch (xxx) {
    case IXX:
      return (tbin > 3);

      break;

    case XIX:
      return ((tbin == 2) or (tbin == 3) or (tbin == 6) or (tbin == 7));

      break;

    case XXI:
      return ((tbin == 1) or (tbin == 3) or (tbin == 5) or (tbin == 7));

      break;

    default:
      return false;

      break;
    }
  }
}


SCTHitsNoiseMonAlg::SCTHitsNoiseMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :AthMonitorAlgorithm(name,pSvcLocator)
{
}


StatusCode SCTHitsNoiseMonAlg::initialize() {

  ATH_CHECK(m_dataObjectName.initialize());
  ATH_CHECK(m_SCTSPContainerName.initialize());
  ATH_CHECK(m_dataObjectName.initialize());
  ATH_CHECK(m_clusContainerKey.initialize());
  ATH_CHECK(m_tracksName.initialize());

  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  // Get the helper:
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(m_ConfigurationTool.retrieve());

  if (m_pSCTHelper->wafer_hash_max()!=SCT_Monitoring::N_WAFERS) {
    ATH_MSG_ERROR("wafer_hash_max()=" << m_pSCTHelper->wafer_hash_max() <<
                  " differs from SCT_Monitoring::N_WAFERS=" << SCT_Monitoring::N_WAFERS);
    return StatusCode::RECOVERABLE;
  }
  return AthMonitorAlgorithm::initialize();
}


StatusCode SCTHitsNoiseMonAlg::fillHistograms(const EventContext& ctx) const {
  ++m_numberOfEvents; 
  ++m_numberOfEventsRecent;

  // If track hits are selected, make the vector of track rdo identifiers
  std::array<std::unordered_set<Identifier>, N_WAFERS> rdosOnTracks;
  if (m_doTrackHits) {
    if (makeVectorOfTrackRDOIdentifiers(rdosOnTracks, ctx).isFailure()) {
      ATH_MSG_WARNING("Couldn't make vector of track RDO identifiers");
    }
  }
  if (generalHistsandNoise(rdosOnTracks, ctx).isFailure()) {
    ATH_MSG_WARNING("Error in generalHists");
  }
  if (makeSPvsEventNumber().isFailure()) {
    ATH_MSG_WARNING("Error in makeSPvsEventNumber");
  }

  return StatusCode::SUCCESS;
}

StatusCode SCTHitsNoiseMonAlg::generalHistsandNoise(const std::array<std::unordered_set<Identifier>, N_WAFERS>& rdosOnTracks, const EventContext& ctx) const{
  static const unsigned int limits[N_REGIONS] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };

  const EventIDBase& pEvent{ctx.eventID()};
  const int lumi_block{static_cast<int>(pEvent.lumi_block())};
    
  SG::ReadHandle<SCT_RDO_Container> rdoContainer{m_dataObjectName, ctx};
  if (not rdoContainer.isValid()) {
    ATH_MSG_WARNING("SCT_RDO_Container not valid");
    return StatusCode::FAILURE;
  }
  // Get the space point container
  SG::ReadHandle<SpacePointContainer> spacePointContainer{m_SCTSPContainerName, ctx};
  if (not spacePointContainer.isValid()) {
    ATH_MSG_WARNING("SpacePointContainer not valid");
    return StatusCode::FAILURE;
  }

  bool isSelectedTrigger{false};
  // EDAVIES - have now changed back to using L1_RD0_EMPTY
  if (m_doTrigger) {
    if (AthMonitorAlgorithm::dataType() != AthMonitorAlgorithm::DataType_t::cosmics) {
      if (m_trigDecTool->isPassed(m_NOTriggerItem)) {
        isSelectedTrigger = true;
        ++m_numberOfEventsTrigger;
      }
    }
  }
  m_events_lb++;
  if (isSelectedTrigger) {
    m_eventsTrigger_lb++;
  }

  std::vector<float> occ(N_WAFERS, 0.);
  std::vector<float> hitOcc(N_WAFERS, 0.);

  int local_tothits{0};

  std::vector<int> barrel_local_nhitslayer(N_BARRELSx2, 0);
  std::vector<int> ECp_local_nhitslayer(N_DISKSx2, 0);
  std::vector<int> ECm_local_nhitslayer(N_DISKSx2, 0);
  std::vector<int>* hitsInLayer[N_REGIONS] = {
    &ECm_local_nhitslayer, &barrel_local_nhitslayer, &ECp_local_nhitslayer
  };
  const bool doThisSubsystem[N_REGIONS] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };
  // vectors to store data to decrease number of fill() calls for better perfomance 
  std::vector<int> vLumiBlock[N_REGIONS];
  std::vector<int> vNumberOfHitsFromAllRDOs[N_REGIONS];
  std::vector<int> vNumberOfHitsFromSPs[N_REGIONS];
  std::vector<bool> vIsSelectedTriggerHits[N_REGIONS];
  for (unsigned int jReg{0}; jReg<N_REGIONS; jReg++) {
    unsigned int size{0};
    if (jReg==ENDCAP_C_INDEX or jReg==ENDCAP_A_INDEX) size = N_SIDES * N_MOD_ENDCAPS;
    else if (jReg==BARREL_INDEX) size = N_SIDES * N_MOD_BARREL;
    vLumiBlock[jReg].reserve(size);
    vNumberOfHitsFromAllRDOs[jReg].reserve(size);
    vNumberOfHitsFromSPs[jReg].reserve(size);
    vIsSelectedTriggerHits[jReg].reserve(size);
  }

  std::vector<int> vEtaOnTrack;
  std::vector<int> vPhiOnTrack;
  std::vector<float> vSystemIndexOnTrack;
  std::vector<bool> vDTbinOnTrack;

  std::vector<int> vEta;
  std::vector<int> vPhi;
  std::vector<int> vNumberOfStrips;

  // Outer Loop on RDO Collection
  for (const InDetRawDataCollection<SCT_RDORawData>* rdoCollection: *rdoContainer) {
    // MJW new code- try getting the ID of the collection using the identify() method
    const Identifier wafer_id{rdoCollection->identify()};
    const IdentifierHash wafer_hash{rdoCollection->identifyHash()};
    const Identifier module_id{m_pSCTHelper->module_id(wafer_id)};
    const IdentifierHash theModuleHash0{m_pSCTHelper->wafer_hash(module_id)}; // This hash is not necessarily for side 0.
    IdentifierHash theModuleHash1; // This hash is not necessarily for side 1.
    m_pSCTHelper->get_other_side(theModuleHash0, theModuleHash1);
    const int barrel_ec{m_pSCTHelper->barrel_ec(wafer_id)};
    const unsigned int systemIndex{bec2Index(barrel_ec)};
    
    const int thisLayerDisk{m_pSCTHelper->layer_disk(wafer_id)};
    const int thisPhi{m_pSCTHelper->phi_module(wafer_id)};
    const int thisEta{m_pSCTHelper->eta_module(wafer_id)};
    const int thisSide{m_pSCTHelper->side(wafer_id)};
    const int thisElement{(N_SIDES * thisLayerDisk) + thisSide};

    LayerSideFormatter layerSide{static_cast<unsigned int>(thisElement), systemIndex};      

    int numberOfHitsFromSPs{0};
    int numberOfHitsFromAllRDOs{0};
    // Now we want the space point container for this module
    // We have to compare module IDs- the SP collection is defined for the 'normal' (i.e. no stereo) module side
    // Define a set of spIDs
    std::unordered_set<Identifier> mySetOfSPIds;
    for (int side{0}; side<N_SIDES; side++) {
      auto spContainerIterator{spacePointContainer->indexFindPtr(side==0 ? theModuleHash0 : theModuleHash1)};
      if (spContainerIterator==nullptr) continue;
      for (const Trk::SpacePoint* sp: *spContainerIterator) {
        const std::vector<Identifier>& rdoList{(thisSide==side ? sp->clusterList().first : sp->clusterList().second)->rdoList()};
        mySetOfSPIds.insert(rdoList.begin(), rdoList.end());
      }
    }

    vEtaOnTrack.clear();
    vPhiOnTrack.clear();
    vEta.clear();
    vPhi.clear();
    vNumberOfStrips.clear();
    
    // Now we loop over the RDOs in the RDO collection, and add to the NO vector any that are in the mySetOfSPIds
    for (const SCT_RDORawData* rdo: *rdoCollection) {
      const int numberOfStrips{rdo->getGroupSize()};
      (*hitsInLayer[systemIndex])[thisElement] += numberOfStrips;
      local_tothits += numberOfStrips;
      
      if (doThisSubsystem[systemIndex]) {
        const SCT3_RawData* rdo3{dynamic_cast<const SCT3_RawData*>(rdo)};
        int tbin{3};
        if (rdo3) {
          tbin = rdo3->getTimeBin();
        }
        const Identifier strip_id{rdo->identify()};
        const int firstStrip{m_pSCTHelper->strip(strip_id)};
        const int limit{firstStrip + numberOfStrips};

        if (rdosOnTracks[wafer_hash].find(strip_id) != rdosOnTracks[wafer_hash].end()) {
          for (int ichan{firstStrip}; ichan < limit; ++ichan) {
            vEtaOnTrack.push_back(thisEta);
            vPhiOnTrack.push_back(thisPhi);
            vDTbinOnTrack.push_back((tbin == 2) or (tbin == 3));
            vSystemIndexOnTrack.push_back(systemIndex);
          }
        }
        vEta.push_back(thisEta);
        vPhi.push_back(thisPhi);
        vNumberOfStrips.push_back(numberOfStrips);

        numberOfHitsFromAllRDOs += numberOfStrips;
        // Record number of hits in space points if timebin filtering is on hits not in bin X1X are counted as in space
        // points
        if (mySetOfSPIds.find(strip_id) != mySetOfSPIds.end()) {
          numberOfHitsFromSPs += numberOfStrips;
        } else if (m_doTimeBinFilteringForNoise and (not timeBinInPattern(tbin, XIX))) {
          numberOfHitsFromSPs += numberOfStrips;
        }
      }

    } // End of Loop on rdoCollection, so end of loop over the RDOs in the RDO container
      // We can now do the NO calculation for this wafer
      // For the Time Dependent plots

    const std::string streamhitmap{"mapsOfHitsOnTracks" + abbreviations[systemIndex] + "_" +
        "trackhitsmap_" + layerSide.name()};

    auto etaMapsOfHitsOnTracksAcc{Monitored::Collection("eta_"+streamhitmap, vEtaOnTrack)};
    auto phiMapsOfHitsOnTracksAcc{Monitored::Collection("phi_"+streamhitmap, vPhiOnTrack)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex), etaMapsOfHitsOnTracksAcc, phiMapsOfHitsOnTracksAcc);

    const std::string hitmap{"hitsmap" + abbreviations[systemIndex] + "_" + layerSide.name()};

    auto etahitsmapAcc{Monitored::Collection("eta_"+hitmap, vEta)};
    auto phihitsmapAcc{Monitored::Collection("phi_"+hitmap, vPhi)};
    auto numberOfStripsAcc{Monitored::Collection("numberOfStrips_"+hitmap, vNumberOfStrips)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex), etahitsmapAcc, phihitsmapAcc, numberOfStripsAcc);

    if (numberOfHitsFromAllRDOs > 0) {
      int den{N_STRIPS - numberOfHitsFromSPs};
      int num{numberOfHitsFromAllRDOs - numberOfHitsFromSPs};
      if (num < 0) {
        num = 0;
        ATH_MSG_WARNING("Too many reconstructed space points for number of real hits");
      }
      if (den > 0) {
        occ[wafer_hash] = static_cast<float>(num) / static_cast<float>(den) * 1.E5;
      }
    
      hitOcc[wafer_hash] = static_cast<float>(numberOfHitsFromAllRDOs) / static_cast<float>(N_STRIPS) * 1.E5;

      vLumiBlock[systemIndex].push_back(lumi_block);
      vNumberOfHitsFromAllRDOs[systemIndex].push_back(numberOfHitsFromAllRDOs);
      vNumberOfHitsFromSPs[systemIndex].push_back(numberOfHitsFromSPs);
      vIsSelectedTriggerHits[systemIndex].push_back(isSelectedTrigger);
      // end of hit occupancy
    }
  }// End of Loop on RDO container

  auto Bec_TBinFracAllAcc{Monitored::Collection("Bec_TBinFracAll", vSystemIndexOnTrack)};
  auto TBin_TBinFracAllAcc{Monitored::Collection("TBin_TBinFracAll", vDTbinOnTrack)};
  fill("SCTHitsNoiseMonitorGeneral", Bec_TBinFracAllAcc, TBin_TBinFracAllAcc);

  for (unsigned int jReg{0}; jReg<N_REGIONS; jReg++) {
    auto lbHitsAcc{Monitored::Collection("LBHits", vLumiBlock[jReg])};
    auto numberOfHitsFromAllRDOsAcc{Monitored::Collection("numberOfHitsFromAllRDOs", vNumberOfHitsFromAllRDOs[jReg])};
    auto numberOfHitsFromSPsAcc{Monitored::Collection("numberOfHitsFromSPs", vNumberOfHitsFromSPs[jReg])};
    auto isSelectedTriggerHitsAcc{Monitored::Collection("isSelectedTriggerHits", vIsSelectedTriggerHits[jReg])};
    fill("SCTHitsNoiseMonitor_" + std::to_string(jReg), lbHitsAcc, numberOfHitsFromAllRDOsAcc, numberOfHitsFromSPsAcc, isSelectedTriggerHitsAcc);
  }

  // Fill Cluster size histogram
  SG::ReadHandle<InDet::SCT_ClusterContainer> clusterContainer{m_clusContainerKey, ctx};
  if (not clusterContainer.isValid()) {
    ATH_MSG_WARNING("Couldn't retrieve clusters");
  }

  std::vector<long unsigned int> vGroupSize;
  for (const InDet::SCT_ClusterCollection* clusterCollection: *clusterContainer) {
    for (const InDet::SCT_Cluster* cluster: *clusterCollection) {
      vGroupSize.push_back(cluster->rdoList().size());
    }
  }
  auto cluSizeAcc{Monitored::Collection("clu_size", vGroupSize)};
  fill("SCTHitsNoiseMonitorGeneral", cluSizeAcc);
  auto hitsAcc{Monitored::Scalar<int>("sct_hits", local_tothits)};
  fill("SCTHitsNoiseMonitorGeneral", hitsAcc);

  // Fill hit occupancy and noise occupancy plots
  // vectors for storing the data and then use only one fill call to decrease time
  std::vector<int> vLB[N_REGIONS_INC_GENERAL];
  std::vector<float> vNO[N_REGIONS_INC_GENERAL];
  std::vector<float> vHO[N_REGIONS_INC_GENERAL];
  std::vector<bool> vIsSelectedTrigger[N_REGIONS_INC_GENERAL];
  std::vector<std::vector<float>> vNO2D[N_REGIONS];
  std::vector<std::vector<float>> vHO2D[N_REGIONS];
  std::vector<std::vector<int>> vEtaNOHO[N_REGIONS];
  std::vector<std::vector<int>> vPhiNOHO[N_REGIONS];
  std::vector<std::vector<bool>> vIsSelectedTriggerNOHO[N_REGIONS];
  for (unsigned int jReg{0}; jReg<N_REGIONS_INC_GENERAL; jReg++) {
    unsigned int size{N_WAFERS};
    if (jReg==ENDCAP_C_INDEX or jReg==ENDCAP_A_INDEX) size = N_SIDES * N_MOD_ENDCAPS;
    else if (jReg==BARREL_INDEX) size = N_SIDES * N_MOD_BARREL;
    vLB[jReg].reserve(size);
    vNO[jReg].reserve(size);
    vHO[jReg].reserve(size);
    vIsSelectedTrigger[jReg].reserve(size);

    if (jReg<GENERAL_INDEX) {
      vNO2D[jReg].resize(limits[jReg], {});
      vHO2D[jReg].resize(limits[jReg], {});
      vEtaNOHO[jReg].resize(limits[jReg], {});
      vPhiNOHO[jReg].resize(limits[jReg], {});
      vIsSelectedTriggerNOHO[jReg].resize(limits[jReg], {});
      for (unsigned int element{0}; element< limits[jReg]; ++element) {
        const int nWafers{getNumModules(jReg, element)*N_SIDES};
        vNO2D[jReg][element].reserve(nWafers);
        vHO2D[jReg][element].reserve(nWafers);
        vEtaNOHO[jReg][element].reserve(nWafers);
        vPhiNOHO[jReg][element].reserve(nWafers);
        vIsSelectedTriggerNOHO[jReg][element].reserve(nWafers);
      }
    }
  }  

  for (unsigned int iHash{0}; iHash<N_WAFERS; iHash++) {
    const IdentifierHash wafer_hash{iHash};
    if (not m_ConfigurationTool->isGood(wafer_hash)) continue;

    const Identifier wafer_id{m_pSCTHelper->wafer_id(wafer_hash)};
    const int barrel_ec{m_pSCTHelper->barrel_ec(wafer_id)};
    const unsigned int systemIndex{bec2Index(barrel_ec)};
    vLB[systemIndex].push_back(lumi_block);
    vNO[systemIndex].push_back(occ[iHash]);
    vHO[systemIndex].push_back(hitOcc[iHash]);
    vIsSelectedTrigger[systemIndex].push_back(isSelectedTrigger);
    vLB[GENERAL_INDEX].push_back(lumi_block);
    vNO[GENERAL_INDEX].push_back(occ[iHash]);
    vHO[GENERAL_INDEX].push_back(hitOcc[iHash]);
    vIsSelectedTrigger[GENERAL_INDEX].push_back(isSelectedTrigger);
    if (doThisSubsystem[systemIndex]) {
      const int element{N_SIDES * m_pSCTHelper->layer_disk(wafer_id) + m_pSCTHelper->side(wafer_id)};
      vNO2D[systemIndex][element].push_back(occ[iHash]);
      vHO2D[systemIndex][element].push_back(hitOcc[iHash]);
      vEtaNOHO[systemIndex][element].push_back(m_pSCTHelper->eta_module(wafer_id));
      vPhiNOHO[systemIndex][element].push_back(m_pSCTHelper->phi_module(wafer_id));
      vIsSelectedTriggerNOHO[systemIndex][element].push_back(isSelectedTrigger);
    }
  }

  for (unsigned int jReg{0}; jReg<N_REGIONS_INC_GENERAL; jReg++) {
    std::string monitor;
    if (jReg==GENERAL_INDEX) monitor = "SCTHitsNoiseMonitorGeneral";
    else monitor = "SCTHitsNoiseMonitor_" + std::to_string(jReg);

    auto LBAcc{Monitored::Collection("LB", vLB[jReg])};
    auto noAcc{Monitored::Collection("NO", vNO[jReg])};
    auto hoAcc{Monitored::Collection("HO", vHO[jReg])};
    auto IsSelectedTriggerAcc{Monitored::Collection("IsSelectedTrigger", vIsSelectedTrigger[jReg])};
    fill(monitor, LBAcc, noAcc, hoAcc, IsSelectedTriggerAcc);
  }

  for (unsigned int jReg{0}; jReg<N_REGIONS; ++jReg){
    for (unsigned int element{0}; element < limits[jReg]; ++element) {
      LayerSideFormatter layerSide{element, jReg};
      const std::string occMap{"occupancymap" + abbreviations[jReg] + "_" + layerSide.name()};
      auto etaEacc{Monitored::Collection("eta_" + occMap, vEtaNOHO[jReg][element])};
      auto phiAcc{Monitored::Collection("phi_" + occMap, vPhiNOHO[jReg][element])};
      auto hoAcc{Monitored::Collection("HO_" + occMap, vHO2D[jReg][element])};
      auto noAcc{Monitored::Collection("NO_" + occMap, vNO2D[jReg][element])};
      auto isSelectedTriggerAcc{Monitored::Collection("IsSelectedTrigger_"+occMap, vIsSelectedTriggerNOHO[jReg][element])};
      fill("SCTHitsNoiseMonitor_" + std::to_string(jReg), etaEacc, phiAcc, hoAcc, noAcc, isSelectedTriggerAcc);
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode SCTHitsNoiseMonAlg::makeVectorOfTrackRDOIdentifiers(std::array<std::unordered_set<Identifier>, N_WAFERS>& rdosOnTracks, const EventContext& ctx) const{
  // Clear the rdosOnTracks vector
  rdosOnTracks.fill(std::unordered_set<Identifier>());
  SG::ReadHandle<SCT_RDO_Container> rdoContainer{m_dataObjectName, ctx};
  if (not rdoContainer.isValid()) {
    ATH_MSG_FATAL("Could not find the data object " << m_dataObjectName.key() << " !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Data object " << m_dataObjectName.key() << " found");
  }
  
  SG::ReadHandle<TrackCollection> tracks{m_tracksName, ctx};
  if (not tracks.isValid()) {
    ATH_MSG_FATAL("No tracks for you!");
    return StatusCode::FAILURE;
  }
  // Only do for events with less than some number of tracks
  if (tracks->size() > m_maxTracks) {
    ATH_MSG_DEBUG("The event has more than " << m_maxTracks
                  << " tracks. Don't do hits-on-track-hists");
    return StatusCode::SUCCESS;
  }
  // assemble list of rdo ids associated with tracks
  for (const Trk::Track* track : *tracks) {
    if (track == nullptr) {
      ATH_MSG_WARNING("no pointer to track!!!");
      break;
    }
    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface>* trackStates{track->trackStateOnSurfaces()};
    if (trackStates == nullptr) {
      ATH_MSG_WARNING("for current track is TrackStateOnSurfaces == Null, no data will be written for this track");
    } else {// Loop over all track states on surfaces
      for (const Trk::TrackStateOnSurface* TSOS: *trackStates) {
        // Get pointer to RIO of right type
        const InDet::SiClusterOnTrack* clus{dynamic_cast<const InDet::SiClusterOnTrack*>(TSOS->measurementOnTrack())};
        if (clus) {
          // Get Pointer to prepRawDataObject
          const InDet::SiCluster* RawDataClus{dynamic_cast<const InDet::SiCluster*>(clus->prepRawData())};
          if (RawDataClus == nullptr) {
            ATH_MSG_WARNING("SiCluster WITHOUT prepRawData!!!!");
            break;
          }
          // if Cluster is in SCT ...
          if (RawDataClus->detectorElement()->isSCT()) {
            const std::vector<Identifier>& rdoList{RawDataClus->rdoList()};
            rdosOnTracks[RawDataClus->detectorElement()->identifyHash()].insert(rdoList.begin(), rdoList.end());
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SCTHitsNoiseMonAlg::makeSPvsEventNumber() const{
  return StatusCode::SUCCESS;    
}
