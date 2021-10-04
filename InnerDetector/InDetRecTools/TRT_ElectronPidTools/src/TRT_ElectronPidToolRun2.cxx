/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidToolRun2.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "TRT_ElectronPidTools/BaseTRTPIDCalculator.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidToolRun2.h"

// StoreGate, Athena, and Database stuff:
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "StoreGate/ReadCondHandle.h"

// Tracking:
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackInfo.h"

// Drift circles and TRT identifiers:
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetIdentifier/TRT_ID.h"

// ToT Tool Interface
#include "TRT_ElectronPidTools/ITRT_ToT_dEdx.h"

// For the track length in straw calculations
#include "TRT_ToT_dEdx.h"

// Particle masses

// Math functions:
#include <cmath>

//STL includes
#include <sstream>

//#define TRTDBG ATH_MSG_INFO("To line "<<__LINE__);
//#define TRTDBG 0;

//#include "TRT_ElectronPidToolRun2_HTcalculation.cxx"

// Helper method to store NN input variables into maps
template <typename T>
void storeNNVariable(std::map<std::string, T>& theMap, const std::string& name, const T& value) {
  auto it = theMap.find(name);
  if (it != theMap.end()) {
    it->second = value;
  }
}



/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  PID Tool Constructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::TRT_ElectronPidToolRun2(const std::string& t, const std::string& n, const IInterface* p )
  :
  AthAlgTool(t,n,p),
  m_trtId(nullptr),
  m_minTRThits(5),
  m_ptMinNN(2000.),
  m_calculateNN(true)
{
  declareInterface<ITRT_ElectronPidTool>(this);
  declareInterface<ITRT_ElectronToTTool>(this);
  declareProperty("MinimumTRThitsForIDpid", m_minTRThits);
  declareProperty("MinimumTrackPtForNNPid", m_ptMinNN);
  declareProperty("CalculateNNPid", m_calculateNN);
}


/*****************************************************************************\
|*%%%  PID Tool Destructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::~TRT_ElectronPidToolRun2()
= default;

/*****************************************************************************\
|*%%%  Initialisation  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

StatusCode InDet::TRT_ElectronPidToolRun2::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  // Get the TRT Identifier-helper:
  CHECK (detStore()->retrieve(m_trtId, "TRT_ID"));

  /* Get the TRT_ToT_dEdx tool */
  CHECK( m_TRTdEdxTool.retrieve() );

  CHECK( m_LocalOccTool.retrieve() );

  ATH_CHECK( m_HTReadKey.initialize() );

  ATH_CHECK( m_TRTPIDNNReadKey.initialize(m_calculateNN) );

  CHECK( m_TRTStrawSummaryTool.retrieve() );
  if ( !m_TRTStrawSummaryTool.empty()) ATH_MSG_INFO( "Retrieved tool " << m_TRTStrawSummaryTool);

  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}



/*****************************************************************************\
|*%%%  Finalisation  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

StatusCode InDet::TRT_ElectronPidToolRun2::finalize()
{
  return AthAlgTool::finalize();
}

// Kept for backward compatibility.
// See TRT_ElectronPidTools-01-00-28 for the full (commented) code.
std::vector<float> InDet::TRT_ElectronPidToolRun2::electronProbability_old(const Trk::Track& track)
{
  // Simply return values without calculation
  std::vector<float> PIDvalues = Trk::eProbabilityDefault;
  const Trk::TrackParameters* perigee = track.perigeeParameters();
  if (!perigee) { return PIDvalues; }
  return PIDvalues;
}


/*****************************************************************************\
|*%%%  electronProbability - The interface method during reconstruction  %%%%*|
\*****************************************************************************/

