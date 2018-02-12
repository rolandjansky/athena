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
  m_pixid(nullptr),
  m_pixman(nullptr),
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

   msg(MSG::INFO) << "Entering PixelDCSTestReadWrite::initialize()" << endmsg;


   if(StatusCode::SUCCESS !=detStore()->retrieve(m_pixman, "Pixel") || m_pixman==0){
     msg(MSG::FATAL) << "Could not find Pixel manager "<<endmsg; 
     return StatusCode::FAILURE; 
   }
   msg(MSG::INFO)  << "Pixel manager  retrieved" << endmsg;

   if( StatusCode::SUCCESS != detStore()->retrieve( m_pixid,"PixelID") ){
     ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
     return StatusCode::FAILURE;
   }

   // if (StatusCode::SUCCESS!=service("PixelDCSSvc",m_pixelDCSSvc)) {
   if (StatusCode::SUCCESS!=m_pixelDCSSvc.retrieve()) {
     msg(MSG::FATAL) << "Unable to retrieve PixelDCSSvc" << endmsg;
     return StatusCode::FAILURE;
   } 
   msg(MSG::INFO)  << "PixelDCSSvc retrieved" << endmsg;


   if (StatusCode::SUCCESS!=m_pixelSvc.retrieve()) {
     msg(MSG::FATAL) << "Unable to retrieve PixelConditionsSummarySvc" << endmsg;
     return StatusCode::FAILURE;
   } 
   msg(MSG::INFO) << "PixelConditionsSummarySvc retrieved" << endmsg;


   return StatusCode::SUCCESS;


}


StatusCode PixelDCSTestSvc::execute(){


   msg(MSG::INFO)  << " in PixelDCSTestSvc::execute()" << endmsg;
  

  StatusCode sc = StatusCode::SUCCESS;

  sc = m_pixelDCSSvc->printData();
  if (sc.isFailure()) {
     msg(MSG::ERROR)<< "Could not print data" << endmsg;
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

	msg(MSG::ALWAYS) << "Identifier:   " << ident.get_compact()  << endmsg
			 << "                 temperature: " << m_pixelDCSSvc->getTemperature(ident) << endmsg
			 << "                 HV         : " << m_pixelDCSSvc->getHV(ident)          << endmsg
			 << "                 FSMStatus  : " << m_pixelDCSSvc->getFSMStatus(ident)    << endmsg
			 << "                 FSMState   : " << m_pixelDCSSvc->getFSMState(ident)    << endmsg;

	msg(MSG::ALWAYS) << "IdentifierHash:   " <<(unsigned int)id_hash << endmsg
			 << "                 temperature: " << m_pixelDCSSvc->getTemperature(id_hash) << endmsg
			 << "                 HV         : " << m_pixelDCSSvc->getHV(id_hash)          << endmsg
			 << "                 FSMStatus  : " << m_pixelDCSSvc->getFSMStatus(id_hash)    << endmsg
			 << "                 FSMState   : " << m_pixelDCSSvc->getFSMState(id_hash)    << endmsg;

	msg(MSG::ALWAYS) << "isActive  " << m_pixelSvc->isActive(id_hash) << endmsg;
	msg(MSG::ALWAYS) << "isGood  " << m_pixelSvc->isGood(id_hash) << endmsg;
  
      }
    }
  }

  return StatusCode::SUCCESS;

}  

StatusCode PixelDCSTestSvc::finalize(){

  msg(MSG::INFO) << "in PixelDCSTestSvc::finalize()" << endmsg;
  
  return StatusCode::SUCCESS;
}  
