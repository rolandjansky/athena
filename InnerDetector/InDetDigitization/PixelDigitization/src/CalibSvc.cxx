/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibSvc.h"
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
#include <fstream>
 
/**
 ** Constructor(s)
 **/
CalibSvc::CalibSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc),
    m_rndmSvc("AtDSFMTGenSvc",name),
    m_rndmEngineName("PixelDigitization"),
    m_rndmEngine(0),
    m_useCalibCDB(true),
    m_usePixMapCDB(true),
    m_pixelCalibSvc("PixelCalibSvc",name)
{
	declareProperty("PixelCalibSvc",m_pixelCalibSvc);
	declareProperty("UsePixMapCondDB",m_usePixMapCDB,"Use pixel map conditions DB");
	declareProperty("UseCalibCondDB",m_useCalibCDB,"Use calibration conditions DB");
	declareProperty("RndmSvc",         m_rndmSvc,          "Random Number Service used in SCT & Pixel digitization");
	declareProperty("RndmEngine",      m_rndmEngineName,   "Random engine name");
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
  if (!m_pixelCalibSvc.retrieve().isSuccess()) {
	ATH_MSG_FATAL("Unable to retrieve PixelCalibSvc");
	return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved PixelCalibSvc");
  if (!m_useCalibCDB) m_pixelCalibSvc->disableDb();
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
  double tot    = m_pixelCalibSvc->getTotMean(id,Q);
  double totsig = m_pixelCalibSvc->getTotRes(id,tot);
  int ktot = static_cast<int>( CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,tot,totsig));
  if (ktot<1) ktot=1;
  return ktot;
}

