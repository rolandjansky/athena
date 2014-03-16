/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExL2muCalibTest/L2muCalibTest.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// static const AlgFactory<L2muCalibTest>    Factory;
// const IAlgFactory& L2muCalibTestFactory = Factory;

/////////////////////////////////////////////////////////////////////////////

L2muCalibTest::L2muCalibTest(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_jobOptionsSvc(0), m_StoreGateSvc(0),
  m_allowOksConfig(true), m_dumpCatalogue(false), 
  m_muonCalBufferName("NONE"), m_muonCalBufferSize(0)
{
  // Declare the properties
  declareProperty("AllowOksConfig",        m_allowOksConfig);
  declareProperty("DumpPropertyCatalogue", m_dumpCatalogue);
  declareProperty("MuonCalBufferName",     m_muonCalBufferName);
  declareProperty("MuonCalBufferSize",     m_muonCalBufferSize);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode L2muCalibTest::initialize(){

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  StatusCode sc;

#ifdef ATLAS_GAUDI_V21
  SmartIF<IService> tmp_msgSvc(msgSvc());
  if(tmp_msgSvc.isValid()) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name() << endreq;
  }
#else
  Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
  if(tmp_msgSvc != 0) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name() << endreq;
  }
#endif

  // Print out the property values after update from catalogue
  log << MSG::INFO << " ---------------------------------------------------------- " << endreq;
  log << MSG::INFO << " --- Properties before update from JobOptions Catalogue --- " << endreq;
  log << MSG::INFO << " ---------------------------------------------------------- " << endreq;
  log << MSG::INFO << " AllowOksConfig        = " << m_allowOksConfig << endreq;
  log << MSG::INFO << " DumpPropertyCatalogue = " << m_dumpCatalogue << endreq;
  log << MSG::INFO << " MuonCalBufferName     = " << m_muonCalBufferName << endreq;
  log << MSG::INFO << " MuonCalBufferSize     = " << m_muonCalBufferSize << endreq;

  // Part 2: Get the JobOptionsSvc service, and update properties from catalogue
  if ( m_allowOksConfig ) {
    sc = service("JobOptionsSvc", m_jobOptionsSvc);
    if (sc.isFailure()) {
      log << MSG::ERROR << "Could not find JobOptionsSvc" << endreq;
      return sc;
    } else {
      IService* svc = dynamic_cast<IService*>(m_jobOptionsSvc);
      if(svc != 0 ) {
	log << MSG::INFO << " Algorithm = " << name() << " is connected to JobOptionsSvc Service = "
	    << svc->name() << endreq;
      }

      if ( m_dumpCatalogue ) {
	// Dump of the catalogue
	log << MSG::INFO << "=================================================" << endreq;
	log << MSG::INFO << "Dump of the property catalogue.... " << endreq;
	std::vector<std::string> clients = m_jobOptionsSvc->getClients();
	std::vector<std::string>::iterator cit;
	std::vector<const Property*>::const_iterator pit;

	for( cit = clients.begin(); cit != clients.end(); cit++ ) {
	  log << MSG::INFO << " Properties of " <<  *cit << ": ";
	  const std::vector<const Property*>* properties = m_jobOptionsSvc->getProperties(*cit);
	  for( pit = properties->begin(); pit != properties->end(); pit++ ) {
	    log << (*pit)->name();
	    if( (pit+1) != properties->end())  log << ", ";
	  }
	  log << endreq;
	}
	log << MSG::INFO << "=================================================" << endreq;
      }

      // Overwrite the properties with values from the catalogue using the non existing algorithm
      // 'MuonL2CalibrationConfig'
      sc = m_jobOptionsSvc->setMyProperties("MuonL2CalibrationConfig",this);
      if (sc.isFailure()) {
	log << MSG::ERROR << "Could not set properties from JobOptionsSvc catalogue." << endreq;
	return sc;
      }

      // Find the Data Flow application name
      bool df_found = false;
      const std::vector<const Property*>* dataFlowProps = m_jobOptionsSvc->getProperties("DataFlowConfig");
      if ( dataFlowProps ) { 
	log << MSG::INFO << " Properties available for 'DataFlowConfig': number = " << dataFlowProps->size() << endreq;
	log << MSG::INFO << " --------------------------------------------------- " << endreq;
	for ( std::vector<const Property*>::const_iterator cur = dataFlowProps->begin();
	      cur != dataFlowProps->end(); cur++) {
	  log << MSG::INFO << (*cur)->name() << " = " << (*cur)->toString() << endreq;
	  // the application name is found
	  if ( (*cur)->name() == "DF_ApplicationName" ) {
	    df_found = true;
	  }
	  // the enabled ROB list is found
	  if ( (*cur)->name() == "DF_Enabled_ROB_IDs" ) {
	    SimpleProperty< std::vector<uint32_t> > enabledROBs;
	    enabledROBs.setName("enabledROBIds");
	    if (enabledROBs.assign(**cur)) {
	      log << MSG::INFO << " ---> got from OKS = " << enabledROBs.value().size() << " ROB ids." << endreq;
	    } else {
	      log << MSG::WARNING << " ---> set property failed." << endreq;
	    }
	  }
	}
  
	if ( !df_found ) {
	  log << MSG::INFO << " Property for DF_ApplicationName not found." << endreq;
	}
      } else {
	log << MSG::INFO << " No Properties for 'DataFlowConfig' found." << endreq;
      } 
      log << MSG::INFO << "=================================================" << endreq;


      // release JobOptionsSvc
      unsigned long mjcounter = m_jobOptionsSvc->release();
      log << MSG::INFO << " --> Release JobOptionsSvc Service, Counter = " << mjcounter << endreq;
    }
  }

  // Print out the property values after update from catalogue
  log << MSG::INFO << " ---------------------------------------------------------- " << endreq;
  log << MSG::INFO << " --- Properties after update from JobOptions Catalogue  --- " << endreq;
  log << MSG::INFO << " ---------------------------------------------------------- " << endreq;
  log << MSG::INFO << " AllowOksConfig        = " << m_allowOksConfig << endreq;
  log << MSG::INFO << " DumpPropertyCatalogue = " << m_dumpCatalogue << endreq;
  log << MSG::INFO << " MuonCalBufferName     = " << m_muonCalBufferName << endreq;
  log << MSG::INFO << " MuonCalBufferSize     = " << m_muonCalBufferSize << endreq;
  log << MSG::INFO << "=================================================" << endreq;

  // Part 3: Locate the StoreGateSvc
  sc = service("StoreGateSvc", m_StoreGateSvc);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  } else {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to StoreGate Service          = "
        << m_StoreGateSvc->name() << endreq;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode L2muCalibTest::execute() {

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;

  /*
  log << MSG::DEBUG << " StoreGate structure: Begin execute " << name() << "\n"
      << m_StoreGateSvc->dump() << endreq;


  log << MSG::DEBUG << " StoreGate structure: End execute " << name() << "\n"
      << m_StoreGateSvc->dump() << endreq;
  */

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode L2muCalibTest::finalize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  return StatusCode::SUCCESS;
}
