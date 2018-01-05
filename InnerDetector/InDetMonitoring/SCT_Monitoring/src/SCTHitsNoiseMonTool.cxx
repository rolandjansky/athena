/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTHitsNoiseMonTool.cxx
 *
 *     (merging SCTHitsMonTool.cxx and SCTNoiseMonTool.cxx in one Tool)
 *
 *
 *    @author Sofia Chouridou based on code from Martin White, Luca Fiorini, Shaun Roe, Manuel Diaz & Rob McPherson
 *    'The end of all of our exploring...' (Shaun, March 2008)
 */

#include "SCT_NameFormatter.h"

#include <sstream> // ?
#include <iostream>
#include <fstream> // for writing an xml summary (debugging)
#include <algorithm>


#include "GaudiKernel/IJobOptionsSvc.h"
#include "SCT_Monitoring/SCTHitsNoiseMonTool.h"
#include "SCT_Monitoring/SCT_ClusterStruct.h"

#include "TMath.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/SCT_TB03_RawData.h" // ?
#include "InDetRawData/InDetRawDataContainer.h" // ?
#include "InDetRawData/InDetRawDataCLASS_DEF.h" // ?
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" // ?

#include "PathResolver/PathResolver.h" // ?
#include "InDetPrepRawData/SCT_ClusterContainer.h" // ?
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h" // ?
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h" // ?
#include "cArrayUtilities.h"

#include "StoreGate/ReadHandle.h"

typedef Trk::SpacePoint SpacePoint;
using namespace std;
using namespace SCT_Monitoring;
/**
 *  Utility functions to use in this file only
 **/
namespace { // use anonymous namespace to restrict scope to this file, equivalent to 'static' keyword here
  // is a variable in the specified range?
  template <typename T>
  bool
  inRange(const T &variable, const T &lo, const T &hi) {
    return not ((variable < lo)or(variable > hi)); // assumes both less_than and greater_than exist for this type
  }

  // Possible timing patterns, IXX is 1XX etc.
  enum
  Pattern {
    IXX=0, XIX, XXI
  };

  // possible coordinate species
  enum
  Coordinate {
    ETA=0, PHI
  };
  // ... and the corresponding names
  const std::string coordinateNames[] = {
    "ETA", "PHI"
  };
  // possible histogram species

  // string names for the detector parts
  const std::string names[] = {
    "Endcap C", "Barrel", "Endcap A"
  };
  // abbreviations for detector parts
  const std::string abbreviations[] = {
    "ECm", "", "ECp"
  };
  const std::string streamDelimiter("_");
  const std::string pathDelimiter("/");


  // is the timebin in the desired pattern?
  bool
  timeBinInPattern(const int tbin, const Pattern xxx) {
    switch (xxx) {
    case IXX:
      return(tbin > 3);

      break;

    case XIX:
      return(tbin == 2 || tbin == 3 || tbin == 6 || tbin == 7);

      break;

    case XXI:
      return(tbin == 1 || tbin == 3 || tbin == 5 || tbin == 7);

      break;

    default:
      // std::cout<<"A strange timepattern was found; this should never happen"<<std::endl;
      return false;

      break;
    }
  }
}// end of anonymous namespace

// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
// ====================================================================================================
SCTHitsNoiseMonTool::SCTHitsNoiseMonTool(const std::string &type,
                                         const std::string &name,
                                         const IInterface *parent) :
  SCTMotherTrigMonTool(type, name, parent),
  m_nSP(nullptr),
  m_nSP_buf(nullptr),
  m_nSP_pos(0),
  m_nHits(nullptr),
  m_nHits_buf(nullptr),
  m_nHits_pos(0),
  m_nmaxHits(nullptr),
  m_nmaxHits_buf(nullptr),
  m_nmaxModule_buf(nullptr),
  m_nminHits(nullptr),
  m_nminHits_buf(nullptr),
  m_nminModule_buf(nullptr),
  m_numBarrelHitsPerLumiBlock(nullptr),
  m_numHitsPerLumiBlockECp(nullptr),
  m_numHitsPerLumiBlockECm(nullptr),
  m_numBarrelSPPerLumiBlock(nullptr),
  m_numSPPerLumiBlockECp(nullptr),
  m_numSPPerLumiBlockECm(nullptr),
  m_rioMap(nullptr),
  //
  m_BARNO(nullptr),
  m_BARNOTrigger(nullptr),
  m_ECmNO(nullptr),
  m_ECmNOTrigger(nullptr),
  m_ECpNO(nullptr),
  m_ECpNOTrigger(nullptr),
  m_SCTNO(nullptr),
  m_SCTNOTrigger(nullptr),
  //
  m_NallHitsBAR_vsLB(nullptr),
  m_NSPHitsBAR_vsLB(nullptr),
  m_NallHitsECm_vsLB(nullptr),
  m_NSPHitsECm_vsLB(nullptr),
  m_NallHitsECp_vsLB(nullptr),
  m_NSPHitsECp_vsLB(nullptr),
  m_BARNO_vsLB(nullptr),
  m_ECmNO_vsLB(nullptr),
  m_ECpNO_vsLB(nullptr),
  m_SCTNO_vsLB(nullptr),
  m_NoisyModules100_vsLB(nullptr),
  m_NoisyModules1000_vsLB(nullptr),
  m_NoisyModules10000_vsLB(nullptr),
  //
  m_NallHitsTriggerBAR_vsLB(nullptr),
  m_NSPHitsTriggerBAR_vsLB(nullptr),
  m_NallHitsTriggerECm_vsLB(nullptr),
  m_NSPHitsTriggerECm_vsLB(nullptr),
  m_NallHitsTriggerECp_vsLB(nullptr),
  m_NSPHitsTriggerECp_vsLB(nullptr),
  m_BARNOTrigger_vsLB(nullptr),
  m_ECmNOTrigger_vsLB(nullptr),
  m_ECpNOTrigger_vsLB(nullptr),
  m_SCTNOTrigger_vsLB(nullptr),
  m_NoisyModulesTrigger100_vsLB(nullptr),
  m_NoisyModulesTrigger1000_vsLB(nullptr),
  m_NoisyModulesTrigger10000_vsLB(nullptr),
  //
  m_MaxOccupancyStripHist(nullptr),
  m_MinOccupancyStripHist(nullptr),
  m_clusize(nullptr),
  m_clusizeRecent(nullptr),
  m_hitsvstrigger(nullptr),
  m_hitsvsL1ID(nullptr),
  m_ncluHisto(nullptr),
  m_coincidenceHist(nullptr),
  m_numberOfEvents(0),
  m_numberOfEventsTrigger(0),
  m_numberOfEventsRecent(0),
  m_skipEvents(0),
  m_booltxscan(false),
  m_current_lb(0),
  m_last_reset_lb(0),
  m_tbinHisto(nullptr),
  m_tbinHistoECp(nullptr),
  m_tbinHistoECm(nullptr),
  m_tbinHistoRecent(nullptr),
  m_tbinHistoRecentECp(nullptr),
  m_tbinHistoRecentECm(nullptr),
  m_tbinfracall(nullptr),
  m_tbinfracVsLB(nullptr),
  m_tbinfracVsLBECp(nullptr),
  m_tbinfracVsLBECm(nullptr),
  m_initialize(false),
  m_SCTSPContainerName(std::string("SCT_SpacePoints")),
  m_noisyM100{},
  m_noisyM1000{},
  m_noisyM10000{},
  m_occ_lb{},
  m_occBAR_lb{},
  m_occECp_lb{},
  m_occECm_lb{},
  m_events_lb(0),
  m_noisyMTrigger100{},
  m_noisyMTrigger1000{},
  m_noisyMTrigger10000{},
  m_occTrigger_lb{},
  m_occTriggerBAR_lb{},
  m_occTriggerECp_lb{},
  m_occTriggerECm_lb{},
  m_eventsTrigger_lb(0),
  // Add 09.09.2016
  m_BARHO(nullptr),
  m_BARHOTrigger(nullptr),
  m_ECmHO(nullptr),
  m_ECmHOTrigger(nullptr),
  m_ECpHO(nullptr),
  m_ECpHOTrigger(nullptr),
  m_SCTHO(nullptr),
  m_SCTHOTrigger(nullptr),
  //
  m_HallHitsBAR_vsLB(nullptr),
  m_HSPHitsBAR_vsLB(nullptr),
  m_HallHitsECm_vsLB(nullptr),
  m_HSPHitsECm_vsLB(nullptr),
  m_HallHitsECp_vsLB(nullptr),
  m_HSPHitsECp_vsLB(nullptr),
  m_BARHO_vsLB(nullptr),
  m_ECmHO_vsLB(nullptr),
  m_ECpHO_vsLB(nullptr),
  m_SCTHO_vsLB(nullptr),
  m_NoisyModulesWithHO100_vsLB(nullptr),
  m_NoisyModulesWithHO1000_vsLB(nullptr),
  m_NoisyModulesWithHO10000_vsLB(nullptr),
  //
  m_HallHitsTriggerBAR_vsLB(nullptr),
  m_HSPHitsTriggerBAR_vsLB(nullptr),
  m_HallHitsTriggerECm_vsLB(nullptr),
  m_HSPHitsTriggerECm_vsLB(nullptr),
  m_HallHitsTriggerECp_vsLB(nullptr),
  m_HSPHitsTriggerECp_vsLB(nullptr),
  m_BARHOTrigger_vsLB(nullptr),
  m_ECmHOTrigger_vsLB(nullptr),
  m_ECpHOTrigger_vsLB(nullptr),
  m_SCTHOTrigger_vsLB(nullptr),
  m_NoisyModulesWithHOTrigger100_vsLB(nullptr),
  m_NoisyModulesWithHOTrigger1000_vsLB(nullptr),
  m_NoisyModulesWithHOTrigger10000_vsLB(nullptr),
  //
  m_noisyMWithHO100{},
  m_noisyMWithHO1000{},
  m_noisyMWithHO10000{},
  m_hitocc_lb{},
  m_hitoccBAR_lb{},
  m_hitoccECp_lb{},
  m_hitoccECm_lb{},
  m_noisyMWithHOTrigger100{},
  m_noisyMWithHOTrigger1000{},
  m_noisyMWithHOTrigger10000{},
  m_hitoccTrigger_lb{},
  m_hitoccTriggerBAR_lb{},
  m_hitoccTriggerECp_lb{},
  m_hitoccTriggerECm_lb{},
  //
  m_dataObjectName(std::string("SCT_RDOs")),
  m_pSCTHelper(nullptr),
  m_ConfigurationSvc("InDetSCT_ConfigurationConditionsSvc", name),
  m_eventInfoKey(std::string("EventInfo")),
  m_clusContainerKey("SCT_Clusters") {
    /** sroe 3 Sept 2015:
	histoPathBase is declared as a property in the base class, assigned to m_path
	with default as empty string.
	Declaring it here as well gives rise to compilation warning
	WARNING duplicated property name 'histoPathBase', see https://its.cern.ch/jira/browse/GAUDI-1023

	declareProperty("histoPathBase", m_stream = "/stat"); **/
    m_stream = "/stat";
    declareProperty("localSummary", m_localSummary = 0);
    declareProperty("doHitmapHistos", m_boolhitmaps = true);
    declareProperty("doTXScan", m_booltxscan = false);
    declareProperty("CheckRate", m_checkrate = 1000);
    declareProperty("CheckRecent", m_checkrecent = 30);
    declareProperty("tracksName", m_tracksName = "Combined_Tracks");
    declareProperty("NOTrigger", m_NOTrigger = "L1_RD0_EMPTY");
    declareProperty("numberOfSigma", m_numSigma = 3);
    declareProperty("EvtsBins", m_evtsbins = 5000);
    declareProperty("doPositiveEndcap", m_doPositiveEndcap = true);
    declareProperty("doNegativeEndcap", m_doNegativeEndcap = true);
    declareProperty("doTrackBasedNoise", m_doTrackBasedNoise = false);
    declareProperty("doSpacePointBasedNoise", m_doSpacePointBasedNoise = true);
    declareProperty("doTimeBinFilteringForNoise", m_doTimeBinFilteringForNoise = true);
    declareProperty("doTrackHits", m_doTrackHits = true);
    declareProperty("MaxTracks", m_maxTracks = 250);
    declareProperty("doLogXNoise", m_doLogXNoise = true);
    declareProperty("conditionsService", m_ConfigurationSvc);

    clear1D(m_tbinfrac);
    clear1D(m_tbinfracECp);
    clear1D(m_tbinfracECm);
    clear1D(m_clusizedist);
    clear1D(m_clusizedistECp);
    clear1D(m_clusizedistECm);
  }

// ====================================================================================================
// ====================================================================================================
SCTHitsNoiseMonTool::~SCTHitsNoiseMonTool() {
  free(m_nSP_buf);
  free(m_nmaxHits_buf);
  free(m_nminHits_buf);
  free(m_nmaxModule_buf);
  free(m_nminModule_buf);
  free(m_nHits_buf);
}

