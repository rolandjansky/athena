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

using namespace std;

namespace {
  static const string abbreviations[N_REGIONS] = {
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
  array<unordered_set<Identifier>, N_WAFERS> rdosOnTracks;
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

StatusCode SCTHitsNoiseMonAlg::generalHistsandNoise(const array<unordered_set<Identifier>, N_WAFERS>& rdosOnTracks, const EventContext& ctx) const{
  static const unsigned int limits[N_REGIONS] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };

  const EventIDBase& pEvent{ctx.eventID()};
  int lumi_block{static_cast<int>(pEvent.lumi_block())};

  //postprocessing hists are filled with dummy 
  int test = 2; 
  int test2 = 1;
  int test3 = 1;
  
  //General
    
  auto NO_NO_vsLBAccGeneral{Monitored::Scalar<int>("NO_NO_vsLB", test3)};
  auto LB_NO_vsLBAccGeneral{Monitored::Scalar<int>("LB_NO_vsLB", test)};
  fill("SCTHitsNoiseMonitorGeneral",LB_NO_vsLBAccGeneral,NO_NO_vsLBAccGeneral);
       
  auto NO_NOTrigger_vsLBAccGeneral{Monitored::Scalar<int>("NO_NOTrigger_vsLB", test3)};
  auto LB_NOTrigger_vsLBAccGeneral{Monitored::Scalar<int>("LB_NOTrigger_vsLB", test)};
  fill("SCTHitsNoiseMonitorGeneral",LB_NOTrigger_vsLBAccGeneral,NO_NOTrigger_vsLBAccGeneral);
  
  auto HO_HO_vsLBAccGeneral{Monitored::Scalar<int>("HO_HO_vsLB", test3)};
  auto LB_HO_vsLBAccGeneral{Monitored::Scalar<int>("LB_HO_vsLB", test)};
  fill("SCTHitsNoiseMonitorGeneral",LB_HO_vsLBAccGeneral,HO_HO_vsLBAccGeneral);
       
  auto HO_HOTrigger_vsLBAccGeneral{Monitored::Scalar<int>("HO_HOTrigger_vsLB", test3)};
  auto LB_HOTrigger_vsLBAccGeneral{Monitored::Scalar<int>("LB_HOTrigger_vsLB", test)};
  fill("SCTHitsNoiseMonitorGeneral",LB_HOTrigger_vsLBAccGeneral,HO_HOTrigger_vsLBAccGeneral);
    
  for (unsigned int isub = 0; isub<N_REGIONS; ++isub){
      
    auto HO_HO_vsLBAcc{Monitored::Scalar<int>("HO_HO_vsLB", test3)};
    auto LB_HO_vsLBAcc{Monitored::Scalar<int>("LB_HO_vsLB", test)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(isub),LB_HO_vsLBAcc,HO_HO_vsLBAcc);

    auto HO_HOTrigger_vsLBAcc{Monitored::Scalar<int>("HO_HOTrigger_vsLB", test3)};
    auto LB_HOTrigger_vsLBAcc{Monitored::Scalar<int>("LB_HOTrigger_vsLB", test)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(isub),LB_HOTrigger_vsLBAcc,HO_HOTrigger_vsLBAcc);
    

    auto NO_NO_vsLBAcc{Monitored::Scalar<int>("NO_NO_vsLB", test3)};
    auto LB_NO_vsLBAcc{Monitored::Scalar<int>("LB_NO_vsLB", test)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(isub),LB_NO_vsLBAcc,NO_NO_vsLBAcc);
       
    auto NO_NOTrigger_vsLBAcc{Monitored::Scalar<int>("NO_NOTrigger_vsLB", test3)};
    auto LB_NOTrigger_vsLBAcc{Monitored::Scalar<int>("LB_NOTrigger_vsLB", test)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(isub),LB_NOTrigger_vsLBAcc,NO_NOTrigger_vsLBAcc);
    
       
    for (unsigned int i{0}; i < limits[isub]; ++i) {

      LayerSideFormatter layerSide{i, isub};
      const string streamhitmapR{"hitoccupancymap" + abbreviations[isub] + "_" + layerSide.name()};
      auto EtahitoccupancymapAcc{Monitored::Scalar<int>("eta_" + streamhitmapR, test)};    
      auto PhihitoccupancymapAcc{Monitored::Scalar<int>("phi_" + streamhitmapR, test2)};
      auto OcchitoccupancymapAcc{Monitored::Scalar<int>("occ_" + streamhitmapR, test3)};
      fill("SCTHitsNoiseMonitor_" + std::to_string(isub),EtahitoccupancymapAcc,PhihitoccupancymapAcc,OcchitoccupancymapAcc);
           
           
      const string noiseOccupancy{"noiseoccupancymaptrigger" + abbreviations[isub] + "_" + layerSide.name()};
      auto EtanoiseoccupancymapAcc{Monitored::Scalar<int>("eta_" + noiseOccupancy, test)};    
      auto PhinoiseoccupancymapAcc{Monitored::Scalar<int>("phi_" + noiseOccupancy, test2)};
      auto OccnoiseoccupancymapAcc{Monitored::Scalar<int>("occ_" + noiseOccupancy, test3)};
      fill("SCTHitsNoiseMonitor_" + std::to_string(isub),EtanoiseoccupancymapAcc,PhinoiseoccupancymapAcc,OccnoiseoccupancymapAcc);
            
    } 
        
  }
  
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

  vector<int> barrel_local_nhitslayer(N_BARRELSx2, 0);
  vector<int> ECp_local_nhitslayer(N_DISKSx2, 0);
  vector<int> ECm_local_nhitslayer(N_DISKSx2, 0);
  vector<int>* hitsInLayer[N_REGIONS] = {
    &ECm_local_nhitslayer, &barrel_local_nhitslayer, &ECp_local_nhitslayer
  };
  const bool doThisSubsystem[N_REGIONS] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };

