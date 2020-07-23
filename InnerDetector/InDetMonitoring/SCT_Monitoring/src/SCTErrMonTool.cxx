/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*    @file SCTErrMonTool.cxx
 *
 *    @author Martin White, based on code by Luca Fiorini, Shaun Roe, Manuel Diaz & Rob McPherson
 *    Major tidying/restructuring by Martin Flechl
 */

#include "SCTErrMonTool.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "StoreGate/ReadHandle.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1I.h"
#include "TH2F.h"
#include "TProfile.h"

#include <iostream>
#include <map>
#include <utility>

using namespace SCT_Monitoring;

namespace {
  // anon. namespace for file-scoped functions
  // test offline the online code
  static const bool testOffline{false};

  int
  numberOfInefficientSides(TH2* pHistogramArray[], const int xbin, const int ybin, const float threshold) {
    // If at least either of pointers is null, no inefficient sides are reported.
    if (pHistogramArray[0]==nullptr or pHistogramArray[1]==nullptr) return 0;

    double histogramBinContent0{pHistogramArray[0]->GetBinContent(xbin, ybin)};
    double histogramBinContent1{pHistogramArray[1]->GetBinContent(xbin, ybin)};
    int side0Inefficient{histogramBinContent0 > threshold};
    int side1Inefficient{histogramBinContent1 > threshold};

    return side0Inefficient + side1Inefficient;
  }

  bool
  eitherSideIsOverThreshold(TH2* pHistogramArray[], const int xbin, const int ybin, const float threshold) {
    // If at least either of pointers is null, no sides with over threshold are reported.
    if (pHistogramArray[0]==nullptr or pHistogramArray[1]==nullptr) return false;

    double histogramBinContent0{pHistogramArray[0]->GetBinContent(xbin, ybin)};
    double histogramBinContent1{pHistogramArray[1]->GetBinContent(xbin, ybin)};

    return ((histogramBinContent0 > threshold) or (histogramBinContent1 > threshold));
  }

  bool
  thereAreEnoughEntries(TH2* pHistogramArray[], const float threshold) {
    // If at least either of pointers is null, enough entries are not reported.
    if (pHistogramArray[0]==nullptr or pHistogramArray[1]==nullptr) return false;

    double histogramEntries0{pHistogramArray[0]->GetEntries()};
    double histogramEntries1{pHistogramArray[1]->GetEntries()};

    return ((histogramEntries0 > threshold) and (histogramEntries1 > threshold));
  }

  void
  countNoisyModules(const int regionIndex, const int generalIndex, TH2* pHistogram[],
                    const float threshold, int countArray[]) {
    // If at least either of pointers is null, counting is not done.
    if (pHistogram[0]==nullptr or pHistogram[1]==nullptr) return;

    const int xbins{pHistogram[0]->GetNbinsX() + 1};
    const int ybins{pHistogram[0]->GetNbinsY() + 1};

    for (int xb{1}; xb < xbins; ++xb) {
      for (int yb{1}; yb < ybins; ++yb) {
        if (eitherSideIsOverThreshold(pHistogram, xb, yb, threshold)) {
          countArray[generalIndex]++;
          countArray[regionIndex]++;
        }
      }
    }
  }
}


// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection of the filepath for histograms,
 *  the first and second plane numbers to be used, and the timebin.
 */
// ====================================================================================================
SCTErrMonTool::SCTErrMonTool(const std::string& type, const std::string& name, const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent) {
}

// ====================================================================================================
//====================================================================================================
StatusCode SCTErrMonTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));

  ATH_CHECK(m_eventInfoKey.initialize());

  moduleGeo_t moduleGeo; // dummy value
  const unsigned int maxHash{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())}; // 8176
  m_geo.resize(maxHash, moduleGeo);
  
  double rz{0.};
  const double deltaZ{0.};

  const InDetDD::SCT_DetectorManager* sctManager{nullptr};
  ATH_CHECK(detStore()->retrieve(sctManager, "SCT"));

  for (unsigned int i{0}; i<maxHash; i++) {
    IdentifierHash hash{i};
    const InDetDD::SiDetectorElement* newElement{sctManager->getDetectorElement(hash)};
    newElement->getEtaPhiRegion(deltaZ,
                                moduleGeo.first.first,  moduleGeo.first.second,
                                moduleGeo.second.first, moduleGeo.second.second,
                                rz);
    m_geo[i] = moduleGeo;
  }
  ////
  return ManagedMonitorToolBase::initialize();
}

// ====================================================================================================
//             SCTErrMonTool :: transfer [enum CategoryErrors] -> [TString ErrorName]
// ====================================================================================================
TString
SCTErrMonTool::errorsString(int errtype) const {
  if (errtype == MASKEDLINKALL) {
    return "MaskedLinkALL";
  }
  if (errtype == SUMMARY) {
    return "Errors";
  }
  if (errtype == BADERR) {
    return "BadErrors";
  }
  if (errtype == LINKLEVEL) {
    return "LinkLevelErrors";
  }
  if (errtype == RODLEVEL) {
    return "RODLevelErrors";
  }
  if (errtype == MASKEDCHIP) {
    return "MaskedChipALL";
  }
  return "";
}

// ====================================================================================================
//                            SCTErrMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTErrMonTool::bookHistograms() {

  ATH_MSG_DEBUG(" initialize being called ");
  if (newRunFlag()) {
    m_numberOfEvents = 0;
    if (AthenaMonManager::dataType() == AthenaMonManager::cosmics) {
      m_checkrate = 100;
    }
  }
  if (ManagedMonitorToolBase::newLumiBlockFlag()) {
    m_numberOfEventsLumi = 0;
  }

  // Services for Summary Histograms: SCT_ModuleConditionsTool from CondDB
  ATH_MSG_INFO("Checking for CondDB");
  ATH_CHECK(m_ConfigurationTool.retrieve());

  // get a handle on the histogramming service //
  ATH_CHECK(m_thistSvc.retrieve());

  /// SUMMARY HISTOGRAM
  if (bookConfMapsGen().isFailure()) {
    ATH_MSG_WARNING("Error in bookConfMapsGen()");
  }
  if (bookErrHistosGen().isFailure()) {
    ATH_MSG_WARNING("Error in bookErrHistosGen()");
  }

  for (int reg{0}; reg<N_REGIONS; reg++) {
    if (bookErrHistos(reg).isFailure()) {
      ATH_MSG_WARNING("Error in bookErrHistos(): " << "SCT" + subDetNameShort[reg]);
    }
  }

  ATH_CHECK(m_byteStreamErrTool.retrieve());
  if (m_useDCS) ATH_CHECK(m_dcsTool.retrieve());
  else m_dcsTool.disable();
  ATH_CHECK(m_pSummaryTool.retrieve());
  ATH_CHECK(m_flaggedTool.retrieve());
  m_initialize = true;
  return StatusCode::SUCCESS;
}

