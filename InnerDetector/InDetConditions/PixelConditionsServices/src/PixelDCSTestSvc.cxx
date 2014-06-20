/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSTestSvc.h"

#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelConditionsData/PixelDCSData.h"
#include "PixelConditionsServices/IPixelDCSSvc.h"
#include "PixelConditionsSummarySvc.h"

PixelDCSTestSvc::PixelDCSTestSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_pixelDCSSvc("PixelDCSSvc", name),
  m_pixelSvc("PixelConditionsSummarySvc", name)
{

  declareProperty("PixelDCSSvc",m_pixelDCSSvc);
  declareProperty("PixelConditionsSummarySvc",m_pixelSvc);

}

  
PixelDCSTestSvc::~PixelDCSTestSvc()
{}


StatusCode PixelDCSTestSvc::initialize()
{

   msg(MSG::INFO) << "Entering PixelDCSTestReadWrite::initialize()" << endreq;


   if(StatusCode::SUCCESS !=detStore()->retrieve(m_pixman, "Pixel") || m_pixman==0){
     msg(MSG::FATAL) << "Could not find Pixel manager "<<endreq; 
     return StatusCode::FAILURE; 
   }
   msg(MSG::INFO)  << "Pixel manager  retrieved" << endreq;

   if( StatusCode::SUCCESS != detStore()->retrieve( m_pixid,"PixelID") ){
     ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
     return StatusCode::FAILURE;
   }

   // if (StatusCode::SUCCESS!=service("PixelDCSSvc",m_pixelDCSSvc)) {
   if (StatusCode::SUCCESS!=m_pixelDCSSvc.retrieve()) {
     msg(MSG::FATAL) << "Unable to retrieve PixelDCSSvc" << endreq;
     return StatusCode::FAILURE;
   } 
   msg(MSG::INFO)  << "PixelDCSSvc retrieved" << endreq;


   if (StatusCode::SUCCESS!=m_pixelSvc.retrieve()) {
     msg(MSG::FATAL) << "Unable to retrieve PixelConditionsSummarySvc" << endreq;
     return StatusCode::FAILURE;
   } 
   msg(MSG::INFO) << "PixelConditionsSummarySvc retrieved" << endreq;


   return StatusCode::SUCCESS;


}


StatusCode PixelDCSTestSvc::execute(){


   msg(MSG::INFO)  << " in PixelDCSTestSvc::execute()" << endreq;
  

  StatusCode sc = StatusCode::SUCCESS;

  sc = m_pixelDCSSvc->printData();
  if (sc.isFailure()) {
     msg(MSG::ERROR)<< "Could not print data" << endreq;
    return(StatusCode::FAILURE);
  }

  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax; 

  itermin = m_pixman->getDetectorElementBegin(); 
  itermax = m_pixman->getDetectorElementEnd(); 

  for( iter=itermin; iter !=itermax; ++iter){ 
    const InDetDD::SiDetectorElement* element = *iter; 
    if(element !=0){ 
      Identifier ident = element->identify(); 
      if(m_pixid->is_pixel(ident)){


	IdentifierHash id_hash = m_pixid->wafer_hash(ident);

	msg(MSG::ALWAYS) << "Identifier:   " << ident.get_compact()  << endreq
			 << "                 temperature: " << m_pixelDCSSvc->getTemperature(ident) << endreq
			 << "                 HV         : " << m_pixelDCSSvc->getHV(ident)          << endreq
			 << "                 FSMStatus  : " << m_pixelDCSSvc->getFSMStatus(ident)    << endreq
			 << "                 FSMState   : " << m_pixelDCSSvc->getFSMState(ident)    << endreq;

	msg(MSG::ALWAYS) << "IdentifierHash:   " <<(unsigned int)id_hash << endreq
			 << "                 temperature: " << m_pixelDCSSvc->getTemperature(id_hash) << endreq
			 << "                 HV         : " << m_pixelDCSSvc->getHV(id_hash)          << endreq
			 << "                 FSMStatus  : " << m_pixelDCSSvc->getFSMStatus(id_hash)    << endreq
			 << "                 FSMState   : " << m_pixelDCSSvc->getFSMState(id_hash)    << endreq;

	msg(MSG::ALWAYS) << "isActive  " << m_pixelSvc->isActive(id_hash) << endreq;
	msg(MSG::ALWAYS) << "isGood  " << m_pixelSvc->isGood(id_hash) << endreq;
  
      }
    }
  }

  return StatusCode::SUCCESS;

}  

StatusCode PixelDCSTestSvc::finalize(){

  msg(MSG::INFO) << "in PixelDCSTestSvc::finalize()" << endreq;
  
  return StatusCode::SUCCESS;
}  
