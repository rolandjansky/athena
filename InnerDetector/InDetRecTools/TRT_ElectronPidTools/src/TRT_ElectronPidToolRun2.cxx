/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidToolRun2.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "TRT_ElectronPidTools/BaseTRTPIDCalculator.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidToolRun2.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidToolRun2_ToTcalculation.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidToolRun2_HTcalculation.h"

// StoreGate, Athena, and Database stuff:
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
// Tracking:
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackInfo.h"

// Drift circles and TRT identifiers:
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TRT_ToT_Tools/ITRT_ToT_dEdx.h"

// Particle masses

// Math functions:
#include <cmath>

//STL includes
#include <sstream>

//#define TRTDBG ATH_MSG_INFO("To line "<<__LINE__);
//#define TRTDBG 0;

#include "TRT_ElectronPidToolRun2_ToTcalculation.cxx"
#include "TRT_ElectronPidToolRun2_HTcalculation.cxx"


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  PID Tool Constructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::TRT_ElectronPidToolRun2(const std::string& t, const std::string& n, const IInterface* p )
  :
  AthAlgTool(t,n,p),
  m_trtId(0),
  m_minTRThits(5),
  m_bremFitterEnabled(false),
  ToTcalc(*(new ToTcalculator(*this))),
  HTcalc(*(new HTcalculator(*this))),
  m_TRTdEdxTool("TRT_ToT_dEdx"),
  m_LocalOccTool(),
  m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",n)
{
  declareInterface<ITRT_ElectronPidTool>(this);
  declareInterface<ITRT_ElectronToTTool>(this);
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
  declareProperty("MinimumTRThitsForIDpid", m_minTRThits);
  declareProperty("BremfitterEnabled", m_bremFitterEnabled);
  declareProperty("TRT_ToT_dEdx_Tool", m_TRTdEdxTool);
  declareProperty("TRT_LocalOccupancyTool", m_LocalOccTool);
  declareProperty("isData", m_DATA = true);
  declareProperty("TRTStrawSummarySvc",    m_TRTStrawSummarySvc);
  declareProperty("OccupancyUsedInPID", m_OccupancyUsedInPID=true);
}


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  PID Tool Destructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::~TRT_ElectronPidToolRun2()
{
  delete &ToTcalc;
  delete &HTcalc;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Initialisation  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

StatusCode InDet::TRT_ElectronPidToolRun2::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  // Get the TRT Identifier-helper:
  if (detStore()->retrieve(m_trtId, "TRT_ID").isFailure()) {
    ATH_MSG_FATAL ("Could not get TRT ID helper");
    return StatusCode::FAILURE;
  }

  // Register callback function for cache updates - HT:
  //const DataHandle<CondAttrListCollection> aptr;
  //const DataHandle<AthenaAttributeList> aptr
  const DataHandle<CondAttrListVec> aptr;;
  //if (StatusCode::SUCCESS == detStore()->regFcn(&InDet::TRT_ElectronPidToolRun2::update,this, aptr, "/TRT/Calib/PIDver_New" )) {
  if (StatusCode::SUCCESS == detStore()->regFcn(&InDet::TRT_ElectronPidToolRun2::update,this, aptr, "/TRT/Calib/PID_vector" )) {
    ATH_MSG_DEBUG ("Registered callback for TRT_ElectronPidToolRun2 - HT.");
  } else {
    ATH_MSG_ERROR ("Callback registration failed for TRT_ElectronPidToolRun2 - HT! ");
  }

  // Register callback function for cache updates - RToT:
  const DataHandle<AthenaAttributeList> aptr_rtot;
  if (StatusCode::SUCCESS == detStore()->regFcn(&InDet::TRT_ElectronPidToolRun2::update,this, aptr_rtot, "/TRT/Calib/PID_RToTver_New" )) {
    ATH_MSG_DEBUG ("Registered callback for TRT_ElectronPidToolRun2 - RToT.");
  } else {
    ATH_MSG_ERROR ("Callback registration failed for TRT_ElectronPidToolRun2 - RToT! ");
  }
  /* Get the TRT_ToT_dEdx tool */
  if ( m_TRTdEdxTool.retrieve().isFailure() )
    ATH_MSG_DEBUG("Failed to retrieve ToT dE/dx tool " << m_TRTdEdxTool);
  else
    ATH_MSG_DEBUG("Retrieved tool " << m_TRTdEdxTool);

  if ( m_LocalOccTool.retrieve().isFailure() ){
    ATH_MSG_WARNING("Failed retrieve Local Occ tool " << m_LocalOccTool << " the tool will not be called!!!" );
  }
  else ATH_MSG_INFO("Retrieved tool " << m_LocalOccTool);

  sc = m_TRTStrawSummarySvc.retrieve();
  if (StatusCode::SUCCESS!= sc ){
    ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
    ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
    return sc;
  } else {
    if ( !m_TRTStrawSummarySvc.empty()) msg(MSG::INFO) << "Retrieved tool " << m_TRTStrawSummarySvc << endreq;
  }

//   m_timingProfile=0;
//   sc = service("ChronoStatSvc", m_timingProfile);
//   if ( sc.isFailure() || 0 == m_timingProfile) {
//     ATH_MSG_ERROR("Could not get ChronoStat!");
//     return StatusCode::FAILURE;
//   }


  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}



