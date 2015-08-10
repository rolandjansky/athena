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
#include <algorithm>
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "StoreGate/StoreGate.h" 
#include "Identifier/Identifier.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "PixelConditionsTools/IPixelRecoDbTool.h"

#include "InDetIdentifier/PixelID.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelGeoModel/IBLParameterSvc.h"

// for callback
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"


//================ Constructor =================================================

PixelOfflineCalibSvc::PixelOfflineCalibSvc(const std::string& name, ISvcLocator* sl)
  : 
  AthService(name, sl),
  m_dbTool("PixelRecoDbTool"),
  m_pat(0),
  m_detStore("DetectorStore", name),
  m_pixelCabling("PixelCablingSvc",name),
  m_sgSvc("StoreGateSvc",name),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_HitDiscCnfg(-1),
  m_IBLToToverflowBin(0),
  m_HDCfromCOOL(true)
{
  //  template for property declaration
  declareProperty("StoreGateSvc"   , m_sgSvc);
  declareProperty("PixelRecoDbTool", m_dbTool);
  declareProperty("OverrideIBLToToverflowBin", m_IBLToToverflowBin);
  declareProperty("HDCFromCOOL", m_HDCfromCOOL);
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
  
  StatusCode sc = m_detStore.retrieve();
  if(!sc.isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve detector store");
    return StatusCode::FAILURE;
  }

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

// IBL Overflow:
  if(m_HDCfromCOOL){
    if (m_pixelCabling.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Failed to retrieve service " << m_pixelCabling << endreq;
        return StatusCode::FAILURE;
    } else
        msg(MSG::INFO) << "Retrieved service " << m_pixelCabling << endreq;
    
    //if (m_sgSvc->retrieve(m_pixel_id, "PixelID").isFailure()) {
    if (m_detStore->retrieve(m_pixel_id, "PixelID").isFailure()) {
        msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
        return StatusCode::FAILURE;
    } else
        msg(MSG::INFO) << "Retrieved Pixel ID helper" << endreq;

    if (m_IBLParameterSvc.retrieve().isFailure()) {
        ATH_MSG_FATAL("Could not retrieve IBLParameterSvc");
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO("Retrieved service " << m_IBLParameterSvc);

    // Read HitDiscCnfg from COOL:
    // Need to do a callback to get updated values from PixelCabling
    if (m_IBLParameterSvc->containsIBL()) { // Run-1 protection
    
        std::string m_keyHDC = "/PIXEL/HitDiscCnfg";
        const DataHandle<AthenaAttributeList> attrlist_hdc;
        if (m_detStore->contains<AthenaAttributeList>(m_keyHDC)) {
            sc = m_detStore->regFcn(&IPixelOfflineCalibSvc::IOVCallBack_HDC,
                                    dynamic_cast<IPixelOfflineCalibSvc*>(this),
                                    attrlist_hdc, m_keyHDC);
    
            if (!sc.isSuccess()) {
                ATH_MSG_FATAL("Unable to register HitDiscCnfg callback");
                return StatusCode::FAILURE;
            }
        }
    }
  }
  
  //Case where HDC should not be taken from COOL and pixel cabling dependence should be avoided, e.g. ITK
  else{
    //If ToT overflow not defined in JOs, set to 14
    if(m_IBLToToverflowBin == 0) {
      m_IBLToToverflowBin = 14;
      ATH_MSG_INFO("Not reading HitDiscCnfg from COOL and no IBL ToT Overflow set - setting to  " << m_IBLToToverflowBin);
    }
    else ATH_MSG_INFO("Not reading HitDiscCnfg from COOL - setting IBL ToT Overflow to  " << m_IBLToToverflowBin << " from Configuration");
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

int PixelOfflineCalibSvc::getIBLToToverflow() const
{
  int overflow;
  if( m_IBLToToverflowBin != 0 ) {
    overflow = m_IBLToToverflowBin;
  } else {
    if( m_HitDiscCnfg == 0 ) overflow = 14;
    if( m_HitDiscCnfg == 1 ) overflow = 15;
    if( m_HitDiscCnfg == 2 ) overflow = 16;
    if( m_HitDiscCnfg == 3 ) overflow = 14;
    if( m_HitDiscCnfg > 3 )msg(MSG::FATAL) << " IBL FE-I4 chips have non physical values for HitDiscCnfg" << endreq; 
  }
  return overflow;
}

int PixelOfflineCalibSvc::getIBLToToverflow(Identifier*) const
{
  // The version with pixel identifier is the same as without identifier. In case it is needed in the future, it should be recoded.
   
  int overflow;
  if( m_IBLToToverflowBin != 0 ) {
    overflow = m_IBLToToverflowBin;
  } else {
    if( m_HitDiscCnfg == 0 ) overflow = 14;
    if( m_HitDiscCnfg == 1 ) overflow = 15;
    if( m_HitDiscCnfg == 2 ) overflow = 16;
    if( m_HitDiscCnfg == 3 ) overflow = 14;
    if( m_HitDiscCnfg > 3 )msg(MSG::FATAL) << " IBL FE-I4 chips have non physical values for HitDiscCnfg" << endreq; 
  }
  return overflow;
}

void PixelOfflineCalibSvc::readHDC() {

    int nchip = 0;
    std::vector<int> HDCOccurence(5,0);
    for( int phi_index = 0; phi_index < 14; phi_index++ ){
      for( int eta_index = -10; eta_index < 10; eta_index++ ){
        Identifier  thiswafer = m_pixel_id->wafer_id(0,0,phi_index,eta_index);
	uint32_t maxFE = 2;
	if( eta_index < -6 || eta_index > 5 )maxFE = 1;
	for( uint32_t FE_index = 0; FE_index < maxFE; FE_index++ ){
	  Identifier FirstPix = m_pixelCabling->getPixelId(thiswafer,FE_index,1,1);
	  int HitDiscCnfg = m_pixelCabling->getHitDiscCnfg(&FirstPix);
          nchip++;
	  if ( HitDiscCnfg > 3 ) HitDiscCnfg = 4;
	  HDCOccurence[HitDiscCnfg]++;
	}
      }
    }  	     

    // If there is no common HDC value (i.e. less than 4 ouf of 5 elements in HDCOccurence are 0), issue a warning
    if (std::count(HDCOccurence.begin(), HDCOccurence.end(), 0) < 4) ATH_MSG_WARNING(" IBL FE-I4 chips have different values of HitDiscCnfg");
    
    // Now get the index for the the most frequent value
    m_HitDiscCnfg = std::distance(HDCOccurence.begin(), std::max_element(HDCOccurence.begin(),HDCOccurence.end()));
    
    msg(MSG::INFO) << " m_HitDiscCnfg = " << m_HitDiscCnfg << endreq;

}

// Callback to get HitDiscCnfg values from COOL
StatusCode PixelOfflineCalibSvc::IOVCallBack_HDC(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
    ATH_MSG_INFO("Callback for key " << keys <<  " (number " << I << ")");
    readHDC();
    return StatusCode::SUCCESS;
}
