/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidToolRun2.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "TRT_ElectronPidTools/BaseTRTPIDCalculator.h"
#include "TRT_ElectronPidTools/TRT_ElectronPidToolRun2.h"
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

// ToT Tool Interface
#include "TRT_ToT_Tools/ITRT_ToT_dEdx.h"

// Particle masses

// Math functions:
#include <cmath>

//STL includes
#include <sstream>

//#define TRTDBG ATH_MSG_INFO("To line "<<__LINE__);
//#define TRTDBG 0;

#include "TRT_ElectronPidToolRun2_HTcalculation.cxx"


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  PID Tool Constructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::TRT_ElectronPidToolRun2(const std::string& t, const std::string& n, const IInterface* p )
  :
  AthAlgTool(t,n,p),
  m_trtId(nullptr),
  m_TRTdetMgr(nullptr),
  m_minTRThits(5),
  HTcalc(*(new HTcalculator(*this))),
  m_TRTdEdxTool("TRT_ToT_dEdx"),
  m_LocalOccTool(),
  m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",n)
{
  declareInterface<ITRT_ElectronPidTool>(this);
  declareInterface<ITRT_ElectronToTTool>(this);
  declareProperty("MinimumTRThitsForIDpid", m_minTRThits);
  declareProperty("TRT_ToT_dEdx_Tool", m_TRTdEdxTool);
  declareProperty("TRT_LocalOccupancyTool", m_LocalOccTool);
  declareProperty("isData", m_DATA = true);
  declareProperty("TRTStrawSummarySvc",    m_TRTStrawSummarySvc);
  declareProperty("OccupancyUsedInPID", m_OccupancyUsedInPID=true);
}


/*****************************************************************************\
|*%%%  PID Tool Destructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::~TRT_ElectronPidToolRun2()
{
  delete &HTcalc;
}

/*****************************************************************************\
|*%%%  Initialisation  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

StatusCode InDet::TRT_ElectronPidToolRun2::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  // Get the TRT Identifier-helper:
  CHECK (detStore()->retrieve(m_trtId, "TRT_ID"));

  // Register callback function for cache updates - HT:
  const DataHandle<CondAttrListVec> aptr;;
  if (StatusCode::SUCCESS == detStore()->regFcn(&InDet::TRT_ElectronPidToolRun2::update,this, aptr, "/TRT/Calib/PID_vector" )) {
    ATH_MSG_DEBUG ("Registered callback for TRT_ElectronPidToolRun2 - HT.");
  } else {
    ATH_MSG_ERROR ("Callback registration failed for TRT_ElectronPidToolRun2 - HT! ");
  }

  /* Get the TRT_ToT_dEdx tool */
  CHECK( m_TRTdEdxTool.retrieve() );

  CHECK( m_LocalOccTool.retrieve() );

  CHECK( m_TRTStrawSummarySvc.retrieve() );
  if ( !m_TRTStrawSummarySvc.empty()) ATH_MSG_INFO( "Retrieved tool " << m_TRTStrawSummarySvc);

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

/* Jared - remove ToTcalc
double InDet::TRT_ElectronPidToolRun2::GetD(double R_Track)const {
  R_Track=fabs(R_Track);
  if(R_Track>2.) return 0;
  return 2. * sqrt( 4. - R_Track * R_Track );
}

double InDet::TRT_ElectronPidToolRun2::GetToT(unsigned int bitpattern, double HitZ, double HitR, int BEC, int Layer, int Strawlayer)const {
  return -999.99; //ToTcalc.GetToT( bitpattern, HitZ, HitR, BEC, Layer, Strawlayer);
}
*/

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
|*%%%  electronProbability - The interface method during reconstruction  %%%%*|
\*****************************************************************************/

