/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelOfflineCalibSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "StoreGate/StoreGate.h" 
#include "Identifier/Identifier.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "PixelConditionsTools/IPixelRecoDbTool.h"
//================ Constructor =================================================

PixelOfflineCalibSvc::PixelOfflineCalibSvc(const std::string& name, ISvcLocator* sl)
  : 
  AthService(name, sl),
  m_sgSvc("StoreGateSvc",name),
  m_dbTool("PixelRecoDbTool"),
  m_pat(0)
{
  //  template for property declaration
  declareProperty("StoreGateSvc"   , m_sgSvc);
  declareProperty("PixelRecoDbTool", m_dbTool);
}

//================ Destructor =================================================

PixelOfflineCalibSvc::~PixelOfflineCalibSvc()
{}

//=============== queryInterface ==================================================
StatusCode PixelOfflineCalibSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if( IID_IPixelOfflineCalibSvc == riid )
    {
      *ppvIF = dynamic_cast < IPixelOfflineCalibSvc* > (this); 
    } else{ 
      return AthService::queryInterface(riid, ppvIF); 
    }
  addRef();
  return StatusCode::SUCCESS; 
}

//================ Initialisation =================================================

StatusCode PixelOfflineCalibSvc::initialize()
{
  
  msg(MSG::INFO) << "initialize()" << endreq;
  /*
  StatusCode sc;
  sc = AthService::initialize();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Service::initialise() failed" <<endreq; 
    return sc;
  }
  msg(MSG::INFO) << "Service::initialize() successful in " << name() << endreq;
  */
  if (m_sgSvc.retrieve().isFailure()){
    msg(MSG::FATAL) << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  else
    msg(MSG::INFO) << "StoreGateSvc retrieved!" << endreq;
 
  if ( m_dbTool.retrieve().isFailure() )
    {
      msg(MSG::ERROR) 
	<< " Failed to retrieve tool " << m_dbTool.type() << endreq;
      return StatusCode::RECOVERABLE;
    }
  else{
    msg(MSG::INFO) 
      << "Retrieved tool " <<  m_dbTool.type() << endreq;
  }
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode PixelOfflineCalibSvc::finalize()
{
  msg(MSG::INFO) <<" PixelOfflineCalibSvc Finalizing " <<endreq; 
  return StatusCode::SUCCESS; 
}


// getBarrelPhiError
double PixelOfflineCalibSvc::getBarrelErrorPhi(double eta, int etasize, 
					       int phisize) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get barrel error phi" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  int ibin = m_pat->getPixelClusterErrorData()->getBarrelBin(eta, etasize, phisize);
  if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
  return m_pat->getPixelClusterErrorData()->getPixelBarrelPhiError(ibin);
}
  
// getBarrelEtaError
double PixelOfflineCalibSvc::getBarrelErrorEta(double eta, int etasize, 
					       int phisize) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get barrel error eta" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){
    if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib pointer undefined!" 
                                 << endreq;
    if(msgLvl(MSG::ERROR)) msg() << "returning 400 um/sqrt(12.)" << endreq;
    return 115.5*CLHEP::micrometer;
  }
  if(!m_pat->getPixelClusterErrorData()){
    if(msgLvl(MSG::ERROR)){
      msg() << "PixelClusterErrorData pointer undefined!" << endreq;
      msg() << "returning 400 um/sqrt(12.)" << endreq;
    }
    return 115.5*CLHEP::micrometer;
  }
  int ibin = m_pat->getPixelClusterErrorData()->getBarrelBin(eta, etasize, phisize);
  if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
  return m_pat->getPixelClusterErrorData()->getPixelBarrelEtaError(ibin);
}

