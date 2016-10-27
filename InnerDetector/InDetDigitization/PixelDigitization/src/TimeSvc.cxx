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

TimeSvc::TimeSvc(const std::string& name, ISvcLocator* svc ) :
  AthService(name,svc),
  m_comTime(0.0),
  m_timePerBCO(25.0),
  m_timeBCN(5),
  m_timeZero(30.0),
  m_timeJitter(0),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0)
{
	declareProperty("RndmSvc",m_rndmSvc,"Random Number Service used in Pixel digitization");
	declareProperty("RndmEngine",m_rndmEngineName,"Random engine name");
	declareProperty("TimePerBCO",m_timePerBCO,"Time per BCO - should be 25ns");
	declareProperty("TimeBCN",            m_timeBCN,              "Number of BCID");	
	declareProperty("TimeJitter",         m_timeJitter,           "Time jitter");
	declareProperty("TimeZero",           m_timeZero,             "Time zero...?");
}

TimeSvc::~TimeSvc()
{}


inline StatusCode TimeSvc::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if (riid == TimeSvc::interfaceID()){
    *ppvIf = dynamic_cast<TimeSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );

}


//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode TimeSvc::initialize() {
  StatusCode sc = AthService::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "TimeSvc::initialize() failed");
    return sc ;
  }
  if (m_rndmSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get RndmSvc");
	return StatusCode::FAILURE;
  }
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
	ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
	return StatusCode::FAILURE;
  } else { 
	ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName ); 
  }


  ATH_MSG_DEBUG ( "TimeSvc::initialize()");
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode TimeSvc::finalize() {
  StatusCode sc = AthService::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "TimeSvc::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "TimeSvc::finalize()");
  return sc ;
}

int TimeSvc::relativeBunch(const double threshold,
					  const double intimethreshold,
					  const SiTotalCharge &totalCharge, bool CTW) const {
    int BCID=0;
    double myTimeWalkEff = 0.;
    if(CTW){
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

double TimeSvc::getG4Time(const SiTotalCharge &totalCharge) const
{
  // If there is one single charge, return its time:
  if (totalCharge.chargeComposition().empty()) return totalCharge.time();
  std::list<SiCharge>::const_iterator p_charge=totalCharge.chargeComposition().begin();

  // look for the earliest charge and return its time

  //for ( std::list<SiCharge>::const_iterator g_charge=totalCharge.chargeComposition().begin();
  //g_charge!=totalCharge.chargeComposition().end() ; g_charge++) {
  //  const SiCharge::Process gproc = g_charge->processType();
  //  double gtime = g_charge->time();
  //  double gcharge = g_charge->charge();
  //  ATH_MSG_DEBUG ( "process = " << gproc << "time = " << gtime << "charge = " << gcharge );
  //}
  //ATH_MSG_DEBUG ( "****************************************************************");


  int findfirst = 0;
  SiCharge first = *p_charge;
  //
  // Look for first charge which is not noise
  //
  for ( ; p_charge!=totalCharge.chargeComposition().end() ; p_charge++) {
    if (p_charge->processType()!=SiCharge::noise) {
      findfirst = 1;
      break;
    }
  }
  //
  // if all charges were noise, return the time of the highest charge
  //
  if (findfirst == 0) return totalCharge.time();   
  //
  // look for the earlist charge among the remaining non-noise charges:
  //
  first = *p_charge;
  p_charge++;

  for ( ; p_charge!=totalCharge.chargeComposition().end() ; p_charge++) {
    if (p_charge->time()<first.time() && p_charge->processType()!=SiCharge::noise) first = *p_charge;
  }

  //ATH_MSG_DEBUG ( "first.time() = " << first.time() << " process type = " << p_charge->processType());
  return first.time();
}