// ====================================================================================================
// ====================================================================================================
StatusCode SCTHitsNoiseMonTool::initialize() {
  ATH_CHECK(SCTMotherTrigMonTool::initialize());

  ATH_CHECK(m_SCTSPContainerName.initialize());
  ATH_CHECK(m_dataObjectName.initialize());
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_clusContainerKey.initialize());
  ATH_CHECK(m_tracksName.initialize());

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTHitsNoiseMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::bookHistograms() {
  ATH_MSG_DEBUG("initialize being called");
  if (newRunFlag()) {
    m_numberOfEvents = 0;
  }
  // Get the helper:
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(m_ConfigurationSvc.retrieve());
  m_initialize = true;

  const bool doSystem[] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };
  for (unsigned int det(0); det != N_REGIONS; ++det) {
    if (doSystem[det]) {
      if (m_doTrackHits and bookGeneralTrackTimeHistos(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralTrackTimeHistos()" << endmsg;
      }
      if (m_doTrackHits and bookGeneralTrackHits(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralTrackHits()" << endmsg;
      }
      if (bookGeneralNoiseOccupancyMaps(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralNoiseOccupancyMaps()" << endmsg;
      }
      if (bookGeneralHitOccupancyMaps(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralHitOccupancyMaps()" << endmsg;
      }
      if (bookGeneralHits(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralHits()" << endmsg;
      }
      // offline only?
      if (bookGeneralCluSize(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralSize()" << endmsg;
      }
      //
    }
  }

  if (bookSPvsEventNumber().isFailure()) {
    msg(MSG::WARNING) << "Error in bookSPvsEventNumber()" << endmsg;
  }
  if (bookNoiseDistributions().isFailure()) {
    msg(MSG::WARNING) << "Error in bookNoiseDistributions()" << endmsg;
  }

  if (m_boolhitmaps) {
    if (book1DHitHists().isFailure()) {
      msg(MSG::WARNING) << "Error in barrel book1DHitHists()" << endmsg;
    }
    // Only want these histograms when running the code offline
  }

  if (bookClusterSize().isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Error in bookClusterSize()" << endmsg;
    }
  }

  if (newRunFlag()) {
    std::string stem = m_stream + "/SCT/GENERAL/hits/summary";
    // book hits histogram
    MonGroup clu(this, "SCT/GENERAL/hits/summary", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    m_ncluHisto = new TH1F("sct_hits", "Total SCT Hits", N_HIT_BINS * 4, FIRST_HIT_BIN, LAST_HIT_BIN * 150);
    m_ncluHisto->GetXaxis()->SetTitle("Total SCT Hits");
    m_ncluHisto->GetYaxis()->SetTitle("Entries");
    if (clu.regHist(m_ncluHisto).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << stem + "sct_hits" << endmsg;
    }
    if (m_booltxscan) {
      if (newEventsBlockFlag()) {
        // book histograms for TX scans
        m_hitsvstrigger = TH1F_LW::create("hits_vs_trigger", "Hits vs Trigger Type", 32, -0.5, 31.5);
        if (clu.regHist(m_hitsvstrigger).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:" << stem + "hitsvstrigger" << endmsg;
        }
        m_hitsvsL1ID = TH1F_LW::create("hits_vs_L1ID", "Hits vs L1ID", 1000, 0., 10000.);
        if (clu.regHist(m_hitsvsL1ID).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:" << stem + "hitsvsL1ID" << endmsg;
        }
      }
    }
    if (initializeNoiseMaps().isFailure()) {
      msg(MSG::WARNING) << "Failed to initalize noise occupancy maps!" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTHitsNoiseMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::bookHistogramsRecurrent() {
  ATH_MSG_DEBUG("bookHistogramsRecurrent being called");
  m_numberOfEvents = 0;
  // Get the helper:
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(m_ConfigurationSvc.retrieve());
  m_initialize = true;

  const bool doSystem[] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };
  for (unsigned int det(0); det != N_REGIONS; ++det) {
    if (doSystem[det]) {
      if (m_doTrackHits and bookGeneralTrackTimeHistos(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralTrackTimeHistos()" << endmsg;
      }
      if (m_doTrackHits and bookGeneralTrackHits(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralTrackHits()" << endmsg;
      }
      if (bookGeneralNoiseOccupancyMaps(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralNoiseOccupancyMaps()" << endmsg;
      }
      if (bookGeneralHitOccupancyMaps(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralHitOccupancyMaps()" << endmsg;
      }
      if (bookGeneralHits(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralHits()" << endmsg;
      }
      // offline only?
      if (bookGeneralCluSize(det).isFailure()) {
        msg(MSG::WARNING) << "Error in bookGeneralSize()" << endmsg;
      }
      //
    }
  }

  if (bookSPvsEventNumber().isFailure()) {
    msg(MSG::WARNING) << "Error in bookSPvsEventNumber()" << endmsg;
  }
  if (bookNoiseDistributions().isFailure()) {
    msg(MSG::WARNING) << "Error in bookNoiseDistributions()" << endmsg;
  }

  if (m_boolhitmaps) {
    if (book1DHitHists().isFailure()) {
      msg(MSG::WARNING) << "Error in barrel book1DHitHists()" << endmsg;
    }
    // Only want these histograms when running the code offline
  }

  if (bookClusterSize().isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Error in bookClusterSize()" << endmsg;
    }
  }

  std::string stem = m_stream + "/SCT/GENERAL/hits/summary";
  // book hits histogram
  MonGroup clu(this, "SCT/GENERAL/hits/summary", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
  m_ncluHisto = new TH1F("sct_hits", "Total SCT Hits", N_HIT_BINS * 4, FIRST_HIT_BIN, LAST_HIT_BIN * 150);
  m_ncluHisto->GetXaxis()->SetTitle("Event Number");
  m_ncluHisto->GetYaxis()->SetTitle("Num of SCT Hits");
  if (clu.regHist(m_ncluHisto).isFailure()) {
    msg(MSG::WARNING) << "Cannot book Histogram:" << stem + "sct_hits" << endmsg;
  }
  if (m_booltxscan) {
    // book histograms for TX scans
    m_hitsvstrigger = TH1F_LW::create("hits_vs_trigger", "Hits vs Trigger Type", 32, -0.5, 31.5);
    if (clu.regHist(m_hitsvstrigger).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << stem + "hitsvstrigger" << endmsg;
    }
    m_hitsvsL1ID = TH1F_LW::create("hits_vs_L1ID", "Hits vs L1ID", 1000, 0., 10000.);
    if (clu.regHist(m_hitsvsL1ID).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << stem + "hitsvsL1ID" << endmsg;
    }
  }
  if (initializeNoiseMaps().isFailure()) {
    msg(MSG::WARNING) << "Failed to initalize noise occupancy maps!" << endmsg;
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTHitsNoiseMonTool :: fillHistograms
// This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::fillHistograms() {
  ++m_numberOfEvents;
  ++m_numberOfEventsRecent;
  SG::ReadHandle<xAOD::EventInfo> pEvent(m_eventInfoKey);
  if (not pEvent.isValid()) {
    if (msgLvl(MSG::ERROR)) {
      msg(MSG::ERROR) << "Could not retrieve event info!" << endmsg;
    }
    return StatusCode::RECOVERABLE;
  }
  int tmp_lb = pEvent->lumiBlock();
  if ((tmp_lb > m_current_lb) and (m_current_lb<=SCT_Monitoring::NBINS_LBs)) {
    m_noisyM100[m_current_lb] = 0;
    m_noisyM1000[m_current_lb] = 0;
    m_noisyM10000[m_current_lb] = 0;
    m_occ_lb[m_current_lb] = 0;
    m_occBAR_lb[m_current_lb] = 0;
    m_occECp_lb[m_current_lb] = 0;
    m_occECm_lb[m_current_lb] = 0;
    m_noisyMTrigger100[m_current_lb] = 0;
    m_noisyMTrigger1000[m_current_lb] = 0;
    m_noisyMTrigger10000[m_current_lb] = 0;
    m_occTrigger_lb[m_current_lb] = 0;
    m_occTriggerBAR_lb[m_current_lb] = 0;
    m_occTriggerECp_lb[m_current_lb] = 0;
    m_occTriggerECm_lb[m_current_lb] = 0;
    // add 09.09.2016
    m_noisyMWithHO100[m_current_lb] = 0;
    m_noisyMWithHO1000[m_current_lb] = 0;
    m_noisyMWithHO10000[m_current_lb] = 0;
    m_hitocc_lb[m_current_lb] = 0;
    m_hitoccBAR_lb[m_current_lb] = 0;
    m_hitoccECp_lb[m_current_lb] = 0;
    m_hitoccECm_lb[m_current_lb] = 0;
    m_noisyMWithHOTrigger100[m_current_lb] = 0;
    m_noisyMWithHOTrigger1000[m_current_lb] = 0;
    m_noisyMWithHOTrigger10000[m_current_lb] = 0;
    m_hitoccTrigger_lb[m_current_lb] = 0;
    m_hitoccTriggerBAR_lb[m_current_lb] = 0;
    m_hitoccTriggerECp_lb[m_current_lb] = 0;
    m_hitoccTriggerECm_lb[m_current_lb] = 0;
    int nlinks = 0;
    int nlinksBAR = 0;
    int nlinksECp = 0;
    int nlinksECm = 0;
    SCT_ID::const_id_iterator planeIterator(m_pSCTHelper->wafer_begin());
    SCT_ID::const_id_iterator planeEnd = m_pSCTHelper->wafer_end();
    for (; planeIterator not_eq planeEnd; ++planeIterator) {
      Identifier planeId(*planeIterator);
      const int bec(m_pSCTHelper->barrel_ec(planeId));
      // Don't initialize a value for disabled  modules
      if (!m_ConfigurationSvc->isGood(*planeIterator, InDetConditions::SCT_SIDE)) {
        continue;
      }
      if (m_events_lb > 0) {
        m_occ_lb[m_current_lb] += m_occSumUnbiased_lb[*planeIterator] / m_events_lb;
        m_occBAR_lb[m_current_lb] += m_occSumUnbiasedBAR_lb[*planeIterator] / m_events_lb;
        m_occECp_lb[m_current_lb] += m_occSumUnbiasedECp_lb[*planeIterator] / m_events_lb;
        m_occECm_lb[m_current_lb] += m_occSumUnbiasedECm_lb[*planeIterator] / m_events_lb;
        if ((1E5) * m_occSumUnbiased_lb[*planeIterator] / m_events_lb > 100.) {
          m_noisyM100[m_current_lb]++;
        }
        if ((1E5) * m_occSumUnbiased_lb[*planeIterator] / m_events_lb > 1000.) {
          m_noisyM1000[m_current_lb]++;
        }
        if ((1E5) * m_occSumUnbiased_lb[*planeIterator] / m_events_lb > 10000.) {
          m_noisyM10000[m_current_lb]++;
        }
        // 09.09.2016
        m_hitocc_lb[m_current_lb] += m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb;
        m_hitoccBAR_lb[m_current_lb] += m_hitoccSumUnbiasedBAR_lb[*planeIterator] / m_events_lb;
        m_hitoccECp_lb[m_current_lb] += m_hitoccSumUnbiasedECp_lb[*planeIterator] / m_events_lb;
        m_hitoccECm_lb[m_current_lb] += m_hitoccSumUnbiasedECm_lb[*planeIterator] / m_events_lb;
        if ((1E5) * m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb > 100.) {
          m_noisyMWithHO100[m_current_lb]++;
        }
        if ((1E5) * m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb > 1000.) {
          m_noisyMWithHO1000[m_current_lb]++;
        }
        if ((1E5) * m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb > 10000.) {
          m_noisyMWithHO10000[m_current_lb]++;
        }
      }
      if (m_eventsTrigger_lb > 0) {
        m_occTrigger_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb;
        m_occTriggerBAR_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTriggerBAR_lb[*planeIterator] / m_eventsTrigger_lb;
        m_occTriggerECp_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTriggerECp_lb[*planeIterator] / m_eventsTrigger_lb;
        m_occTriggerECm_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTriggerECm_lb[*planeIterator] / m_eventsTrigger_lb;
        if ((1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 100.) {
          m_noisyMTrigger100[m_current_lb]++;
        }
        if ((1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 1000.) {
          m_noisyMTrigger1000[m_current_lb]++;
        }
        if ((1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 10000.) {
          m_noisyMTrigger10000[m_current_lb]++;
        }
        // add 09.09.2016
        m_hitoccTrigger_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb;
        m_hitoccTriggerBAR_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTriggerBAR_lb[*planeIterator] /
	  m_eventsTrigger_lb;
        m_hitoccTriggerECp_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTriggerECp_lb[*planeIterator] /
	  m_eventsTrigger_lb;
        m_hitoccTriggerECm_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTriggerECm_lb[*planeIterator] /
	  m_eventsTrigger_lb;
        if ((1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 100.) {
          m_noisyMWithHOTrigger100[m_current_lb]++;
        }
        if ((1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 1000.) {
          m_noisyMWithHOTrigger1000[m_current_lb]++;
        }
        if ((1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 10000.) {
          m_noisyMWithHOTrigger10000[m_current_lb]++;
        }
      }
      m_occSumUnbiased_lb[*planeIterator] = 0;
      m_occSumUnbiasedBAR_lb[*planeIterator] = 0;
      m_occSumUnbiasedECp_lb[*planeIterator] = 0;
      m_occSumUnbiasedECm_lb[*planeIterator] = 0;
      m_occSumUnbiasedTrigger_lb[*planeIterator] = 0;
      m_occSumUnbiasedTriggerBAR_lb[*planeIterator] = 0;
      m_occSumUnbiasedTriggerECp_lb[*planeIterator] = 0;
      m_occSumUnbiasedTriggerECm_lb[*planeIterator] = 0;
      // 09.09.2016
      m_hitoccSumUnbiased_lb[*planeIterator] = 0;
      m_hitoccSumUnbiasedBAR_lb[*planeIterator] = 0;
      m_hitoccSumUnbiasedECp_lb[*planeIterator] = 0;
      m_hitoccSumUnbiasedECm_lb[*planeIterator] = 0;
      m_hitoccSumUnbiasedTrigger_lb[*planeIterator] = 0;
      m_hitoccSumUnbiasedTriggerBAR_lb[*planeIterator] = 0;
      m_hitoccSumUnbiasedTriggerECp_lb[*planeIterator] = 0;
      m_hitoccSumUnbiasedTriggerECm_lb[*planeIterator] = 0;
      if (bec == BARREL)  nlinksBAR++; 
      if (bec == ENDCAP_A)nlinksECp++;
      if (bec == ENDCAP_C)nlinksECm++;
      nlinks++;
    }
    //std::cout <<"Line660: Number of links = "<<nlinks << std::endl;
    if (nlinks >0){
      m_occ_lb[m_current_lb] /= nlinks;
      m_occTrigger_lb[m_current_lb] /= nlinks;
      // 09.09.2016
      m_hitocc_lb[m_current_lb] /= nlinks;
      m_hitoccTrigger_lb[m_current_lb] /= nlinks;
    }
    if (nlinksBAR >0){
      m_occBAR_lb[m_current_lb] /= nlinksBAR;
      m_occTriggerBAR_lb[m_current_lb] /= nlinksBAR;
      // 09.09.2016
      m_hitoccBAR_lb[m_current_lb] /= nlinksBAR;
      m_hitoccTriggerBAR_lb[m_current_lb] /= nlinksBAR;
    }
    if (nlinksECp >0){
      m_occECp_lb[m_current_lb] /= nlinksECp;
      m_occTriggerECp_lb[m_current_lb] /= nlinksECp;
      // 09.09.2016
      m_hitoccECp_lb[m_current_lb] /= nlinksECp;
      m_hitoccTriggerECp_lb[m_current_lb] /= nlinksECp;
    }
    if (nlinksECm >0){
      m_occECm_lb[m_current_lb] /= nlinksECm;
      m_occTriggerECm_lb[m_current_lb] /= nlinksECm;
      // 09.09.2016
      m_hitoccECm_lb[m_current_lb] /= nlinksECm;
      m_hitoccTriggerECm_lb[m_current_lb] /= nlinksECm;
    }
    //
    m_events_lb = 0;
    m_eventsTrigger_lb = 0;
  }
  m_current_lb = pEvent->lumiBlock();
  // If track hits are selected, make the vector of track rdo identifiers
  if (m_doTrackHits) {
    if (makeVectorOfTrackRDOIdentifiers().isFailure() and msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Couldn't make vector of track RDO identifiers" << endmsg;
    }
  }
  if (generalHistsandNoise().isFailure() and msgLvl(MSG::WARNING)) {
    msg(MSG::WARNING) << "Error in generalHists" << endmsg;
  }
  if (makeSPvsEventNumber().isFailure() and msgLvl(MSG::WARNING)) {
    msg(MSG::WARNING) << "Error in makeSPvsEventNumber" << endmsg;
  }

  if (m_environment == AthenaMonManager::online) {
    if ((m_numberOfEvents % m_checkrate) == 0) {
      if (checkNoiseMaps(true).isFailure()) {
        if (msgLvl(MSG::WARNING)) {
          msg(MSG::WARNING) << "Error in checkNoiseMaps(true)" << endmsg;
        }
      }
    }
    if ((m_current_lb % m_checkrecent == 0) && (m_current_lb > m_last_reset_lb)) {
      if (checkNoiseMaps(true).isFailure()) {
        if (msgLvl(MSG::WARNING)) {
          msg(MSG::WARNING) << "Error in checkNoiseMaps(true)" << endmsg;
        }
      }
      if (resetNoiseMapsRecent().isFailure()) {
        if (msgLvl(MSG::WARNING)) {
          msg(MSG::WARNING) << "Error in resetNoiseMapsRecent" << endmsg;
        }
      }
      if (resetHitMapHists().isFailure()) {
        if (msgLvl(MSG::WARNING)) {
          msg(MSG::WARNING) << "Error in resetHitMapsRecent" << endmsg;
        }
      }
      m_last_reset_lb = m_current_lb;
      m_numberOfEventsRecent = 0;
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                      SCTHitsNoiseMonTool :: procHistograms
// PJB 08/08/2005
// Here we have the statistics by plane which can be written to the
// overall statistics object (the map) and then sent to the DB
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::procHistograms() {
  bool endOfEventsBlock(endOfLumiBlockFlag());

  if (endOfEventsBlock) {
    if (checkNoiseMaps(true).isFailure()) {
      if (msgLvl(MSG::WARNING)) {
        msg(MSG::WARNING) << "Error in checkNoiseMaps(true)" << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                   SCTHitsNoiseMonTool :: checkHists
// DD 08/10/2010
// Inherited method from base class that gets called by monitoring finalize after procHists - not used
// at this time. Previously was and unnecessary middle function between procHistograms and
// checkNoiseMaps which caused checkNoiseMaps to be called twice float-filling NO histograms
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::checkHists(bool /*fromFinalize*/) {
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTHitsNoiseMonTool :: generalHistsandNoise
//
// This method adds the spBasedNoise algorithm to the main RDO loop in an attempt to speed things up
// Once this works, it'll be the default- the old methods will be left as backups because we know they definitely work!
// This code was added by Martin White in June 2008
// Note that this is the only function with the online IPC functionality implemented- if you revert to the old methods
// you'll lose it
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::generalHistsandNoise() {
  typedef SCT_RDORawData SCTRawDataType;
  SG::ReadHandle<SCT_RDO_Container> p_rdocontainer(m_dataObjectName);
  SG::ReadHandle<xAOD::EventInfo> pEvent(m_eventInfoKey);
  if (not pEvent.isValid()) {
    if (msgLvl(MSG::ERROR)) {
      msg(MSG::ERROR) << "Could not retrieve event info!" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  unsigned int current_lb = pEvent->lumiBlock();
  if (not p_rdocontainer.isValid()) {
    return StatusCode::FAILURE;
  }
  // Get the space point container
  SG::ReadHandle<SpacePointContainer> sctContainer(m_SCTSPContainerName);
  if (not sctContainer.isValid()) {
    return StatusCode::FAILURE;
  }
  Identifier SCT_Identifier;
  // Use new IDC
  SCT_RDO_Container::const_iterator col_it = p_rdocontainer->begin();
  SCT_RDO_Container::const_iterator lastCol = p_rdocontainer->end();
  int local_tothits = 0;
  int totalmodules = 0;
  int meanhits = 0;
  int maxrodhits = 0;
  int maxhits = 0;
  Identifier maxhits_Id;
  int minhits = 99999;
  Identifier minhits_Id;

  bool isSelectedTrigger = false;
  // EDAVIES - have now changed back to using L1_RD0_EMPTY
  if (m_doTrigger) {
    if (AthenaMonManager::dataType() != AthenaMonManager::cosmics) {
      if (SCTMotherTrigMonTool::isCalibrationNoise(m_NOTrigger)) {
        isSelectedTrigger = true;
        ++m_numberOfEventsTrigger;
      }
    }
  }
  m_events_lb++;
  if (isSelectedTrigger) {
    m_eventsTrigger_lb++;
  }

  // int local_tothits=0;
  std::vector<int> barrel_local_nhitslayer(N_BARRELSx2, 0);
  std::vector<int> ECp_local_nhitslayer(N_DISKSx2, 0);
  std::vector<int> ECm_local_nhitslayer(N_DISKSx2, 0);
  //
  std::vector<H2_t> *hitHistogramVectors[3] = {
    &m_phitsHistoVectorECm, &m_phitsHistoVector, &m_phitsHistoVectorECp
  };
  std::vector<H2_t> *hitHistogramVectorsRecent[3] = {
    &m_phitsHistoVectorRecentECm, &m_phitsHistoVectorRecent, &m_phitsHistoVectorRecentECp
  };
  H1_t hitPerLumiBlockHists[3] = {
    m_numHitsPerLumiBlockECm, m_numBarrelHitsPerLumiBlock, m_numHitsPerLumiBlockECp
  };
  std::vector<int> *hitsInLayer[3] = {
    &ECm_local_nhitslayer, &barrel_local_nhitslayer, &ECp_local_nhitslayer
  };
  const bool doThisSubsystem[3] = {
    m_doNegativeEndcap, true, m_doPositiveEndcap
  };
  //
  // Define some iterators over the SP container
  SpacePointContainer::const_iterator spContainerIterator = sctContainer->begin();
  SpacePointContainer::const_iterator spContainerIteratorEnd = sctContainer->end();
  // Outer Loop on RDO Collection
  for (; col_it != lastCol; ++col_it) {
    const InDetRawDataCollection<SCTRawDataType> *SCT_Collection(*col_it);
    if (!SCT_Collection) {
      continue;  // select only SCT RDOs
    }
    // MJW new code- try getting the ID of the collection using the identify() method
    Identifier tempID = SCT_Collection->identify();
    Identifier theWaferIdentifierOfTheRDOCollection = tempID;
    Identifier theModuleIdentifierOfTheRDOCollection = m_pSCTHelper->module_id(tempID);
    int Bec = m_pSCTHelper->barrel_ec(tempID);
    int numberOfHitsFromSPs = 0;
    int numberOfHitsFromAllRDOs = 0;
    // Now we want the space point container for this module
    // We have to compare module IDs- the SP collection is defined for the 'normal' (i.e. no stereo) module side
    // Define a set of spIDs
    set<Identifier>mySetOfSPIds;
    for (; spContainerIterator != spContainerIteratorEnd; ++spContainerIterator) {
      Identifier tempSPID = (*spContainerIterator)->identify();
      Identifier theModuleIdentifierOfTheSPCollection = m_pSCTHelper->module_id(tempSPID);
      if (theModuleIdentifierOfTheSPCollection > theModuleIdentifierOfTheRDOCollection) {
        break;
      }
      if (theModuleIdentifierOfTheRDOCollection == theModuleIdentifierOfTheSPCollection) {
        DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint = (*spContainerIterator)->begin();
        DataVector<Trk::SpacePoint>::const_iterator nextSpacePointEnd = (*spContainerIterator)->end();
        for (; nextSpacePoint != nextSpacePointEnd; ++nextSpacePoint) {
          const Trk::SpacePoint &sp = **nextSpacePoint;
          // the following is nasty; the 'normal' sides (where the sp is defined) swap from layer to layer. To be safe,
          // we get both sides
          const VecId_t &rdoList0 = sp.clusterList().first->rdoList();
          const VecId_t &rdoList1 = sp.clusterList().second->rdoList();
          // copy to mySetOfSPIds. Use inserter(set, iterator_hint) for a set, or back_inserter(vec) for vector...
          copy(rdoList0.begin(), rdoList0.end(), inserter(mySetOfSPIds, mySetOfSPIds.end()));
          copy(rdoList1.begin(), rdoList1.end(), inserter(mySetOfSPIds, mySetOfSPIds.end()));
        }
        break;
      }
    }
    totalmodules++;
    maxrodhits = 0;
    // Now we loop over the RDOs in the RDO collection, and add to the NO vector any that are in the mySetOfSPIds
    DataVector<SCTRawDataType>::const_iterator p_rdo = SCT_Collection->begin();
    DataVector<SCTRawDataType>::const_iterator end_rdo = SCT_Collection->end();
    for (; p_rdo != end_rdo; ++p_rdo) {
      const SCT3_RawData *rdo3 = dynamic_cast<const SCT3_RawData *>(*p_rdo);
      int tbin;// = (*p_rdo)->getTimeBin();
      if (rdo3 != 0) {
        tbin = (rdo3)->getTimeBin();
      }else {
        tbin = 3;
      }
      SCT_Identifier = (*p_rdo)->identify();
      const unsigned int firstStrip(m_pSCTHelper->strip(SCT_Identifier));
      const unsigned int numberOfStrips((*p_rdo)->getGroupSize());
      int thisBec = m_pSCTHelper->barrel_ec(SCT_Identifier);
      int thisLayerDisk = m_pSCTHelper->layer_disk(SCT_Identifier);
      int thisPhi = m_pSCTHelper->phi_module(SCT_Identifier);
      int thisEta = m_pSCTHelper->eta_module(SCT_Identifier);
      int thisSide = m_pSCTHelper->side(SCT_Identifier);
      int thisElement = (2 * thisLayerDisk) + thisSide;
      // CAM adds mod id needed for map
      Identifier wafer_id = m_pSCTHelper->wafer_id(SCT_Identifier);
      unsigned int chan(firstStrip), limit(chan + numberOfStrips);
      unsigned int systemIndex = bec2Index(thisBec);
      // CAM wafer id was here
      unsigned int module_hash = m_pSCTHelper->wafer_hash(wafer_id);
      //
      if (doThisSubsystem[systemIndex]) {
        H2_t histogram = (*hitHistogramVectors[systemIndex])[thisElement];
        H2_t histogram_recent = 0;
        if (m_environment == AthenaMonManager::online) {
          histogram_recent = (*hitHistogramVectorsRecent[systemIndex])[thisElement];
        }

        if (find(m_RDOsOnTracks.begin(), m_RDOsOnTracks.end(), SCT_Identifier) != m_RDOsOnTracks.end()) {
          for (unsigned int ichan = chan; ichan != limit; ++ichan) {
            // Fill the track hits histogram
            if (thisBec == 0) {
              m_tbinHisto->Fill(tbin, 1.);
              m_tbinHistoVector[ thisLayerDisk ]->Fill(tbin, 1.);
              m_ptrackhitsHistoVector[thisElement]->Fill(thisEta, thisPhi);
              int layersidenum = thisLayerDisk * 2 + thisSide;
              if (tbin == 2 || tbin == 3) {
                m_tbinfrac[layersidenum]->Fill(thisEta, thisPhi, 1.);
                m_tbinfracVsLB->Fill(current_lb, 1.0);
                m_tbinfracall->Fill(1.5, 1.0);
              }else {
                m_tbinfrac[layersidenum]->Fill(thisEta, thisPhi, 0.);
                m_tbinfracVsLB->Fill(current_lb, 0.0);
                m_tbinfracall->Fill(1.5, 0.0);
              }
              if (m_environment == AthenaMonManager::online) {
                m_tbinHistoRecent->Fill(tbin, 1.);
                m_tbinHistoVectorRecent[ thisLayerDisk ]->Fill(tbin, 1.);
                m_ptrackhitsHistoVectorRecent[thisElement]->Fill(thisEta, thisPhi);
              }
            }
            if (thisBec == 2) {
              m_tbinHistoECp->Fill(tbin, 1.);
              m_tbinHistoVectorECp[ thisLayerDisk ]->Fill(tbin, 1.);
              m_ptrackhitsHistoVectorECp[thisElement]->Fill(thisEta, thisPhi);
              int layersidenum = thisLayerDisk * 2 + thisSide;
              if (tbin == 2 || tbin == 3) {
                m_tbinfracECp[layersidenum]->Fill(thisEta, thisPhi, 1.);
                m_tbinfracVsLBECp->Fill(current_lb, 1.0);
                m_tbinfracall->Fill(2.5, 1.0);
              }else {
                m_tbinfracECp[layersidenum]->Fill(thisEta, thisPhi, 0.);
                m_tbinfracVsLBECp->Fill(current_lb, 0.0);
                m_tbinfracall->Fill(2.5, 0.0);
              }
              if (m_environment == AthenaMonManager::online) {
                m_tbinHistoRecentECp->Fill(tbin, 1.);
                m_tbinHistoVectorRecentECp[ thisLayerDisk ]->Fill(tbin, 1.);
                m_ptrackhitsHistoVectorRecentECp[thisElement]->Fill(thisEta, thisPhi);
              }
            }
            if (thisBec == -2) {
              m_tbinHistoECm->Fill(tbin, 1.);
              m_tbinHistoVectorECm[ thisLayerDisk ]->Fill(tbin, 1.);
              m_ptrackhitsHistoVectorECm[thisElement]->Fill(thisEta, thisPhi);
              int layersidenum = thisLayerDisk * 2 + thisSide;
              if (tbin == 2 || tbin == 3) {
                m_tbinfracECm[layersidenum]->Fill(thisEta, thisPhi, 1.);
                m_tbinfracVsLBECm->Fill(current_lb, 1.0);
                m_tbinfracall->Fill(0.5, 1.0);
              }else {
                m_tbinfracECm[layersidenum]->Fill(thisEta, thisPhi, 0.);
                m_tbinfracVsLBECm->Fill(current_lb, 0.0);
                m_tbinfracall->Fill(0.5, 0.0);
              }
              if (m_environment == AthenaMonManager::online) {
                m_tbinHistoRecentECm->Fill(tbin, 1.);
                m_tbinHistoVectorRecentECm[ thisLayerDisk ]->Fill(tbin, 1.);
                m_ptrackhitsHistoVectorRecentECm[thisElement]->Fill(thisEta, thisPhi);
              }
            }
          }
        }
        histogram->Fill(thisEta, thisPhi, numberOfStrips);
        if (m_environment == AthenaMonManager::online) {
          histogram_recent->Fill(thisEta, thisPhi, numberOfStrips);
        }
        numberOfHitsFromAllRDOs += numberOfStrips;
        // Fill hits per lumi block histogram
        hitPerLumiBlockHists[systemIndex]->Fill(thisLayerDisk, numberOfStrips);
        // Record number of hits in space points if timebin filtering is on hits not in bin X1X are counted as in space
        // points
        if (mySetOfSPIds.find(SCT_Identifier) != mySetOfSPIds.end()) {
          numberOfHitsFromSPs += numberOfStrips;
        } else if (m_doTimeBinFilteringForNoise && !timeBinInPattern(tbin, XIX)) {
          numberOfHitsFromSPs += numberOfStrips;
        }
      }
      (*hitsInLayer[systemIndex])[thisElement] += numberOfStrips;
      local_tothits += numberOfStrips;
      maxrodhits += numberOfStrips;

      if (m_boolhitmaps) {
        for (unsigned int ichan = chan; ichan != limit; ++ichan) {
          (m_phitmapHistoVector[module_hash])->Fill(ichan, 1.0);  // increment channel hit for this plane
        }
      }
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
    meanhits = local_tothits / totalmodules;

    if (numberOfHitsFromAllRDOs > 0) {
      int diff = numberOfHitsFromAllRDOs - numberOfHitsFromSPs;
      int num(diff);
      int den(N_STRIPS - numberOfHitsFromSPs);
      float sumocc(0.);
      if (diff < 0) {
        num = 0;
        m_skipEvents++;
        if (msgLvl(MSG::WARNING)) {
          msg(MSG::WARNING) << "Too many reconstructed space points for number of real hits" << endmsg;
        }
      }
      if (den > 0) {
        sumocc = num / static_cast<float> (den);
        m_occSumUnbiased[theWaferIdentifierOfTheRDOCollection] += sumocc;
        if (m_environment == AthenaMonManager::online) {
          m_occSumUnbiasedRecent[theWaferIdentifierOfTheRDOCollection] += sumocc;
        }
        if (isSelectedTrigger) {
          m_occSumUnbiasedTrigger[theWaferIdentifierOfTheRDOCollection] += sumocc;
        }
        if (Bec == 0) {
          m_NallHitsBAR_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
          m_NSPHitsBAR_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
          m_occSumUnbiasedBAR_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
        }
        if (Bec == 2) {
          m_NallHitsECp_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
          m_NSPHitsECp_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
          m_occSumUnbiasedECp_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
        }
        if (Bec == -2) {
          m_NallHitsECm_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
          m_NSPHitsECm_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
          m_occSumUnbiasedECm_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
        }
        m_occSumUnbiased_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
        if (isSelectedTrigger) {
          if (Bec == 0) {
            m_NallHitsTriggerBAR_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
            m_NSPHitsTriggerBAR_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
            m_occSumUnbiasedTriggerBAR_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
          }
          if (Bec == 2) {
            m_NallHitsTriggerECp_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
            m_NSPHitsTriggerECp_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
            m_occSumUnbiasedTriggerECp_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
          }
          if (Bec == -2) {
            m_NallHitsTriggerECm_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
            m_NSPHitsTriggerECm_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
            m_occSumUnbiasedTriggerECm_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
          }
          m_occSumUnbiasedTrigger_lb[theWaferIdentifierOfTheRDOCollection] += sumocc;
        }
      }

      { // hit occupancy 09.09.2016
        float sumhitocc(0.);
        sumhitocc = static_cast<float> (numberOfHitsFromAllRDOs) / static_cast<float> (N_STRIPS);
        m_hitoccSumUnbiased[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        if (m_environment == AthenaMonManager::online) {
          m_hitoccSumUnbiasedRecent[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        }
        if (isSelectedTrigger) {
          m_hitoccSumUnbiasedTrigger[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        }
        if (Bec == 0) {
          m_HallHitsBAR_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
          m_HSPHitsBAR_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
          m_hitoccSumUnbiasedBAR_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        }
        if (Bec == 2) {
          m_HallHitsECp_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
          m_HSPHitsECp_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
          m_hitoccSumUnbiasedECp_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        }
        if (Bec == -2) {
          m_HallHitsECm_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
          m_HSPHitsECm_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
          m_hitoccSumUnbiasedECm_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        }
        m_hitoccSumUnbiased_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        if (isSelectedTrigger) {
          if (Bec == 0) {
            m_HallHitsTriggerBAR_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
            m_HSPHitsTriggerBAR_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
            m_hitoccSumUnbiasedTriggerBAR_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
          }
          if (Bec == 2) {
            m_HallHitsTriggerECp_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
            m_HSPHitsTriggerECp_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
            m_hitoccSumUnbiasedTriggerECp_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
          }
          if (Bec == -2) {
            m_HallHitsTriggerECm_vsLB->Fill(m_current_lb, numberOfHitsFromAllRDOs);
            m_HSPHitsTriggerECm_vsLB->Fill(m_current_lb, numberOfHitsFromSPs);
            m_hitoccSumUnbiasedTriggerECm_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
          }
          m_hitoccSumUnbiasedTrigger_lb[theWaferIdentifierOfTheRDOCollection] += sumhitocc;
        }
      }// end of hit occupancy*/
    }
  }// End of Loop on RDO container


  m_ncluHisto->Fill(local_tothits, 1);

  if (m_boolhitmaps) {
    for (int i(0); i != N_BARRELSx2; ++i) {
      m_ncluHistoVector[i]->Fill(barrel_local_nhitslayer[i], 1.);
    }
    for (int i(0); i != N_DISKSx2; ++i) {
      if (m_doPositiveEndcap) {
        m_ncluHistoVectorECp[i]->Fill(ECp_local_nhitslayer[i], 1.);
      }
      if (m_doNegativeEndcap) {
        m_ncluHistoVectorECm[i]->Fill(ECm_local_nhitslayer[i], 1.);
      }
    }
  }

  if (m_environment == AthenaMonManager::online) {
    // Time Dependent plots only online
    bool badmodule = false;
    m_nHits_buf[m_nHits_pos] = meanhits;
    m_nmaxHits_buf[m_nHits_pos] = maxhits;
    m_nmaxModule_buf[m_nHits_pos] = maxhits_Id;
    m_nminHits_buf[m_nHits_pos] = minhits;
    m_nminModule_buf[m_nHits_pos] = minhits_Id;
    m_nHits_pos++;
    if (m_nHits_pos == m_evtsbins) {
      m_nHits_pos = 0;
    }

    if (m_numberOfEvents % m_checkrate == 0) {
      m_nHits->Reset();
      m_nmaxHits->Reset();
      Int_t latest_nHits_pos = m_nHits_pos;
      m_nminHits->Reset();
      // Check if the same module is the noisiest one for at least 10 events
      // Need to add a similar check for modules with the minimum number of hits
      if (m_nHits_pos > 10) {
        for (int s = 10; s > 0; --s) {
          int npos = m_nHits_pos - s;
          if (m_nmaxModule_buf[npos] == m_nmaxModule_buf[npos - 1]) {
            badmodule = true;
          } else {
            badmodule = false;
          }
        }
      } else if (m_nHits_pos < 10 && m_numberOfEvents > m_evtsbins) {
        for (int s = 10; s > 0; --s) {
          int npos = m_nHits_pos - s;
          int nppos = m_nHits_pos - s - 1;
          if (npos < 0) {
            npos = m_evtsbins + npos;
          }
          // copy-and-paste error? coverity id 29910
          if (nppos < 0) {
            nppos = m_evtsbins + nppos;
          }
          if (m_nmaxModule_buf[npos] == m_nmaxModule_buf[nppos]) {
            badmodule = true;
          } else {
            badmodule = false;
          }
        }
      }
      // Fill the histograms with the values from the buffer
      for (Int_t i = 1; i != m_evtsbins; ++i) {
        if (latest_nHits_pos == m_evtsbins) {
          latest_nHits_pos = 0;
        }

        if (m_numberOfEvents < m_evtsbins) {
          if (i < m_nHits_pos) {
            m_nHits->SetBinContent(i, m_nHits_buf[i]);
            m_nmaxHits->SetBinContent(i, m_nmaxHits_buf[i]);
            m_nminHits->SetBinContent(i, m_nminHits_buf[i]);
          }else {
            m_nHits->SetBinContent(i, 0);
            m_nmaxHits->SetBinContent(i, 0);
            m_nminHits->SetBinContent(i, 0);
          }
        } else {
          // Fill average histo
          m_nHits->SetBinContent(i, m_nHits_buf[latest_nHits_pos]);
          m_nHits->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
          // Fill max histo
          m_nmaxHits->SetBinContent(i, m_nmaxHits_buf[latest_nHits_pos]);
          m_nmaxHits->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
          // Fill min histo
          m_nminHits->SetBinContent(i, m_nminHits_buf[latest_nHits_pos]);
          m_nminHits->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
          // Change color of the Line if found a noisy module
          // Need to add similar system for minimum histogram
          if (badmodule) {
            m_nmaxHits->SetFillColor(2);
          } else {
            m_nmaxHits->SetLineColor(0);
          }
        }
        latest_nHits_pos++;
        if (latest_nHits_pos == m_evtsbins) {
          latest_nHits_pos = 0;
        }
      }
    }
  }

  // if(m_environment!=AthenaMonManager::online){ // Uncomment this line to turn off cluster hists in online
  // Fill Cluster size histogram
  SG::ReadHandle<InDet::SCT_ClusterContainer> p_clucontainer(m_clusContainerKey);
  if (not p_clucontainer.isValid()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Couldn't retrieve clusters" << endmsg;
    }
  }
  InDet::SCT_ClusterContainer::const_iterator clucol_it = p_clucontainer->begin();
  InDet::SCT_ClusterContainer::const_iterator lastcluCol = p_clucontainer->end();
  for (; clucol_it != lastcluCol; ++clucol_it) {
    const InDet::SCT_ClusterCollection *SCT_Collection(*clucol_it);
    if (!SCT_Collection) {
      continue; // select only SCT RDOs
    }
    // Middle Loop on SCT Clusters only
    DataVector<InDet::SCT_Cluster>::const_iterator p_clu(SCT_Collection->begin());
    DataVector<InDet::SCT_Cluster>::const_iterator clus_end = SCT_Collection->end();
    for (; p_clu != clus_end; ++p_clu) {
      Identifier cluId = (*p_clu)->identify();
      const InDet::SCT_Cluster &cluster = **p_clu;
      int GroupSize = cluster.rdoList().size();
      // Fill  Cluster Size histogram
      int elementIndex = 2 * m_pSCTHelper->layer_disk(cluId) + m_pSCTHelper->side(cluId);
      int thisPhi = m_pSCTHelper->phi_module(cluId);
      int thisEta = m_pSCTHelper->eta_module(cluId);
      m_clusize->Fill(GroupSize, 1.);
      if (m_environment == AthenaMonManager::online) {
        m_clusizeRecent->Fill(GroupSize, 1.);
      }
      if (m_pSCTHelper->barrel_ec(cluId) == BARREL) {
        m_clusizeHistoVector[elementIndex]->Fill(GroupSize, 1.);
        m_clusizedist[elementIndex]->Fill(thisEta, thisPhi, GroupSize);
        if (m_environment == AthenaMonManager::online) {
          m_clusizeHistoVectorRecent[elementIndex]->Fill(GroupSize, 1.);
        }
      }else if (m_doPositiveEndcap && m_pSCTHelper->barrel_ec(cluId) == ENDCAP_A) {
        m_clusizeHistoVectorECp[elementIndex]->Fill(GroupSize, 1.);
        m_clusizedistECp[elementIndex]->Fill(thisEta, thisPhi, GroupSize);
        if (m_environment == AthenaMonManager::online) {
          m_clusizeHistoVectorRecentECp[elementIndex]->Fill(GroupSize, 1.);
        }
      } else if (m_doNegativeEndcap && m_pSCTHelper->barrel_ec(cluId) == ENDCAP_C) {
        m_clusizeHistoVectorECm[elementIndex]->Fill(GroupSize, 1.);
        m_clusizedistECm[elementIndex]->Fill(thisEta, thisPhi, GroupSize);
        if (m_environment == AthenaMonManager::online) {
          m_clusizeHistoVectorRecentECm[elementIndex]->Fill(GroupSize, 1.);
        }
      }
    }
  }
  // } // Uncomment this line as well to turn off cluster hists in online

  if ((m_numberOfEvents % m_checkrate) == 0) {
    if (checkHists(false).isFailure()) {
      if (msgLvl(MSG::WARNING)) {
        msg(MSG::WARNING) << "Error in checkHists(false)" << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTHitsNoiseMonTool :: book1DHitHists
// Book HitMaps histograms for all present SCT modules
// using ToolRootHistSvc()
// The histograms go to the subdirectory ../hitmaps/
// to improve readability
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::book1DHitHists() {
  if (newRunFlag()) {
    m_phitmapHistoVector.clear();
    std::string stem;
    MonGroup BarrelHitHists(this, "SCT/SCTB/hitmaps", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    MonGroup PlusECHitHists(this, "SCT/SCTEA/hitmaps", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    MonGroup MinusECHitHists(this, "SCT/SCTEC/hitmaps", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    //
    SCT_ID::const_id_iterator planeIterator(m_pSCTHelper->wafer_begin());
    SCT_ID::const_id_iterator planeEnd = m_pSCTHelper->wafer_end();
    for (; planeIterator not_eq planeEnd; ++planeIterator) {
      Identifier planeId(*planeIterator);
      const int bec(m_pSCTHelper->barrel_ec(planeId));
      const string formattedPosition(positionString(*planeIterator));
      std::string histotitle = string("SCT ") + names[bec2Index(bec)] + string(" Hitmap: plane ") + formattedPosition;
      if (bec == BARREL) {
        h1Factory(formattedPosition, histotitle, BarrelHitHists, m_phitmapHistoVector, FIRST_STRIP - 0.5,
                  LAST_STRIP + 0.5, N_BINS);
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Have registered the barrel hists" << endmsg;
        }
      } else if (m_doPositiveEndcap && bec == ENDCAP_A) {
        h1Factory(formattedPosition, histotitle, PlusECHitHists, m_phitmapHistoVector, FIRST_STRIP - 0.5,
                  LAST_STRIP + 0.5, N_BINS);
      }else if (m_doNegativeEndcap && bec == ENDCAP_C) {
        h1Factory(formattedPosition, histotitle, MinusECHitHists, m_phitmapHistoVector, FIRST_STRIP - 0.5,
                  LAST_STRIP + 0.5, N_BINS);
      }
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Have registered the hists" << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
SCTHitsNoiseMonTool::bookGeneralHits(const unsigned int systemIndex) {
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));

  if (bec == INVALID_SYSTEM) {
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endmsg;
    return StatusCode::FAILURE;
  }
  const string paths[] = {
    "SCT/SCTEC/hits", "SCT/SCTB/hits", "SCT/SCTEA/hits"
  };
  const unsigned int limits[] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };
  //
  if (newLumiBlockFlag()) {
    MonGroup lumiHits(this, paths[systemIndex], lumiBlock, ATTRIB_UNMANAGED);
    switch (bec) {
    case ENDCAP_C: {
      m_numHitsPerLumiBlockECm = h1Factory("numHitsPerLBECm", "Number of Endcap C hits in a luminosity block", lumiHits,
                                           0., N_DISKS, N_DISKS);
      m_numHitsPerLumiBlockECm->GetXaxis()->SetTitle("Disk");
      m_numHitsPerLumiBlockECm->GetYaxis()->SetTitle("Num of Hits Per LumiBlock");
      m_numSPPerLumiBlockECm = h1Factory("numSPPerLBECm", "Number of Endcap C spacepoints in a luminosity block",
                                         lumiHits, 0., N_DISKS, N_DISKS);
      m_numSPPerLumiBlockECm->GetXaxis()->SetTitle("Disk");
      m_numSPPerLumiBlockECm->GetYaxis()->SetTitle("Num of SpacePoint Per LumiBlock");
      break;
    }

    case BARREL: {
      m_numBarrelHitsPerLumiBlock = h1Factory("numBarrelHitsPerLB", "Number of barrel hits in a luminosity block",
                                              lumiHits, 0., N_BARRELS, N_BARRELS);
      m_numBarrelHitsPerLumiBlock->GetXaxis()->SetTitle("Layer");
      m_numBarrelHitsPerLumiBlock->GetYaxis()->SetTitle("Num of Hits Per LumiBlock");
      m_numBarrelSPPerLumiBlock = h1Factory("numBarrelSPPerLB", "Number of barrel spacepoints in a luminosity block",
                                            lumiHits, 0., N_BARRELS, N_BARRELS);
      m_numBarrelSPPerLumiBlock->GetXaxis()->SetTitle("Layer");
      m_numBarrelSPPerLumiBlock->GetYaxis()->SetTitle("Num of SpacePoint Per LumiBlock");
      break;
    }

    case ENDCAP_A: {
      m_numHitsPerLumiBlockECp = h1Factory("numHitsPerLBECp", "Number of Endcap A hits in a luminosity block", lumiHits,
                                           0., N_DISKS, N_DISKS);
      m_numHitsPerLumiBlockECp->GetXaxis()->SetTitle("Disk");
      m_numHitsPerLumiBlockECp->GetYaxis()->SetTitle("Num of Hits Per LumiBlock");
      m_numSPPerLumiBlockECp = h1Factory("numSPPerLBECp", "Number of Endcap A spacepoints in a luminosity block",
                                         lumiHits, 0., N_DISKS, N_DISKS);
      m_numSPPerLumiBlockECp->GetXaxis()->SetTitle("Disk");
      m_numSPPerLumiBlockECp->GetYaxis()->SetTitle("Num of SpacePoint Per LumiBlock");

      break;
    }

    default: {
      msg(MSG::FATAL) << "Invalid barrel/endcap indicator, should be -2, 0 or 2, was " << bec << endmsg;
      return StatusCode::FAILURE;

      break;
    }
    }
  }
  if (newRunFlag()) {
    VecH2_t *hitsArray[] = {
      &m_phitsHistoVectorECm, &m_phitsHistoVector, &m_phitsHistoVectorECp
    };
    VecH2_t *hitsArrayRecent[] = {
      &m_phitsHistoVectorRecentECm, &m_phitsHistoVectorRecent, &m_phitsHistoVectorRecentECp
    };
    VecH1_t *nClustersArray[] = {
      &m_ncluHistoVectorECm, &m_ncluHistoVector, &m_ncluHistoVectorECp
    };
    (hitsArray[systemIndex])->clear();
    (hitsArrayRecent[systemIndex])->clear();
    (nClustersArray[systemIndex])->clear();
    MonGroup hitHists(this, paths[systemIndex], run, ATTRIB_UNMANAGED);
    for (unsigned int i(0); i != limits[systemIndex]; ++i) {
      LayerSideFormatter layerSide(i, systemIndex);
      string streamhitmap = "hitsmap" + abbreviations[systemIndex] + "_" + layerSide.name();
      string streamhitmaprecent = "hitsmaprecent" + abbreviations[systemIndex] + "_" + layerSide.name();
      string streamhits = "hits" + abbreviations[systemIndex] + "_" + layerSide.name();
      std::string histotitle = "SCT Hitmap for " + names[systemIndex] + ": " + layerSide.title();
      std::string histotitlerecent = "SCT Hitmap from recent events for " + names[systemIndex] + ": " +
	layerSide.title();
      h2Factory(streamhitmap, histotitle, bec, hitHists, *(hitsArray[systemIndex]));
      if (m_environment == AthenaMonManager::online) {
        h2Factory(streamhitmaprecent, histotitlerecent, bec, hitHists, *(hitsArrayRecent[systemIndex]));
      }
      histotitle = "SCT Hits for " + names[systemIndex] + ": " + layerSide.title();
      if (m_boolhitmaps) {
        h1Factory(streamhits, histotitle, hitHists, *(nClustersArray[systemIndex]), FIRST_HIT_BIN, LAST_HIT_BIN,
                   N_HIT_BINS);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
SCTHitsNoiseMonTool::bookClusterSize() {
  MonGroup clusizebGroup(this, "SCT/SCTB/hits", run, ATTRIB_UNMANAGED);
  MonGroup clusizeeaGroup(this, "SCT/SCTEA/hits", run, ATTRIB_UNMANAGED);
  MonGroup clusizeecGroup(this, "SCT/SCTEC/hits", run, ATTRIB_UNMANAGED);
  std::string disksidenameB[] = {
    "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1"
  };
  std::string disksidenameECp[] = {
    "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1", "4_0", "4_1", "5_0", "5_1", "6_0", "6_1", "7_0", "7_1",
    "8_0", "8_1"
  };
  std::string disksidenameECm[] = {
    "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1", "4_0", "4_1", "5_0", "5_1", "6_0", "6_1", "7_0", "7_1",
    "8_0", "8_1"
  };

  for (int i = 0; i < 8; i++) {
    m_clusizedist[i] = prof2DFactory("clusize_dist_B_" + disksidenameB[i], "cluster size in Barrel_" + disksidenameB[i],
                                     clusizebGroup, 13, -6.5, 6.5, 56, -0.5, 55.5);
  }
  for (int i = 0; i < 18; i++) {
    m_clusizedistECp[i] = prof2DFactory("clusize_dist_EA_" + disksidenameECp[i],
                                        "cluster size in EndcapA_" + disksidenameECp[i], clusizeeaGroup, 3, -0.5, 2.5,
                                        52, -0.5, 51.5);
    m_clusizedistECm[i] = prof2DFactory("clusize_dist_EC_" + disksidenameECm[i],
                                        "cluster size in EndcapC_" + disksidenameECm[i], clusizeecGroup, 3, -0.5, 2.5,
                                        52, -0.5, 51.5);
  }
  if (newRunFlag()) {
    MonGroup BarrelCluSize(this, "SCT/GENERAL/hits", run, ATTRIB_UNMANAGED);
    // book Cluster width histogram for all SCT Detector
    m_clusize = h1Factory("clu_size", "SCT Cluster size", BarrelCluSize, 0., 200., 200);
    m_clusize->GetXaxis()->SetTitle("Cluster Size");
    m_clusize->GetYaxis()->SetTitle("Num of Events");
    for (int i = 0; i < N_BARRELSx2; i++) {
      m_clusizeHistoVector[i]->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeHistoVector[i]->GetYaxis()->SetTitle("Num of Events");
    }
    for (int m = 0; m < N_DISKSx2; m++) {
      m_clusizeHistoVectorECp[m]->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeHistoVectorECp[m]->GetYaxis()->SetTitle("Num of Events");
      m_clusizeHistoVectorECm[m]->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeHistoVectorECm[m]->GetYaxis()->SetTitle("Num of Events");
    }

    if (m_environment == AthenaMonManager::online) {
      m_clusizeRecent = h1Factory("clu_size_recent", "SCT Cluster size from recent events", BarrelCluSize, 0., 200.,
                                   200);
      m_clusizeRecent->GetXaxis()->SetTitle("Cluster Size");
      m_clusizeRecent->GetYaxis()->SetTitle("Num of Events");
      for (int ii = 0; ii < N_BARRELSx2; ii++) {
        m_clusizeHistoVectorRecent[ii]->GetXaxis()->SetTitle("Cluster Size");
        m_clusizeHistoVectorRecent[ii]->GetYaxis()->SetTitle("Num of Events");
      }
      for (int mm = 0; mm < N_DISKSx2; mm++) {
        m_clusizeHistoVectorRecentECp[mm]->GetXaxis()->SetTitle("Cluster Size");
        m_clusizeHistoVectorRecentECp[mm]->GetYaxis()->SetTitle("Num of Events");
        m_clusizeHistoVectorRecentECm[mm]->GetXaxis()->SetTitle("Cluster Size");
        m_clusizeHistoVectorRecentECm[mm]->GetYaxis()->SetTitle("Num of Events");
      }
    }
  }
  return m_clusize ? (StatusCode::SUCCESS) : (StatusCode::FAILURE);
}

StatusCode
SCTHitsNoiseMonTool::bookGeneralCluSize(const unsigned int systemIndex) {
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));
  VecH1_t *clusterSizeArray[] = {
    &m_clusizeHistoVectorECm, &m_clusizeHistoVector, &m_clusizeHistoVectorECp
  };
  VecH1_t *clusterSizeArrayRecent[] = {
    &m_clusizeHistoVectorRecentECm, &m_clusizeHistoVectorRecent, &m_clusizeHistoVectorRecentECp
  };




  const string paths[] = {
    "SCT/SCTEC/hits", "SCT/SCTB/hits", "SCT/SCTEA/hits"
  };
  const unsigned int limits[] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };

  if (bec == INVALID_SYSTEM) {
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endmsg;
    return StatusCode::FAILURE;
  }
  VecH1_t &clusterSizeVector = *(clusterSizeArray[systemIndex]);
  VecH1_t &clusterSizeVectorRecent = *(clusterSizeArrayRecent[systemIndex]);
  if (newRunFlag()) {
    clusterSizeVector.clear();
    clusterSizeVectorRecent.clear();
    MonGroup clusterSize(this, paths[systemIndex], run, ATTRIB_UNMANAGED);
    for (unsigned int i(0); i != limits[systemIndex]; ++i) {
      LayerSideFormatter layerSide(i, systemIndex);
      const string streamclusize = "clusize" + abbreviations[systemIndex] + "_" + layerSide.name();
      std::string histotitle = "SCT " + names[systemIndex] + " Cluster size: " + layerSide.title();
      h1Factory(streamclusize, histotitle, clusterSize, clusterSizeVector, 0., 200., 200);
      // h1Factory(streamclusize,histotitle);
      // clusterSizeVector[systemIndex]->GetXaxis()->SetTitle("Cluster Size");
      // clusterSizeVector[systemIndex]->GetYaxis()->SetTitle("Num of Events");
      if (m_environment == AthenaMonManager::online) {
        const string streamclusizerecent = "clusize_recent" + abbreviations[systemIndex] + "_" + layerSide.name();
        std::string histotitlerecent = "SCT " + names[systemIndex] + " Cluster size from recent events: " +
	  layerSide.title();
        h1Factory(streamclusizerecent, histotitlerecent, clusterSize, clusterSizeVectorRecent, 0., 200., 200);
        // clusterSizeVectorRecent[systemIndex]->GetXaxis()->SetTitle("Cluster Size");
        // clusterSizeVectorRecent[systemIndex]->GetYaxis()->SetTitle("Num of Events");
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
SCTHitsNoiseMonTool::bookGeneralNoiseOccupancyMaps(const unsigned int systemIndex) {
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));

  if (bec == INVALID_SYSTEM) {
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endmsg;
    return StatusCode::FAILURE;
  }
  if (newRunFlag()) {
    const string paths[] = {
      "SCT/SCTEC/Noise", "SCT/SCTB/Noise", "SCT/SCTEA/Noise"
    };
    const unsigned int limits[] = {
      N_DISKSx2, N_BARRELSx2, N_DISKSx2
    };
    VecProf2_t *storageVectors[] = {
      &m_pnoiseoccupancymapHistoVectorECm, &m_pnoiseoccupancymapHistoVector, &m_pnoiseoccupancymapHistoVectorECp
    };
    VecProf2_t *storageVectorsTrigger[] = {
      &m_pnoiseoccupancymapHistoVectorECmTrigger, &m_pnoiseoccupancymapHistoVectorTrigger,
      &m_pnoiseoccupancymapHistoVectorECpTrigger
    };
    VecProf2_t *storageVectorsRecent[] = {
      &m_pnoiseoccupancymapHistoVectorECmRecent, &m_pnoiseoccupancymapHistoVectorRecent,
      &m_pnoiseoccupancymapHistoVectorECpRecent
    };

    MonGroup noiseOccMaps(this, paths[systemIndex], run, ATTRIB_UNMANAGED);
    (storageVectors[systemIndex])->clear();
    (storageVectorsTrigger[systemIndex])->clear();
    (storageVectorsRecent[systemIndex])->clear();
    // book 2D "noise" maps, containing hits that aren't associated to tracks
    for (unsigned int i = 0; i != limits[systemIndex]; ++i) {
      LayerSideFormatter layerSide(i, systemIndex);
      const string streamhitmap = "noiseoccupancymap" + abbreviations[systemIndex] + "_" + layerSide.name();
      const string streamhitmaptrigger = "noiseoccupancymaptrigger" + abbreviations[systemIndex] + "_" +
	layerSide.name();
      const string streamhitmaprecent = "noiseoccupancymaprecent" + abbreviations[systemIndex] + "_" + layerSide.name();
      std::string histotitle = "SCT Noise Occupancy map for " + names[systemIndex] + ": " + layerSide.title();
      std::string histotitletrigger = "SCT Noise Occupancy map for " + m_NOTrigger + " Trigger and " +
	names[systemIndex] + ": " + layerSide.title();
      std::string histotitlerecent = "SCT Noise Occupancy map in recent events for " + names[systemIndex] + ": " +
	layerSide.title();
      prof2Factory(streamhitmap, histotitle, bec, noiseOccMaps, *(storageVectors[systemIndex]));
      prof2Factory(streamhitmaptrigger, histotitletrigger, bec, noiseOccMaps, *(storageVectorsTrigger[systemIndex]));
      if (m_environment == AthenaMonManager::online) {
        prof2Factory(streamhitmaprecent, histotitlerecent, bec, noiseOccMaps, *(storageVectorsRecent[systemIndex]));
      }
    }
    if (systemIndex == 0) { // ECm=ECC
      m_NallHitsECm_vsLB = TProfile_LW::create("h_NallHitsECm_vsLB", "Average num of all Hits in ECm vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                        SCT_Monitoring::NBINS_LBs + 0.5);
      m_NallHitsECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NallHitsECm_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (noiseOccMaps.regHist(m_NallHitsECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NallHitsECm_vsLB" << endmsg;
      }
      m_NSPHitsECm_vsLB = TProfile_LW::create("h_NSPHitsECm_vsLB", "Average num of SP Hits in ECm vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                       SCT_Monitoring::NBINS_LBs + 0.5);
      m_NSPHitsECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NSPHitsECm_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (noiseOccMaps.regHist(m_NSPHitsECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NSPHitsECm_vsLB" << endmsg;
      }
      m_ECmNO_vsLB =
        TProfile_LW::create("ECCNO_vsLB", "NO vs LB for the EndCap C (SP noise)", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECmNO_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECmNO_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      if (noiseOccMaps.regHist(m_ECmNO_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECmNO_vsLB" << endmsg;
      }
      m_NallHitsTriggerECm_vsLB = TProfile_LW::create("h_NallHitsTriggerECm_vsLB",
                                               "Average num of all Hits in ECm with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                               SCT_Monitoring::NBINS_LBs + 0.5);
      m_NallHitsTriggerECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NallHitsTriggerECm_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (noiseOccMaps.regHist(m_NallHitsTriggerECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NallHitsTriggerECm_vsLB" << endmsg;
      }
      m_NSPHitsTriggerECm_vsLB = TProfile_LW::create("h_NSPHitsTriggerECm_vsLB",
                                              "Average num of SP Hits in ECm with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                              SCT_Monitoring::NBINS_LBs + 0.5);
      m_NSPHitsTriggerECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NSPHitsTriggerECm_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (noiseOccMaps.regHist(m_NSPHitsTriggerECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NSPHitsTriggerECm_vsLB" << endmsg;
      }
      m_ECmNOTrigger_vsLB = TProfile_LW::create("ECCNOTrigger_vsLB", "NO with trigger vs LB for the EndCap C (SP noise)",
                                         SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECmNOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECmNOTrigger_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      if (noiseOccMaps.regHist(m_ECmNOTrigger_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECmNOTrigger_vsLB" << endmsg;
      }
    }
    if (systemIndex == 1) { // BAR
      m_NallHitsBAR_vsLB = TProfile_LW::create("h_NallHitsBAR_vsLB", "Average num of all Hits in Barrel vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                        SCT_Monitoring::NBINS_LBs + 0.5);
      m_NallHitsBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NallHitsBAR_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (noiseOccMaps.regHist(m_NallHitsBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NallHitsBAR_vsLB" << endmsg;
      }
      m_NSPHitsBAR_vsLB = TProfile_LW::create("h_NSPHitsBAR_vsLB", "Average num of SP Hits in Barrel vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                       SCT_Monitoring::NBINS_LBs + 0.5);
      m_NSPHitsBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NSPHitsBAR_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (noiseOccMaps.regHist(m_NSPHitsBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NSPHitsBAR_vsLB" << endmsg;
      }
      m_BARNO_vsLB = TProfile_LW::create("BARNO_vsLB", "NO vs LB for the Barrel (SP noise)", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_BARNO_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_BARNO_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      if (noiseOccMaps.regHist(m_BARNO_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book BARNO_vsLB" << endmsg;
      }
      m_NallHitsTriggerBAR_vsLB = TProfile_LW::create("h_NallHitsTriggerBAR_vsLB",
                                               "Average num of all Hits in Barrel with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                               SCT_Monitoring::NBINS_LBs + 0.5);
      m_NallHitsTriggerBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NallHitsTriggerBAR_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (noiseOccMaps.regHist(m_NallHitsTriggerBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NallHitsTriggerBAR_vsLB" << endmsg;
      }
      m_NSPHitsTriggerBAR_vsLB = TProfile_LW::create("h_NSPHitsTriggerBAR_vsLB",
                                              "Average num of SP Hits in Barrel with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                              SCT_Monitoring::NBINS_LBs + 0.5);
      m_NSPHitsTriggerBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NSPHitsTriggerBAR_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (noiseOccMaps.regHist(m_NSPHitsTriggerBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NSPHitsTriggerBAR_vsLB" << endmsg;
      }
      m_BARNOTrigger_vsLB = TProfile_LW::create("BARNOTrigger_vsLB", "NO with trigger vs LB for the Barrel (SP noise)",
                                         SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_BARNOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_BARNOTrigger_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      if (noiseOccMaps.regHist(m_BARNOTrigger_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book BARNOTrigger_vsLB" << endmsg;
      }
    }
    if (systemIndex == 2) { // ECp=ECA
      m_NallHitsECp_vsLB = TProfile_LW::create("h_NallHitsECp_vsLB", "Average num of all Hits in ECp vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                        SCT_Monitoring::NBINS_LBs + 0.5);
      m_NallHitsECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NallHitsECp_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (noiseOccMaps.regHist(m_NallHitsECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NallHitsECp_vsLB" << endmsg;
      }
      m_NSPHitsECp_vsLB = TProfile_LW::create("h_NSPHitsECp_vsLB", "Average num of SP Hits in ECp vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                       SCT_Monitoring::NBINS_LBs + 0.5);
      m_NSPHitsECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NSPHitsECp_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (noiseOccMaps.regHist(m_NSPHitsECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NSPHitsECp_vsLB" << endmsg;
      }
      m_ECpNO_vsLB =
        TProfile_LW::create("ECANO_vsLB", "NO vs LB for the EndCap A (SP noise)", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECpNO_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECpNO_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      if (noiseOccMaps.regHist(m_ECpNO_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECpNO_vsLB" << endmsg;
      }
      m_NallHitsTriggerECp_vsLB = TProfile_LW::create("h_NallHitsTriggerECp_vsLB",
                                               "Average num of all Hits in ECp with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                               SCT_Monitoring::NBINS_LBs + 0.5);
      m_NallHitsTriggerECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NallHitsTriggerECp_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (noiseOccMaps.regHist(m_NallHitsTriggerECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NallHitsTriggerECp_vsLB" << endmsg;
      }
      m_NSPHitsTriggerECp_vsLB = TProfile_LW::create("h_NSPHitsTriggerECp_vsLB",
                                              "Average num of SP Hits in ECp with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                              SCT_Monitoring::NBINS_LBs + 0.5);
      m_NSPHitsTriggerECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_NSPHitsTriggerECp_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (noiseOccMaps.regHist(m_NSPHitsTriggerECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NSPHitsTriggerECp_vsLB" << endmsg;
      }
      m_ECpNOTrigger_vsLB = TProfile_LW::create("ECANOTrigger_vsLB", "NO with trigger vs LB for the EndCap A (SP noise)",
                                         SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECpNOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECpNOTrigger_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
      if (noiseOccMaps.regHist(m_ECpNOTrigger_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECpNOTrigger_vsLB" << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

// 09.09.2016
StatusCode
SCTHitsNoiseMonTool::bookGeneralHitOccupancyMaps(const unsigned int systemIndex) {
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));

  if (bec == INVALID_SYSTEM) {
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endmsg;
    return StatusCode::FAILURE;
  }
  if (newRunFlag()) {
    const string paths[] = {
      "SCT/SCTEC/Noise", "SCT/SCTB/Noise", "SCT/SCTEA/Noise"
    };
    const unsigned int limits[] = {
      N_DISKSx2, N_BARRELSx2, N_DISKSx2
    };
    VecProf2_t *storageVectorsHO[] = {
      &m_phitoccupancymapHistoVectorECm, &m_phitoccupancymapHistoVector, &m_phitoccupancymapHistoVectorECp
    };
    VecProf2_t *storageVectorsTriggerHO[] = {
      &m_phitoccupancymapHistoVectorECmTrigger, &m_phitoccupancymapHistoVectorTrigger,
      &m_phitoccupancymapHistoVectorECpTrigger
    };
    VecProf2_t *storageVectorsRecentHO[] = {
      &m_phitoccupancymapHistoVectorECmRecent, &m_phitoccupancymapHistoVectorRecent,
      &m_phitoccupancymapHistoVectorECpRecent
    };

    MonGroup hitOccMaps(this, paths[systemIndex], run, ATTRIB_UNMANAGED);

    (storageVectorsHO[systemIndex])->clear();
    (storageVectorsTriggerHO[systemIndex])->clear();
    (storageVectorsRecentHO[systemIndex])->clear();

    // book 2D "noise" maps, containing hits that aren't associated to tracks
    for (unsigned int i = 0; i != limits[systemIndex]; ++i) {
      LayerSideFormatter layerSide(i, systemIndex);
      const string streamhitmapR = "hitoccupancymap" + abbreviations[systemIndex] + "_" + layerSide.name();
      const string streamhitmaptriggerR = "hitoccupancymaptrigger" + abbreviations[systemIndex] + "_" +
	layerSide.name();
      const string streamhitmaprecentR = "hitoccupancymaprecent" + abbreviations[systemIndex] + "_" + layerSide.name();
      std::string histotitleR = "SCT Hit Occupancy map for " + names[systemIndex] + ": " + layerSide.title();
      std::string histotitletriggerR = "SCT Hit Occupancy map for " + m_NOTrigger + " Trigger and " +
	names[systemIndex] + ": " + layerSide.title();
      std::string histotitlerecentR = "SCT Hit Occupancy map in recent events for " + names[systemIndex] + ": " +
	layerSide.title();
      prof2Factory(streamhitmapR, histotitleR, bec, hitOccMaps, *(storageVectorsHO[systemIndex]));
      prof2Factory(streamhitmaptriggerR, histotitletriggerR, bec, hitOccMaps, *(storageVectorsTriggerHO[systemIndex]));
      if (m_environment == AthenaMonManager::online) {
        prof2Factory(streamhitmaprecentR, histotitlerecentR, bec, hitOccMaps, *(storageVectorsRecentHO[systemIndex]));
      }
    }

    if (systemIndex == 0) { // ECm=ECC
      m_HallHitsECm_vsLB = TProfile_LW::create("h_HallHitsECm_vsLB", "Average num of all Hits in ECm vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                        SCT_Monitoring::NBINS_LBs + 0.5);
      m_HallHitsECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HallHitsECm_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (hitOccMaps.regHist(m_HallHitsECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HallHitsECm_vsLB" << endmsg;
      }
      m_HSPHitsECm_vsLB = TProfile_LW::create("h_HSPHitsECm_vsLB", "Average num of SP Hits in ECm vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                       SCT_Monitoring::NBINS_LBs + 0.5);
      m_HSPHitsECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HSPHitsECm_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (hitOccMaps.regHist(m_HSPHitsECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HSPHitsECm_vsLB" << endmsg;
      }
      m_ECmHO_vsLB = TProfile_LW::create("ECCHO_vsLB", "HO vs LB for the EndCap C", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECmHO_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECmHO_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
      if (hitOccMaps.regHist(m_ECmHO_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECmHO_vsLB" << endmsg;
      }
      m_HallHitsTriggerECm_vsLB = TProfile_LW::create("h_HallHitsTriggerECm_vsLB",
                                               "Average num of all Hits in ECm with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                               SCT_Monitoring::NBINS_LBs + 0.5);
      m_HallHitsTriggerECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HallHitsTriggerECm_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (hitOccMaps.regHist(m_HallHitsTriggerECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HallHitsTriggerECm_vsLB" << endmsg;
      }
      m_HSPHitsTriggerECm_vsLB = TProfile_LW::create("h_HSPHitsTriggerECm_vsLB",
                                              "Average num of SP Hits in ECm with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                              SCT_Monitoring::NBINS_LBs + 0.5);
      m_HSPHitsTriggerECm_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HSPHitsTriggerECm_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (hitOccMaps.regHist(m_HSPHitsTriggerECm_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HSPHitsTriggerECm_vsLB" << endmsg;
      }
      m_ECmHOTrigger_vsLB = TProfile_LW::create("ECCHOTrigger_vsLB", "HO with trigger vs LB for the EndCap C (SP noise)",
                                         SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECmHOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECmHOTrigger_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
      if (hitOccMaps.regHist(m_ECmHOTrigger_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECmHOTrigger_vsLB" << endmsg;
      }
    }
    if (systemIndex == 1) { // BAR
      m_HallHitsBAR_vsLB = TProfile_LW::create("h_HallHitsBAR_vsLB", "Average num of all Hits in Barrel vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                        SCT_Monitoring::NBINS_LBs + 0.5);
      m_HallHitsBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HallHitsBAR_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (hitOccMaps.regHist(m_HallHitsBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HallHitsBAR_vsLB" << endmsg;
      }
      m_HSPHitsBAR_vsLB = TProfile_LW::create("h_HSPHitsBAR_vsLB", "Average num of SP Hits in Barrel vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                       SCT_Monitoring::NBINS_LBs + 0.5);
      m_HSPHitsBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HSPHitsBAR_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (hitOccMaps.regHist(m_HSPHitsBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HSPHitsBAR_vsLB" << endmsg;
      }
      m_BARHO_vsLB = TProfile_LW::create("BARHO_vsLB", "HO vs LB for the Barrel (SP noise)", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_BARHO_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_BARHO_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
      if (hitOccMaps.regHist(m_BARHO_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book BARHO_vsLB" << endmsg;
      }
      m_HallHitsTriggerBAR_vsLB = TProfile_LW::create("h_HallHitsTriggerBAR_vsLB",
                                               "Average num of all Hits in Barrel with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                               SCT_Monitoring::NBINS_LBs + 0.5);
      m_HallHitsTriggerBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HallHitsTriggerBAR_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (hitOccMaps.regHist(m_HallHitsTriggerBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HallHitsTriggerBAR_vsLB" << endmsg;
      }
      m_HSPHitsTriggerBAR_vsLB = TProfile_LW::create("h_HSPHitsTriggerBAR_vsLB",
                                              "Average num of SP Hits in Barrel with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                              SCT_Monitoring::NBINS_LBs + 0.5);
      m_HSPHitsTriggerBAR_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HSPHitsTriggerBAR_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (hitOccMaps.regHist(m_HSPHitsTriggerBAR_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HSPHitsTriggerBAR_vsLB" << endmsg;
      }
      m_BARHOTrigger_vsLB = TProfile_LW::create("BARHOTrigger_vsLB", "HO with trigger vs LB for the Barrel (SP noise)",
                                         SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_BARHOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_BARHOTrigger_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
      if (hitOccMaps.regHist(m_BARHOTrigger_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book BARHOTrigger_vsLB" << endmsg;
      }
    }
    if (systemIndex == 2) { // ECp=ECA
      m_HallHitsECp_vsLB = TProfile_LW::create("h_HallHitsECp_vsLB", "Average num of all Hits in ECp vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                        SCT_Monitoring::NBINS_LBs + 0.5);
      m_HallHitsECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HallHitsECp_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (hitOccMaps.regHist(m_HallHitsECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HallHitsECp_vsLB" << endmsg;
      }
      m_HSPHitsECp_vsLB = TProfile_LW::create("h_HSPHitsECp_vsLB", "Average num of SP Hits in ECp vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                       SCT_Monitoring::NBINS_LBs + 0.5);
      m_HSPHitsECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HSPHitsECp_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (hitOccMaps.regHist(m_HSPHitsECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HSPHitsECp_vsLB" << endmsg;
      }
      m_ECpHO_vsLB =
        TProfile_LW::create("ECAHO_vsLB", "HO vs LB for the EndCap A (SP noise)", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECpHO_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECpHO_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
      if (hitOccMaps.regHist(m_ECpHO_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECpHO_vsLB" << endmsg;
      }
      m_HallHitsTriggerECp_vsLB = TProfile_LW::create("h_HallHitsTriggerECp_vsLB",
                                               "Average num of all Hits in ECp with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                               SCT_Monitoring::NBINS_LBs + 0.5);
      m_HallHitsTriggerECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HallHitsTriggerECp_vsLB->GetYaxis()->SetTitle("Average number of all Hits");
      if (hitOccMaps.regHist(m_HallHitsTriggerECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HallHitsTriggerECp_vsLB" << endmsg;
      }
      m_HSPHitsTriggerECp_vsLB = TProfile_LW::create("h_HSPHitsTriggerECp_vsLB",
                                              "Average num of SP Hits in ECp with trigger vs LB", SCT_Monitoring::NBINS_LBs, 0.5,
                                              SCT_Monitoring::NBINS_LBs + 0.5);
      m_HSPHitsTriggerECp_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_HSPHitsTriggerECp_vsLB->GetYaxis()->SetTitle("Average number of SP Hits");
      if (hitOccMaps.regHist(m_HSPHitsTriggerECp_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book HSPHitsTriggerECp_vsLB" << endmsg;
      }
      m_ECpHOTrigger_vsLB = TProfile_LW::create("ECAHOTrigger_vsLB", "HO with trigger vs LB for the EndCap A (SP noise)",
                                         SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_ECpHOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
      m_ECpHOTrigger_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
      if (hitOccMaps.regHist(m_ECpHOTrigger_vsLB).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book ECpHOTrigger_vsLB" << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: checkNoiseMaps
// LF 13/01/2006
// Here we have the check of the histograms and we can also build
// efficiency, occupancy, rms histograms, etc from the basic ones
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::checkNoiseMaps(bool final) {
  IdentifierHash next;
  std::vector<float> vectorOfOccupancies;

  if (final) {
    if (m_doSpacePointBasedNoise) {
      // CAM Added code for track and SP algorithms
      ////////////////////////////////////////////////////////
      if (m_environment == AthenaMonManager::online) { // Reset histos for online so there is proper weighting of events
        if (resetNoiseMapHists().isFailure()) {
          msg(MSG::WARNING) << "Failed to reset noise occupancy maps!" << endmsg;
        }
      }
      if (m_occSumUnbiased.size() && m_numberOfEvents) {
        for (std::map<Identifier, float>::iterator it = m_occSumUnbiased.begin(); it != m_occSumUnbiased.end(); it++) {
          Identifier wafer_id = it->first;
          int element = 2 * m_pSCTHelper->layer_disk(wafer_id) + m_pSCTHelper->side(wafer_id);
          float occ = (m_numberOfEvents > 0) ? it->second / (m_numberOfEvents) :  it->second;
          int eta = m_pSCTHelper->eta_module(wafer_id);
          int phi = m_pSCTHelper->phi_module(wafer_id);
          int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);
          if (barrel_ec == BARREL) {
            m_pnoiseoccupancymapHistoVector[element]->Fill(eta, phi, occ * 1E5);
            m_BARNO->Fill(occ * 1E5);
            m_SCTNO->Fill(occ * 1E5);
          }
          if (m_doPositiveEndcap && barrel_ec == ENDCAP_A) {
            m_pnoiseoccupancymapHistoVectorECp[element]->Fill(eta, phi, occ * 1E5);
            m_ECpNO->Fill(occ * 1E5);
            m_SCTNO->Fill(occ * 1E5);
          }
          if (m_doNegativeEndcap && barrel_ec == ENDCAP_C) {
            m_pnoiseoccupancymapHistoVectorECm[element]->Fill(eta, phi, occ * 1E5);
            m_ECmNO->Fill(occ * 1E5);
            m_SCTNO->Fill(occ * 1E5);
          }
        }
      }
      if (m_environment == AthenaMonManager::online) {
        if (m_occSumUnbiasedRecent.size() && m_numberOfEventsRecent) {
          for (std::map<Identifier, float>::iterator it = m_occSumUnbiasedRecent.begin();
               it != m_occSumUnbiasedRecent.end(); it++) {
            Identifier wafer_id = it->first;
            int element = 2 * m_pSCTHelper->layer_disk(wafer_id) + m_pSCTHelper->side(wafer_id);
            float occ = (m_numberOfEventsRecent > 0) ? it->second / (m_numberOfEventsRecent) :  it->second;
            int eta = m_pSCTHelper->eta_module(wafer_id);
            int phi = m_pSCTHelper->phi_module(wafer_id);
            int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);

            if (barrel_ec == BARREL) {
              m_pnoiseoccupancymapHistoVectorRecent[element]->Fill(eta, phi, occ * 1E5);
            }
            if (m_doPositiveEndcap && barrel_ec == ENDCAP_A) {
              m_pnoiseoccupancymapHistoVectorECpRecent[element]->Fill(eta, phi, occ * 1E5);
            }
            if (m_doNegativeEndcap && barrel_ec == ENDCAP_C) {
              m_pnoiseoccupancymapHistoVectorECmRecent[element]->Fill(eta, phi, occ * 1E5);
            }
          }
        }
      }
      if (m_occSumUnbiasedTrigger.size() && m_numberOfEventsTrigger) {
        for (std::map<Identifier, float>::iterator it = m_occSumUnbiasedTrigger.begin();
             it != m_occSumUnbiasedTrigger.end(); it++) {
          Identifier wafer_id = it->first;
          int element = 2 * m_pSCTHelper->layer_disk(wafer_id) + m_pSCTHelper->side(wafer_id);
          float occ = (m_numberOfEventsTrigger > 0) ? it->second / (m_numberOfEventsTrigger) :  it->second;
          int eta = m_pSCTHelper->eta_module(wafer_id);
          int phi = m_pSCTHelper->phi_module(wafer_id);
          int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);
          if (barrel_ec == BARREL) {
            m_pnoiseoccupancymapHistoVectorTrigger[element]->Fill(eta, phi, occ * 1E5);
            m_BARNOTrigger->Fill(occ * 1E5);
            m_SCTNOTrigger->Fill(occ * 1E5);
          }
          if (m_doPositiveEndcap && barrel_ec == ENDCAP_A) {
            m_pnoiseoccupancymapHistoVectorECpTrigger[element]->Fill(eta, phi, occ * 1E5);
            m_ECpNOTrigger->Fill(occ * 1E5);
            m_SCTNOTrigger->Fill(occ * 1E5);
          }
          if (m_doNegativeEndcap && barrel_ec == ENDCAP_C) {
            m_pnoiseoccupancymapHistoVectorECmTrigger[element]->Fill(eta, phi, occ * 1E5);
            m_ECmNOTrigger->Fill(occ * 1E5);
            m_SCTNOTrigger->Fill(occ * 1E5);
          }
        }
      }

      // Add 09.09.2016
      if (m_hitoccSumUnbiased.size() && m_numberOfEvents) {
        for (std::map<Identifier, float>::iterator it = m_hitoccSumUnbiased.begin(); it != m_hitoccSumUnbiased.end();
             it++) {
          Identifier wafer_id = it->first;
          int element = 2 * m_pSCTHelper->layer_disk(wafer_id) + m_pSCTHelper->side(wafer_id);
          float hitocc = (m_numberOfEvents > 0) ? it->second / (m_numberOfEvents) :  it->second;
          int eta = m_pSCTHelper->eta_module(wafer_id);
          int phi = m_pSCTHelper->phi_module(wafer_id);
          int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);
          if (barrel_ec == BARREL) {
            m_phitoccupancymapHistoVector[element]->Fill(eta, phi, hitocc * 1E5);
            m_BARHO->Fill(hitocc * 1E5);
            m_SCTHO->Fill(hitocc * 1E5);
          }
          if (m_doPositiveEndcap && barrel_ec == ENDCAP_A) {
            m_phitoccupancymapHistoVectorECp[element]->Fill(eta, phi, hitocc * 1E5);
            m_ECpHO->Fill(hitocc * 1E5);
            m_SCTHO->Fill(hitocc * 1E5);
          }
          if (m_doNegativeEndcap && barrel_ec == ENDCAP_C) {
            m_phitoccupancymapHistoVectorECm[element]->Fill(eta, phi, hitocc * 1E5);
            m_ECmHO->Fill(hitocc * 1E5);
            m_SCTHO->Fill(hitocc * 1E5);
          }
        }
      }
      if (m_environment == AthenaMonManager::online) {
        if (m_hitoccSumUnbiasedRecent.size() && m_numberOfEventsRecent) {
          for (std::map<Identifier, float>::iterator it = m_hitoccSumUnbiasedRecent.begin();
               it != m_hitoccSumUnbiasedRecent.end(); it++) {
            Identifier wafer_id = it->first;
            int element = 2 * m_pSCTHelper->layer_disk(wafer_id) + m_pSCTHelper->side(wafer_id);
            float hitocc = (m_numberOfEventsRecent > 0) ? it->second / (m_numberOfEventsRecent) :  it->second;
            int eta = m_pSCTHelper->eta_module(wafer_id);
            int phi = m_pSCTHelper->phi_module(wafer_id);
            int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);

            if (barrel_ec == BARREL) {
              m_phitoccupancymapHistoVectorRecent[element]->Fill(eta, phi, hitocc * 1E5);
            }
            if (m_doPositiveEndcap && barrel_ec == ENDCAP_A) {
              m_phitoccupancymapHistoVectorECpRecent[element]->Fill(eta, phi, hitocc * 1E5);
            }
            if (m_doNegativeEndcap && barrel_ec == ENDCAP_C) {
              m_phitoccupancymapHistoVectorECmRecent[element]->Fill(eta, phi, hitocc * 1E5);
            }
          }
        }
      }
      if (m_hitoccSumUnbiasedTrigger.size() && m_numberOfEventsTrigger) {
        for (std::map<Identifier, float>::iterator it = m_hitoccSumUnbiasedTrigger.begin();
             it != m_hitoccSumUnbiasedTrigger.end(); it++) {
          Identifier wafer_id = it->first;
          int element = 2 * m_pSCTHelper->layer_disk(wafer_id) + m_pSCTHelper->side(wafer_id);
          float hitocc = (m_numberOfEventsTrigger > 0) ? it->second / (m_numberOfEventsTrigger) :  it->second;
          int eta = m_pSCTHelper->eta_module(wafer_id);
          int phi = m_pSCTHelper->phi_module(wafer_id);
          int barrel_ec = m_pSCTHelper->barrel_ec(wafer_id);
          if (barrel_ec == BARREL) {
            m_phitoccupancymapHistoVectorTrigger[element]->Fill(eta, phi, hitocc * 1E5);
            m_BARHOTrigger->Fill(hitocc * 1E5);
            m_SCTHOTrigger->Fill(hitocc * 1E5);
          }
          if (m_doPositiveEndcap && barrel_ec == ENDCAP_A) {
            m_phitoccupancymapHistoVectorECpTrigger[element]->Fill(eta, phi, hitocc * 1E5);
            m_ECpHOTrigger->Fill(hitocc * 1E5);
            m_SCTHOTrigger->Fill(hitocc * 1E5);
          }
          if (m_doNegativeEndcap && barrel_ec == ENDCAP_C) {
            m_phitoccupancymapHistoVectorECmTrigger[element]->Fill(eta, phi, hitocc * 1E5);
            m_ECmHOTrigger->Fill(hitocc * 1E5);
            m_SCTHOTrigger->Fill(hitocc * 1E5);
          }
        }
      }

      if(m_current_lb<=SCT_Monitoring::NBINS_LBs) {
	m_noisyM100[m_current_lb] = 0;
	m_noisyM1000[m_current_lb] = 0;
	m_noisyM10000[m_current_lb] = 0;
	m_occ_lb[m_current_lb] = 0;
	m_occBAR_lb[m_current_lb] = 0;
	m_occECp_lb[m_current_lb] = 0;
	m_occECm_lb[m_current_lb] = 0;
	m_noisyMTrigger100[m_current_lb] = 0;
	m_noisyMTrigger1000[m_current_lb] = 0;
	m_noisyMTrigger10000[m_current_lb] = 0;
	m_occTrigger_lb[m_current_lb] = 0;
	m_occTriggerBAR_lb[m_current_lb] = 0;
	m_occTriggerECp_lb[m_current_lb] = 0;
	m_occTriggerECm_lb[m_current_lb] = 0;
	// 09.09.2016
	m_noisyMWithHO100[m_current_lb] = 0;
	m_noisyMWithHO1000[m_current_lb] = 0;
	m_noisyMWithHO10000[m_current_lb] = 0;
	m_hitocc_lb[m_current_lb] = 0;
	m_hitoccBAR_lb[m_current_lb] = 0;
	m_hitoccECp_lb[m_current_lb] = 0;
	m_hitoccECm_lb[m_current_lb] = 0;
	m_noisyMWithHOTrigger100[m_current_lb] = 0;
	m_noisyMWithHOTrigger1000[m_current_lb] = 0;
	m_noisyMWithHOTrigger10000[m_current_lb] = 0;
	m_hitoccTrigger_lb[m_current_lb] = 0;
	m_hitoccTriggerBAR_lb[m_current_lb] = 0;
	m_hitoccTriggerECp_lb[m_current_lb] = 0;
	m_hitoccTriggerECm_lb[m_current_lb] = 0;

	int nlinks = 0;
	int nlinksBAR = 0;
	int nlinksECp = 0;
	int nlinksECm = 0;
	SCT_ID::const_id_iterator planeIterator(m_pSCTHelper->wafer_begin());
	SCT_ID::const_id_iterator planeEnd = m_pSCTHelper->wafer_end();
	for (; planeIterator not_eq planeEnd; ++planeIterator) {
	  Identifier planeId(*planeIterator);
	  const int bec(m_pSCTHelper->barrel_ec(planeId));
	  // Don't initialize a value for disabled  modules
	  if (!m_ConfigurationSvc->isGood(*planeIterator, InDetConditions::SCT_SIDE)) {
	    continue;
	  }
	  if (m_events_lb > 0) {
	    m_occ_lb[m_current_lb] += (1E5) * m_occSumUnbiased_lb[*planeIterator] / m_events_lb;
	    m_occBAR_lb[m_current_lb] += (1E5) * m_occSumUnbiasedBAR_lb[*planeIterator] / m_events_lb;
	    m_occECp_lb[m_current_lb] += (1E5) * m_occSumUnbiasedECp_lb[*planeIterator] / m_events_lb;
	    m_occECm_lb[m_current_lb] += (1E5) * m_occSumUnbiasedECm_lb[*planeIterator] / m_events_lb;
	    if ((1E5) * m_occSumUnbiased_lb[*planeIterator] / m_events_lb > 100.) {
	      m_noisyM100[m_current_lb]++;
	    }
	    if ((1E5) * m_occSumUnbiased_lb[*planeIterator] / m_events_lb > 1000.) {
	      m_noisyM1000[m_current_lb]++;
	    }
	    if ((1E5) * m_occSumUnbiased_lb[*planeIterator] / m_events_lb > 10000.) {
	      m_noisyM10000[m_current_lb]++;
	    }
	    // 09.09.2016
	    m_hitocc_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb;
	    m_hitoccBAR_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedBAR_lb[*planeIterator] / m_events_lb;
	    m_hitoccECp_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedECp_lb[*planeIterator] / m_events_lb;
	    m_hitoccECm_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedECm_lb[*planeIterator] / m_events_lb;
	    if ((1E5) * m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb > 100.) {
	      m_noisyMWithHO100[m_current_lb]++;
	    }
	    if ((1E5) * m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb > 1000.) {
	      m_noisyMWithHO1000[m_current_lb]++;
	    }
	    if ((1E5) * m_hitoccSumUnbiased_lb[*planeIterator] / m_events_lb > 10000.) {
	      m_noisyMWithHO10000[m_current_lb]++;
	    }
	  }
	  if (m_eventsTrigger_lb > 0) {
	    m_occTrigger_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb;
	    m_occTriggerBAR_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTriggerBAR_lb[*planeIterator] /
	      m_eventsTrigger_lb;
	    m_occTriggerECp_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTriggerECp_lb[*planeIterator] /
	      m_eventsTrigger_lb;
	    m_occTriggerECm_lb[m_current_lb] += (1E5) * m_occSumUnbiasedTriggerECm_lb[*planeIterator] /
	      m_eventsTrigger_lb;
	    if ((1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 100.) {
	      m_noisyMTrigger100[m_current_lb]++;
	    }
	    if ((1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 1000.) {
	      m_noisyMTrigger1000[m_current_lb]++;
	    }
	    if ((1E5) * m_occSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 10000.) {
	      m_noisyMTrigger10000[m_current_lb]++;
	    }
	    // 09.09.2016
	    m_hitoccTrigger_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] /
	      m_eventsTrigger_lb;
	    m_hitoccTriggerBAR_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTriggerBAR_lb[*planeIterator] /
	      m_eventsTrigger_lb;
	    m_hitoccTriggerECp_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTriggerECp_lb[*planeIterator] /
	      m_eventsTrigger_lb;
	    m_hitoccTriggerECm_lb[m_current_lb] += (1E5) * m_hitoccSumUnbiasedTriggerECm_lb[*planeIterator] /
	      m_eventsTrigger_lb;
	    if ((1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 100.) {
	      m_noisyMWithHOTrigger100[m_current_lb]++;
	    }
	    if ((1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 1000.) {
	      m_noisyMWithHOTrigger1000[m_current_lb]++;
	    }
	    if ((1E5) * m_hitoccSumUnbiasedTrigger_lb[*planeIterator] / m_eventsTrigger_lb > 10000.) {
	      m_noisyMWithHOTrigger10000[m_current_lb]++;
	    }
	  }
	  if (bec == BARREL)  nlinksBAR++; 
	  if (bec == ENDCAP_A)nlinksECp++;
	  if (bec == ENDCAP_C)nlinksECm++;
	  nlinks++;
	}
	//
	if (nlinks >0){
	  m_occ_lb[m_current_lb] /= nlinks;
	  m_occTrigger_lb[m_current_lb] /= nlinks;
	  // 09.09.2016
	  m_hitocc_lb[m_current_lb] /= nlinks;
	  m_hitoccTrigger_lb[m_current_lb] /= nlinks;
	}
	if (nlinksBAR >0){
	  m_occBAR_lb[m_current_lb] /= nlinksBAR;
	  m_occTriggerBAR_lb[m_current_lb] /= nlinksBAR;
	  // 09.09.2016
	  m_hitoccBAR_lb[m_current_lb] /= nlinksBAR;
	  m_hitoccTriggerBAR_lb[m_current_lb] /= nlinksBAR;
	}
	if (nlinksECp >0){
	  m_occECp_lb[m_current_lb] /= nlinksECp;
	  m_occTriggerECp_lb[m_current_lb] /= nlinksECp;
	  // 09.09.2016
	  m_hitoccECp_lb[m_current_lb] /= nlinksECp;
	  m_hitoccTriggerECp_lb[m_current_lb] /= nlinksECp;
	}
	if (nlinksECm >0){
	  m_occECm_lb[m_current_lb] /= nlinksECm;
	  m_occTriggerECm_lb[m_current_lb] /= nlinksECm;
	  // 09.09.2016
	  m_hitoccECm_lb[m_current_lb] /= nlinksECm;
	  m_hitoccTriggerECm_lb[m_current_lb] /= nlinksECm;
	}
      }

      m_BARNO_vsLB->Reset();
      m_ECmNO_vsLB->Reset();
      m_ECpNO_vsLB->Reset();
      m_SCTNO_vsLB->Reset();
      m_NoisyModules100_vsLB->Reset();
      m_NoisyModules1000_vsLB->Reset();
      m_NoisyModules10000_vsLB->Reset();
      m_BARNOTrigger_vsLB->Reset();
      m_ECmNOTrigger_vsLB->Reset();
      m_ECpNOTrigger_vsLB->Reset();
      m_SCTNOTrigger_vsLB->Reset();
      m_NoisyModulesTrigger100_vsLB->Reset();
      m_NoisyModulesTrigger1000_vsLB->Reset();
      m_NoisyModulesTrigger10000_vsLB->Reset();
      // 09.09.2016
      m_BARHO_vsLB->Reset();
      m_ECmHO_vsLB->Reset();
      m_ECpHO_vsLB->Reset();
      m_SCTHO_vsLB->Reset();
      m_NoisyModulesWithHO100_vsLB->Reset();
      m_NoisyModulesWithHO1000_vsLB->Reset();
      m_NoisyModulesWithHO10000_vsLB->Reset();
      m_BARHOTrigger_vsLB->Reset();
      m_ECmHOTrigger_vsLB->Reset();
      m_ECpHOTrigger_vsLB->Reset();
      m_SCTHOTrigger_vsLB->Reset();
      m_NoisyModulesWithHOTrigger100_vsLB->Reset();
      m_NoisyModulesWithHOTrigger1000_vsLB->Reset();
      m_NoisyModulesWithHOTrigger10000_vsLB->Reset();
      //
      for (int bin = 1; bin <= SCT_Monitoring::NBINS_LBs; bin++) {
	if (m_occBAR_lb[bin] != 0) {
	  m_BARNO_vsLB->Fill(bin, m_occBAR_lb[bin]);
	}
	if (m_occECm_lb[bin] != 0) {
	  m_ECmNO_vsLB->Fill(bin, m_occECm_lb[bin]);
	}
	if (m_occECp_lb[bin] != 0) {
	  m_ECpNO_vsLB->Fill(bin, m_occECp_lb[bin]);
	}
	if (m_occ_lb[bin] != 0) {
	  m_SCTNO_vsLB->Fill(bin, m_occ_lb[bin]);
	}
	if (m_noisyM100[bin] != 0) {
	  m_NoisyModules100_vsLB->Fill(bin, m_noisyM100[bin]);
	}
	if (m_noisyM1000[bin] != 0) {
	  m_NoisyModules1000_vsLB->Fill(bin, m_noisyM1000[bin]);
	}
	if (m_noisyM10000[bin] != 0) {
	  m_NoisyModules10000_vsLB->Fill(bin, m_noisyM10000[bin]);
	}
	if (m_occTriggerBAR_lb[bin] != 0) {
	  m_BARNOTrigger_vsLB->Fill(bin, m_occTriggerBAR_lb[bin]);
	}
	if (m_occTriggerECm_lb[bin] != 0) {
	  m_ECmNOTrigger_vsLB->Fill(bin, m_occTriggerECm_lb[bin]);
	}
	if (m_occTriggerECp_lb[bin] != 0) {
	  m_ECpNOTrigger_vsLB->Fill(bin, m_occTriggerECp_lb[bin]);
	}
	if (m_occTrigger_lb[bin] != 0) {
	  m_SCTNOTrigger_vsLB->Fill(bin, m_occTrigger_lb[bin]);
	}
	if (m_noisyMTrigger100[bin] != 0) {
	  m_NoisyModulesTrigger100_vsLB->Fill(bin, m_noisyMTrigger100[bin]);
	}
	if (m_noisyMTrigger1000[bin] != 0) {
	  m_NoisyModulesTrigger1000_vsLB->Fill(bin, m_noisyMTrigger1000[bin]);
	}
	if (m_noisyMTrigger10000[bin] != 0) {
	  m_NoisyModulesTrigger10000_vsLB->Fill(bin, m_noisyMTrigger10000[bin]);
	}
      }
      // 09.09.2016
      for (int bin = 1; bin <= SCT_Monitoring::NBINS_LBs; bin++) {
	if (m_hitoccBAR_lb[bin] != 0) {
	  m_BARHO_vsLB->Fill(bin, m_hitoccBAR_lb[bin]);
	}
	if (m_hitoccECm_lb[bin] != 0) {
	  m_ECmHO_vsLB->Fill(bin, m_hitoccECm_lb[bin]);
	}
	if (m_hitoccECp_lb[bin] != 0) {
	  m_ECpHO_vsLB->Fill(bin, m_hitoccECp_lb[bin]);
	}
	if (m_hitocc_lb[bin] != 0) {
	  m_SCTHO_vsLB->Fill(bin, m_hitocc_lb[bin]);
	}
	if (m_noisyMWithHO100[bin] != 0) {
	  m_NoisyModulesWithHO100_vsLB->Fill(bin, m_noisyMWithHO100[bin]);
	}
	if (m_noisyMWithHO1000[bin] != 0) {
	  m_NoisyModulesWithHO1000_vsLB->Fill(bin, m_noisyMWithHO1000[bin]);
	}
	if (m_noisyMWithHO10000[bin] != 0) {
	  m_NoisyModulesWithHO10000_vsLB->Fill(bin, m_noisyMWithHO10000[bin]);
	}
	if (m_hitoccTriggerBAR_lb[bin] != 0) {
	  m_BARHOTrigger_vsLB->Fill(bin, m_hitoccTriggerBAR_lb[bin]);
	}
	if (m_hitoccTriggerECm_lb[bin] != 0) {
	  m_ECmHOTrigger_vsLB->Fill(bin, m_hitoccTriggerECm_lb[bin]);
	}
	if (m_hitoccTriggerECp_lb[bin] != 0) {
	  m_ECpHOTrigger_vsLB->Fill(bin, m_hitoccTriggerECp_lb[bin]);
	}
	if (m_hitoccTrigger_lb[bin] != 0) {
	  m_SCTHOTrigger_vsLB->Fill(bin, m_hitoccTrigger_lb[bin]);
	}
	if (m_noisyMWithHOTrigger100[bin] != 0) {
	  m_NoisyModulesWithHOTrigger100_vsLB->Fill(bin, m_noisyMWithHOTrigger100[bin]);
	}
	if (m_noisyMWithHOTrigger1000[bin] != 0) {
	  m_NoisyModulesWithHOTrigger1000_vsLB->Fill(bin, m_noisyMWithHOTrigger1000[bin]);
	}
	if (m_noisyMWithHOTrigger10000[bin] != 0) {
	  m_NoisyModulesWithHOTrigger10000_vsLB->Fill(bin, m_noisyMWithHOTrigger10000[bin]);
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: initializeNoiseMaps
// DD 12/06/2011
// Here we initialize the noise occupancy maps to avoid a bug that is present
// when adding TProfiles when no noise hit has occured in a module side
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::initializeNoiseMaps() {
  for (SCT_ID::const_id_iterator planeIterator =
         m_pSCTHelper->wafer_begin(); planeIterator != m_pSCTHelper->wafer_end();
       ++planeIterator) {
    // Don't initialize a value for disabled  modules
    if (!m_ConfigurationSvc->isGood(*planeIterator, InDetConditions::SCT_SIDE)) {
      continue;
    }
    m_occSumUnbiased[*planeIterator] = 0.;
    m_occSumUnbiasedTrigger[*planeIterator] = 0.;
    m_occSumUnbiasedRecent[*planeIterator] = 0.;
    m_occSumUnbiased_lb[*planeIterator] = 0.;
    m_occSumUnbiasedBAR_lb[*planeIterator] = 0.;
    m_occSumUnbiasedECp_lb[*planeIterator] = 0.;
    m_occSumUnbiasedECm_lb[*planeIterator] = 0.;
    m_occSumUnbiasedTrigger_lb[*planeIterator] = 0.;
    m_occSumUnbiasedTriggerBAR_lb[*planeIterator] = 0.;
    m_occSumUnbiasedTriggerECp_lb[*planeIterator] = 0.;
    m_occSumUnbiasedTriggerECm_lb[*planeIterator] = 0.;
    // 09.09.2016
    m_hitoccSumUnbiased[*planeIterator] = 0.;
    m_hitoccSumUnbiasedTrigger[*planeIterator] = 0.;
    m_hitoccSumUnbiasedRecent[*planeIterator] = 0.;
    m_hitoccSumUnbiased_lb[*planeIterator] = 0.;
    m_hitoccSumUnbiasedBAR_lb[*planeIterator] = 0.;
    m_hitoccSumUnbiasedECp_lb[*planeIterator] = 0.;
    m_hitoccSumUnbiasedECm_lb[*planeIterator] = 0.;
    m_hitoccSumUnbiasedTrigger_lb[*planeIterator] = 0.;
    m_hitoccSumUnbiasedTriggerBAR_lb[*planeIterator] = 0.;
    m_hitoccSumUnbiasedTriggerECp_lb[*planeIterator] = 0.;
    m_hitoccSumUnbiasedTriggerECm_lb[*planeIterator] = 0.;
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: resetNoiseMapHists
// DD 12/06/2011
// Resets entries for all the noise occupancy histograms
//
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::resetNoiseMapHists() {
  if (resetVecProf2(m_pnoiseoccupancymapHistoVector).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVector!" << endmsg;
  }
  if (resetVecProf2(m_pnoiseoccupancymapHistoVectorECp).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECp!" << endmsg;
  }
  if (resetVecProf2(m_pnoiseoccupancymapHistoVectorECm).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECm!" << endmsg;
  }
  if (m_environment == AthenaMonManager::online) {
    if (resetVecProf2(m_pnoiseoccupancymapHistoVectorRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorRecent!" << endmsg;
    }
    if (resetVecProf2(m_pnoiseoccupancymapHistoVectorECpRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECpRecent!" << endmsg;
    }
    if (resetVecProf2(m_pnoiseoccupancymapHistoVectorECmRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECmRecent!" << endmsg;
    }
  }
  if (resetVecProf2(m_pnoiseoccupancymapHistoVectorTrigger).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorTrigger!" << endmsg;
  }
  if (resetVecProf2(m_pnoiseoccupancymapHistoVectorECpTrigger).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECpTrigger!" << endmsg;
  }
  if (resetVecProf2(m_pnoiseoccupancymapHistoVectorECmTrigger).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset noiseoccupancymapHistoVectorECmTrigger!" << endmsg;
  }
  // 09.09.2016
  if (resetVecProf2(m_phitoccupancymapHistoVector).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVector!" << endmsg;
  }
  if (resetVecProf2(m_phitoccupancymapHistoVectorECp).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorECp!" << endmsg;
  }
  if (resetVecProf2(m_phitoccupancymapHistoVectorECm).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorECm!" << endmsg;
  }
  if (m_environment == AthenaMonManager::online) {
    if (resetVecProf2(m_phitoccupancymapHistoVectorRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorRecent!" << endmsg;
    }
    if (resetVecProf2(m_phitoccupancymapHistoVectorECpRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorECpRecent!" << endmsg;
    }
    if (resetVecProf2(m_phitoccupancymapHistoVectorECmRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorECmRecent!" << endmsg;
    }
  }
  if (resetVecProf2(m_phitoccupancymapHistoVectorTrigger).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorTrigger!" << endmsg;
  }
  if (resetVecProf2(m_phitoccupancymapHistoVectorECpTrigger).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorECpTrigger!" << endmsg;
  }
  if (resetVecProf2(m_phitoccupancymapHistoVectorECmTrigger).isFailure()) {
    msg(MSG::WARNING) << "Failed to reset hitoccupancymapHistoVectorECmTrigger!" << endmsg;
  }


  m_BARNO->Reset();
  m_BARNOTrigger->Reset();
  m_ECmNO->Reset();
  m_ECmNOTrigger->Reset();
  m_ECpNO->Reset();
  m_ECpNOTrigger->Reset();
  m_SCTNO->Reset();
  m_SCTNOTrigger->Reset();
  // 09.09.2016
  m_BARHO->Reset();
  m_BARHOTrigger->Reset();
  m_ECmHO->Reset();
  m_ECmHOTrigger->Reset();
  m_ECpHO->Reset();
  m_ECpHOTrigger->Reset();
  m_SCTHO->Reset();
  m_SCTHOTrigger->Reset();

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: resetNoiseMapsRecent
// DD 19/07/2011
// Resets all the values in the recent noise occ maps to zero
//
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::resetNoiseMapsRecent() {
  for (std::map<Identifier, float>::iterator it = m_occSumUnbiasedRecent.begin(); it != m_occSumUnbiasedRecent.end();
       ++it) {
    it->second = 0.0;
  }
  for (std::map<Identifier, float>::iterator it = m_hitoccSumUnbiasedRecent.begin();
       it != m_hitoccSumUnbiasedRecent.end(); ++it) {
    it->second = 0.0;
  }// 09.09.2016
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: resetHitMapHists
// DD 23/07/2011
// Resets all the values in the recent hit maps to zero
//
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::resetHitMapHists() {
  if (m_environment == AthenaMonManager::online) {
    if (resetVecH2(m_ptrackhitsHistoVectorRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset ptrackhitsHistoVectorRecent!" << endmsg;
    }
    if (resetVecH2(m_ptrackhitsHistoVectorRecentECp).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset ptrackhitsHistoVectorRecentECp!" << endmsg;
    }
    if (resetVecH2(m_ptrackhitsHistoVectorRecentECm).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset ptrackhitsHistoVectorRecentECm!" << endmsg;
    }
    if (resetVecH1(m_tbinHistoVectorRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_tbinHistoVectorRecent!" << endmsg;
    }
    if (resetVecH1(m_tbinHistoVectorRecentECp).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_tbinHistoVectorRecentECp!" << endmsg;
    }
    if (resetVecH1(m_tbinHistoVectorRecentECm).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_tbinHistoVectorRecentECm!" << endmsg;
    }
    if (resetVecH2(m_phitsHistoVectorRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_phitsHistoVectorRecent!" << endmsg;
    }
    if (resetVecH2(m_phitsHistoVectorRecentECp).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_phitsHistoVectorRecentECp!" << endmsg;
    }
    if (resetVecH2(m_phitsHistoVectorRecentECm).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_phitsHistoVectorRecentECm!" << endmsg;
    }
    if (resetVecH1(m_clusizeHistoVectorRecent).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_clusizeHistoVectorRecent!" << endmsg;
    }
    if (resetVecH1(m_clusizeHistoVectorRecentECp).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_clusizeHistoVectorRecentECp!" << endmsg;
    }
    if (resetVecH1(m_clusizeHistoVectorRecentECm).isFailure()) {
      msg(MSG::WARNING) << "Failed to reset m_clusizeHistoVectorRecentECm!" << endmsg;
    }
    m_tbinHistoRecent->Reset();
    m_tbinHistoRecentECp->Reset();
    m_tbinHistoRecentECm->Reset();
    m_clusizeRecent->Reset();
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: resetVecProf2
// DD 12/06/2011
// Resets entries for all histograms in a vector of TProfile2Ds
//
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::resetVecProf2(VecProf2_t hists) {
  for (unsigned int i = 0; i < hists.size(); ++i) {
    if (!hists[i]) {
      continue;
    }
    hists[i]->Reset();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: resetVecProf2
// DD 23/07/2011
// Resets entries for all histograms in a vector of TH2s
//
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::resetVecH2(VecH2_t hists) {
  for (unsigned int i = 0; i < hists.size(); ++i) {
    if (!hists[i]) {
      continue;
    }
    hists[i]->Reset();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                SCTHitsNoiseMonTool :: resetVecH1
// DD 23/07/2011
// Resets entries for all histograms in a vector of TH1s
//
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::resetVecH1(VecH1_t hists) {
  for (unsigned int i = 0; i < hists.size(); ++i) {
    if (!hists[i]) {
      continue;
    }
    hists[i]->Reset();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTHitsNoiseMonTool :: book1DNoiseHists
// Book noise occupancy histograms for all present SCT modules
// using ToolRootHistSvc()
// The histograms go to the subdirectory ../Modulemaps/
// to improve readability
//
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::bookNoiseDistributions() {
  // SCTHitsNoiseMonTool::bookNoiseDistributions(bool isNewRun){

  //  if(isNewRun){
  if (newRunFlag()) {
    MonGroup NoiseDistributions(this, "SCT/GENERAL/noise", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    const Int_t bins = 8000;
    Double_t xmin = 1e-1;
    Double_t xmax = 20000;
    Double_t logxmin = log10(xmin);
    Double_t logxmax = log10(xmax);
    Double_t binwidth = (logxmax - logxmin) / bins;
    Double_t xbins[bins + 1];
    xbins[0] = xmin;
    if (m_doLogXNoise) {
      for (Int_t i = 1; i <= bins; ++i) {
        xbins[i] = xmin + TMath::Power(10, logxmin + i * binwidth);
      }
    }

    m_BARNO = new TH1F("barrelNOdistribution", "NO Distribution for the Barrel", bins, xmin, xmax);
    m_BARNO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_BARNO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_BARNO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_BARNO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book BARNO" << endmsg;
    }

    std::string barrelNO_title = "NO Distribution for the Barrel for " + m_NOTrigger + " trigger";
    m_BARNOTrigger = new TH1F("barrelNOdistributionTrigger", TString(barrelNO_title), bins, xmin, xmax);
    m_BARNOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_BARNOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_BARNOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_BARNOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book BARNOTrigger" << endmsg;
    }

    m_ECmNO = new TH1F("ECCNOdistribution", "NO Distribution for the EndCap C", bins, xmin, xmax);
    m_ECmNO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECmNO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECmNO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECmNO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECmNO" << endmsg;
    }

    std::string ECmNO_title = "NO Distribution for the EndCap C for " + m_NOTrigger + " trigger";
    m_ECmNOTrigger = new TH1F("ECCNOdistributionTrigger", TString(ECmNO_title), bins, xmin, xmax);
    m_ECmNOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECmNOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECmNOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECmNOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECmNOTrigger" << endmsg;
    }

    m_ECpNO = new TH1F("ECANOdistribution", "NO Distribution for the EndCap A", bins, xmin, xmax);
    m_ECpNO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECpNO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECpNO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECpNO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECpNO" << endmsg;
    }

    std::string ECpNO_title = "NO Distribution for the EndCap A for " + m_NOTrigger + " trigger";
    m_ECpNOTrigger = new TH1F("ECANOdistributionTrigger", TString(ECpNO_title), bins, xmin, xmax);
    m_ECpNOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_ECpNOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECpNOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECpNOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECpNOTrigger" << endmsg;
    }

    m_SCTNO = new TH1F("SCTNOdistribution", "NO Distribution for the SCT", bins, xmin, xmax);
    m_SCTNO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_SCTNO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_SCTNO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_SCTNO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NO" << endmsg;
    }

    std::string NO_title = "NO Distribution for the SCT for " + m_NOTrigger + " trigger";
    m_SCTNOTrigger = new TH1F("SCTNOdistributionTrigger", TString(NO_title), bins, xmin, xmax);
    m_SCTNOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    m_SCTNOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_SCTNOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_SCTNOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NOTrigger" << endmsg;
    }

    m_SCTNO_vsLB = TProfile_LW::create("NO_vsLB", "NO vs LB for all region (SP noise)", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_SCTNO_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_SCTNO_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    if (NoiseDistributions.regHist(m_SCTNO_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book SCTNO_vsLB" << endmsg;
    }

    m_NoisyModulesTrigger100_vsLB = TProfile_LW::create("NoisyModulesTrigger100_vsLB",
                                                 "NoisyModules100 with trigger vs LB for all region (SP noise)", SCT_Monitoring::NBINS_LBs,
                                                 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesTrigger100_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesTrigger100_vsLB->GetYaxis()->SetTitle("Noisy Modules");
    if (NoiseDistributions.regHist(m_NoisyModulesTrigger100_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesTrigger100_vsLB" << endmsg;
    }

    m_NoisyModulesTrigger1000_vsLB = TProfile_LW::create("NoisyModulesTrigger1000_vsLB",
                                                  "NoisyModules1000 with trigger vs LB for all region (SP noise)",
                                                  SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesTrigger1000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesTrigger1000_vsLB->GetYaxis()->SetTitle("Noisy Modules");
    if (NoiseDistributions.regHist(m_NoisyModulesTrigger1000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesTrigger1000_vsLB" << endmsg;
    }

    m_NoisyModulesTrigger10000_vsLB = TProfile_LW::create("NoisyModulesTrigger10000_vsLB",
                                                   "NoisyModules10000 with trigger vs LB for all region (SP noise)",
                                                   SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesTrigger10000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesTrigger10000_vsLB->GetYaxis()->SetTitle("Noisy Modules");
    if (NoiseDistributions.regHist(m_NoisyModulesTrigger10000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesTrigger10000_vsLB" << endmsg;
    }

    m_SCTNOTrigger_vsLB = TProfile_LW::create("NOTrigger_vsLB", "NO with trigger vs LB for all region (SP noise)", SCT_Monitoring::NBINS_LBs,
                                       0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_SCTNOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_SCTNOTrigger_vsLB->GetYaxis()->SetTitle("Noise Occupancy [10^{-5}]");
    if (NoiseDistributions.regHist(m_SCTNOTrigger_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book SCTNOTrigger_vsLB" << endmsg;
    }

    m_NoisyModules100_vsLB = TProfile_LW::create("NoisyModules100_vsLB", "NoisyModules100 vs LB for all region (SP noise)",
                                          SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModules100_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModules100_vsLB->GetYaxis()->SetTitle("Noisy Modules");
    if (NoiseDistributions.regHist(m_NoisyModules100_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModules100_vsLB" << endmsg;
    }

    m_NoisyModules1000_vsLB = TProfile_LW::create("NoisyModules1000_vsLB", "NoisyModules1000 vs LB for all region (SP noise)",
                                           SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModules1000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModules1000_vsLB->GetYaxis()->SetTitle("Noisy Modules");
    if (NoiseDistributions.regHist(m_NoisyModules1000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModules1000_vsLB" << endmsg;
    }

    m_NoisyModules10000_vsLB = TProfile_LW::create("NoisyModules10000_vsLB", "NoisyModules10000 vs LB for all region (SP noise)",
                                            SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModules10000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModules10000_vsLB->GetYaxis()->SetTitle("Noisy Modules");
    if (NoiseDistributions.regHist(m_NoisyModules10000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModules10000_vsLB" << endmsg;
    }
    // 09.09.2016
    m_BARHO = new TH1F("barrelHOdistribution", "HO Distribution for the Barrel", bins, xmin, xmax);
    m_BARHO->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_BARHO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_BARHO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_BARHO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book BARHO" << endmsg;
    }

    std::string barrelHO_title = "HO Distribution for the Barrel for " + m_NOTrigger + " trigger";
    m_BARHOTrigger = new TH1F("barrelHOdistributionTrigger", TString(barrelHO_title), bins, xmin, xmax);
    m_BARHOTrigger->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_BARHOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_BARHOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_BARHOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book BARHOTrigger" << endmsg;
    }
    m_ECmHO = new TH1F("ECCHOdistribution", "HO Distribution for the EndCap C", bins, xmin, xmax);

    m_ECmHO->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_ECmHO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECmHO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECmHO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECmHO" << endmsg;
    }

    std::string ECmHO_title = "HO Distribution for the EndCap C for " + m_NOTrigger + " trigger";
    m_ECmHOTrigger = new TH1F("ECCHOdistributionTrigger", TString(ECmHO_title), bins, xmin, xmax);
    m_ECmHOTrigger->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_ECmHOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECmHOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECmHOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECmHOTrigger" << endmsg;
    }

    m_ECpHO = new TH1F("ECAHOdistribution", "HO Distribution for the EndCap A", bins, xmin, xmax);
    m_ECpHO->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_ECpHO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECpHO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECpHO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECpHO" << endmsg;
    }

    std::string ECpHO_title = "HO Distribution for the EndCap A for " + m_NOTrigger + " trigger";
    m_ECpHOTrigger = new TH1F("ECAHOdistributionTrigger", TString(ECpHO_title), bins, xmin, xmax);
    m_ECpHOTrigger->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_ECpHOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_ECpHOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_ECpHOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book ECpHOTrigger" << endmsg;
    }

    m_SCTHO = new TH1F("SCTHOdistribution", "HO Distribution for the SCT", bins, xmin, xmax);
    m_SCTHO->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_SCTHO->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_SCTHO->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_SCTHO).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book HO" << endmsg;
    }

    std::string HO_title = "HO Distribution for the SCT for " + m_NOTrigger + " trigger";
    m_SCTHOTrigger = new TH1F("SCTHOdistributionTrigger", TString(HO_title), bins, xmin, xmax);
    m_SCTHOTrigger->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    m_SCTHOTrigger->GetYaxis()->SetTitle("Modules");
    if (m_doLogXNoise) {
      m_SCTHOTrigger->SetBins(bins, xbins);
    }
    if (NoiseDistributions.regHist(m_SCTHOTrigger).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book HOTrigger" << endmsg;
    }

    m_SCTHO_vsLB = TProfile_LW::create("HO_vsLB", "HO vs LB for all region (SP noise)", SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_SCTHO_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_SCTHO_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    if (NoiseDistributions.regHist(m_SCTHO_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book SCTHO_vsLB" << endmsg;
    }

    m_NoisyModulesWithHOTrigger100_vsLB = TProfile_LW::create("NoisyModulesWithHOTrigger100_vsLB",
                                                       "NoisyModules100 with HO with trigger vs LB for all region with threshold 100", SCT_Monitoring::NBINS_LBs, 0.5,
                                                       SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesWithHOTrigger100_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesWithHOTrigger100_vsLB->GetYaxis()->SetTitle("Noisy Modules with HO");
    if (NoiseDistributions.regHist(m_NoisyModulesWithHOTrigger100_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesWithHOTrigger100_vsLB" << endmsg;
    }

    m_NoisyModulesWithHOTrigger1000_vsLB = TProfile_LW::create("NoisyModulesWithHOTrigger1000_vsLB",
                                                        "NoisyModules1000 with HO with trigger vs LB for all region with threshold 1000", SCT_Monitoring::NBINS_LBs, 0.5,
                                                        SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesWithHOTrigger1000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesWithHOTrigger1000_vsLB->GetYaxis()->SetTitle("Noisy Modules with HO");
    if (NoiseDistributions.regHist(m_NoisyModulesWithHOTrigger1000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesWithHOTrigger1000_vsLB" << endmsg;
    }

    m_NoisyModulesWithHOTrigger10000_vsLB = TProfile_LW::create("NoisyModulesWithHOTrigger10000_vsLB",
                                                         "NoisyModules10000 with HO with trigger vs LB for all region with threshold 10000", SCT_Monitoring::NBINS_LBs, 0.5,
                                                         SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesWithHOTrigger10000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesWithHOTrigger10000_vsLB->GetYaxis()->SetTitle("Noisy Modules with HO");
    if (NoiseDistributions.regHist(m_NoisyModulesWithHOTrigger10000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesWithHOTrigger10000_vsLB" << endmsg;
    }

    m_SCTHOTrigger_vsLB = TProfile_LW::create("HOTrigger_vsLB", "HO with trigger vs LB for all region (SP noise)", SCT_Monitoring::NBINS_LBs,
                                       0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_SCTHOTrigger_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_SCTHOTrigger_vsLB->GetYaxis()->SetTitle("Hit Occupancy [10^{-5}]");
    if (NoiseDistributions.regHist(m_SCTHOTrigger_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book SCTHOTrigger_vsLB" << endmsg;
    }

    m_NoisyModulesWithHO100_vsLB = TProfile_LW::create("NoisyModulesWithHO100_vsLB",
                                                "NoisyModulesWithHO vs LB for all region with threshold 100", SCT_Monitoring::NBINS_LBs,
                                                0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesWithHO100_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesWithHO100_vsLB->GetYaxis()->SetTitle("Noisy Modules with HO");
    if (NoiseDistributions.regHist(m_NoisyModulesWithHO100_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesWithHO100_vsLB" << endmsg;
    }

    m_NoisyModulesWithHO1000_vsLB = TProfile_LW::create("NoisyModulesWithHO1000_vsLB",
                                                 "NoisyModulesWithHO vs LB for all region with threshold 1000",
                                                 SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesWithHO1000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesWithHO1000_vsLB->GetYaxis()->SetTitle("Noisy Modules with HO");
    if (NoiseDistributions.regHist(m_NoisyModulesWithHO1000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesWithHO1000_vsLB" << endmsg;
    }

    m_NoisyModulesWithHO10000_vsLB = TProfile_LW::create("NoisyModulesWithHO10000_vsLB",
                                                  "NoisyModulesWithHO vs LB for all region with threshold 10000",
                                                  SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
    m_NoisyModulesWithHO10000_vsLB->GetXaxis()->SetTitle("LumiBlock");
    m_NoisyModulesWithHO10000_vsLB->GetYaxis()->SetTitle("Noisy Modules with HO");
    if (NoiseDistributions.regHist(m_NoisyModulesWithHO10000_vsLB).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book NoisyModulesWithHO10000_vsLB" << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                      SCTHitsNoiseMonTool :: bookSPvsEventNumber
// Book 1D histogram of number of spacepoints vs event number
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::bookSPvsEventNumber() {
  if (newRunFlag()) {
    free(m_nSP_buf);
    free(m_nHits_buf);
    free(m_nmaxHits_buf);
    free(m_nminHits_buf);
    free(m_nmaxModule_buf);
    free(m_nminModule_buf);
    MonGroup BarrelSPHist(this, "SCT/GENERAL/hits", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    // Book a histogram
    m_nSP = th1Factory("sct_sp_vs_en", "Number of Spacepoints vs Event Number", BarrelSPHist, 1, m_evtsbins + 1,
                       m_evtsbins);
    m_nSP->GetXaxis()->SetTitle("Event Number");
    m_nSP->GetYaxis()->SetTitle("Num of Spacepoints");
    size_t nSP_buf_size;
    nSP_buf_size = m_evtsbins * sizeof(int);
    m_nSP_buf = (int *) malloc(nSP_buf_size);
    m_nSP_pos = 0;

    m_nHits = th1Factory("sct_av_hits_vs_en", "Number of Average Hits vs Event Number", BarrelSPHist, 1, m_evtsbins + 1,
                         m_evtsbins);
    m_nHits->GetXaxis()->SetTitle("Event Number");
    m_nHits->GetYaxis()->SetTitle("Num of Average Hits");
    size_t nHits_buf_size;
    nHits_buf_size = m_evtsbins * sizeof(int);
    m_nHits_buf = (int *) malloc(nHits_buf_size);
    m_nHits_pos = 0;

    m_nmaxHits = th1Factory("sct_max_hits_vs_en", "Max Number of Hits vs Event Number", BarrelSPHist, 1, m_evtsbins + 1,
                            m_evtsbins);
    m_nmaxHits->GetXaxis()->SetTitle("Event Number");
    m_nmaxHits->GetYaxis()->SetTitle("Num of Max Hits");
    size_t nmaxHits_buf_size;
    nmaxHits_buf_size = m_evtsbins * sizeof(int);
    m_nmaxHits_buf = (int *) malloc(nmaxHits_buf_size);
    size_t nmaxModule_buf_size;
    nmaxModule_buf_size = m_evtsbins * sizeof(Identifier);
    m_nmaxModule_buf = (Identifier *) malloc(nmaxModule_buf_size);

    m_nminHits = th1Factory("sct_min_hits_vs_en", "Min Number of Hits vs Event Number", BarrelSPHist, 1, m_evtsbins + 1,
                            m_evtsbins);
    m_nminHits->GetXaxis()->SetTitle("Event Number");
    m_nminHits->GetYaxis()->SetTitle("Num of Min Hits");
    size_t nminHits_buf_size;
    nminHits_buf_size = m_evtsbins * sizeof(int);
    m_nminHits_buf = (int *) malloc(nminHits_buf_size);
    size_t nminModule_buf_size;
    nminModule_buf_size = m_evtsbins * sizeof(Identifier);
    m_nminModule_buf = (Identifier *) malloc(nminModule_buf_size);
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                         SCTHitsNoiseMonTool :: makeSPvsEventNumber
// ====================================================================================================
StatusCode
SCTHitsNoiseMonTool::makeSPvsEventNumber() {
  // Retrieve the spacepoint collection
  SG::ReadHandle<SpacePointContainer> SCT_spcontainer(m_SCTSPContainerName);

  // get space points for SCT from TDS
  if (not SCT_spcontainer.isValid()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Si SpacePoint container for SCT not found" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  int sct_nspacepoints(0);
  // loop over SCT space points collections
  SpacePointContainer::const_iterator it = SCT_spcontainer->begin();
  SpacePointContainer::const_iterator endit = SCT_spcontainer->end();
  for (; it != endit; ++it) {
    const SpacePointCollection *colNext = &(**it);
    if (!colNext) {
      continue;
    }
    // Identify the spacepoint collection and gets its barrel_ec to update the num SP histogram
    int thisBec = m_pSCTHelper->barrel_ec(colNext->identify());
    int thisLayerDisk = m_pSCTHelper->layer_disk(colNext->identify());
    if (thisBec == 0) {
      m_numBarrelSPPerLumiBlock->Fill(thisLayerDisk, colNext->size());
    }
    if (thisBec == -2) {
      m_numSPPerLumiBlockECm->Fill(thisLayerDisk, colNext->size());
    }
    if (thisBec == 2) {
      m_numSPPerLumiBlockECp->Fill(thisLayerDisk, colNext->size());
    }
    sct_nspacepoints += (int) colNext->size();
  }

  if (m_environment == AthenaMonManager::online) {
    // Time Dependent SP plots only online
    m_nSP_buf[m_nSP_pos] = sct_nspacepoints;
    m_nSP_pos++;
    if (m_nSP_pos == m_evtsbins) {
      m_nSP_pos = 0;
    }
    if (m_numberOfEvents % m_checkrate == 0) {
      m_nSP->Reset();
      Int_t latest_nSP_pos = m_nSP_pos;
      for (Int_t i = 1; i < m_evtsbins; i++) {
        if (latest_nSP_pos == m_evtsbins) {
          latest_nSP_pos = 0;
        }
        if (m_numberOfEvents < m_evtsbins) {
          if (i < m_nSP_pos) {
            m_nSP->SetBinContent(i, m_nSP_buf[i]);
          } else {
            m_nSP->SetBinContent(i, 0);
          }
        } else {
          m_nSP->SetBinContent(i, m_nSP_buf[latest_nSP_pos]);
          m_nSP->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
        }
        latest_nSP_pos++;
        if (latest_nSP_pos == m_evtsbins) {
          latest_nSP_pos = 0;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

std::string
SCTHitsNoiseMonTool::positionString(const Identifier &plane) const {
  ostringstream position_txt;

  position_txt << m_pSCTHelper->barrel_ec(plane) << "_" << m_pSCTHelper->layer_disk(plane) << "_" <<
    m_pSCTHelper->phi_module(plane) << "_" << m_pSCTHelper->eta_module(plane) << "_" << m_pSCTHelper->side(plane);
  return position_txt.str();
}

StatusCode
SCTHitsNoiseMonTool::makeVectorOfTrackRDOIdentifiers() {
  // Clear the RDOsOnTracks vector
  m_RDOsOnTracks.clear();
  SG::ReadHandle<SCT_RDO_Container> p_rdocontainer(m_dataObjectName);
  if (not p_rdocontainer.isValid()) {
    msg(MSG::FATAL) << "Could not find the data object " << m_dataObjectName.key() << " !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Data object " << m_dataObjectName.key() << " found" << endmsg;
    }
  }
  
  SG::ReadHandle<TrackCollection> tracks(m_tracksName);
  if (not tracks.isValid()) {
    msg(MSG::FATAL) << "No tracks for you!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Only do for events with less than some number of tracks
  if (tracks->size() > m_maxTracks) {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "The event has more than " << m_maxTracks
                      << " tracks. Don't do hits-on-track-hists" << endmsg;
    }
    return StatusCode::SUCCESS;
  }
  // assemble list of rdo ids associated with tracks
  for (int i = 0; i < (int) tracks->size(); i++) {
    const Trk::Track *track = (*tracks)[i];
    if (track == 0) {
      if (msgLvl(MSG::WARNING)) {
        msg(MSG::WARNING) << "no pointer to track!!!" << endmsg;
      }
      break;
    }
    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface> *trackStates = track->trackStateOnSurfaces();
    if (trackStates == 0) {
      if (msgLvl(MSG::WARNING)) {
        msg(MSG::WARNING) <<
	  "for current track is TrackStateOnSurfaces == Null, no data will be written for this track" << endmsg;
      }
    }else {// Loop over all track states on surfaces
      for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it = trackStates->begin();
           it != trackStates->end(); it++) {
        // Get pointer to RIO of right type
        const InDet::SiClusterOnTrack *clus =
          dynamic_cast<const InDet::SiClusterOnTrack *>((*it)->measurementOnTrack());
        if (clus) {
          // Get Pointer to prepRawDataObject
          const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster *>(clus->prepRawData());
          if (RawDataClus == 0) {
            if (msgLvl(MSG::WARNING)) {
              msg(MSG::WARNING) << "SiCluster WITHOUT prepRawData!!!!" << endmsg;
            }
            break;
          }
          // if Cluster is in SCT ...
          if (RawDataClus->detectorElement()->isSCT()) {
            const std::vector<Identifier> &rdoList = RawDataClus->rdoList();
            m_RDOsOnTracks.insert(m_RDOsOnTracks.end(), rdoList.begin(), rdoList.end());
          }
        }
      }
    }
  }
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "JEGN!! num RDOs on tracks is " << m_RDOsOnTracks.size() << endmsg;
  }
  return StatusCode::SUCCESS;
}

// StatusCode SCTHitsNoiseMonTool::bookGeneralTrackHits(bool isNewRun, const unsigned int systemIndex){
StatusCode
SCTHitsNoiseMonTool::bookGeneralTrackHits(const unsigned int systemIndex) {
  const SCT_Monitoring::Bec bec(index2Bec(systemIndex));

  if (bec == INVALID_SYSTEM) {
    msg(MSG::FATAL) << "Invalid subsystem index, should be 0-2, was " << systemIndex << endmsg;
    return StatusCode::FAILURE;
  }
  const string paths[] = {
    "SCT/SCTEC/hits/", "SCT/SCTB/hits/", "SCT/SCTEA/hits/"
  };
  const unsigned int limits[] = {
    N_DISKSx2, N_BARRELSx2, N_DISKSx2
  };
  VecH2_t *histoVec[] = {
    &m_ptrackhitsHistoVectorECm, &m_ptrackhitsHistoVector, &m_ptrackhitsHistoVectorECp
  };
  VecH2_t *histoVecRecent[] = {
    &m_ptrackhitsHistoVectorRecentECm, &m_ptrackhitsHistoVectorRecent, &m_ptrackhitsHistoVectorRecentECp
  };
  string stem = m_stream + "/" + paths[systemIndex] + "mapsOfHitsOnTracks";
  MonGroup tracksMon(this, paths[systemIndex] + "mapsOfHitsOnTracks", run, ATTRIB_UNMANAGED);
  if (newRunFlag()) {
    (histoVec[systemIndex])->clear();
    (histoVecRecent[systemIndex])->clear();
    // book Hitmaps and hits per layer histograms
    for (unsigned int i(0); i != limits[systemIndex]; ++i) {
      LayerSideFormatter layerSide(i, systemIndex);
      const string streamhitmap = "mapsOfHitsOnTracks" + abbreviations[systemIndex] + streamDelimiter +
	"trackhitsmap_" + layerSide.name();
      const string streamhitmaprecent = "mapsOfHitsOnTracksRecent" + abbreviations[systemIndex] + streamDelimiter +
	"trackhitsmap_" + layerSide.name();
      std::string histoName = stem + streamhitmap;
      std::string histotitle = "SCT hits on tracks for " + names[systemIndex] + " " + layerSide.title();
      std::string histotitlerecent = "SCT hits on tracks from recent events for " + names[systemIndex] + " " +
	layerSide.title();
      h2Factory(streamhitmap, histotitle, bec, tracksMon, *(histoVec[systemIndex]));
      if (m_environment == AthenaMonManager::online) {
        h2Factory(streamhitmaprecent, histotitlerecent, bec, tracksMon, *(histoVecRecent[systemIndex]));
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
SCTHitsNoiseMonTool::bookGeneralTrackTimeHistos(const unsigned int systemIndex) {
  if (newRunFlag()) {
    const string path[] = {
      "SCT/SCTEC/tbin/tracks/", "SCT/SCTB/tbin/tracks/", "SCT/SCTEA/tbin/tracks/"
    };
    const unsigned int limits[N_REGIONS] = {
      N_DISKS, N_BARRELS, N_DISKS
    };
    std::vector<H1_t> *tbinHistoVectorArray[] = {
      &m_tbinHistoVectorECm, &m_tbinHistoVector, &m_tbinHistoVectorECp
    };
    std::vector<H1_t> *tbinHistoVectorArrayRecent[] = {
      &m_tbinHistoVectorRecentECm, &m_tbinHistoVectorRecent, &m_tbinHistoVectorRecentECp
    };
    //
    std::vector<H1_t> &tbinHistoVector = *(tbinHistoVectorArray[systemIndex]);
    std::vector<H1_t> &tbinHistoVectorRecent = *(tbinHistoVectorArrayRecent[systemIndex]);
    tbinHistoVector.clear();
    tbinHistoVectorRecent.clear();
    MonGroup timeGroup(this, path[systemIndex], run, ATTRIB_UNMANAGED);
    MonGroup tbinGroup(this, "SCT/GENERAL/tbin", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    std::string stem = m_stream + pathDelimiter + path[systemIndex];
    const unsigned int nBins = 8;
    std::string tbinsNames[] = {
      "000", "001", "010", "011", "100", "101", "110", "111"
    };
    std::string histoName = "TrackTimeBin" + abbreviations[systemIndex];
    std::string histoTitle = "RDO Track TimeBin for " + names[systemIndex];
    std::string histoNameRecent = "TrackTimeBinRecent" + abbreviations[systemIndex];
    std::string histoTitleRecent = "RDO Track TimeBin from recent events for " + names[systemIndex];
    m_tbinfracall = profFactory("TBinFracAll", "fraction of 01X for each region", tbinGroup);
    switch (systemIndex) {
    case 0: {
      m_tbinHistoECm = h1Factory(histoName, histoTitle, timeGroup, -0.5, 7.5, nBins);
      std::string disksidenameECm[] = {
        "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1", "4_0", "4_1", "5_0", "5_1", "6_0", "6_1", "7_0", "7_1",
        "8_0", "8_1"
      };
      for (int i = 0; i < 18; i++) {
        std::string nameECmfrac = "TBinFracEC_" + disksidenameECm[i];
        std::string titleECmfrac = "fraction of 01X in EndcapC" + disksidenameECm[i];
        m_tbinfracECm[i] = prof2DFactory(nameECmfrac, titleECmfrac, timeGroup, 3, -0.5, 2.5, 52, -0.5, 51.5);
      }
      m_tbinfracVsLBECm = profFactory("TBinFrac01XVsLBEC", "fraction of 01X vs LumiBlock in EndcapC", timeGroup, 2000,
                                      0, 2000);
      for (unsigned int bin(0); bin < nBins; bin++) {
        m_tbinHistoECm->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
      }
      m_tbinHistoECm->GetXaxis()->SetTitle("TimeBin");
      if (m_environment == AthenaMonManager::online) {
        m_tbinHistoRecentECm = h1Factory(histoNameRecent, histoTitleRecent, timeGroup, -0.5, 7.5, nBins);
        for (unsigned int bin(0); bin < nBins; bin++) {
          m_tbinHistoRecentECm->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
        }
        m_tbinHistoRecentECm->GetXaxis()->SetTitle("TimeBin");
      }
      break;
    }

    case 1: {
      m_tbinHisto = h1Factory(histoName, histoTitle, timeGroup, -0.5, 7.5, nBins);
      std::string layersidenameB[] = {
        "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1"
      };
      for (int i = 0; i < 8; i++) {
        std::string nameBfrac = "TBinFrac_" + layersidenameB[i];
        std::string titleBfrac = "fraction of 01X in Barrel" + layersidenameB[i];
        m_tbinfrac[i] = prof2DFactory(nameBfrac, titleBfrac, timeGroup, 13, -6.5, 6.5, 56, -0.5, 55.5);
      }
      m_tbinfracVsLB =
        profFactory("TBinFrac01XVsLB", "fraction of 01X vs LumiBlock in Barrel", timeGroup, 2000, 0, 2000);
      for (unsigned int bin(0); bin < nBins; bin++) {
        m_tbinHisto->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
      }
      m_tbinHisto->GetXaxis()->SetTitle("TimeBin");
      if (m_environment == AthenaMonManager::online) {
        m_tbinHistoRecent = h1Factory(histoNameRecent, histoTitleRecent, timeGroup, -0.5, 7.5, nBins);
        for (unsigned int bin(0); bin < nBins; bin++) {
          m_tbinHistoRecent->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
        }
        m_tbinHistoRecent->GetXaxis()->SetTitle("TimeBin");
      }
      break;
    }

    case 2: {
      m_tbinHistoECp = h1Factory(histoName, histoTitle, timeGroup, -0.5, 7.5, nBins);
      std::string disksidenameECp[] = {
        "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1", "4_0", "4_1", "5_0", "5_1", "6_0", "6_1", "7_0", "7_1",
        "8_0", "8_1"
      };
      for (int i = 0; i < 18; i++) {
        std::string nameECpfrac = "TBinFracEA_" + disksidenameECp[i];
        std::string titleECpfrac = "fraction of 01X in EndcapA" + disksidenameECp[i];
        m_tbinfracECp[i] = prof2DFactory(nameECpfrac, titleECpfrac, timeGroup, 3, -0.5, 2.5, 52, -0.5, 51.5);
      }
      m_tbinfracVsLBECp = profFactory("TBinFrac01XVsLBEA", "fraction of 01X vs LumiBlock in EndcapA", timeGroup, 2000,
                                      0, 2000);
      for (unsigned int bin(0); bin < nBins; bin++) {
        m_tbinHistoECp->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
      }
      m_tbinHistoECp->GetXaxis()->SetTitle("TimeBin");
      if (m_environment == AthenaMonManager::online) {
        m_tbinHistoRecentECp = h1Factory(histoNameRecent, histoTitleRecent, timeGroup, -0.5, 7.5, nBins);
        for (unsigned int bin(0); bin < nBins; bin++) {
          m_tbinHistoRecentECp->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
        }
        m_tbinHistoRecentECp->GetXaxis()->SetTitle("TimeBin");
      }
      break;
    }
    }
    for (unsigned int i(0); i != limits[systemIndex]; ++i) {
      const string streamlayer(std::to_string(i));
      const string streamhitmap = "TrackTimeBin" + abbreviations[systemIndex] + streamDelimiter + streamlayer;
      const string streamhitmaprecent = "TrackTimeBinRecent" + abbreviations[systemIndex] + streamDelimiter +
	streamlayer;
      histoName = stem + streamhitmap;
      histoNameRecent = stem + streamhitmaprecent;
      histoTitle = "RDO Track TimeBin: layer " + streamlayer;
      histoTitleRecent = "RDO Track TimeBin form recent events: layer " + streamlayer;
      if (systemIndex != 1) {
        histoTitle = "RDO Track TimeBin: disk " + streamlayer;
        histoTitleRecent = "RDO Track TimeBin form recent events: disk " + streamlayer;
      }
      h1Factory(streamhitmap, histoTitle, timeGroup, *(tbinHistoVectorArray[systemIndex]), -0.5, 7.5, nBins);
      for (unsigned int bin(0); bin < nBins; bin++) {
        tbinHistoVector[i]->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
      }
      tbinHistoVector[i]->GetXaxis()->SetTitle("TimeBin");
      if (m_environment == AthenaMonManager::online) {
        h1Factory(streamhitmaprecent, histoTitleRecent, timeGroup, *(tbinHistoVectorArrayRecent[systemIndex]), -0.5,
                   7.5, nBins);
        for (unsigned int bin(0); bin < nBins; bin++) {
          tbinHistoVectorRecent[i]->GetXaxis()->SetBinLabel(bin + 1, tbinsNames[bin].c_str());
        }
        tbinHistoVectorRecent[i]->GetXaxis()->SetTitle("TimeBin");
      }
    }
  }
  return StatusCode::SUCCESS;
}

SCTHitsNoiseMonTool::H2_t
SCTHitsNoiseMonTool::h2Factory(const std::string &name, const std::string &title, const SCT_Monitoring::Bec bec,
                               MonGroup &registry, VecH2_t &storageVector) {
  int firstEta(FIRST_ETA_BIN), lastEta(LAST_ETA_BIN), firstPhi(FIRST_PHI_BIN), lastPhi(LAST_PHI_BIN), nEta(N_ETA_BINS),
    nPhi(N_PHI_BINS);

  if (bec != BARREL) {
    firstEta = FIRST_ETA_BIN_EC;
    lastEta = LAST_ETA_BIN_EC;
    firstPhi = FIRST_PHI_BIN_EC;
    lastPhi = LAST_PHI_BIN_EC;
    nEta = N_ETA_BINS_EC;
    nPhi = N_PHI_BINS_EC;
  }
  H2_t tmp = TH2F_LW::create(TString(name), TString(
					     title), nEta, firstEta - 0.5, lastEta + 0.5, nPhi, firstPhi - 0.5, lastPhi + 0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  storageVector.push_back(tmp);
  return success ? tmp : NULL;
}

SCTHitsNoiseMonTool::H2I_t
SCTHitsNoiseMonTool::h2IFactory(const std::string &name, const std::string &title, MonGroup &registry, int nbinx,
                                float xlo, float xhi, int nbiny, float ylo, float yhi) {
  H2I_t tmp = TH2I_LW::create(TString(name), TString(title), nbinx, xlo, xhi, nbiny, ylo, yhi);

  tmp->SetXTitle("module #");
  tmp->SetYTitle("Time bin");
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  return success ? tmp : NULL;
}

SCTHitsNoiseMonTool::Prof2_t
SCTHitsNoiseMonTool::prof2DFactory(const std::string &name, const std::string &title, MonGroup &registry, int nbinx,
                                   int xlo, int xhi, int nbiny, int ylo, int yhi) {
  Prof2_t tmp = new TProfile2D(TString(name), TString(title), nbinx, xlo - 0.5, xhi + 0.5, nbiny, ylo - 0.5, yhi + 0.5);

  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  return success ? tmp : NULL;
}

SCTHitsNoiseMonTool::Prof_t
SCTHitsNoiseMonTool::profFactory(const std::string &name, const std::string &title, MonGroup &registry, int nbin,
                                 int lo, int hi) {
  Prof_t tmp = TProfile_LW::create(TString(name), TString(title), nbin, lo, hi);

  tmp->SetXTitle("LumiBlock");
  tmp->SetYTitle("Fraction of 01X");
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  return success ? tmp : NULL;
}

SCTHitsNoiseMonTool::Prof_t
SCTHitsNoiseMonTool::profFactory(const std::string &name, const std::string &title, MonGroup &registry) {
  Prof_t tmp = TProfile_LW::create(TString(name), TString(title), 3, 0, 3);

  tmp->SetYTitle("Fraction of 01X");
  tmp->GetXaxis()->SetBinLabel(1.5, "Endcap C");
  tmp->GetXaxis()->SetBinLabel(2.5, "Barrel");
  tmp->GetXaxis()->SetBinLabel(3.5, "Endcap A");
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  return success ? tmp : NULL;
}

SCTHitsNoiseMonTool::Prof2_t
SCTHitsNoiseMonTool::prof2Factory(const std::string &name, const std::string &title, const SCT_Monitoring::Bec bec,
                                  MonGroup &registry, VecProf2_t &storageVector) {
  int firstEta(FIRST_ETA_BIN), lastEta(LAST_ETA_BIN), firstPhi(FIRST_PHI_BIN), lastPhi(LAST_PHI_BIN), nEta(N_ETA_BINS),
    nPhi(N_PHI_BINS);

  if (bec != BARREL) {
    firstEta = FIRST_ETA_BIN_EC;
    lastEta = LAST_ETA_BIN_EC;
    firstPhi = FIRST_PHI_BIN_EC;
    lastPhi = LAST_PHI_BIN_EC;
    nEta = N_ETA_BINS_EC;
    nPhi = N_PHI_BINS_EC;
  }
  Prof2_t tmp = new TProfile2D(TString(name), TString(
							     title), nEta, firstEta - 0.5, lastEta + 0.5, nPhi, firstPhi - 0.5, lastPhi + 0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  bool success(registry.regHist(tmp).isSuccess());
  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  storageVector.push_back(tmp);
  return success ? tmp : NULL;
}

SCTHitsNoiseMonTool::H1_t
SCTHitsNoiseMonTool::h1Factory(const std::string &name, const std::string &title, MonGroup &registry, const float lo,
                               const float hi, const unsigned int nbins) {
  H1_t tmp = TH1F_LW::create(TString(name), TString(title), nbins, lo, hi);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  return success ? tmp : NULL;
}

SCTHitsNoiseMonTool::H1_t
SCTHitsNoiseMonTool::h1Factory(const std::string &name, const std::string &title, MonGroup &registry,
                               VecH1_t &storageVector, const float lo, const float hi, const unsigned int nbins) {
  H1_t tmp = TH1F_LW::create(TString(name), TString(title), nbins, lo, hi);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  storageVector.push_back(tmp);
  return success ? tmp : NULL;
}

TH1F*
SCTHitsNoiseMonTool::th1Factory(const std::string &name, const std::string &title, MonGroup &registry, const float lo,
				const float hi, const unsigned int nbins) {
  TH1F* tmp = new TH1F(TString(name), TString(title), nbins, lo, hi);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  return success ? tmp : NULL;
}

TH1F*
SCTHitsNoiseMonTool::th1Factory(const std::string &name, const std::string &title, MonGroup &registry,
                               std::vector<TH1F*> &storageVector, const float lo, const float hi, const unsigned int nbins) {
  TH1F* tmp = new TH1F(TString(name), TString(title), nbins, lo, hi);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    msg(MSG::WARNING) << "Cannot book SCT histogram: " << name << endmsg;
  }
  storageVector.push_back(tmp);
  return success ? tmp : NULL;
}
