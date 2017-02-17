/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PixelCellDiscriminator.cxx
//   Implementation file for class PixelCellDiscriminator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelCellDiscriminator.h"
#include "InDetSimEvent/SiTotalCharge.h"
#include <cmath>
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "TimeSvc.h"

TimeSvc::TimeSvc(const std::string& name, ISvcLocator* svc):
  AthService(name,svc),
  m_useComTime(false),
  m_comTime(0.0),
  m_timePerBCO(25.0),
  m_timeBCN(5),
  m_timeZero(30.0),
  m_timeJitter(0),
  m_eventStore("StoreGateSvc", name),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0)
{
	declareProperty("RndmSvc",    m_rndmSvc,        "Random Number Service used in Pixel digitization");
	declareProperty("RndmEngine", m_rndmEngineName, "Random engine name");
	declareProperty("TimePerBCO", m_timePerBCO,     "Time per BCO - should be 25ns");
	declareProperty("TimeBCN",    m_timeBCN,        "Number of BCID");	
	declareProperty("TimeJitter", m_timeJitter,     "Time jitter");
	declareProperty("TimeZero",   m_timeZero,       "Time zero...?");
  declareProperty("UseComTime", m_useComTime,     "Use ComTime for timing");
}

TimeSvc::~TimeSvc() { }

inline StatusCode TimeSvc::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if (riid == TimeSvc::interfaceID()) {
    *ppvIf = dynamic_cast<TimeSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface(riid,ppvIf);
}


//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode TimeSvc::initialize() {

  CHECK(m_rndmSvc.retrieve());
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (!m_rndmEngine) {
    ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
    return StatusCode::FAILURE;
  } 
  else { 
    ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);  
  }

  if (m_useComTime) {
    CHECK(m_eventStore.retrieve());
    if (StatusCode::SUCCESS==m_eventStore->retrieve(m_ComputedTime,"ComTime")) {
      m_comTime = m_ComputedTime->getTime();
      ATH_MSG_DEBUG("Found tool for cosmic/commissioning timing: ComTime");
    } 
    else {
      ATH_MSG_WARNING("Did not find tool needed for cosmic/commissioning timing: ComTime");
    }
  }

  ATH_MSG_DEBUG("TimeSvc::initialize()");
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode TimeSvc::finalize() {
  return StatusCode::SUCCESS;
}

int TimeSvc::relativeBunch(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge, bool CTW) const {

  int BCID=0;
  double myTimeWalkEff = 0.;
  if (CTW) {
    double overdrive  = intimethreshold - threshold ;

    //my TimeWalk computation through PARAMETRIZATION (by Francesco De Lorenzi - Milan)
    //double curvature  =  7.6e7*overdrive-2.64e10;
    //double divergence = -1.6*overdrive+942 ;

    //double myTimeWalk    = curvature/(pow((totalCharge.charge()-divergence),2.5));

    //my TimeWalk computation through PARAMETRIZATION from 2009 cosmic data (by I. Ibragimov and D. Miller)
    double p1 = 20./log(intimethreshold/overdrive);
    double p0 = p1 * log (1. - threshold/100000.);

    double myTimeWalk    = -p0 -p1 * log(1. - threshold/totalCharge.charge());

    myTimeWalkEff = myTimeWalk+myTimeWalk*0.2*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
  } 

  double randomjitter  = CLHEP::RandFlat::shoot(m_rndmEngine,(-m_timeJitter/2.0),(m_timeJitter/2.0));    	

  //double G4Time	 = totalCharge.time();

  double G4Time = getG4Time(totalCharge);
  double timing        = m_timeZero+myTimeWalkEff+(randomjitter)+G4Time-m_comTime; 
  BCID                 = static_cast<int>(floor(timing/m_timePerBCO));
  //ATH_MSG_DEBUG (  CTW << " , " << myTimeWalkEff << " , " << G4Time << " , " << timing << " , " << BCID );    

  return BCID;
}