/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Finalisation  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

StatusCode InDet::TRT_ElectronPidToolRun2::finalize()
{
  //StatusCode sc = AthAlgTool::finalize();
  //return sc;
  return AthAlgTool::finalize();
}

double InDet::TRT_ElectronPidToolRun2::GetD(double R_Track)const {
  R_Track=fabs(R_Track);
  if(R_Track>2.) return 0;
  return 2. * sqrt( 4. - R_Track * R_Track );
}

double InDet::TRT_ElectronPidToolRun2::GetToT(unsigned int bitpattern, double HitZ, double HitR, int BEC, int Layer, int Strawlayer)const {
  return  ToTcalc.GetToT( bitpattern, HitZ, HitR, BEC, Layer, Strawlayer);
}



// Kept for backward compatibility.
// See TRT_ElectronPidTools-01-00-28 for the full (commented) code.
std::vector<float> InDet::TRT_ElectronPidToolRun2::electronProbability_old(const Trk::Track& track)
{
  // Simply return values without calculation
  std::vector<float> PIDvalues(4);
  PIDvalues[0] = 0.5;
  PIDvalues[1] = 0.5;
  PIDvalues[2] = 0.0;
  PIDvalues[3] = 0.5;
  const Trk::TrackParameters* perigee = track.perigeeParameters();
  if (!perigee) { return PIDvalues; }
  return PIDvalues;
}


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  electronProbability - The interface method during reconstruction  %%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

