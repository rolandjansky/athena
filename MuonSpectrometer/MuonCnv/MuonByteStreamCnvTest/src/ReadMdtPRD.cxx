/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

Fill an ntuple with MdtPrepData

S. Rosati Dec 2005

*/

//#include <strstream>
//#include <cassert>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGate.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/INTupleSvc.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonByteStreamCnvTest/ReadMdtPRD.h"

using namespace std;

static const int maxColl =   1200;
static const int maxPrd =    5000;

/////////////////////////////////////////////////////////////////////////////

ReadMdtPRD::ReadMdtPRD(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  // Declare the properties

  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteMdtNtuple", m_mdtNtuple = true);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtPRD::initialize(){

  m_log = new MsgStream(msgSvc(), name());
  *m_log << MSG::DEBUG << " in initialize()" << endreq;
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

  StatusCode sc;

  // Store Gate active store
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc != StatusCode::SUCCESS ) {
    *m_log << MSG::ERROR << " Cannot get StoreGateSvc " << endreq;
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    *m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  }   
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()) {
    *m_log << MSG::FATAL << "Cannot get MdtIdHelper" << endreq;
    return StatusCode::FAILURE;
  }  
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the MdtIdHelper. " << endreq;
  }
  

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  if ( accessNtuple() != StatusCode::SUCCESS || ! m_ntuplePtr ) {
    *m_log << MSG::ERROR << "accessNtuple has failed !" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_ntuplePtr -> addItem ("mdtprd/nprdcoll",   m_nColl,       0, maxColl);
  sc = m_ntuplePtr -> addItem ("mdtprd/nprd",    m_nPrd,        0, maxPrd);
  sc = m_ntuplePtr -> addItem ("mdtprd/tdc",     m_nPrd,        m_tdc);
  sc = m_ntuplePtr -> addItem ("mdtprd/adc",     m_nPrd,        m_adc);
  sc = m_ntuplePtr -> addItem ("mdtprd/imulti",  m_nPrd,   m_multi);
  sc = m_ntuplePtr -> addItem ("mdtprd/ilayer",  m_nPrd,   m_layer);
  sc = m_ntuplePtr -> addItem ("mdtprd/iwire",   m_nPrd,   m_wire);
  
  if ( sc == StatusCode::FAILURE ) {
       
    *m_log << MSG::ERROR 
           << "Could not add items to column wise ntuple" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtPRD::execute() {

  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  std::string	key = "MDT_PREPDATA_NEW";
  const Muon::MdtPrepDataContainer* mdt_container;
  StatusCode sc_read = m_eventStore->retrieve(mdt_container, key);
  if (sc_read.isFailure()) {
    *m_log << MSG::ERROR << " Cannot retrieve MdtPrepDataContainer " << endreq;
    return sc_read;
  }
 
  if ( m_debug ) *m_log << MSG::DEBUG <<"****** mdt->size() : " << mdt_container->size()<<endreq;

  const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
  const DataHandle<Muon::MdtPrepDataCollection> lastColl;

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  m_nColl = 0;
  m_nPrd = 0;
  //Identifier ch_id;
  Identifier dig_id;

  // Access by Collection
  if (m_eventStore->retrieve(mdtCollection,lastColl) ==StatusCode::SUCCESS) {

    for ( ; mdtCollection != lastColl ; ++mdtCollection ) {
      
      //ch_id = mdtCollection->identify();
      
      if (mdtCollection->size()>0) {
	
	for (Muon::MdtPrepDataCollection::const_iterator mdtPrd = mdtCollection->begin(); 
	     mdtPrd != mdtCollection->end(); ++mdtPrd) {
	  
	  dig_id = (*mdtPrd)->identify();
	  
	  m_tdc[m_nPrd] = (*mdtPrd)->tdc();
	  m_adc[m_nPrd] = (*mdtPrd)->adc();
	  
	  m_multi[m_nPrd] = m_mdtIdHelper->multilayer(dig_id); 
	  m_layer[m_nPrd] = m_mdtIdHelper->tubeLayer(dig_id);
	  m_wire[m_nPrd]  = m_mdtIdHelper->tube(dig_id); 
	  ++m_nPrd;
	  if ( m_debug ) *m_log << MSG::DEBUG << " MdtPrepData number:  " << m_nPrd<<endreq;
	  
	  if (m_nPrd > maxPrd-1) {
	    *m_log << MSG::WARNING << "Maximum number of MdtPrepData in the ntuple reached: " 
		   << maxPrd << endreq;
	    return StatusCode::SUCCESS;
	  }
	  
	  
	}
	++m_nColl;
	
	if ( m_debug ) *m_log << MSG::DEBUG << " Collection number  " << m_nColl<<endreq;
	if (m_nColl > maxColl-1) {
	  *m_log << MSG::WARNING << "Maximum number of MdtPrepDataCollection in the ntuple reached: " 
	         << maxColl << endreq;
	  return StatusCode::SUCCESS;
	}
	
      }
    }
  }
 
 if ( m_debug ) { 
    *m_log << MSG::DEBUG << " done collecting histograms" << endreq;
 
    *m_log << MSG::DEBUG
           << "ReadMdtPRD::execute reports success" 
           << endreq;
 }
 return StatusCode::SUCCESS;
 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtPRD::finalize() {

  *m_log << MSG::INFO << "in finalize()" << endreq;
  delete m_log;

  return StatusCode::SUCCESS;
}


StatusCode ReadMdtPRD::accessNtuple() {

  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID ;

  //try to access it  
  NTuplePtr nt(ntupleService(), m_NtupleLocID );

  if ((int) nt)     {
     m_ntuplePtr=nt;
     *m_log << MSG::INFO << "Ntuple " << m_NtupleLocID << " reaccessed! " << endreq;
  } 
  else {
     *m_log << MSG::FATAL << "Cannot reaccess " << m_NtupleLocID << endreq;
     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}