// IBL
// getIBLPhiError
double PixelOfflineCalibSvc::getIBLErrorPhi(double eta, int etasize, 
					       int phisize) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get IBL error phi" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){
     if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib pointer undefined!" 
				  << endreq;
     if(msgLvl(MSG::ERROR)) msg() << "returning 50 um/sqrt(12.)" << endreq;
     return 14.43*CLHEP::micrometer;
  }
  int v1 =  m_pat->getPixelClusterErrorData()->getVersion();
  if(v1<-1){
    int ibin = m_pat->getPixelClusterErrorData()->getBarrelBin(eta, etasize, phisize);
    if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
    return m_pat->getPixelClusterErrorData()->getPixelIBLPhiError(ibin);
  }
  else{
    if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib getPixelClusterErrorData() getPixelIBLPhiError undefined for IBL! " << v1<< endreq;
    if(msgLvl(MSG::ERROR)) msg() << "returning 50 um/sqrt(12.)" << endreq;
    return 14.43*CLHEP::micrometer;
  }   
}
  
// getIBLEtaError
double PixelOfflineCalibSvc::getIBLErrorEta(double eta, int etasize, 
					       int phisize) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get IBL error eta" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){
    if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib pointer undefined!" 
                                 << endreq;
    if(msgLvl(MSG::ERROR)) msg() << "returning 250 um/sqrt(12.)" << endreq;
    return 72.2*CLHEP::micrometer;
  }
  if(!m_pat->getPixelClusterErrorData()){
    if(msgLvl(MSG::ERROR)){
      msg() << "PixelClusterErrorData pointer undefined!" << endreq;
      msg() << "returning 250 um/sqrt(12.)" << endreq;
    }
    return 72.2*CLHEP::micrometer;
  }
  int v1 =  m_pat->getPixelClusterErrorData()->getVersion();
  if(v1<-1){
    int ibin = m_pat->getPixelClusterErrorData()->getBarrelBin(fabs(eta), etasize, phisize);
    if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
    return m_pat->getPixelClusterErrorData()->getPixelIBLEtaError(ibin);
  }
  else{
    if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib getPixelClusterErrorData() getPixelIBLEtaError undefined for IBL! " <<v1<< endreq;
    if(msgLvl(MSG::ERROR)) msg() << "returning 250 um/sqrt(12.)" << endreq;
    return 72.2*CLHEP::micrometer;
  }
}
//

// getEndcapPhiError
double PixelOfflineCalibSvc::getEndCapErrorPhi(int etasize, 
					       int phisize) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get endcap error phi" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){
    if(msgLvl(MSG::ERROR)){
      msg() << "PixelOfflineCalib pointer undefined!" << endreq;
      msg() << "returning 50 um/sqrt(12.)" << endreq;
    }
    return 14.43*CLHEP::micrometer;
  }
  if(!m_pat->getPixelClusterErrorData()){
    if(msgLvl(MSG::ERROR)){
      msg() << "PixelClusterErrorData pointer undefined!" << endreq;
      msg() << "returning 50 um/sqrt(12.)" << endreq;
    }
    return 14.43*CLHEP::micrometer;
  }
  int ibin = m_pat->getPixelClusterErrorData()->getEndcapBin(etasize, phisize);
  if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
  return m_pat->getPixelClusterErrorData()->getPixelEndcapPhiError(ibin);
}

// getEndcapEtaError
double PixelOfflineCalibSvc::getEndCapErrorEta(int etasize, 
					       int phisize) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get endcap error eta" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  int ibin = m_pat->getPixelClusterErrorData()->getEndcapBin(etasize, phisize);
  if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
  return m_pat->getPixelClusterErrorData()->getPixelEndcapRError(ibin);
}

// getBarrelPhiError
double PixelOfflineCalibSvc::getBarrelNewErrorPhi(double ang,  
						  int phisize) 
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get barrel new error phi" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  int ibin =  m_pat->getPixelClusterOnTrackErrorData()->getBarrelBinPhi(ang, phisize);
  if(msgLvl(MSG::VERBOSE)) msg() << "bin value is" << ibin << endreq;
  double error = m_pat->getPixelClusterOnTrackErrorData()->getPixelBarrelPhiError(ibin);
  if(error < 0){ 
    if(msgLvl(MSG::ERROR)){ 
      msg() << "Something wrong, error is" << error << endreq;
      msg() << "returning 50 um/sqrt(12.)" << endreq;
    }
    return 14.4*CLHEP::micrometer;
  }
  else{
    if(msgLvl(MSG::VERBOSE)) msg() << "error is" << error << endreq;
  }
  return error;
}