std::vector<float> InDet::TRT_ElectronPidToolRun2::electronProbability(const Trk::Track& track) {

  //ATH_MSG_INFO("started electronProbabaility");
  //Intialize the return vector
  //m_timingProfile->chronoStart("Tool::electronProb");
  std::vector<float> PIDvalues(5);
  float & prob_El_Comb      = PIDvalues[0] = 0.5;
  float & prob_El_HT        = PIDvalues[1] = 0.5;
  float & sum_ToT_by_sum_L  = PIDvalues[2] = 0.0;
  float & prob_El_Brem      = PIDvalues[3] = 0.5;
  float & occ_local         = PIDvalues[4] = 0.0;
  //float & prob_El_ToT       = PIDvalues[4] = 0.5;
  float prob_El_ToT   = 0.5;

  // Check for perigee:
  const Trk::TrackParameters* perigee = track.perigeeParameters();
  if (!perigee) {
    //ATH_MSG_INFO("no perigee parmeters retunred");
    // m_timingProfile->chronoStop("Tool::electronProb");
    return PIDvalues;
  }

  // Get parameters at perigee and check that they are reasonable:
  const Amg::VectorX& parameterVector = perigee->parameters();
  double qOverP = parameterVector[Trk::qOverP];
  double theta  = parameterVector[Trk::theta];
  double phi    = parameterVector[Trk::phi];

  // Check the parameters are reasonable:
  if (tan(theta/2.0) < 0.0001) {
    ATH_MSG_DEBUG ("  Track has negative theta or is VERY close to beampipe! (tan(theta/2) < 0.0001). Returning default Pid values.");
    //m_timingProfile->chronoStop("Tool::electronProb");
    return PIDvalues;
  }

  if (qOverP == 0.0) {
    ATH_MSG_DEBUG ("  Track momentum infinite! (i.e. q/p = 0). Returning default Pid values.");
    //m_timingProfile->chronoStop("Tool::electronProb");
    return PIDvalues;
  }

  double pTrk = fabs(1.0 / qOverP);
  double pT   = pTrk * sin(theta);
  double eta  = -log(tan(theta/2.0));

  // Check the tool to get the local occupancy (i.e. for the track in question):
  occ_local = m_LocalOccTool->LocalOccupancy(track);

  //  for (unsigned int i = 0; i < occ.size() ; i++){
  //    ATH_MSG_DEBUG("Local occ: " << i << "\t" << occ.at(i) );
  //  }

  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("check  Got track:   pT: " << pT << "   eta: " << eta << "   phi: " << phi);
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");

  // For calculation of HT probability:
  double pHTel_prod = 1.0;
  double pHTpi_prod = 1.0;

  // For calculation of ToT probability:
  double sumToT=0;
  double sumD=0;

  // ------------------------------------------------------------------------------------
  // Loop over TRT hits on track, and calculate HT and R-ToT probability:
  // ------------------------------------------------------------------------------------

  unsigned int nTRThits     = 0;
  unsigned int nTRThitsHTMB = 0;

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
    const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement);
    if (!driftcircle) continue;

    // From now (May 2015) onwards, we ONLY USE MIDDLE HT BIT:
    // bool isHTMB  = ((TRTDriftCircle->auxdata<unsigned int>("bitPattern")&int(pow(2,17))) > 0) ? true : false;
    bool isHTMB  = ((driftcircle->prepRawData()->getWord() & 0x00020000) > 0) ? true : false;       // 131072 = 2^17 in non-hex

    nTRThits++;
    if (isHTMB) nTRThitsHTMB++;


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
      if      (m_trtId->layer_or_wheel(DCid) == 0) StrawLayer = m_trtId->straw_layer(DCid);
      else if (m_trtId->layer_or_wheel(DCid) == 1) StrawLayer = 19 + m_trtId->straw_layer(DCid);
      else                                         StrawLayer = 19 + 24 + m_trtId->straw_layer(DCid);
    } else {
      // Endcap:
      if (m_trtId->layer_or_wheel(DCid) < 6) StrawLayer = 16*m_trtId->layer_or_wheel(DCid) + m_trtId->straw_layer(DCid);
      else                                   StrawLayer = 8*(m_trtId->layer_or_wheel(DCid)-6) + m_trtId->straw_layer(DCid);
    }

    // Get Z (Barrel) or R (Endcap) location of the hit, and distance from track to wire (i.e. anode) in straw:
    double HitZ, HitR, rTrkWire;
    if ((*tsosIter)->trackParameters()) {
      // If we have precise information (from hit), get that:
      const Amg::Vector3D& gp = driftcircle->globalPosition();
      HitR = gp.perp();
      HitZ = gp.z();
      rTrkWire = fabs((*tsosIter)->trackParameters()->parameters()[Trk::driftRadius]);
    } else {
      // Otherwise just use the straw coordinates:
      HitZ = driftcircle->associatedSurface().center().z();
      HitR = driftcircle->associatedSurface().center().perp();
      rTrkWire = 0;
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

    if (rTrkWire > 2.2) rTrkWire = 2.175;   // Happens once in a while - no need for warning!

    if (occ_local > 1.0  ||  occ_local < 0.0) {
      ATH_MSG_WARNING("  Occupancy was outside allowed range!  TrtPart = " << TrtPart << "  Occupancy = " << occ_local);
      continue;
    }

    // ------------------------------------------------------------------------------------
    // Calculate the HT probability:
    // ------------------------------------------------------------------------------------

    // getStatusHT returns enum {Undefined, Dead, Good, Xenon, Argon, Krypton, EmulatedArgon, EmulatedKrypton}.
    // Our representation of 'GasType' is 0:Xenon, 1:Argon, 2:Krypton
    int GasType=0; // Xenon is default
    if (!m_TRTStrawSummarySvc.empty()) {
      int stat = m_TRTStrawSummarySvc->getStatusHT(DCid);
      if       ( stat==2 || stat==3 ) { GasType = 0; } // Xe
      else if  ( stat==1 || stat==4 ) { GasType = 1; } // Ar
      else if  ( stat==5 )            { GasType = 1; } // Kr -- ESTIMATED AS AR UNTIL PID IS TUNED TO HANDLE KR
      else if  ( stat==6 )            { GasType = 1; } // Emulated Ar
      else if  ( stat==7 )            { GasType = 1; } // Emulated Kr -- ESTIMATED AS AR UNTIL PID IS TUNED TO HANDLE KR
      else { ATH_MSG_FATAL ("getStatusHT = " << stat << ", must be 'Good(2)||Xenon(3)' or 'Dead(1)||Argon(4)' or 'Krypton(5)' or 'EmulatedArgon(6)' or 'EmulatedKr(7)'!");
             throw std::exception();
           }
    }

    ATH_MSG_DEBUG ("check Hit: " << nTRThits << "  TrtPart: " << TrtPart << "  GasType: " << GasType << "  SL: " << StrawLayer
             << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire << "  Occ_Local: " << occ_local << "  HTMB: " << isHTMB );

    // For now use constant hardcoded values! Even in the long run, might stay with this, as Argon hits doesn't add much PID info!
    // double pHTel_Argon[3] = {0.089, 0.113, 0.113};
    // double pHTpi_Argon[3] = {0.039, 0.056, 0.056};

    // Then call pHT functions with these values:
    // ------------------------------------------
    double pHTel = HTcalc.getProbHT( pTrk, Trk::electron, TrtPart, GasType, StrawLayer, ZRpos[TrtPart], rTrkWire, occ_local);
    double pHTpi = HTcalc.getProbHT( pTrk, Trk::pion,     TrtPart, GasType, StrawLayer, ZRpos[TrtPart], rTrkWire, occ_local);

    if (pHTel > 0.999 || pHTpi > 0.999 || pHTel < 0.001 || pHTpi < 0.001) {
      ATH_MSG_DEBUG("  pHT outside allowed range!  pHTel = " << pHTel << "  pHTpi = " << pHTpi << "     TrtPart: " << TrtPart << "  SL: " << StrawLayer << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire << "  Occ_Local: " << occ_local);
      continue;
    }

    if (pHTel > 0.80 || pHTpi > 0.50 || pHTel < 0.025 || pHTpi < 0.010) {
      ATH_MSG_DEBUG("  pHT has abnormal value!  pHTel = " << pHTel << "  pHTpi = " << pHTpi << "     TrtPart: " << TrtPart << "  SL: " << StrawLayer << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire << "  Occ_Local: " << occ_local);
      continue;
    }

    // From now (May 2015) onwards, we ONLY USE MIDDLE HT BIT:
    if (isHTMB) {pHTel_prod *=     pHTel;  pHTpi_prod *=     pHTpi;}
    else        {pHTel_prod *= 1.0-pHTel;  pHTpi_prod *= 1.0-pHTpi;}
   ATH_MSG_DEBUG ("check         pHT(el): " << pHTel << "  pHT(pi): " << pHTpi );

    // ------------------------------------------------------------------------------------
    // Calculate the ToT probability:
    // ------------------------------------------------------------------------------------

    int HitPart = m_trtId->barrel_ec(DCid);
    int StrawLayer_ToT = m_trtId->straw_layer(driftcircle->identify());
    int Layer_ToT = m_trtId->layer_or_wheel(driftcircle->identify());
    unsigned int BitPattern = driftcircle->prepRawData()->getWord();
    if (not CheckGeometry(HitPart, Layer_ToT, StrawLayer_ToT)) {
      ATH_MSG_ERROR("TRT straw address has incorrect format, skipping this hit");
      continue;
    }


    double locR=1.999;
    if ((*tsosIter)->trackParameters()) {
      locR = (*tsosIter)->trackParameters()->parameters()[Trk::locR];
      //ATH_MSG_INFO("LocR is "<<locR);
    }
    if (!driftcircle->highLevel()){
      //if(fabs(locR)<1.9 && fabs(locR)>0.01&&  fabs(driftcircle->localParameters()[Trk::driftRadius])!=0){  //MJ tube hits and close to wire/straw
      //m_timingProfile->chronoStart("Tool::PID");
      sumToT += ToTcalc.GetToT( BitPattern, HitZ, HitR, HitPart, Layer_ToT, StrawLayer_ToT);
      sumD   += ToTcalc.GetD(locR);
      // m_timingProfile->chronoStop("Tool::PID");
      //}
    }



  }//of loop over hits


  // If number of hits is adequate (default is 5 hits), calculate HT and ToT probability.
  if (not (nTRThits >= m_minTRThits)) {
    //ATH_MSG_INFO("TRT minimum not met, "<< nTRThits<<" hits, need "<<m_minTRThits);
    //m_timingProfile->chronoStop("Tool::electronProb");
    return PIDvalues;
  }
  // this is what the ToT part is returning
  sum_ToT_by_sum_L = sumD > 0 ? (sumToT/sumD) : 0.;

  //  ATH_MSG_INFO("Sum ToT = "<<sumToT<<" Sum D = "<<sumD);
  //  ATH_MSG_INFO("sum_ToT_by_sum_L = "<<sum_ToT_by_sum_L);


  // prob_El_HT = likelProd_El_HT  / (likelProd_El_HT  + likelProd_Pi_HT);
  prob_El_HT = pHTel_prod / (pHTel_prod + pHTpi_prod);

  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("check  nTRThits: " << nTRThits << "  : " << nTRThitsHTMB << "  pHTel_prod: " << pHTel_prod << "  pHTpi_prod: " << pHTpi_prod << "  probEl: " << prob_El_HT);
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("");


  if(sumD>0)
    prob_El_ToT  = ToTcalc.GetElProb( sumToT / sumD );

  //Calculate the probability based on Brem
  if(m_bremFitterEnabled)
    prob_El_Brem = probBrem(track);

  // Limit the probability values the the upper and lower limits that are given/trusted for each part:
  double limProbHT = HTcalc.Limit(prob_El_HT);
  double limProbToT = ToTcalc.Limit(prob_El_ToT);
  
  // Calculate the combined probability, assuming no correlations (none are expected).
  prob_El_Comb = (limProbHT * limProbToT * prob_El_Brem) / 
    ( (limProbHT * limProbToT * prob_El_Brem)
      + ( (1.0-limProbHT) * (1.0-limProbToT) * (1.0-prob_El_Brem) ) );

  // Troels: VERY NASTY NAMING, BUT AGREED UPON FOR NOW (for debugging, 27. NOV. 2014):
  prob_El_Brem = pHTel_prod;
  
  //ATH_MSG_INFO("----------------------");
  //ATH_MSG_INFO("  Prob_Comb = " << prob_El_Comb);
  //ATH_MSG_INFO("  Prob_HT   = " << prob_El_HT);
  //ATH_MSG_INFO("  ToT_by_L  = " << sum_ToT_by_sum_L);
  //ATH_MSG_INFO("  Prob_ToT  = " << prob_El_ToT);
  //ATH_MSG_INFO("  Prob_Brem = " << prob_El_Brem);
  //m_timingProfile->chronoStop("Tool::electronProb");
  return PIDvalues;  
}




