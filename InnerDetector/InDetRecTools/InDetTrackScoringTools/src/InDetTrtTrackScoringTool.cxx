/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackScoringTools/InDetTrtTrackScoringTool.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"

//---------------------------------------------------------------------------------------------------------------------

InDet::InDetTrtTrackScoringTool::InDetTrtTrackScoringTool(const std::string& t,
                                                          const std::string& n,
                                                          const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_trtId(nullptr)
  , m_maxSigmaChi2(-1)
  , m_maxTrtRatio(-1)
  , m_maxTrtFittedRatio(-1)
  , m_summaryTypeScore(Trk::numberOfTrackSummaryTypes)
  , m_selectortool("InDet::InDetTrtDriftCircleCutTool")
{
  declareInterface<Trk::ITrackScoringTool>(this);

  // cuts for tracks

  // There is room for 10 bins, for future development.
  // The default below represents one eta bin between 0 and 999
  // and no cuts applied.
  m_TRTTrksEtaBins.clear();
  m_TRTTrksMinTRTHitsThresholds.clear();
  m_TRTTrksMinTRTHitsMuDependencies.clear();
  for (unsigned int i = 0; i < 10; ++i) {
    m_TRTTrksEtaBins.push_back(999);
    m_TRTTrksMinTRTHitsThresholds.push_back(0);
    m_TRTTrksMinTRTHitsMuDependencies.push_back(0);
  }

  // declare properties
  declareProperty("DriftCircleCutTool", m_selectortool);
  declareProperty("useAmbigFcn", m_useAmbigFcn = true);
  declareProperty("useSigmaChi2", m_useSigmaChi2 = false);
  declareProperty("minTRTonTrk", m_minTRTonTrk = 15);
  declareProperty("maxEta", m_maxEta = 2.1);
  declareProperty("PtMin", m_ptmin = 1.0); // pt min cut
  declareProperty("UseParameterization", m_parameterization = true);
  declareProperty("OldTransitionLogic", m_oldLogic = false);
  declareProperty("minTRTPrecisionFraction", m_minTRTprecision = 0.5);
  declareProperty("TRTTrksEtaBins", m_TRTTrksEtaBins); /* expects 10 eta bins (set unused bins to e.g. 999) */
  declareProperty("TRTTrksMinTRTHitsThresholds", m_TRTTrksMinTRTHitsThresholds);         /* expects 10 values */
  declareProperty("TRTTrksMinTRTHitsMuDependencies", m_TRTTrksMinTRTHitsMuDependencies); /* expects 10 values */
  m_summaryTypeScore[Trk::numberOfTRTHits] = 1;                                          // 10 straws ~ 1 SCT
  m_summaryTypeScore[Trk::numberOfTRTHighThresholdHits] = 0;                             // addition for being TR
}

//---------------------------------------------------------------------------------------------------------------------

InDet::InDetTrtTrackScoringTool::~InDetTrtTrackScoringTool() {}

//---------------------------------------------------------------------------------------------------------------------

StatusCode
InDet::InDetTrtTrackScoringTool::initialize()
{
  ATH_CHECK(m_selectortool.retrieve( DisableTool{m_selectortool.empty() } ));
  ATH_CHECK(m_lumiBlockMuTool.retrieve());

  if (detStore()->retrieve(m_trtId, "TRT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get TRT_ID helper !");
    return StatusCode::FAILURE;
  }
  if (m_useAmbigFcn) {
    setupTRT_ScoreModifiers();
  }

  // Read handle for AtlasFieldCacheCondObj
  ATH_CHECK(m_fieldCacheCondObjInputKey.initialize());

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------

StatusCode
InDet::InDetTrtTrackScoringTool::finalize()
{
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------

Trk::TrackScore
InDet::InDetTrtTrackScoringTool::score(const Trk::Track& track, const bool suppressHoleSearch) const
{
  (void)suppressHoleSearch;
  if (!track.trackSummary()) {
    ATH_MSG_FATAL("Track without a summary");
  }
  Trk::TrackScore score = Trk::TrackScore(simpleScore(track, *track.trackSummary()));
  return score;
}

//---------------------------------------------------------------------------------------------------------------------

Trk::TrackScore
InDet::InDetTrtTrackScoringTool::simpleScore(const Trk::Track& track, const Trk::TrackSummary& trackSummary) const
{
  int numTRT = trackSummary.get(Trk::numberOfTRTHits);
  int numTRTTube = trackSummary.get(Trk::numberOfTRTTubeHits);

  // TRT precision hits cut
  if (numTRT >= 15 && ((double)(numTRT - numTRTTube)) / numTRT < m_minTRTprecision) {
    return Trk::TrackScore(0);
  }

  const Trk::Perigee* perigee = track.perigeeParameters();
  int numTRT_plusOutliers = numTRT + trackSummary.get(Trk::numberOfTRTOutliers);
  unsigned int eta_bin = getEtaBin(*perigee);
  double minTRT = getMuDependentNtrtMinCut(eta_bin);

  // nTRT cut from egamma optimization
  if (numTRT_plusOutliers < minTRT) {
    return Trk::TrackScore(0);
  }

  // Cut on the minimum number of hits
  bool isGood = m_selectortool.isEnabled() ? isGoodTRT(track) : true;
  if (!isGood) {
    return Trk::TrackScore(0);
  }

  // get parameters without error - this is faster
  const Trk::TrackParameters* input = track.trackParameters()->front();

  /// Reject track below the pT cut

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{ m_fieldCacheCondObjInputKey, ctx };
  const AtlasFieldCacheCondObj* fieldCondObj{ *readHandle };
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("simpleScore: Failed to retrieve AtlasFieldCacheCondObj with key "
                  << m_fieldCacheCondObjInputKey.key());
    return Trk::TrackScore(0);
  }
  MagField::AtlasFieldCache fieldCache;
  fieldCondObj->getInitializedCache(fieldCache);

  if (fieldCache.solenoidOn()) { // B field
    if (input->pT() < m_ptmin) {
      ATH_MSG_DEBUG("Reject track below Pt cut !");
      return Trk::TrackScore(0);
    }
  }

  if (fabs(input->eta()) > m_maxEta) {
    return Trk::TrackScore(0);
  }
  //
  // --- Now Start Scoring
  //

  if (m_useAmbigFcn) {

    //
    //  -- modern score
    //
    return TRT_ambigScore(track, trackSummary);

  } else {

    //
    // --classical scoring !
    //
    // score of 100 per track
    Trk::TrackScore score(100);
    // --- summary score analysis
    for (int i = 0; i < Trk::numberOfTrackSummaryTypes; ++i) {
      int value = trackSummary.get(static_cast<Trk::SummaryType>(i));
      // value is -1 if undefined.
      if (value > 0) {
        score += m_summaryTypeScore[i] * value;
      }
    }
    // --- prob(chi2,NDF), protect for chi2 <= 0
    if (track.fitQuality() != nullptr && track.fitQuality()->chiSquared() > 0 && track.fitQuality()->numberDoF() > 0) {
      double p = 1.0 - Genfun::CumulativeChiSquare(track.fitQuality()->numberDoF())(track.fitQuality()->chiSquared());
      if (p > 0)
        score += log10(p);
      else
        score -= 50;
    }
    return score;
  }
}

//---------------------------------------------------------------------------------------------------------------

Trk::TrackScore
InDet::InDetTrtTrackScoringTool::TRT_ambigScore(const Trk::Track& track, const Trk::TrackSummary& trackSummary) const
{
  //
  // --- start with bonus for high pt tracks
  //
  // double prob = 1.;
  double pt = fabs(track.trackParameters()->front()->pT());
  double prob = log10(pt) - 1.; // 100 MeV is min and gets score 1

  //
  // --- special treatment for TRT hits
  //
  int iTRT_Hits = trackSummary.get(Trk::numberOfTRTHits);
  int iTRT_Outliers = trackSummary.get(Trk::numberOfTRTOutliers);
  //

  if (iTRT_Hits > 0 && m_maxTrtRatio > 0) {
    // get expected number of TRT hits
    double nTrtExpected = 30.;
    assert( m_selectortool.isEnabled());
    nTrtExpected = m_selectortool->minNumberDCs(track.trackParameters()->front());
    double ratio = iTRT_Hits / nTrtExpected;
    if (ratio > m_boundsTrtRatio[m_maxTrtRatio])
      ratio = m_boundsTrtRatio[m_maxTrtRatio];
    for (int i = 0; i < m_maxTrtRatio; ++i) {
      if (m_boundsTrtRatio[i] < ratio && ratio <= m_boundsTrtRatio[i + 1]) {
        prob *= m_factorTrtRatio[i];
        break;
      }
    }
  }

  if (iTRT_Hits > 0 && iTRT_Outliers >= 0 && m_maxTrtFittedRatio > 0) {
    double fitted = double(iTRT_Hits) / double(iTRT_Hits + iTRT_Outliers);
    if (fitted > m_boundsTrtFittedRatio[m_maxTrtFittedRatio])
      fitted = m_boundsTrtFittedRatio[m_maxTrtFittedRatio];
    for (int i = 0; i < m_maxTrtFittedRatio; ++i) {
      if (fitted <= m_boundsTrtFittedRatio[i + 1]) {
        prob *= m_factorTrtFittedRatio[i];
        break;
      }
    }
  }

  //
  // --- non binned Chi2

  if (track.fitQuality() != nullptr && track.fitQuality()->chiSquared() > 0 && track.fitQuality()->numberDoF() > 0) {
    int indf = track.fitQuality()->numberDoF();
    double chi2 = track.fitQuality()->chiSquared();
    double fac = 1. / log10(10. + 10. * chi2 / indf); // very soft chi2
    prob *= fac;
  }

  //
  // --- do we use the binned prob for chi2/NDF or sigma chi2 ?
  //
  if ((m_useSigmaChi2) && track.fitQuality()) {
    int indf = track.fitQuality()->numberDoF();
    double ichi2 = track.fitQuality()->chiSquared();
    if (indf > 0) {
      //
      // --- binned sigma chi2 score
      //
      if (m_useSigmaChi2) {
        int sigmaChi2times100 = trackSummary.get(Trk::standardDeviationOfChi2OS);
        if (sigmaChi2times100 > 0) {
          double testvar = double(sigmaChi2times100) / 100. - sqrt(2. * ichi2 / indf);
          if (testvar < m_boundsSigmaChi2[0]) {
            prob *= m_factorSigmaChi2[0];
          } else if (m_boundsSigmaChi2[m_maxSigmaChi2] <= testvar) {
            prob *= m_factorSigmaChi2[m_maxSigmaChi2 - 1];
          } else {
            for (int i = 0; i < m_maxSigmaChi2; ++i) {
              if (m_boundsSigmaChi2[i] <= testvar && testvar < m_boundsSigmaChi2[i + 1]) {
                prob *= m_factorSigmaChi2[i];
              }
            }
          }
        }
      }
    }
  }

  Trk::TrackScore score(prob);
  return score;
}

//-----------------------------------------------------------------------------------------------------------
void
InDet::InDetTrtTrackScoringTool::setupTRT_ScoreModifiers()
{

  //
  // --- ratio of TRT hits over expected
  //
  constexpr int maxTrtRatio = 7;
  constexpr double TrtRatioBounds[maxTrtRatio + 1] = { 0,   0.2, 0.4, 0.6,
                                                       0.8, 1.0, 1.2, 2.4 };
  // this needs tuning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  constexpr double goodTrtRatio[maxTrtRatio] = { 0.05, 0.11, 0.12, 0.15,
                                                 0.20, 0.16, 0.17 };
  constexpr double fakeTrtRatio[maxTrtRatio] = { 0.6,  0.08, 0.06, 0.05,
                                                 0.04, 0.03, 0.03 };
  // put it into the private members
  m_maxTrtRatio = m_selectortool.isEnabled() ? maxTrtRatio : 0;
  for (int i = 0; i < m_maxTrtRatio; ++i)
    m_factorTrtRatio.push_back(goodTrtRatio[i] / fakeTrtRatio[i]);
  for (int i = 0; i <= m_maxTrtRatio; ++i)
    m_boundsTrtRatio.push_back(TrtRatioBounds[i]);

  //
  // --- ratio of TRT fitted to (fitted+outliers)
  //
  const int maxTrtFittedRatio = 4;
  const double TrtFittedRatioBounds[maxTrtFittedRatio + 1] = { 0, 0.3, 0.6, 0.9, 1.0 };
  // this needs tuning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  const double goodTrtFittedRatio[maxTrtFittedRatio] = { 0.1, 0.2, 0.3, 0.5 };
  const double fakeTrtFittedRatio[maxTrtFittedRatio] = { 0.6, 0.1, 0.1, 0.1 };
  // put it into the private members
  m_maxTrtFittedRatio = maxTrtFittedRatio;
  for (int i = 0; i < m_maxTrtFittedRatio; ++i)
    m_factorTrtFittedRatio.push_back(goodTrtFittedRatio[i] / fakeTrtFittedRatio[i]);
  for (int i = 0; i <= m_maxTrtFittedRatio; ++i)
    m_boundsTrtFittedRatio.push_back(TrtFittedRatioBounds[i]);

  //
  // --- sigma chi2
  //
  if (!m_useSigmaChi2) {
    // do not use it !
    m_maxSigmaChi2 = -1;
  } else {
    constexpr int maxSigmaChi2 = 26;
    constexpr double SigmaChi2Bounds[maxSigmaChi2 + 1] = {
      -5.0, -4.5, -4.0, -3.5, -3.0, -2.5, -2.0, -1.5, -1.0,
      -0.5, 0.0,  0.5,  1.0,  1.5,  2.0,  2.5,  3.0,  3.5,
      4.0,  4.5,  5.0,  5.5,  6.0,  6.5,  7.0,  7.5,  8.0
    };
    constexpr double modiSigmaChi2[maxSigmaChi2] = {
      0.0001, 0.0001, 0.0001, 0.0001, 0.001,  0.005,  0.024,  0.255, 0.644,
      0.045,  0.008,  0.005,  0.004,  0.003,  0.002,  0.001,  0.001, 0.001,
      0.001,  0.001,  0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001
    };
    constexpr double vetoSigmaChi2[maxSigmaChi2] = {
      0.001, 0.001, 0.001, 0.003, 0.014, 0.030, 0.079, 0.244, 0.295,
      0.064, 0.029, 0.028, 0.030, 0.026, 0.023, 0.023, 0.021, 0.019,
      0.016, 0.018, 0.012, 0.009, 0.007, 0.005, 0.001, 0.001
    };
    // put it into the private members
    m_maxSigmaChi2 = maxSigmaChi2;
    for (int i = 0; i < maxSigmaChi2; ++i) {
      if (vetoSigmaChi2[i] == 0.0)
        m_factorSigmaChi2.push_back(1.0);
      else
        m_factorSigmaChi2.push_back(modiSigmaChi2[i] / vetoSigmaChi2[i]);
    }
    for (int i = 0; i < maxSigmaChi2; ++i)
      m_boundsSigmaChi2.push_back(SigmaChi2Bounds[i]);
  }
}

bool
InDet::InDetTrtTrackScoringTool::isGoodTRT(const Trk::Track& track) const
{

  int nTRT = 0;

  // some cointer for the custom cuts
  int nEC = 0;
  int nBRL = 0;
  int firstWheel = -999;
  int lastLayer = -999;

  // get list of measurements
  const DataVector<const Trk::MeasurementBase>* trkV = track.measurementsOnTrack();

  // loop over the measurements
  DataVector<const Trk::MeasurementBase>::const_iterator im, ime = trkV->end();
  for (im = trkV->begin(); im != ime; ++im) {

    const InDet::TRT_DriftCircleOnTrack* trtcircle = nullptr;
    // Make sure is not a pseudomeasurement
    if ((*im)->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
      const Trk::RIO_OnTrack* rot = static_cast<const Trk::RIO_OnTrack*>((*im));
      // is it really a TRT ?
      if (rot->rioType(Trk::RIO_OnTrackType::TRT_DriftCircle)) {
        trtcircle = static_cast<const InDet::TRT_DriftCircleOnTrack*>(*im);
      }
    }
    if (!trtcircle) {
      continue;
    }

    // increment measurment
    ++nTRT;

    // compute some transition area variables...
    if (!m_parameterization && m_oldLogic) {
      Identifier id = trtcircle->detectorElement()->identify();
      int isB = m_trtId->barrel_ec(id);
      if (isB == 2 || isB == -2)
        nEC++;
      else if (isB == 1 || isB == -1)
        nBRL++;
      if (nBRL > 0)
        lastLayer = m_trtId->layer_or_wheel(id);
      if (nEC == 1)
        firstWheel = m_trtId->layer_or_wheel(id);
    }
  }

  if (!m_parameterization) {

    bool toLower = false;

    // Cases where the min number of required TRT drift circles drops to 10
    if (m_oldLogic && int(trkV->size()) <= m_minTRTonTrk) {
      if ((nEC > 0 && nBRL > 0) || (nEC == 0 && nBRL > 0 && lastLayer < 2) ||
          (nEC > 0 && nBRL == 0 && (firstWheel > 10 || firstWheel < 2))) {
        toLower = true;
      }
    }

    if ((int(trkV->size()) > m_minTRTonTrk) || toLower) {
      return true; // Ask for a minimum number of TRT hits to process
    } else {
      return false;
    }
  } else {

    // new pass, this is using the parameterization
    const DataVector<const Trk::TrackParameters>* vpar = track.trackParameters();
    const Trk::TrackParameters* par = (*vpar)[0];
    int nCutTRT = m_minTRTonTrk;
    assert( m_selectortool.isEnabled() );
    int expected = m_selectortool->minNumberDCs(par);
    if (expected > m_minTRTonTrk)
      nCutTRT = expected;
    return nTRT > nCutTRT;
  }
}

unsigned int
InDet::InDetTrtTrackScoringTool::getEtaBin(const Trk::Perigee& perigee) const
{
  // Find the correct bin for applying eta-dependent cuts

  double tanThetaOver2 = std::tan(perigee.parameters()[Trk::theta] / 2.);
  double abs_eta = (tanThetaOver2 == 0) ? 999.0 : std::fabs(std::log(tanThetaOver2));

  for (unsigned int i = 0; i < m_TRTTrksEtaBins.size(); ++i) {
    if (abs_eta < m_TRTTrksEtaBins[i]) {
      return i;
    }
  }
  return m_TRTTrksEtaBins.size() - 1;
}

double
InDet::InDetTrtTrackScoringTool::getMuDependentNtrtMinCut(unsigned int eta_bin) const
{

  double minTRT = m_TRTTrksMinTRTHitsThresholds[eta_bin];

  if (m_TRTTrksMinTRTHitsMuDependencies[eta_bin] > 0) {

    float avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();

    // The mu-dependent cuts have only been validted up to mu = 80.
    // Also there is some physical limit to nTRT, so at some point
    // there needs to be a ceiling for this threshold.
    // To be revisited when a higher mu value is reached.
    avg_mu = std::min(80.f,avg_mu);

    // minTRT = a + avg_mu * b
    minTRT += avg_mu * m_TRTTrksMinTRTHitsMuDependencies[eta_bin];
  }

  return minTRT;
}

