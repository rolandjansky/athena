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

#define PI 3.141592653589793238462643383


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  PID Tool Constructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::TRT_ElectronPidToolRun2(const std::string& t,
        const std::string& n,
        const IInterface*  p )
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
  const DataHandle<AthenaAttributeList> aptr;
  if (StatusCode::SUCCESS == detStore()->regFcn(&InDet::TRT_ElectronPidToolRun2::update,this, aptr, "/TRT/Calib/PIDver_New" )) {
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
  else ATH_MSG_DEBUG("Retrieved tool " << m_TRTdEdxTool);
 

  if ( m_LocalOccTool.retrieve().isFailure() )
       ATH_MSG_WARNING("Failed retrieve Local Occ tool " << m_LocalOccTool);
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




std::vector<float> InDet::TRT_ElectronPidToolRun2::electronProbability_old(const Trk::Track& track) {

  // Check the tool for Occupancy:
//  m_LocalOccTool->BeginEvent();
//  std::vector<float>  occ2 = m_LocalOccTool->GlobalOccupancy();
//  for (unsigned int i = 0; i < occ2.size() ; i++){
//  ATH_MSG_DEBUG("Local occ2: " << i << "\t" << occ2.at(i) );
//  }
  std::vector<float>  occ = m_LocalOccTool->LocalOccupancy(track);
  for (unsigned int i = 0; i < occ.size() ; i++){
  ATH_MSG_DEBUG("Local occ: " << i << "\t" << occ.at(i) );
  }

  // Intialize the return vector
  // m_timingProfile->chronoStart("Tool::electronProb_new");
  std::vector<float> PIDvalues(4);
  float & prob_El_Comb         = PIDvalues[0] = 0.5;
  float & prob_El_HT           = PIDvalues[1] = 0.5;
  float & sum_ToT_by_sum_L     = PIDvalues[2] = 0.0;
  float & prob_El_Brem         = PIDvalues[3] = 0.5;
  float prob_El_ToT   = 0.5;

  // Check for perigee:
  const Trk::TrackParameters* perigee = track.perigeeParameters();
  if (!perigee) {
    //ATH_MSG_INFO("no perigee parmeters retunred");
    //m_timingProfile->chronoStop("Tool::electronProb_new");
    return PIDvalues;
  }

  // Get parameters at perigee and check that they are reasonable:
  const Amg::VectorX& parameterVector = perigee->parameters();
  double qOverP = parameterVector[Trk::qOverP];
  double theta  = parameterVector[Trk::theta];
  // double phi    = parameterVector[Trk::phi];
  // Check the parameters are reasonable;
  if (tan(theta/2.0) < 0.0001) {
    ATH_MSG_DEBUG ("  Track has negative theta or is VERY close to beampipe! (tan(theta/2) < 0.0001). Returning default Pid values.");
    //m_timingProfile->chronoStop("Tool::electronProb_new");
    return PIDvalues;
  }
 
  if (qOverP == 0.0) {
    ATH_MSG_DEBUG ("  Track momentum infinite! (i.e. q/p = 0). Returning default Pid values.");
    //m_timingProfile->chronoStop("Tool::electronProb_new");
    return PIDvalues;
  }

  double pTrk = fabs(1.0 / qOverP);

  // ATH_MSG_INFO ("    pTrk_old: " << pTrk);



  // ------------------------------------------------------------------------------------
  // Loop over TRT hits on track, and calculate HT and R-ToT probability:
  // ------------------------------------------------------------------------------------
  //  m_timingProfile->chronoStart("Tool::Part2");
  unsigned int nTRThits = 0;

  // For calculation of HT probability:
  double likelProd_El_HT  = 1.0;
  double likelProd_Pi_HT  = 1.0;
  
  // For calculation of ToT probability:
  std::vector<double> vecToT;
  std::vector<double> vecToT_HT;
  int nHits =0;
  //int nHits_HT =0;
  double ToTsum = 0;
  //double ToTsum_HT = 0;
 

  // Check for track states:
  const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = track.trackStateOnSurfaces();
  if (not recoTrackStates) {
    ATH_MSG_DEBUG("track.trackStateOnSurfaces() was zero");
    //m_timingProfile->chronoStop("Tool::electronProb_new");
    return PIDvalues;
  }

  
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();

  // Loop over track states on surfaces (i.e. generalized hits):
  // -----------------------------------------------------------
  for ( ; tsosIter != tsosIterEnd; ++tsosIter) {
    const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
    if (!measurement) continue;
    
    const Trk::TrackParameters* trkP = (*tsosIter)->trackParameters();
    if (!trkP) continue;

    // Get drift circle (ensures that hit is from TRT):
    const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement);
    if (!driftcircle) continue;

    nTRThits++;
    
    // ------------------------------------------------------------------------------------
    // Get the necessary input for the probability calculations:
    // ------------------------------------------------------------------------------------
    Identifier DCid = driftcircle->identify();

    // Part of TRT hit belongs to (HitPart = +-1: Barrel, +-2: Wheels, TRTpart = 0: Barrel, 1: EndcapA, 2: EndcapB).
    int HitPart = m_trtId->barrel_ec(DCid);

    // The Straw location
    int Strawlayer=m_trtId->straw_layer(DCid);
    int Layer=m_trtId->layer_or_wheel(DCid);

    if( not CheckGeometry(HitPart,Layer,Strawlayer) ){
      ATH_MSG_ERROR("TRT straw address has incorrect format, skipping this hit");
      continue;
    } 

    // ------------------------------------------------------------------------------------
    // Calculate the HT probability:
    // ------------------------------------------------------------------------------------
    if (driftcircle->highLevel()) {
      // FIXME
      //likelProd_Pi_HT *= HTcalc.getProbHT(pTrk, Trk::pion,     HitPart, Layer, Strawlayer);
      //likelProd_El_HT *= HTcalc.getProbHT(pTrk, Trk::electron, HitPart, Layer, Strawlayer);
      likelProd_Pi_HT *= 1.0; //HTcalc.getProbHT(pTrk, Trk::pion,     HitPart, Layer, Strawlayer);
      likelProd_El_HT *= 1.0; //HTcalc.getProbHT(pTrk, Trk::electron, HitPart, Layer, Strawlayer);
    } else {
      likelProd_Pi_HT *= 1.0; //(1.0 - HTcalc.getProbHT(pTrk, Trk::pion,     HitPart, Layer, Strawlayer));
      likelProd_El_HT *= 1.0; //(1.0 - HTcalc.getProbHT(pTrk, Trk::electron, HitPart, Layer, Strawlayer));
    }

    // ------------------------------------------------------------------------------------
    // Calculate the ToT probability:
    // ------------------------------------------------------------------------------------
    
    double ToTovLCorr =  m_TRTdEdxTool->correctToT_corrRZL(trkP,driftcircle,HitPart, Layer, Strawlayer, m_DATA);
    if (ToTovLCorr>0) {  // fulfills hit criteria
      vecToT.push_back(ToTovLCorr);
      nHits++;
    }
  }
  
  // Reject highest hit for truncation
  if (nHits>1) {
    sort(vecToT.begin(), vecToT.end());
    for (unsigned int i = 0; i < vecToT.size()-1;i++){ 
      ToTsum+=vecToT.at(i);
    } 
    
    sum_ToT_by_sum_L     = ToTsum/(nHits-1);
  }
  
  // If number of hits is adequate (default is 5 hits), calculate HT and ToT probability.
  if (not (nTRThits >= m_minTRThits)) {
    //ATH_MSG_INFO("TRT minimum not met, "<< nTRThits<<" hits, need "<<m_minTRThits);
    //     m_timingProfile->chronoStop("Tool::electronProb_new");
    return PIDvalues;  
  }
  prob_El_HT   = likelProd_El_HT  / (likelProd_El_HT  + likelProd_Pi_HT);
  //ATH_MSG_INFO("now calculating prob_El_HT   = "<<likelProd_El_HT<<"  / ("<<likelProd_El_HT<<"  + "<<likelProd_Pi_HT<<") = "<<prob_El_HT);
  
  if (sum_ToT_by_sum_L>0 && nHits>0) {
    // correct for data/MC normalisation
    // unfortunately vertex container not yet available here so pileup correction not applied
    sum_ToT_by_sum_L*= m_TRTdEdxTool->correctNormalization(true,m_DATA);
    prob_El_ToT = m_TRTdEdxTool->getTest(sum_ToT_by_sum_L, pTrk,  Trk::electron,  Trk::pion, nHits);
  }
  
  // Calculate the probability based on Brem
  if (m_bremFitterEnabled)
    prob_El_Brem = probBrem(track);
  
  // Limit the likelyhood values the the upper and lower limits that are given for each part
  double limProbHT = HTcalc.Limit(prob_El_HT);
  double limProbToT = ToTcalc.Limit(prob_El_ToT);
  
  // Calculate the combined probability, assuming no correlations (none are expected).
  prob_El_Comb = (limProbHT * limProbToT * prob_El_Brem) / 
    ( (limProbHT * limProbToT * prob_El_Brem)  +  ( (1.0-limProbHT) * (1.0-limProbToT) * (1.0-prob_El_Brem) ) );

  //ATH_MSG_INFO("----------------------");
  //ATH_MSG_INFO("  Prob_Comb = " << prob_El_Comb);
  //ATH_MSG_INFO("  Prob_HT   = " << prob_El_HT);
  //ATH_MSG_INFO("  ToT_by_L  = " << sum_ToT_by_sum_L);
  //ATH_MSG_INFO("  Prob_ToT  = " << prob_El_ToT);
  //ATH_MSG_INFO("  Prob_Brem = " << prob_El_Brem);
  //ATH_MSG_INFO("  ToT_by_L HT = " << sum_ToT_by_sum_L_HT);
  //   m_timingProfile->chronoStop("Tool::electronProb_new");
  return PIDvalues;  
}



