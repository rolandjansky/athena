/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArVHitMerger.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"


LArVHitMerger::LArVHitMerger(StoreGateSvc* detStore):
  m_log(0),
  m_detStore(detStore)
{
  // Get pointer to the message service
  ISvcLocator* svcLocator = Gaudi::svcLocator(); 
  IMessageSvc* msgSvc(0);
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if(status.isFailure())
    throw std::runtime_error("LArVHitMerger: Unable to retrieve Message Service!");

  m_log = new MsgStream(msgSvc, "LArVHitMerger");

  IService* pSvc;
  status = svcLocator->service("DetectorStore",pSvc);

  const CaloIdManager* caloIdManager;
  status = m_detStore->retrieve(caloIdManager);
  if(status.isSuccess())
    (*m_log) << MSG::DEBUG << "Constructor: CaloIDManager retrieved!" <<endreq;
  else
    throw std::runtime_error("LArVHitMerger: Unable to retrieve CaloIDManager");
  
  m_larEmID = caloIdManager->getEM_ID();
  if(m_larEmID==0)
    throw std::runtime_error("LArVHitMerger: Invalid LAr EM ID helper");
  
  m_larFcalID = caloIdManager->getFCAL_ID();
  if(m_larFcalID==0)
    throw std::runtime_error("LArVHitMerger: Invalid FCAL ID helper");
  
  m_larHecID = caloIdManager->getHEC_ID();
  if(m_larHecID==0)
    throw std::runtime_error("LArVHitMerger: Invalid HEC ID helper");    

  m_larMiniFcalID = caloIdManager->getMiniFCAL_ID();
  if(m_larMiniFcalID==0)
    throw std::runtime_error("LArVHitMerger: Invalid Mini FCAL ID helper");     
}

LArVHitMerger::~LArVHitMerger()
{
  delete m_log;
}

Identifier LArVHitMerger::ConvertID(const LArG4Identifier& a_ident)
{
  Identifier id;

  if(a_ident[0]==4)
  {
    // is LAr
    if(a_ident[1]==1)
    {
      //is LAr EM
      try
      {
        id = m_larEmID->channel_id(a_ident[2],  // barrel_ec
                                   a_ident[3],  // sampling
                                   a_ident[4],  // region
                                   a_ident[5],  // eta
                                   a_ident[6]); // phi
      }
      catch (LArID_Exception& e)
      {
        (*m_log) << MSG::ERROR << "ConvertID: LArEM_ID error code " << e.code() << " "
		 << (std::string) e << endreq;
      }
    }
    else if(a_ident[1]==2)
    {
      //is EM HEC
      try
      {
        id = m_larHecID->channel_id(a_ident[2],  // zSide
                                    a_ident[3],  // sampling
                                    a_ident[4],  // region
                                    a_ident[5],  // eta
                                    a_ident[6]); // phi
      }
      catch(LArID_Exception& e)
      {
        (*m_log) << MSG::ERROR << "ConvertID: BuildHitCollections: LArHEC_ID error code " << e.code() << " "
		 << (std::string) e << endreq;
      }
    }
    else if(a_ident[1]==3)
    {
      // FCAL
      if(a_ident[3]>0)
      {	
	//is EM FCAL
	try
        {
	  id = m_larFcalID->channel_id(a_ident[2],  // zSide
				       a_ident[3],  // sampling
				       a_ident[4],  // eta
				       a_ident[5]); // phi
	}
	catch(LArID_Exception& e)
        {
	  (*m_log) << MSG::ERROR
		   << "ConvertID:: LArFCAL_ID error code " << e.code() << " "
		   << (std::string) e
		   << endreq;
	}
      }
      else
      {
	//is Mini FCAL
	try
        {
	  id = m_larMiniFcalID->channel_id(a_ident[2],  // zSide
					   a_ident[3],  // module
					   a_ident[4],  // depth
					   a_ident[5],  // eta
					   a_ident[6]); // phi
	}
	catch(LArID_Exception& e)
        {
	  (*m_log) << MSG::ERROR
		   << "ConvertID:: LArMiniFCAL_ID error code " << e.code() << " "
		   << (std::string) e
		   << endreq;
	}
      }
    }
  }
  return id;
}