// getBarrelEtaError
double PixelOfflineCalibSvc::getBarrelNewErrorEta(double eta,  
						  int phisize, int etasize) 
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get barrel new error eta " << endreq;
  m_pat = m_dbTool->getCalibPtr();
  int ibin =  m_pat->getPixelClusterOnTrackErrorData()->getBarrelBinEta(eta, etasize, phisize);
  if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
  double error =  m_pat->getPixelClusterOnTrackErrorData()->getPixelBarrelEtaError(ibin);
  if(error < 0){
    if(msgLvl(MSG::ERROR)){
      msg() << "Something wrong, error is" << error << endreq;
      msg() << "returning 400 um/sqrt(12.)" << endreq;
    }
    return 115.5*CLHEP::micrometer;
  }
  else{
    if(msgLvl(MSG::VERBOSE)) msg() << "error is" << error << endreq;
  }
  return error;
}
// IBL
// getIBLPhiError
double PixelOfflineCalibSvc::getIBLNewErrorPhi(double ang,  
						  int phisize) 
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get IBL new error phi" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  int v2 =  m_pat->getPixelClusterOnTrackErrorData()->getVersion();
  if(v2<-1){
    int ibin =  m_pat->getPixelClusterOnTrackErrorData()->getIBLBinPhi(ang, phisize);
    if(msgLvl(MSG::VERBOSE)) msg() << "bin value is" << ibin << endreq;
    double error = m_pat->getPixelClusterOnTrackErrorData()->getPixelIBLPhiError(ibin);
    if(error < 0){ 
      if(msgLvl(MSG::ERROR)){ 
	msg() << "Something wrong, error is" << error << endreq;
	msg() << "returning 50 um/sqrt(12.)" << endreq;
      }
      return 14.4*CLHEP::micrometer;
    }
    else{
      if(msgLvl(MSG::VERBOSE)) msg() << "error is" << error << endreq;
      return error;
    }
  }
  else{
    if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib getPixelClusterOnTrackErrorData() getPixelIBLPhiError undefined !" << endreq;
    if(msgLvl(MSG::ERROR)) msg() << "returning 50 um/sqrt(12.)" << endreq;
    return 14.4*CLHEP::micrometer;
  }
}

// getIBLEtaError
double PixelOfflineCalibSvc::getIBLNewErrorEta(double eta, int etasize) 
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get barrel new error eta " << endreq;
  m_pat = m_dbTool->getCalibPtr();
  int v2 = m_pat->getPixelClusterOnTrackErrorData()->getVersion();
  if(v2<-1){
    int ibin =  m_pat->getPixelClusterOnTrackErrorData()->getIBLBinEta(fabs(eta), etasize);
    if(msgLvl(MSG::VERBOSE)) msg() << "for bin " << ibin << endreq;
    double error =  m_pat->getPixelClusterOnTrackErrorData()->getPixelIBLEtaError(ibin);
    if(error < 0){
      if(msgLvl(MSG::ERROR)){
	msg() << "Something wrong, error is" << error << endreq;
	msg() << "returning 250 um/sqrt(12.)" << endreq;
      }
      return 72.2*CLHEP::micrometer;
    }
    else{
      if(msgLvl(MSG::VERBOSE)) msg() << "error is" << error << endreq;
      return error;
    }
  }
  else{
    if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib getPixelClusterOnTrackErrorData() getPixelIBLEtaError undefined !" << v2<<endreq;
    if(msgLvl(MSG::ERROR)) msg() << "returning 250 um/sqrt(12.)" << endreq;
    return 72.2*CLHEP::micrometer;
  }
}