/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

  float InDet::TRT_ElectronPidToolRun2::probBrem(const Trk::Track& /*track*/){
/*
  //old code in place in place up to 00-00-22:
  if ( (track.info().trackFitter() == Trk::TrackInfo::KalmanFitter ||
        track.info().trackFitter() == Trk::TrackInfo::KalmanDNAFitter ||
        track.info().trackFitter() == Trk::TrackInfo::DeterministicAnnealingFilter ) &&
       track.info().trackProperties(Trk::TrackInfo::BremFit) ) {
    if (track.info().trackProperties(Trk::TrackInfo::BremFitSuccessful)) 
      prob_El_Brem = 0.30 / (0.30 + 0.03);
    else
      prob_El_Brem = 0.70 / (0.70 + 0.97);
  } else if (m_bremFitterEnabled) {
    if (track.info().trackFitter() == Trk::TrackInfo::GlobalChi2Fitter && !track.info().trackProperties(Trk::TrackInfo::BremFit))
      prob_El_Brem = 0.70 / (0.70 + 0.97);
    else if (track.info().trackFitter() == Trk::TrackInfo::GlobalChi2Fitter && track.info().trackProperties(Trk::TrackInfo::BremFit))
      prob_El_Brem = 0.30 / (0.30 + 0.03);
  }
*/
  return 0.5;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

/* ----------------------------------------------------------------------------------- */
// Callback function to update constants from database: 
/* ----------------------------------------------------------------------------------- */

StatusCode InDet::TRT_ElectronPidToolRun2::update( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {

  ATH_MSG_DEBUG ("Updating constants for the TRT_ElectronPidToolRun2! ");

  // Callback function to update HT onset parameter cache when condDB data changes:
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
    ATH_MSG_DEBUG("IOVCALLBACK for key " << *key << " number " << I);

  const char * calcName[2]  = {"HT",             "ToT"};
  BaseTRTPIDCalculator * calc[2]  = {&HTcalc,          &ToTcalc};
  const char * storeName[2] = {"/TRT/Calib/PID_vector", "/TRT/Calib/PID_RToTver_New"};
//  const char * storeName[2] = {"/TRT/Calib/PIDver_New", "/TRT/Calib/PID_RToTver_New"};
  const char * objName[2]   = {"TRT",            "TRT_RToT"};

  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_INFO("HT Calculator : Reading vector format");

		// NEW reading from DB
  ATH_MSG_INFO(calcName[0]<<" : Reading vector format");

  const DataHandle<CondAttrListVec> channel_values;
  if (StatusCode::SUCCESS == detStore()->retrieve(channel_values, "/TRT/Calib/PID_vector" )){
        sc = HTcalc.ReadVectorDB(        channel_values  );
  } else {
        ATH_MSG_ERROR ("Problem reading condDB object. -"<<calcName[0]);
  }


  ATH_MSG_INFO(calcName[1]<<" : Reading ToT Corrections ");
  const AthenaAttributeList* atrlist;
  if (StatusCode::SUCCESS == detStore()->retrieve(atrlist, storeName[1] ) && atrlist != 0) {
        const coral::Blob& blob = (*atrlist)[objName[1]].data<coral::Blob>();
        const unsigned char* BlobStart = static_cast<const unsigned char*> (blob.startingAddress());
        int currver = calc[1]->CurrentVersion;
        int blobsize= calc[1]->BLOB_SIZE;
        int version = static_cast<int>(*(BlobStart+calc[1]->_Version));
        int day     = static_cast<int>(*(BlobStart+calc[1]->_Day));
        int month   = static_cast<int>(*(BlobStart+calc[1]->_Month));
        int year    = static_cast<int>(*(BlobStart+calc[1]->_Year));
        if(version != currver){
          ATH_MSG_ERROR("Found wrong version of "<<calcName[1]<<" calibration constants in database. Found "<<version<<" expected "<<currver<<" details follow:");
          ATH_MSG_ERROR(calcName[1]<<" Database entry written on:"<<day<<"/"<<month<<"/"<<year<<" (DD/MM/YY)");
          ATH_MSG_ERROR("Size of Blob:       " << blob.size() << "  expected: " << blobsize );
          ATH_MSG_ERROR("Setting calibration constants to hard-coded default values.");
          calc[1]->checkInitialization();
        }else{
          ATH_MSG_INFO(calcName[1]<<" Database entry found");
          ATH_MSG_INFO(calcName[1]<<" Database entry written on:"<<day<<"/"<<month<<"/"<<year<<" (DD/MM/YY)" << " version used: " << version << " blob size expected: " << blobsize << " Blob size: " << blob.size());
          calc[1]->FillBlob(BlobStart);
        }
  } else {
        ATH_MSG_ERROR ("Problem reading condDB object. -"<<calcName[1]);
  }

   return sc;
/*
  for(int ic=0; ic<2; ic++){
    const AthenaAttributeList* atrlist;
    if (StatusCode::SUCCESS == detStore()->retrieve(atrlist, storeName[ic] ) && atrlist != 0) {
      const coral::Blob& blob = (*atrlist)[objName[ic]].data<coral::Blob>();
      const unsigned char* BlobStart = static_cast<const unsigned char*> (blob.startingAddress());

      int currver = calc[ic]->CurrentVersion;
      int blobsize= calc[ic]->BLOB_SIZE;
      int version = static_cast<int>(*(BlobStart+calc[ic]->_Version));
      int day     = static_cast<int>(*(BlobStart+calc[ic]->_Day));
      int month   = static_cast<int>(*(BlobStart+calc[ic]->_Month));
      int year    = static_cast<int>(*(BlobStart+calc[ic]->_Year));

      //UPDATE_MARKER
      //use the following line to force the use of the hard-coded calibration constants:
      // version=-1;
      if(version != currver){
        ATH_MSG_ERROR("Found wrong version of "<<calcName[ic]<<" calibration constants in database. Found "<<version<<" expected "<<currver<<" details follow:");
        ATH_MSG_ERROR(calcName[ic]<<" Database entry written on:"<<day<<"/"<<month<<"/"<<year<<" (DD/MM/YY)");
        ATH_MSG_ERROR("Size of Blob:       " << blob.size() << "  expected: " << blobsize );
        ATH_MSG_ERROR("Setting calibration constants to hard-coded default values.");
        calc[ic]->checkInitialization();
      }else{
        ATH_MSG_INFO(calcName[ic]<<" Database entry found");
        ATH_MSG_INFO(calcName[ic]<<" Database entry written on:"<<day<<"/"<<month<<"/"<<year<<" (DD/MM/YY)" << " version used: " << version << " blob size expected: " << blobsize << " Blob size: " << blob.size());

	// HACK: To fix Invalid read
	// see for example https://its.cern.ch/jira/browse/ATLASRECTS-1879 
        // calc[ic]->FillBlob(BlobStart);
        calc[1]->FillBlob(BlobStart);
	// ENDHACK
      }

    } else {
      // m_log << MSG::ERROR << "Problem reading condDB object." << endreq;
      ATH_MSG_ERROR ("Problem reading condDB object. -"<<calcName[ic]);
      //return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
*/

}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  TRT straw address check, done once per hit.  %%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Nowhere else are these numbers checked. If this is deemed  %%%%%%%%%%%*|
|*%%%  unnecessary it can be taken out by simply letting the function %%%%%%%*|
|*%%%  return true every time  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

bool InDet::TRT_ElectronPidToolRun2::CheckGeometry(int BEC, int Layer, int StrawLayer){

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
    ATH_MSG_ERROR("TRT part "<<BEC<<" Layer "<<Layer<<" only has "<<strawsPerBEC[part][Layer]<<" straws. Found index "<<StrawLayer);
    return false;
  }
  
  return true;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Auxiliary function to return the HT pobability to Atlfast  %%%%%%%%%%%*|
|*%%%  a geometry ckeck is perfored every time here  %%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

double InDet::TRT_ElectronPidToolRun2::probHT( const double /*pTrk*/, const Trk::ParticleHypothesis /*hypothesis*/, const int HitPart, const int Layer, const int StrawLayer){
  if (not CheckGeometry(HitPart,Layer,StrawLayer) ){
    ATH_MSG_ERROR("TRT geometry fail. Returning default value.");
    return 0.5;
  }
  //return HTcalc.getProbHT(pTrk, hypothesis, HitPart, Layer, StrawLayer);
  // FIXME
  return 1.0;//HTcalc.getProbHT(pTrk, hypothesis, HitPart, Layer, StrawLayer);
}


double InDet::TRT_ElectronPidToolRun2::probHTRun2( float pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int GasType, int StrawLayer, float ZR, float rTrkWire, float Occupancy ){
   return HTcalc.getProbHT( pTrk, hypothesis, TrtPart, GasType, StrawLayer, ZR, rTrkWire, Occupancy );
}



/**************************************************************************** \
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  The following functions can be very useful for developement  %%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/
/*
std::string InDet::TRT_ElectronPidToolRun2::PrintBitPattern(unsigned int bitpattern){
  //  Will print the bitpattern in the format
  //    Bits:000000000000000001111100 HT:000
  //  To find out how HT and LT bins are distributed please check TRT_DriftCircle.h
  
  std::stringstream hlout,llout;
  hlout<<" HT:";
  llout<<" Bits:";
  
  unsigned int mask =0x4000000;
  for(int i=0; i<27; i++){ //3 blocks of 8 plus 3 HT bits
    bool ThisBit = mask & bitpattern;
    mask>>=1;
    if(i%9==0)
      hlout<<int(ThisBit);
    else
      llout<<int(ThisBit);
  }
  return llout.str()+hlout.str();
}


int InDet::TRT_ElectronPidToolRun2::CountLTBitPattern(unsigned int bitpattern){
  //    Will count the total number of LT bits in bitpattern
  //    To find out how HT and LT bins are distributed please check TRT_DriftCircle.h
  //  
  int n=0;
  
  for(unsigned int mask=0x2000000; mask > 0; mask>>=1){
    if(mask & bitpattern) n++;
    //skip over the HT bits
    if(mask==0x20000 || mask==0x100) mask>>=1;
  }
  return n;
}


int InDet::TRT_ElectronPidToolRun2::CountHTBitPattern(unsigned int bitpattern){
//    Will count the total number of HT bits in bitpattern
//    To find out how HT and LT bins are distributed please check TRT_DriftCircle.h

  int n=0;

  for(unsigned int mask=0x4000000; mask > 0; mask>>=9)
    if(mask & bitpattern) n++;

  return n;
}
*/
