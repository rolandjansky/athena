/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTErrMonTool.cxx
 *
 *
 *
 *
 *    @author Martin White, based on code by Luca Fiorini, Shaun Roe, Manuel Diaz & Rob McPherson
 *    Major tidying/restructuring by Martin Flechl
 */


#include "SCT_Monitoring/SCTErrMonTool.h"
#include "SCT_Monitoring/SCT_ClusterStruct.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"

// conditions stuff
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "xAODEventInfo/EventInfo.h"
#include "cArrayUtilities.h"
#include <vector>
#include <set>
#include <iostream>
#include <map>


using namespace std;
using namespace SCT_Monitoring;

namespace { // anon. namespace for file-scoped functions
  // test offline the online code
  const bool testOffline(false);

  enum Pattern {
    IXX=0, XIX, XXI
  };

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
      return false;

      break;
    }
  }

  const std::string names[] = {
    "Endcap C", "Barrel", "Endcap A"
  };
  const std::string abbreviations[] = {
    "ECC", "Bar", "ECA"
  };

  int
  numberOfInefficientSides(TH2 *pHistogramArray[], const int xbin, const int ybin, const float threshold) {
    float histogramBinContent0 = pHistogramArray[0]->GetBinContent(xbin, ybin);
    float histogramBinContent1 = pHistogramArray[1]->GetBinContent(xbin, ybin);
    int side0Inefficient = (histogramBinContent0 > threshold);
    int side1Inefficient = (histogramBinContent1 > threshold);

    return side0Inefficient + side1Inefficient;
  }

  bool
  eitherSideIsOverThreshold(TH2 *pHistogramArray[], const int xbin, const int ybin, const float threshold) {
    float histogramBinContent0 = pHistogramArray[0]->GetBinContent(xbin, ybin);
    float histogramBinContent1 = pHistogramArray[1]->GetBinContent(xbin, ybin);

    return((histogramBinContent0 > threshold)or(histogramBinContent1 > threshold));
  }

  bool
  checkOverThreshold(TProfile2D_LW *pHistogram, const int xbin, const int ybin, const float threshold) {
    float histogramBinContent = pHistogram->GetBinContent(xbin, ybin);

    return(histogramBinContent > threshold);
  }

  bool
  thereAreEnoughEntries(TH2 *pHistogramArray[], const float threshold) {
    float histogramEntries0 = pHistogramArray[0]->GetEntries();
    float histogramEntries1 = pHistogramArray[1]->GetEntries();

    return((histogramEntries0 > threshold)and(histogramEntries1 > threshold));
  }

  void
  fillRateHistogram(const TH2F_LW *sourceHisto, TProfile2D_LW *targetHisto, const int xbin, const int ybin,
                    const float centreX, const float centreY, const int nEvent) {
    const float content = sourceHisto->GetBinContent(xbin, ybin);

    targetHisto->Fill(centreX, centreY, 1, content);
    targetHisto->Fill(centreX, centreY, 0, nEvent - content);
  }

  void
  countNoisyModules(const int regionIndex, const int generalIndex, TH2 *pHistogramArray[], const float threshold,
                    int countArray[]) {
    const int xbins(pHistogramArray[0]->GetNbinsX() + 1);
    const int ybins(pHistogramArray[0]->GetNbinsY() + 1);

    for (int xb(1); xb != xbins; ++xb) {
      for (int yb(1); yb != ybins; ++yb) {
        if (eitherSideIsOverThreshold(pHistogramArray, xb, yb, threshold)) {
          countArray[generalIndex]++;
          countArray[regionIndex]++;
        }
      }
    }
  }

  void
  countNoisyModulesRN(const int regionIndex, const int generalIndex, TProfile2D_LW *pHistogram, const float threshold,
                      int countArray[]) {
    const int xbins(pHistogram->GetNbinsX() + 1);
    const int ybins(pHistogram->GetNbinsY() + 1);

    for (int xb(1); xb != xbins; ++xb) {
      for (int yb(1); yb != ybins; ++yb) {
        if (checkOverThreshold(pHistogram, xb, yb, threshold)) {
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
SCTErrMonTool::SCTErrMonTool(const std::string &type, const std::string &name, const IInterface *parent)
  : ManagedMonitorToolBase(type, name, parent),
  m_firstHit{},
  m_firstHit_ECp{},
  m_firstHit_ECm{},
  m_secondHit{},
  m_secondHit_ECp{},
  m_secondHit_ECm{},
  m_allErrs{},
  m_allErrsPerLumi{},
  m_pallErrs{},
  m_pallErrsPerLumi{},
  m_summaryErrsRecent{},
  m_numErrorsPerLumi{},
  m_rateErrorsPerLumi{},
  m_nErrors{},
  m_nLinksWithErrors{},
  nErrors_buf{},
  nLinksWithErrors_buf{},
  nErrors_pos{},
  m_MaskedAllLinks{},
  m_numberOfEventsLumi{},
  m_numberOfEvents{},
  m_initialize{},
  m_previous_lb{},
  m_nLink0{},
  m_nLink1{},
  m_goodModules{},
  m_pnoiseoccupancymapHistoVectorECC{},
  m_pnoiseoccupancymapHistoVectorECCSide0{},
  m_pnoiseoccupancymapHistoVectorECCSide1{},
  m_pnoiseoccupancymapHistoVectorBar{},
  m_pnoiseoccupancymapHistoVectorBarSide0{},
  m_pnoiseoccupancymapHistoVectorBarSide1{},
  m_pnoiseoccupancymapHistoVectorECA{},
  m_pnoiseoccupancymapHistoVectorECASide0{},
  m_pnoiseoccupancymapHistoVectorECASide1{},
  m_noSidesHit{},
  m_oneSideHit{},
  nZero{},
  nOne{},
  nOneSide0{},
  nOneSide1{},
  nLayer{},
  nEta{},
  nPhi{},
  nNonGoodModule{},
  tbin(-1),
  modNum(0),
  ratio(-1.0),
  ratioside0(-1.0),
  ratioside1(-1.0),
  m_p2DmapHistoVectorAll{},
  // m_stream{}, declared property
  m_path{},
  // m_checkrate{}, declared property
  // m_checkrecent{}, declared property
  m_current_lb{},
  m_last_reset_lb{},
  m_sctflag(false),
  // m_runOnline{}, declared property
  // m_evtsbins{}, declared property
  // m_doPositiveEndcap{}, declared property
  // m_doNegativeEndcap{}, declared property
  // m_makeConfHisto{}, property
  // m_doPerLumiErrors{}, property
  // m_doErr2DPerLumiHists{}, property
  // m_min_stat_ineff_mod{}, property
  m_dataObjectName{},
  m_pSCTHelper{},
  m_ConfigurationSvc("InDetSCT_ConfigurationConditionsSvc", name),
  m_flaggedSvc("InDetSCT_FlaggedConditionSvc", name),
  m_Conf{},
  m_ConfRN{},
  m_ConfOnline{},
  m_ByteStreamVsLB{},
  m_ByteStreamWithSctFlagVsLB{},
  m_LinksWithAnyErrorsVsLB{},
  m_LinksWithBadErrorsVsLB{},
  m_LinksWithLnkErrorsVsLB{},
  m_LinksWithRODErrorsVsLB{},
  m_LinksWithAnyErrorsVsLBLayer{},
  m_LinksWithBadErrorsVsLBLayer{},
  m_LinksWithLnkErrorsVsLBLayer{},
  m_LinksWithRODErrorsVsLBLayer{},
  m_NumberOfSCTFlagErrorsVsLB{},
  m_NumberOfEventsVsLB{},
  m_ConfEffOnline{},
  m_ConfNoiseOnline{},
  m_ConfNoiseOnlineRecent{},
  m_DetailedConfiguration{},
  m_thistSvc("THistSvc", name),
  m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc", name),
  m_checkBadModules(true),
  m_ignore_RDO_cut_online(true) {
    // m_errThreshold{}, property
    // m_effThreshold{}, property
    // m_noiseThreshold{}, property
    /** sroe 3 Sept 2015:
	histoPathBase is declared as a property in the base class, assigned to m_path
	with default as empty string.
	Declaring it here as well gives rise to compilation warning
	WARNING duplicated property name 'histoPathBase', see https://its.cern.ch/jira/browse/GAUDI-1023

	declareProperty("histoPathBase", m_stream = "/stat"); **/

    m_stream = "/stat";
    declareProperty("CheckRate", m_checkrate = 1000);
    declareProperty("runOnline", m_runOnline = false);
    declareProperty("CheckRecent", m_checkrecent = 20);
    declareProperty("doPositiveEndcap", m_doPositiveEndcap = true);
    declareProperty("doNegativeEndcap", m_doNegativeEndcap = true);
    declareProperty("EvtsBins", m_evtsbins = 5000);
    declareProperty("MakeConfHisto", m_makeConfHisto = true);
    declareProperty("conditionsService", m_ConfigurationSvc);
    declareProperty("flaggedService", m_flaggedSvc);

    // Thresholds for the SCTConf histogram
    declareProperty("error_threshold", m_errThreshold = 0.7);
    declareProperty("efficiency_threshold", m_effThreshold = 0.9);
    declareProperty("noise_threshold", m_noiseThreshold = 150);
    // Min stats per layer to use for number of inefficient modules
    declareProperty("MinStatsForInEffModules", m_min_stat_ineff_mod = 500.0);

    declareProperty("DoPerLumiErrors", m_doPerLumiErrors = true);
    declareProperty("DoErr2DPerLumiHists", m_doErr2DPerLumiHists = false);
    declareProperty("checkBadModules", m_checkBadModules);
    declareProperty("IgnoreRDOCutOnline", m_ignore_RDO_cut_online);
  }

// ====================================================================================================
// ====================================================================================================
SCTErrMonTool::~SCTErrMonTool() {

  VecProf2_t *storageVectors[] = {
    &m_pnoiseoccupancymapHistoVectorECC, &m_pnoiseoccupancymapHistoVectorBar, &m_pnoiseoccupancymapHistoVectorECA
  };
  VecProf2_t *storageVectorsSide0[] = {
    &m_pnoiseoccupancymapHistoVectorECCSide0, &m_pnoiseoccupancymapHistoVectorBarSide0,
    &m_pnoiseoccupancymapHistoVectorECASide0
  };
  VecProf2_t *storageVectorsSide1[] = {
    &m_pnoiseoccupancymapHistoVectorECCSide1, &m_pnoiseoccupancymapHistoVectorBarSide1,
    &m_pnoiseoccupancymapHistoVectorECASide1
  };
  for (int systemIndex = 0; systemIndex < 3; systemIndex++) {
    for (auto prof2: *storageVectors[systemIndex]) {
      LWHist::safeDelete(prof2);
    }
    (storageVectors[systemIndex])->clear();
    for (auto prof2: *storageVectorsSide0[systemIndex]) {
      LWHist::safeDelete(prof2);
    }
    (storageVectorsSide0[systemIndex])->clear();
    for (auto prof2: *storageVectorsSide1[systemIndex]) {
      LWHist::safeDelete(prof2);
    }
    (storageVectorsSide1[systemIndex])->clear();
  }

  free(nErrors_buf);
  free(nLinksWithErrors_buf);
}

// ====================================================================================================
//             SCTErrMonTool :: transfer [enum ErrorTypes] -> [SCT_ByteStreamErrors]
// ====================================================================================================
int
SCTErrMonTool::errorsToGet(int errtype) {
  if (errtype == ABCD) {
    return SCT_ByteStreamErrors::ABCDError;
  }
  if (errtype == RAW) {
    return SCT_ByteStreamErrors::RawError;
  }
  if (errtype == TIMEOUT) {
    return SCT_ByteStreamErrors::TimeOutError;
  }
  if (errtype == LVL1ID) {
    return SCT_ByteStreamErrors::LVL1IDError;
  }
  if (errtype == BCID) {
    return SCT_ByteStreamErrors::BCIDError;
  }
  if (errtype == PREAMBLE) {
    return SCT_ByteStreamErrors::PreambleError;
  }
  if (errtype == FORMATTER) {
    return SCT_ByteStreamErrors::FormatterError;
  }
  if (errtype == MASKEDLINKS) {
    return SCT_ByteStreamErrors::MaskedLink;
  }
  if (errtype == RODCLOCK) {
    return SCT_ByteStreamErrors::RODClockError;
  }
  if (errtype == TRUNCATEDROD) {
    return SCT_ByteStreamErrors::TruncatedROD;
  }
  if (errtype == ROBFRAG) {
    return SCT_ByteStreamErrors::ROBFragmentError;
  }
  if (errtype == BSPARSE) {
    return SCT_ByteStreamErrors::ByteStreamParseError;
  }
  if (errtype == MISSINGLINK) {
    return SCT_ByteStreamErrors::MissingLinkHeaderError;
  }
  if (errtype == MASKEDRODS) {
    return SCT_ByteStreamErrors::MaskedROD;
  }
  return errtype;
}

// ====================================================================================================
//             SCTErrMonTool :: transfer [enum ErrorTypes] -> [TString ErrorName]
// ====================================================================================================
TString
SCTErrMonTool::errorsString(int errtype) {
  if (errtype == ABCD) {
    return "ABCDerrs";
  }
  if (errtype == RAW) {
    return "Rawerrs";
  }
  if (errtype == TIMEOUT) {
    return "TimeOut";
  }
  if (errtype == LVL1ID) {
    return "LVL1IDerrs";
  }
  if (errtype == BCID) {
    return "BCIDerrs";
  }
  if (errtype == PREAMBLE) {
    return "Preamble";
  }
  if (errtype == FORMATTER) {
    return "Formattererrs";
  }
  if (errtype == MASKEDLINKS) {
    return "MaskedLinkLink";
  }
  if (errtype == RODCLOCK) {
    return "RODClockerrs";
  }
  if (errtype == TRUNCATEDROD) {
    return "TruncatedROD";
  }
  if (errtype == ROBFRAG) {
    return "ROBFragment";
  }
  if (errtype == BSPARSE) {
    return "BSParseerrs";
  }
  if (errtype == MISSINGLINK) {
    return "MissingLink";
  }
  if (errtype == MASKEDRODS) {
    return "MaskedROD";
  }
  if (errtype == SUMMARY) {
    return "NumberOfErrors";
  }
  if (errtype == BADERR) {
    return "NumberOfBadErrors";
  }
  if (errtype == LINKLEVEL) {
    return "NumberOfLinkLevelErrors";
  }
  if (errtype == RODLEVEL) {
    return "NumberOfRODLevelErrors";
  }
  if (errtype == MASKEDLINKALL) {
    return "MaskedLink";
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
  m_dataObjectName = "SCT_RDOs";
  const InDetDD::SCT_DetectorManager *mgr; // confusingly this is in a dedicated namespace
  ATH_CHECK(detStore()->retrieve(mgr, "SCT"));
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));

  // Services for Summary Histograms: SCT_ModuleConditionsTool from CondDB and flagged conditions service
  ATH_MSG_INFO("Checking for CondDB");
  ATH_CHECK(m_ConfigurationSvc.retrieve());
  ATH_MSG_INFO("Checking for Flagged Service");
  ATH_CHECK(m_flaggedSvc.retrieve());

  // get a handle on the histogramming service //
  ATH_CHECK(m_thistSvc.retrieve());

  /// SUMMARY HISTOGRAM
  if (bookConfMaps().isFailure()) {
    msg(MSG::WARNING) << "Error in bookConfMaps()" << endmsg;
  }
  if (bookPositiveEndCapConfMaps().isFailure()) {
    msg(MSG::WARNING) << "Error in bookPositiveEndCapConfMaps()" << endmsg;
  }
  if (bookNegativeEndCapConfMaps().isFailure()) {
    msg(MSG::WARNING) << "Error in bookNegativeEndCapConfMaps()" << endmsg;
  }

  /// BARREL
  if (bookErrHistos().isFailure()) {
    msg(MSG::WARNING) << "Error in bookErrHistos()" << endmsg;
  }

  /// POSITIVE ENDCAP
  if (m_doPositiveEndcap) {
    if (bookPositiveEndCapErrHistos().isFailure()) {
      msg(MSG::WARNING) << "Error in bookPositiveEndCapErrHistos()" << endmsg;
    }
  }
  /// NEGATIVE ENDCAP
  if (m_doNegativeEndcap) {
    if (bookNegativeEndCapErrHistos().isFailure()) {
      msg(MSG::WARNING) << "Error in bookNegativeEndCapErrHistos()" << endmsg;
    }
  }
  ATH_CHECK(m_byteStreamErrSvc.retrieve());

  m_initialize = true;
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                            SCTErrMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTErrMonTool::fillHistograms() {
  typedef SCT_RDORawData SCTRawDataType;
  const xAOD::EventInfo *pEvent(0);
  if (evtStore()->retrieve(pEvent).isFailure()) {
    ATH_MSG_WARNING("Could not retrieve event info!");
    return StatusCode::RECOVERABLE;
  }
  m_current_lb = pEvent->lumiBlock();
  m_sctflag = false;
  if (pEvent->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error) {
    // ATH_MSG_WARNING("SCT_Flag==FALSE:: LVL1ID Errors >500 ");
    m_NumberOfSCTFlagErrorsVsLB->Fill(m_current_lb);
    m_sctflag = true;
    //return StatusCode::SUCCESS;
  }
  if (fillByteStreamErrors().isFailure()) {
    ATH_MSG_ERROR("Could not fill ByteStreamErrors ");
    return StatusCode::FAILURE;
  }

  if(m_sctflag){
    return StatusCode::SUCCESS;
  }

  m_NumberOfEventsVsLB->Fill(m_current_lb);
  const SCT_RDO_Container *p_rdocontainer;
  ATH_CHECK(evtStore()->retrieve(p_rdocontainer, m_dataObjectName));
  Identifier SCT_Identifier;

  // Define variables for error histograms
  int numFirstHit = 0, numSecondHit = 0, numSCTRDOs = 0;
  int numFirstHit_ECp = 0, numSecondHit_ECp = 0, numSCTRDOs_ECp = 0;
  int numFirstHit_ECm = 0, numSecondHit_ECm = 0, numSCTRDOs_ECm = 0;

  for (int i = 0; i < n_mod[GENERAL_INDEX]; i++) {
    m_nLink0[i] = 0;
    m_nLink1[i] = 0;
    m_goodModules[i] = true;
  }

  tbin = -1;

  int count_SCT_RDO = 0;
  // Use new IDC
  SCT_RDO_Container::const_iterator lastCol = p_rdocontainer->end();
  for (SCT_RDO_Container::const_iterator col_it = p_rdocontainer->begin(); col_it != lastCol; ++col_it) {
    const InDetRawDataCollection<SCT_RDORawData> *SCT_Collection(*col_it);
    if (!SCT_Collection) {
      continue;  // select only SCT RDOs
    }
    DataVector<SCTRawDataType>::const_iterator p_rdo_end = SCT_Collection->end();
    const InDetRawDataCollection<SCT_RDORawData> *rd(*col_it);

    Identifier SCT_Identifier = SCT_Collection->identify();
    int thisLayerDisk = m_pSCTHelper->layer_disk(SCT_Identifier);
    int thisPhi = m_pSCTHelper->phi_module(SCT_Identifier);
    int thisEta = m_pSCTHelper->eta_module(SCT_Identifier);
    int thisSide = m_pSCTHelper->side(SCT_Identifier);

    modNum = (rd->identifyHash()) / 2;

    bool goodModule =
      (m_checkBadModules and m_ConfigurationSvc) ? m_ConfigurationSvc->isGood(rd->identifyHash()) : true;

    m_goodModules[modNum] = goodModule;
    nLayer[modNum] = thisLayerDisk;
    nEta[modNum] = thisEta;
    nPhi[modNum] = thisPhi;

    if (!goodModule) {
      nNonGoodModule[modNum] = 1;
    }
    for (DataVector<SCTRawDataType>::const_iterator p_rdo = SCT_Collection->begin(); p_rdo != p_rdo_end; ++p_rdo) {
      count_SCT_RDO++;
      SCT3_RawData *rdo3 = dynamic_cast<SCT3_RawData *>(*p_rdo);
      if (rdo3 != 0) {
        tbin = (rdo3)->getTimeBin();
      }
      if (timeBinInPattern(tbin, XIX) && goodModule) {
        if (thisSide == 1) {
          m_nLink1[modNum] += 1;
        }else {
          m_nLink0[modNum] += 1;
        }
      }
      SCT_Identifier = (*p_rdo)->identify();
      int barrel_ec = m_pSCTHelper->barrel_ec(SCT_Identifier);
      if (barrel_ec == BARREL) {
        ++numSCTRDOs;
      }
      if (barrel_ec == ENDCAP_A && m_doPositiveEndcap) {
        ++numSCTRDOs_ECp;
      }
      if (barrel_ec == ENDCAP_C && m_doNegativeEndcap) {
        ++numSCTRDOs_ECm;
      }

      if (not SCT_Identifier.is_valid()) {
        return msg(MSG::ERROR) << "The SCT identifier is not valid." << endmsg, StatusCode::FAILURE;
      }

      // Start filling histograms, whilst counting the number of each type of errors. A map of vectors contains
      // histograms for each type of error. The vectors contain one histogram for each plane of the SCT
      if (rdo3 != 0) {
        if ((rdo3)->FirstHitError()) {
          if (barrel_ec == BARREL) {
            numFirstHit++;
          } else if (barrel_ec == ENDCAP_A && m_doPositiveEndcap) {
            numFirstHit_ECp++;
          } else if (barrel_ec == ENDCAP_C && m_doNegativeEndcap) {
            numFirstHit_ECm++;
          }
        }
        if ((rdo3)->SecondHitError()) {
          if (barrel_ec == BARREL) {
            numSecondHit++;
          } else if (barrel_ec == ENDCAP_A && m_doPositiveEndcap) {
            numSecondHit_ECp++;
          } else if (barrel_ec == ENDCAP_C && m_doNegativeEndcap) {
            numSecondHit_ECm++;
          }
        }
      }
    }
  }// end for(SCT_RDO_Container...

  if (count_SCT_RDO < 1E6 || (m_ignore_RDO_cut_online && m_environment == AthenaMonManager::online)) {
    for (int j = 0; j < n_mod[GENERAL_INDEX]; j++) {
      m_noSidesHit = false;
      m_oneSideHit = false;
      if (m_nLink0[j] != 0 && m_nLink1[j] != 0) {
      } else {
        if (m_nLink0[j] != 0 || m_nLink1[j] != 0) {
          m_oneSideHit = true;
        }
      }
      if (m_nLink0[j] == 0 && m_nLink1[j] == 0 && m_goodModules[j] /*no-sides-fix*/) {
        m_noSidesHit = true;
      }
      if (m_noSidesHit && !nNonGoodModule[j]) {
        nZero[j] += 1;
      }
      if (m_oneSideHit) {
        nOne[j] += 1;
        if (m_nLink0[j] != 0) {
          nOneSide0[j] += 1;
        }
        if (m_nLink1[j] != 0) {
          nOneSide1[j] += 1;
        }
      }   // end if(oneSideHit)
    }
  }
  if (numSCTRDOs != 0) {
    float scale = 100. / float(numSCTRDOs);
    m_firstHit->Fill(float(numFirstHit) * scale, 1.);
    m_secondHit->Fill(float(numSecondHit) * scale, 1.);
  }
  if (numSCTRDOs_ECp != 0 && m_doPositiveEndcap) {
    float scale = 100. / float(numSCTRDOs_ECp);
    m_firstHit_ECp->Fill(float(numFirstHit_ECp) * scale, 1.);
    m_secondHit_ECp->Fill(float(numSecondHit_ECp) * scale, 1.);
  }
  if (numSCTRDOs_ECm != 0 && m_doNegativeEndcap) {
    float scale = 100. / float(numSCTRDOs_ECm);
    m_firstHit_ECm->Fill(float(numFirstHit_ECm) * scale, 1.);
    m_secondHit_ECm->Fill(float(numSecondHit_ECm) * scale, 1.);
  }
  if (m_environment == AthenaMonManager::online) {
    if (m_numberOfEvents == 1 or(m_numberOfEvents > 1 && (m_numberOfEvents % m_checkrate) == 0)) {
      if (resetCondDBMaps().isFailure()) {
        msg(MSG::WARNING) << "Error in resetCondDBMaps()" << endmsg;
      }
      if (fillCondDBMaps().isFailure()) {
        msg(MSG::WARNING) << "Error in fillCondDBMaps()" << endmsg;
      }
      if ((m_numberOfEvents % (m_checkrate * 10)) == 0) {
        if (resetConfigurationDetails().isFailure()) {
          msg(MSG::WARNING) << "Error in resetConfigurationDetails()" << endmsg;
        }
        if (fillConfigurationDetails().isFailure()) {
          msg(MSG::WARNING) << "Error in fillConfigurationDetails()" << endmsg;
        }
      }
    }
  }
  if (m_environment == AthenaMonManager::online) {
    if ((m_current_lb % m_checkrecent == 0) && (m_current_lb > m_last_reset_lb)) {
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
  bool isEndOfEventsBlock(endOfLumiBlockFlag());
  const int nRegions(3);
  const int barrelRegion(0);

  if (m_environment != AthenaMonManager::online) {
    if (m_initialize) {
      float content = 0;
      float cxb = 0;
      float cyb = 0;
      int evt = m_numberOfEvents;
      int evt_lumi = m_numberOfEventsLumi;
      for (int reg = barrelRegion; reg != nRegions; ++reg) {
        if (m_doPerLumiErrors && endOfLumiBlockFlag()) {
          const int xbins = m_numErrorsPerLumi[reg]->GetNbinsX() + 1;
          const int ybins = m_numErrorsPerLumi[reg]->GetNbinsY() + 1;
          for (int xb(1); xb != xbins; ++xb) {
            cxb = m_numErrorsPerLumi[reg]->GetXaxis()->GetBinCenter(xb);
            for (int yb(1); yb != ybins; ++yb) {
              cyb = m_numErrorsPerLumi[reg]->GetYaxis()->GetBinCenter(yb);
              int num_modules = getNumModules(reg, yb - 1);
              content = m_numErrorsPerLumi[reg]->GetBinContent(xb, yb);
              if (num_modules > 0) {
                m_rateErrorsPerLumi[reg]->Fill(cxb, cyb, 1, content);
                m_rateErrorsPerLumi[reg]->Fill(cxb, cyb, 0, (evt_lumi * ((float) num_modules) - content));
              }
            }
          }
        }
        int nlyr = (reg == barrelRegion) ? (N_BARRELSx2) : (N_ENDCAPSx2);
        for (int lyr = 0; lyr < nlyr; ++lyr) {
          const int xbins = m_pallErrs[SUMMARY][reg][lyr]->GetNbinsX() + 1;
          const int ybins = m_pallErrs[SUMMARY][reg][lyr]->GetNbinsY() + 1;
          for (int xb = 1; xb != xbins; ++xb) {
            cxb = m_pallErrs[SUMMARY][reg][lyr]->GetXaxis()->GetBinCenter(xb);
            for (int yb = 1; yb != ybins; ++yb) {
              cyb = m_pallErrs[SUMMARY][reg][lyr]->GetYaxis()->GetBinCenter(yb);
              if (isEndOfEventsBlock) {
                for (int errType(0); errType != SUMMARY; ++errType) {
                  fillRateHistogram(m_pallErrs[errType][reg][lyr], m_allErrs[errType][reg][lyr], xb, yb, cxb, cyb, evt);
                }
              }

              // per lumi hists
              if (m_doPerLumiErrors and endOfLumiBlockFlag() and m_doErr2DPerLumiHists) {
                for (int errType(0); errType != SUMMARY; ++errType) {
                  fillRateHistogram(m_pallErrsPerLumi[errType][reg][lyr], m_allErrsPerLumi[errType][reg][lyr], xb, yb,
                                    cxb, cyb, evt);
                }
              }
            }
          }
        }
      }

      if (isEndOfEventsBlock) {
        if (resetCondDBMaps().isFailure()) {
          msg(MSG::WARNING) << "Error in resetCondDBMaps()" << endmsg;
        }
        if (fillCondDBMaps().isFailure()) {
          msg(MSG::WARNING) << "Error in fillCondDBMaps()" << endmsg;
        }
        if (resetConfigurationDetails().isFailure()) {
          msg(MSG::WARNING) << "Error in resetConfigurationDetails()" << endmsg;
        }
        if (fillConfigurationDetails().isFailure()) {
          msg(MSG::WARNING) << "Error in fillConfigurationDetails()" << endmsg;
        }
      }
    } // mf moved brackets to allow online compilation
  }
  msg(MSG::INFO) << "SCTErrMonTool, package version " << PACKAGE_VERSION << endmsg;
  return StatusCode::SUCCESS;
}

// //====================================================================================================
//                    SCTErrMonTool :: procHistograms
// ====================================================================================================
// StatusCode  SCTErrMonTool::procHistograms(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool /*isEndOfRun*/){
StatusCode
SCTErrMonTool::procHistograms() {
/** TO BE REPLACED **/
  bool endOfEventsBlock(endOfLumiBlockFlag());

  if (endOfEventsBlock || endOfLumiBlockFlag()) {
    ATH_MSG_DEBUG("finalHists()");
    ATH_MSG_DEBUG("Total Rec Event Number: " << m_numberOfEvents);
    ATH_MSG_DEBUG("Calling checkHists(true); true := end of run");
    if (checkRateHists().isFailure()) {
      msg(MSG::WARNING) << "Error in checkRateHists()" << endmsg;
    }
    ATH_MSG_DEBUG("Exiting finalHists");
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrorsHelper, Martin Flechl 10/09/2009
// ====================================================================================================
// int SCTErrMonTool::fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F_LW*
// histo[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2], bool lumi2DHist, int err_type){
int
SCTErrMonTool::fillByteStreamErrorsHelper(const std::set<IdentifierHash> *errors,
                                          TH2F_LW *histo[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2],
                                          bool lumi2DHist, int err_type,
                                          TH2F_LW *histo2[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]) {
  bool b_MaskedLinks = false;

  b_MaskedLinks = (err_type == MASKEDLINKS) || (err_type == MASKEDRODS);
  bool b_BadErrors = false;
  b_BadErrors = (err_type == MASKEDLINKS) || (err_type == MASKEDRODS) || (err_type == ROBFRAG) ||
                (err_type == TIMEOUT) || (err_type == BCID) || (err_type == LVL1ID) || (err_type == MISSINGLINK);
  bool b_RODLevel = false;
  b_RODLevel = (err_type == RODCLOCK) || (err_type == MASKEDRODS) || (err_type == ROBFRAG) ||
               (err_type == TRUNCATEDROD);
  bool b_LinkLevel = false;
  b_LinkLevel = (err_type == ABCD) || (err_type == RAW) || (err_type == TIMEOUT) || (err_type == LVL1ID) ||
                (err_type == BCID) || (err_type == PREAMBLE) || (err_type == FORMATTER) || (err_type == MASKEDLINKS) ||
                (err_type == BSPARSE) || (err_type == MISSINGLINK);

  int nerrors = 0;
  std::set<IdentifierHash>::iterator fit = errors->begin();
  std::set<IdentifierHash>::iterator fitEnd = errors->end();
  for (; fit != fitEnd; ++fit) {
    nerrors++;
    if (b_MaskedLinks) {
      m_MaskedAllLinks->Fill(0);
    }
    if (fit->is_valid()) {
      Identifier fitId = m_pSCTHelper->wafer_id(*fit);
      int layer = m_pSCTHelper->layer_disk(fitId);
      int side = m_pSCTHelper->side(fitId);
      int barrel_ec = m_pSCTHelper->barrel_ec(fitId);
      int ieta(m_pSCTHelper->eta_module(fitId));
      int iphi(m_pSCTHelper->phi_module(fitId));
      layer = layer * 2 + side;
      // why these different indexing systems?
      int regionIndex = iGEN;
      int maskIndex = 0;
      if (barrel_ec == BARREL && layer >= 0 && layer < N_BARRELSx2) {
        regionIndex = iBARREL;
        maskIndex = 2;
      }else if (barrel_ec == ENDCAP_A) {
        regionIndex = iECp;
        maskIndex = 1;
      } else if (barrel_ec == ENDCAP_C) {
        regionIndex = iECm;
        maskIndex = 3;
      }

      if (b_MaskedLinks) {
        m_MaskedAllLinks->Fill(maskIndex);
      }
      histo[regionIndex][layer]->Fill(ieta, iphi);
      if (b_MaskedLinks) {
        histo2[regionIndex][layer]->Fill(ieta, iphi);
      }
      if (lumi2DHist) {
        if (m_doPerLumiErrors && m_doErr2DPerLumiHists) {
          m_pallErrsPerLumi[SUMMARY][regionIndex][layer]->Fill(ieta, iphi);
          if (b_BadErrors) {
            m_pallErrsPerLumi[BADERR][regionIndex][layer]->Fill(ieta, iphi);
          }
          if (b_LinkLevel) {
            m_pallErrsPerLumi[LINKLEVEL][regionIndex][layer]->Fill(ieta, iphi);
          }
          if (b_RODLevel) {
            m_pallErrsPerLumi[RODLEVEL][regionIndex][layer]->Fill(ieta, iphi);
          }
        }
      }else {
        if (m_doPerLumiErrors) {
          m_numErrorsPerLumi[regionIndex]->Fill(err_type, layer);
        }
        m_pallErrs[SUMMARY][regionIndex][layer]->Fill(ieta, iphi);
        if (b_BadErrors) {
          m_pallErrs[BADERR][regionIndex][layer]->Fill(ieta, iphi);
        }
        if (b_LinkLevel) {
          m_pallErrs[LINKLEVEL][regionIndex][layer]->Fill(ieta, iphi);
        }
        if (b_RODLevel) {
          m_pallErrs[RODLEVEL][regionIndex][layer]->Fill(ieta, iphi);
        }
      }
    }
  }
  return nerrors;
}

// ====================================================================================================
//          SCTErrMonTool :: numByteStreamErrors, Daniel Damiani 04/07/2011
// ====================================================================================================
void
SCTErrMonTool::numByteStreamErrors(const std::set<IdentifierHash> *errors, int &ntot, int &nbar, int &neca, int &necc) {
  for (auto fit: *errors) {
    if (fit.is_valid()) {
      Identifier fitId = m_pSCTHelper->wafer_id(fit);
      int layer = m_pSCTHelper->layer_disk(fitId);
      int side = m_pSCTHelper->side(fitId);
      int barrel_ec = m_pSCTHelper->barrel_ec(fitId);
      layer = layer * 2 + side;
      ntot++;
      if (barrel_ec == BARREL && layer >= 0 && layer < N_BARRELSx2) {
        nbar++;
      } else if (barrel_ec == ENDCAP_A) {
        neca++;
      } else if (barrel_ec == ENDCAP_C) {
        necc++;
      }
    }
  }
}

// ====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrors
// ====================================================================================================
StatusCode
SCTErrMonTool::fillByteStreamErrors() {
  // Masked and ROB Fragment vs. lb
  const xAOD::EventInfo *pEvent(0);

  if (evtStore()->retrieve(pEvent).isFailure()) {
    if (msgLvl(MSG::ERROR)) {
      msg(MSG::ERROR) << "Could not retrieve event info!" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  unsigned int current_lb = pEvent->lumiBlock();

  int bytestream_errs[N_ERRTYPES][NREGIONS_INC_GENERAL];
  int tot_mod_bytestream_errs[N_ERRTYPES][NREGIONS_INC_GENERAL];
  int tot_mod_bytestream_errs_layer[N_ERRTYPES][NREGIONS_INC_GENERAL][N_DISKSx2];

  // BS Errors
  for (int errType = 0; errType != SUMMARY; ++errType) {
    bytestream_errs[errType][iGEN] = 0;   // ALL
    bytestream_errs[errType][iBARREL] = 0;// BARREL
    bytestream_errs[errType][iECp] = 0;   // ECA
    bytestream_errs[errType][iECm] = 0;   // ECC
    if (errType == MASKEDLINKALL) {
      continue;
    }
    // get number of BS errors
    numByteStreamErrors(m_byteStreamErrSvc->getErrorSet(errorsToGet(errType)),
			bytestream_errs[errType][iGEN],bytestream_errs[errType][iBARREL],
			bytestream_errs[errType][iECp],bytestream_errs[errType][iECm]);
    // fill number of BS errors vs LBs
    for (int reg = 0; reg != NREGIONS_INC_GENERAL; ++reg) {
      if(!m_sctflag) {
	m_ByteStreamVsLB[errType][reg]->Fill(current_lb, float (bytestream_errs[errType][reg]));
      }
      else {
	m_ByteStreamWithSctFlagVsLB[errType][reg]->Fill(current_lb, float (bytestream_errs[errType][reg]));
      }
    }
  }
  // retrieve MASKEDLINKALL errors
  for (int reg = 0; reg != NREGIONS_INC_GENERAL; ++reg) {
    bytestream_errs[MASKEDLINKALL][reg] = bytestream_errs[MASKEDLINKS][reg] + bytestream_errs[MASKEDRODS][reg];
    if(!m_sctflag) {
      m_ByteStreamVsLB[MASKEDLINKALL][reg]->Fill(current_lb, float (bytestream_errs[MASKEDLINKALL][reg]));
    }
    else {
      m_ByteStreamWithSctFlagVsLB[MASKEDLINKALL][reg]->Fill(current_lb, float (bytestream_errs[MASKEDLINKALL][reg]));
    }
  }

  if(m_sctflag) {
    return StatusCode::SUCCESS;
  }

  // Reset Histograms
  int total_errors = 0;

  m_MaskedAllLinks->Reset();

  for (int reg = 0; reg != NREGIONS_INC_GENERAL; ++reg) {
    int nlyr = (reg == iBARREL) ? (N_BARRELSx2) : (N_ENDCAPSx2);
    for (int errType = SUMMARY; errType != N_ERRTYPES; ++errType) {
      bytestream_errs[errType][reg] = 0;
      tot_mod_bytestream_errs[errType][reg] = 0;
    }
    for (int lyr = 0; lyr != nlyr; ++lyr) {
      if (m_environment == AthenaMonManager::online) {
        if (m_current_lb % m_checkrecent == 0 && m_current_lb > m_last_reset_lb && m_summaryErrsRecent[reg][lyr]) {
          m_summaryErrsRecent[reg][lyr]->Reset();
        }
      }
      for (int errType = SUMMARY; errType != N_ERRTYPES; ++errType) {
        tot_mod_bytestream_errs_layer[errType][reg][lyr] = 0;
        if (m_pallErrs[errType][reg][lyr]) {
          m_pallErrs[errType][reg][lyr]->Reset();
        }
        if (m_doPerLumiErrors && m_doErr2DPerLumiHists) {
          if (m_pallErrsPerLumi[errType][reg][lyr]) {
            m_pallErrsPerLumi[errType][reg][lyr]->Reset();
          }
        }
      }
    }
  }

  // Fill Histograms
  for (int errType = 0; errType != SUMMARY; ++errType) {
    if (errType == MASKEDLINKALL) {
      continue;
    }
    total_errors += fillByteStreamErrorsHelper(m_byteStreamErrSvc->getErrorSet(errorsToGet(
                                                                                 errType)), m_pallErrs[errType], false, errType,
                                               m_pallErrs[MASKEDLINKALL]);
    if (m_doPerLumiErrors && m_doErr2DPerLumiHists) {
      total_errors += fillByteStreamErrorsHelper(m_byteStreamErrSvc->getErrorSet(errorsToGet(
                                                                                   errType)), m_pallErrsPerLumi[errType], false, errType,
                                                 m_pallErrsPerLumi[MASKEDLINKALL]);
    }
  }

  // Count # of links with some type errors (any errors, bad errors, link-level errors, ROD-level errors)
  for (int reg = 0; reg != NREGIONS_INC_GENERAL - 1; ++reg) {
    int nlyr = (reg == iBARREL) ? (N_BARRELSx2) : (N_ENDCAPSx2);
    for (int lyr = 0; lyr != nlyr; ++lyr) {
      if (m_environment == AthenaMonManager::online) {
        if (m_numberOfEvents % m_checkrate == 0 && m_runOnline == true) {
          for (int errType(0); errType != SUMMARY; ++errType) {
            m_allErrs[errType][reg][lyr]->Reset();
          }
        }
      }
      const int nBinsX(m_pallErrs[SUMMARY][reg][lyr]->GetNbinsX() + 1);
      const int nBinsY(m_pallErrs[SUMMARY][reg][lyr]->GetNbinsY() + 1);
      for (int xb = 1; xb != nBinsX; ++xb) {
        float cxb = m_pallErrs[SUMMARY][reg][lyr]->GetXaxis()->GetBinCenter(xb);
        for (int yb = 1; yb != nBinsY; ++yb) {
          float cyb = m_pallErrs[SUMMARY][reg][lyr]->GetYaxis()->GetBinCenter(yb);

          for (int errType = SUMMARY; errType != N_ERRTYPES; ++errType) {
            bytestream_errs[errType][reg] += m_pallErrs[errType][reg][lyr]->GetBinContent(xb, yb);
            if (m_pallErrs[errType][reg][lyr]->GetBinContent(xb, yb) > 0) {
              m_allErrs[errType][reg][lyr]->Fill(cxb, cyb, 1);
              if (errType == SUMMARY && m_environment == AthenaMonManager::online) {
                m_summaryErrsRecent[reg][lyr]->Fill(cxb, cyb, 1);
              }
              tot_mod_bytestream_errs[errType][reg]++;
              tot_mod_bytestream_errs_layer[errType][reg][lyr]++;
            }else {
              m_allErrs[errType][reg][lyr]->Fill(cxb, cyb, 0);
              if (errType == SUMMARY && m_environment == AthenaMonManager::online) {
                m_summaryErrsRecent[reg][lyr]->Fill(cxb, cyb, 0);
              }
            }
            if (m_doPerLumiErrors && m_doErr2DPerLumiHists) {
              if (m_pallErrsPerLumi[errType][reg][lyr]->GetBinContent(xb, yb) > 0) {
                m_allErrsPerLumi[errType][reg][lyr]->Fill(cxb, cyb, 1);
              } else {
                m_allErrsPerLumi[errType][reg][lyr]->Fill(cxb, cyb, 0);
              }
            }
          }

          if (m_environment == AthenaMonManager::online) {
            // mf cast to (int) below to avoid compiler warnings... we do want int, right? Some code duplication
            // below...
            if (m_numberOfEvents % m_checkrate == 0 && m_runOnline == true) {
              for (int errType(0); errType != SUMMARY; ++errType) {
                int content = (int) m_pallErrs[errType][reg][lyr]->GetBinContent(xb, yb);
                int evt = m_numberOfEvents;
                m_allErrs[errType][reg][lyr]->Fill(cxb, cyb, 1, content);
                m_allErrs[errType][reg][lyr]->Fill(cxb, cyb, 0, evt - content);
              }
            }
          }
        } // Loop for BinY
      } // Loop for BinX
    } // Loop for Layer
  } // Loop for Region

  for (int errType = SUMMARY; errType != N_ERRTYPES; ++errType) {
    bytestream_errs[errType][iGEN] = bytestream_errs[errType][iBARREL] + bytestream_errs[errType][iECp] +
                                     bytestream_errs[errType][iECm];
    tot_mod_bytestream_errs[errType][iGEN] = tot_mod_bytestream_errs[errType][iBARREL] +
                                             tot_mod_bytestream_errs[errType][iECp] +
                                             tot_mod_bytestream_errs[errType][iECm];
  }
  if (tot_mod_bytestream_errs[SUMMARY][iGEN] < tot_mod_bytestream_errs[BADERR][iGEN]) {
    std::cout << "====== SCT:: total modules with bad errors is larger than modules with any errors, ALL    : "
              << "tot_mod_err = " << tot_mod_bytestream_errs[SUMMARY][iGEN] << " and tot_mod_baderr = " <<
      tot_mod_bytestream_errs[BADERR][iGEN] << std::endl;
  }
  for (int reg = 0; reg != NREGIONS_INC_GENERAL; ++reg) {
    for (int errType = SUMMARY; errType != N_ERRTYPES; ++errType) {
      m_ByteStreamVsLB[errType][reg]->Fill((int) current_lb, (bytestream_errs[errType][reg]));
    }
    m_LinksWithAnyErrorsVsLB[reg]->Fill((int) current_lb, (tot_mod_bytestream_errs[SUMMARY][reg]));
    m_LinksWithBadErrorsVsLB[reg]->Fill((int) current_lb, (tot_mod_bytestream_errs[BADERR][reg]));
    m_LinksWithLnkErrorsVsLB[reg]->Fill((int) current_lb, (tot_mod_bytestream_errs[LINKLEVEL][reg]));
    m_LinksWithRODErrorsVsLB[reg]->Fill((int) current_lb, (tot_mod_bytestream_errs[RODLEVEL][reg]));
  }
  for (int reg = 0; reg != NREGIONS_INC_GENERAL - 1; ++reg) {
    int nlyr = (reg == iBARREL) ? (N_BARRELSx2) : (N_ENDCAPSx2);
    for (int lyr = 0; lyr != nlyr; ++lyr) {
      m_LinksWithAnyErrorsVsLBLayer[reg][lyr]->Fill((int) current_lb,
                                                    (tot_mod_bytestream_errs_layer[SUMMARY][reg][lyr]));
      m_LinksWithBadErrorsVsLBLayer[reg][lyr]->Fill((int) current_lb,
                                                    (tot_mod_bytestream_errs_layer[BADERR][reg][lyr]));
      m_LinksWithLnkErrorsVsLBLayer[reg][lyr]->Fill((int) current_lb,
                                                    (tot_mod_bytestream_errs_layer[LINKLEVEL][reg][lyr]));
      m_LinksWithRODErrorsVsLBLayer[reg][lyr]->Fill((int) current_lb,
                                                    (tot_mod_bytestream_errs_layer[RODLEVEL][reg][lyr]));
    }
  }

  //  if(m_environment==AthenaMonManager::online){
  // Time Dependent SP plots only online
  nErrors_buf[nErrors_pos] = total_errors;
  nLinksWithErrors_buf[nErrors_pos] = tot_mod_bytestream_errs[SUMMARY][iGEN];
  nErrors_pos++;
  if (nErrors_pos == m_evtsbins) {
    nErrors_pos = 0;
  }
  if (m_numberOfEvents % m_checkrate == 0) {
    m_nErrors->Reset();
    Int_t latest_nErrors_pos = nErrors_pos;
    m_nLinksWithErrors->Reset();
    for (Int_t i = 1; i < m_evtsbins; i++) {
      if (latest_nErrors_pos == m_evtsbins) {
        latest_nErrors_pos = 0;
      }
      if (m_numberOfEvents < m_evtsbins) {
        if (i < nErrors_pos) {
          m_nErrors->SetBinContent(i, nErrors_buf[i]);
          m_nLinksWithErrors->SetBinContent(i, nLinksWithErrors_buf[i]);
        }else {
          m_nErrors->SetBinContent(i, 0);
          m_nLinksWithErrors->SetBinContent(i, 0);
        }
      } else {
        m_nErrors->SetBinContent(i, nErrors_buf[latest_nErrors_pos]);
        m_nLinksWithErrors->SetBinContent(i, nLinksWithErrors_buf[latest_nErrors_pos]);
        m_nErrors->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
        m_nLinksWithErrors->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
      }
      latest_nErrors_pos++;
      if (latest_nErrors_pos == m_evtsbins) {
        latest_nErrors_pos = 0;
      }
    }
  }
  // }5.12.2014
  m_previous_lb = current_lb;
  m_numberOfEvents++;
  m_numberOfEventsLumi++;
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTErrMonTool :: bookErrHistosHelper
// Avoids duplicate code in the bookErrHistosXXXX functions; added 8/9/09, Martin Flechl
// ====================================================================================================
StatusCode
SCTErrMonTool::bookErrHistosHelper(MonGroup &mg, TString name, TString title, TString titlehitmap,
                                   TProfile2D_LW * &tprof, TH2F_LW * &th, const int layer, const bool barrel) {
  ostringstream streamhitmap;

  streamhitmap << layer / 2 << "_" << layer % 2;

  name += streamhitmap.str();
  title += streamhitmap.str();
  titlehitmap += streamhitmap.str();

  if (barrel) {
    const float xlo = FIRST_ETA_BIN - 0.5;
    const float xhi = LAST_ETA_BIN + 0.5;
    const float ylo = FIRST_PHI_BIN - 0.5;
    const float yhi = LAST_PHI_BIN + 0.5;
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS, xlo, xhi, N_PHI_BINS, ylo, yhi);
    th = TH2F_LW::create(titlehitmap, title, N_ETA_BINS, xlo, xhi, N_PHI_BINS, ylo, yhi);
  } else {
    const float xlo = FIRST_ETA_BIN_EC - 0.5;
    const float xhi = LAST_ETA_BIN_EC + 0.5;
    const float ylo = FIRST_PHI_BIN_EC - 0.5;
    const float yhi = LAST_PHI_BIN_EC + 0.5;
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS_EC, xlo, xhi, N_PHI_BINS_EC, ylo, yhi);
    th = TH2F_LW::create(titlehitmap, title, N_ETA_BINS_EC, xlo, xhi, N_PHI_BINS_EC, ylo, yhi);
  }
  StatusCode sc0 = mg.regHist(th);
  StatusCode sc1 = mg.regHist(tprof);
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
SCTErrMonTool::bookErrHistosHelper(MonGroup &mg, TString name, TString title, TProfile2D_LW * &tprof, const int layer,
                                   const bool barrel) {
  ostringstream streamhitmap;

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

  StatusCode sc = mg.regHist(tprof);
  if (sc.isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================

//                       SCTErrMonTool :: bookErrHistos
// Book 1D and 2D Histograms of errors
// ====================================================================================================
// StatusCode SCTErrMonTool::bookErrHistos(bool newRunFlag(), bool newLumiBlockFlag()){
StatusCode
SCTErrMonTool::bookErrHistos() {
  const std::string m_errorsNames[] = {
    "ABCD", "Raw", "TimeOut", "LVL1ID", "BCID", "Preamble", "Formatter", "MaskedLinkLink", "RODClock", "TruncROD",
    "ROBFrag", "BSParse", "MissingLink", "MaskedROD", "MaskedLink", "summary", "badError", "LinkLevel", "RODLevel"
  };
  std::string m_errorsNamesMG[] = {
    "SCT/SCTB/errors", "SCT/SCTB/errors", "SCT/SCTB/errors", "SCT/SCTB/errors/LVL1ID", "SCT/SCTB/errors/BCID",
    "SCT/SCTB/errors/Preamble", "SCT/SCTB/errors/Formatter", "SCT/SCTB/errors/MaskedLinkLink",
    "SCT/SCTB/errors/RODClock", "SCT/SCTB/errors/TruncROD", "SCT/SCTB/errors/ROBFrag", "SCT/SCTB/errors",
    "SCT/SCTB/errors", "SCT/SCTB/errors/maskedROD", "SCT/SCTB/errors/MaskedLink", "SCT/SCTB/errors", "SCT/SCTB/errors",
    "SCT/SCTB/errors/LinkLevel", "SCT/SCTB/errors/RODLevel"
  };

  if (m_doPerLumiErrors) {
    MonGroup lumiErr(this, "SCT/SCTB/errors", lumiBlock, ATTRIB_UNMANAGED);
    if (ManagedMonitorToolBase::newLumiBlockFlag()) {
      std::string m_layerNames[] = {
        "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1"
      };
      m_numErrorsPerLumi[iBARREL] = TH2F_LW::create("NumErrsPerLumi",
                                                    "Total Number of Error Types for Layer per Lumi-Block",
                                                    n_lumiErrBins, -0.5, n_lumiErrBins - 0.5, N_BARRELSx2, -0.5,
                                                    N_BARRELSx2 - 0.5);
      if (lumiErr.regHist(m_numErrorsPerLumi[iBARREL]).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endmsg;
      }
      m_rateErrorsPerLumi[iBARREL] = TProfile2D_LW::create("RateErrorsPerLumi",
                                                           "Rate of Error Types for Layers per Lumi-Block",
                                                           n_lumiErrBins, -0.5, n_lumiErrBins - 0.5, N_BARRELSx2, -0.5,
                                                           N_BARRELSx2 - 0.5);
      if (lumiErr.regHist(m_rateErrorsPerLumi[iBARREL]).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book RateErrorsPerLumi" << endmsg;
      }
      for (unsigned int bin(0); bin < n_lumiErrBins; bin++) {
        m_numErrorsPerLumi[iBARREL]->GetXaxis()->SetBinLabel(bin + 1, m_errorsNames[bin].c_str());
        m_rateErrorsPerLumi[iBARREL]->GetXaxis()->SetBinLabel(bin + 1, m_errorsNames[bin].c_str());
      }
      for (unsigned int bin(0); bin < N_BARRELSx2; bin++) {
        m_numErrorsPerLumi[iBARREL]->GetYaxis()->SetBinLabel(bin + 1, m_layerNames[bin].c_str());
        m_rateErrorsPerLumi[iBARREL]->GetYaxis()->SetBinLabel(bin + 1, m_layerNames[bin].c_str());
      }
      // int limit = 2*n_barrels;
      bool somethingFailed(false);
      if (m_doErr2DPerLumiHists) {
        for (int layer(0); layer != N_BARRELSx2; ++layer) {
          for (int errType(0); errType != N_ERRTYPES; ++errType) {
            MonGroup lumiErr2(this, m_errorsNamesMG[errType], lumiBlock, ATTRIB_UNMANAGED);
            std::string name1 = m_errorsNames[errType] + "ErrsPerLumi";
            std::string title = m_errorsNames[errType] + " errors per lumiblock layer ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsPerLumi_";
            somethingFailed |=
              bookErrHistosHelper(lumiErr2, name1, title, name2, m_allErrsPerLumi[errType][iBARREL][layer],
                                  m_pallErrsPerLumi[errType][iBARREL][layer], layer).isFailure();
          }
        }
      }
      if (somethingFailed) {
        return StatusCode::FAILURE;
      }
    }
  }
  MonGroup err(this, "SCT/SCTB/errors", run, ATTRIB_UNMANAGED);
  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup MaskErrs(this, "SCT/GENERAL/errors", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    m_MaskedAllLinks = new TH1I("Masked Links", "Number of Masked Links for SCT,ECA,B,ECC", 4, -0.5, 3.5); // should
                                                                                                           // reorder to
                                                                                                           // C,B,A,
                                                                                                           // total
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(1, "All");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(2, "EndCapA");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(3, "Barrel");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(4, "EndCapC");
    if (MaskErrs.regHist(m_MaskedAllLinks).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book MaskedLinks" << endmsg;
    }
    std::string stem = m_stream + "/SCT/SCTB/errors/";
    int nbins = 50;
    // Book errors vs event numbers
    MonGroup Errors(this, "SCT/GENERAL/errors", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    m_nErrors = new TH1I("sct_errors_vs_en", "Number of Errors vs Event Number", m_evtsbins, 1, m_evtsbins + 1);
    m_nErrors->GetXaxis()->SetTitle("Event Number");
    m_nErrors->GetYaxis()->SetTitle("Num of Errors");
    m_nLinksWithErrors = new TH1I("sct_links_with_errors_vs_en", "Number of Links with Errors vs Event Number",
                                  m_evtsbins, 1, m_evtsbins + 1);
    m_nLinksWithErrors->GetXaxis()->SetTitle("Event Number");
    m_nLinksWithErrors->GetYaxis()->SetTitle("Num of Links with Errors");
    size_t nErrors_buf_size;
    nErrors_buf_size = m_evtsbins * sizeof(int);
    nErrors_buf = (int *) malloc(nErrors_buf_size);
    nLinksWithErrors_buf = (int *) malloc(nErrors_buf_size);
    nErrors_pos = 0;
    if (Errors.regHist(m_nErrors).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book nErrors vs event number hist" << endmsg;
    }
    if (Errors.regHist(m_nLinksWithErrors).isFailure()) {
      msg(MSG::WARNING) << "Couldn't book nLinksWithErrors vs event number hist" << endmsg;
    }
    // Book percentage error histograms
    m_firstHit = TH1F_LW::create("FirstHit", "Percentage of FirstHit errors", nbins, 0., 100.);
    m_firstHit->GetXaxis()->SetTitle("Percentage of FirstHit errors");
    m_secondHit = TH1F_LW::create("SecondHit", "Percentage of SecondHit errors", nbins, 0., 100.);
    m_secondHit->GetXaxis()->SetTitle("Percentage of SecondHit errors");
    if (err.regHist(m_firstHit).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endmsg;
    }
    if (err.regHist(m_secondHit).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endmsg;
    }
    bool somethingFailed(false);
    for (int layer(0); layer != N_BARRELSx2; ++layer) {
      for (int errType(0); errType != N_ERRTYPES; ++errType) {
        MonGroup err2(this, m_errorsNamesMG[errType], run, ATTRIB_UNMANAGED);
        std::string name1 = m_errorsNames[errType] + "Errs_";
        std::string title = m_errorsNames[errType] + " errors layer ";
        std::string name2 = std::string("T") + m_errorsNames[errType] + "Errs_";
        somethingFailed |= bookErrHistosHelper(err2, name1, title, name2, m_allErrs[errType][iBARREL][layer],
                                               m_pallErrs[errType][iBARREL][layer], layer).isFailure();
        m_allErrs[errType][iBARREL][layer]->GetXaxis()->SetTitle("Index in the direction of #eta");
        m_allErrs[errType][iBARREL][layer]->GetYaxis()->SetTitle("Index in the direction of #phi");
      }
      if (m_environment == AthenaMonManager::online) {
        somethingFailed |=
          bookErrHistosHelper(err, "summaryErrsRecent_", "summary recent Layer ", m_summaryErrsRecent[iBARREL][layer],
                              layer).isFailure();
      }
    }
    if (somethingFailed) {
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                 SCTErrMonTool :: bookPositiveEndCapErrHistos
// Book 1D and 2D Histograms of errors for positive endcap
// ====================================================================================================
StatusCode
SCTErrMonTool::bookPositiveEndCapErrHistos() {
  const std::string m_errorsNames[] = {
    "ABCD", "Raw", "TimeOut", "LVL1ID", "BCID", "Preamble", "Formatter", "MaskedLinkLink", "RODClock", "TruncROD",
    "ROBFrag", "BSParse", "MissingLink", "MaskedROD", "MaskedLink", "summary", "badError", "LinkLevel", "RODLevel"
  };
  std::string m_errorsNamesMG[] = {
    "SCT/SCTEA/errors", "SCT/SCTEA/errors", "SCT/SCTEA/errors", "SCT/SCTEA/errors/LVL1ID", "SCT/SCTEA/errors/BCID",
    "SCT/SCTEA/errors/Preamble", "SCT/SCTEA/errors/Formatter", "SCT/SCTEA/errors/MaskedLinkLink",
    "SCT/SCTEA/errors/RODClock", "SCT/SCTEA/errors/TruncROD", "SCT/SCTEA/errors/ROBFrag", "SCT/SCTEA/errors",
    "SCT/SCTEA/errors", "SCT/SCTEA/errors/maskedROD", "SCT/SCTEA/errors/MaskedLink", "SCT/SCTEA/errors",
    "SCT/SCTEA/errors", "SCT/SCTEA/errors/LinkLevel", "SCT/SCTEA/errors/RODLevel"
  };

  if (m_doPerLumiErrors) {
    MonGroup lumiErr(this, "SCT/SCTEA/errors", lumiBlock, ATTRIB_UNMANAGED);
    if (ManagedMonitorToolBase::newLumiBlockFlag()) {
      std::string m_layerNames[N_DISKSx2] = {
        "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1", "4_0", "4_1", "5_0", "5_1", "6_0", "6_1", "7_0", "7_1",
        "8_0", "8_1"
      };
      m_numErrorsPerLumi[iECp] = TH2F_LW::create("NumErrsPerLumi",
                                                 "Total Number of Error Types for Disk per Lumi-Block", n_lumiErrBins,
                                                 -0.5, n_lumiErrBins - 0.5, N_ENDCAPSx2, -0.5, N_ENDCAPSx2 - 0.5);
      if (lumiErr.regHist(m_numErrorsPerLumi[iECp]).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endmsg;
      }
      m_rateErrorsPerLumi[iECp] = TProfile2D_LW::create("RateErrorsPerLumi",
                                                        "Rate of Error Types for Disks per Lumi-Block", n_lumiErrBins,
                                                        -0.5, n_lumiErrBins - 0.5, N_ENDCAPSx2, -0.5,
                                                        N_ENDCAPSx2 - 0.5);
      if (lumiErr.regHist(m_rateErrorsPerLumi[iECp]).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book RateErrorsPerLumi" << endmsg;
      }
      for (unsigned int bin(0); bin < n_lumiErrBins; bin++) {
        m_numErrorsPerLumi[iECp]->GetXaxis()->SetBinLabel(bin + 1, m_errorsNames[bin].c_str());
        m_rateErrorsPerLumi[iECp]->GetXaxis()->SetBinLabel(bin + 1, m_errorsNames[bin].c_str());
      }
      for (unsigned int bin(0); bin != N_ENDCAPSx2; ++bin) {
        m_numErrorsPerLumi[iECp]->GetYaxis()->SetBinLabel(bin + 1, m_layerNames[bin].c_str());
        m_rateErrorsPerLumi[iECp]->GetYaxis()->SetBinLabel(bin + 1, m_layerNames[bin].c_str());
      }
      bool failedBooking(false);
      if (m_doErr2DPerLumiHists) {
        for (int layer(0); layer != N_DISKSx2; ++layer) {
          for (int errType(0); errType != N_ERRTYPES; ++errType) {
            MonGroup lumiErr2(this, m_errorsNamesMG[errType], lumiBlock, ATTRIB_UNMANAGED);// 07.01.2015
            std::string name1 = m_errorsNames[errType] + "ErrsECpPerLumi_";
            std::string title = m_errorsNames[errType] + " errors per lumiblock Disk ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECpPerLumi_";
            failedBooking |= bookErrHistosHelper(lumiErr2, name1, title, name2, m_allErrsPerLumi[errType][iECp][layer],
                                                 m_pallErrsPerLumi[errType][iECp][layer], layer).isFailure();
          }
        }
      }
      if (failedBooking) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "Something went wrong in bookPositiveEndCapErrHistos lumi part" << endmsg;
        }
      }
    }
  }
  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup err(this, "SCT/SCTEA/errors", run, ATTRIB_UNMANAGED);
    std::string stem = m_stream + "/SCT/SCTEA/errors/";
    int nbins = 50;
    m_firstHit_ECp =
      TH1F_LW::create("FirstHit_ECp", "Percentage of FirstHit errors in positive endcap", nbins, 0., 100.);
    m_firstHit_ECp->GetXaxis()->SetTitle("Percentage of FirstHit errors");
    m_secondHit_ECp = TH1F_LW::create("SecondHit_ECp", "Percentage of SecondHit errors in positive endcap", nbins, 0.,
                                      100.);
    m_secondHit_ECp->GetXaxis()->SetTitle("Percentage of SecondHit errors");
    if (err.regHist(m_firstHit_ECp).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endmsg;
    }
    if (err.regHist(m_secondHit_ECp).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endmsg;
    }

    bool failedBooking(false);
    for (int layer(0); layer != N_DISKSx2; ++layer) {
      for (int errType(0); errType != N_ERRTYPES; ++errType) {
        MonGroup err2(this, m_errorsNamesMG[errType], run, ATTRIB_UNMANAGED);
        std::string name1 = m_errorsNames[errType] + "ErrsECp_";
        std::string title = m_errorsNames[errType] + " errors Disk ";
        std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECp_";
        failedBooking |=
          bookErrHistosHelper(err2, name1, title, name2, m_allErrs[errType][iECp][layer],
                              m_pallErrs[errType][iECp][layer], layer, false).isFailure();
        m_allErrs[errType][iECp][layer]->GetXaxis()->SetTitle("Index in the direction of #eta");
        m_allErrs[errType][iECp][layer]->GetYaxis()->SetTitle("Index in the direction of #phi");
      }

      if (m_environment == AthenaMonManager::online) {
        failedBooking |=
          bookErrHistosHelper(err, "summaryErrsRecentECp_", "summary recent Disk ", m_summaryErrsRecent[iECp][layer],
                              layer, false).isFailure();
      }
    }
    if (failedBooking) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Something went wrong in bookPositiveEndCapErrHistos" << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                 SCTErrMonTool :: bookNegativeEndCapErrHistos
// Book 1D and 2D Histograms of errors for negative endcap
// ====================================================================================================
// StatusCode SCTErrMonTool::bookNegativeEndCapErrHistos(bool isNewRun, bool isNewLumiBlock){
StatusCode
SCTErrMonTool::bookNegativeEndCapErrHistos() {
  const std::string m_errorsNames[] = {
    "ABCD", "Raw", "TimeOut", "LVL1ID", "BCID", "Preamble", "Formatter", "MaskedLinkLink", "RODClock", "TruncROD",
    "ROBFrag", "BSParse", "MissingLink", "MaskedROD", "MaskedLink", "summary", "badError", "LinkLevel", "RODLevel"
  };
  std::string m_errorsNamesMG[] = {
    "SCT/SCTEC/errors", "SCT/SCTEC/errors", "SCT/SCTEC/errors", "SCT/SCTEC/errors/LVL1ID", "SCT/SCTEC/errors/BCID",
    "SCT/SCTEC/errors/Preamble", "SCT/SCTEC/errors/Formatter", "SCT/SCTEC/errors/MaskedLinkLink",
    "SCT/SCTEC/errors/RODClock", "SCT/SCTEC/errors/TruncROD", "SCT/SCTEC/errors/ROBFrag", "SCT/SCTEC/errors",
    "SCT/SCTEC/errors", "SCT/SCTEC/errors/maskedROD", "SCT/SCTEC/errors/MaskedLink", "SCT/SCTEC/errors",
    "SCT/SCTEC/errors", "SCT/SCTEC/errors/LinkLevel", "SCT/SCTEC/errors/RODLevel"
  };

  if (m_doPerLumiErrors) {
    MonGroup lumiErr(this, "SCT/SCTEC/errors", lumiBlock, ATTRIB_UNMANAGED);
    if (ManagedMonitorToolBase::newLumiBlockFlag()) {
      std::string m_layerNames[N_DISKSx2] = {
        "0_0", "0_1", "1_0", "1_1", "2_0", "2_1", "3_0", "3_1", "4_0", "4_1", "5_0", "5_1", "6_0", "6_1", "7_0", "7_1",
        "8_0", "8_1"
      };
      m_numErrorsPerLumi[iECm] = TH2F_LW::create("NumErrsPerLumi",
                                                 "Total Number of Error Types for Disk per Lumi-Block", n_lumiErrBins,
                                                 -0.5, n_lumiErrBins - 0.5, N_ENDCAPSx2, -0.5, N_ENDCAPSx2 - 0.5);
      if (lumiErr.regHist(m_numErrorsPerLumi[iECm]).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book NumErrsPerLumi" << endmsg;
      }
      m_rateErrorsPerLumi[iECm] = TProfile2D_LW::create("RateErrorsPerLumi",
                                                        "Rate of Error Types for Disks per Lumi-Block", n_lumiErrBins,
                                                        -0.5, n_lumiErrBins - 0.5, N_ENDCAPSx2, -0.5,
                                                        N_ENDCAPSx2 - 0.5);
      if (lumiErr.regHist(m_rateErrorsPerLumi[iECm]).isFailure()) {
        msg(MSG::WARNING) << "Couldn't book RateErrorsPerLumi" << endmsg;
      }
      for (unsigned int bin(0); bin < n_lumiErrBins; bin++) {
        m_numErrorsPerLumi[iECm]->GetXaxis()->SetBinLabel(bin + 1, m_errorsNames[bin].c_str());
        m_rateErrorsPerLumi[iECm]->GetXaxis()->SetBinLabel(bin + 1, m_errorsNames[bin].c_str());
      }
      for (unsigned int bin(0); bin < N_ENDCAPSx2; bin++) {
        m_numErrorsPerLumi[iECm]->GetYaxis()->SetBinLabel(bin + 1, m_layerNames[bin].c_str());
        m_rateErrorsPerLumi[iECm]->GetYaxis()->SetBinLabel(bin + 1, m_layerNames[bin].c_str());
      }
      bool failedBooking(false);
      if (m_doErr2DPerLumiHists) {
        for (int layer(0); layer != N_DISKSx2; ++layer) {
          for (int errType(0); errType != N_ERRTYPES; ++errType) {
            MonGroup lumiErr2(this, m_errorsNamesMG[errType], lumiBlock, ATTRIB_UNMANAGED);// 07.01.2015
            std::string name1 = m_errorsNames[errType] + "ErrsECmPerLumi_";
            std::string title = m_errorsNames[errType] + " errors per lumiblock layer ";
            std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECmPerLumi_";
            failedBooking |= bookErrHistosHelper(lumiErr2, name1, title, name2, m_allErrsPerLumi[errType][iECm][layer],
                                                 m_pallErrsPerLumi[errType][iECm][layer], layer).isFailure();
          }
        }
      }
      if (failedBooking and msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Something went wrong in bookNegativeEndCapErrHistos lumi part" << endmsg;
      }
    }
  }
  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup err(this, "SCT/SCTEC/errors", run, ATTRIB_UNMANAGED);
    std::string stem = m_stream + "/SCT/SCTEC/errors/";
    int nbins = 50;
    m_firstHit_ECm =
      TH1F_LW::create("FirstHit_ECm", "Percentage of FirstHit errors in negative endcap", nbins, 0., 100.);
    m_firstHit_ECm->GetXaxis()->SetTitle("Percentage of FirstHit errors");
    m_secondHit_ECm = TH1F_LW::create("SecondHit_ECm", "Percentage of SecondHit errors in negative endcap", nbins, 0.,
                                      100.);
    m_secondHit_ECm->GetXaxis()->SetTitle("Percentage of SecondHit errors");
    if (err.regHist(m_firstHit_ECm).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << "FirstHit" << endmsg;
    }
    if (err.regHist(m_secondHit_ECm).isFailure()) {
      msg(MSG::WARNING) << "Cannot book Histogram:" << "SecondHit" << endmsg;
    }

    // int limit = 2*n_disks;
    bool failedBooking(false);
    for (int layer(0); layer != N_DISKSx2; ++layer) {
      for (int errType(0); errType != N_ERRTYPES; ++errType) {
        MonGroup err2(this, m_errorsNamesMG[errType], run, ATTRIB_UNMANAGED);
        std::string name1 = m_errorsNames[errType] + "ErrsECm_";
        std::string title = m_errorsNames[errType] + " errors Disk ";
        std::string name2 = std::string("T") + m_errorsNames[errType] + "ErrsECm_";
        failedBooking |=
          bookErrHistosHelper(err2, name1, title, name2, m_allErrs[errType][iECm][layer],
                              m_pallErrs[errType][iECm][layer], layer, false).isFailure();
        m_allErrs[errType][iECm][layer]->GetXaxis()->SetTitle("Index in the direction of #eta");
        m_allErrs[errType][iECm][layer]->GetYaxis()->SetTitle("Index in the direction of #phi");
      }
      if (m_environment == AthenaMonManager::online) {
        failedBooking |=
          bookErrHistosHelper(err, "summaryErrsRecentECm_", "summary recent Disk ", m_summaryErrsRecent[iECm][layer],
                              layer, false).isFailure();
      }
    }
    if (failedBooking) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Something went wrong in bookNegativeEndCapErrHistos" << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                         SCTErrMonTool :: bookConfMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::bookConfMaps() {
  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup ConfMapsExpert(this, "SCT/SCTB/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    MonGroup ConfHist[NREGIONS_INC_GENERAL] = {
      MonGroup(this, "SCT/SCTB/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED),
      MonGroup(this, "SCT/SCTEA/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED),
      MonGroup(this, "SCT/SCTEC/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED),
      MonGroup(this, "SCT/GENERAL/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED)
    };

    std::string streamName = streamNameFunction()->getStreamName(this, ConfHist[iGEN], "", false); // Hidetoshi Otono
                                                                                                   // 14.03.05
    m_path = streamName.substr(0, streamName.rfind("SCT/GENERAL/Conf"));
    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "Global Path :" << m_path << endmsg;
    }

    std::string m_SummaryBinNames[] = {
      "Mod Out", "Flagged Links", "Masked Links", "Errors", "Inefficient", "Noisy"
    };
    std::string m_ConfigurationBinNames[] = {
      "Modules", "Link 0", "Link 1", "Chips", "Strips (10^{2})"
    };
    std::string m_ConfigurationOnlineBinNames[] = {
      "Mod Out", "Flagged Links", "Masked Links", "Errors"
    };
    std::string m_ConfigurationEffBinNames[] = {
      "Ineff B", "Ineff EA", "Ineff EC", "ALL"
    };

    std::string m_ConfigurationNoiseBinNames[] = {
      "Noisy B", "Noisy EA", "Noisy EC", "ALL"
    };
    string hNumEndcap[N_DISKS] = {
      "0", "1", "2", "3", "4", "5", "6", "7", "8"
    };

    m_p2DmapHistoVectorAll[iBARREL].clear();
    VecProf2_t *storageVectors[] = {
      &m_pnoiseoccupancymapHistoVectorECC, &m_pnoiseoccupancymapHistoVectorBar, &m_pnoiseoccupancymapHistoVectorECA
    };
    VecProf2_t *storageVectorsSide0[] = {
      &m_pnoiseoccupancymapHistoVectorECCSide0, &m_pnoiseoccupancymapHistoVectorBarSide0,
      &m_pnoiseoccupancymapHistoVectorECASide0
    };
    VecProf2_t *storageVectorsSide1[] = {
      &m_pnoiseoccupancymapHistoVectorECCSide1, &m_pnoiseoccupancymapHistoVectorBarSide1,
      &m_pnoiseoccupancymapHistoVectorECASide1
    };
    const int bec[] = {
      -2, 0, 2
    };
    const unsigned int limits[] = {
      N_DISKS, N_BARRELS, N_DISKS
    };
    for (int systemIndex = 0; systemIndex < 3; systemIndex++) {
      for (auto prof2: *storageVectors[systemIndex]) {
        LWHist::safeDelete(prof2);
      }
      (storageVectors[systemIndex])->clear();
      for (auto prof2: *storageVectorsSide0[systemIndex]) {
        LWHist::safeDelete(prof2);
      }
      (storageVectorsSide0[systemIndex])->clear();
      for (auto prof2: *storageVectorsSide1[systemIndex]) {
        LWHist::safeDelete(prof2);
      }
      (storageVectorsSide1[systemIndex])->clear();

      for (unsigned int i = 0; i != limits[systemIndex]; ++i) {
        const string streamhitmap = "noiseoccupancymap" + abbreviations[systemIndex] + "_" + hNumEndcap[i];
        const string streamhitmapside0 = "noiseoccupancymap" + abbreviations[systemIndex] + "_" + hNumEndcap[i] +
                                         "_side0";
        const string streamhitmapside1 = "noiseoccupancymap" + abbreviations[systemIndex] + "_" + hNumEndcap[i] +
                                         "_side1";
        std::string histotitle = "SCT Noise Occupancy map for " + names[systemIndex] + ": " + hNumEndcap[i];
        std::string histotitleside0 = "SCT Noise Occupancy map for " + names[systemIndex] + ": " + hNumEndcap[i] +
                                      " side0";
        std::string histotitleside1 = "SCT Noise Occupancy map for " + names[systemIndex] + ": " + hNumEndcap[i] +
                                      " side1";
        prof2Factory(streamhitmap, histotitle, bec[systemIndex], *(storageVectors[systemIndex]));
        prof2Factory(streamhitmapside0, histotitleside0, bec[systemIndex], *(storageVectorsSide0[systemIndex]));
        prof2Factory(streamhitmapside1, histotitleside1, bec[systemIndex], *(storageVectorsSide1[systemIndex]));
      }
    }
    // book 2D "noise" maps, containing hits that aren't associated to tracks
    for (int i = 0; i < N_BARRELSx2; ++i) {
      ostringstream stream2dmap;
      stream2dmap << "modulemap_" << i / 2 << "_" << i % 2;
      TH2F_LW *hitsHisto_tmp = TH2F_LW::create(TString(stream2dmap.str()), // path
                                               Form("Module Out of Configuration : Layer %d Side %d", i / 2, i % 2), // title
                                               N_ETA_BINS, FIRST_ETA_BIN - 0.5, LAST_ETA_BIN + 0.5,
                                               N_PHI_BINS, FIRST_PHI_BIN - 0.5, LAST_PHI_BIN + 0.5);
      hitsHisto_tmp->GetXaxis()->SetTitle("Index in the direction of #eta");
      hitsHisto_tmp->GetYaxis()->SetTitle("Index in the direction of #phi");

      if (ConfMapsExpert.regHist(hitsHisto_tmp).isFailure()) {
        msg(MSG::WARNING) << "Cannot book Histogram:" << stream2dmap.str() << endmsg;
      }
      m_p2DmapHistoVectorAll[iBARREL].push_back(hitsHisto_tmp);
    }
    if (m_makeConfHisto or testOffline) {
      m_DetailedConfiguration = TProfile_LW::create("SCTConfDetails", "Exclusion from the Configuration",
                                                    ConfbinsDetailed, -0.5, ConfbinsDetailed - 0.5);
      if (ConfHist[iGEN].regHist(m_DetailedConfiguration).isFailure()) {
        msg(MSG::WARNING) << "Cannot book Histogram:SCTConfDetails" << endmsg;
      }

      TString conftitle[4] = {
        "SCTConfBarrel", "SCTConfEndcapA", "SCTConfEndcapC", "SCTConf"
      };
      TString conftitleRN[4] = {
        "SCTConfBarrelRN", "SCTConfEndcapARN", "SCTConfEndcapCRN", "SCTConfRN"
      };
      TString conflabel[4] = {
        "Num of Problematic Module in Barrel", "Num of Problematic Module in EndcapA",
        "Num of Problematic Module in EndcapC", "Num of Problematic Module in All Region"
      };
      TString confonlinetitle[4] = {
        "SCTOnlineConfBarrel", "SCTOnlineConfEndcapA", "SCTOnlineConfEndcapC", "SCTOnlineConf"
      };
      TString confefftitle = "SCTEffConf";

      TString regLabel[4] = {
        "Barrel", "EndcapA", "EndcapC", ""
      };
      TString regTitle[4] = {
        "Barrel", "EndcapA", "EndcapC", "All Region"
      };

      TString confnoisetitle = "SCTNoiseConf";
      TString confnoisetitle_recent = "SCTNoiseConfRecent";

      for (int reg = 0; reg != NREGIONS_INC_GENERAL; ++reg) {
        m_Conf[reg] = TProfile_LW::create(conftitle[reg], conflabel[reg], Confbins, -0.5, Confbins - 0.5);// 30.11.2014
        for (int bin = 0; bin < Confbins; bin++) {
          m_Conf[reg]->GetXaxis()->SetBinLabel(bin + 1, m_SummaryBinNames[bin].c_str());
        }
        m_ConfRN[reg] = TProfile_LW::create(conftitleRN[reg], conflabel[reg], Confbins, -0.5, Confbins - 0.5);// 30.11.2014
        for (int bin = 0; bin < Confbins; bin++) {
          m_ConfRN[reg]->GetXaxis()->SetBinLabel(bin + 1, m_SummaryBinNames[bin].c_str());
        }
        const int conf_online_bins = 4;
        if (m_environment == AthenaMonManager::online or testOffline) {
          m_ConfOnline[reg] = TProfile_LW::create(confonlinetitle[reg], conflabel[reg] + " Online", conf_online_bins,
                                                  -0.5, conf_online_bins - 0.5);// 30.11.2014
          for (int bin = 0; bin < conf_online_bins; bin++) {
            m_ConfOnline[reg]->GetXaxis()->SetBinLabel(bin + 1, m_ConfigurationOnlineBinNames[bin].c_str());
          }
        }
        for (int errType = 0; errType != SUMMARY; ++errType) {
          m_ByteStreamVsLB[errType][reg] = TProfile_LW::create("SCT" + errorsString(errType) + "VsLbs" + regLabel[reg],
                                                               "Ave. " + errorsString(errType) + " per LB in " + regTitle[reg],
							       SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
          m_ByteStreamVsLB[errType][reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ByteStreamVsLB[errType][reg]->GetYaxis()->SetTitle("Num of " + errorsString(errType));
          m_ByteStreamWithSctFlagVsLB[errType][reg] = TProfile_LW::create("SCT" + errorsString(errType) + "WithSctFlagVsLbs" + regLabel[reg],
									  "Ave. " + errorsString(errType) + " with SCT flag per LB in " + regTitle[reg],
									  SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
          m_ByteStreamWithSctFlagVsLB[errType][reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ByteStreamWithSctFlagVsLB[errType][reg]->GetYaxis()->SetTitle("Num of " + errorsString(errType));
        }
        for (int errType = SUMMARY; errType != N_ERRTYPES; ++errType) {
          m_ByteStreamVsLB[errType][reg] = TProfile_LW::create("SCT" + errorsString(errType) + regLabel[reg],
                                                               "Ave. " + errorsString(errType) + " per LB in " + regTitle[reg],
							       SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
          m_ByteStreamVsLB[errType][reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ByteStreamVsLB[errType][reg]->GetYaxis()->SetTitle(errorsString(errType));
        }

        m_LinksWithAnyErrorsVsLB[reg] = TProfile_LW::create("SCTModulesWithErrors" + regLabel[reg],
                                                            "Ave. num of links with errors per LB in " + regTitle[reg],
                                                            SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
        m_LinksWithAnyErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
        m_LinksWithBadErrorsVsLB[reg] = TProfile_LW::create("SCTModulesWithBadErrors" + regLabel[reg],
                                                            "Ave. num of links with bad errors per LB in " +
                                                            regTitle[reg], SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
        m_LinksWithBadErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
        m_LinksWithLnkErrorsVsLB[reg] = TProfile_LW::create("SCTModulesWithLinkLevelErrors" + regLabel[reg],
                                                            "Ave. num of links with Link-level errors per LB in " +
                                                            regTitle[reg], SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
        m_LinksWithLnkErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
        m_LinksWithRODErrorsVsLB[reg] = TProfile_LW::create("SCTModulesWithRODLevelErrors" + regLabel[reg],
                                                            "Ave. num of links with ROD-level errors per LB in " +
                                                            regTitle[reg], SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
        m_LinksWithRODErrorsVsLB[reg]->GetXaxis()->SetTitle("LumiBlock");
      }

      for (int reg = 0; reg != NREGIONS_INC_GENERAL - 1; ++reg) {
        int nlyr = (reg == iBARREL) ? (N_BARRELSx2) : (N_ENDCAPSx2);
        for (int lyr = 0; lyr != nlyr; ++lyr) {
          ostringstream streamlayer;
          streamlayer << lyr / 2 << "_" << lyr % 2;
          streamlayer.str();
          m_LinksWithAnyErrorsVsLBLayer[reg][lyr] = TProfile_LW::create(
            "SCTLinksWithErrors" + regLabel[reg] + "lyr" + streamlayer.str(),
            "Ave. num of links with errors per LB in " +
            regTitle[reg] + " layer" + streamlayer.str(), SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
          m_LinksWithAnyErrorsVsLBLayer[reg][lyr]->GetXaxis()->SetTitle("LumiBlock");
          m_LinksWithBadErrorsVsLBLayer[reg][lyr] = TProfile_LW::create(
            "SCTLinksWithBadErrors" + regLabel[reg] + "lyr" + streamlayer.str(),
            "Ave. num of links with bad errors per LB in " +
            regTitle[reg] + " layer" + streamlayer.str(), SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
          m_LinksWithBadErrorsVsLBLayer[reg][lyr]->GetXaxis()->SetTitle("LumiBlock");
          m_LinksWithLnkErrorsVsLBLayer[reg][lyr] = TProfile_LW::create(
            "SCTLinksWithLinkLevelErrors" + regLabel[reg] + "lyr" + streamlayer.str(),
            "Ave. num of links with Link-level errors per LB in " +
            regTitle[reg] + " layer" + streamlayer.str(), SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
          m_LinksWithLnkErrorsVsLBLayer[reg][lyr]->GetXaxis()->SetTitle("LumiBlock");
          m_LinksWithRODErrorsVsLBLayer[reg][lyr] = TProfile_LW::create(
            "SCTLinksWithRODLevelErrors" + regLabel[reg] + "lyr" + streamlayer.str(),
            "Ave. num of links with ROD-level errors per LB in " +
            regTitle[reg] + " layer" + streamlayer.str(), SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
          m_LinksWithRODErrorsVsLBLayer[reg][lyr]->GetXaxis()->SetTitle("LumiBlock");
        }
      }

      m_NumberOfSCTFlagErrorsVsLB = TH1F_LW::create("NumberOfSCTFlagErrorsVsLB", "Num of SCT Flag errors per LB ",
                                                    SCT_Monitoring::NBINS_LBs, 0.5, SCT_Monitoring::NBINS_LBs + 0.5);
      m_NumberOfSCTFlagErrorsVsLB->GetXaxis()->SetTitle("LumiBlock");

      m_NumberOfEventsVsLB = TH1F_LW::create("NumberOfEventsVsLB", "Num of events per LB ", SCT_Monitoring::NBINS_LBs, 0.5,
                                             SCT_Monitoring::NBINS_LBs + 0.5);
      m_NumberOfEventsVsLB->GetXaxis()->SetTitle("LumiBlock");

      const int conf_noise_bins = 4;
      const int conf_eff_bins = 4;

      if (m_environment == AthenaMonManager::online or testOffline) {
        m_ConfEffOnline = new TProfile(confefftitle, "Number of Inefficient Modules Online", conf_eff_bins, -0.5,
                                       conf_eff_bins - 0.5);
        for (int bin = 0; bin < conf_eff_bins; bin++) {
          m_ConfEffOnline->GetXaxis()->SetBinLabel(bin + 1, m_ConfigurationEffBinNames[bin].c_str());
        }
        m_ConfNoiseOnline = TProfile_LW::create(confnoisetitle, "Number of Noisy Modules Online", conf_noise_bins, -0.5,
                                                conf_noise_bins - 0.5);
        for (int bin = 0; bin < conf_noise_bins; bin++) {
          m_ConfNoiseOnline->GetXaxis()->SetBinLabel(bin + 1, m_ConfigurationNoiseBinNames[bin].c_str());
        }
        m_ConfNoiseOnlineRecent = TProfile_LW::create(confnoisetitle_recent, "Number of Noisy Modules Online Recent",
                                                      conf_noise_bins, -0.5, conf_noise_bins - 0.5);

        for (int bin = 0; bin < conf_noise_bins; bin++) {
          m_ConfNoiseOnlineRecent->GetXaxis()->SetBinLabel(bin + 1, m_ConfigurationNoiseBinNames[bin].c_str());
        }
      }
      for (int bin = 0; bin < ConfbinsDetailed; bin++) {
        m_DetailedConfiguration->GetXaxis()->SetBinLabel(bin + 1, m_ConfigurationBinNames[bin].c_str());
      }

      for (int reg = 0; reg != NREGIONS_INC_GENERAL; ++reg) {
        if (ConfHist[reg].regHist(m_Conf[reg]).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endmsg;
        }
        if (ConfHist[reg].regHist(m_ConfRN[reg]).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTConf" << endmsg;
        }

        if (m_environment == AthenaMonManager::online or testOffline) {
          if (ConfHist[reg].regHist(m_ConfOnline[reg]).isFailure()) {
            msg(MSG::WARNING) << "Cannot book Histogram:SCTConfOnline" << endmsg;
          }
        }

        for (int errType = 0; errType != N_ERRTYPES; ++errType) {
          if (ConfHist[reg].regHist(m_ByteStreamVsLB[errType][reg]).isFailure()) {
            msg(MSG::WARNING) << "Cannot book Histogram:" + errorsString(errType) << endmsg;
          }
        }

        for (int errType = 0; errType != SUMMARY; ++errType) {
          if (ConfHist[reg].regHist(m_ByteStreamWithSctFlagVsLB[errType][reg]).isFailure()) {
            msg(MSG::WARNING) << "Cannot book Histogram:" + errorsString(errType) + " WithSctFlag" << endmsg;
          }
        }

        if (ConfHist[reg].regHist(m_LinksWithAnyErrorsVsLB[reg]).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithErrors" << endmsg;
        }
        if (ConfHist[reg].regHist(m_LinksWithBadErrorsVsLB[reg]).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithBadErrors" << endmsg;
        }
        if (ConfHist[reg].regHist(m_LinksWithLnkErrorsVsLB[reg]).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithLinkLevelErrors" << endmsg;
        }
        if (ConfHist[reg].regHist(m_LinksWithRODErrorsVsLB[reg]).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTModulesWithRODLevelErrors" << endmsg;
        }
      }

      for (int reg = 0; reg != NREGIONS_INC_GENERAL - 1; ++reg) {
        int nlyr = (reg == iBARREL) ? (N_BARRELSx2) : (N_ENDCAPSx2);
        for (int lyr = 0; lyr != nlyr; ++lyr) {
          if (ConfHist[reg].regHist(m_LinksWithAnyErrorsVsLBLayer[reg][lyr]).isFailure()) {
            msg(MSG::WARNING) << "Cannot book Histogram:SCTLinksWithErrorsLayer" << endmsg;
          }
          if (ConfHist[reg].regHist(m_LinksWithBadErrorsVsLBLayer[reg][lyr]).isFailure()) {
            msg(MSG::WARNING) << "Cannot book Histogram:SCTLinksWithBadErrorsLayer" << endmsg;
          }
          if (ConfHist[reg].regHist(m_LinksWithLnkErrorsVsLBLayer[reg][lyr]).isFailure()) {
            msg(MSG::WARNING) << "Cannot book Histogram:SCTLinksWithLinkLevelErrorsLayer" << endmsg;
          }
          if (ConfHist[reg].regHist(m_LinksWithRODErrorsVsLBLayer[reg][lyr]).isFailure()) {
            msg(MSG::WARNING) << "Cannot book Histogram:SCTLinksWithRODLevelErrorsLayer" << endmsg;
          }
        }
      }

      if (ConfHist[iGEN].regHist(m_NumberOfSCTFlagErrorsVsLB).isFailure()) {
        msg(MSG::WARNING) << "Cannot book Histogram:NumberOfSCTFlagErrors" << endmsg;
      }

      if (ConfHist[iGEN].regHist(m_NumberOfEventsVsLB).isFailure()) {
        msg(MSG::WARNING) << "Cannot book Histogram:NumberOfEventsVsLB" << endmsg;
      }

      if (m_environment == AthenaMonManager::online or testOffline) {
        if (ConfHist[iGEN].regHist(m_ConfEffOnline).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTConfEffOnline" << endmsg;
        }
        if (ConfHist[iGEN].regHist(m_ConfNoiseOnline).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTConfNoiseOnline" << endmsg;
        }
        if (ConfHist[iGEN].regHist(m_ConfNoiseOnlineRecent).isFailure()) {
          msg(MSG::WARNING) << "Cannot book Histogram:SCTConfNoiseOnlineRecent" << endmsg;
        }
      }
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Finished registering Conf Histograms :" << m_path << endmsg;
      }
    }// end if m_makeConfHisto or testOffline
  }// end if isNewRun
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                   SCTErrMonTool :: bookPositiveEndCapConfMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::bookPositiveEndCapConfMaps() {
  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup PlusEndCapConfMaps(this, "SCT/SCTEA/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    m_p2DmapHistoVectorAll[iECp].clear();
    // book 2D "noise" maps, containing hits that aren't associated to tracks, for positive endcap
    for (int i(0); i != N_ENDCAPSx2; ++i) {
      ostringstream stream2dmap;
      stream2dmap << "modulemapECp_" << i / 2 << "_" << i % 2;
      TH2F_LW *hitsHisto_tmp = TH2F_LW::create(TString(stream2dmap.str()),         // path
                                               Form("Module Out of Configuration : Disk %d Side %d", i / 2, i % 2),  // title
                                               N_ETA_BINS_EC, FIRST_ETA_BIN_EC - 0.5, LAST_ETA_BIN_EC + 0.5,              //
                                                                                                                          // X
                                                                                                                          // num
                                                                                                                          // bins,
                                                                                                                          // X_lo,
                                                                                                                          // X_hi
                                               N_PHI_BINS_EC, FIRST_PHI_BIN_EC - 0.5, LAST_PHI_BIN_EC + 0.5);             //
                                                                                                                          // Y
                                                                                                                          // num
                                                                                                                          // bins,
                                                                                                                          // Y_lo,
                                                                                                                          // Y_hi
      hitsHisto_tmp->GetXaxis()->SetTitle("Index in the direction of #eta");
      hitsHisto_tmp->GetYaxis()->SetTitle("Index in the direction of #phi");
      if (PlusEndCapConfMaps.regHist(hitsHisto_tmp).isFailure()) {
        msg(MSG::WARNING) << "Cannot book Histogram:" << stream2dmap.str() << endmsg;
      }
      m_p2DmapHistoVectorAll[iECp].push_back(hitsHisto_tmp);
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                    SCTErrMonTool :: bookNegativeEndCapConfMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::bookNegativeEndCapConfMaps() {
  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup MinusEndCapConfMaps(this, "SCT/SCTEC/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED);
    m_p2DmapHistoVectorAll[iECm].clear();
    // book 2D "noise" maps, containing hits that aren't associated to tracks, for negative endcap
    for (int i = 0; i != N_ENDCAPSx2; ++i) {
      ostringstream stream2dmap;
      stream2dmap << "modulemapEcm_" << i / 2 << "_" << i % 2;
      TH2F_LW *hitsHisto_tmp = TH2F_LW::create(TString(stream2dmap.str()),         // path
                                               Form("Module Out of Configuration : Disk %d Side %d", i / 2, i % 2),  // title
                                               N_ETA_BINS_EC, FIRST_ETA_BIN_EC - 0.5, LAST_ETA_BIN_EC + 0.5,
                                               N_PHI_BINS_EC, FIRST_PHI_BIN_EC - 0.5, LAST_PHI_BIN_EC + 0.5);
      hitsHisto_tmp->GetXaxis()->SetTitle("Index in the direction of #eta");
      hitsHisto_tmp->GetYaxis()->SetTitle("Index in the direction of #phi");

      if (MinusEndCapConfMaps.regHist(hitsHisto_tmp).isFailure()) {
        msg(MSG::WARNING) << "Cannot book Histogram:" << stream2dmap.str() << endmsg;
      }
      m_p2DmapHistoVectorAll[iECm].push_back(hitsHisto_tmp);
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: fillCondDBMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::fillCondDBMaps() {
  int Flagged[4] = {
    0
  };
  int MOut[4] = {
    0
  };
  int MaskedAllLinks[4] = {
    (int) m_MaskedAllLinks->GetBinContent(3), (int) m_MaskedAllLinks->GetBinContent(2),
    (int) m_MaskedAllLinks->GetBinContent(4), (int) m_MaskedAllLinks->GetBinContent(1)
  };
  int ModErr[4] = {
    0
  };
  int InEffModules[4] = {
    0
  };
  int NoisyModules[4] = {
    0
  };
  int NoisyModulesRecent[4] = {
    0
  };
  int RNoisyModules[4] = {
    0
  };

  bool failedbooking = false;
  TH2 *hitsHisto_tmp[2];

  // Pointers are deleted by regHist() method
  std::string stem;
  SCT_ID::const_id_iterator planeIterator(m_pSCTHelper->wafer_begin());
  SCT_ID::const_id_iterator planeEnd = m_pSCTHelper->wafer_end();

  for (; planeIterator not_eq planeEnd; ++planeIterator) {
    Identifier planeId(*planeIterator);
    int eta = m_pSCTHelper->eta_module(planeId);
    int phi = m_pSCTHelper->phi_module(planeId);
    int barrel_ec = m_pSCTHelper->barrel_ec(planeId);
    int element = 2 * m_pSCTHelper->layer_disk(planeId) + m_pSCTHelper->side(planeId);
    int IN = (m_ConfigurationSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
    if (m_flaggedSvc->numBadIds() != 0) {
      Flagged[iGEN] += (m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
      if (barrel_ec == BARREL) {
        Flagged[iBARREL] += (m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
      }
      if (barrel_ec == ENDCAP_A) {
        Flagged[iECp] += (m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
      }
      if (barrel_ec == ENDCAP_C) {
        Flagged[iECm] += (m_flaggedSvc->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1);
      }
    }
    if (m_pSCTHelper->side(planeId) == 0) {
      if (IN == 1) {
        MOut[iGEN]++;
        if (barrel_ec == BARREL) {
          MOut[iBARREL]++;
        }
        if (barrel_ec == ENDCAP_A) {
          MOut[iECp]++;
        }
        if (barrel_ec == ENDCAP_C) {
          MOut[iECm]++;
        }
      }
    }
    // danger: no check that barrel_ec is valid
    int y = phi + 1;
    int x = (barrel_ec == BARREL) ? (eta + 7) : (eta + 1);
    int iRegion = iBARREL;
    if (barrel_ec == ENDCAP_A) {
      iRegion = iECp;
    }
    if (barrel_ec == ENDCAP_C) {
      iRegion = iECm;
    }
    m_p2DmapHistoVectorAll[iRegion][element]->SetBinContent(x, y, IN);
  }

  // calculate ratio noise
  for (int j = 0; j < n_mod[GENERAL_INDEX]; j++) {
    int layer = nLayer[j];
    if (nZero[j] != 0) {
      ratio = 0;
      ratioside0 = 0;
      ratioside1 = 0;
      ratio = 1E5 * calculateNoiseOccupancyUsingRatioMethod(nOne[j], nZero[j]);
      ratioside0 = 1E5 * calculateOneSideNoiseOccupancyUsingRatioMethod(nOneSide0[j], nZero[j]);
      ratioside1 = 1E5 * calculateOneSideNoiseOccupancyUsingRatioMethod(nOneSide1[j], nZero[j]);
      if (isBarrel(j)) {
        m_pnoiseoccupancymapHistoVectorBar[layer]->Fill(nEta[j], nPhi[j], ratio);
        m_pnoiseoccupancymapHistoVectorBarSide0[layer]->Fill(nEta[j], nPhi[j], ratioside0);
        m_pnoiseoccupancymapHistoVectorBarSide1[layer]->Fill(nEta[j], nPhi[j], ratioside1);
      }
      if (isEndcapA(j)) {
        m_pnoiseoccupancymapHistoVectorECA[layer]->Fill(nEta[j], nPhi[j], ratio);
        m_pnoiseoccupancymapHistoVectorECASide0[layer]->Fill(nEta[j], nPhi[j], ratioside0);
        m_pnoiseoccupancymapHistoVectorECASide1[layer]->Fill(nEta[j], nPhi[j], ratioside1);
      }
      if (isEndcapC(j)) {
        m_pnoiseoccupancymapHistoVectorECC[layer]->Fill(nEta[j], nPhi[j], ratio);
        m_pnoiseoccupancymapHistoVectorECCSide0[layer]->Fill(nEta[j], nPhi[j], ratioside0);
        m_pnoiseoccupancymapHistoVectorECCSide1[layer]->Fill(nEta[j], nPhi[j], ratioside1);
      }
    }
  }

  // barrel region
  for (int lyr = 0; lyr != N_BARRELS; ++lyr) {
    // Check for the number of Modules with more than m_errThreshold error rate
    const int xbins(m_allErrs[SUMMARY][iBARREL][lyr]->GetNbinsX() + 1);
    const int ybins(m_allErrs[SUMMARY][iBARREL][lyr]->GetNbinsY() + 1);
    for (int xb = 1; xb != xbins; ++xb) {
      for (int yb = 1; yb != ybins; ++yb) {
        if (m_allErrs[SUMMARY][iBARREL][2 * lyr]->GetBinContent(xb,
                                                                yb) > m_errThreshold ||
            m_allErrs[SUMMARY][iBARREL][2 * lyr + 1]->GetBinContent(xb, yb) > m_errThreshold) {
          ModErr[iGEN]++;
          ModErr[iBARREL]++;
        }
      }
    }

    // noisy modules
    failedbooking = getHisto(lyr, iBARREL, 0, hitsHisto_tmp);
    if (failedbooking == false) {
      countNoisyModules(iBARREL, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
    }
    countNoisyModulesRN(iBARREL, iGEN, m_pnoiseoccupancymapHistoVectorBar[lyr], m_noiseThreshold, RNoisyModules);
    // noisy modules recent
    if (m_environment == AthenaMonManager::online) {
      failedbooking = getHistoRecent(lyr, iBARREL, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(iBARREL, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
      }
    }

    // ineff modules
    failedbooking = getHisto(lyr, iBARREL, 1, hitsHisto_tmp);
    if (failedbooking == false) {
      if (m_environment !=
          AthenaMonManager::online or thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or testOffline) {
        const int xbins(hitsHisto_tmp[0]->GetNbinsX() + 1);
        const int ybins(hitsHisto_tmp[0]->GetNbinsY() + 1);
        // if (testOffline) printContents(hitsHisto_tmp[0]);
        for (int xb(1); xb != xbins; ++xb) {
          for (int yb(1); yb != ybins; ++yb) {
            float outOfConfig = m_p2DmapHistoVectorAll[iBARREL][2 * lyr]->GetBinContent(xb, yb);
            if (outOfConfig < 1) { // i.e. its in the configuration
              int nSides = numberOfInefficientSides(hitsHisto_tmp, xb, yb, 1 - m_effThreshold);
              InEffModules[iGEN] += nSides;
              InEffModules[iBARREL] += nSides;
            }
          }
        }
      }
    }
  }


  // endcaps
  for (int lyr = 0; lyr != N_ENDCAPS; ++lyr) {
    // module errors
    const int xbins(m_allErrs[SUMMARY][iECp][lyr]->GetNbinsX() + 1);
    const int ybins(m_allErrs[SUMMARY][iECp][lyr]->GetNbinsY() + 1);
    for (int xb = 1; xb != xbins; xb++) {
      for (int yb = 1; yb != ybins; yb++) {
        if (m_allErrs[SUMMARY][iECp][2 * lyr]->GetBinContent(xb,
                                                             yb) > m_errThreshold ||
            m_allErrs[SUMMARY][iECp][2 * lyr + 1]->GetBinContent(xb, yb) > m_errThreshold) {
          ModErr[iGEN]++;
          ModErr[iECp]++;
        }
        if (m_allErrs[SUMMARY][iECm][2 * lyr]->GetBinContent(xb,
                                                             yb) > m_errThreshold ||
            m_allErrs[SUMMARY][iECm][2 * lyr + 1]->GetBinContent(xb, yb) > m_errThreshold) {
          ModErr[iGEN]++;
          ModErr[iECm]++;
        }
      }
    }

    // noisy modules
    // endcap C
    failedbooking = getHisto(lyr, iECm, 0, hitsHisto_tmp);
    if (failedbooking == false) {
      countNoisyModules(iECm, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
    }
    countNoisyModulesRN(iECm, iGEN, m_pnoiseoccupancymapHistoVectorECC[lyr], m_noiseThreshold, RNoisyModules);
    // endcap A
    failedbooking = getHisto(lyr, iECp, 0, hitsHisto_tmp);
    if (failedbooking == false) {
      countNoisyModules(iECp, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
    }
    countNoisyModulesRN(iECp, iGEN, m_pnoiseoccupancymapHistoVectorECA[lyr], m_noiseThreshold, RNoisyModules);
    // noisy modules recent
    if (m_environment == AthenaMonManager::online) {
      failedbooking = getHistoRecent(lyr, iECm, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(iECm, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
      }
      //
      failedbooking = getHistoRecent(lyr, iECp, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(iECp, iGEN, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
      }
    }
    // ineff modules
    failedbooking = getHisto(lyr, iECm, 1, hitsHisto_tmp);
    if (failedbooking == false) {
      if (m_environment !=
          AthenaMonManager::online or thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or testOffline) {
        const int xbins(hitsHisto_tmp[0]->GetNbinsX() + 1);
        const int ybins(hitsHisto_tmp[0]->GetNbinsY() + 1);
        for (int xb = 1; xb != xbins; ++xb) {
          for (int yb = 1; yb != ybins; ++yb) {
            if (m_p2DmapHistoVectorAll[iECm][2 * lyr]->GetBinContent(xb, yb) < 1) {
              int nSides = numberOfInefficientSides(hitsHisto_tmp, xb, yb, 1 - m_effThreshold);
              InEffModules[iGEN] += nSides;
              InEffModules[iECm] += nSides;
            }
          }
        }
      }
    }


    failedbooking = getHisto(lyr, iECp, 1, hitsHisto_tmp);
    if (failedbooking == false) {
      if (m_environment !=
          AthenaMonManager::online or thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or testOffline) {
        const int xbins(hitsHisto_tmp[0]->GetNbinsX() + 1);
        const int ybins(hitsHisto_tmp[0]->GetNbinsY() + 1);
        for (int xb = 1; xb != xbins; ++xb) {
          for (int yb = 1; yb != ybins; ++yb) {
            if (m_p2DmapHistoVectorAll[iECp][2 * lyr]->GetBinContent(xb, yb) < 1) {
              int nSides = numberOfInefficientSides(hitsHisto_tmp, xb, yb, 1 - m_effThreshold);
              InEffModules[iGEN] += nSides;
              InEffModules[iECp] += nSides;
            }
          }
        }
      }
    }
  }


  if (m_makeConfHisto) {
    for (int reg = 0; reg <= 3; ++reg) {
      m_Conf[reg]->Fill(0., float (MOut[reg]));
      m_ConfRN[reg]->Fill(0., float (MOut[reg]));
      m_Conf[reg]->Fill(1., float (Flagged[reg]));
      m_ConfRN[reg]->Fill(1., float (Flagged[reg]));
      m_Conf[reg]->Fill(2., float (MaskedAllLinks[reg]));
      m_ConfRN[reg]->Fill(2., float (MaskedAllLinks[reg]));
      m_Conf[reg]->Fill(3., float (ModErr[reg]));
      m_ConfRN[reg]->Fill(3., float (ModErr[reg]));
      m_Conf[reg]->Fill(4., float (InEffModules[reg]));
      m_ConfRN[reg]->Fill(4., float (InEffModules[reg]));
      m_Conf[reg]->Fill(5., float (NoisyModules[reg]));
      m_ConfRN[reg]->Fill(5., float (RNoisyModules[reg]));
      if (m_environment == AthenaMonManager::online) {
        m_ConfOnline[reg]->Fill(0., float (MOut[reg]));
        m_ConfOnline[reg]->Fill(1., float (Flagged[reg]));
        m_ConfOnline[reg]->Fill(2., float (MaskedAllLinks[reg]));
        m_ConfOnline[reg]->Fill(3., float (ModErr[reg]));
      }
    }
    if (m_environment == AthenaMonManager::online or testOffline) {
      m_ConfEffOnline->Reset("ICE");
      for (int i(0); i != 4; ++i) {
        const float f(i);
        m_ConfEffOnline->Fill(f, float (InEffModules[i]));
        m_ConfNoiseOnline->Fill(f, float (NoisyModules[i]));
        m_ConfNoiseOnlineRecent->Fill(f, float (NoisyModulesRecent[i]));
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
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Inside fillConfigurationDetails()" << endmsg;
  }
  unsigned int nBadMods = m_ConfigurationSvc->badModules()->size(); // bad modules
  std::map<Identifier, std::pair<bool, bool> > *badLinks = m_ConfigurationSvc->badLinks(); // bad links
  unsigned int nBadLink0(0), nBadLink1(0), nBadLinkBoth(0);
  for (auto link: *badLinks) {
    std::pair<bool, bool> status = link.second;
    if (status.first == false && status.second == true) {
      ++nBadLink0;
    }
    if (status.first == true && status.second == false) {
      ++nBadLink1;
    }
    if (status.first == false && status.second == false) {
      ++nBadLinkBoth;
    }
  }

  std::map<Identifier, unsigned int> *badChips = m_ConfigurationSvc->badChips(); // bad chips
  unsigned int nBadChips(0);
  for (auto chip : *badChips) {
    unsigned int status = chip.second;
    for (unsigned int i(0); i != CHIPS_PER_MODULE; i++) {
      nBadChips += ((status & (1 << i)) == 0 ? 0 : 1);
    }
  }

  std::set<Identifier> badStripsAll; // bad strips
  m_ConfigurationSvc->badStrips(badStripsAll);
  unsigned int nBadStrips = badStripsAll.size();

  std::set<Identifier> badStripsExclusive; // bad strips w/o bad modules and chips
  m_ConfigurationSvc->badStrips(badStripsExclusive, true, true);
  int nBadStripsExclusive = badStripsExclusive.size();
  int nBadStripsExclusiveBEC[] = {
    0, 0, 0
  };
  for (auto strip: badStripsExclusive) {
    int bec(m_pSCTHelper->barrel_ec(strip));
    nBadStripsExclusiveBEC[SCT_Monitoring::bec2Index(bec)] += 1;
  }

  m_DetailedConfiguration->Fill(0., float (nBadMods));
  m_DetailedConfiguration->Fill(1., float (nBadLink0));
  m_DetailedConfiguration->Fill(2., float (nBadLink1));
  m_DetailedConfiguration->Fill(3., float (nBadChips));
  m_DetailedConfiguration->Fill(4., float (nBadStripsExclusive) / 100.);

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------" << endmsg;
    msg(MSG::DEBUG) << "Number of bad modules                          = " << nBadMods << endmsg;
    msg(MSG::DEBUG) << "Number of bad link 0                           = " << nBadLink0 << endmsg;
    msg(MSG::DEBUG) << "Number of bad link 1                           = " << nBadLink1 << endmsg;
    msg(MSG::DEBUG) << "Number of bad link both                        = " << nBadLinkBoth << endmsg;
    msg(MSG::DEBUG) << "Number of bad chips                            = " << nBadChips << endmsg;
    msg(MSG::DEBUG) << "Number of bad strips                           = " << nBadStrips << endmsg;
    msg(MSG::DEBUG) << "Number of bad strips exclusive                 = " << nBadStripsExclusive << endmsg;
    msg(MSG::DEBUG) << "Number of bad strips exclusive (ECC, B, ECA)   = "
                    << nBadStripsExclusiveBEC[0] << ", "
                    << nBadStripsExclusiveBEC[1] << ", "
                    << nBadStripsExclusiveBEC[2] << ", " << endmsg;
    msg(MSG::DEBUG) << "-----------------------------------------------------------------------" << endmsg;
  }
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
    for (int reg = 0; reg <= 3; ++reg) {
      m_Conf[reg]->Reset();
      m_ConfRN[reg]->Reset();
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
SCTErrMonTool::getHisto(const int layer, const int reg, const int type, TH2 *histo[2]) {
  const string trm[2][N_REGIONS] = {
    {"SCT/SCTB/Noise/noiseoccupancymap_", "SCT/SCTEA/Noise/noiseoccupancymapECp_",
     "SCT/SCTEC/Noise/noiseoccupancymapECm_"},
    {"SCT/SCTB/eff/ineff_", "SCT/SCTEA/eff/ineffp_", "SCT/SCTEC/eff/ineffm_"}
  };
  const string trm_trig[2][N_REGIONS] = {
    {"SCT/SCTB/Noise/noiseoccupancymaptrigger_", "SCT/SCTEA/Noise/noiseoccupancymaptriggerECp_",
     "SCT/SCTEC/Noise/noiseoccupancymaptriggerECm_"},
    {"SCT/SCTB/eff/ineff_", "SCT/SCTEA/eff/ineffp_", "SCT/SCTEC/eff/ineffm_"}
  };
  string shname1 = m_path + trm[type][reg] + std::to_string(layer) + "_0";
  string shname2 = m_path + trm[type][reg] + std::to_string(layer) + "_1";

  if (testOffline or((m_environment != AthenaMonManager::online) &&
                     (AthenaMonManager::dataType() != AthenaMonManager::cosmics))) {
    shname1 = m_path + trm_trig[type][reg] + std::to_string(layer) + "_0";
    shname2 = m_path + trm_trig[type][reg] + std::to_string(layer) + "_1";
  }
  bool failedBooking(false);
  failedBooking |= m_thistSvc->getHist(shname1, histo[0]).isFailure();
  failedBooking |= m_thistSvc->getHist(shname2, histo[1]).isFailure();
  if (failedBooking) {
    msg(MSG::WARNING) << "Error getting Histogram from ITHistSvc : " << shname1 << "," << shname2 << endmsg;
    return true;
  }
  return false;
}

// ====================================================================================================
//                          SCTErrMonTool :: getHistoRecent, Dan Damiani  21/7/2011
// ====================================================================================================
bool
SCTErrMonTool::getHistoRecent(const int layer, const int reg, const int type, TH2 *histo[2]) {
  const string trm[1][N_REGIONS] = {
    {"SCT/SCTB/Noise/noiseoccupancymaprecent_", "SCT/SCTEA/Noise/noiseoccupancymaprecentECp_",
     "SCT/SCTEC/Noise/noiseoccupancymaprecentECm_"}
  };
  string shname1 = m_path + trm[type][reg] + std::to_string(layer) + "_0";
  string shname2 = m_path + trm[type][reg] + std::to_string(layer) + "_1";
  bool failedBooking(false);

  failedBooking |= m_thistSvc->getHist(shname1, histo[0]).isFailure();
  failedBooking |= m_thistSvc->getHist(shname2, histo[1]).isFailure();
  if (failedBooking) {
    msg(MSG::WARNING) << "Error getting Histogram from ITHistSvc : " << shname1 << "," << shname2 << endmsg;
    return true;
  }
  return false;
}

SCTErrMonTool::Prof2_t
SCTErrMonTool::prof2Factory(const std::string &name, const std::string &title, const unsigned int &bec,
                            VecProf2_t &storageVector) {
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
  Prof2_t tmp = TProfile2D_LW::create(TString(name), TString(
                                        title), nEta, firstEta - 0.5, lastEta + 0.5, nPhi, firstPhi - 0.5,
                                      lastPhi + 0.5);
  tmp->SetXTitle("Index in the direction of #eta");
  tmp->SetYTitle("Index in the direction of #phi");
  storageVector.push_back(tmp);
  return tmp;
}

float
SCTErrMonTool::calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide) {
  float div, rat;
  constexpr int nstrips(768);

  if (numberZeroSide != 0) {
    div = numberOneSide / numberZeroSide;
    rat = (div / (div + 2)) / nstrips;
    return rat;
  }else {
    return -1;
  }
}

float
SCTErrMonTool::calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide) {
  float div, rat;
  constexpr int nstrips(768);

  if (numberZeroSide != 0) {
    div = numberOneSide / numberZeroSide;
    rat = (div / (div + 1)) / nstrips;
    return rat;
  }else {
    return -1;
  }
}

bool
SCTErrMonTool::isEndcapC(const int moduleNumber) {
  bool moduleinEndcapC = false;

  if (0 <= moduleNumber && moduleNumber < f_mod[BARREL_INDEX]) {
    moduleinEndcapC = true;
  }
  return moduleinEndcapC;
}

bool
SCTErrMonTool::isBarrel(const int moduleNumber) {
  bool moduleinBarrel = false;

  if (f_mod[BARREL_INDEX] <= moduleNumber && moduleNumber < f_mod[ENDCAP_A_INDEX]) {
    moduleinBarrel = true;
  }
  return moduleinBarrel;
}

bool
SCTErrMonTool::isEndcapA(const int moduleNumber) {
  bool moduleinEndcapA = false;

  if (f_mod[ENDCAP_A_INDEX] <= moduleNumber && moduleNumber < n_mod[GENERAL_INDEX]) {
    moduleinEndcapA = true;
  }
  return moduleinEndcapA;
}