//====================================================================================================
//          SCTErrMonTool :: bookHistogramsRecurrent, Keisuke Koda 12.09.2016
//====================================================================================================
StatusCode SCTErrMonTool::bookHistogramsRecurrent() {
  static const std::string profNames[numberOfProblemForCoverage] = {
    "", // All
    "SCT_CoverageOfEnabledLinksVsLbs", // All - Disabled
    "SCT_CoverageWithNoBadLinkLevelErrorVsLbs", // All - BadLinkLevelError
    "SCT_CoverageWithNoBadRODLevelErrorVsLbs", // All - BadRODLevelError
    "SCT_CoverageWithNoBadErrorVsLbs", // All - BadError
    "SCT_CoverageWithNoPSTripVsLbs", // All - PSTrip(DCS)
    "SCT_CoverageOfLinksWithNoBadProblemVsLbs" // All - Summary
  };
  static const std::string profTitles[numberOfProblemForCoverage] = {
    "", // All
    "Ave. Coverage of Enabled Links per LB", // All - Disabled
    "Ave. Coverage of Links with No Bad LinkLevelError per LB", // All - BadLinkLevelError
    "Ave. Coverage of Links with No Bad RODLevelError per LB", // All - BadRODLevelError
    "Ave. Coverage of Links with No Bad Error per LB", // All - BadError
    "Ave. Coverage of links Not Affected by PS Trip", // All - PSTrip(DCS)
    "Ave. Coverage of Links With No Bad Problem per LB" // All - Summary
  };

  bool status{true};
  if ( not m_CoverageCheck ) {
    return StatusCode::SUCCESS;
  }

  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup monGr_shift{this, "SCT/DetectorCoverage", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED};

    //All SCT module for counting good module
    m_mapSCT[allRegion] = new TH2F( "SCT_AllRegion", "Map of All Region",
                              s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI );
    //Disabled
    m_mapSCT[disabled] = new TH2F( "SCT_MapOfDisabledLinks", "Map of Disabled Links",
                                   s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI );
    //BadLinkLevelError
    m_mapSCT[badLinkError] = new TH2F( "SCT_MapOfLinksWithBadLinkLevelErrors", "Map of Links with bad LinkLevelErrors",
                                       s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI );
    //BadRODLevelError
    m_mapSCT[badRODError] = new TH2F( "SCT_MapOfLinksWithBadRODLevelErrors", "Map of Links with Bad RODLevelErrors",
                                      s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI );
    //BadError = BadLinkLevelError + BadRODLevelError
    m_mapSCT[badError] = new TH2F( "SCT_MapOfLinksWithBadErrors", "Map of Links with Bad Errors",
                                   s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI );
    //Power supply trip (SCT_DCSConditionsTool)
    m_mapSCT[psTripDCS] = new TH2F( "SCT_MapOfLinksWithPSTrip", "Map of Links Affected by PS Trip",
                                    s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI );
    //Total (SCT_ConditionsSummaryTool)
    m_mapSCT[summary] = new TH2F( "SCT_MapOfLinksWithAnyProbelm", "Map of Links with Any Bad Problem",
                                  s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI );
    
    //Detector Coverage vs LumiBlock
    for (int iProblem{0}; iProblem<numberOfProblemForCoverage ; iProblem++) {
      m_mapSCT[iProblem]->GetXaxis()->SetTitle("#eta");
      m_mapSCT[iProblem]->GetYaxis()->SetTitle("#phi");
      status &= monGr_shift.regHist( m_mapSCT[iProblem] ).isSuccess();
      m_mapSCT[iProblem]->SetStats(0);
    }

    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      if (iProblem==allRegion) continue;

      m_detectorCoverageVsLbs[iProblem] = new TProfile(profNames[iProblem].c_str(), profTitles[iProblem].c_str(), NBINS_LBs,0.5,NBINS_LBs+0.5);
      m_detectorCoverageVsLbs[iProblem]->GetXaxis()->SetTitle("LumiBlock");
      m_detectorCoverageVsLbs[iProblem]->GetYaxis()->SetTitle("Detector Coverage [%]");
      if (monGr_shift.regHist(m_detectorCoverageVsLbs[iProblem]).isFailure()) {
        ATH_MSG_WARNING("Cannot book " << profNames[iProblem] << ", " << profTitles[iProblem]);
      }
    }

    //Number of Modules affected by PS Trip vs LumiBlock
    m_PSTripModulesVsLbs = new TProfile("SCT_ModulesWithPSTripVsLbs",
                                        "Ave. Num of Modules Affected by PS Trip per LB in All Region",
                                        NBINS_LBs,0.5,NBINS_LBs+0.5);
    m_PSTripModulesVsLbs->GetXaxis()->SetTitle("LumiBlock");
    m_PSTripModulesVsLbs->GetYaxis()->SetTitle("Num. of Modules Affected by PS Trip");
    if ( monGr_shift.regHist(m_PSTripModulesVsLbs).isFailure() ) {
      ATH_MSG_WARNING("Cannot book Histogram:SCTpstripWafer" );
    }
  }

  return ( status ) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