std::vector<float>
InDet::TRT_ElectronPidToolRun2::electronProbability(
  const EventContext& ctx,
  const Trk::Track& track) const
{

  // Get the probability calculator
  SG::ReadCondHandle<HTcalculator> readHandle{m_HTReadKey,ctx};
  const HTcalculator* HTcalc = (*readHandle);
  // make sure some calibration is available
  if(HTcalc==nullptr) {
    ATH_MSG_WARNING ("  No Pid calibration from the DB.");
  }

  // Get the PID NN
  const InDet::TRTPIDNN* PIDNN = nullptr;
  if (m_calculateNN) {
    SG::ReadCondHandle<InDet::TRTPIDNN> readHandlePIDNN{m_TRTPIDNNReadKey,ctx};
    PIDNN = (*readHandlePIDNN);
    // make sure some calibration is available
    if(PIDNN==nullptr) {
      ATH_MSG_WARNING ("  No PID NN available from the DB.");
    }
  }

  // Initialize the vector with default PID values
  std::vector<float> PIDvalues = Trk::eProbabilityDefault;

  // Check for perigee:
  const Trk::TrackParameters* perigee = track.perigeeParameters();
  if (!perigee) return PIDvalues;

  // Get parameters at perigee and check that they are reasonable:
  const AmgVector(Trk::TrackParameters::dim)& parameterVector = perigee->parameters();
  double qOverP = parameterVector[Trk::qOverP];
  double theta  = parameterVector[Trk::theta];
  double phi    = parameterVector[Trk::phi];

  // Check the parameters are reasonable:
  if (tan(theta/2.0) < 0.0001) {
    ATH_MSG_DEBUG("  Track has negative theta or is VERY close to beampipe! "
                  "(tan(theta/2) < 0.0001). Returning default Pid values.");
    return PIDvalues;
  }

  if (qOverP == 0.0) {
    ATH_MSG_DEBUG ("  Track momentum infinite! (i.e. q/p = 0). Returning default Pid values.");
    return PIDvalues;
  }

  double pTrk = fabs(1.0 / qOverP);
  double pT   = pTrk * sin(theta);
  double eta  = -log(tan(theta/2.0));

  // Check the tool to get the local occupancy (i.e. for the track in question):
  PIDvalues[Trk::TRTTrackOccupancy] = m_LocalOccTool->LocalOccupancy(ctx,track);

  if (PIDvalues[Trk::TRTTrackOccupancy] > 1.0  || PIDvalues[Trk::TRTTrackOccupancy]  < 0.0) {
    ATH_MSG_WARNING("  Occupancy was outside allowed range! Returning default Pid values. Occupancy = "
                    << PIDvalues[Trk::TRTTrackOccupancy] );
    return PIDvalues;
  }

  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("check  Got track:   pT: " << pT << "   eta: " << eta << "   phi: " << phi);
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");

  // For calculation of HT probability:
  double pHTel_prod = 1.0;
  double pHTpi_prod = 1.0;

  // ------------------------------------------------------------------------------------
  // Loop over TRT hits on track, and calculate HT and R-ToT probability:
  // ------------------------------------------------------------------------------------

  std::vector<double> hit_HTMB;
  std::vector<double> hit_gasType;
  std::vector<double> hit_tot;
  std::vector<double> hit_L;
  std::vector<double> hit_rTrkWire;
  std::vector<double> hit_HitZ;
  std::vector<double> hit_HitR;
  std::vector<double> hit_isPrec;

  unsigned int nTRThits     = 0;
  unsigned int nTRThitsHTMB = 0;
  unsigned int nXehits      = 0;
  unsigned int nArhits      = 0;
  unsigned int nPrecHits    = 0;


  // Check for track states:
  const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = track.trackStateOnSurfaces();
  if (not recoTrackStates) {
    ATH_MSG_DEBUG("track.trackStateOnSurfaces() was zero");
    //m_timingProfile->chronoStop("Tool::electronProb");
    return PIDvalues;
  }

  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();

  // Loop over track states on surfaces (i.e. generalized hits):
  for ( ; tsosIter != tsosIterEnd; ++tsosIter) {

    const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
    if (!measurement) continue;

    // Get drift circle (ensures that hit is from TRT):
    // use the type methods to avoid dynamic_cast in a loop
    const InDet::TRT_DriftCircleOnTrack* driftcircle = nullptr;
    if (measurement->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
      const Trk::RIO_OnTrack* tmpRio =
        static_cast<const Trk::RIO_OnTrack*>(measurement);
      if (tmpRio->rioType(Trk::RIO_OnTrackType::TRT_DriftCircle)) {
        driftcircle = static_cast<const InDet::TRT_DriftCircleOnTrack*>(tmpRio);
      }
    }

    if (!driftcircle) continue;

    // From now (May 2015) onwards, we ONLY USE MIDDLE HT BIT:
    bool isHTMB  = (driftcircle->prepRawData()->getWord() & 0x00020000) > 0;

    nTRThits++;
    if (isHTMB) nTRThitsHTMB++;
    hit_HTMB.push_back(static_cast<double>(isHTMB));


    // ------------------------------------------------------------------------------------
    // Get the necessary input for the probability calculations:
    // ------------------------------------------------------------------------------------
    Identifier DCid = driftcircle->identify();

    // Part of TRT hit belongs to (TrtPart = 0: Barrel, 1: EndcapA, 2: EndcapB).
    int TrtPart = 0;                      // 0: Barrel, 1: EndcapA, 2: EndcapB
    if (abs(m_trtId->barrel_ec(DCid)) == 2)
      TrtPart = (m_trtId->layer_or_wheel(DCid) < 6) ? 1 : 2;

    // Get Straw Layer (Barrel: 0-72, EndcapA: 0-95 (16 layers in 6 modules), EndcapB: 0-63 (8 layers in 8 modules)):
    int StrawLayer = 0;
    if (TrtPart == 0) {
      // Barrel:
      if (m_trtId->layer_or_wheel(DCid) == 0) {
        StrawLayer = m_trtId->straw_layer(DCid);
      } else if (m_trtId->layer_or_wheel(DCid) == 1) {
        StrawLayer = 19 + m_trtId->straw_layer(DCid);
      } else {
        StrawLayer = 19 + 24 + m_trtId->straw_layer(DCid);
      }
    } else {
      // Endcap:
      if (m_trtId->layer_or_wheel(DCid) < 6) {
        StrawLayer =
          16 * m_trtId->layer_or_wheel(DCid) + m_trtId->straw_layer(DCid);
      } else {
        StrawLayer =
          8 * (m_trtId->layer_or_wheel(DCid) - 6) + m_trtId->straw_layer(DCid);
      }
    }

    // Get Z (Barrel) or R (Endcap) location of the hit, and distance from track to wire (i.e. anode) in straw:
    double HitZ = 0.;
    double HitR = 0.;
    double rTrkWire = 0.;
    bool hasTrackParameters = true; // Keep track of this for HT prob calculation
    if ((*tsosIter)->trackParameters()) {
      // If we have precise information (from hit), get that:
      const Amg::Vector3D& gp = driftcircle->globalPosition();
      HitR = gp.perp();
      HitZ = gp.z();
      rTrkWire = fabs((*tsosIter)->trackParameters()->parameters()[Trk::driftRadius]);
      if (rTrkWire > 2.2) rTrkWire = 2.175;   // cut off track-to-wire distance for outliers
    } else {
      // Otherwise just use the straw coordinates:
      hasTrackParameters = false; // Jared - pass this to HT calculation
      HitZ = driftcircle->associatedSurface().center().z();
      HitR = driftcircle->associatedSurface().center().perp();
      rTrkWire = 0;
    }

    // fill vectors for NN PID
    if (m_calculateNN and pT >= m_ptMinNN) {
      hit_HitZ.push_back(HitZ);
      hit_HitR.push_back(HitR);
      hit_rTrkWire.push_back(rTrkWire);
      hit_L.push_back(TRT_ToT_dEdx::calculateTrackLengthInStraw((*tsosIter), m_trtId));
      hit_tot.push_back(driftcircle->timeOverThreshold());
    }

    // ------------------------------------------------------------------------------------
    // Collection and checks of input variables for HT probability calculation:
    // ------------------------------------------------------------------------------------

    int SL_max[3] = {73, 96, 64};
    if (StrawLayer > SL_max[TrtPart]  ||  StrawLayer < 0) {
      ATH_MSG_WARNING("  StrawLayer was outside allowed range!  TrtPart = " << TrtPart << "  SL = " << StrawLayer);
      continue;
    }

    double ZRpos[3] = {fabs(HitZ), HitR, HitR};
    double ZRpos_min[3] = {  0.0,  630.0,  630.0};
    double ZRpos_max[3] = {720.0, 1030.0, 1030.0};
    if (ZRpos[TrtPart] > ZRpos_max[TrtPart]) {
      ATH_MSG_WARNING("  ZRpos was above allowed range - adjusted!  TrtPart = " << TrtPart << "  ZRpos = " << ZRpos[TrtPart]);
      ZRpos[TrtPart] = ZRpos_max[TrtPart] - 0.001;
    }
    if (ZRpos[TrtPart] < ZRpos_min[TrtPart]) {
      ATH_MSG_WARNING("  ZRpos was below allowed range - adjusted!  TrtPart = " << TrtPart << "  ZRpos = " << ZRpos[TrtPart]);
      ZRpos[TrtPart] = ZRpos_min[TrtPart] + 0.001;
    }

    // ------------------------------------------------------------------------------------
    // Calculate the HT probability:
    // ------------------------------------------------------------------------------------

    // getStatusHT returns enum {Undefined, Dead, Good, Xenon, Argon, Krypton, EmulatedArgon, EmulatedKrypton}.
    // Our representation of 'GasType' is 0:Xenon, 1:Argon, 2:Krypton
    int GasType=0; // Xenon is default
    if (!m_TRTStrawSummaryTool.empty()) {
      int stat = m_TRTStrawSummaryTool->getStatusHT(DCid,ctx);
      if       ( stat==2 || stat==3 ) { GasType = 0; } // Xe
      else if  ( stat==1 || stat==4 ) { GasType = 1; } // Ar
      else if  ( stat==5 )            { GasType = 1; } // Kr -- ESTIMATED AS AR UNTIL PID IS TUNED TO HANDLE KR
      else if  ( stat==6 )            { GasType = 1; } // Emulated Ar
      else if  ( stat==7 )            { GasType = 1;
      } // Emulated Kr -- ESTIMATED AS AR UNTIL PID IS TUNED TO HANDLE KR
      else {
        ATH_MSG_FATAL(
          "getStatusHT = "
          << stat
          << ", must be 'Good(2)||Xenon(3)' or 'Dead(1)||Argon(4)' or "
             "'Krypton(5)' or 'EmulatedArgon(6)' or 'EmulatedKr(7)'!");
        throw std::exception();
      }
    }

    ATH_MSG_DEBUG("check Hit: "
                  << nTRThits << "  TrtPart: " << TrtPart
                  << "  GasType: " << GasType << "  SL: " << StrawLayer
                  << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire
                  << "  Occ_Local: " << PIDvalues[Trk::TRTTrackOccupancy]  << "  HTMB: " << isHTMB);

    if (m_calculateNN and pT >= m_ptMinNN) {
      // RNN gas type observables
      hit_gasType.push_back(static_cast<double>(GasType));
      if (GasType == 0) {
        nXehits++;
      } else if (GasType == 1) {
        nArhits++;
      }

      // RNN hit preciion observables
      float errDc = sqrt(driftcircle->localCovariance()(Trk::driftRadius, Trk::driftRadius));
      bool isPrec = false;
      if (errDc < 1.0) {
        isPrec = true;
        nPrecHits++;
      }
      hit_isPrec.push_back(static_cast<double>(isPrec));
    }

    // Then call pHT functions with these values:
    // ------------------------------------------

    double pHTel = HTcalc->getProbHT(pTrk,
                                     Trk::electron,
                                     TrtPart,
                                     GasType,
                                     StrawLayer,
                                     ZRpos[TrtPart],
                                     rTrkWire,
                                     PIDvalues[Trk::TRTTrackOccupancy] ,
                                     hasTrackParameters);
    double pHTpi = HTcalc->getProbHT(pTrk,
                                     Trk::pion,
                                     TrtPart,
                                     GasType,
                                     StrawLayer,
                                     ZRpos[TrtPart],
                                     rTrkWire,
                                     PIDvalues[Trk::TRTTrackOccupancy] ,
                                     hasTrackParameters);

    if (pHTel > 0.999 || pHTpi > 0.999 || pHTel < 0.001 || pHTpi < 0.001) {
      ATH_MSG_DEBUG("  pHT outside allowed range!  pHTel = "
                    << pHTel << "  pHTpi = " << pHTpi
                    << "     TrtPart: " << TrtPart << "  SL: " << StrawLayer
                    << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire
                    << "  Occ_Local: " << PIDvalues[Trk::TRTTrackOccupancy] );
      continue;
    }

    if (pHTel > 0.80 || pHTpi > 0.50 || pHTel < 0.025 || pHTpi < 0.010) {
      ATH_MSG_DEBUG("  pHT has abnormal value!  pHTel = "
                    << pHTel << "  pHTpi = " << pHTpi
                    << "     TrtPart: " << TrtPart << "  SL: " << StrawLayer
                    << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire
                    << "  Occ_Local: " << PIDvalues[Trk::TRTTrackOccupancy] );
      continue;
    }

    // From now (May 2015) onwards, we ONLY USE MIDDLE HT BIT:
    if (isHTMB) {pHTel_prod *=     pHTel;  pHTpi_prod *=     pHTpi;}
    else        {pHTel_prod *= 1.0-pHTel;  pHTpi_prod *= 1.0-pHTpi;}
    ATH_MSG_DEBUG ("check         pHT(el): " << pHTel << "  pHT(pi): " << pHTpi );

  } // end of loop over hits


  // If number of hits is adequate (default is 5 hits), calculate HT and ToT probability.
  if (not (nTRThits >= m_minTRThits)) return PIDvalues;

  // Calculate electron probability (HT)
  PIDvalues[Trk::eProbabilityHT] = pHTel_prod / (pHTel_prod + pHTpi_prod);

  ATH_MSG_DEBUG ("check  nTRThits: " << nTRThits << "  : " << nTRThitsHTMB
                                     << "  pHTel_prod: " << pHTel_prod
                                     << "  pHTpi_prod: " << pHTpi_prod
                                     << "  probEl: " << PIDvalues[Trk::eProbabilityHT]);

  PIDvalues[Trk::TRTdEdx] = m_TRTdEdxTool->dEdx(
    ctx,
    &track,
    true, //be expicit as optional below can be converted to bool
    PIDvalues[Trk::TRTTrackOccupancy]); // default dEdx using all hits

  PIDvalues[Trk::eProbabilityNumberOfTRTHitsUsedFordEdx] =
    m_TRTdEdxTool->usedHits(ctx, &track);
  double dEdx_noHTHits = m_TRTdEdxTool->dEdx(
    ctx,
    &track,
    false,//be expicit as optional below can be converted to bool 
    PIDvalues[Trk::TRTTrackOccupancy]); // Divide by L, exclude HT hits

  double dEdx_usedHits_noHTHits = m_TRTdEdxTool->usedHits(ctx, &track, false);
  PIDvalues[Trk::eProbabilityToT] = m_TRTdEdxTool->getTest(
    ctx, dEdx_noHTHits, pTrk, Trk::electron, Trk::pion, dEdx_usedHits_noHTHits);

  // Limit the probability values the upper and lower limits that are given/trusted for each part:
  double limProbHT = HTcalc->Limit(PIDvalues[Trk::eProbabilityHT]);
  double limProbToT = HTcalc->Limit(PIDvalues[Trk::eProbabilityToT]);

  // Calculate the combined probability, assuming no correlations (none are expected).
  PIDvalues[Trk::eProbabilityComb] =
    (limProbHT * limProbToT) /
    ((limProbHT * limProbToT) + ((1.0 - limProbHT) * (1.0 - limProbToT)));

  // Troels: VERY NASTY NAMING, BUT AGREED UPON FOR NOW (for debugging, 27. NOV. 2014):
  PIDvalues[Trk::eProbabilityBrem] = pHTel_prod; // decorates electron LH to el brem for now... (still used?)

  if (!m_calculateNN or pT < m_ptMinNN) {
    return PIDvalues;
  }

  // Calculate RNN PID score
  std::map<std::string, std::map<std::string, double>> scalarInputs_NN = PIDNN->getScalarInputs();
  std::map<std::string, std::map<std::string, std::vector<double>>> vectorInputs_NN = PIDNN->getVectorInputs();

  // Calculate the hit fraction
  double fAr = static_cast<double>(nArhits) / nTRThits;
  double fHTMB = static_cast<double>(nTRThitsHTMB) / nTRThits;
  double PHF = static_cast<double>(nPrecHits) / nTRThits;

  if (!scalarInputs_NN.empty()) {
    std::map<std::string, double>& trackVarMap = scalarInputs_NN.begin()->second;
    storeNNVariable(trackVarMap, "trkOcc", static_cast<double>(PIDvalues[Trk::TRTTrackOccupancy]));
    storeNNVariable(trackVarMap, "p", pTrk);
    storeNNVariable(trackVarMap, "pT", pT);
    storeNNVariable(trackVarMap, "nXehits", static_cast<double>(nXehits));
    storeNNVariable(trackVarMap, "fAr", fAr);
    storeNNVariable(trackVarMap, "fHTMB", fHTMB);
    storeNNVariable(trackVarMap, "PHF", PHF);
    storeNNVariable(trackVarMap, "dEdx", static_cast<double>(dEdx_noHTHits));
  }

  if (!vectorInputs_NN.empty()) {
    std::map<std::string, std::vector<double>>& hitVarMap = vectorInputs_NN.begin()->second;
    storeNNVariable(hitVarMap, "hit_HTMB", hit_HTMB);
    storeNNVariable(hitVarMap, "hit_gasType", hit_gasType);
    storeNNVariable(hitVarMap, "hit_tot", hit_tot);
    storeNNVariable(hitVarMap, "hit_L", hit_L);
    storeNNVariable(hitVarMap, "hit_rTrkWire", hit_rTrkWire);
    storeNNVariable(hitVarMap, "hit_HitZ", hit_HitZ);
    storeNNVariable(hitVarMap, "hit_HitR", hit_HitR);
    storeNNVariable(hitVarMap, "hit_isPrec", hit_isPrec);
  }
  PIDvalues[Trk::eProbabilityNN] = PIDNN->evaluate(scalarInputs_NN, vectorInputs_NN);

  ATH_MSG_DEBUG ("check NN PID calculation: ");
  for (const auto& scalarInputs : scalarInputs_NN) {
    ATH_MSG_DEBUG ("  scalar inputs: " << scalarInputs.first);
    for (const auto& variable : scalarInputs.second) {
      ATH_MSG_DEBUG ("    " << variable.first << " = " << variable.second);
    }
  }
  for (const auto& vectorInputs : vectorInputs_NN) {
    ATH_MSG_DEBUG ("  vector inputs: " << vectorInputs.first);
    for (const auto& variable : vectorInputs.second) {
      ATH_MSG_DEBUG ("    " << variable.first << " = " << variable.second);
    }
  }
  ATH_MSG_DEBUG ("  eProbilityNN: " << PIDvalues[Trk::eProbabilityNN]);

  return PIDvalues;
}

