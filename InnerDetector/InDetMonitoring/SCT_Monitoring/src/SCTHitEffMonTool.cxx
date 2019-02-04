/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** Tool to measure the intrinsic single hit efficiency in the SCT
 */

// std and STL includes
#include <cmath>
// #include <ostream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <limits>       // std::numeric_limits
#include <array>



// Gaudi
#include "GaudiKernel/IChronoStatSvc.h"

// ROOT
#include "TH2I.h"
#include "TProfile2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"

// Athena
#include "AthenaKernel/errorcheck.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"

// InDet
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
// Conditions
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
// Track
#include "TrkSurfaces/Surface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkTrackSummary/TrackSummary.h"

// SCT
#include "SCT_Monitoring/SCTHitEffMonTool.h"
#include "SCT_NameFormatter.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
// macros (ugh)
#define DEBUG(x) ATH_MSG_DEBUG(x)
#define INFO(x) ATH_MSG_INFO(x)
#define WARNING(x) ATH_MSG_WARNING(x)
#define ERROR(x) ATH_MSG_ERROR(x)
#define VERBOSE(x) ATH_MSG_VERBOSE(x)

using namespace SCT_Monitoring;



namespace {// anonymous namespace for functions at file scope
  const bool testOffline(false);

  std::string histogramPath[] = {
    "SCT/SCTEC/eff", "SCT/SCTB/eff", "SCT/SCTEA/eff", "SCT/GENERAL/eff"
  };
  std::string histogramPathRe[] = {
    "SCT/SCTEC/eff/perLumiBlock", "SCT/SCTB/eff/perLumiBlock", "SCT/SCTEA/eff/perLumiBlock"
  };// 23.01.2015

  template< typename T > Identifier
  surfaceOnTrackIdentifier(const T &tsos, const bool useTrackParameters = true) {
    Identifier result(0); // default constructor produces invalid value
    const Trk::MeasurementBase *mesb = tsos->measurementOnTrack();

    if (mesb and mesb->associatedSurface().associatedDetectorElement()) {
      result = mesb->associatedSurface().associatedDetectorElement()->identify();
    } else if (useTrackParameters and tsos->trackParameters()) {
      //       result = tsos->trackParameters()->associatedSurface()->associatedDetectorElementIdentifier();
      result = tsos->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
    }
    return result;
  }

  constexpr Double_t radianDegrees(180. / 3.1415927);

  float
  amgPseudoRapidity(const Amg::Vector3D &position) {
    float pseudo(0.);
    float ma = position.mag(), dz = position.z();

    if (ma == 0) {
      return 0;
    }
    if (ma == dz) {
      return std::numeric_limits<float>::max();
    }
    if (ma == -dz) {
      return std::numeric_limits<float>::min();
    }
    pseudo = 0.5 * log((ma + dz) / (ma - dz));
    return pseudo;
  }
}// namespace end

using namespace SCT_Monitoring;
using std::ostringstream;
using std::string;

// Constructor with parameters:
SCTHitEffMonTool::SCTHitEffMonTool(const string &type, const string &name, const IInterface *parent) :
  ManagedMonitorToolBase(type, name, parent),
  m_TrackName(std::string("ResolvedSCTTracks")),// original track collection
  m_chrono(nullptr),
  m_badChips(nullptr),
  m_fieldServiceHandle("AtlasFieldSvc", name),
  m_bunchCrossingTool("Trig::BunchCrossingTool/BunchCrossingTool", this),
  m_DetectorMode(1), // Barrel = 1, endcap =2, both =3
  m_RunningMode(2),
  m_minPixelHits(-1),
  m_minTRTHits(-1),
  m_minSCTHits(-1),    // If all 3 of these -1, defaults depend on cosmic/not
  m_minOtherHits(6),
  m_maxPhiAngle(40.), // Degrees, 100 implies no cut.
  m_maxChi2(3.),
  m_maxD0(10.), // mm of D0
  m_minPt(1000.), // minimu pt in MeV/c
  m_effdistcut(2.),
  m_maxZ0sinTheta(0.),
  m_maxTracks(250.),
  m_insideOutOnly(false),
  m_usemasks(true),
  m_detailed(false),
  m_superDetailed(false),
  m_isCosmic(false),
  m_isSim(false),
  m_useTRTPhase(false),
  m_useSCTorTRT(false),
  m_requireEnclosingHits(false),
  m_requireOtherFace(false),
  m_requireGuardRing(false), // should be returned to true
  m_vetoBadChips(true),
  m_chronotime(false),
  m_useIDGlobal(false),
  m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator", this),
  m_rotcreator("InDet::SCT_ClusterOnTrackTool/SCT_ClusterOnTrackTool", this),
  m_holeSearchTool("InDet::InDetTrackHoleSearchTool", this),
  m_Eff_Total(nullptr),
  m_Eff_TotalBCID(nullptr),
  m_Eff_hashCodeHisto(nullptr),
  m_Eff_LumiBlockHisto_Total(nullptr),
  m_effdistribution(nullptr),
  m_effHashLumiB(nullptr),
  m_mNHitHisto(nullptr),
  m_barrelNHitHisto(nullptr),
  m_pNHitHisto(nullptr),
  m_SCTNHitHisto(nullptr),
  m_trtNHitHisto(nullptr),
  m_pixelNHitHisto(nullptr),
  m_PtTkHisto(nullptr),
  m_etaTkHisto(nullptr),
  m_d0TkHisto(nullptr),
  m_d0PrecTkHisto(nullptr),
  m_nTrkHisto(nullptr),
  m_nTrkParsHisto(nullptr),
  m_nTrkGoodHisto(nullptr),
  m_LumiBlock(nullptr),
  m_z0TkHisto(nullptr),
  m_hashCodeHisto(nullptr),
  m_badModFineMap(nullptr),
  m_badModMap(nullptr),
  m_badChipMap(nullptr),
  m_pixelId(nullptr),
  m_sctId(nullptr),
  m_trtId(nullptr),
  m_comTimeName(std::string("TRT_Phase")),
  m_eventInfoKey(std::string("EventInfo")){
  declareProperty("TrackName", m_TrackName);
  declareProperty("IsCosmic", m_isCosmic);
  declareProperty("IsSim", m_isSim);
  declareProperty("UseMasks", m_usemasks);
  declareProperty("Detailed", m_detailed);
  declareProperty("SuperDetailed", m_superDetailed);
  declareProperty("UseTRTPhase", m_useTRTPhase);
  declareProperty("MaxPhiAngle", m_maxPhiAngle);
  declareProperty("MinTRTHits", m_minTRTHits);
  declareProperty("MinSCTHits", m_minSCTHits);
  declareProperty("MinPixelHits", m_minPixelHits);
  declareProperty("UseSCTorTRT", m_useSCTorTRT);
  declareProperty("MinOtherHits", m_minOtherHits);
  declareProperty("InsideOutOnly", m_insideOutOnly);
  declareProperty("MaxChi2", m_maxChi2);
  declareProperty("Maxd0", m_maxD0);
  declareProperty("MaxZ0sinTheta", m_maxZ0sinTheta);
  declareProperty("MaxTracks", m_maxTracks);
  declareProperty("MinPt", m_minPt);
  declareProperty("RequireEnclosingHits", m_requireEnclosingHits);
  declareProperty("RequireOtherFace", m_requireOtherFace);
  declareProperty("RequireGuardRing", m_requireGuardRing);
  declareProperty("VetoBadChips", m_vetoBadChips);
  declareProperty("LookAtDatabase", m_usedatabase);
  declareProperty("DetectorMode", m_DetectorMode);
  declareProperty("RunningMode", m_RunningMode);
  declareProperty("effDistanceCut", m_effdistcut);
  declareProperty("ChronoTime", m_chronotime);
  declareProperty("SCT_ClusterContainer", m_sctContainerName = std::string("SCT_Clusters"));
  declareProperty("ROTCreator", m_rotcreator);
  declareProperty("HoleSearch", m_holeSearchTool);
  declareProperty("ResPullCalc", m_residualPullCalculator);
  declareProperty("useIDGlobal", m_useIDGlobal);
  declareProperty("MagFieldSvc", m_fieldServiceHandle);
  declareProperty("BunchCrossingTool", m_bunchCrossingTool);

  m_countEvent = 0;

  m_Eff_summaryHisto.fill(0);
  m_Eff_summaryHistoFirstBCID.fill(0);
  m_Eff_summaryHisto_old.fill(0);
  m_holesPerTrackHisto.fill(0);
  m_holesDistPerTrackHisto.fill(0);
  m_Unas_summaryHisto.fill(0);
  m_Eff_etaHisto.fill(0);
  m_Eff_etaPhiCutHisto.fill(0);
  m_Eff_ptiHisto.fill(0);
  m_Eff_ptHisto.fill(0);
  m_Unas_ptiHisto.fill(0);
  m_Eff_phiHisto.fill(0);
  m_Eff_phiEtaCutHisto.fill(0);
  m_Eff_phiHistoFinal.fill(0);
  m_Unas_phiHisto.fill(0);
  m_Eff_d0Histo.fill(0);
  m_Eff_d0PrecHisto.fill(0);
  m_Eff_z0Histo.fill(0);
  m_Eff_xlocHisto.fill(0);
  m_Eff_ylocHistos.fill(0);
  m_Unas_xlocHisto.fill(0);
  m_Unas_ylocHisto.fill(0);
  m_Eff_nSCTHisto.fill(0);
  m_Eff_nTRTHisto.fill(0);
  m_Eff_nOtherHisto.fill(0);
  m_Eff_otherFaceHisto.fill(0);
  m_Unas_otherFaceHisto.fill(0);
  m_Eff_timecorHisto.fill(0);
  m_probEnclosedHisto.fill(0);
  m_Eff_SelectionHisto.fill(0);
  m_Eff_EventHisto.fill(0);
  m_Eff_LumiBlockHisto.fill(0);
  m_Eff_chi2Histo.fill(0);
  m_Eff_chi2HistoFinal.fill(0);
  m_chi2vPhiHisto.fill(0);
  m_EventHisto.fill(0);
  m_SelectionHisto.fill(0);
  m_ResidualHisto.fill(0);
  m_ResidualUnasHisto.fill(0);
  m_ResidualMissHisto.fill(0);
  m_timecorHisto.fill(0);
  m_nOtherHisto.fill(0);
  m_etaTkUsedHisto.fill(0);
  m_phiTkUsedHisto.fill(0);
  m_ptiTkUsedHisto.fill(0);
  m_ptTkUsedHisto.fill(0);
  m_d0TkUsedHisto.fill(0);
  m_d0PrecTkUsedHisto.fill(0);
  m_nTrkUsedHisto.fill(0);
  m_z0TkUsedHisto.fill(0);
  m_phiLocalHisto.fill(0);
  m_phiLocalCutHisto.fill(0);
  m_chi2Histo.fill(0);
  m_localHitXHisto.fill(0);
  m_localHitYHistos.fill(0);
  m_localHitHisto.fill(0);
  m_localMissHisto.fill(0);
  m_localUnasHisto.fill(0);
  m_localHitGHisto.fill(0);
  m_TwoSidesResiduals.fill(0);
  m_TwoSidesResidualsIneff.fill(0);
  m_chi2ResidualHisto.fill(0);
  m_accMap.fill(0);
  m_accPhysMap.fill(0);
  m_inEffChip.fill(0);
  m_inEffStrip.fill(0);
  m_badModPerSide.fill(0);
  m_Eff_summaryIncBadMod.fill(0);
  m_Eff_nTrk.fill(0);
  m_Eff_nGoodTrk.fill(0);
  // assume zero inefficiency
  for (unsigned int i(0); i != m_ineffMap.size(); ++i) {
    m_ineffMap[i].fill(0);
  }
  for (unsigned int i(0); i != m_effMap.size(); ++i) {
    m_effMap[i].fill(0);
  }
  for (unsigned int i(0); i != m_effLumiBlock.size(); ++i) {
    m_effLumiBlock[i].fill(0);
  }
  for (unsigned int i(0); i != m_layerResidualHistos.size(); ++i) {
    m_layerResidualHistos[i].fill(0);
  }
  for (unsigned int i(0); i != m_xlResidualHisto.size(); ++i) {
    m_xlResidualHisto[i].fill(0);
  }
  for (unsigned int i(0); i != m_xlResidualE0Histo.size(); ++i) {
    m_xlResidualE0Histo[i].fill(0);
  }
  for (unsigned int i(0); i != m_xlResidualE1Histo.size(); ++i) {
    m_xlResidualE1Histo[i].fill(0);
  }
  for (unsigned int i(0); i != m_xlResidualUnasHisto.size(); ++i) {
    m_xlResidualUnasHisto[i].fill(0);
  }
}

/*---------------------------------------------------------*/
StatusCode
SCTHitEffMonTool::initialize() {
  INFO("Initializing SCTHitEffMonTool");

  CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));
  CHECK(detStore()->retrieve(m_pixelId, "PixelID"));
  CHECK(detStore()->retrieve(m_trtId, "TRT_ID"));

  if (m_chronotime) {
    CHECK(service("ChronoStatSvc", m_chrono));
  }
  CHECK(ManagedMonitorToolBase::initialize());
  CHECK(m_holeSearchTool.retrieve());
  INFO("Retrieved hole search tool " << m_holeSearchTool);
  CHECK(m_residualPullCalculator.retrieve());
  INFO("Retrieved pull calculator tool " << m_residualPullCalculator);
  CHECK(m_rotcreator.retrieve());
  INFO("Retrieved tool " << m_rotcreator);
  CHECK(m_fieldServiceHandle.retrieve());
  CHECK(m_bunchCrossingTool.retrieve());
  INFO("Retrieved BunchCrossing tool " << m_bunchCrossingTool);

  m_path = (m_useIDGlobal) ? ("/InDetGlobal/") : ("");

  if (m_superDetailed) {
    m_detailed = true;
  }

  if ((m_minSCTHits == -1)and(m_minTRTHits == -1) and(m_minOtherHits == -1)) {
    if (m_isCosmic) {
      m_minTRTHits = 45;
      m_minSCTHits = 7;
      m_minOtherHits = 5;
    } else {
      m_minTRTHits = 30;
      m_minSCTHits = 4;
      m_minOtherHits = 3;
    }
  }

  ATH_CHECK( m_comTimeName.initialize(m_useTRTPhase or m_isCosmic) );
  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_TrackName.initialize() );
  ATH_CHECK( m_sctContainerName.initialize() );

  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode
SCTHitEffMonTool::bookHistograms() {
  //  if (not isNewRun) return StatusCode::SUCCESS;                                                                   //
  // hidetoshi 14.01.22
  if (not newRunFlag()) {
    return StatusCode::SUCCESS;                                                                         // hidetoshi
                                                                                                        // 14.01.22
  }
  if (m_isCosmic) {
    WARNING("Running on cosmics: releasing d0 cut and forcing use of TRT timing");
  }
  if (not m_fieldServiceHandle->solenoidOn()) {
    WARNING("Running with solenoid off: releasing pT cut");
    m_minPt = -1.;
  }
  if (newRunFlag()) {
    m_badChips = m_configConditions->badChips();
    INFO("Found " << m_badChips->size() << " bad chips");
    for (std::map<Identifier, unsigned int>::const_iterator chip(m_badChips->begin()); chip != m_badChips->end();
         ++chip) {
      VERBOSE("Module " << m_sctId->wafer_hash(chip->first) << ", chip " << chip->second);
    }

    std::array < MonGroup, N_REGIONS + 1 > histGroupE = {{
      MonGroup(this, m_path + histogramPath[ENDCAP_C_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[BARREL_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[ENDCAP_A_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[GENERAL_INDEX], run, ATTRIB_UNMANAGED)
    }};

    std::array < MonGroup, N_REGIONS > histGroupL = {{
      MonGroup(this, m_path + histogramPathRe[ENDCAP_C_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPathRe[BARREL_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPathRe[ENDCAP_A_INDEX], run, ATTRIB_UNMANAGED)
    }};// 23.01.2015

    std::array < MonGroup, N_REGIONS + 1 > histGroupShift = {{
      MonGroup(this, m_path + histogramPath[ENDCAP_C_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[BARREL_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[ENDCAP_A_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[GENERAL_INDEX], run, ATTRIB_UNMANAGED)
    }};

    CHECK(bookEffHisto(m_Eff_Total, histGroupE[GENERAL_INDEX], "SctTotalEff", "SCT Total Efficiency", N_REGIONS, 0,
                       N_REGIONS));
    for (Int_t i(0); i != 3; ++i) {
      m_Eff_Total->GetXaxis()->SetBinLabel(i + 1, subDetName[i]);
    }
    CHECK(bookEffHisto(m_Eff_TotalBCID, histGroupE[GENERAL_INDEX], "SctTotalEffBCID",
                       "SCT Total Efficiency for First BCID", N_REGIONS, 0, N_REGIONS));
    for (Int_t i(0); i != 3; ++i) {
      m_Eff_TotalBCID->GetXaxis()->SetBinLabel(i + 1, subDetName[i]);
    }
    CHECK(bookEffHisto(m_Eff_hashCodeHisto, histGroupE[GENERAL_INDEX], "effHashCode", "Efficiency vs module Hash code",
                       n_mod[GENERAL_INDEX] * 2, -0.5, n_mod[GENERAL_INDEX] * 2 - 0.5));
    m_Eff_hashCodeHisto->GetXaxis()->SetTitle("Module Hash Code");
    m_Eff_hashCodeHisto->GetYaxis()->SetTitle("Efficiency");
    CHECK(bookEffHisto(m_Eff_LumiBlockHisto_Total, histGroupE[GENERAL_INDEX], "effLumiBlock",
                       "Efficiency vs Luminosity block", NBINS_LBs, 0.5, NBINS_LBs + 0.5));// 02.09.2016
    m_Eff_LumiBlockHisto_Total->GetXaxis()->SetTitle("Luminosity block");
    m_Eff_LumiBlockHisto_Total->GetYaxis()->SetTitle("Efficiency");
    CHECK(bookEffHisto(m_effdistribution, histGroupE[GENERAL_INDEX], "SctEffDistribution",
                       "SCT Efficiency Distribution", 500, 0, 1));
    m_effdistribution->GetXaxis()->SetTitle("Efficiency");
    m_effdistribution->GetYaxis()->SetTitle("Links");

    if (m_detailed) {
      CHECK(bookEffHisto(m_SCTNHitHisto, histGroupE[BARREL_INDEX], "SCTNHit", "Number of total SCT hits", 30, -0.5,
                         29.5));
      CHECK(bookEffHisto(m_barrelNHitHisto, histGroupE[BARREL_INDEX], "barrelNHit", "Number of hits in B", 30, -0.5,
                         29.5));
      CHECK(bookEffHisto(m_pNHitHisto, histGroupE[BARREL_INDEX], "pNHit", "Number of hits in EA", 30, -0.5, 29.5));
      CHECK(bookEffHisto(m_mNHitHisto, histGroupE[BARREL_INDEX], "mNHit", "Number of hits in EC", 30, -0.5, 29.5));
      CHECK(bookEffHisto(m_trtNHitHisto, histGroupE[BARREL_INDEX], "trtNHit", "Number of TRT hits", 140, -0.5, 139.5));
      CHECK(bookEffHisto(m_pixelNHitHisto, histGroupE[BARREL_INDEX], "pixelNHit", "Number of pixel hits", 30, -0.5,
                         29.5));
      CHECK(bookEffHisto(m_hashCodeHisto, histGroupE[BARREL_INDEX], "hashCode", "module Hash code",
                         n_mod[GENERAL_INDEX] * 2, -0.5, n_mod[GENERAL_INDEX] * 2 - 0.5));

      CHECK(bookEffHisto(m_d0TkHisto, histGroupE[GENERAL], "D0Tk", "Track D0", 50, -500., 500.));
      CHECK(bookEffHisto(m_z0TkHisto, histGroupE[GENERAL], "Z0Tk", "Track Z0", 500, -500., 500.));
      CHECK(bookEffHisto(m_PtTkHisto, histGroupE[GENERAL], "PtTk", "log10 Track Pt", 40, 2., 6.));
      CHECK(bookEffHisto(m_nTrkHisto, histGroupE[GENERAL], "nTrk", "num Tracks", 400, -0.5, 399.5));
      CHECK(bookEffHisto(m_etaTkHisto, histGroupE[GENERAL], "EtaTk", "Track Eta", 100, -2.5, 2.5));
      CHECK(bookEffHisto(m_d0PrecTkHisto, histGroupE[GENERAL], "D0Tk-prec", "Track D0 prec", 100, -25., 25.));
      CHECK(bookEffHisto(m_nTrkParsHisto, histGroupE[GENERAL], "nTrk pars", "num Tracks pars", 400, -0.5, 399.5));
      CHECK(bookEffHisto(m_nTrkGoodHisto, histGroupE[GENERAL], "nTrk good", "num Tracks good", 400, -0.5, 399.5));
    }
    if (m_superDetailed) {
      CHECK(bookEffHisto(m_LumiBlock, histGroupE[GENERAL], "LumiBlocks", "Luminosity blocks", NBINS_LBs, 0.5, NBINS_LBs + 0.5));
      CHECK(bookEffHisto(m_effHashLumiB, histGroupE[GENERAL], "effHashCodeLumiBlock",
                         "Modules efficiencies vs. lumi. block",
                         n_mod[GENERAL_INDEX] * 2, -0.5, n_mod[GENERAL_INDEX] * 2 - 0.5, NBINS_LBs, 0.5, NBINS_LBs + 0.5));
      m_badModMap = new TGraphErrors();
      m_badModMap->SetName("MapOfDisabledModules");
      CHECK(histGroupE[GENERAL].regGraph(m_badModMap));
      m_badChipMap = new TGraphErrors();
      m_badChipMap->SetName("MapOfDisabledChips");
      CHECK(histGroupE[GENERAL].regGraph(m_badChipMap));
      CHECK(bookEffHisto(m_badModFineMap, histGroupE[GENERAL], "FineMapOfDisabledModules", "", 60, -3, 3, 64, -3.2,
                         3.2));
    }
    // Booking efficiency maps
    // inefficiency plots, i.e. 1 - efficiency
    std::array < TString, N_REGIONS > mapName = {{
      "m_effm_", "m_eff_", "m_effp_"
    }};
    std::array < TString, N_REGIONS > ineffMapName = {{
      "ineffm_", "ineff_", "ineffp_"
    }};
    std::array < TString, N_REGIONS > effLumiName = {{
      "m_eff_Lumi_", "eff_Lumi_", "p_eff_Lumi_"
    }};// 23.01.2015
    std::array < TString, N_REGIONS > sumeff = {{
      "summaryeffm", "summaryeff", "summaryeffp"
    }};
    std::array < TString, N_REGIONS > sumeffBCID = {{
      "summaryeffmBCID", "summaryeffBCID", "summaryeffpBCID"
    }};
    std::array < TString, N_REGIONS > sumeff_old = {{
      "summaryeffm_old", "summaryeff_old", "summaryeffp_old"
    }};
    TString sumefftitle[3] = {
      "Summary Module Efficiency in Endcap C", "Summary Module Efficiency in Barrel",
      "Summary Module Efficiency in Endcap A"
    };// 30.11.2014
    TString sumefftitleBCID[3] = {
      "Summary Module Efficiency in Endcap C for First BC", "Summary Module Efficiency in Barrel for First BC",
      "Summary Module Efficiency in Endcap A for First BC"
    };// 30.11.2014
    std::array < TString, 12 > selecName = {{
      "All", "Module", "nHits", "TRTPhase", "Enclosed", "Phi", "Chi2", "Face", "Guard", "Bad chip", "d0", "pT"
    }};
    for (Int_t isub(0); isub != N_REGIONS; ++isub) {
      for (Long_t i(0); i != n_layers[isub]; ++i) {
        const int detIndex = becIdxLayer2Index(isub, i);
        if (detIndex == -1) {
          WARNING("Subdetector region (barrel, endcap A, endcap C) could not be determined");
          continue;
        }
        for (Long_t j(0); j != 2; ++j) {
          // book inefficiency histogram
          CHECK(bookEffHisto(m_ineffMap[detIndex][j], histGroupE[isub],
                             ineffMapName[isub] + i + "_" + j,
                             "Hit inefficiency of" + layerName[isub] + i + " / side " + j + " in " + subDetName[isub],
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          m_ineffMap[detIndex][j]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_ineffMap[detIndex][j]->GetYaxis()->SetTitle("Index in the direction of #phi");
          //

          CHECK(bookEffHisto(m_effMap[detIndex][j], histGroupE[isub],
                             mapName[isub] + i + "_" + j,
                             "Hit efficiency of" + layerName[isub] + i + " / side " + j + " in " + subDetName[isub],
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          m_effMap[detIndex][j]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_effMap[detIndex][j]->GetYaxis()->SetTitle("Index in the direction of #phi");

          CHECK(bookEffHisto(m_effLumiBlock[detIndex][j], histGroupL[isub],
                             effLumiName[isub] + i + "_" + j,
                             "Efficiency vs LumiBlock of" + layerName[isub] + i + " / side " + j + " in " +
                             subDetName[isub],
                             NBINS_LBs, 0.5, NBINS_LBs + 0.5));// 23.01.2015
          m_effLumiBlock[detIndex][j]->GetXaxis()->SetTitle("Luminosity Block");
          m_effLumiBlock[detIndex][j]->GetYaxis()->SetTitle("Efficiency");
        }

        if (m_superDetailed) {
          CHECK(bookEffHisto(m_accMap[detIndex], histGroupE[isub],
                             "nDisabledChips_" + subDetNameShort[isub] + "_" + i,
                             "Map of the acceptance for" + layerName[isub] + i + " in " + subDetName[isub],
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          TString resid("xlResidual_");
          CHECK(bookEffHisto(m_xlResidualHisto[isub][i], histGroupE[isub], resid + i, resid + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_xlResidualE0Histo[isub][i], histGroupE[isub], resid + "eta0_" + i, resid + "eta0_" + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_xlResidualE1Histo[isub][i], histGroupE[isub], resid + "eta1_" + i, resid + "eta1_" + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_xlResidualUnasHisto[isub][i], histGroupE[isub], resid + "unas_" + i, resid + "unas_" + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_layerResidualHistos[isub][2 * i], histGroupE[isub], TString("residlayer") + i,
                             TString("residlayer") + i,
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          CHECK(bookEffHisto(m_layerResidualHistos[isub][2 * i + 1], histGroupE[isub], TString("residlayer") + i + ".5",
                             TString("residlayer") + i + ".5",
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
        }
      }

      CHECK(bookEffHisto(m_Eff_summaryHisto[isub], histGroupShift[isub], sumeff[isub], sumefftitle[isub],
                         2 * n_layers[isub], 0., n_layers[isub]));
      CHECK(bookEffHisto(m_Eff_summaryHistoFirstBCID[isub], histGroupShift[isub], sumeffBCID[isub],
                         sumefftitleBCID[isub], 2 * n_layers[isub], 0., n_layers[isub]));
      CHECK(bookEffHisto(m_Eff_summaryHisto_old[isub], histGroupShift[isub], sumeff_old[isub], sumefftitle[isub],
                         2 * n_layers[isub], 0., n_layers[isub]));
      unsigned int limit[3] = {
        N_DISKSx2, N_BARRELSx2, N_DISKSx2
      };// 30.11.2014
      for (unsigned int i(0); i != limit[isub]; i++) {
        LayerSideFormatter layerSide(i, isub);
        // use dedicated title for the bin labels (from 0_1 to 3_0)
        m_Eff_summaryHisto[isub]->GetXaxis()->SetBinLabel(i + 1, layerSide.dedicated_title().c_str());
        m_Eff_summaryHistoFirstBCID[isub]->GetXaxis()->SetBinLabel(i + 1, layerSide.dedicated_title().c_str());
        m_Eff_summaryHisto_old[isub]->GetXaxis()->SetBinLabel(i + 1, layerSide.title().c_str());
      }// 30.11.2014
      m_Eff_summaryHisto[isub]->GetYaxis()->SetTitle("Efficiency");
      m_Eff_summaryHistoFirstBCID[isub]->GetYaxis()->SetTitle("Efficiency");
      m_Eff_summaryHisto_old[isub]->GetYaxis()->SetTitle("Efficiency");

      CHECK(bookEffHisto(m_Eff_LumiBlockHisto[isub], histGroupE[isub], "effLumiBlock",
                         "Efficiency vs Luminosity block in " + subDetName[isub], NBINS_LBs, 0.5, NBINS_LBs + 0.5));// 20.01.2015
      m_Eff_LumiBlockHisto[isub]->GetXaxis()->SetTitle("Luminosity block");
      m_Eff_LumiBlockHisto[isub]->GetYaxis()->SetTitle("Efficiency");

      if (m_detailed) {
        CHECK(bookEffHisto(m_nTrkUsedHisto[isub], histGroupE[isub], "nTrk Used", "nTrk used", 50, -0.5, 49.5));
        CHECK(bookEffHisto(m_probEnclosedHisto[isub], histGroupE[isub], "probEnclosed", "Probability Enclosed",
                           2 * n_layers[isub], -.25, n_layers[isub] - .25));
        CHECK(bookEffHisto(m_EventHisto[isub], histGroupE[isub], "EventNo", "Event No", 800, 0., 40000000.));
        CHECK(bookEffHisto(m_Eff_EventHisto[isub], histGroupE[isub], "effEventNo", "Efficiency v Event no.", 800, 0.,
                           40000000.));
        CHECK(bookEffHisto(m_SelectionHisto[isub], histGroupE[isub], "selectionCuts", "Selection Cuts", 12, -0.5,
                           11.5));
        CHECK(bookEffHisto(m_Eff_SelectionHisto[isub], histGroupE[isub], "effSelectionCuts",
                           "Efficiency v Selection Cuts", 12, -0.5, 11.5));
        for (Int_t bin(0); bin != 12; ++bin) {
          m_SelectionHisto[isub]->GetXaxis()->SetBinLabel(bin + 1, selecName[bin]);
          m_Eff_SelectionHisto[isub]->GetXaxis()->SetBinLabel(bin + 1, selecName[bin]);
        }
        CHECK(bookEffHisto(m_ResidualHisto[isub], histGroupE[isub], "residuals", "Residuals", 2000, -10, 10));
        CHECK(bookEffHisto(m_ResidualUnasHisto[isub], histGroupE[isub], "residualsUnas", "Residuals Unassociated", 2000,
                           -10, 10));
        CHECK(bookEffHisto(m_ResidualMissHisto[isub], histGroupE[isub], "residualsMiss", "Residuals Miss", 2000, -10,
                           10));
        CHECK(bookEffHisto(m_Unas_summaryHisto[isub], histGroupE[isub], "summaryunas",
                           "Summary Module Unassociated Per Event", 2 * 9, 0., 9.));
        CHECK(bookEffHisto(m_chi2Histo[isub], histGroupE[isub], "chi2", "Chi2", 50, 0., 10.));
        CHECK(bookEffHisto(m_Eff_chi2Histo[isub], histGroupE[isub], "effChi2", "Efficiency v Chi2", 25, 0., 10.));
        CHECK(bookEffHisto(m_Eff_chi2HistoFinal[isub], histGroupE[isub], "effChi2Final", "Efficiency v Chi2, Final", 25,
                           0., 10.));
        CHECK(bookEffHisto(m_localMissHisto[isub], histGroupE[isub], "localMiss", "local position, hole", 40, -40., 40.,
                           40, -80., 80.));
        CHECK(bookEffHisto(m_localUnasHisto[isub], histGroupE[isub], "localUnas", "local position, unassociated hit",
                           40, -40., 40., 40, -80., 80.));
        CHECK(bookEffHisto(m_Eff_etaHisto[isub], histGroupE[isub], "effEta", "Efficiency versus track eta", 30, -2.8,
                           2.8));
        CHECK(bookEffHisto(m_Eff_etaPhiCutHisto[isub], histGroupE[isub], "effEtaPhiCut",
                           "Efficiency versus track eta, phi cut", 30, -2.8, 2.8));
        CHECK(bookEffHisto(m_Eff_otherFaceHisto[isub], histGroupE[isub], "effOtherFace",
                           "Efficiency versus other face hit y/n", 3, -1.5, 1.5));
        CHECK(bookEffHisto(m_Unas_otherFaceHisto[isub], histGroupE[isub], "effOtherFaceUnas",
                           "other face hit Unassociated per event", 3, -1.5, 1.5));
        CHECK(bookEffHisto(m_Eff_nSCTHisto[isub], histGroupE[isub], "effNSCT", "Efficiency versus Number of SCT hits",
                           30, -0.5, 29.5));
        CHECK(bookEffHisto(m_Eff_nTRTHisto[isub], histGroupE[isub], "effNTRT", "Efficiency versus Number of TRT hits",
                           70, -1, 139.));
        CHECK(bookEffHisto(m_Eff_nOtherHisto[isub], histGroupE[isub], "effNOther",
                           "Efficiency versus Number of other SCT  sides hit", 30, -0.5, 29.5));
        CHECK(bookEffHisto(m_nOtherHisto[isub], histGroupE[isub], "nOther", "Number of other SCT barrel sides hit", 30,
                           -0.5, 29.5));
        CHECK(bookEffHisto(m_timecorHisto[isub], histGroupE[isub], "timecor", "TRT phase", 120, -50.5, 69.5));
        CHECK(bookEffHisto(m_Eff_timecorHisto[isub], histGroupE[isub], "effTimecor", "Efficiency vs TRT phase", 120,
                           -50.5, 69.5));
        CHECK(bookEffHisto(m_Eff_xlocHisto[isub], histGroupE[isub], "effXloc", "Module Efficiency by x local", 32, -32.,
                           32.));
        CHECK(bookEffHisto(m_Eff_ylocHistos[isub], histGroupE[isub], "effYloc", "Module Efficiency by y local", 32,
                           -64., 64.));
        CHECK(bookEffHisto(m_Unas_xlocHisto[isub], histGroupE[isub], "unasXloc",
                           "Unassociated hit per event by x local", 32, -32., 32.));
        CHECK(bookEffHisto(m_Unas_ylocHisto[isub], histGroupE[isub], "unasYloc",
                           "Unassociated hit per event by y local", 32, -64., 64.));
        CHECK(bookEffHisto(m_Eff_ptiHisto[isub], histGroupE[isub], "effPti", "Module Efficiency by q/p", 20, -0.002,
                           0.002));
        CHECK(bookEffHisto(m_Eff_ptHisto[isub], histGroupE[isub], "effPt", "Module Efficiency by log10 |p|", 40, 2.,
                           6.));
        CHECK(bookEffHisto(m_Unas_ptiHisto[isub], histGroupE[isub], "unasPti", "Unassociated Hit per event by q/p", 20,
                           -0.002, 0.002));
        CHECK(bookEffHisto(m_Unas_phiHisto[isub], histGroupE[isub], "unasPhi",
                           "Unassociated hit per event by phi local", 30, -90., 90.));
        CHECK(bookEffHisto(m_Eff_phiHisto[isub], histGroupE[isub], "effPhi", "Module Efficiency by Phi", 30, -90.,
                           90.));
        CHECK(bookEffHisto(m_Eff_phiEtaCutHisto[isub], histGroupE[isub], "effPhiEtaCut",
                           "Module Efficiency by Phi, Eta cut", 30, -90., 90.));
        CHECK(bookEffHisto(m_Eff_phiHistoFinal[isub], histGroupE[isub], "effPhiFinal", "Module Efficiency by Phi Final",
                           30, -90., 90.));
        CHECK(bookEffHisto(m_Eff_d0Histo[isub], histGroupE[isub], "effD0", "Module Efficiency by D0", 10, -500., 500.));
        CHECK(bookEffHisto(m_Eff_d0PrecHisto[isub], histGroupE[isub], "effD0Prec", "Module Efficiency by D0 prec", 25,
                           -25., 25.));
        CHECK(bookEffHisto(m_Eff_z0Histo[isub], histGroupE[isub], "effZ0", "Module Efficiency by Z0", 10, -1000.,
                           1000.));
        CHECK(bookEffHisto(m_etaTkUsedHisto[isub], histGroupE[isub], "EtaTkUsed", "Used Track Eta", 100, -2.8, 2.8));
        CHECK(bookEffHisto(m_phiTkUsedHisto[isub], histGroupE[isub], "PhiTkUsed", "Used Track Phi", 50, -3.142, 3.142));
        CHECK(bookEffHisto(m_ptiTkUsedHisto[isub], histGroupE[isub], "PtiTkUsed", "Used Track q/p (MeV)", 50, -0.002,
                           0.002));
        CHECK(bookEffHisto(m_ptTkUsedHisto[isub], histGroupE[isub], "PtTkUsed", "Used Track log10|p| (MeV)", 40, 2.0,
                           6.));
        CHECK(bookEffHisto(m_d0TkUsedHisto[isub], histGroupE[isub], "D0TkUsed", "Used Track D0", 50, -500., 500.));
        CHECK(bookEffHisto(m_d0PrecTkUsedHisto[isub], histGroupE[isub], "D0PrecTkUsed", "Used Track D0 prec", 100, -25.,
                           25.));
        CHECK(bookEffHisto(m_z0TkUsedHisto[isub], histGroupE[isub], "Z0TkUsed", "Used Track Z0", 50, -1000., 1000.));
        CHECK(bookEffHisto(m_localHitXHisto[isub], histGroupE[isub], "localHitX", "local x position, hit", 80, -40.,
                           40.));
        CHECK(bookEffHisto(m_localHitYHistos[isub], histGroupE[isub], "localHitY", "local y position, hit", 320, -80.,
                           80.));
        CHECK(bookEffHisto(m_phiLocalHisto[isub], histGroupE[isub], "phiLocal", "local phi of tracks", 60, -90., 90.));
        CHECK(bookEffHisto(m_phiLocalCutHisto[isub], histGroupE[isub], "phiLocalCut", "local phi of low D0 tracks", 60,
                           -90., 90.));
        CHECK(bookEffHisto(m_Eff_nTrk[isub], histGroupE[isub], "effTrk", "Efficiency versus number of tracks", 200, 0,
                           400.));
        CHECK(bookEffHisto(m_Eff_nGoodTrk[isub], histGroupE[isub], "effGoodTrk",
                           "Efficiency versus number of good tracks", 200, 0, 400.));
      }

      if (m_superDetailed) {
        // CHECK (bookEffHisto(m_Eff_LumiBlockHisto[isub], histGroupE[isub],"effLumiBlock", "Efficiency v Luminosity
        // block",1000,1,1001));20.01.2015
        CHECK(bookEffHisto(m_inEffStrip[isub], histGroupE[isub],
                           "StripInEfficiency" + subDetNameShort[isub], "Strips inefficiencies in " + subDetName[isub],
                           n_mod[isub] * 2, f_mod[isub] * 2 - 0.5, (f_mod[isub] + n_mod[isub]) * 2 - 0.5, N_STRIPS,
                           FIRST_STRIP - 0.5, LAST_STRIP + 0.5));
        CHECK(bookEffHisto(m_inEffChip[isub], histGroupE[isub],
                           "ChipInEfficiency" + subDetNameShort[isub], "Chips ineficiencies in " + subDetName[isub],
                           n_mod[isub] * 2, f_mod[isub] * 2 - 0.5, (f_mod[isub] + n_mod[isub]) * 2 - 0.5, N_CHIPS,
                           FIRST_CHIP - 0.5, LAST_CHIP + 0.5));
        CHECK(bookEffHisto(m_badModPerSide[isub], histGroupE[isub], "badModPerSide", "Number of bad module per side",
                           2 * n_layers[isub], 0., n_layers[isub]));
        CHECK(bookEffHisto(m_holesPerTrackHisto[isub], histGroupE[isub], "holesPerTrack", "Number of holes per track",
                           2 * n_layers[isub], 0, n_layers[isub]));
        CHECK(bookEffHisto(m_holesDistPerTrackHisto[isub], histGroupE[isub], "holesDistPerTrack",
                           "Number of holes per track", 2 * n_layers[isub], 0, n_layers[isub]));
        CHECK(bookEffHisto(m_chi2vPhiHisto[isub], histGroupE[isub], "Chi2vPhi", "Chi2 v Phi", 30, -90., 90.));
        CHECK(bookEffHisto(m_localHitHisto[isub], histGroupE[isub], "localHit", "local position, hit", 40, -40., 40.,
                           40, -80., 80.));
        CHECK(bookEffHisto(m_localHitGHisto[isub], histGroupE[isub], "localHitG", "local position Guard, hit", 40, -40.,
                           40., 40, -80., 80.));
        CHECK(bookEffHisto(m_TwoSidesResiduals[isub], histGroupE[isub], "TwoSidesResiduals", "Two Sides Residuals", 120,
                           -3., 3., 120, -3., 3.));
        CHECK(bookEffHisto(m_TwoSidesResidualsIneff[isub], histGroupE[isub], "TwoSidesResidualsIneff",
                           "Two Sides Residuals (Inefficient/Unas)", 120, -30., 30., 120, -30., 30.));
        CHECK(bookEffHisto(m_chi2ResidualHisto[isub], histGroupE[isub], "chi2Residual", "Chi2 v Residual", 200, -10.,
                           10., 120, 0., 10.));
        CHECK(bookEffHisto(m_Eff_summaryIncBadMod[isub], histGroupE[isub], "summaryIncBadMod" + subDetNameShort[isub],
                           "Efficiency vs. layer including bad sensors",
                           2 * n_layers[isub], 0., n_layers[isub]));
      }
    }
  }
  return StatusCode::SUCCESS;
}

// StatusCode SCTHitEffMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool isNewRun )
//    // hidetoshi 14.01.22
StatusCode
SCTHitEffMonTool::bookHistogramsRecurrent() {                                                                       // hidetoshi
                                                                                                                    // 14.01.22
  //  if (not isNewRun) return StatusCode::SUCCESS;                                                                   //
  // hidetoshi 14.01.22
  //  if (not newRunFlag()) return StatusCode::SUCCESS;
  //                                                                         // hidetoshi 14.01.22
  if (m_isCosmic) {
    WARNING("Running on cosmics: releasing d0 cut and forcing use of TRT timing");
  }
  if (not m_fieldServiceHandle->solenoidOn()) {
    WARNING("Running with solenoid off: releasing pT cut");
    m_minPt = -1.;
  }
  if (newRunFlag()) {
    m_badChips = m_configConditions->badChips();
    INFO("Found " << m_badChips->size() << " bad chips");
    for (std::map<Identifier, unsigned int>::const_iterator chip(m_badChips->begin()); chip != m_badChips->end();
         ++chip) {
      VERBOSE("Module " << m_sctId->wafer_hash(chip->first) << ", chip " << chip->second);
    }

    std::array < MonGroup, N_REGIONS + 1 > histGroupE = {{
      MonGroup(this, m_path + histogramPath[ENDCAP_C_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[BARREL_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[ENDCAP_A_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[GENERAL_INDEX], run, ATTRIB_UNMANAGED)
    }};

    std::array < MonGroup, N_REGIONS > histGroupL = {{
      MonGroup(this, m_path + histogramPathRe[ENDCAP_C_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPathRe[BARREL_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPathRe[ENDCAP_A_INDEX], run, ATTRIB_UNMANAGED)
    }};// 23.01.2015

    std::array < MonGroup, N_REGIONS + 1 > histGroupShift = {{
      MonGroup(this, m_path + histogramPath[ENDCAP_C_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[BARREL_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[ENDCAP_A_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[GENERAL_INDEX], run, ATTRIB_UNMANAGED)
    }};

    CHECK(bookEffHisto(m_Eff_Total, histGroupE[GENERAL_INDEX], "SctTotalEff", "SCT Total Efficiency", N_REGIONS, 0,
                       N_REGIONS));
    for (Int_t i(0); i != 3; ++i) {
      m_Eff_Total->GetXaxis()->SetBinLabel(i + 1, subDetName[i]);
    }
    CHECK(bookEffHisto(m_Eff_TotalBCID, histGroupE[GENERAL_INDEX], "SctTotalEffBCID",
                       "SCT Total Efficiency for First BCID", N_REGIONS, 0, N_REGIONS));
    for (Int_t i(0); i != 3; ++i) {
      m_Eff_TotalBCID->GetXaxis()->SetBinLabel(i + 1, subDetName[i]);
    }
    CHECK(bookEffHisto(m_Eff_hashCodeHisto, histGroupE[GENERAL_INDEX], "effHashCode", "Efficiency vs module Hash code",
                       n_mod[GENERAL_INDEX] * 2, -0.5, n_mod[GENERAL_INDEX] * 2 - 0.5));// 26.11.2014
    m_Eff_hashCodeHisto->GetXaxis()->SetTitle("Module Hash Code");
    m_Eff_hashCodeHisto->GetYaxis()->SetTitle("Efficiency");// 15.12.2014
    CHECK(bookEffHisto(m_Eff_LumiBlockHisto_Total, histGroupE[GENERAL_INDEX], "effLumiBlock",
                       "Efficiency vs Luminosity block", NBINS_LBs, 0.5, NBINS_LBs + 0.5));// 02.09.2016

    if (m_detailed) {
      CHECK(bookEffHisto(m_SCTNHitHisto, histGroupE[BARREL_INDEX], "SCTNHit", "Number of total SCT hits", 30, -0.5,
                         29.5));
      CHECK(bookEffHisto(m_barrelNHitHisto, histGroupE[BARREL_INDEX], "barrelNHit", "Number of hits in B", 30, -0.5,
                         29.5));
      CHECK(bookEffHisto(m_pNHitHisto, histGroupE[BARREL_INDEX], "pNHit", "Number of hits in EA", 30, -0.5, 29.5));
      CHECK(bookEffHisto(m_mNHitHisto, histGroupE[BARREL_INDEX], "mNHit", "Number of hits in EC", 30, -0.5, 29.5));
      CHECK(bookEffHisto(m_trtNHitHisto, histGroupE[BARREL_INDEX], "trtNHit", "Number of TRT hits", 140, -0.5, 139.5));
      CHECK(bookEffHisto(m_pixelNHitHisto, histGroupE[BARREL_INDEX], "pixelNHit", "Number of pixel hits", 30, -0.5,
                         29.5));
      CHECK(bookEffHisto(m_hashCodeHisto, histGroupE[BARREL_INDEX], "hashCode", "module Hash code",
                         n_mod[GENERAL_INDEX] * 2, -0.5, n_mod[GENERAL_INDEX] * 2 - 0.5));

      CHECK(bookEffHisto(m_d0TkHisto, histGroupE[GENERAL], "D0Tk", "Track D0", 50, -500., 500.));
      CHECK(bookEffHisto(m_z0TkHisto, histGroupE[GENERAL], "Z0Tk", "Track Z0", 500, -500., 500.));
      CHECK(bookEffHisto(m_PtTkHisto, histGroupE[GENERAL], "PtTk", "log10 Track Pt", 40, 2., 6.));
      CHECK(bookEffHisto(m_nTrkHisto, histGroupE[GENERAL], "nTrk", "num Tracks", 400, -0.5, 399.5));
      CHECK(bookEffHisto(m_etaTkHisto, histGroupE[GENERAL], "EtaTk", "Track Eta", 100, -2.5, 2.5));
      CHECK(bookEffHisto(m_d0PrecTkHisto, histGroupE[GENERAL], "D0Tk-prec", "Track D0 prec", 100, -25., 25.));
      CHECK(bookEffHisto(m_nTrkParsHisto, histGroupE[GENERAL], "nTrk pars", "num Tracks pars", 400, -0.5, 399.5));
      CHECK(bookEffHisto(m_nTrkGoodHisto, histGroupE[GENERAL], "nTrk good", "num Tracks good", 400, -0.5, 399.5));
    }
    if (m_superDetailed) {
      CHECK(bookEffHisto(m_LumiBlock, histGroupE[GENERAL], "LumiBlocks", "Luminosity blocks", NBINS_LBs, 0.5, NBINS_LBs + 0.5));
      CHECK(bookEffHisto(m_effHashLumiB, histGroupE[GENERAL], "effHashCodeLumiBlock",
                         "Modules efficiencies vs. lumi. block",
                         n_mod[GENERAL_INDEX] * 2, -0.5, n_mod[GENERAL_INDEX] * 2 - 0.5, NBINS_LBs, 0.5, NBINS_LBs + 0.5));
      m_badModMap = new TGraphErrors();
      m_badModMap->SetName("MapOfDisabledModules");
      CHECK(histGroupE[GENERAL].regGraph(m_badModMap));
      m_badChipMap = new TGraphErrors();
      m_badChipMap->SetName("MapOfDisabledChips");
      CHECK(histGroupE[GENERAL].regGraph(m_badChipMap));
      CHECK(bookEffHisto(m_badModFineMap, histGroupE[GENERAL], "FineMapOfDisabledModules", "", 60, -3, 3, 64, -3.2,
                         3.2));
    }
    // Booking efficiency maps
    std::array < TString, N_REGIONS > mapName = {{
      "m_eff_", "eff_", "p_eff_"
    }};
    std::array < TString, N_REGIONS > effLumiName = {{
      "m_eff_Lumi_", "eff_Lumi_", "p_eff_Lumi_"
    }};// 23.01.2015
    // inefficiency plots, i.e. 1 - efficiency
    std::array< TString, N_REGIONS > ineffMapName = {{
      "ineffm_", "ineff_", "ineffp_"
    }};
    //
    std::array < TString, N_REGIONS > sumeff = {{
      "summaryeffm", "summaryeff", "summaryeffp"
    }};
    std::array < TString, N_REGIONS > sumeffBCID = {{
      "summaryeffmBCID", "summaryeffBCID", "summaryeffpBCID"
    }};
    std::array < TString, N_REGIONS > sumeff_old = {{
      "summaryeffm_old", "summaryeff_old", "summaryeffp_old"
    }};
    TString sumefftitle[3] = {
      "Summary Module Efficiency in Endcap C", "Summary Module Efficiency in Barrel",
      "Summary Module Efficiency in Endcap A"
    };
    TString sumefftitleBCID[3] = {
      "Summary Module Efficiency in Endcap C for First BC", "Summary Module Efficiency in Barrel for First BC",
      "Summary Module Efficiency in Endcap A for First BC"
    };
    std::array < TString, 12 > selecName = {{
      "All", "Module", "nHits", "TRTPhase", "Enclosed", "Phi", "Chi2", "Face", "Guard", "Bad chip", "d0", "pT"
    }};
    for (Int_t isub(0); isub != N_REGIONS; ++isub) {
      for (Long_t i(0); i != n_layers[isub]; ++i) {
        const int detIndex(becIdxLayer2Index(isub, i));
        if (detIndex == -1) {
          WARNING("Detector region (barrel, endcap A, endcap C) could not be determined");
          continue;
        }
        for (Long_t j(0); j != 2; ++j) {
          // book inefficiency histogram
          CHECK(bookEffHisto(m_ineffMap[detIndex][j], histGroupE[isub],
                             ineffMapName[isub] + i + "_" + j,
                             "Hit inefficiency of" + layerName[isub] + i + " / side " + j + " in " + subDetName[isub],
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          m_ineffMap[detIndex][j]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_ineffMap[detIndex][j]->GetYaxis()->SetTitle("Index in the direction of #phi");
          //

          CHECK(bookEffHisto(m_effMap[detIndex][j], histGroupE[isub],
                             mapName[isub] + i + "_" + j,
                             "Hit efficiency of" + layerName[isub] + i + " / side " + j + " in " + subDetName[isub],
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          m_effMap[detIndex][j]->GetXaxis()->SetTitle("Index in the direction of #eta");
          m_effMap[detIndex][j]->GetYaxis()->SetTitle("Index in the direction of #phi");


          CHECK(bookEffHisto(m_effLumiBlock[detIndex][j], histGroupL[isub],
                             effLumiName[isub] + i + "_" + j,
                             "Efficiency vs LumiBlock" + layerName[isub] + i + " / side " + j + " in " +
                             subDetName[isub],
                             150, 1, 3001));// 23.01.2015
          m_effLumiBlock[detIndex][j]->GetXaxis()->SetTitle("Luminosity Block");
          m_effLumiBlock[detIndex][j]->GetYaxis()->SetTitle("Efficiency");
        }
        if (m_superDetailed) {
          CHECK(bookEffHisto(m_accMap[detIndex], histGroupE[isub],
                             "nDisabledChips_" + subDetNameShort[isub] + "_" + i,
                             "Map of the acceptance for" + layerName[isub] + i + " in " + subDetName[isub],
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          TString resid("xlResidual_");
          CHECK(bookEffHisto(m_xlResidualHisto[isub][i], histGroupE[isub], resid + i, resid + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_xlResidualE0Histo[isub][i], histGroupE[isub], resid + "eta0_" + i, resid + "eta0_" + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_xlResidualE1Histo[isub][i], histGroupE[isub], resid + "eta1_" + i, resid + "eta1_" + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_xlResidualUnasHisto[isub][i], histGroupE[isub], resid + "unas_" + i, resid + "unas_" + i,
                             100, -30.5, 30.5, 100, -60., 60.));
          CHECK(bookEffHisto(m_layerResidualHistos[isub][2 * i], histGroupE[isub], TString("residlayer") + i,
                             TString("residlayer") + i,
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
          CHECK(bookEffHisto(m_layerResidualHistos[isub][2 * i + 1], histGroupE[isub], TString("residlayer") + i + ".5",
                             TString("residlayer") + i + ".5",
                             n_etabins[isub], f_etabin[isub] - .5, l_etabin[isub] + .5,
                             n_phibins[isub], f_phibin[isub] - .5, l_phibin[isub] + .5));
        }
      }

      CHECK(bookEffHisto(m_Eff_summaryHisto[isub], histGroupShift[isub], sumeff[isub], sumefftitle[isub],
                         2 * n_layers[isub], 0., n_layers[isub]));
      CHECK(bookEffHisto(m_Eff_summaryHistoFirstBCID[isub], histGroupShift[isub], sumeffBCID[isub],
                         sumefftitleBCID[isub], 2 * n_layers[isub], 0., n_layers[isub]));
      CHECK(bookEffHisto(m_Eff_summaryHisto_old[isub], histGroupShift[isub], sumeff_old[isub], sumefftitle[isub],
                         2 * n_layers[isub], 0., n_layers[isub]));
      unsigned int limit[3] = {
        N_DISKSx2, N_BARRELSx2, N_DISKSx2
      };// 30.11.2014
      for (unsigned int i(0); i != limit[isub]; i++) {
        LayerSideFormatter layerSide(i, isub);
        m_Eff_summaryHisto[isub]->GetXaxis()->SetBinLabel(i + 1, layerSide.dedicated_title().c_str());
        m_Eff_summaryHistoFirstBCID[isub]->GetXaxis()->SetBinLabel(i + 1, layerSide.dedicated_title().c_str());
        m_Eff_summaryHisto_old[isub]->GetXaxis()->SetBinLabel(i + 1, layerSide.title().c_str());
      }// 30.11.2014
      m_Eff_summaryHisto[isub]->GetYaxis()->SetTitle("Efficiency");
      m_Eff_summaryHistoFirstBCID[isub]->GetYaxis()->SetTitle("Efficiency");
      m_Eff_summaryHisto_old[isub]->GetYaxis()->SetTitle("Efficiency");
      CHECK(bookEffHisto(m_Eff_LumiBlockHisto[isub], histGroupE[isub], "effLumiBlock",
                         "Efficiency vs Luminosity block in " + subDetName[isub], NBINS_LBs, 0.5, NBINS_LBs + 0.5));// 20.01.2015
      m_Eff_LumiBlockHisto[isub]->GetXaxis()->SetTitle("Luminosity block");
      m_Eff_LumiBlockHisto[isub]->GetYaxis()->SetTitle("Efficiency");

      if (m_detailed) {
        CHECK(bookEffHisto(m_nTrkUsedHisto[isub], histGroupE[isub], "nTrk Used", "nTrk used", 50, -0.5, 49.5));
        CHECK(bookEffHisto(m_probEnclosedHisto[isub], histGroupE[isub], "probEnclosed", "Probability Enclosed",
                           2 * n_layers[isub], -.25, n_layers[isub] - .25));
        CHECK(bookEffHisto(m_EventHisto[isub], histGroupE[isub], "EventNo", "Event No", 800, 0., 40000000.));
        CHECK(bookEffHisto(m_Eff_EventHisto[isub], histGroupE[isub], "effEventNo", "Efficiency v Event no.", 800, 0.,
                           40000000.));
        CHECK(bookEffHisto(m_SelectionHisto[isub], histGroupE[isub], "selectionCuts", "Selection Cuts", 12, -0.5,
                           11.5));
        CHECK(bookEffHisto(m_Eff_SelectionHisto[isub], histGroupE[isub], "effSelectionCuts",
                           "Efficiency v Selection Cuts", 12, -0.5, 11.5));
        for (Int_t bin(0); bin != 12; ++bin) {
          m_SelectionHisto[isub]->GetXaxis()->SetBinLabel(bin + 1, selecName[bin]);
          m_Eff_SelectionHisto[isub]->GetXaxis()->SetBinLabel(bin + 1, selecName[bin]);
        }
        CHECK(bookEffHisto(m_ResidualHisto[isub], histGroupE[isub], "residuals", "Residuals", 2000, -10, 10));
        CHECK(bookEffHisto(m_ResidualUnasHisto[isub], histGroupE[isub], "residualsUnas", "Residuals Unassociated", 2000,
                           -10, 10));
        CHECK(bookEffHisto(m_ResidualMissHisto[isub], histGroupE[isub], "residualsMiss", "Residuals Miss", 2000, -10,
                           10));
        CHECK(bookEffHisto(m_Unas_summaryHisto[isub], histGroupE[isub], "summaryunas",
                           "Summary Module Unassociated Per Event", 2 * 9, 0., 9.));
        CHECK(bookEffHisto(m_chi2Histo[isub], histGroupE[isub], "chi2", "Chi2", 50, 0., 10.));
        CHECK(bookEffHisto(m_Eff_chi2Histo[isub], histGroupE[isub], "effChi2", "Efficiency v Chi2", 25, 0., 10.));
        CHECK(bookEffHisto(m_Eff_chi2HistoFinal[isub], histGroupE[isub], "effChi2Final", "Efficiency v Chi2, Final", 25,
                           0., 10.));
        CHECK(bookEffHisto(m_localMissHisto[isub], histGroupE[isub], "localMiss", "local position, hole", 40, -40., 40.,
                           40, -80., 80.));
        CHECK(bookEffHisto(m_localUnasHisto[isub], histGroupE[isub], "localUnas", "local position, unassociated hit",
                           40, -40., 40., 40, -80., 80.));
        CHECK(bookEffHisto(m_Eff_etaHisto[isub], histGroupE[isub], "effEta", "Efficiency versus track eta", 30, -2.8,
                           2.8));
        CHECK(bookEffHisto(m_Eff_etaPhiCutHisto[isub], histGroupE[isub], "effEtaPhiCut",
                           "Efficiency versus track eta, phi cut", 30, -2.8, 2.8));
        CHECK(bookEffHisto(m_Eff_otherFaceHisto[isub], histGroupE[isub], "effOtherFace",
                           "Efficiency versus other face hit y/n", 3, -1.5, 1.5));
        CHECK(bookEffHisto(m_Unas_otherFaceHisto[isub], histGroupE[isub], "effOtherFaceUnas",
                           "other face hit Unassociated per event", 3, -1.5, 1.5));
        CHECK(bookEffHisto(m_Eff_nSCTHisto[isub], histGroupE[isub], "effNSCT", "Efficiency versus Number of SCT hits",
                           30, -0.5, 29.5));
        CHECK(bookEffHisto(m_Eff_nTRTHisto[isub], histGroupE[isub], "effNTRT", "Efficiency versus Number of TRT hits",
                           70, -1, 139.));
        CHECK(bookEffHisto(m_Eff_nOtherHisto[isub], histGroupE[isub], "effNOther",
                           "Efficiency versus Number of other SCT  sides hit", 30, -0.5, 29.5));
        CHECK(bookEffHisto(m_nOtherHisto[isub], histGroupE[isub], "nOther", "Number of other SCT barrel sides hit", 30,
                           -0.5, 29.5));
        CHECK(bookEffHisto(m_timecorHisto[isub], histGroupE[isub], "timecor", "TRT phase", 120, -50.5, 69.5));
        CHECK(bookEffHisto(m_Eff_timecorHisto[isub], histGroupE[isub], "effTimecor", "Efficiency vs TRT phase", 120,
                           -50.5, 69.5));
        CHECK(bookEffHisto(m_Eff_xlocHisto[isub], histGroupE[isub], "effXloc", "Module Efficiency by x local", 32, -32.,
                           32.));
        CHECK(bookEffHisto(m_Eff_ylocHistos[isub], histGroupE[isub], "effYloc", "Module Efficiency by y local", 32,
                           -64., 64.));
        CHECK(bookEffHisto(m_Unas_xlocHisto[isub], histGroupE[isub], "unasXloc",
                           "Unassociated hit per event by x local", 32, -32., 32.));
        CHECK(bookEffHisto(m_Unas_ylocHisto[isub], histGroupE[isub], "unasYloc",
                           "Unassociated hit per event by y local", 32, -64., 64.));
        CHECK(bookEffHisto(m_Eff_ptiHisto[isub], histGroupE[isub], "effPti", "Module Efficiency by q/p", 20, -0.002,
                           0.002));
        CHECK(bookEffHisto(m_Eff_ptHisto[isub], histGroupE[isub], "effPt", "Module Efficiency by log10 |p|", 40, 2.,
                           6.));
        CHECK(bookEffHisto(m_Unas_ptiHisto[isub], histGroupE[isub], "unasPti", "Unassociated Hit per event by q/p", 20,
                           -0.002, 0.002));
        CHECK(bookEffHisto(m_Unas_phiHisto[isub], histGroupE[isub], "unasPhi",
                           "Unassociated hit per event by phi local", 30, -90., 90.));
        CHECK(bookEffHisto(m_Eff_phiHisto[isub], histGroupE[isub], "effPhi", "Module Efficiency by Phi", 30, -90.,
                           90.));
        CHECK(bookEffHisto(m_Eff_phiEtaCutHisto[isub], histGroupE[isub], "effPhiEtaCut",
                           "Module Efficiency by Phi, Eta cut", 30, -90., 90.));
        CHECK(bookEffHisto(m_Eff_phiHistoFinal[isub], histGroupE[isub], "effPhiFinal", "Module Efficiency by Phi Final",
                           30, -90., 90.));
        CHECK(bookEffHisto(m_Eff_d0Histo[isub], histGroupE[isub], "effD0", "Module Efficiency by D0", 10, -500., 500.));
        CHECK(bookEffHisto(m_Eff_d0PrecHisto[isub], histGroupE[isub], "effD0Prec", "Module Efficiency by D0 prec", 25,
                           -25., 25.));
        CHECK(bookEffHisto(m_Eff_z0Histo[isub], histGroupE[isub], "effZ0", "Module Efficiency by Z0", 10, -1000.,
                           1000.));
        CHECK(bookEffHisto(m_etaTkUsedHisto[isub], histGroupE[isub], "EtaTkUsed", "Used Track Eta", 100, -2.8, 2.8));
        CHECK(bookEffHisto(m_phiTkUsedHisto[isub], histGroupE[isub], "PhiTkUsed", "Used Track Phi", 50, -3.142, 3.142));
        CHECK(bookEffHisto(m_ptiTkUsedHisto[isub], histGroupE[isub], "PtiTkUsed", "Used Track q/p (MeV)", 50, -0.002,
                           0.002));
        CHECK(bookEffHisto(m_ptTkUsedHisto[isub], histGroupE[isub], "PtTkUsed", "Used Track log10|p| (MeV)", 40, 2.0,
                           6.));
        CHECK(bookEffHisto(m_d0TkUsedHisto[isub], histGroupE[isub], "D0TkUsed", "Used Track D0", 50, -500., 500.));
        CHECK(bookEffHisto(m_d0PrecTkUsedHisto[isub], histGroupE[isub], "D0PrecTkUsed", "Used Track D0 prec", 100, -25.,
                           25.));
        CHECK(bookEffHisto(m_z0TkUsedHisto[isub], histGroupE[isub], "Z0TkUsed", "Used Track Z0", 50, -1000., 1000.));
        CHECK(bookEffHisto(m_localHitXHisto[isub], histGroupE[isub], "localHitX", "local x position, hit", 80, -40.,
                           40.));
        CHECK(bookEffHisto(m_localHitYHistos[isub], histGroupE[isub], "localHitY", "local y position, hit", 320, -80.,
                           80.));
        CHECK(bookEffHisto(m_phiLocalHisto[isub], histGroupE[isub], "phiLocal", "local phi of tracks", 60, -90., 90.));
        CHECK(bookEffHisto(m_phiLocalCutHisto[isub], histGroupE[isub], "phiLocalCut", "local phi of low D0 tracks", 60,
                           -90., 90.));
        CHECK(bookEffHisto(m_Eff_nTrk[isub], histGroupE[isub], "effTrk", "Efficiency versus number of tracks", 200, 0,
                           400.));
        CHECK(bookEffHisto(m_Eff_nGoodTrk[isub], histGroupE[isub], "effGoodTrk",
                           "Efficiency versus number of good tracks", 200, 0, 400.));
      }

      if (m_superDetailed) {
        // CHECK (bookEffHisto(m_Eff_LumiBlockHisto[isub], histGroupE[isub],"effLumiBlock", "Efficiency v Luminosity
        // block",1000,1,1001));//20.01.2015
        CHECK(bookEffHisto(m_inEffStrip[isub], histGroupE[isub],
                           "StripInEfficiency" + subDetNameShort[isub], "Strips inefficiencies in " + subDetName[isub],
                           n_mod[isub] * 2, f_mod[isub] * 2 - 0.5, (f_mod[isub] + n_mod[isub]) * 2 - 0.5, N_STRIPS,
                           FIRST_STRIP - 0.5, LAST_STRIP + 0.5));
        CHECK(bookEffHisto(m_inEffChip[isub], histGroupE[isub],
                           "ChipInEfficiency" + subDetNameShort[isub], "Chips ineficiencies in " + subDetName[isub],
                           n_mod[isub] * 2, f_mod[isub] * 2 - 0.5, (f_mod[isub] + n_mod[isub]) * 2 - 0.5, N_CHIPS,
                           FIRST_CHIP - 0.5, LAST_CHIP + 0.5));
        CHECK(bookEffHisto(m_badModPerSide[isub], histGroupE[isub], "badModPerSide", "Number of bad module per side",
                           2 * n_layers[isub], 0., n_layers[isub]));
        CHECK(bookEffHisto(m_holesPerTrackHisto[isub], histGroupE[isub], "holesPerTrack", "Number of holes per track",
                           2 * n_layers[isub], 0, n_layers[isub]));
        CHECK(bookEffHisto(m_holesDistPerTrackHisto[isub], histGroupE[isub], "holesDistPerTrack",
                           "Number of holes per track", 2 * n_layers[isub], 0, n_layers[isub]));
        CHECK(bookEffHisto(m_chi2vPhiHisto[isub], histGroupE[isub], "Chi2vPhi", "Chi2 v Phi", 30, -90., 90.));
        CHECK(bookEffHisto(m_localHitHisto[isub], histGroupE[isub], "localHit", "local position, hit", 40, -40., 40.,
                           40, -80., 80.));
        CHECK(bookEffHisto(m_localHitGHisto[isub], histGroupE[isub], "localHitG", "local position Guard, hit", 40, -40.,
                           40., 40, -80., 80.));
        CHECK(bookEffHisto(m_TwoSidesResiduals[isub], histGroupE[isub], "TwoSidesResiduals", "Two Sides Residuals", 120,
                           -3., 3., 120, -3., 3.));
        CHECK(bookEffHisto(m_TwoSidesResidualsIneff[isub], histGroupE[isub], "TwoSidesResidualsIneff",
                           "Two Sides Residuals (Inefficient/Unas)", 120, -30., 30., 120, -30., 30.));
        CHECK(bookEffHisto(m_chi2ResidualHisto[isub], histGroupE[isub], "chi2Residual", "Chi2 v Residual", 200, -10.,
                           10., 120, 0., 10.));
        CHECK(bookEffHisto(m_Eff_summaryIncBadMod[isub], histGroupE[isub], "summaryIncBadMod" + subDetNameShort[isub],
                           "Efficiency vs. layer including bad sensors",
                           2 * n_layers[isub], 0., n_layers[isub]));
      }
    }
  }
  return StatusCode::SUCCESS;
}

// This is the real workhorse, called for each event. It retrieves the data each time
/*---------------------------------------------------------*/
StatusCode
SCTHitEffMonTool::fillHistograms() {
  errorcheck::ReportMessage::hideErrorLocus(true);

  VERBOSE("SCTHitEffMonTool::fillHistograms()");
  Double_t timecor(-20.);
  if (m_useTRTPhase or m_isCosmic) {
    SG::ReadHandle<ComTime> theComTime(m_comTimeName);
    if (theComTime.isValid()) {
      timecor = theComTime->getTime();
      VERBOSE("Retrieved ComTime object with name " << m_comTimeName.key() << " found: Time = " << timecor);
    } else {
      timecor = -18.;
      WARNING("ComTime object not found with name " << m_comTimeName.key());
    }
  }
  // If we are going to use TRT phase in anger, need run-dependent corrections.
  SG::ReadHandle<xAOD::EventInfo> pEvent(m_eventInfoKey);
  if (not pEvent.isValid()) {
    return ERROR("Could not find EventInfo"), StatusCode::FAILURE;
  }
  unsigned BCID = pEvent->bcid();
  int BCIDpos = m_bunchCrossingTool->distanceFromFront(BCID);
  bool InTrain = m_bunchCrossingTool->isInTrain(BCID);

  if (m_chronotime) {
    m_chrono->chronoStart("SCTHitEff");
  }

  // ---- First try if m_tracksName is a TrackCollection
  SG::ReadHandle<TrackCollection>tracks(m_TrackName);
  if (not tracks.isValid()) {
    WARNING("Tracks not found: " << tracks << " / " << m_TrackName.key());
    if (m_chronotime) {
      m_chrono->chronoStop("SCTHitEff");
    }
    return StatusCode::SUCCESS;
  } else {
    VERBOSE("Successfully retrieved " << m_TrackName.key() << " : " << tracks->size() << " items");
  }

  SG::ReadHandle<InDet::SCT_ClusterContainer> p_sctclcontainer(m_sctContainerName);
  if (not p_sctclcontainer.isValid()) {
    WARNING("SCT clusters container not found: " << m_sctContainerName.key());
    if (m_chronotime) {
      m_chrono->chronoStop("SCTHitEff");
    }
    return StatusCode::SUCCESS;
  }

  // cut on number of tracks (skip this cut for online)
  if (m_environment != AthenaMonManager::online) {
    if (failCut(tracks->size() <= m_maxTracks, "# of tracks cut")) {
      if (m_chronotime) {
        m_chrono->chronoStop("SCTHitEff");
      }
      return StatusCode::SUCCESS;
    }
  }

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved in fillHistograms()");
    return StatusCode::FAILURE;
  }

  Int_t nTrk(0), nTrkPars(0), nTrkGood(0);

  // Loop over track collection to count tracks
  const TrackCollection::const_iterator endOfTracks(tracks->end());
  for (TrackCollection::const_iterator itr(tracks->begin()); itr != endOfTracks; ++itr) {
    nTrk++;
    const Trk::Track *pthisTrack(*itr);
    if (not pthisTrack) {
      continue;
    }
    if (failCut(pthisTrack and pthisTrack->trackParameters() and pthisTrack->trackParameters()->size(),
                "track cut: presence")) {
      continue;
    }
    nTrkPars++;

    if (m_insideOutOnly and failCut(pthisTrack->info().patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder),
                                    "track cut: inside-out only")) {
      continue;
    }
    if (pthisTrack->perigeeParameters() == 0) {
      continue;
    }
    const Trk::Perigee *perigee = pthisTrack->perigeeParameters();
    const AmgVector(5) & perigeeParameters(perigee->parameters());
    const float d0(perigeeParameters[Trk::d0]);
    const float z0(perigeeParameters[Trk::z0]);
    const float perigeeTheta(perigeeParameters[Trk::theta]);

    if (failCut(perigee->pT() >= m_minPt, "track cut: Min Pt")) {
      continue;
    }
    if (not m_isCosmic and failCut(fabs(d0) <= m_maxD0, "track cut: max D0")) {
      continue;
    }
    if (m_maxZ0sinTheta and failCut(fabs(z0 * sin(perigeeTheta)) <= m_maxZ0sinTheta, "track cut: Max Z0sinTheta")) {
      continue;
    }
    nTrkGood++;
  }

  // Loop over original track collection
  //  const TrackCollection::const_iterator endOfTracks(tracks->end());
  for (TrackCollection::const_iterator itr(tracks->begin()); itr != endOfTracks; ++itr) {
    VERBOSE("Starting new track");
    const Trk::Track *pthisTrack(*itr);
    if (not pthisTrack) {
      continue;
    }
    if (failCut(pthisTrack and pthisTrack->trackParameters() and pthisTrack->trackParameters()->size(),
                "track cut: presence")) {
      continue;
    }

    if (m_insideOutOnly and failCut(pthisTrack->info().patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder),
                                    "track cut: inside-out only")) {
      continue;
    }
    if (pthisTrack->perigeeParameters() == 0) {
      continue;
    }
    const Trk::Perigee *perigee = pthisTrack->perigeeParameters();
    const AmgVector(5) & perigeeParameters(perigee->parameters());
    const float d0(perigeeParameters[Trk::d0]);
    const float z0(perigeeParameters[Trk::z0]);
    const float perigeeTheta(perigeeParameters[Trk::theta]);
    const float pT(perigee->pT());
    const float eta(perigee->eta());
    const float phi(perigeeParameters[Trk::phi]);
    const float perigeeCharge(perigee->charge());

    if (failCut(perigee->pT() >= m_minPt, "track cut: Min Pt")) {
      continue;
    }
    if (not m_isCosmic and failCut(fabs(d0) <= m_maxD0, "track cut: max D0")) {
      continue;
    }
    if (m_maxZ0sinTheta and failCut(fabs(z0 * sin(perigeeTheta)) <= m_maxZ0sinTheta, "track cut: Max Z0sinTheta")) {
      continue;
    }

    bool useDetector[N_REGIONS] = {
      false, false, false
    };
    const Trk::TrackSummary *summary = pthisTrack->trackSummary();

    if (summary and summary->get(Trk::numberOfSCTHits) < 1) {
      continue;
    }

    const Trk::Track *trackWithHoles = m_holeSearchTool->getTrackWithHoles(*pthisTrack);
    if (not trackWithHoles) {
      WARNING("trackWithHoles pointer is invalid");
      continue;
    }
    VERBOSE("Found " << trackWithHoles->trackStateOnSurfaces()->size() << " states on track");
    // loop over all hits on track
    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr =
      trackWithHoles->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItrE = trackWithHoles->trackStateOnSurfaces()->end();

    Int_t NHits[3] = {
      0, 0, 0
    };
    Int_t pixelNHits(0);
    Int_t trtNHits(0);
    std::map < Identifier, Double_t > mapOfTrackHitResiduals;
    Double_t zmin = std::numeric_limits<float>::max();
    Double_t zmax = -std::numeric_limits<float>::max();
    Double_t zpos(0.);
    Float_t layerSide(-1);
    Float_t min_layerSide(999);
    Float_t max_layerSide(-1);
    Identifier surfaceID;

    for (; TSOSItr != TSOSItrE; ++TSOSItr) {
      surfaceID = surfaceOnTrackIdentifier(*TSOSItr);

      if (!surfaceID.is_valid()) {
        continue;
      }
      if ((*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) or(*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier))
      {
        if (m_pixelId->is_pixel(surfaceID)) {
          pixelNHits++;
        }
        if (m_trtId->is_trt(surfaceID)) {
          trtNHits++;
        }
        if (m_sctId->is_sct(surfaceID)) {
          NHits[bec2Index(m_sctId->barrel_ec(surfaceID))]++;
          mapOfTrackHitResiduals[surfaceID] = getResidual(surfaceID, (*TSOSItr)->trackParameters(), &*p_sctclcontainer);
        }
      }

      if ((*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) { // Determine zmin and zmax taking multiple
                                                                     // hits/side into account
        if (m_isCosmic) { // If this is cosmics use z, bad method but ok...
          if ((*TSOSItr)->trackParameters()) {
            zpos = (*TSOSItr)->trackParameters()->position().z();
            zmax = std::max(zpos, zmax);
            zmin = std::min(zpos, zmin);
          }else {
            WARNING("No track parameter found. Zmin and Zmax not recalculated.");
          }
        }else { // else use layer/side number : better but does not work for cosmics
          if (m_sctId->is_sct(surfaceID)) {
            layerSide = (m_sctId->barrel_ec(surfaceID) != 0) * N_BARRELS +
                        float(m_sctId->layer_disk(surfaceID)) + (float(m_sctId->side(surfaceID)) == 0) * 0.5;
            min_layerSide = std::min(min_layerSide, layerSide);
            max_layerSide = std::max(max_layerSide, layerSide);
          }else if (m_pixelId->is_pixel(surfaceID)) {
            min_layerSide = -1;
          }else if (m_trtId->is_trt(surfaceID)) {
            max_layerSide = N_BARRELS + N_ENDCAPS + 1;
          }
        }
      }
    }

    Int_t sctNHits(NHits[ENDCAP_C_INDEX] + NHits[BARREL_INDEX] + NHits[ENDCAP_A_INDEX]);
    std::vector<Bool_t> layersCrossedByTrack[N_REGIONS];
    std::vector<Int_t> nHolesOnLayer[N_REGIONS];
    std::vector<Int_t> nHolesDistOnLayer[N_REGIONS];
    for (Int_t i(0); i != N_REGIONS; ++i) {
      nHolesDistOnLayer[i].resize(n_layers[i] * 2, 0);
      nHolesOnLayer[i].resize(n_layers[i] * 2, 0);
      layersCrossedByTrack[i].resize(n_layers[i] * 2, false);
    }

    for (TSOSItr = trackWithHoles->trackStateOnSurfaces()->begin(); TSOSItr != TSOSItrE; ++TSOSItr) {
      VERBOSE("Starting new hit");
      surfaceID = surfaceOnTrackIdentifier(*TSOSItr);

      if (failCut(m_sctId->is_sct(surfaceID), "hit cut: is in SCT")) {
        continue;
      }

      Int_t isub(bec2Index(m_sctId->barrel_ec(surfaceID)));
      if (failCut(not (isub == INVALID_INDEX), "hit cut: valid sub-detector")) {
        continue;
      }
      VERBOSE("New SCT candidate: " << m_sctId->print_to_string(surfaceID));

      Int_t side(m_sctId->side(surfaceID));
      Int_t layer(m_sctId->layer_disk(surfaceID));
      const int detIndex = becIdxLayer2Index(isub, layer);
      if (detIndex == -1) {
        WARNING("The detector region (barrel, endcap A, endcap C) could not be determined");
        continue;
      }
      Int_t histnumber(detIndex);
      Int_t eff(0), unas(0);
      IdentifierHash sideHash(m_sctId->wafer_hash(surfaceID));
      Identifier module_id(m_sctId->module_id(surfaceID));
      Float_t layerPlusHalfSide(float(layer) + float(side) * 0.5);
      Float_t dedicated_layerPlusHalfSide(float(layer) + float((side + 1) % 2) * 0.5);
      const Trk::TrackParameters *trkParamOnSurface((*TSOSItr)->trackParameters());
      Double_t trackHitResidual(getResidual(surfaceID, trkParamOnSurface, &*p_sctclcontainer));


      Float_t distCut(m_effdistcut);

      if ((*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) or(*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier))
      {
        eff = 1;
      } else if ((*TSOSItr)->type(Trk::TrackStateOnSurface::Hole) and fabs(trackHitResidual) < distCut) {
        eff = 1;
        unas = 1;
      }

      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(0.); // Some sort of track on silicon
        m_Eff_SelectionHisto[isub]->Fill(0., eff); // Some sort of track on silicon
        m_SelectionHisto[isub]->Fill(1.); // Past the bad module selection
        m_Eff_SelectionHisto[isub]->Fill(1., eff); // Past the bad module selection
      }

      bool otherFaceFound(false);
      IdentifierHash otherSideHash;
      Identifier otherSideSurfaceID;
      IdContext context(m_sctId->wafer_context());
      m_sctId->get_other_side(sideHash, otherSideHash);
      m_sctId->get_id(otherSideHash, otherSideSurfaceID, &context);
      otherFaceFound = mapOfTrackHitResiduals.find(otherSideSurfaceID) != mapOfTrackHitResiduals.end();
      // (std::find(TrackSurfaceID.begin(), TrackSurfaceID.end(), otherSideSurfaceID) != TrackSurfaceID.end());

      Int_t nOther(sctNHits);
      if ((*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) or(*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier))
      {
        --nOther;
      }

      // Get the track phi; we may cut on it.
      Double_t phiUp(90.);
      Double_t theta(90.);
      if (trkParamOnSurface and not findAnglesToWaferSurface(trkParamOnSurface->momentum(), surfaceID, elements, theta, phiUp)) {
        WARNING("Error from findAngles");
      }

      if (m_detailed) {
        m_Eff_nSCTHisto[isub]->Fill(sctNHits, eff);
        m_Eff_nTRTHisto[isub]->Fill(trtNHits, eff);
      }
      if (m_useSCTorTRT) {
        if (failCut(trtNHits >= m_minTRTHits or
                    sctNHits >= m_minSCTHits, "track cut: min TRT or SCT hits")) {
          continue;
        }
      } else {
        if (failCut(trtNHits >= m_minTRTHits, "track cut: min TRT hits")) {
          continue;
        }
        if (failCut(sctNHits >= m_minSCTHits, "track cut: min SCT hits")) {
          continue;
        }
        if (failCut(pixelNHits >= m_minPixelHits, "track cut: min Pixel hits")) {
          continue;
        }
      }

      if (m_detailed) {
        m_nOtherHisto[isub]->Fill(nOther);
        m_Eff_nOtherHisto[isub]->Fill(nOther, eff);
      }

      if (failCut(nOther >= m_minOtherHits, "track cut: minOtherHits")) {
        continue;
      }

      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(2.); // Past SCT/TRT Hits selection
        m_Eff_SelectionHisto[isub]->Fill(2., eff); // Past SCT/TRT Hits selection
        if (m_useTRTPhase or m_isCosmic) {
          m_timecorHisto[isub]->Fill(timecor);
          m_Eff_timecorHisto[isub]->Fill(timecor, eff);
        }
      }
      DEBUG("Use TRT phase " << m_useTRTPhase << " is cosmic? " << m_isCosmic << " timecor " << timecor);
      if (m_useTRTPhase or m_isCosmic) {
        if (timecor == 0) {
          continue;
        }
        Double_t tmin(1.);
        Double_t tmax(40.);
        if (pEvent->runNumber() > 96000) {
          /// Guess that all this set are the same timing.
          tmin = -5;
          tmax = 20;
        }
        if (pEvent->runNumber() > 110000) {
          /// Guess that all this set are the same timing.
          tmin = -15;
          tmax = 10;
        }
        switch (pEvent->runNumber()) {
        case  91885: tmin = 1;
          tmax = 40;
          break;

        case  91890: tmin = 1;
          tmax = 40;
          break;

        case  92160: tmin = -10;
          tmax = 30;
          break;

        case  96858: tmin = -10;
          tmax = 20;
          break; // b field off

        case  96884: tmin = -10;
          tmax = 20;
          break; // b field off

        case  96903: tmin = -5;
          tmax = 20;
          break; // b field off

        case  96916: tmin = -10;
          tmax = 15;
          break; // b field off

        case 108878: tmin = -99;
          tmax = 99;
          break; // MC
        }
        if (failCut(timecor >= tmin and timecor <= tmax, "track cut: timing cut")) {
          continue;
        }
        DEBUG(timecor << " " << tmin << " " << tmax);
      }
      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(3.); // Past TRT time/Hits selection
        m_Eff_SelectionHisto[isub]->Fill(3., eff); // Past TRT time/Hits selection
      }

      Bool_t enclosingHits(true);
      if (m_isCosmic) {
        if ((*TSOSItr)->trackParameters()) {
          zpos = (*TSOSItr)->trackParameters()->position().z();
          enclosingHits = (zpos > zmin)and(zpos < zmax);
        }else {
          WARNING("No track parameters found. Cannot determine whether it is an enclosed hit.");
          enclosingHits = false;
        }
      }else {
        layerSide =
          (m_sctId->barrel_ec(surfaceID) !=
           0) * N_BARRELS + float(m_sctId->layer_disk(surfaceID)) + (float(m_sctId->side(surfaceID)) == 0) * 0.5;
        enclosingHits = layerSide > min_layerSide and layerSide < max_layerSide;
      }

      if (m_detailed) {
        m_probEnclosedHisto[isub]->Fill(layerPlusHalfSide, Int_t(enclosingHits));
      }

      if (m_requireEnclosingHits and not (layerPlusHalfSide == .5)and not (isub == 1 and layerPlusHalfSide ==
                                                                           3)and not (layerPlusHalfSide == 8)) {
        if (failCut(enclosingHits, "hit cut: enclosing hits")) {
          continue;
        }
      }

      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(4.); //
        m_Eff_SelectionHisto[isub]->Fill(4., eff); // Past nHits cut
      }

      // Now fill with the local z
      Double_t chi2 = trackWithHoles->fitQuality()->chiSquared();
      Double_t ndf = trackWithHoles->fitQuality()->numberDoF();
      Double_t chi2_div_ndf = ndf > 0 ? chi2 / ndf : -1;

      if (m_detailed) {
        m_Eff_phiHisto[isub]->Fill(phiUp, eff);
        if (ndf > 0 and chi2_div_ndf <= m_maxChi2 and otherFaceFound) {
          m_Eff_phiHistoFinal[isub]->Fill(phiUp, eff);
        }
        m_Unas_phiHisto[isub]->Fill(phiUp, unas);
        m_phiLocalHisto[isub]->Fill(phiUp);
        if (fabs(d0) < 50) {
          m_phiLocalCutHisto[isub]->Fill(phiUp);
        }
      }

      if (m_superDetailed and ndf > 0) {
        m_chi2vPhiHisto[isub]->Fill(phiUp, chi2_div_ndf);
      }
      if (failCut(fabs(phiUp) <= m_maxPhiAngle, "hit cut: incidence angle")) {
        continue;
      }
      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(5.); // Past phiUp cut
        m_Eff_SelectionHisto[isub]->Fill(5., eff); // Past phiUp cut
        if (ndf > 0) {
          m_chi2Histo[isub]->Fill(chi2_div_ndf);
          m_Eff_chi2Histo[isub]->Fill(chi2_div_ndf, eff);
          if (otherFaceFound) {
            m_Eff_chi2HistoFinal[isub]->Fill(chi2_div_ndf, eff);
          }
          if (m_superDetailed) {
            m_chi2ResidualHisto[isub]->Fill(trackHitResidual, chi2_div_ndf);
          }
        }
      }

      if (failCut(ndf > 0 and chi2_div_ndf <= m_maxChi2, "track cut: chi2 cut")) {
        continue;
      }

      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(6.); //
        m_Eff_SelectionHisto[isub]->Fill(6., eff); // Past chi2
        const float face(otherFaceFound);
        m_Eff_otherFaceHisto[isub]->Fill(face, eff);
        m_Unas_otherFaceHisto[isub]->Fill(face, unas);
        m_Eff_otherFaceHisto[isub]->Fill(-1, eff);
        m_Unas_otherFaceHisto[isub]->Fill(-1, unas);
      }

      if (m_requireOtherFace and failCut(otherFaceFound, "hit cut: other face found")) {
        continue;
      }

      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(7.); // Past other face selection
        m_Eff_SelectionHisto[isub]->Fill(7., eff); // Past other face selection
      }
      if (not trkParamOnSurface) continue;
      Double_t xl(trkParamOnSurface->localPosition()[0]);
      Double_t yl(trkParamOnSurface->localPosition()[1]);

      if (m_superDetailed) {
        const int layer(m_sctId->layer_disk(surfaceID));
        const int ieta(m_sctId->eta_module(surfaceID));
        m_xlResidualHisto[isub][layer]->Fill(xl, trackHitResidual);
        if (ieta == 0) {
          m_xlResidualE0Histo[isub][layer]->Fill(xl, trackHitResidual);
        }
        if (ieta == 1) {
          m_xlResidualE1Histo[isub][layer]->Fill(xl, trackHitResidual);
        }
        if (unas) {
          m_xlResidualUnasHisto[isub][layer]->Fill(xl, trackHitResidual);
        }
      }
      
      // Check guard ring
      Bool_t insideGuardRing(true);
      if (isub == BARREL_INDEX) {
        float_t xGuard(m_effdistcut);
        float_t yGuard(3.);
        if (xl < -30.7 + xGuard) {
          insideGuardRing = false;
        }
        if (xl > 30.7 - xGuard) {
          insideGuardRing = false;
        }

        Double_t yend(63.960 + 0.03 - 1.);  // The far sensitive end
        Double_t ydead(2.06 / 2.); // the near sensitive end
        if (yl > yend - yGuard) {
          insideGuardRing = false;
        }
        if (yl < -yend + yGuard) {
          insideGuardRing = false;
        }
        if (yl < ydead + yGuard and yl > -ydead - yGuard) {
          insideGuardRing = false;
        }
      } else {
        // No guard ring for the endcaps for now...just set true.
        insideGuardRing = true;
      }

      if (m_detailed) {
        if (eff == 1) {
          if (m_superDetailed) {
            m_localHitHisto[isub]->Fill(xl, yl);
          }
          if (unas) {
            m_localUnasHisto[isub]->Fill(xl, yl);
          }
          m_localHitXHisto[isub]->Fill(xl);
          m_localHitYHistos[isub]->Fill(yl);
        } else {
          m_localMissHisto[isub]->Fill(xl, yl);
        }
        m_Eff_xlocHisto[isub]->Fill(xl, eff);
        m_Eff_ylocHistos[isub]->Fill(yl, eff);
        m_Unas_xlocHisto[isub]->Fill(xl, unas);
        m_Unas_ylocHisto[isub]->Fill(yl, unas);
      }

      if (m_requireGuardRing and failCut(insideGuardRing, "hit cut: inside guard ring")) {
        continue;
      }

      if (eff == 1 and m_superDetailed) {
        m_localHitGHisto[isub]->Fill(xl, yl);
      }

      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(8.); // Past guard ring
        m_Eff_SelectionHisto[isub]->Fill(8., eff); // Past guard ring
      }

      // Check bad chips
      Bool_t nearBadChip(false);
      IdentifierHash waferHash = m_sctId->wafer_hash(surfaceID);
      const InDetDD::SiDetectorElement *pElement = elements->getDetectorElement(waferHash);
      bool swap = (pElement->swapPhiReadoutDirection()) ? true : false;
      Int_t chipPos(previousChip(xl, side, swap));
      unsigned int status(0);
      std::map<Identifier, unsigned int>::const_iterator badChip(m_badChips->find(module_id));
      if (badChip != m_badChips->end()) {
        status = (*badChip).second;
        // Veto if either of closest two chips is dead
        const bool nearBadChipDead = (status & (1 << chipPos)) != 0;
        const bool nextBadChipDead = (status & (1 << (chipPos + 1))) != 0;
        const bool isNotEndChip = (chipPos != 5)and(chipPos != 11); // cant have a 'next' if its the end chip on that
                                                                    // side
        // nearBadChip = status & (1 << chipPos) or
        //  (chipPos != 5 and chipPos != 11 and status & (1 << (chipPos + 1)));
        // clarify logic:
        nearBadChip = nearBadChipDead or(isNotEndChip and nextBadChipDead);
      }
      if (m_vetoBadChips and failCut(not nearBadChip, "hit cut: not near bad chip")) {
        continue;
      }
      VERBOSE("Candidate passed all cuts");
      m_Eff_summaryHisto_old[isub]->Fill(layerPlusHalfSide, eff); // in order to calculate m_EffsummaryIncBadMod
      m_Eff_summaryHisto[isub]->Fill(dedicated_layerPlusHalfSide, eff); // adjustment for dedicated_title()
      m_Eff_hashCodeHisto->Fill(Double_t(sideHash), eff);// 15.12.2014
      m_Eff_LumiBlockHisto[isub]->Fill(pEvent->lumiBlock(), eff);// 20.01.2015
      m_Eff_LumiBlockHisto_Total->Fill(pEvent->lumiBlock(), eff);// 02.09.2016
      if (BCIDpos == 0 && InTrain) {
        m_Eff_summaryHistoFirstBCID[isub]->Fill(dedicated_layerPlusHalfSide, eff); // adjustment for dedicated_title()
      }
      if (m_detailed) {
        m_SelectionHisto[isub]->Fill(9.); // Past bad chip
        m_Eff_SelectionHisto[isub]->Fill(9., eff); // Past bad chip
        m_EventHisto[isub]->Fill(pEvent->eventNumber());
        m_Eff_EventHisto[isub]->Fill(pEvent->eventNumber(), eff);
        m_hashCodeHisto->Fill(Double_t(sideHash));
        m_Unas_summaryHisto[isub]->Fill(layerPlusHalfSide, unas);
        m_Eff_etaHisto[isub]->Fill(eta, eff);
        m_Eff_ptiHisto[isub]->Fill(perigeeCharge / pT, eff);
        m_Unas_ptiHisto[isub]->Fill(perigeeCharge / pT, unas);
        m_Eff_d0Histo[isub]->Fill(d0, eff);
        m_Eff_z0Histo[isub]->Fill(z0, eff);
        m_Eff_ptHisto[isub]->Fill(log10(pT), eff);
        m_ResidualHisto[isub]->Fill(trackHitResidual);
        if (unas == 1) {
          m_ResidualUnasHisto[isub]->Fill(trackHitResidual);
        }
        if (eff == 0) {
          m_ResidualMissHisto[isub]->Fill(trackHitResidual);
        }
        m_Eff_nTrk[isub]->Fill(nTrk, eff);
        m_Eff_nGoodTrk[isub]->Fill(nTrkGood, eff);
      }
      if (m_superDetailed) {
        // m_Eff_LumiBlockHisto[isub]->Fill(pEvent->lumiBlock(), eff);//20.01.2015
        chipPos = (side == 1) ? 11 - chipPos : chipPos;
        m_inEffChip[isub]->Fill(sideHash, chipPos, int(eff == 0));
        m_inEffStrip[isub]->Fill(sideHash, xl / 79.95e-3 + 768. / 2., int(eff == 0));
        // Find the opposite side of this module,
        if (side == 0) {
          Double_t trackHitResidual2(999);
          std::map < Identifier, Double_t >::const_iterator otherItr(mapOfTrackHitResiduals.find(otherSideSurfaceID));
          if (otherItr != mapOfTrackHitResiduals.end()) {
            trackHitResidual2 = otherItr->second;
          }
          m_TwoSidesResiduals[isub]->Fill(trackHitResidual, trackHitResidual2);
          if (eff == 0 or unas == 1) {
            m_TwoSidesResidualsIneff[isub]->Fill(trackHitResidual, trackHitResidual2);
          }
        }
        const int layerSideIndex = int(layerPlusHalfSide * 2);
        layersCrossedByTrack[isub][layerSideIndex] = true;
        nHolesDistOnLayer[isub][layerSideIndex] += int(eff == 0);
        nHolesOnLayer[isub][layerSideIndex] += int((*TSOSItr)->type(Trk::TrackStateOnSurface::Hole));
        if (fabs(trackHitResidual) < 65) {
          m_layerResidualHistos[isub][layerSideIndex]->Fill(m_sctId->eta_module(surfaceID),
                                                            m_sctId->phi_module(surfaceID), trackHitResidual);
        }
      }
      m_Eff_Total->Fill(Double_t(isub), eff);
      if (BCIDpos == 0 && InTrain) {
        m_Eff_TotalBCID->Fill(Double_t(isub), eff);
      }
      useDetector[isub] = true;
      const int ieta(m_sctId->eta_module(surfaceID));
      const int iphi(m_sctId->phi_module(surfaceID));
      m_effMap[histnumber][side]->Fill(ieta, iphi, eff);
      m_effLumiBlock[histnumber][side]->Fill(pEvent->lumiBlock(), eff);// 23.01.2015

      if (testOffline) {
        m_ineffMap[histnumber][side]->Fill(ieta, iphi, 1); // dummyfill for testing
      }else {
        m_ineffMap[histnumber][side]->Fill(ieta, iphi, (eff == 0));
      }
      if (testOffline) {
        INFO("Filling " << histnumber << ", " << side << " eta " << ieta << " phi " << iphi);
      }
    } // End of loop over hits/holes
    if (m_detailed) {
      m_etaTkHisto->Fill(eta);
      m_d0TkHisto->Fill(d0);
      m_z0TkHisto->Fill(z0);
      m_PtTkHisto->Fill(log10(pT));
      m_mNHitHisto->Fill(NHits[ENDCAP_C_INDEX]);
      m_pNHitHisto->Fill(NHits[ENDCAP_A_INDEX]);
      m_barrelNHitHisto->Fill(NHits[BARREL_INDEX]);
      m_SCTNHitHisto->Fill(sctNHits);
      m_pixelNHitHisto->Fill(pixelNHits);
      m_trtNHitHisto->Fill(trtNHits);
      for (Int_t thisRegion(0); thisRegion != N_REGIONS; ++thisRegion) {
        if (useDetector[thisRegion]) {
          m_etaTkUsedHisto[thisRegion]->Fill(eta);
          m_d0TkUsedHisto[thisRegion]->Fill(d0);
          m_z0TkUsedHisto[thisRegion]->Fill(z0);
          m_phiTkUsedHisto[thisRegion]->Fill(phi);
          m_ptiTkUsedHisto[thisRegion]->Fill(perigeeCharge / pT);
          m_ptTkUsedHisto[thisRegion]->Fill(log10(pT));
        }
      }
    }

    if (m_superDetailed) {
      for (Int_t isub(0); isub != N_REGIONS; ++isub) {
        for (Int_t i(0); i != n_layers[isub] * 2; ++i) {
          if (layersCrossedByTrack[isub][i]) {
            m_holesPerTrackHisto[isub]->Fill(float(i) / 2, nHolesOnLayer[isub][i]);
            m_holesDistPerTrackHisto[isub]->Fill(float(i) / 2, nHolesOnLayer[isub][i]);
          }
        }
      }
    }
    delete trackWithHoles;
    trackWithHoles = 0;
  }
  VERBOSE("finished loop over tracks = " << (*tracks).size());
  if (m_detailed) {
    m_nTrkHisto->Fill(nTrk);
    m_nTrkParsHisto->Fill(nTrkPars);
    m_nTrkGoodHisto->Fill(nTrkGood);
  }
  if (m_superDetailed) {
    m_LumiBlock->Fill(pEvent->lumiBlock());
  }
  m_countEvent++;
  if (m_chronotime) {
    m_chrono->chronoStop("SCTHitEff");
  }
  return StatusCode::SUCCESS;
}

// StatusCode SCTHitEffMonTool::procHistograms(bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/, bool
// /*isEndOfRun*/){ // hidetoshi 14.01.22
StatusCode
SCTHitEffMonTool::procHistograms() {                                                                             // hidetoshi
                                                                                                                 // 14.01.22
  if (m_superDetailed) {
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
    if (elements==nullptr) {
      ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved in SyncDisabledSCT()");
      return StatusCode::FAILURE;
    }

    const std::set<Identifier> *badModules = m_configConditions->badModules();
    INFO("Found " << badModules->size() << " bad modules");
    std::array < std::array < double, N_ENDCAPS >, N_REGIONS > MaxEta;
    std::array < std::array < double, N_ENDCAPS >, N_REGIONS > MaxPhi;
    for (int isub(0); isub < N_REGIONS; ++isub) {
      MaxEta[isub].fill(-1e3);
      MaxPhi[isub].fill(-1e3);
    }
    std::array < std::array < std::vector<double>, N_ENDCAPS >, N_REGIONS > etabins;
    std::array < std::array < std::vector<double>, N_ENDCAPS >, N_REGIONS > phibins;
    double etaMin(0.), etaMax(0.), phiMin(0.), phiMax(0.), rz(0.);
    std::map < Identifier, Float_t > mapOfInEff;
    for (std::vector<Identifier>::const_iterator wafItr(m_sctId->wafer_begin()); wafItr != m_sctId->wafer_end();
         ++wafItr) {
      Int_t side(m_sctId->side(*wafItr));
      if (side == 1) {
        continue;
      }
      Int_t isub(bec2Index(m_sctId->barrel_ec(*wafItr)));
      Int_t layer(m_sctId->layer_disk(*wafItr));
      IdentifierHash waferHash = m_sctId->wafer_hash(*wafItr);
      const InDetDD::SiDetectorElement *element = elements->getDetectorElement(waferHash);
      const Amg::Vector3D position = element->center();
      element->getEtaPhiRegion(0., etaMin, etaMax, phiMin, phiMax, rz);
      etabins[isub][layer].push_back(etaMin);
      phibins[isub][layer].push_back(phiMin);
      MaxEta[isub][layer] = std::max(etaMax, MaxEta[isub][layer]);
      MaxPhi[isub][layer] = std::max(phiMax, MaxPhi[isub][layer]);
      float dPhi(fabs(phiMax - phiMin));
      if (dPhi > M_PI) {
        dPhi = 2 * M_PI - dPhi;
      }
      float dEta(fabs(etaMax - etaMin));
      std::set< Identifier >::const_iterator bMItr(badModules->find(*wafItr));
      Float_t eff(Float_t(bMItr == badModules->end()));
      Float_t accSide[2] = {
        eff, eff
      };
      if (eff == 1) {
        std::map< Identifier, unsigned int >::const_iterator badChip(m_badChips->find(*wafItr));
        if (badChip != m_badChips->end()) {
          unsigned int status((*badChip).second);
          for (Int_t i(0); i < 2 * N_CHIPS; ++i) {
            if ((status & (1 << i)) != 0) {
              accSide[int(i >= N_CHIPS)] -= 1. / N_CHIPS;
            }
          }
          eff = (accSide[0] + accSide[1]) / 2;
        }
      }else {
        VERBOSE("Bad module : " << *wafItr << " " << m_sctId->wafer_hash(*wafItr));
        m_badModMap->SetPoint(m_badModMap->GetN(), amgPseudoRapidity(position), position.phi());
        m_badModMap->SetPointError(m_badModMap->GetN() - 1, dEta / 2, dPhi / 2);
        m_badModMap->GetXaxis()->SetRangeUser(-3, 3);
        m_badModMap->GetYaxis()->SetRangeUser(-3.4, 3.4);
        for (int i(1); i <= m_badModFineMap->GetNbinsX(); ++i) {
          Float_t binx(m_badModFineMap->GetXaxis()->GetBinCenter(i));
          for (int j(1); j <= m_badModFineMap->GetNbinsY(); ++j) {
            Float_t biny(m_badModFineMap->GetYaxis()->GetBinCenter(j));
            if ((binx > etaMin)and(binx < etaMax) and(biny > phiMin) and(biny < phiMax)) {
              m_badModFineMap->Fill(binx, biny);
            }
          }
        }
      }
      for (int i(0); i != 2; ++i) {
        m_badModPerSide[isub]->Fill(layer + i * 0.5, accSide[i]);
      }
      if (eff == 1) {
        continue;
      }
      mapOfInEff[*wafItr] = eff;
      Int_t histnumber(becIdxLayer2Index(isub, layer));
      if (histnumber == -1) {
        WARNING("Barrel-or-endcap index is invalid");
        return StatusCode::FAILURE;
      }
      m_accMap[histnumber]->Fill(m_sctId->eta_module(*wafItr), m_sctId->phi_module(*wafItr), (1. - eff) * N_CHIPS * 2);
      m_badChipMap->SetPoint(m_badChipMap->GetN(), amgPseudoRapidity(position), position.phi());
      m_badChipMap->SetPointError(m_badChipMap->GetN() - 1, dEta / 2, dPhi / 2);
      m_badChipMap->GetXaxis()->SetRangeUser(-3, 3);
      m_badChipMap->GetYaxis()->SetRangeUser(-3.4, 3.4);
    }

    std::array < MonGroup, N_REGIONS + 1 > histGroupE = {{
      MonGroup(this, m_path + histogramPath[ENDCAP_C_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[BARREL_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[ENDCAP_A_INDEX], run, ATTRIB_UNMANAGED),
      MonGroup(this, m_path + histogramPath[GENERAL_INDEX], run, ATTRIB_UNMANAGED)
    }};

    for (Int_t isub(0); isub != N_REGIONS; ++isub) {
      for (Long_t i(0); i != n_layers[isub]; ++i) {
        etabins[isub][i].push_back(MaxEta[isub][i]);
        phibins[isub][i].push_back(MaxPhi[isub][i]);
        std::sort(etabins[isub][i].begin(), etabins[isub][i].end());
        std::sort(phibins[isub][i].begin(), phibins[isub][i].end());
        Int_t histnumber(becIdxLayer2Index(isub, i));
        if (histnumber < 0) {
          WARNING("Barrel-or-endcap index is invalid");
          return StatusCode::FAILURE;
        }
        CHECK(bookEffHisto(m_accPhysMap[histnumber], histGroupE[isub],
                           "nDisabledChipsPhys_" + subDetNameShort[isub] + "_" + i, "",
                           etabins[isub][i].size() - 1, &etabins[isub][i][0], phibins[isub][i].size() - 1,
                           &phibins[isub][i][0]));
        m_accPhysMap[histnumber]->GetZaxis()->SetRangeUser(1, N_CHIPS * 2 + 1);
        m_accPhysMap[histnumber]->GetZaxis()->SetNdivisions(N_CHIPS * 2, false);
        m_accPhysMap[histnumber]->SetContour(N_CHIPS * 2);
        m_accMap[histnumber]->GetZaxis()->SetRangeUser(1, N_CHIPS * 2 + 1);
        m_accMap[histnumber]->GetZaxis()->SetNdivisions(N_CHIPS * 2, false);
        m_accMap[histnumber]->SetContour(N_CHIPS * 2);
        for (float j(0); j != 2; ++j) {
          const int binNumber(1 + 2 * i + j);
          const int numberOfBins(m_Eff_summaryHisto_old[isub]->GetBinEntries(binNumber));
          const int summaryBinIndex(i + j / 2);
          const float summaryBinContent(m_badModPerSide[isub]->GetBinContent(
                                          binNumber) * m_Eff_summaryHisto_old[isub]->GetBinContent(binNumber));
          for (int e(0); e != numberOfBins; ++e) {
            m_Eff_summaryIncBadMod[isub]->Fill(summaryBinIndex, summaryBinContent);
          }
        }
      }
    }
    for (std::map< Identifier, Float_t >::const_iterator bMod(mapOfInEff.begin()); bMod != mapOfInEff.end(); ++bMod) {
      Identifier surfaceID(bMod->first);
      Int_t histnumber(becIdxLayer2Index(bec2Index(m_sctId->barrel_ec(surfaceID)), m_sctId->layer_disk(surfaceID)));
      if (histnumber < 0) {
        WARNING("Barrel-or-endcap index is invalid");
        return StatusCode::FAILURE;
      }
      IdentifierHash waferHash = m_sctId->wafer_hash(surfaceID);
      const InDetDD::SiDetectorElement *element = elements->getDetectorElement(waferHash);
      //      const HepGeom::Point3D<float> position = element->center();
      //      m_accPhysMap[histnumber]->Fill(position.pseudoRapidity(), position.phi(), (1. - bMod->second) * N_CHIPS *
      // 2);
      const Amg::Vector3D position = element->center();
      m_accPhysMap[histnumber]->Fill(amgPseudoRapidity(position), position.phi(), (1. - bMod->second) * N_CHIPS * 2);
    }
  }

  m_effdistribution->Reset();
  for (Int_t isub(0); isub != N_REGIONS; ++isub) {
    for (Long_t i(0); i != n_layers[isub]; ++i) {
      const int detIndex(becIdxLayer2Index(isub, i));
      if (detIndex != -1) {
        for (Long_t j(0); j != 2; ++j) {
          for (int xbin = 1; xbin < m_effMap[detIndex][j]->GetNbinsX() + 1; xbin++) {
            for (int ybin = 1; ybin < m_effMap[detIndex][j]->GetNbinsY() + 1; ybin++) {
              if (m_effMap[detIndex][j]->GetBinEntries(m_effMap[detIndex][j]->GetBin(xbin, ybin)) == 0) {
                continue;
              }
              m_effdistribution->Fill(m_effMap[detIndex][j]->GetBinContent(xbin, ybin));
            }
          }
        }
      }//if detIndex !=-1
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
SCTHitEffMonTool::failCut(Bool_t value, std::string name) {
  if (value) {
    VERBOSE("Passed " << name);
    return StatusCode::FAILURE;
  }
  VERBOSE("Failed " << name);
  return StatusCode::SUCCESS;
}

Int_t
SCTHitEffMonTool::previousChip(Double_t xl, Int_t side, bool swap) {
  Float_t nStrips(768.);
  Float_t stripWidth(79.95e-3);
  Float_t nChips(6.);
  Float_t xLeftEdge(xl + nStrips / 2. * stripWidth); // xl defined wrt center of module, convert to edge of module
  Int_t chipPos(int(xLeftEdge / (stripWidth * nStrips) * nChips));

  if (side == 0) {
    chipPos = swap ? 5 - chipPos : chipPos;
  } else {
    chipPos = swap ? 11 - chipPos : 6 + chipPos;
  }
  return chipPos;
}

StatusCode
SCTHitEffMonTool::findAnglesToWaferSurface(const Amg::Vector3D &mom, const Identifier id, 
                                           const InDetDD::SiDetectorElementCollection* elements,
                                           Double_t &theta, Double_t &phi) {
  phi = 90.;
  theta = 90.;

  const Identifier waferId = m_sctId->wafer_id(id);
  const IdentifierHash waferHash = m_sctId->wafer_hash(waferId);
  const InDetDD::SiDetectorElement *element = elements->getDetectorElement(waferHash);
  if (not element) {
    VERBOSE("findAnglesToWaferSurface: failed to find detector element for id = " << m_sctId->print_to_string(id));
    return StatusCode::FAILURE;
  }
  //  Double_t pNormal(mom * element->normal());
  //  Double_t pEta(mom * element->etaAxis());
  //  Double_t pPhi(mom * element->phiAxis());
  Double_t pNormal(mom.dot(element->normal()));
  Double_t pEta(mom.dot(element->etaAxis()));
  Double_t pPhi(mom.dot(element->phiAxis()));
  if (pPhi < 0.) {
    phi = -90.;
  }
  if (pEta < 0.) {
    theta = -90.;
  }
  if (pNormal != 0.) {
    phi = atan(pPhi / pNormal) * radianDegrees;
    theta = atan(pEta / pNormal) * radianDegrees;
  }
  return StatusCode::SUCCESS;
}

template <class T> StatusCode
SCTHitEffMonTool::bookEffHisto(T * &histo, MonGroup &MG, TString name, TString title, Int_t nbin, Double_t x1,
                               Double_t x2) {
  histo = new T(name, title, nbin, x1, x2);
  CHECK(MG.regHist(histo));
  VERBOSE("Registered " << name << " at " << histo);
  return StatusCode::SUCCESS;
}

template <class T> StatusCode
SCTHitEffMonTool::bookEffHisto(T * &histo, MonGroup &MG, TString name, TString title,
                               Int_t nbinx, Double_t x1, Double_t x2,
                               Int_t nbiny, Double_t y1, Double_t y2) {
  histo = new T(name, title, nbinx, x1, x2, nbiny, y1, y2);
  CHECK(MG.regHist(histo));
  VERBOSE("Registered " << name << " at " << histo);
  return StatusCode::SUCCESS;
}

template <class T> StatusCode
SCTHitEffMonTool::bookEffHisto(T * &histo, MonGroup &MG, TString name, TString title,
                               Int_t nbinx, Double_t *xbins, Int_t nbiny, Double_t *ybins) {
  histo = new T(name, title, nbinx, xbins, nbiny, ybins);
  CHECK(MG.regHist(histo));
  VERBOSE("Registered " << name << " at " << histo);
  return StatusCode::SUCCESS;
}

// Find the residual to track.
Double_t
SCTHitEffMonTool::getResidual(const Identifier &surfaceID, const Trk::TrackParameters *trkParam,
                              const InDet::SCT_ClusterContainer *p_sctclcontainer) {
  Double_t trackHitResidual(-999.);

  if (not trkParam) {
    WARNING("Not track parameters found. Returning default residual value.");
    return trackHitResidual;
  }
  IdentifierHash idh(m_sctId->wafer_hash(surfaceID));
  InDet::SCT_ClusterContainer::const_iterator containerIterator(p_sctclcontainer->indexFind(idh));
  InDet::SCT_ClusterContainer::const_iterator containerEnd(p_sctclcontainer->end());
  for (; containerIterator != containerEnd; ++containerIterator) {
    for (InDet::SCT_ClusterCollection::const_iterator rioIterator((*containerIterator)->begin());
         rioIterator != (*containerIterator)->end(); ++rioIterator) {
      if (not (*rioIterator)or not (*rioIterator)->detectorElement()) {
        WARNING("NULL pointer to RIO or detElement");
        continue;
      }
      if (m_sctId->wafer_id(surfaceID) == m_sctId->wafer_id(((*rioIterator)->detectorElement())->identify())) {
        const Trk::PrepRawData *rioo(dynamic_cast<const Trk::PrepRawData *>(*rioIterator));
        const Trk::RIO_OnTrack *rio(m_rotcreator->correct(*rioo, *trkParam));
        if (!m_residualPullCalculator.empty()) {
          //          if(m_residualPullCalculator->residualPull(rio, trkParam,Trk::ResidualPull::Unbiased)==0)continue;
          const Trk::ResidualPull *residualPull(m_residualPullCalculator->residualPull(rio, trkParam,
                                                                                       Trk::ResidualPull::Unbiased));
          if (residualPull == 0) {
            continue;
          }
          if (fabs(residualPull->residual()[Trk::loc1]) < fabs(trackHitResidual)) {
            trackHitResidual = residualPull->residual()[Trk::loc1];
          }
          delete residualPull;
        }
        delete rio;
      }
    }
  }
  return trackHitResidual;
}
