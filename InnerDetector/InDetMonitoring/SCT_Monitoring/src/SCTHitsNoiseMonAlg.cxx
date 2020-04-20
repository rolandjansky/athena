/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTHitsNoiseMonAlg.h"
#include "SCT_NameFormatter.h"

#include "AthContainers/DataVector.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/InDetRawDataContainer.h" // ?
#include "InDetRawData/InDetRawDataCLASS_DEF.h" // ?
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "StoreGate/ReadHandle.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"

#include "GaudiKernel/IJobOptionsSvc.h"

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


const double SCTHitsNoiseMonAlg::s_thresholds[]{100., 1000., 10000.};
const string SCTHitsNoiseMonAlg::s_thresholdNames[]{"100", "1000", "10000"};

SCTHitsNoiseMonAlg::SCTHitsNoiseMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :AthMonitorAlgorithm(name,pSvcLocator),
   m_data (std::make_unique<Data>())
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
  for (unsigned int hash{0}; hash<SCT_Monitoring::N_WAFERS; hash++) {
    m_data->m_occSumUnbiased[hash] = 0.;
    m_data->m_occSumUnbiasedTrigger[hash] = 0.;
    m_data->m_occSumUnbiasedRecent[hash] = 0.;
  for (unsigned int jReg{0}; jReg<N_REGIONS+1; jReg++) {
      m_data->m_occSumUnbiased_lb[jReg][hash] = 0.;
      m_data->m_occSumUnbiasedTrigger_lb[jReg][hash] = 0.;
    }
    m_data->m_hitoccSumUnbiased[hash] = 0.;
    m_data->m_hitoccSumUnbiasedTrigger[hash] = 0.;
    m_data->m_hitoccSumUnbiasedRecent[hash] = 0.;
    for (unsigned int jReg{0}; jReg<N_REGIONS+1; jReg++) {
      m_data->m_hitoccSumUnbiased_lb[jReg][hash] = 0.;
      m_data->m_hitoccSumUnbiasedTrigger_lb[jReg][hash] = 0.;
    }
  }
  return AthMonitorAlgorithm::initialize();
}


StatusCode SCTHitsNoiseMonAlg::fillHistograms(const EventContext& ctx) const {
  ctx.evt();
  const EventIDBase& pEvent{ctx.eventID()};
  int tmp_lb{static_cast<int>(pEvent.lumi_block())};
  if (tmp_lb < m_current_lb) {
    // I am too late! We already moved on to the next LB!
    return StatusCode::SUCCESS;
  }

  ++m_numberOfEvents; 
  ++m_numberOfEventsRecent;
  if ((tmp_lb > m_current_lb) and (m_current_lb<NBINS_LBs)) {
    std::unique_lock lockUnique( m_sharedMutex );
    if ((tmp_lb > m_current_lb) and (m_current_lb<NBINS_LBs)) {
      m_current_lb = pEvent.lumi_block();
      // Once Per LB 
      resetCaches();
      
    }
  }
  m_current_lb = pEvent.lumi_block();
  // If track hits are selected, make the vector of track rdo identifiers
  std::vector<Identifier> rdosOnTracks;
  if (m_doTrackHits) {
    if (makeVectorOfTrackRDOIdentifiers(rdosOnTracks, ctx).isFailure()) {
      ATH_MSG_WARNING("Couldn't make vector of track RDO identifiers");
    }
  }
  if (generalHistsandNoise(rdosOnTracks).isFailure()) {
    ATH_MSG_WARNING("Error in generalHists");
  }
  if (makeSPvsEventNumber().isFailure()) {
    ATH_MSG_WARNING("Error in makeSPvsEventNumber");
  }

  /* postprocessing not working yet
   if (m_environment == AthenaMonManager::online) {
    if ((m_numberOfEvents % m_checkrate) == 0) {
      if (checkNoiseMaps(true).isFailure()) {
        ATH_MSG_WARNING("Error in checkNoiseMaps(true)");
      }
    }
    if ((m_current_lb % m_checkrecent == 0) and (m_current_lb > m_last_reset_lb)) {
      if (checkNoiseMaps(true).isFailure()) {
        ATH_MSG_WARNING("Error in checkNoiseMaps(true)");
      }
      if (resetNoiseMapsRecent().isFailure()) {
        ATH_MSG_WARNING("Error in resetNoiseMapsRecent");
      }
      if (resetHitMapHists().isFailure()) {
        ATH_MSG_WARNING("Error in resetHitMapsRecent");
      }
      m_last_reset_lb = m_current_lb;
      m_numberOfEventsRecent = 0;
    }
  }*/


  return StatusCode::SUCCESS;
}

