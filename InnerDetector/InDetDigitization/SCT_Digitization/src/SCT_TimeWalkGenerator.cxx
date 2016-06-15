/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_TimeWalkGenerator.h"

#include "CommissionEvent/ComTime.h"

// STL classes
#include <cmath>

// random number service includes
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"  // for RandGaussZiggurat

// constructor
SCT_TimeWalkGenerator::SCT_TimeWalkGenerator(const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_comTime(0),
    m_ComTime(0),
    m_rndmEngine(0)
{
  declareInterface< ISCT_TimeWalkGenerator >( this );

  declareProperty("UseComTimeFlag", m_useComTimeFlag=true, "Flag to set ComTime");
  declareProperty("TimeJitter",     m_timeJitter=0,        "Time jitter");
  declareProperty("TimePerBCO",     m_timePerBCO=25.0,     "Time per BCO - should be 25ns");
  declareProperty("TimeZero",       m_timeZero=5.0,        "Time zero...?");
}

// Destructor: 
SCT_TimeWalkGenerator::~SCT_TimeWalkGenerator() {}

// Initialize
StatusCode SCT_TimeWalkGenerator::initialize() {
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_DEBUG ( "SCT_TimeWalkGenerator::initialize()" );

  //**  ComTime for Cosmic Simulation in Pit 
  if (m_useComTimeFlag) {
    // Retrieve Com time for Cosmic Simulation in Pit  
    sc = evtStore()->retrieve(m_ComTime,"ComTime") ;
    if (sc.isFailure()) {
      ATH_MSG_ERROR ( "\tDid not find tool needed for cosmic/commissioning timing: ComTime "<<m_ComTime);
      return sc ;
    }
    m_comTime = m_ComTime->getTime() ;
    ATH_MSG_INFO ( "\tFound tool for cosmic/commissioning timing: ComTime " <<m_ComTime ) ;
    return sc ;
  }

  return sc ;
}

// Finalize
StatusCode SCT_TimeWalkGenerator::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "SCT_TimeWalkGenerator::finalize() failed" );
    return sc ;
  }
  ATH_MSG_DEBUG ( "SCT_TimeWalkGenerator::finalize()" );
  return sc ;

}

int SCT_TimeWalkGenerator::relativeBunch(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge) const {
  int BCID=0 ;
  double overdrive  = intimethreshold - threshold ;   
  double curvature  =  7.6e7*overdrive - 2.64e10 ;
  double divergence = -1.6*overdrive + 942 ;
  double myTimeWalk    = curvature/(pow((totalCharge.charge()-divergence),2.5)) ;
  double myTimeWalkEff = myTimeWalk + myTimeWalk*0.2*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine) ;
  double randomjitter  = CLHEP::RandFlat::shoot(m_rndmEngine,-m_timeJitter/2.0, m_timeJitter/2.0) ;
  double G4Time	      = totalCharge.time() ;
  double timing       = m_timeZero + myTimeWalkEff + randomjitter + G4Time - m_comTime ;
  BCID                = static_cast<int>(floor(timing/m_timePerBCO)) ;
  return BCID ;
}