std::vector<float>
InDet::TRT_ElectronPidToolRun2::electronProbability(const Trk::Track& track) const {

  //ATH_MSG_INFO("started electronProbabaility");
  //Initialize the return vector
  std::vector<float> PIDvalues(5);
  float & prob_El_Comb      = PIDvalues[0] = 0.5;
  float & prob_El_HT        = PIDvalues[1] = 0.5;
  float & prob_El_ToT       = PIDvalues[2] = 0.5;
  float & prob_El_Brem      = PIDvalues[3] = 0.5;
  float & occ_local         = PIDvalues[4] = 0.0;

  //  float & dEdx              = PIDvalues[2] = 0.0;
  float dEdx = 0.0;

  // Check for perigee:
  const Trk::TrackParameters* perigee = track.perigeeParameters();
  if (!perigee) return PIDvalues;

  // Get parameters at perigee and check that they are reasonable:
  const Amg::VectorX& parameterVector = perigee->parameters();
  double qOverP = parameterVector[Trk::qOverP];
  double theta  = parameterVector[Trk::theta];
  double phi    = parameterVector[Trk::phi];

  // Check the parameters are reasonable:
  if (tan(theta/2.0) < 0.0001) {
    ATH_MSG_DEBUG ("  Track has negative theta or is VERY close to beampipe! (tan(theta/2) < 0.0001). Returning default Pid values.");
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
  occ_local = m_LocalOccTool->LocalOccupancy(track);

  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("check  Got track:   pT: " << pT << "   eta: " << eta << "   phi: " << phi);
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
 
  // Jared - Development Output... 
  /*
  std::cout << "check---------------------------------------------------------------------------------------" << std::endl;
  std::cout << "check  Got track:   pT: " << pT << "   eta: " << eta << "   phi: " << phi << std::endl;
  std::cout << "check---------------------------------------------------------------------------------------" << std::endl;
  */

  // For calculation of HT probability:
  double pHTel_prod = 1.0;
  double pHTpi_prod = 1.0;

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
    bool isHTMB  = ((driftcircle->prepRawData()->getWord() & 0x00020000) > 0) ? true : false; 

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
    bool hasTrackParameters= true; // Keep track of this for HT prob calculation
    if ((*tsosIter)->trackParameters()) {
      // If we have precise information (from hit), get that:
      const Amg::Vector3D& gp = driftcircle->globalPosition();
      HitR = gp.perp();
      HitZ = gp.z();
      rTrkWire = fabs((*tsosIter)->trackParameters()->parameters()[Trk::driftRadius]);
    } else {
      // Otherwise just use the straw coordinates:
      hasTrackParameters = false; // Jared - pass this to HT calculation
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


    // Jared - Development Output... 
    /*
    std::cout << "check Hit: " << nTRThits << "  TrtPart: " << TrtPart << "  GasType: " << GasType << "  SL: " << StrawLayer
             << "  ZRpos: " << ZRpos[TrtPart] << "  TWdist: " << rTrkWire << "  Occ_Local: " << occ_local 
            << "  HTMB: " << isHTMB << std::endl;
            */

    // Then call pHT functions with these values:
    // ------------------------------------------
    double pHTel = HTcalc.getProbHT( pTrk, Trk::electron, TrtPart, GasType, StrawLayer, ZRpos[TrtPart], rTrkWire, occ_local, hasTrackParameters);
    double pHTpi = HTcalc.getProbHT( pTrk, Trk::pion,     TrtPart, GasType, StrawLayer, ZRpos[TrtPart], rTrkWire, occ_local, hasTrackParameters);

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
    
    // Jared - Development Output... 
    //std::cout << "check         pHT(el): " << pHTel << "  pHT(pi): " << pHTpi << std::endl;

  }//of loop over hits


  // If number of hits is adequate (default is 5 hits), calculate HT and ToT probability.
  if (not (nTRThits >= m_minTRThits)) return PIDvalues;

  // Calculate electron probability (HT)
  prob_El_HT = pHTel_prod / (pHTel_prod + pHTpi_prod);

  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("check  nTRThits: " << nTRThits << "  : " << nTRThitsHTMB << "  pHTel_prod: " << pHTel_prod << "  pHTpi_prod: " << pHTpi_prod << "  probEl: " << prob_El_HT);
  ATH_MSG_DEBUG ("check---------------------------------------------------------------------------------------");
  ATH_MSG_DEBUG ("");
  ATH_MSG_DEBUG ("");
    
  // Jared - Development Output... 
  /*
  std::cout << "check---------------------------------------------------------------------------------------" << std::endl;
  std::cout << "check  nTRThits: " << nTRThits << "  : " << nTRThitsHTMB << "  pHTel_prod: " << pHTel_prod << "  pHTpi_prod: " << pHTpi_prod << "  probEl: " << prob_El_HT << std::endl;
  std::cout << "check---------------------------------------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;
  */

  // Jared - ToT Implementation
  dEdx = m_TRTdEdxTool->dEdx( &track, true, false, true); // Divide by L, exclude HT hits 
  double usedHits = m_TRTdEdxTool->usedHits( &track, true, false);
  prob_El_ToT = m_TRTdEdxTool->getTest( dEdx, pTrk, Trk::electron, Trk::pion, usedHits, true ); 
  
  // Limit the probability values the upper and lower limits that are given/trusted for each part:
  double limProbHT = HTcalc.Limit(prob_El_HT); 
  double limProbToT = HTcalc.Limit(prob_El_ToT); 
  
  // Calculate the combined probability, assuming no correlations (none are expected).
  prob_El_Comb = (limProbHT * limProbToT ) / ( (limProbHT * limProbToT) + ( (1.0-limProbHT) * (1.0-limProbToT)) );
  
  // Troels: VERY NASTY NAMING, BUT AGREED UPON FOR NOW (for debugging, 27. NOV. 2014):
  prob_El_Brem = pHTel_prod; // decorates electron LH to el brem for now... (still used?) 

  //std::cout << "Prob_HT = " << prob_El_HT << "   Prob_ToT = " << prob_El_ToT << "   Prob_Comb = " << prob_El_Comb << std::endl;
     
  return PIDvalues;  
}

/* ----------------------------------------------------------------------------------- */
// Callback function to update constants from database: 
/* ----------------------------------------------------------------------------------- */

StatusCode InDet::TRT_ElectronPidToolRun2::update( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {

  ATH_MSG_DEBUG ("Updating constants for the TRT_ElectronPidToolRun2! ");

  // Callback function to update HT onset parameter cache when condDB data changes:
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
    ATH_MSG_DEBUG("IOVCALLBACK for key " << *key << " number " << I);

	// NEW reading from DB
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_INFO("HT Calculator : Reading vector format");

  const DataHandle<CondAttrListVec> channel_values;
  if (StatusCode::SUCCESS == detStore()->retrieve(channel_values, "/TRT/Calib/PID_vector" )){
        sc = HTcalc.ReadVectorDB(        channel_values  );
  } else {
        ATH_MSG_ERROR ("Problem reading condDB object. HT Calculator.");
  }

  return sc;
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
    ATH_MSG_ERROR("TRT part "<<BEC<<" Layer "<<Layer<<" only has "<<strawsPerBEC[part][Layer]<<" straws. Found index "<<StrawLayer);
    return false;
  }
  
  return true;
}

/*****************************************************************************\
|*%%%  Auxiliary function to return the HT probability to Atlfast  %%%%%%%%%%*|
|*%%%  a geometry check is performed every time here  %%%%%%%%%%%%%%%%%%%%%%%*|
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