StatusCode SCTHitsNoiseMonAlg::generalHistsandNoise(const std::vector<Identifier>& rdosOnTracks) const{
  static const unsigned int limits[N_REGIONS] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };
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
  
  SG::ReadHandle<SCT_RDO_Container> p_rdocontainer = SG::makeHandle(m_dataObjectName); 
  if (not p_rdocontainer.isValid()) {
    ATH_MSG_WARNING("SCT_RDO_Container not valid");
    return StatusCode::FAILURE;
  }
  // Get the space point container
  SG::ReadHandle<SpacePointContainer> sctContainer{m_SCTSPContainerName};
  if (not sctContainer.isValid()) {
    ATH_MSG_WARNING("SpacePointContainer not valid");
    return StatusCode::FAILURE;
  }
  Identifier SCT_Identifier;
  // Use new IDC
  int local_tothits{0};
  int totalmodules{0};
  int maxrodhits{0};
  int maxhits{0};
  Identifier maxhits_Id;
  int minhits{99999};
  Identifier minhits_Id;

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
  // Outer Loop on RDO Collection
  for (const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection: *p_rdocontainer) {
    if (SCT_Collection==nullptr) {
      continue;  // select only SCT RDOs
    }
    // MJW new code- try getting the ID of the collection using the identify() method
    Identifier tempID{SCT_Collection->identify()};
    Identifier theWaferIdentifierOfTheRDOCollection{tempID};
    Identifier theModuleIdentifierOfTheRDOCollection{m_pSCTHelper->module_id(tempID)};
    IdentifierHash theModuleHash0{m_pSCTHelper->wafer_hash(theModuleIdentifierOfTheRDOCollection)}; // This hash is not necessarily for side 0.
    IdentifierHash theModuleHash1; // This hash is not necessarily for side 1.
    m_pSCTHelper->get_other_side(theModuleHash0, theModuleHash1);
    int Bec{m_pSCTHelper->barrel_ec(tempID)};
    
    int numberOfHitsFromSPs{0};
    int numberOfHitsFromAllRDOs{0};
    // Now we want the space point container for this module
    // We have to compare module IDs- the SP collection is defined for the 'normal' (i.e. no stereo) module side
    // Define a set of spIDs
    set<Identifier> mySetOfSPIds;
    for (unsigned int side{0}; side<N_SIDES; side++) {
      SpacePointContainer::const_iterator spContainerIterator{sctContainer->indexFind(side==0 ? theModuleHash0 : theModuleHash1)};
      if (spContainerIterator==sctContainer->end()) continue;
      Identifier tempSPID{(*spContainerIterator)->identify()};
      Identifier theModuleIdentifierOfTheSPCollection{m_pSCTHelper->module_id(tempSPID)};
      if (theModuleIdentifierOfTheRDOCollection == theModuleIdentifierOfTheSPCollection) {
        for (const Trk::SpacePoint* sp: **spContainerIterator) {
          // the following is nasty; the 'normal' sides (where the sp is defined) swap from layer to layer. To be safe,
          // we get both sides
          const vector<Identifier>& rdoList0{sp->clusterList().first->rdoList()};
          const vector<Identifier>& rdoList1{sp->clusterList().second->rdoList()};
          // copy to mySetOfSPIds. Use inserter(set, iterator_hint) for a set, or back_inserter(vec) for vector...
          copy(rdoList0.begin(), rdoList0.end(), inserter(mySetOfSPIds, mySetOfSPIds.end()));
          copy(rdoList1.begin(), rdoList1.end(), inserter(mySetOfSPIds, mySetOfSPIds.end()));
        }
      } else {
        ATH_MSG_ERROR("Module identifiers are different. indexFind gives a wrong collection??");
      }
    }
    totalmodules++;
    maxrodhits = 0;
    std::shared_lock lockShared( m_sharedMutex ); 
    // Now we loop over the RDOs in the RDO collection, and add to the NO vector any that are in the mySetOfSPIds
    for (const SCT_RDORawData* rdo: *SCT_Collection) {
      const SCT3_RawData* rdo3{dynamic_cast<const SCT3_RawData*>(rdo)};
      int tbin{3};
      if (rdo3) {
        tbin = rdo3->getTimeBin();
      }
      SCT_Identifier = rdo->identify();
      const int firstStrip{m_pSCTHelper->strip(SCT_Identifier)};
      const int numberOfStrips{rdo->getGroupSize()};
      int thisBec{m_pSCTHelper->barrel_ec(SCT_Identifier)};
      int thisLayerDisk{m_pSCTHelper->layer_disk(SCT_Identifier)};
      int thisPhi{m_pSCTHelper->phi_module(SCT_Identifier)};
      int thisEta{m_pSCTHelper->eta_module(SCT_Identifier)};
      int thisSide{m_pSCTHelper->side(SCT_Identifier)};
      int thisElement{(N_SIDES * thisLayerDisk) + thisSide};
      // CAM adds mod id needed for map
      int chan{firstStrip};
      int limit{chan + numberOfStrips};
      unsigned int systemIndex{bec2Index(thisBec)};
      // CAM wafer id was here
      
      LayerSideFormatter layerSide{static_cast<unsigned int>(thisElement), systemIndex};

      if (doThisSubsystem[systemIndex]) {
        
        if (find(rdosOnTracks.begin(), rdosOnTracks.end(), SCT_Identifier) != rdosOnTracks.end()) {
          for (int ichan{chan}; ichan < limit; ++ichan) {
            
            const string streamhitmap{"mapsOfHitsOnTracks" + abbreviations[systemIndex] + "_" +
               "trackhitsmap_" + layerSide.name()};
            auto etaMapsOfHitsOnTracksAcc{Monitored::Scalar<int>("eta_"+streamhitmap, thisEta)};
            auto phiMapsOfHitsOnTracksAcc{Monitored::Scalar<int>("phi_"+streamhitmap, thisPhi)};
            fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex),etaMapsOfHitsOnTracksAcc,phiMapsOfHitsOnTracksAcc);
           
            double dTbin{static_cast<double>((tbin == 2) or (tbin == 3))};
            auto Bec_TBinFracAllAcc{Monitored::Scalar<float>("Bec_TBinFracAll", thisBec+0.5)};
            auto TBin_TBinFracAllAcc{Monitored::Scalar<float>("TBin_TBinFracAll", dTbin)};
            fill("SCTHitsNoiseMonitorGeneral",Bec_TBinFracAllAcc,TBin_TBinFracAllAcc);
       
            if (m_environment == AthMonitorAlgorithm::Environment_t::online) {
              
              fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex),etaMapsOfHitsOnTracksAcc,phiMapsOfHitsOnTracksAcc);
            }
          }
        }
        string hitmap{"hitsmap" + abbreviations[systemIndex] + "_" + layerSide.name()};
        auto etahitsmapAcc{Monitored::Scalar<int>("eta_"+hitmap, thisEta)};
        auto phihitsmapAcc{Monitored::Scalar<int>("phi_"+hitmap, thisPhi)};
        auto numberOfStripsAcc{Monitored::Scalar<int>("numberOfStrips_"+hitmap, numberOfStrips)};
        fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex),etahitsmapAcc,phihitsmapAcc,numberOfStripsAcc);

        numberOfHitsFromAllRDOs += numberOfStrips;
        // Record number of hits in space points if timebin filtering is on hits not in bin X1X are counted as in space
        // points
        if (mySetOfSPIds.find(SCT_Identifier) != mySetOfSPIds.end()) {
          numberOfHitsFromSPs += numberOfStrips;
        } 
        else if (m_doTimeBinFilteringForNoise and (not timeBinInPattern(tbin, XIX))) {
          numberOfHitsFromSPs += numberOfStrips;
        }
      }
      (*hitsInLayer[systemIndex])[thisElement] += numberOfStrips;
      local_tothits += numberOfStrips;
      maxrodhits += numberOfStrips;

      
    } // End of Loop on SCT_Collection, so end of loop over the RDOs in the RDO container
      // We can now do the NO calculation for this wafer
      // For the Time Dependent plots
    if (maxhits < maxrodhits) {
      maxhits_Id = theWaferIdentifierOfTheRDOCollection;
      maxhits = maxrodhits;
    }
    if (minhits > maxrodhits) {
      minhits_Id = theWaferIdentifierOfTheRDOCollection;
      minhits = maxrodhits;
    }
    //    meanhits = local_tothits / totalmodules;

    if (numberOfHitsFromAllRDOs > 0) {
      unsigned int systemIndex{bec2Index(Bec)};
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
        m_data->m_occSumUnbiased[theModuleHash0] = m_data->m_occSumUnbiased[theModuleHash0].load() + sumocc;
        if (m_environment == AthMonitorAlgorithm::Environment_t::online) {
          m_data->m_occSumUnbiasedRecent[theModuleHash0] = m_data->m_occSumUnbiasedRecent[theModuleHash0].load() + sumocc;
        }
        if (isSelectedTrigger) {
          m_data->m_occSumUnbiasedTrigger[theModuleHash0] = m_data->m_occSumUnbiasedTrigger[theModuleHash0].load() + sumocc;
        }
	IdentifierHash theWaferhash{m_pSCTHelper->wafer_hash(theWaferIdentifierOfTheRDOCollection)};
	m_data->m_occSumUnbiased_lb[systemIndex][theWaferhash]= m_data->m_occSumUnbiased_lb[systemIndex][theWaferhash].load() + sumocc;
        m_data->m_occSumUnbiased_lb[GENERAL_INDEX][theWaferhash] = m_data->m_occSumUnbiased_lb[GENERAL_INDEX][theWaferhash].load() + sumocc;
	if (isSelectedTrigger) {
	  m_data->m_occSumUnbiasedTrigger_lb[systemIndex][theWaferhash] = m_data->m_occSumUnbiasedTrigger_lb[systemIndex][theWaferhash].load() + sumocc;
          m_data->m_occSumUnbiasedTrigger_lb[GENERAL_INDEX][theWaferhash] = m_data->m_occSumUnbiasedTrigger_lb[GENERAL_INDEX][theWaferhash].load() + sumocc;
	}
      }

       // hit occupancy
          
      float sumhitocc{static_cast<float> (numberOfHitsFromAllRDOs) / static_cast<float> (N_STRIPS)};
      m_data->m_hitoccSumUnbiased[theModuleHash0] = m_data->m_hitoccSumUnbiased[theModuleHash0].load() + sumhitocc;
      if (m_environment == AthMonitorAlgorithm::Environment_t::online) {
        m_data->m_hitoccSumUnbiasedRecent[theModuleHash0] = m_data->m_hitoccSumUnbiasedRecent[theModuleHash0].load() + sumhitocc;
      }
      if (isSelectedTrigger) {
        m_data->m_hitoccSumUnbiasedTrigger[theModuleHash0] = m_data->m_hitoccSumUnbiasedTrigger[theModuleHash0].load() + sumhitocc;
      }
        
      auto lbHits_vsLBAcc{Monitored::Scalar<int>("lbh_Hits_vsLB", m_current_lb)};
        
      auto numhitsHallHits_vsLBAcc{Monitored::Scalar<int>("numhitsh_HallHits_vsLB", numberOfHitsFromAllRDOs)};
        
      auto numberOfHitsFromSPsHSPHits_vsLBAcc{Monitored::Scalar<int>("numberOfHitsFromSPsh_HSPHits_vsLB", numberOfHitsFromSPs)};
      fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex),lbHits_vsLBAcc,numberOfHitsFromSPsHSPHits_vsLBAcc,numhitsHallHits_vsLBAcc);
        
      if (isSelectedTrigger) {
        auto lbHitsTrigger_vsLBAcc{Monitored::Scalar<int>("lbh_HitsTrigger_vsLB", m_current_lb)};
          
	auto numhitsHallHitsTrigger_vsLBAcc{Monitored::Scalar<int>("numhitsh_HallHits_vsLB", numberOfHitsFromAllRDOs)};
            
        auto numberOfHitsFromSPsHSPHitsTrigger_vsLBAcc{Monitored::Scalar<int>("numberOfHitsFromSPsh_HSPHitsTrigger_vsLB", numberOfHitsFromSPs)};
        fill("SCTHitsNoiseMonitor_" + std::to_string(systemIndex),lbHitsTrigger_vsLBAcc,numberOfHitsFromSPsHSPHitsTrigger_vsLBAcc,numhitsHallHits_vsLBAcc);
     
      }
      // end of hit occupancy
    }
  }// End of Loop on RDO container

  

  // if (m_environment!=AthMonitorAlgorithm::Environment_t::online) { // Uncomment this line to turn off cluster hists in online
  // Fill Cluster size histogram
  SG::ReadHandle<InDet::SCT_ClusterContainer> p_clucontainer{m_clusContainerKey};
  if (not p_clucontainer.isValid()) {
    ATH_MSG_WARNING("Couldn't retrieve clusters");
  }
  for (const InDet::SCT_ClusterCollection* SCT_Collection: *p_clucontainer) {
    for (const InDet::SCT_Cluster* cluster: *SCT_Collection) {
      long unsigned int GroupSize{cluster->rdoList().size()};
      auto cluSizeAcc{Monitored::Scalar<long unsigned int>("clu_size", GroupSize)};
      fill("SCTHitsNoiseMonitorGeneral",cluSizeAcc);

      
    }
  }
  // } // Uncomment this line as well to turn off cluster hists in online
  
  return StatusCode::SUCCESS;
}