//
// getBarrel charge interpolation normalization
double PixelOfflineCalibSvc::getBarrelDeltaX(int nrows, 
					     float angle) const
{
  ATH_MSG_VERBOSE ( "get barrel deltax" );
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){   
    ATH_MSG_WARNING ( "No tool!" ); 
    return 0;
  }
  if(m_pat->getPixelChargeInterpolationParameters() == 0){ 
    ATH_MSG_WARNING ( "No charge interpolation!" );
    return 0;
  }
  ATH_MSG_VERBOSE ( "Barrel deltax = " <<  m_pat->getPixelChargeInterpolationParameters()->getDeltaXbarrel(nrows, angle, 1) );
  return m_pat->getPixelChargeInterpolationParameters()->getDeltaXbarrel(
									 nrows, angle, 1);
}

double PixelOfflineCalibSvc::getBarrelDeltaY(int ncol, 
					     float eta) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get barrel deltay" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){   
    if(msgLvl(MSG::WARNING)) msg() << "No tool!" << std::endl;
    return 0;
  }
  if(m_pat->getPixelChargeInterpolationParameters() == 0){ 
    if(msgLvl(MSG::WARNING)) msg() << "No charge interpolation!" << endreq;
    return 0;
  }
  return m_pat->getPixelChargeInterpolationParameters()->getDeltaYbarrel(
									 ncol, eta, 1);
}

bool PixelOfflineCalibSvc::includesIBLParams() const {
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){
    ATH_MSG_WARNING ( "No tool!" );
    return false;
  }
  if(m_pat->getPixelChargeInterpolationParameters() == 0){
    ATH_MSG_WARNING ( "No charge interpolation!" );
    return false;
  }
  return m_pat->getPixelChargeInterpolationParameters()->getVersion()<-1;
}

//
// getIBL charge interpolation normalization
double PixelOfflineCalibSvc::getIBLDeltaX(int nrows,
					  float angle) const
{
  ATH_MSG_VERBOSE ( "get barrel deltax" );
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){
    ATH_MSG_WARNING ( "No tool!" );
    return 0;
  }
  if(m_pat->getPixelChargeInterpolationParameters() == 0){
    ATH_MSG_WARNING ( "No charge interpolation!" );
    return 0;
  }
  ATH_MSG_VERBOSE ( "Barrel deltax = " <<  m_pat->getPixelChargeInterpolationParameters()->getDeltaXbarrel(nrows, angle, 0) );
  return m_pat->getPixelChargeInterpolationParameters()->getDeltaXbarrel(
                                                                         nrows, angle, 0);
}

double PixelOfflineCalibSvc::getIBLDeltaY(int ncol,
                                             float eta) const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get barrel deltay" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){
    if(msgLvl(MSG::WARNING)) msg() << "No tool!" << std::endl;
    return 0;
  }
  if(m_pat->getPixelChargeInterpolationParameters() == 0){
    if(msgLvl(MSG::WARNING)) msg() << "No charge interpolation!" << endreq;
    return 0;
  }
  return m_pat->getPixelChargeInterpolationParameters()->getDeltaYbarrel(
                                                                         ncol, fabs(eta), 0);
}


double PixelOfflineCalibSvc::getEndcapDeltaX() const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get endcap deltax" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){   
    if(msgLvl(MSG::WARNING)) msg() << "No tool!" << std::endl;
    return 0;
  }
  if(m_pat->getPixelChargeInterpolationParameters() == 0){ 
    if(msgLvl(MSG::WARNING)) msg() << "No charge interpolation!" << endreq;
    return 0;
  }
  return m_pat->getPixelChargeInterpolationParameters()->getDeltaXendcap();
}

double PixelOfflineCalibSvc::getEndcapDeltaY() const
{
  if(msgLvl(MSG::VERBOSE)) msg() << "get endcap deltay" << endreq;
  m_pat = m_dbTool->getCalibPtr();
  if(!m_pat){   
    if(msgLvl(MSG::WARNING)) msg() << "No tool!" << std::endl;
    return 0;
  }
  if(m_pat->getPixelChargeInterpolationParameters() == 0){ 
    if(msgLvl(MSG::WARNING)) msg() << "No charge interpolation!" << endreq;
    return 0;
  }
  return m_pat->getPixelChargeInterpolationParameters()->getDeltaYendcap();
}