/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  electronProbability - The interface method during reconstruction  %%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

std::vector<float> InDet::TRT_ElectronPidToolRun2::electronProbability(const Trk::Track& track){


  // Check the tool for Occupancy:
//  m_LocalOccTool->BeginEvent();
//  std::vector<float>  occ2 = m_LocalOccTool->GlobalOccupancy();
//  for (unsigned int i = 0; i < occ2.size() ; i++){
//  ATH_MSG_DEBUG("Local occ2: " << i << "\t" << occ2.at(i) );
//  }
  std::vector<float>  occ = m_LocalOccTool->LocalOccupancy(track);
  for (unsigned int i = 0; i < occ.size() ; i++){
    ATH_MSG_DEBUG("Local occ: " << i << "\t" << occ.at(i) );
  }

  //ATH_MSG_INFO("started electronProbabaility");
  //Intialize the return vector
  //m_timingProfile->chronoStart("Tool::electronProb");
 std::vector<float> PIDvalues(4);
  float & prob_El_Comb      = PIDvalues[0] = 0.5;
  float & prob_El_HT        = PIDvalues[1] = 0.5;
  float & sum_ToT_by_sum_L  = PIDvalues[2] = 0.0;
  float & prob_El_Brem      = PIDvalues[3] = 0.5;
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

  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("  Got track:   pT: " << pT << "   eta: " << eta << "   phi: " << phi);
  ATH_MSG_DEBUG ("---------------------------------------------------------------------------------------");


  // For calculation of HT probability:
  double pHTel_prod = 1.0;
  double pHTpi_prod = 1.0;

  // For calculation of ToT probability:
  double sumToT=0;
  double sumD=0;


  // ------------------------------------------------------------------------------------
  // Loop over TRT hits on track, and calculate HT and R-ToT probability:
  // ------------------------------------------------------------------------------------

  unsigned int nTRThits   = 0;
  unsigned int nTRThitsHT = 0;

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

    nTRThits++;
    if (driftcircle->highLevel()) nTRThitsHT++;


    // ------------------------------------------------------------------------------------
    // Get the necessary input for the probability calculations:
    // ------------------------------------------------------------------------------------
    Identifier DCid = driftcircle->identify();

    // Part of TRT hit belongs to (TrtPart = 0: Barrel, 1: EndcapA, 2: EndcapB).
    // int HitPart = m_trtId->barrel_ec(DCid);
    int TrtPart = 0;    // 0: Barrel, 1: EndcapA, 2: EndcapB
    if (abs(m_trtId->barrel_ec(DCid)) == 2)
      TrtPart = (m_trtId->layer_or_wheel(DCid) < 6) ? 1 : 2;

    // Get Straw Layer (Barrel: 0-72, EndcapA: 0-95 mod 16 (16 layers in 6 modules), EndcapB: 0-63 mod 8 (8 layers in 8 modules)):
    // Barrel:
    int StrawLayer = 0;
    if      (m_trtId->layer_or_wheel(DCid) == 0) StrawLayer = m_trtId->straw_layer(DCid); 
    else if (m_trtId->layer_or_wheel(DCid) == 1) StrawLayer = 19 + m_trtId->straw_layer(DCid);
    else                                         StrawLayer = 19 + 24 + m_trtId->straw_layer(DCid);
    // Endcap:
    int slPeriod = m_trtId->straw_layer(DCid);


    // Get Z (Barrel) or R (Endcap) location of the hit, and distance from track to wire (i.e. anode) in straw:
    double HitZ, HitR, rTrkWire;
    if ((*tsosIter)->trackParameters()) {
      // If we have precise information, get that
      const Amg::Vector3D& gp = driftcircle->globalPosition();
      HitR = gp.perp();
      HitZ = gp.z();
      rTrkWire = fabs((*tsosIter)->trackParameters()->parameters()[Trk::driftRadius]);
    } else {
      // Otherwise just use the straw coordinates
      HitZ = driftcircle->associatedSurface().center().z();
      HitR = driftcircle->associatedSurface().center().perp();
      rTrkWire = 0;
    }


    // Occupancy:
    double occB = occ.at(1);
    double occE = occ.at(1);



    // ------------------------------------------------------------------------------------
    // Collection and checks of input variables for HT probability calculation:
    // ------------------------------------------------------------------------------------

    int SL[3] = {StrawLayer, slPeriod, slPeriod};
    int SL_max[3] = {73, 16, 8};
    if (SL[TrtPart] >= SL_max[TrtPart]  ||  SL[TrtPart] < 0) {
      ATH_MSG_WARNING("  StrawLayer or slPeriod was outside allowed range!  TrtPart = " << TrtPart << "  SL = " << SL[TrtPart]);
      continue;
    }
    
    double ZRpos[3] = {fabs(HitZ), HitR, HitR};
    double ZRpos_min[3] = {  0.0,  640.0,  640.0};
    double ZRpos_max[3] = {720.0, 1140.0, 1140.0};
    if (ZRpos[TrtPart] > ZRpos_max[TrtPart]) {
      ATH_MSG_WARNING("  ZRpos was above allowed range - adjusted!  TrtPart = " << TrtPart << "  ZRpos = " << ZRpos[TrtPart]);
      ZRpos[TrtPart] = ZRpos_max[TrtPart] - 0.001;
    }
    if (ZRpos[TrtPart] < ZRpos_min[TrtPart]) {
      ATH_MSG_WARNING("  ZRpos was below allowed range - adjusted!  TrtPart = " << TrtPart << "  ZRpos = " << ZRpos[TrtPart]);
      ZRpos[TrtPart] = ZRpos_min[TrtPart] + 0.001;
    }

    if (rTrkWire > 2.1) rTrkWire = 2.15;   // Happens once in a while - no need for warning!
    
    double Occ[3] = {occB, occE, occE};
    if (Occ[TrtPart] > 1.0  ||  Occ[TrtPart] < 0.0) {
      ATH_MSG_WARNING("  Occupancy was outside allowed range!  TrtPart = " << TrtPart << "  Occupancy = " << Occ[TrtPart]);
      continue;
    }


    // ------------------------------------------------------------------------------------
    // Calculate the HT probability:
    // ------------------------------------------------------------------------------------

    // Then call pHT functions with these values:
    // ------------------------------------------
    double pHTel = 1.0;
    double pHTpi = 1.0;

    int isArgonStraw = 0;
    if (!m_TRTStrawSummarySvc.empty()) {
      if (m_TRTStrawSummarySvc->getStatusHT(DCid) != TRTCond::StrawStatus::Good) {isArgonStraw = 1;}
    }

    // For now use constant hardcoded values! Even in the long run, might stay with this, as Argon hits doesn't add much PID info!
    double pHTel_Argon[3] = {0.089, 0.113, 0.113};
    double pHTpi_Argon[3] = {0.039, 0.056, 0.056};

    if (isArgonStraw) {
      pHTel = pHTel_Argon[TrtPart];
      pHTpi = pHTpi_Argon[TrtPart];
    } else {
      pHTel = HTcalc.getProbHT( pTrk, Trk::electron, TrtPart, SL[TrtPart], ZRpos[TrtPart], rTrkWire, Occ[TrtPart]);
      pHTpi = HTcalc.getProbHT( pTrk, Trk::pion,     TrtPart, SL[TrtPart], ZRpos[TrtPart], rTrkWire, Occ[TrtPart]);
    }

    if (pHTel > 0.999 || pHTpi > 0.999 || pHTel < 0.001 || pHTpi < 0.001) {
      ATH_MSG_WARNING("  pHT outside allowed range!  pHTel = " << pHTel << "  pHTpi = " << pHTpi << "     TrtPart: " << TrtPart << "  SL: " << SL[TrtPart] << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire << "  Occ: " << Occ[TrtPart]);
      continue;
    }
      
    if (pHTel > 0.80 || pHTpi > 0.50 || pHTel < 0.04 || pHTpi < 0.016) {
      ATH_MSG_INFO("  pHT has abnormal value!  pHTel = " << pHTel << "  pHTpi = " << pHTpi << "     TrtPart: " << TrtPart << "  SL: " << SL[TrtPart] << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire << "  Occ: " << Occ[TrtPart]);
      continue;
    }
      
    if (driftcircle->highLevel()) {pHTel_prod *=     pHTel;  pHTpi_prod *=     pHTpi;}
    else                          {pHTel_prod *= 1.0-pHTel;  pHTpi_prod *= 1.0-pHTpi;}

    ATH_MSG_DEBUG (nTRThits << "  TrtPart: " << TrtPart << "  SL: " << SL[TrtPart] << "  ZRpos: " << ZRpos[TrtPart] <<
      "  TWdist: " << rTrkWire << "  Occ: " << Occ[TrtPart] << "  pHT: " << pHTel << " " << pHTpi);




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

  ATH_MSG_DEBUG ("---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("  nTRThits: " << nTRThits << "  : " << nTRThitsHT << "  pHTel_prod: " << pHTel_prod << "  pHTpi_prod: " << pHTpi_prod << "  probEl: " << prob_El_HT);
  ATH_MSG_DEBUG ("---------------------------------------------------------------------------------------");
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
  const char * storeName[2] = {"/TRT/Calib/PIDver_New", "/TRT/Calib/PID_RToTver_New"};
  const char * objName[2]   = {"TRT",            "TRT_RToT"};

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
        ATH_MSG_INFO(calcName[ic]<<" Database entry written on:"<<day<<"/"<<month<<"/"<<year<<" (DD/MM/YY)");
        
        calc[ic]->FillBlob(BlobStart);
      }
      
    } else {
      // m_log << MSG::ERROR << "Problem reading condDB object." << endreq;
      ATH_MSG_ERROR ("Problem reading condDB object. -"<<calcName[ic]);
      //return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
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
