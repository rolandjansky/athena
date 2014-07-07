/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Test byte stream converters, write to and read from byte stream ...
  Ketevi A. Assamagan, CERN Jan-28-2003
  Adapted from H. Ma's test example!

  Re-adapted by Michela Biglietti for Mdt Rods
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

#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonByteStreamCnvTest/ReadMdtDigit.h"

using namespace std;


static const int maxColl =   1200;
static const int maxDig =    5000;

/////////////////////////////////////////////////////////////////////////////

ReadMdtDigit::ReadMdtDigit(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  // Declare the properties

  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteMdtNtuple", m_mdtNtuple = false);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtDigit::initialize(){

  m_log = new MsgStream(msgSvc(), name());
  *m_log << MSG::DEBUG << " in initialize()" << endreq;
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

  StatusCode sc;

  // Store Gate active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    *m_log << MSG::ERROR << " Cannot get ActiveStoreSvc " << endreq;
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
    if ( m_debug ) *m_log << MSG::ERROR << "accessNtuple has failed !" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_ntuplePtr -> addItem ("mdtdig/ncoll",   m_nColl,       0, maxColl);
  sc = m_ntuplePtr -> addItem ("mdtdig/ndig",    m_nDig,        0, maxDig);
  sc = m_ntuplePtr -> addItem ("mdtdig/tdc",     m_nDig,        m_tdc);
  sc = m_ntuplePtr -> addItem ("mdtdig/adc",     m_nDig,        m_adc);
  sc = m_ntuplePtr -> addItem ("mdtdig/imulti",  m_nDig,   m_multi);
  sc = m_ntuplePtr -> addItem ("mdtdig/ilayer",  m_nDig,   m_layer);
  sc = m_ntuplePtr -> addItem ("mdtdig/iwire",   m_nDig,   m_wire);
  
  if ( sc == StatusCode::FAILURE ) {
       
    *m_log << MSG::ERROR 
           << "Could not add items to column wise ntuple" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtDigit::execute() {

  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  std::string	key = "MDT_DIGITS";
  const DataHandle<MdtDigitContainer> mdt_container;
  StatusCode sc_read = (*m_activeStore)->retrieve(mdt_container, key);
  if (sc_read.isFailure()) {
    *m_log << MSG::ERROR << " Cannot retrieve MDT Digit Container " << endreq;
    return sc_read;
  }
 
  if ( m_debug ) *m_log << MSG::DEBUG <<"****** mdt->size() : " << mdt_container->size()<<endreq;

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  m_nColl = 0;
  m_nDig = 0;
  IdentifierHash ch_id;
  Identifier dig_id;

  // Access by Collection
  for (MdtDigitContainer::const_iterator c = mdt_container->begin(); 
       c != mdt_container->end(); ++c) {

    ch_id = (*c)->identifierHash();

    if ((*c)->size()>0) {
     
      for (MdtDigitCollection::const_iterator dig = (*c)->begin(); 
	   dig != (*c)->end(); ++dig) {

	dig_id = (*dig)->identify();

	m_tdc[m_nDig] = (*dig)->tdc();
	m_adc[m_nDig] = (*dig)->adc();

	m_multi[m_nDig] = m_mdtIdHelper->multilayer(dig_id); 
	m_layer[m_nDig] = m_mdtIdHelper->tubeLayer(dig_id);
	m_wire[m_nDig]  = m_mdtIdHelper->tube(dig_id); 
	++m_nDig;
	if ( m_debug ) *m_log << MSG::DEBUG << " Digit number  " << m_nDig<<endreq;

	if (m_nDig > maxDig-1) {
	  *m_log << MSG::WARNING << "Maximum number of MdtDigit in the ntuple reached: " 
	         << maxDig << endreq;
	  return StatusCode::SUCCESS;
	}
	

      }
      ++m_nColl;

      if ( m_debug ) *m_log << MSG::DEBUG << " Collection number  " << m_nColl<<endreq;
      if (m_nColl > maxColl-1) {
	*m_log << MSG::WARNING << "Maximum number of MdtDigitCollection in the ntuple reached: " 
	       << maxColl << endreq;
	return StatusCode::SUCCESS;
      }

    }
  }
 
 if ( m_debug ) { 
   *m_log << MSG::DEBUG << " done collecting histograms" << endreq;
 
   *m_log << MSG::DEBUG
          << "ReadMdtDigit::execute reports success" 
          << endreq;
 }  
 return StatusCode::SUCCESS;
 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtDigit::finalize() {

  *m_log << MSG::INFO << "in finalize()" << endreq;
  delete m_log;

  return StatusCode::SUCCESS;
}


StatusCode ReadMdtDigit::accessNtuple() {


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