/*****************************************************************************\
|*%%%  TRT straw address check, done once per hit.  %%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Nowhere else are these numbers checked. If this is deemed  %%%%%%%%%%%*|
|*%%%  unnecessary it can be taken out by simply letting the function %%%%%%%*|
|*%%%  return true every time  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

bool InDet::TRT_ElectronPidToolRun2::CheckGeometry(int BEC, int Layer, int StrawLayer) const {

  //first check that the BEC is valid:
  if(not ( BEC==-2 || BEC ==-1 || BEC==1 || BEC==2)){
    ATH_MSG_ERROR("Found a wrong TRT part: "<<BEC<<" expected one of (-2,-1,1,2)");
    return false;
  }
  const int part = abs(BEC)-1;

  //next check that the layer is valid
  if( Layer < 0){
    ATH_MSG_ERROR("Found a negative TRT Layer");
    return false; //must be positive
  }

  static const int nlayers[2]={3,14};

  if( not ( Layer < nlayers[part] ) ){
    ATH_MSG_ERROR("Found TRT Layer index "<<Layer<<" in part "<<BEC<<" but part only has "<<nlayers[part]<<" layers.");
    return false;
  }

  //and finally check that the StrawLayer is valid:
  if( StrawLayer < 0){
    ATH_MSG_ERROR("Found a negative TRT StrawLayer");
    return false; //must be positive
  }

  static const int strawsPerBEC[2][14]={{19,24,30, 0, 0, 0,0,0,0,0,0,0,0,0},
                                        {16,16,16,16,16,16,8,8,8,8,8,8,8,8}};

  if(not(StrawLayer < strawsPerBEC[part][Layer])){
    ATH_MSG_ERROR("TRT part " << BEC << " Layer " << Layer << " only has "
                              << strawsPerBEC[part][Layer]
                              << " straws. Found index " << StrawLayer);
    return false;
  }

  return true;
}

/*****************************************************************************\
|*%%%  Auxiliary function to return the HT probability to Atlfast  %%%%%%%%%%*|
|*%%%  a geometry check is performed every time here  %%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

double
InDet::TRT_ElectronPidToolRun2::probHT(
  const double /*pTrk*/,
  const Trk::ParticleHypothesis /*hypothesis*/,
  const int HitPart,
  const int Layer,
  const int StrawLayer) const
{
  if (not CheckGeometry(HitPart,Layer,StrawLayer) ){
    ATH_MSG_ERROR("TRT geometry fail. Returning default value.");
    return 0.5;
  }

  return 1.0;
}

double
InDet::TRT_ElectronPidToolRun2::probHTRun2(const EventContext& ctx,
                                           float pTrk,
                                           Trk::ParticleHypothesis hypothesis,
                                           int TrtPart,
                                           int GasType,
                                           int StrawLayer,
                                           float ZR,
                                           float rTrkWire,
                                           float Occupancy) const
{
  SG::ReadCondHandle<HTcalculator> readHandle{ m_HTReadKey, ctx };
  bool hasTrackPar = true;
  return (*readHandle)
    ->getProbHT(pTrk,
                hypothesis,
                TrtPart,
                GasType,
                StrawLayer,
                ZR,
                rTrkWire,
                Occupancy,
                hasTrackPar);
}