// ====================================================================================================
//                            SCTErrMonTool :: fillHistograms
//  This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTErrMonTool::fillHistograms() {

  SG::ReadHandle<xAOD::EventInfo> pEvent{m_eventInfoKey};
  if (not pEvent.isValid()) {
    ATH_MSG_WARNING("Could not retrieve event info!");
    return StatusCode::SUCCESS;
  }
  m_current_lb = pEvent->lumiBlock();
  bool sctflag{false};
  if (pEvent->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error) {
    m_NumberOfSCTFlagErrorsVsLB->Fill(m_current_lb);
    m_FractionOfSCTFlagErrorsPerLB->Fill(m_current_lb, 1);
    sctflag = true;
  } else {
    m_FractionOfSCTFlagErrorsPerLB->Fill(m_current_lb, 0);
  }
  m_NumberOfEventsVsLB->Fill(m_current_lb);

  ATH_CHECK(fillByteStreamErrors());

  // Check wafers with many fired strips (event dependent) using SCT_FlaggedConditionTool.
  std::array<int, N_REGIONS_INC_GENERAL> nFlaggedWafers;
  nFlaggedWafers.fill(0);
  const unsigned int wafer_hash_max{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())};
  for (unsigned int iHash{0}; iHash<wafer_hash_max; iHash++) {
    const IdentifierHash hash{iHash};
    if (not m_flaggedTool->isGood(hash)) {
      const Identifier wafer_id{m_pSCTHelper->wafer_id(hash)};
      const int barrel_ec{m_pSCTHelper->barrel_ec(wafer_id)};
      nFlaggedWafers[barrel_ec]++;
      nFlaggedWafers[GENERAL_INDEX]++;
    }
  }
  for (int reg{0}; reg<N_REGIONS_INC_GENERAL; reg++) {
    m_flaggedWafers->Fill(reg, nFlaggedWafers[reg]);
  }

  if (sctflag) {
    return StatusCode::SUCCESS;
  }

  if (m_environment == AthenaMonManager::online) {
    if ((m_numberOfEvents == 1) or ((m_numberOfEvents > 1) and (m_numberOfEvents % m_checkrate) == 0)) {
      if (resetCondDBMaps().isFailure()) {
        ATH_MSG_WARNING("Error in resetCondDBMaps()");
      }
      if (fillCondDBMaps().isFailure()) {
        ATH_MSG_WARNING("Error in fillCondDBMaps()");
      }
      if ((m_numberOfEvents % (m_checkrate * 10)) == 0) {
        if (resetConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in resetConfigurationDetails()");
        }
        if (fillConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in fillConfigurationDetails()");
        }
      }
    }
  }
  if (m_environment == AthenaMonManager::online) {
    if ((m_current_lb % m_checkrecent == 0) and (m_current_lb > m_last_reset_lb)) {
      m_last_reset_lb = m_current_lb;
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTErrMonTool :: checkRateHists
// DD 17/08/2010
// Here we have the check of the histograms and we can build rate
// error rate histograms from the basic ones
// ====================================================================================================
StatusCode
SCTErrMonTool::checkRateHists() {
  /** TO BE REPLACED WHEN NEW VERSION OF BASE CLASS IS AVAILABLE: **/
  bool isEndOfEventsBlock{endOfLumiBlockFlag()};

  if (m_environment != AthenaMonManager::online) {
    if (m_initialize) {
      float content{0.};
      float cxb{0.};
      float cyb{0.};
      int evt_lumi{m_numberOfEventsLumi};
      for (int reg{0}; reg<N_REGIONS; reg++) {
        if (m_doPerLumiErrors and endOfLumiBlockFlag()) {
          const unsigned int xbins{m_numErrorsPerLumi[reg]->GetNbinsX() + 1};
          const unsigned int ybins{m_numErrorsPerLumi[reg]->GetNbinsY() + 1};
          for (unsigned int xb{1}; xb < xbins; ++xb) {
            cxb = m_numErrorsPerLumi[reg]->GetXaxis()->GetBinCenter(xb);
            for (unsigned int yb{1}; yb < ybins; ++yb) {
              cyb = m_numErrorsPerLumi[reg]->GetYaxis()->GetBinCenter(yb);
              int num_modules{getNumModules(index2Bec(reg), yb - 1)};
              content = m_numErrorsPerLumi[reg]->GetBinContent(xb, yb);
              if (num_modules > 0) {
                m_rateErrorsPerLumi[reg]->Fill(cxb, cyb, 1, content);
                m_rateErrorsPerLumi[reg]->Fill(cxb, cyb, 0, (evt_lumi * (static_cast<double>(num_modules)) - content));
              }
            }
          }
        }
      }

      if (isEndOfEventsBlock) {
        if (resetCondDBMaps().isFailure()) {
          ATH_MSG_WARNING("Error in resetCondDBMaps()");
        }
        if (fillCondDBMaps().isFailure()) {
          ATH_MSG_WARNING("Error in fillCondDBMaps()");
        }
        if (resetConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in resetConfigurationDetails()");
        }
        if (fillConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in fillConfigurationDetails()");
        }
      }
    } // mf moved brackets to allow online compilation
  }

  ATH_MSG_INFO("SCTErrMonTool, package version " << PACKAGE_VERSION);

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                    SCTErrMonTool :: procHistograms
// ====================================================================================================
StatusCode
SCTErrMonTool::procHistograms() {
  if (endOfLumiBlockFlag()) {
    ATH_MSG_DEBUG("finalHists()");
    ATH_MSG_DEBUG("Total Rec Event Number: " << m_numberOfEvents);
    ATH_MSG_DEBUG("Calling checkHists(true); true := end of run");
    if (checkRateHists().isFailure()) {
      ATH_MSG_WARNING("Error in checkRateHists()");
    }
    ATH_MSG_DEBUG("Exiting finalHists");
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrorsHelper, Martin Flechl 10/09/2009
// ====================================================================================================
int
SCTErrMonTool::fillByteStreamErrorsHelper(const std::set<IdentifierHash>& errors,
                                          bool lumi2DHist, int err_type) {

  //--- Check categories of the BS error
  bool b_category[CategoryErrors::N_ERRCATEGORY];

  b_category[CategoryErrors::MASKEDLINKALL] =
    (err_type == SCT_ByteStreamErrors::MaskedLink) or (err_type == SCT_ByteStreamErrors::MaskedROD);

  b_category[CategoryErrors::SUMMARY] = true;

  b_category[CategoryErrors::BADERR] = false;
  for (SCT_ByteStreamErrors::ErrorType tmpBadError: SCT_ByteStreamErrors::BadErrors) {
    if (err_type == tmpBadError) {
      b_category[CategoryErrors::BADERR] = true;
      break;
    }
  }


  b_category[CategoryErrors::LINKLEVEL] = false;
  for (SCT_ByteStreamErrors::ErrorType linkLevelError: SCT_ByteStreamErrors::LinkLevelErrors) {
    if (err_type == linkLevelError) {
      b_category[CategoryErrors::LINKLEVEL] = true;
      break;
    }
  }

  b_category[CategoryErrors::RODLEVEL] = false;
  for (SCT_ByteStreamErrors::ErrorType rodLevelError: SCT_ByteStreamErrors::RodLevelErrors) {
    if (err_type == rodLevelError) {
      b_category[CategoryErrors::RODLEVEL] = true;
      break;
    }
  }

  b_category[CategoryErrors::MASKEDCHIP] =
    (err_type == SCT_ByteStreamErrors::TempMaskedChip0) or (err_type == SCT_ByteStreamErrors::TempMaskedChip1) or
    (err_type == SCT_ByteStreamErrors::TempMaskedChip2) or (err_type == SCT_ByteStreamErrors::TempMaskedChip3) or
    (err_type == SCT_ByteStreamErrors::TempMaskedChip4) or (err_type == SCT_ByteStreamErrors::TempMaskedChip5);

  //--- Count BS errors
  int nerrors{0};
  for (const auto& hash: errors) {
    nerrors++;
    if (not hash.is_valid()) continue;

    //--- FIll module information with BS error
    Identifier fitId{m_pSCTHelper->wafer_id(hash)};
    int layer{m_pSCTHelper->layer_disk(fitId)};
    int side{m_pSCTHelper->side(fitId)};
    int barrel_ec{m_pSCTHelper->barrel_ec(fitId)};
    int ieta{m_pSCTHelper->eta_module(fitId)};
    int iphi{m_pSCTHelper->phi_module(fitId)};
    layer = layer * 2 + side;
    // barrel_ec = {ENDCAP_C=-2, BARREL=0, ENDCAP_A=2}
    // -> regionIndex = {ENDCAP_C_INDEX=0, BARREL_INDEX=1, ENDCAP_A_INDEX=2, GENERAL_INDEX=3}
    int regionIndex{GENERAL_INDEX};
    if ((barrel_ec == BARREL) and (layer >= 0) and (layer < N_BARRELSx2)) regionIndex = BARREL_INDEX;
    else if (barrel_ec == ENDCAP_A) regionIndex = ENDCAP_A_INDEX;
    else if (barrel_ec == ENDCAP_C) regionIndex = ENDCAP_C_INDEX;

    //--- Fill BS errors to the map for each categories
    if (b_category[CategoryErrors::MASKEDLINKALL]) m_MaskedAllLinks->Fill(GENERAL_INDEX);
    if (b_category[CategoryErrors::MASKEDLINKALL] and (regionIndex!=GENERAL_INDEX)) m_MaskedAllLinks->Fill(regionIndex);

    if (not lumi2DHist) {
      if (m_doPerLumiErrors) m_numErrorsPerLumi[regionIndex]->Fill(err_type, layer);
      for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
        if (b_category[errType]) m_pallErrsCate[errType][regionIndex][layer]->Fill(ieta, iphi);
      }
    }
  }

  if (b_category[CategoryErrors::SUMMARY]) return nerrors;
  return 0;
}

// ====================================================================================================
//          SCTErrMonTool :: numByteStreamErrors, Daniel Damiani 04/07/2011
// ====================================================================================================
void
SCTErrMonTool::numByteStreamErrors(const std::set<IdentifierHash>& errors, int& ntot) const {

  for (const auto& fit: errors) {
    if (fit.is_valid()) {
      ntot++;
    }
  }

}

// ====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrors
// ====================================================================================================
StatusCode
SCTErrMonTool::fillByteStreamErrors() {

  //--- Get event information
  SG::ReadHandle<xAOD::EventInfo> pEvent{m_eventInfoKey};
  if (not pEvent.isValid()) {
    ATH_MSG_ERROR("Could not retrieve event info!");
    return StatusCode::RECOVERABLE;
  }
  unsigned int current_lb{pEvent->lumiBlock()};

  bool sctflag{false};
  if (pEvent->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error) {
    sctflag = true;
  }

  //--- Fill 1D histograms (vs LumiBlock) for each BS
  for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
    int bs_errs{0}; // ALL
    // get number of BS errors
    numByteStreamErrors(m_byteStreamErrTool->getErrorSet(errType),
                        bs_errs);
    // fill number of BS errors vs LBs
    if (not sctflag) m_ByteStreamVsLB[errType]->Fill(current_lb, static_cast<double>(bs_errs));
  }

  if (sctflag) {
    return StatusCode::SUCCESS;
  }

  //--- Reset Histograms
  m_MaskedAllLinks->Reset();

  int tot_mod_bytestreamCate_errs[CategoryErrors::N_ERRCATEGORY];
  for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
    tot_mod_bytestreamCate_errs[errType] = 0;
  }

  for (int reg{0}; reg < N_REGIONS; ++reg) {
    const int nLayers{n_layers[reg]*2};
    for (int lyr{0}; lyr < nLayers; ++lyr) {
      for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
        if (m_pallErrsCate[errType][reg][lyr]) m_pallErrsCate[errType][reg][lyr]->Reset();
      }
      if ((m_environment == AthenaMonManager::online) and 
          (m_current_lb % m_checkrecent == 0) and
          (m_current_lb > m_last_reset_lb) and
          m_summaryErrsRecent[reg][lyr]) {
        m_summaryErrsRecent[reg][lyr]->Reset();
      }
    }
  }

  //--- Fill map histograms for each BS
  int total_errors{0};
  for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
    total_errors += fillByteStreamErrorsHelper(m_byteStreamErrTool->getErrorSet(errType), false, errType);
  }

  //--- Fill detector coverage histograms
  if ( m_CoverageCheck ) {
    ATH_MSG_INFO("Detector Coverage calculation starts" );

    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      m_mapSCT[iProblem]->Reset("ICE");
    }

    std::set<IdentifierHash> sctHash[numberOfProblemForCoverage]{{}};
    syncDisabledSCT(sctHash[disabled]);
    syncErrorSCT(sctHash[badLinkError], sctHash[badRODError], sctHash[badError]);
    summarySCT(sctHash[allRegion], sctHash[summary]);
    float PSTripModules{0.};
    psTripDCSSCT(sctHash[psTripDCS],
                 PSTripModules);
    
    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      for (const IdentifierHash& hash: sctHash[iProblem]) {
        fillWafer(m_geo[hash], m_mapSCT[iProblem]);
      }
    }
    
    //detector coverage
    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      if (iProblem==allRegion) continue;

      double detector_coverage{calculateDetectorCoverage(m_mapSCT[iProblem])};
      m_detectorCoverageVsLbs[iProblem]->Fill(static_cast<double>(current_lb), detector_coverage);
    }

    //Modules affected by PS Tirp
    m_PSTripModulesVsLbs ->Fill(static_cast<double>(current_lb), PSTripModules);
    
  }

  //--- Counting # of links with categorised errors
  for (int reg{0}; reg < N_REGIONS; ++reg) {
    const int nLayers{n_layers[reg]*2};
    for (int lyr{0}; lyr < nLayers; ++lyr) {
      const unsigned int nBinsX{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetNbinsX() + 1};
      const unsigned int nBinsY{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetNbinsY() + 1};
      for (unsigned int xb{1}; xb < nBinsX; ++xb) {
        double cxb{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetXaxis()->GetBinCenter(xb)};
        for (unsigned int yb{1}; yb < nBinsY; ++yb) {
          double cyb{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetYaxis()->GetBinCenter(yb)};
          for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
            if (m_pallErrsCate[errType][reg][lyr]->GetBinContent(xb, yb) > 0) {
              m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 1);
              if ((errType == CategoryErrors::SUMMARY) and (m_environment == AthenaMonManager::online)) {
                m_summaryErrsRecent[reg][lyr]->Fill(cxb, cyb, 1);
              }
              tot_mod_bytestreamCate_errs[errType]++;
            } else {
              m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 0);
              if ((errType == CategoryErrors::SUMMARY) and (m_environment == AthenaMonManager::online)) {
                m_summaryErrsRecent[reg][lyr]->Fill(cxb, cyb, 0);
              }
            }
          }

          if (m_environment == AthenaMonManager::online) {
            // mf cast to (int) below to avoid compiler warnings... we do want int, right? Some code duplication
            // below...
            if ((m_numberOfEvents % m_checkrate == 0) and m_runOnline) {
              for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
                int content{static_cast<int>(m_pallErrsCate[errType][reg][lyr]->GetBinContent(xb, yb))};
                int evt{m_numberOfEvents};
                m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 1, content);
                m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 0, evt - content);
              }
            }
          }
        } // Loop for BinY
      } // Loop for BinX
    } // Loop for Layer
  } // Loop for Region

  for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
    m_LinksWithCategorisedErrorsVsLB[errType]->Fill(current_lb, (tot_mod_bytestreamCate_errs[errType]));
  }

  m_numberOfEvents++;
  m_numberOfEventsLumi++;
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTErrMonTool :: bookErrHistosHelper
// Avoids duplicate code in the bookErrHistosXXXX functions; added 8/9/09, Martin Flechl
// ====================================================================================================
StatusCode
SCTErrMonTool::bookErrHistosHelper(MonGroup& mg, TString name, TString title, TString titlehitmap,
                                   TProfile2D_LW*& tprof, TH2F_LW*& th, const int layer, const bool barrel) const {
  std::ostringstream streamhitmap;

  streamhitmap << layer / 2 << "_" << layer % 2;

  name += streamhitmap.str();
  title += streamhitmap.str();
  titlehitmap += streamhitmap.str();

  if (barrel) {
    const float xlo{FIRST_ETA_BIN - 0.5};
    const float xhi{LAST_ETA_BIN  + 0.5};
    const float ylo{FIRST_PHI_BIN - 0.5};
    const float yhi{LAST_PHI_BIN  + 0.5};
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS, xlo, xhi, N_PHI_BINS, ylo, yhi);
    th = TH2F_LW::create(titlehitmap, title, N_ETA_BINS, xlo, xhi, N_PHI_BINS, ylo, yhi);
  } else {
    const float xlo{FIRST_ETA_BIN_EC - 0.5};
    const float xhi{LAST_ETA_BIN_EC  + 0.5};
    const float ylo{FIRST_PHI_BIN_EC - 0.5};
    const float yhi{LAST_PHI_BIN_EC  + 0.5};
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS_EC, xlo, xhi, N_PHI_BINS_EC, ylo, yhi);
    th = TH2F_LW::create(titlehitmap, title, N_ETA_BINS_EC, xlo, xhi, N_PHI_BINS_EC, ylo, yhi);
  }
  StatusCode sc0{mg.regHist(th)};
  StatusCode sc1{mg.regHist(tprof)};
  if (sc0.isFailure() or sc1.isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTErrMonTool :: bookErrHistosHelper
// Avoids duplicate code in the bookErrHistosXXXX functions; added 08/08/11, Daniel Damiani
// ====================================================================================================
StatusCode
SCTErrMonTool::bookErrHistosHelper(MonGroup& mg, TString name, TString title, TProfile2D_LW*& tprof, const int layer,
                                   const bool barrel) const {
  std::ostringstream streamhitmap;

  streamhitmap << layer / 2 << "_" << layer % 2;

  name += streamhitmap.str();
  title += streamhitmap.str();
  if (barrel) {
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS, FIRST_ETA_BIN - 0.5, LAST_ETA_BIN + 0.5, N_PHI_BINS,
                                  FIRST_PHI_BIN - 0.5, LAST_PHI_BIN + 0.5);
  } else {
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS_EC, FIRST_ETA_BIN_EC - 0.5, LAST_ETA_BIN_EC + 0.5,
                                  N_PHI_BINS_EC, FIRST_PHI_BIN_EC - 0.5, LAST_PHI_BIN_EC + 0.5);
  }

  StatusCode sc{mg.regHist(tprof)};
  if (sc.isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTErrMonTool :: bookErrHistos
//                       Book 1D and 2D Histograms of errors
// ====================================================================================================
StatusCode
SCTErrMonTool::bookErrHistos(int reg=-1) { // reg = 0:EC, 1:B, 2:EA

  if (reg==-1) return StatusCode::FAILURE;

  const int nLayers{n_layers[reg]*2};
  std::string regName{("SCT" + subDetNameShort[reg]).Data()};
  std::string layerTitle{layerName[reg].Data()};
  layerTitle.at(0)=toupper(layerTitle.at(0));

  if (m_doPerLumiErrors) {
    if (ManagedMonitorToolBase::newLumiBlockFlag()) {
      MonGroup lumiErr{this, ("SCT/"+regName+"/errors").c_str(), ManagedMonitorToolBase::lumiBlock, ATTRIB_UNMANAGED};
      //______________________________________________________________________________________
      m_numErrorsPerLumi[reg] =
        TH2F_LW::create("NumErrsPerLumi", ("Total Number of Error Types for "+layerTitle+" per Lumi-Block").c_str(),
                        SCT_ByteStreamErrors::NUM_ERROR_TYPES, -0.5, SCT_ByteStreamErrors::NUM_ERROR_TYPES-0.5,
                        nLayers, -0.5, nLayers-0.5);
      if (lumiErr.regHist(m_numErrorsPerLumi[reg]).isFailure()) {
        ATH_MSG_WARNING("Couldn't book NumErrsPerLumi");
      }
      //______________________________________________________________________________________
      m_rateErrorsPerLumi[reg] =
        TProfile2D_LW::create("RateErrorsPerLumi", ("Rate of Error Types for "+layerTitle+" per Lumi-Block").c_str(),
                              SCT_ByteStreamErrors::NUM_ERROR_TYPES, -0.5, SCT_ByteStreamErrors::NUM_ERROR_TYPES-0.5,
                              nLayers, -0.5, nLayers-0.5);
      if (lumiErr.regHist(m_rateErrorsPerLumi[reg]).isFailure()) {
        ATH_MSG_WARNING("Couldn't book RateErrorsPerLumi");
      }
      //______________________________________________________________________________________
      for (unsigned int bin{0}; bin < SCT_ByteStreamErrors::NUM_ERROR_TYPES; bin++) {
        m_numErrorsPerLumi[reg]->GetXaxis()->SetBinLabel(bin+1, SCT_ByteStreamErrors::ErrorTypeDescription[bin].c_str());
        m_rateErrorsPerLumi[reg]->GetXaxis()->SetBinLabel(bin+1, SCT_ByteStreamErrors::ErrorTypeDescription[bin].c_str());
      }
      for (int bin{0}; bin < nLayers; bin++) {
        m_numErrorsPerLumi[reg]->GetYaxis()->SetBinLabel(bin+1, (std::to_string(bin/2) +"_"+ std::to_string(bin%2)).c_str());
        m_rateErrorsPerLumi[reg]->GetYaxis()->SetBinLabel(bin+1, (std::to_string(bin/2) +"_"+ std::to_string(bin%2)).c_str());
      }
    }
  }

  if (ManagedMonitorToolBase::newRunFlag()) {
    bool somethingFailed{false};
    // Book percentage error histograms
    for (int layer{0}; layer < nLayers; ++layer) {
      for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
        MonGroup err2{this, "SCT/"+regName+"/errors/"+errorsString(errType).Data(), ManagedMonitorToolBase::lumiBlock, ATTRIB_UNMANAGED};
        std::string name1{("SCT_NumberOf" + errorsString(errType)  + subDetNameShort[reg].Data() + "_").Data()};
        std::string title{("Num of " + errorsString(errType) + " per "+layerTitle).Data()};
        std::string name2{("SCT_T" + errorsString(errType)  + subDetNameShort[reg].Data() + "_").Data()};
        somethingFailed |= bookErrHistosHelper(err2, name1, title, name2, m_allErrsCate[errType][reg][layer],
                                               m_pallErrsCate[errType][reg][layer], layer, reg==BARREL_INDEX).isFailure();
     }
     if (m_environment == AthenaMonManager::online) {
       MonGroup err{this, "SCT/"+regName+"/errors/", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED};
       somethingFailed |= bookErrHistosHelper(err, 
                                              ("summaryErrsRecent"+subDetNameShort[reg]+"_").Data(), 
                                              "summary recent Layer ", 
                                              m_summaryErrsRecent[reg][layer], 
                                              layer, reg==BARREL_INDEX).isFailure();
      }
    }
    if (somethingFailed) {
      ATH_MSG_DEBUG("Something went wrong in bookErrHistos "<< regName);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
SCTErrMonTool::bookErrHistosGen() {
  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup MaskErrs{this, "SCT/GENERAL/errors", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED};
    m_MaskedAllLinks = new TH1I("Masked Links", "Number of Masked Links for SCT,ECA,B,ECC", N_REGIONS_INC_GENERAL, -0.5, N_REGIONS_INC_GENERAL-0.5);
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(1, "EndCapC");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(2, "Barrel");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(3, "EndCapA");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(4, "All");
    if (MaskErrs.regHist(m_MaskedAllLinks).isFailure()) {
      ATH_MSG_WARNING("Couldn't book MaskedLinks");
    }
    m_flaggedWafers = new TProfile("FlaggedWafers", "Number of flagged wafers for SCT,ECA,B,ECC", N_REGIONS_INC_GENERAL, -0.5, N_REGIONS_INC_GENERAL-0.5);
    m_flaggedWafers->GetXaxis()->SetBinLabel(1, "EndCapC");
    m_flaggedWafers->GetXaxis()->SetBinLabel(2, "Barrel");
    m_flaggedWafers->GetXaxis()->SetBinLabel(3, "EndCapA");
    m_flaggedWafers->GetXaxis()->SetBinLabel(4, "All");
    if (MaskErrs.regHist(m_flaggedWafers).isFailure()) {
      ATH_MSG_WARNING("Couldn't book FlaggedWafers");
    }    
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                         SCTErrMonTool :: bookConfMapsGen
// ====================================================================================================
StatusCode
SCTErrMonTool::bookConfMapsGen() {
  static const std::string SummaryBinNames[ConfbinsSummary] = {
    "Mod Out", "Flagged Links", "Masked Links", "Errors", "Inefficient", "Noisy"
  };
  static const std::string DetailedBinNames[ConfbinsDetailed] = {
    "Modules", "Link 0", "Link 1", "Chips", "Strips (10^{2})"
  };
  static const std::string OnlineBinNames[ConfbinsOnline] = {
    "Mod Out", "Flagged Links", "Masked Links", "Errors"
  };
  static const TString regLabel[N_REGIONS_INC_GENERAL] = {
    "EndcapC", "Barrel", "EndcapA", ""
  };
  static const TString regTitle[N_REGIONS_INC_GENERAL] = {
    "EndcapC", "Barrel", "EndcapA", "All Region"
  };

  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup ConfHist[N_REGIONS_INC_GENERAL] = {
      MonGroup{this, "SCT/SCTEC/Conf",   ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/SCTB/Conf",    ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/SCTEA/Conf",   ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/GENERAL/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED}
    };

    std::string streamName{streamNameFunction()->getStreamName(this, ConfHist[GENERAL_INDEX], "", false)};
    m_path = streamName.substr(0, streamName.rfind("SCT/GENERAL/Conf"));
    ATH_MSG_INFO("Global Path :" << m_path);

    if (m_makeConfHisto or testOffline) {
      m_DetailedConfiguration = TProfile_LW::create("SCTConfDetails", "Exclusion from the Configuration",
                                                    ConfbinsDetailed, -0.5, ConfbinsDetailed - 0.5);
      for (int bin{0}; bin < ConfbinsDetailed; bin++) {
        m_DetailedConfiguration->GetXaxis()->SetBinLabel(bin + 1, DetailedBinNames[bin].c_str());
      }
      if (ConfHist[GENERAL_INDEX].regHist(m_DetailedConfiguration).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:SCTConfDetails");
      }
      
      m_ConfNew = TProfile_LW::create("SCTConf"+regLabel[GENERAL_INDEX]+"New", "Num of Problematic Modules in "+regTitle[GENERAL_INDEX],
                                      ConfbinsSummary-1, -0.5, ConfbinsSummary-1-0.5);
      for (int bin{1}; bin < ConfbinsSummary; bin++) {
        m_ConfNew->GetXaxis()->SetBinLabel(bin, SummaryBinNames[bin].c_str());
      }
      if (ConfHist[GENERAL_INDEX].regHist(m_ConfNew).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:SCTConf");
      }

      m_ConfOutModules = TProfile_LW::create("SCTConf"+regLabel[GENERAL_INDEX]+"OutM", "Num of Out Modules in "+regTitle[GENERAL_INDEX], 1, -0.5, 0.5);
      m_ConfOutModules->GetXaxis()->SetBinLabel(1, "Mod Out");
      if (ConfHist[GENERAL_INDEX].regHist(m_ConfOutModules).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:SCTConf");
      }

      for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
        m_ByteStreamVsLB[errType] =
          TProfile_LW::create("SCT_" + SCT_ByteStreamErrors::ErrorTypeDescription[errType] + "VsLbs" + regLabel[GENERAL_INDEX],
                              "Ave. " + SCT_ByteStreamErrors::ErrorTypeDescription[errType] + " per LB in " + regTitle[GENERAL_INDEX],
                              NBINS_LBs, 0.5, NBINS_LBs + 0.5);
        m_ByteStreamVsLB[errType]->GetXaxis()->SetTitle("LumiBlock");
        m_ByteStreamVsLB[errType]->GetYaxis()->SetTitle("Num of " + TString(SCT_ByteStreamErrors::ErrorTypeDescription[errType]));

        if (ConfHist[GENERAL_INDEX].regHist(m_ByteStreamVsLB[errType]).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:" + SCT_ByteStreamErrors::ErrorTypeDescription[errType]);
        }
      }

      for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
        m_LinksWithCategorisedErrorsVsLB[errType] =
          TProfile_LW::create("SCT_LinksWith" + errorsString(errType) + "VsLbs" + regLabel[GENERAL_INDEX],
                              "Ave. Num of Links with " + errorsString(errType) + " per LB in " + regTitle[GENERAL_INDEX],
                              NBINS_LBs, 0.5, NBINS_LBs+0.5);
        m_LinksWithCategorisedErrorsVsLB[errType]->GetXaxis()->SetTitle("LumiBlock");
        m_LinksWithCategorisedErrorsVsLB[errType]->GetYaxis()->SetTitle("Num of Links with "+errorsString(errType));

        if (ConfHist[GENERAL_INDEX].regHist(m_LinksWithCategorisedErrorsVsLB[errType]).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTLinksWith" + errorsString(errType) + "VsLbs" + regLabel[GENERAL_INDEX]);
        }
      }

      m_NumberOfSCTFlagErrorsVsLB = TH1F_LW::create("NumberOfSCTFlagErrorsVsLB","Num of SCT Flag errors per LB ",NBINS_LBs, 0.5, NBINS_LBs + 0.5);
      m_NumberOfSCTFlagErrorsVsLB->GetXaxis()->SetTitle("LumiBlock");
      if (ConfHist[GENERAL_INDEX].regHist(m_NumberOfSCTFlagErrorsVsLB).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:NumberOfSCTFlagErrors");
      }

      m_NumberOfEventsVsLB = TH1F_LW::create("NumberOfEventsVsLB", "Num of events per LB ", NBINS_LBs, 0.5,NBINS_LBs + 0.5);
      m_NumberOfEventsVsLB->GetXaxis()->SetTitle("LumiBlock");
      if (ConfHist[GENERAL_INDEX].regHist(m_NumberOfEventsVsLB).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:NumberOfEventsVsLB");
      }

      m_FractionOfSCTFlagErrorsPerLB = TProfile_LW::create("FractionOfSCTFlagErrorsPerLB", "Frac of SCT Flag errors per LB ",NBINS_LBs, 0.5, NBINS_LBs + 0.5);
      m_FractionOfSCTFlagErrorsPerLB->GetXaxis()->SetTitle("LumiBlock");
      if (ConfHist[GENERAL_INDEX].regHist(m_FractionOfSCTFlagErrorsPerLB).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:FractionOfSCTFlagErrorsPerLB");
      }

      if ((m_environment == AthenaMonManager::online) or testOffline) {
        m_ConfEffOnline = new TProfile("SCTEffConf", "Number of Inefficient Modules Online",
                                       N_REGIONS_INC_GENERAL, -0.5, N_REGIONS_INC_GENERAL-0.5);
        m_ConfNoiseOnline = TProfile_LW::create("SCTNoiseConf", "Number of Noisy Modules Online",
                                                N_REGIONS_INC_GENERAL, -0.5, N_REGIONS_INC_GENERAL-0.5);
        m_ConfNoiseOnlineRecent = TProfile_LW::create("SCTNoiseConfRecent", "Number of Noisy Modules Online Recent",
                                                      N_REGIONS_INC_GENERAL, -0.5, N_REGIONS_INC_GENERAL - 0.5);
        for (int reg{0}; reg < N_REGIONS_INC_GENERAL; ++reg) {
          m_ConfOnline[GENERAL_INDEX] = TProfile_LW::create("SCTOnlineConf"+regLabel[GENERAL_INDEX], "Num of Out Links in "+regTitle[GENERAL_INDEX]+" Online",
                                                  ConfbinsOnline, -0.5, ConfbinsOnline-0.5);
          for (int bin{0}; bin < ConfbinsOnline; bin++) {
            m_ConfOnline[GENERAL_INDEX]->GetXaxis()->SetBinLabel(bin + 1, OnlineBinNames[bin].c_str());
          }

          if (ConfHist[GENERAL_INDEX].regHist(m_ConfOnline[GENERAL_INDEX]).isFailure()) {
            ATH_MSG_WARNING("Cannot book Histogram:SCTConfOnline");
          }

          m_ConfEffOnline->GetXaxis()->SetBinLabel(reg + 1, ("Ineff "+subDetNameShort[GENERAL_INDEX]).Data());
          m_ConfNoiseOnline->GetXaxis()->SetBinLabel(reg + 1, ("Noisy "+subDetNameShort[GENERAL_INDEX]).Data());
          m_ConfNoiseOnlineRecent->GetXaxis()->SetBinLabel(reg + 1, ("Noisy "+subDetNameShort[GENERAL_INDEX]).Data());
        }

        if (ConfHist[GENERAL_INDEX].regHist(m_ConfEffOnline).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConfEffOnline");
        }
        if (ConfHist[GENERAL_INDEX].regHist(m_ConfNoiseOnline).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConfNoiseOnline");
        }
        if (ConfHist[GENERAL_INDEX].regHist(m_ConfNoiseOnlineRecent).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConfNoiseOnlineRecent");
        }
      }
      ATH_MSG_DEBUG("Finished registering Conf Histograms :" << m_path);
    }// end if m_makeConfHisto or testOffline
  }// end if isNewRun
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: fillCondDBMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::fillCondDBMaps() {
  double Flagged[N_REGIONS_INC_GENERAL] = {
    m_flaggedWafers->GetBinContent(1),
    m_flaggedWafers->GetBinContent(2),
    m_flaggedWafers->GetBinContent(3),
    m_flaggedWafers->GetBinContent(4)
  };
  int MOut[N_REGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int MaskedAllLinks[N_REGIONS_INC_GENERAL] = {
    static_cast<int>(m_MaskedAllLinks->GetBinContent(1)),
    static_cast<int>(m_MaskedAllLinks->GetBinContent(2)),
    static_cast<int>(m_MaskedAllLinks->GetBinContent(3)),
    static_cast<int>(m_MaskedAllLinks->GetBinContent(4))
  };
  int ModErr[N_REGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int InEffModules[N_REGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int NoisyModules[N_REGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int NoisyModulesRecent[N_REGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };

  bool failedbooking{false};
  TH2* hitsHisto_tmp[2];

  std::vector<TH2F> p2DmapHistoVectorAll[SCT_Monitoring::N_REGIONS]{};
  for (int reg{0}; reg < N_REGIONS; ++reg) {
    std::string regName{("SCT" + subDetNameShort[reg]).Data()};
    const int nLayers{n_layers[reg]*2};
    for (int layer{0}; layer < nLayers; ++layer) {
      std::string mapName{std::string{"modulemap"} + subDetNameShort[reg].Data() + std::to_string(layer/2) + "_" + std::to_string(layer%2)};
      TH2F hitsHisto_tmp2{TH2F(mapName.c_str(),
                               (std::string{"Module Out of Configuration : "}+subDetName[reg].Data()+", "+
                                layerName[reg].Data()+" "+std::to_string(layer/2)+
                                " side "+std::to_string(layer%2)).c_str(),
                               n_etabins[reg], f_etabin[reg] - 0.5, l_etabin[reg] + 0.5,
                               n_phibins[reg], f_phibin[reg] - 0.5, l_phibin[reg] + 0.5)};
      hitsHisto_tmp2.GetXaxis()->SetTitle("Index in the direction of #eta");
      hitsHisto_tmp2.GetYaxis()->SetTitle("Index in the direction of #phi");
      p2DmapHistoVectorAll[reg].push_back(hitsHisto_tmp2);
    }
  }

  // Pointers are deleted by regHist() method
  SCT_ID::const_id_iterator planeIterator{m_pSCTHelper->wafer_begin()};
  SCT_ID::const_id_iterator planeEnd{m_pSCTHelper->wafer_end()};
  for (; planeIterator not_eq planeEnd; ++planeIterator) {
    Identifier planeId{*planeIterator};
    int eta{m_pSCTHelper->eta_module(planeId)};
    int phi{m_pSCTHelper->phi_module(planeId)};
    int barrel_ec{m_pSCTHelper->barrel_ec(planeId)};
    int reg{BARREL_INDEX};
    if (barrel_ec == ENDCAP_A) reg = ENDCAP_A_INDEX;
    if (barrel_ec == ENDCAP_C) reg = ENDCAP_C_INDEX;
    int element{2 * m_pSCTHelper->layer_disk(planeId) + m_pSCTHelper->side(planeId)};
    int IN{m_ConfigurationTool->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1};
    if (m_pSCTHelper->side(planeId) == 0) {
      if (IN == 1) {
        MOut[GENERAL_INDEX]++;
        MOut[reg]++;
      }
    }
    // danger: no check that barrel_ec is valid
    int y{phi + 1};
    int x{(barrel_ec == BARREL) ? (eta + 7) : (eta + 1)};
    p2DmapHistoVectorAll[reg][element].SetBinContent(x, y, IN);
  }

  // region
  for (int reg{0}; reg<N_REGIONS; reg++) {
    const int nLayers{n_layers[reg]};
    for (int lyr{0}; lyr < nLayers; ++lyr) {

      // Check for the number of Modules with more than m_errThreshold error rate
      const unsigned int xbins{m_allErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetNbinsX() + 1};
      const unsigned int ybins{m_allErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetNbinsY() + 1};
      for (unsigned int xb{1}; xb < xbins; ++xb) {
        for (unsigned int yb{1}; yb < ybins; ++yb) {
          if (m_allErrsCate[CategoryErrors::SUMMARY][reg][2*lyr  ]->GetBinContent(xb, yb) > m_errThreshold or
              m_allErrsCate[CategoryErrors::SUMMARY][reg][2*lyr+1]->GetBinContent(xb, yb) > m_errThreshold) {
            ModErr[GENERAL_INDEX]++;
            ModErr[reg]++;
          }
        }
      }

      // noisy modules
      failedbooking = getHisto(lyr, reg, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(reg, GENERAL_INDEX, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
      }
      // noisy modules recent
      if (m_environment == AthenaMonManager::online) {
        failedbooking = getHistoRecent(lyr, reg, 0, hitsHisto_tmp);
        if (failedbooking == false) {
          countNoisyModules(reg, GENERAL_INDEX, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
        }
      }

      // ineff modules
      failedbooking = getHisto(lyr, reg, 1, hitsHisto_tmp);
      if (failedbooking == false) {
        if ((m_environment != AthenaMonManager::online) or 
            thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or
            testOffline) {
          const int xbins{hitsHisto_tmp[0]->GetNbinsX() + 1};
          const int ybins{hitsHisto_tmp[0]->GetNbinsY() + 1};
          // if (testOffline) printContents(hitsHisto_tmp[0]);
          for (int xb{1}; xb < xbins; ++xb) {
            for (int yb{1}; yb < ybins; ++yb) {
              float outOfConfig{static_cast<float>(p2DmapHistoVectorAll[reg][2 * lyr].GetBinContent(xb, yb))};
              if (outOfConfig < 1.) { // i.e. its in the configuration
                int nSides{numberOfInefficientSides(hitsHisto_tmp, xb, yb, 1 - m_effThreshold)};
                InEffModules[GENERAL_INDEX] += nSides;
                InEffModules[reg] += nSides;
              }
            }
          }
        }
      }
    }
  }

  if (m_makeConfHisto) {
    m_ConfOutModules->Fill(0., static_cast<double>(MOut[GENERAL_INDEX]));
    m_ConfNew->Fill(0., Flagged[GENERAL_INDEX]);
    m_ConfNew->Fill(1., static_cast<double>(MaskedAllLinks[GENERAL_INDEX]));
    m_ConfNew->Fill(2., static_cast<double>(ModErr[GENERAL_INDEX]));
    m_ConfNew->Fill(3., static_cast<double>(InEffModules[GENERAL_INDEX]));
    m_ConfNew->Fill(4., static_cast<double>(NoisyModules[GENERAL_INDEX]));
    if (m_environment == AthenaMonManager::online) {
      for (int reg{0}; reg < N_REGIONS_INC_GENERAL; ++reg) {
        m_ConfOnline[reg]->Fill(0., static_cast<double>(MOut[reg]));
        m_ConfOnline[reg]->Fill(1., Flagged[reg]);
        m_ConfOnline[reg]->Fill(2., static_cast<double>(MaskedAllLinks[reg]));
        m_ConfOnline[reg]->Fill(3., static_cast<double>(ModErr[reg]));
      }
    }
    if ((m_environment == AthenaMonManager::online) or testOffline) {
      m_ConfEffOnline->Reset("ICE");
      for (int reg{0}; reg < N_REGIONS_INC_GENERAL; ++reg) {
        const float f{static_cast<float>(reg)};
        m_ConfEffOnline->Fill(f, static_cast<double>(InEffModules[reg]));
        m_ConfNoiseOnline->Fill(f, static_cast<double>(NoisyModules[reg]));
        m_ConfNoiseOnlineRecent->Fill(f, static_cast<double>(NoisyModulesRecent[reg]));
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                   SCTErrMonTool :: fillConfigurationDetails
// Fills Number of Bad Modules / Links / Chips / Strips in to a histogram
// ====================================================================================================
StatusCode
SCTErrMonTool::fillConfigurationDetails() {
  ATH_MSG_DEBUG("Inside fillConfigurationDetails()");
  unsigned int nBadMods{static_cast<unsigned int>(m_ConfigurationTool->badModules()->size())}; // bad modules
  const std::map<IdentifierHash, std::pair<bool, bool>>* badLinks{m_ConfigurationTool->badLinks()}; // bad links
  unsigned int nBadLink0{0}, nBadLink1{0}, nBadLinkBoth{0};
  for (const std::pair<const IdentifierHash, std::pair<bool, bool>>& link: *badLinks) {
    std::pair<bool, bool> status{link.second};
    if ((status.first == false) and (status.second == true)) {
      ++nBadLink0;
    }
    if ((status.first == true) and (status.second == false)) {
      ++nBadLink1;
    }
    if ((status.first == false) and (status.second == false)) {
      ++nBadLinkBoth;
    }
  }

  const std::map<Identifier, unsigned int>* badChips{m_ConfigurationTool->badChips()}; // bad chips
  unsigned int nBadChips{0};
  for (const std::pair<const Identifier, unsigned int>& chip : *badChips) {
    unsigned int status{chip.second};
    for (unsigned int i{0}; i < CHIPS_PER_MODULE; i++) {
      nBadChips += ((status & (1 << i)) == 0 ? 0 : 1);
    }
  }

  std::set<Identifier> badStripsAll; // bad strips
  m_ConfigurationTool->badStrips(badStripsAll);
  unsigned int nBadStrips{static_cast<unsigned int>(badStripsAll.size())};

  std::set<Identifier> badStripsExclusive; // bad strips w/o bad modules and chips
  m_ConfigurationTool->badStrips(badStripsExclusive, true, true);
  int nBadStripsExclusive{static_cast<int>(badStripsExclusive.size())};
  int nBadStripsExclusiveBEC[N_REGIONS] = {
    0, 0, 0
  };
  for (const Identifier& strip: badStripsExclusive) {
    int bec{m_pSCTHelper->barrel_ec(strip)};
    nBadStripsExclusiveBEC[bec2Index(bec)] += 1;
  }

  m_DetailedConfiguration->Fill(0., static_cast<double>(nBadMods));
  m_DetailedConfiguration->Fill(1., static_cast<double>(nBadLink0));
  m_DetailedConfiguration->Fill(2., static_cast<double>(nBadLink1));
  m_DetailedConfiguration->Fill(3., static_cast<double>(nBadChips));
  m_DetailedConfiguration->Fill(4., static_cast<double>(nBadStripsExclusive) / 100.);

  ATH_MSG_DEBUG("-----------------------------------------------------------------------");
  ATH_MSG_DEBUG("Number of bad modules                          = " << nBadMods);
  ATH_MSG_DEBUG("Number of bad link 0                           = " << nBadLink0);
  ATH_MSG_DEBUG("Number of bad link 1                           = " << nBadLink1);
  ATH_MSG_DEBUG("Number of bad link both                        = " << nBadLinkBoth);
  ATH_MSG_DEBUG("Number of bad chips                            = " << nBadChips);
  ATH_MSG_DEBUG("Number of bad strips                           = " << nBadStrips);
  ATH_MSG_DEBUG("Number of bad strips exclusive                 = " << nBadStripsExclusive);
  ATH_MSG_DEBUG("Number of bad strips exclusive (ECC, B, ECA)   = "
                << nBadStripsExclusiveBEC[ENDCAP_C_INDEX] << ", "
                << nBadStripsExclusiveBEC[BARREL_INDEX] << ", "
                << nBadStripsExclusiveBEC[ENDCAP_A_INDEX] << ", ");
  ATH_MSG_DEBUG("-----------------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: resetCondDBMaps, Daniel Damiani 25/07/2011
// ====================================================================================================
StatusCode
SCTErrMonTool::resetCondDBMaps() {
  if (m_environment != AthenaMonManager::online) {
    return StatusCode::SUCCESS;
  }
  if (m_makeConfHisto) {
    for (int reg{0}; reg < N_REGIONS_INC_GENERAL; ++reg) {
      m_ConfOnline[reg]->Reset();
    }
    m_ConfEffOnline->Reset();
    m_ConfNoiseOnline->Reset();
    m_ConfNoiseOnlineRecent->Reset();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: resetConfigurationDetails, Daniel Damiani 25/07/2011
// ====================================================================================================
StatusCode
SCTErrMonTool::resetConfigurationDetails() {
  if (m_environment != AthenaMonManager::online) {
    return StatusCode::SUCCESS;
  }
  if (m_makeConfHisto) {
    m_DetailedConfiguration->Reset();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: getHisto, Martin Flechl 14/9/2009
// ====================================================================================================
bool
SCTErrMonTool::getHisto(const int layer, const int reg, const int type, TH2* histo[2]) const {
  static const std::string trm[2][N_REGIONS] = { // 2 is the number of types (noise, eff)
    {"SCT/SCTEC/Noise/noiseoccupancymapECm_","SCT/SCTB/Noise/noiseoccupancymap_", "SCT/SCTEA/Noise/noiseoccupancymapECp_"},
    {"SCT/SCTEC/eff/ineffm_", "SCT/SCTB/eff/ineff_", "SCT/SCTEA/eff/ineffp_"}
  };
  static const std::string trm_trig[2][N_REGIONS] = { // 2 is the number of types (noise, eff)
    {"SCT/SCTEC/Noise/noiseoccupancymaptriggerECm_", "SCT/SCTB/Noise/noiseoccupancymaptrigger_", "SCT/SCTEA/Noise/noiseoccupancymaptriggerECp_"},
    {"SCT/SCTEC/eff/ineffm_", "SCT/SCTB/eff/ineff_", "SCT/SCTEA/eff/ineffp_"}
  };
  std::string shname1{m_path + trm[type][reg] + std::to_string(layer)};
  std::string shname2{m_path + trm[type][reg] + std::to_string(layer)};

  if (testOffline or ((m_environment != AthenaMonManager::online) and
                      (AthenaMonManager::dataType() != AthenaMonManager::cosmics))) {
    shname1 = m_path + trm_trig[type][reg] + std::to_string(layer);
    shname2 = m_path + trm_trig[type][reg] + std::to_string(layer);
  }
  if ((type==0) or (type==1)) {
    shname1 += "_0";
    shname2 += "_1";
  }
  if (type==2) {
    shname2 = "";
  }
  bool failedBooking{false};
  histo[0]=nullptr;
  histo[1]=nullptr;
  if (shname1!="") {
    if (m_thistSvc->existsHist(shname1)) {
      if (m_thistSvc->getHist(shname1, histo[0]).isFailure()) {
        ATH_MSG_WARNING("Retrieval of " << shname1 << " failed");
        failedBooking = true;
      }
    } else {
      ATH_MSG_INFO(shname1 << " does not exist");
      failedBooking = true;
    }
  }
  if (shname2!="") {
    if (m_thistSvc->existsHist(shname2)) {
      if (m_thistSvc->getHist(shname2, histo[1]).isFailure()) {
        ATH_MSG_WARNING("Retrieval of " << shname2 << " failed");
        failedBooking = true;
      }
    } else {
      ATH_MSG_INFO(shname2 << " does not exist");
      failedBooking = true;
    }
  }
  return failedBooking;
}

// ====================================================================================================
//                          SCTErrMonTool :: getHistoRecent, Dan Damiani  21/7/2011
// ====================================================================================================
bool
SCTErrMonTool::getHistoRecent(const int layer, const int reg, const int type, TH2* histo[2]) const {
  static const std::string trm[1][N_REGIONS] = {
    {"SCT/SCTEC/Noise/noiseoccupancymaprecentECm_", "SCT/SCTB/Noise/noiseoccupancymaprecent_", "SCT/SCTEA/Noise/noiseoccupancymaprecentECp_"}
  };
  std::string shname1{m_path + trm[type][reg] + std::to_string(layer) + "_0"};
  std::string shname2{m_path + trm[type][reg] + std::to_string(layer) + "_1"};

  bool failedBooking{false};
  histo[0] = nullptr;
  histo[1] = nullptr;

  if (m_thistSvc->existsHist(shname1)) {
    if (m_thistSvc->getHist(shname1, histo[0]).isFailure()) {
      ATH_MSG_WARNING("Retrieval of " << shname1 << " failed");
      failedBooking = true;
    }
  } else {
    ATH_MSG_INFO(shname1 << " does not exist");
    failedBooking = true;
  }

  if (m_thistSvc->existsHist(shname2)) {
    if (m_thistSvc->getHist(shname2, histo[1]).isFailure()) {
      ATH_MSG_WARNING("Retrieval of " << shname2 << " failed");
      failedBooking = true;
    }
  } else {
    ATH_MSG_INFO(shname2 << " does not exist");
    failedBooking = true;
  }

  return failedBooking;
}

//====================================================================================================
//                          SCTErrMonTool :: fillWafer, Keisuke Kouda 12.09.2016
//====================================================================================================
void SCTErrMonTool::fillWafer(moduleGeo_t module, TH2F* histo) const {
  double etaMin{module.first.first}, etaMax{module.first.second};
  double phiMin{module.second.first}, phiMax{module.second.second};
  unsigned int nRep{1};
  if (etaMin<-s_rangeEta) { etaMin = -s_rangeEta; }
  if (etaMax> s_rangeEta) { etaMax =  s_rangeEta; }
  if (phiMin>phiMax) {
    phiMin = -M_PI;
    nRep=2;
  }
  for (unsigned int iRep{0}; iRep<nRep; iRep++) {
    if (iRep==1) {
      phiMin = module.second.first;
      phiMax = M_PI;
    }
    const int ixMin{static_cast<int>((etaMin/s_rangeEta+1.)*s_nBinsEta/2)+1};
    const int ixMax{static_cast<int>((etaMax/s_rangeEta+1.)*s_nBinsEta/2)};
    const int iyMin{static_cast<int>((phiMin/M_PI+1.)*s_nBinsPhi/2)+1};
    const int iyMax{static_cast<int>((phiMax/M_PI+1.)*s_nBinsPhi/2)};
    const double xMin{(static_cast<double>(ixMin)/s_nBinsEta*2-1.)*s_rangeEta};
    const double xMax{(static_cast<double>(ixMax)/s_nBinsEta*2-1.)*s_rangeEta};
    const double yMin{(static_cast<double>(iyMin)/s_nBinsPhi*2-1.)*M_PI};
    const double yMax{(static_cast<double>(iyMax)/s_nBinsPhi*2-1.)*M_PI};
    const double wxMin{(xMin-etaMin)/s_rangeEta*s_nBinsEta/2};
    const double wxMax{(etaMax-xMax)/s_rangeEta*s_nBinsEta/2};
    const double wxOne{(etaMax-etaMin)/s_rangeEta*s_nBinsEta/2};
    const double wyMin{(yMin-phiMin)/M_PI*s_nBinsPhi/2};
    const double wyMax{(phiMax-yMax)/M_PI*s_nBinsPhi/2};
    const double wyOne{(phiMax-phiMin)/M_PI*s_nBinsPhi/2};
    for (int ix{ixMin}; ix<=ixMax+1; ix++) {
      double weightx{1.};
      if (ixMin==ixMax+1) weightx = wxOne;
      else if (ix==ixMin) weightx = wxMin;
      else if (ix==ixMax+1) weightx = wxMax;
      for (int iy{iyMin}; iy<=iyMax+1; iy++) {
        double weight{weightx};
        if (iyMin==iyMax+1) weight *= wyOne;
        else if (iy==iyMin) weight *= wyMin;
        else if (iy==iyMax+1) weight *= wyMax;
        histo->SetBinContent(ix, iy, histo->GetBinContent(ix,iy)+weight); //Fill(ix, iy, weight);//
      }
    }
  }
  return;
}

//====================================================================================================
//                          SCTErrMonTool :: SyncSCT, Keisuke Kouda 12.09.2016
//====================================================================================================
bool SCTErrMonTool::syncErrorSCT(std::set<IdentifierHash>& sctHashBadLinkError,
                                 std::set<IdentifierHash>& sctHashBadRODError,
                                 std::set<IdentifierHash>& sctHashBadError) const {
  sctHashBadLinkError.clear();
  sctHashBadRODError.clear();
  sctHashBadError.clear();
 
  //BadLinkLevelError
  for (SCT_ByteStreamErrors::ErrorType linkLevelBadErrors: SCT_ByteStreamErrors::LinkLevelBadErrors) {
    const std::set<IdentifierHash> sctErrors{m_byteStreamErrTool->getErrorSet( linkLevelBadErrors )};
    for (const IdentifierHash& waferHash : sctErrors) {
      sctHashBadLinkError.insert(waferHash);
    }
  }

  //BadRODLevelError
  for (SCT_ByteStreamErrors::ErrorType RodLevelBadErrors: SCT_ByteStreamErrors::RodLevelBadErrors) {
    const std::set<IdentifierHash> sctErrors{m_byteStreamErrTool->getErrorSet( RodLevelBadErrors )};
    for (const IdentifierHash& waferHash: sctErrors) {
      sctHashBadRODError.insert(waferHash);
    }
  }

  //BadError = BadLinkLevelError + BadRODLevelError
  for (SCT_ByteStreamErrors::ErrorType tmpBadError: SCT_ByteStreamErrors::BadErrors) {
    const std::set<IdentifierHash> sctErrors{m_byteStreamErrTool->getErrorSet( tmpBadError )};
    for (const IdentifierHash& waferHash: sctErrors) {
      sctHashBadError.insert(waferHash);
    }
  }
  return true;
}

//Disabled
bool SCTErrMonTool::syncDisabledSCT(std::set<IdentifierHash>& sctHashDisabled) const {
  bool altered{false};
  sctHashDisabled.clear();
  const std::set<Identifier>* badModules{m_ConfigurationTool->badModules()};

  for (const Identifier& badModule: *badModules) {
    altered = true;
    IdentifierHash hashSide0{m_pSCTHelper->wafer_hash(badModule)};
    IdentifierHash hashSide1;
    m_pSCTHelper->get_other_side(hashSide0, hashSide1);
    sctHashDisabled.insert(hashSide0);
    sctHashDisabled.insert(hashSide1);
  }
  return altered;
}

//Total (SCT_ConditionsSummaryTool) //All
bool SCTErrMonTool::summarySCT(std::set<IdentifierHash>& sctHashAll, std::set<IdentifierHash>& sctHashSummary) const  {
  bool altered{false};
  sctHashAll.clear();//All
  sctHashSummary.clear();

  const unsigned int maxHash{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())}; // 8176
  for (unsigned int i{0}; i<maxHash; i++) {
    IdentifierHash hash{i};
    sctHashAll.insert(hash);//All
    if (not m_pSummaryTool->isGood(hash)) {
      sctHashSummary.insert(hash);
    }
  }
  return altered;
}

//Power supply trip (SCT_DCSConditionsTool)
bool SCTErrMonTool::psTripDCSSCT(std::set<IdentifierHash>& sctHashPSTripDCS, float& PSTripModules) const {
  bool altered{false};
  sctHashPSTripDCS.clear();

  const unsigned int maxHash{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())}; // 8176
  int npsw{0};
  for (unsigned int i{0}; i<maxHash; i++) {
    IdentifierHash hash{i};
    if (m_useDCS and (not m_dcsTool->isGood(hash))) {
      npsw++; //Counting the number of PS sides
      altered = true;
      sctHashPSTripDCS.insert(hash);
    }
  }
  PSTripModules = npsw/2.;
  return altered;
}

//====================================================================================================
//                          SCTErrMonTool :: calculateDetectorCoverage, Keisuke Kouda 12.09.2016
//====================================================================================================
double SCTErrMonTool::calculateDetectorCoverage( const TH2F* histo ) const {
  double occupancy{0.};

  for (unsigned int i{0}; i < s_nBinsEta; i++) {
    for (unsigned int j{0}; j < s_nBinsPhi; j++) {
      double waferCell{m_mapSCT[allRegion]->GetBinContent(i+1, j+1) - histo->GetBinContent(i+1, j+1)};

      if (waferCell >= s_WafersThreshold) {
        occupancy += 1.0;
      } else if (waferCell > s_WafersThreshold - 1.0 ) {
        occupancy += waferCell - (s_WafersThreshold - 1.0);
        //Calculating the bin occupancy which has less than 1. 
        //For example, bin have a 2.3. In this case, we can understand that 30% of the bin is coverd by 3 sides/wafers and 70% of the bin is coverd by 2 sides/wafers.
        //And it means that occupancy of the bin is 0.3 So, in this line, I take difference between s_WafersThreshold(3)-1 and waferCell, and add it to the occupancy.
      }
    }
  }
  double detector_coverage{100. * static_cast<double>( occupancy )/( static_cast<double>( s_nBinsEta ) * static_cast<double>( s_nBinsPhi ) )};
  return detector_coverage;
}

const unsigned int SCTErrMonTool::s_nBinsEta = 100;
const double SCTErrMonTool::s_rangeEta = 2.5;
const unsigned int SCTErrMonTool::s_nBinsPhi = 100;
const double SCTErrMonTool::s_WafersThreshold = 3.0;
