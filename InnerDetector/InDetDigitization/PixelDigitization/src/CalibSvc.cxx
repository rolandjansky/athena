/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "src/CalibSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h" 
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/PathResolver.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CalibSvc.h" 
#include <fstream>
 
/**
 ** Constructor(s)
 **/
CalibSvc::CalibSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc),
    m_rndmSvc("AtDSFMTGenSvc",name),
    m_rndmEngineName("PixelDigitization"),
    m_rndmEngine(0),
    m_totparA(70.2),
    m_totparE(-3561.25),
    m_totparC(26000),
    m_totparP1(-0.68),
    m_totparP2(0.17),
    m_discrThresh(3500),
    m_discrThreshIBL(1500),
    m_discrThreshSigma(300),
    m_intimeThresh(5000),
    m_intimeThreshIBL(1900),
    m_noiseThresh(200), 
    m_useCalibCDB(true),
    m_usePixMapCDB(true),
    m_IBLabsent(true),
    m_pixelCalibSvc("PixelCalibSvc",name),
    m_IBLParameterSvc("IBLParameterSvc",name)
{
	declareProperty("PixelCalibSvc",m_pixelCalibSvc);
	declareProperty("UsePixMapCondDB",m_usePixMapCDB,"Use pixel map conditions DB");
	declareProperty("UseCalibCondDB",m_useCalibCDB,"Use calibration conditions DB");
	declareProperty("RndmSvc",         m_rndmSvc,          "Random Number Service used in SCT & Pixel digitization");
	declareProperty("RndmEngine",      m_rndmEngineName,   "Random engine name");
	declareProperty("ToTParA",            m_totparA,              "TOT parameter A");
	declareProperty("ToTParE",            m_totparE,              "TOT parameter E");
	declareProperty("ToTParC",            m_totparC,              "TOT parameter C");
	declareProperty("ToTParP1",           m_totparP1,             "TOT smearing parameter p1");
	declareProperty("ToTParP2",           m_totparP2,             "TOT smearing parameter p2");
	declareProperty("DiscrThresh",        m_discrThresh,          "Discriminator threshold");
	declareProperty("DiscrThreshVar",     m_discrThreshSigma,     "Discriminator threshold sigma");
	declareProperty("IntimeThresh",       m_intimeThresh,         "Discriminator in-time threshold");
	declareProperty("NoiseThresh",        m_noiseThresh,          "Discriminator noise");
}
 
CalibSvc::~CalibSvc()
{
}

inline StatusCode CalibSvc::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if (riid == CalibSvc::interfaceID()){
    *ppvIf = dynamic_cast<CalibSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );

}
 
/**
 ** Initialize Service
 **/
StatusCode CalibSvc::initialize()
{
  //MsgStream log(msgSvc(), name());
 
  StatusCode result = AthService::initialize();
  if (result.isFailure())
  {
   ATH_MSG_FATAL ( "Unable to initialize the service!" );
   return result;
  }
  if (m_IBLParameterSvc.retrieve().isFailure()) {
       ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
  }
  else {
	m_IBLParameterSvc->setBoolParameters(m_usePixMapCDB,"EnableSpecialPixels");
	m_IBLParameterSvc->setBoolParameters(m_useCalibCDB,"UseCalibCondDB");
	m_IBLParameterSvc->setBoolParameters(m_IBLabsent,"IBLAbsent");
  }
  if (m_useCalibCDB) {
	if (!m_pixelCalibSvc.retrieve().isSuccess()) {
		ATH_MSG_FATAL("Unable to retrieve PixelCalibSvc");
		return StatusCode::FAILURE;
	}
	ATH_MSG_DEBUG("Retrieved PixelCalibSvc");
  }
  // read Charge Collection Probability Maps
 if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( " Can't get RndmSvc " );
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( "Retrieved RndmSvc" );
 
  }
  //
  // get the PixelDigitization random stream
  //
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName ); 
  } 
  ATH_MSG_DEBUG ( "initialized service!" );
  return result;
 
}  
 
StatusCode CalibSvc::finalize()
{
        return StatusCode::SUCCESS;
}

int CalibSvc::getTot( const Identifier & id, double Q ) const {
  // get the 'observed' TOT for a given pixelID and charge Q
  double tot    = getTotMean(id,Q);
  double totsig = getTotRes(id,tot);
  int ktot = static_cast<int>( CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,tot,totsig));
  if (ktot<1) ktot=1;
  return ktot;
}