StatusCode SCTHitsNoiseMonAlg::makeVectorOfTrackRDOIdentifiers(std::vector<Identifier>& rdosOnTracks, const EventContext& ctx) const{
  // Clear the rdosOnTracks vector
  rdosOnTracks.clear();
  SG::ReadHandle<SCT_RDO_Container> p_rdocontainer{m_dataObjectName, ctx};
  if (not p_rdocontainer.isValid()) {
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
  for (const Trk::Track* track : *tracks){
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
            rdosOnTracks.insert(rdosOnTracks.end(), rdoList.begin(), rdoList.end());
          }
        }
      }
    }
  }
  ATH_MSG_DEBUG("JEGN!! num RDOs on tracks is " << rdosOnTracks.size());
  return StatusCode::SUCCESS;
}

StatusCode SCTHitsNoiseMonAlg::makeSPvsEventNumber() const{
    
return StatusCode::SUCCESS;    
}

void SCTHitsNoiseMonAlg::resetCaches() const{

  for (unsigned int iThre{0}; iThre<nThreshes; iThre++) {
    m_data->m_noisyM[iThre][m_current_lb] = 0;
    m_data->m_noisyMTrigger[iThre][m_current_lb] = 0;
    m_data->m_noisyMWithHO[iThre][m_current_lb] = 0;
    m_data->m_noisyMWithHOTrigger[iThre][m_current_lb] = 0;
  }
  for (unsigned int jReg{0}; jReg<N_REGIONS+1; jReg++) {
    m_data->m_occ_lb[jReg][m_current_lb] = 0;
    m_data->m_occTrigger_lb[jReg][m_current_lb] = 0;
    m_data->m_hitocc_lb[jReg][m_current_lb] = 0;
    m_data->m_hitoccTrigger_lb[jReg][m_current_lb] = 0;
  }
  int nlinks[N_REGIONS+1]{0, 0, 0, 0};
  SCT_ID::const_id_iterator planeIterator{m_pSCTHelper->wafer_begin()};
  SCT_ID::const_id_iterator planeEnd{m_pSCTHelper->wafer_end()};
  for (; planeIterator not_eq planeEnd; ++planeIterator) {
    Identifier planeId{*planeIterator};
    IdentifierHash planehash{m_pSCTHelper->wafer_hash(planeId)};
    const int bec{m_pSCTHelper->barrel_ec(planeId)};
    const unsigned int systemIndex{bec2Index(bec)};
    // Don't initialize a value for disabled  modules
    if (not m_ConfigurationTool->isGood(*planeIterator, InDetConditions::SCT_SIDE)) {
      continue;
    }
    
    if (m_events_lb > 0) {
      for (unsigned int jReg{0}; jReg<N_REGIONS+1; jReg++) {
	m_data->m_occ_lb[jReg][m_current_lb] += m_data->m_occSumUnbiased_lb[jReg][planehash] / m_events_lb;
	m_data->m_hitocc_lb[jReg][m_current_lb] += m_data->m_hitoccSumUnbiased_lb[jReg][planehash] / m_events_lb;
      }
      for (unsigned int iThre{0}; iThre<nThreshes; iThre++) {
	if ((1E5) * m_data->m_occSumUnbiased_lb[GENERAL_INDEX][planehash] / m_events_lb > s_thresholds[iThre]) {
	  m_data->m_noisyM[iThre][m_current_lb]++;
	}
	if ((1E5) * m_data->m_hitoccSumUnbiased_lb[GENERAL_INDEX][planehash] / m_events_lb > s_thresholds[iThre]) {
	  m_data->m_noisyMWithHO[iThre][m_current_lb]++;
	}
      }
    }
    
    if (m_eventsTrigger_lb > 0) {
      for (unsigned int jReg{0}; jReg<N_REGIONS+1; jReg++) {
	m_data->m_occTrigger_lb[jReg][m_current_lb] += (1E5) * m_data->m_occSumUnbiasedTrigger_lb[jReg][planehash] / m_eventsTrigger_lb;
	m_data->m_hitoccTrigger_lb[jReg][m_current_lb] += (1E5) * m_data->m_hitoccSumUnbiasedTrigger_lb[jReg][planehash] / m_eventsTrigger_lb;
      }
      for (unsigned int iThre{0}; iThre<nThreshes; iThre++) {
	if ((1E5) * m_data->m_occSumUnbiasedTrigger_lb[GENERAL_INDEX][planehash] / m_eventsTrigger_lb > s_thresholds[iThre]) {
	  m_data->m_noisyMTrigger[iThre][m_current_lb]++;
	}
	if ((1E5) * m_data->m_hitoccSumUnbiasedTrigger_lb[GENERAL_INDEX][planehash] / m_eventsTrigger_lb > s_thresholds[iThre]) {
	  m_data->m_noisyMWithHOTrigger[iThre][m_current_lb]++;
	}
      }
    }
    for (unsigned int jReg{0}; jReg<N_REGIONS+1; jReg++) {
      m_data->m_occSumUnbiased_lb[jReg][planehash] = 0;
      m_data->m_occSumUnbiasedTrigger_lb[jReg][planehash] = 0;
      m_data->m_hitoccSumUnbiased_lb[jReg][planehash] = 0;
      m_data->m_hitoccSumUnbiasedTrigger_lb[jReg][planehash] = 0;
    }
    nlinks[systemIndex]++;
    nlinks[GENERAL_INDEX]++;
  }
  for (unsigned int jReg{0}; jReg<N_REGIONS+1; jReg++) {
    if (nlinks[jReg]>0) {
      
      m_data->m_occ_lb[jReg][m_current_lb] = m_data->m_occ_lb[jReg][m_current_lb].load() / nlinks[jReg];
      m_data->m_occTrigger_lb[jReg][m_current_lb] = m_data->m_occTrigger_lb[jReg][m_current_lb].load() / nlinks[jReg];
      m_data->m_hitocc_lb[jReg][m_current_lb] = m_data->m_hitocc_lb[jReg][m_current_lb].load() / nlinks[jReg];
      m_data->m_hitoccTrigger_lb[jReg][m_current_lb] = m_data->m_hitoccTrigger_lb[jReg][m_current_lb].load() / nlinks[jReg];
    
    }
  }
  m_events_lb = 0;
  m_eventsTrigger_lb = 0;

  return ;
}