//====================================================================
// This is the new parameterization based on the 2015 collision data.
//====================================================================
int TimeSvc::relativeBunch2015(const SiTotalCharge &totalCharge, int barrel_ec, int layer_disk, int moduleID) const {

  //=================================
  // 2016.03.29  Soshi.Tsuno@cern.ch
  //
  // The time walk effect is directly tuned with timing scan data (collision) in 2015.
  // 
  // See reference in the talk,
  // https://indico.cern.ch/event/516099/contributions/1195889/attachments/1252177/1846815/pixelOffline_timing_04.04.2016_soshi.pdf
  //
  // Ideally, it could be directly parameterized as a function of given ToT.
  // However, the ToT calibration was changed over 2015-2016, where newly calibrated ToT value was not available for 2016. 
  // For instance, the b-layer charge tuning was changed from ToT30@MIP (2015) to ToT18@MIP (2016).
  // Thus the time walk effect needs to be parameterized with more universal value, that is, charge information.
  // But it was non-trivial because of the migration effect between the border in ToT.
  //
  // Here in 2015 version, we apply the threshold of the 60% total charge to get a certain ToT value, 
  // which most describes the data timing structure.
  //
  // 60% working point tune-2
  double prob = 0.0;
  if (barrel_ec==0 && layer_disk==1) {
    if (abs(moduleID)==0) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9349; }   // corresponds to ToT=4
      else if (totalCharge.charge()<4150.0) { prob = 0.2520; }   //                ToT=5
      else if (totalCharge.charge()<4600.0) { prob = 0.0308; }   //                ToT=6
      else if (totalCharge.charge()<5250.0) { prob = 0.0160; }   //                ToT=7
      else if (totalCharge.charge()<5850.0) { prob = 0.0104; }   //                ToT=8
      else if (totalCharge.charge()<6500.0) { prob = 0.0127; }   //                ToT=9
    }
    if (abs(moduleID)==1) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9087; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2845; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0504; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0198; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0141; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0122; }
    }
    if (abs(moduleID)==2) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9060; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2885; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0387; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0126; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0116; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0052; }
    }
    if (abs(moduleID)==3) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8774; }
      else if (totalCharge.charge()<4150.0) { prob = 0.3066; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0449; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0188; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0169; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0096; }
    }
    if (abs(moduleID)==4) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8725; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2962; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0472; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0188; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0141; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0130; }
    }
    if (abs(moduleID)==5) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8731; }
      else if (totalCharge.charge()<4150.0) { prob = 0.3443; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0686; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0243; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0139; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0089; }
    }
    if (abs(moduleID)==6) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8545; }
      else if (totalCharge.charge()<4150.0) { prob = 0.2946; }
      else if (totalCharge.charge()<4600.0) { prob = 0.0524; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0218; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0218; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0191; }
    }
  }
  if (barrel_ec==0 && layer_disk==2) {
    if (abs(moduleID)==0) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9479; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6051; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2031; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0735; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0462; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0272; }
    }
    if (abs(moduleID)==1) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9736; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6344; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2439; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1000; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0435; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0335; }
    }
    if (abs(moduleID)==2) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9461; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6180; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1755; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0647; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0476; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0470; }
    }
    if (abs(moduleID)==3) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9542; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5839; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1899; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0604; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0576; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0285; }
    }
    if (abs(moduleID)==4) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9233; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5712; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1633; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0796; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0612; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0384; }
    }
    if (abs(moduleID)==5) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8994; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5176; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1626; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0698; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0416; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0382; }
    }
    if (abs(moduleID)==6) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8919; }
      else if (totalCharge.charge()<4150.0) { prob = 0.5313; }
      else if (totalCharge.charge()<4600.0) { prob = 0.1585; }
      else if (totalCharge.charge()<5250.0) { prob = 0.0520; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0318; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0254; }
    }
  }
  if (barrel_ec==0 && layer_disk==3) {
    if (abs(moduleID)==0) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9182; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6744; }
      else if (totalCharge.charge()<4600.0) { prob = 0.3174; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1460; }
      else if (totalCharge.charge()<5850.0) { prob = 0.1001; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0587; }
    }
    if (abs(moduleID)==1) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9255; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6995; }
      else if (totalCharge.charge()<4600.0) { prob = 0.3046; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1449; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0954; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0608; }
    }
    if (abs(moduleID)==2) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9419; }
      else if (totalCharge.charge()<4150.0) { prob = 0.7380; }
      else if (totalCharge.charge()<4600.0) { prob = 0.3346; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1615; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0726; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0564; }
    }
    if (abs(moduleID)==3) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9319; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6747; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2640; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1018; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0588; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0502; }
    }
    if (abs(moduleID)==4) {
      if      (totalCharge.charge()<4100.0) { prob = 0.9276; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6959; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2859; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1214; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0776; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0387; }
    }
    if (abs(moduleID)==5) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8845; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6270; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2798; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1209; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0706; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0703; }
    }
    if (abs(moduleID)==6) {
      if      (totalCharge.charge()<4100.0) { prob = 0.8726; }
      else if (totalCharge.charge()<4150.0) { prob = 0.6358; }
      else if (totalCharge.charge()<4600.0) { prob = 0.2907; }
      else if (totalCharge.charge()<5250.0) { prob = 0.1051; }
      else if (totalCharge.charge()<5850.0) { prob = 0.0646; }
      else if (totalCharge.charge()<6500.0) { prob = 0.0685; }
    }
  }

  double G4Time = getG4Time(totalCharge);
  double rnd    = CLHEP::RandFlat::shoot(m_rndmEngine,0.0,1.0);    	

  double timeWalk = 0.0;
  if (rnd<prob) { timeWalk = 25.0; }

  int BCID = static_cast<int>(floor((G4Time+m_timeZero+timeWalk)/m_timePerBCO));

  return BCID;
}

double TimeSvc::getG4Time(const SiTotalCharge &totalCharge) const {

  // If there is one single charge, return its time:
  if (totalCharge.chargeComposition().empty()) { return totalCharge.time(); }

  std::list<SiCharge>::const_iterator p_charge=totalCharge.chargeComposition().begin();

  int findfirst = 0;
  SiCharge first = *p_charge;

  // Look for first charge which is not noise
  for ( ; p_charge!=totalCharge.chargeComposition().end() ; p_charge++) {
    if (p_charge->processType()!=SiCharge::noise) {
      findfirst = 1;
      break;
    }
  }

  // if all charges were noise, return the time of the highest charge
  if (findfirst == 0) return totalCharge.time();   

  // look for the earlist charge among the remaining non-noise charges:
  first = *p_charge;
  p_charge++;

  for ( ; p_charge!=totalCharge.chargeComposition().end() ; p_charge++) {
    if (p_charge->time()<first.time() && p_charge->processType()!=SiCharge::noise) first = *p_charge;
  }
  return first.time();
}