  vector<int> vLumiBlock[N_REGIONS];
  vector<int> vNumberOfHitsFromAllRDOs[N_REGIONS];
  vector<int> vNumberOfHitsFromSPs[N_REGIONS];
  vector<int> vLumiBlockTrigger[N_REGIONS];
  vector<int> vNumberOfHitsFromAllRDOsTrigger[N_REGIONS];
  vector<int> vNumberOfHitsFromSPsTrigger[N_REGIONS];
  for (unsigned int jReg{0}; jReg<N_REGIONS; jReg++) {
    vLumiBlock[jReg].reserve(N_WAFERS);
    vNumberOfHitsFromAllRDOs[jReg].reserve(N_WAFERS);
    vNumberOfHitsFromSPs[jReg].reserve(N_WAFERS);
    vLumiBlockTrigger[jReg].reserve(N_WAFERS);
    vNumberOfHitsFromAllRDOsTrigger[jReg].reserve(N_WAFERS);
    vNumberOfHitsFromSPsTrigger[jReg].reserve(N_WAFERS);
  }

  vector<int> vEtaOnTrack;
  vector<int> vPhiOnTrack;
  vector<float> vBec0p5OnTrack;
  vector<bool> vDTbinOnTrack;

  vector<int> vEta;
  vector<int> vPhi;
  vector<int> vNumberOfStrips;

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
    unordered_set<Identifier> mySetOfSPIds;
    for (int side{0}; side<N_SIDES; side++) {
      SpacePointContainer::const_iterator spContainerIterator{spacePointContainer->indexFind(side==0 ? theModuleHash0 : theModuleHash1)};
      if (spContainerIterator==spacePointContainer->end()) continue;
      for (const Trk::SpacePoint* sp: **spContainerIterator) {
        const vector<Identifier>& rdoList{(thisSide==side ? sp->clusterList().first : sp->clusterList().second)->rdoList()};
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
            vBec0p5OnTrack.push_back(barrel_ec+0.5);
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

    const string streamhitmap{"mapsOfHitsOnTracks" + abbreviations[systemIndex] + "_" +
        "trackhitsmap_" + layerSide.name()};

    auto etaMapsOfHitsOnTracksAcc{Monitored::Collection("eta_"+streamhitmap, vEtaOnTrack)};
    auto phiMapsOfHitsOnTracksAcc{Monitored::Collection("phi_"+streamhitmap, vPhiOnTrack)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex), etaMapsOfHitsOnTracksAcc, phiMapsOfHitsOnTracksAcc);

    const string hitmap{"hitsmap" + abbreviations[systemIndex] + "_" + layerSide.name()};

    auto etahitsmapAcc{Monitored::Collection("eta_"+hitmap, vEta)};
    auto phihitsmapAcc{Monitored::Collection("phi_"+hitmap, vPhi)};
    auto numberOfStripsAcc{Monitored::Collection("numberOfStrips_"+hitmap, vNumberOfStrips)};
    fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex), etahitsmapAcc, phihitsmapAcc, numberOfStripsAcc);

    if (numberOfHitsFromAllRDOs > 0) {
      /*
      int diff{numberOfHitsFromAllRDOs - numberOfHitsFromSPs};
      int num{diff};
      int den{N_STRIPS - numberOfHitsFromSPs};
      float sumocc{0.};
      if (diff < 0) {
        num = 0;
        m_skipEvents++;
        ATH_MSG_WARNING("Too many reconstructed space points for number of real hits");
      }
      if (den > 0) {
        sumocc = num / static_cast<float> (den);
      }
      */ // Will be used later
    
      // hit occupancy
          
      //      float sumhitocc{static_cast<float> (numberOfHitsFromAllRDOs) / static_cast<float> (N_STRIPS)}; // Will be used later

      vLumiBlock[systemIndex].push_back(lumi_block);
      vNumberOfHitsFromAllRDOs[systemIndex].push_back(numberOfHitsFromAllRDOs);
      vNumberOfHitsFromSPs[systemIndex].push_back(numberOfHitsFromSPs);
        
      if (isSelectedTrigger) {
        vLumiBlockTrigger[systemIndex].push_back(lumi_block);
        vNumberOfHitsFromAllRDOsTrigger[systemIndex].push_back(numberOfHitsFromAllRDOs);
        vNumberOfHitsFromSPsTrigger[systemIndex].push_back(numberOfHitsFromSPs);
      }
      // end of hit occupancy
    }
  }// End of Loop on RDO container

  auto Bec_TBinFracAllAcc{Monitored::Collection("Bec_TBinFracAll", vBec0p5OnTrack)};
  auto TBin_TBinFracAllAcc{Monitored::Collection("TBin_TBinFracAll", vDTbinOnTrack)};
  fill("SCTHitsNoiseMonitorGeneral", Bec_TBinFracAllAcc, TBin_TBinFracAllAcc);

  for (unsigned int jReg{0}; jReg<N_REGIONS; jReg++) {
    auto lbHits_vsLBAcc{Monitored::Collection("lbh_Hits_vsLB", vLumiBlock[jReg])};
    auto numhitsHallHits_vsLBAcc{Monitored::Collection("numhitsh_HallHits_vsLB", vNumberOfHitsFromAllRDOs[jReg])};
    auto numberOfHitsFromSPsHSPHits_vsLBAcc{Monitored::Collection("numberOfHitsFromSPsh_HSPHits_vsLB", vNumberOfHitsFromSPs[jReg])};
    fill("SCTHitsNoiseMonitor_" + std::to_string(jReg), lbHits_vsLBAcc, numberOfHitsFromSPsHSPHits_vsLBAcc, numhitsHallHits_vsLBAcc);

    auto lbHitsTrigger_vsLBAcc{Monitored::Collection("lbh_HitsTrigger_vsLB", vLumiBlockTrigger[jReg])};
    auto numhitsHallHitsTrigger_vsLBAcc{Monitored::Collection("numhitsh_HallHitsTrigger_vsLB", vNumberOfHitsFromAllRDOsTrigger[jReg])};
    auto numberOfHitsFromSPsHSPHitsTrigger_vsLBAcc{Monitored::Collection("numberOfHitsFromSPsh_HSPHitsTrigger_vsLB", vNumberOfHitsFromSPsTrigger[jReg])};
    fill("SCTHitsNoiseMonitor_" + std::to_string(jReg), lbHitsTrigger_vsLBAcc, numberOfHitsFromSPsHSPHitsTrigger_vsLBAcc, numhitsHallHitsTrigger_vsLBAcc);
  }

  // if (m_environment!=AthMonitorAlgorithm::Environment_t::online) { // Uncomment this line to turn off cluster hists in online
  // Fill Cluster size histogram
  SG::ReadHandle<InDet::SCT_ClusterContainer> clusterContainer{m_clusContainerKey, ctx};
  if (not clusterContainer.isValid()) {
    ATH_MSG_WARNING("Couldn't retrieve clusters");
  }

  vector<long unsigned int> vGroupSize;
  for (const InDet::SCT_ClusterCollection* clusterCollection: *clusterContainer) {
    for (const InDet::SCT_Cluster* cluster: *clusterCollection) {
      vGroupSize.push_back(cluster->rdoList().size());
    }
  }
  auto cluSizeAcc{Monitored::Collection("clu_size", vGroupSize)};
  fill("SCTHitsNoiseMonitorGeneral", cluSizeAcc);
  // } // Uncomment this line as well to turn off cluster hists in online
  
  return StatusCode::SUCCESS;
}


StatusCode SCTHitsNoiseMonAlg::makeVectorOfTrackRDOIdentifiers(array<unordered_set<Identifier>, N_WAFERS>& rdosOnTracks, const EventContext& ctx) const{
  // Clear the rdosOnTracks vector
  rdosOnTracks.fill(unordered_set<Identifier>());
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
            const vector<Identifier>& rdoList{RawDataClus->rdoList()};
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
