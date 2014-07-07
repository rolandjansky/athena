/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGate.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonDigitContainer/CscDigitCollection.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonByteStreamCnvTest/ReadCscDigit.h"

const int MAXCOLL = 32;
const int MAXDIGIT = 4096;

/////////////////////////////////////////////////////////////////////////////

ReadCscDigit::ReadCscDigit(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator)
{
  // Declare the properties
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteCscNtuple", m_cscNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadCscDigit::initialize()
{
  m_log = new MsgStream(msgSvc(), name());
  *m_log << MSG::DEBUG << " in initialize()" << endreq;
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;

  StatusCode sc;

  // Store Gate active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc.isFailure())
    {
      *m_log << MSG::ERROR << "Cannot get ActiveStoreSvc" << endreq;
      return sc ;
    }

  // initialize the detectore store service and the CSC Id Helper
  StoreGateSvc* detStore=0;
  sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    *m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore->retrieve(m_cscHelper, "CSCIDHELPER");
  if (sc.isFailure()) {
    *m_log << MSG::FATAL << "Could not get CscIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the CscIdHelper. " << endreq;
  }

  if (!m_cscNtuple) return StatusCode::SUCCESS;

  sc = accessNtuple();
  if (sc.isFailure())
    {
      *m_log << MSG::ERROR << "accessNtuple has failed !" << endreq;
      return StatusCode::FAILURE;
    }

  // add items
  sc = m_ntuplePtr -> addItem ("cscdig/ncollections",   m_nColl, 0, MAXCOLL);
  sc = m_ntuplePtr -> addItem ("cscdig/ndigits",        m_nDig,  0, MAXDIGIT);
  sc = m_ntuplePtr -> addItem ("cscdig/stationName", m_nDig,  m_stationName);
  sc = m_ntuplePtr -> addItem ("cscdig/stationEta",  m_nDig,  m_stationEta);
  sc = m_ntuplePtr -> addItem ("cscdig/stationPhi",  m_nDig,  m_stationPhi);
  sc = m_ntuplePtr -> addItem ("cscdig/chamberLayer",m_nDig,  m_chamberLayer);
  sc = m_ntuplePtr -> addItem ("cscdig/wireLayer",   m_nDig,  m_wireLayer);
  sc = m_ntuplePtr -> addItem ("cscdig/measuresPhi", m_nDig,  m_measuresPhi);
  sc = m_ntuplePtr -> addItem ("cscdig/strip",       m_nDig,  m_strip);
  sc = m_ntuplePtr -> addItem ("cscdig/charge",      m_nDig,  m_charge);
  
  if (sc.isFailure())
    {
      *m_log << MSG::ERROR 
	     << "Could not add items to column wise ntuple" << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadCscDigit::execute()
{
  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  // get CSC digit container
  const std::string key = "CSC_DIGITS";
  const DataHandle<CscDigitContainer> csc_container;
  StatusCode sc = (*m_activeStore)->retrieve(csc_container, key);
  if (sc.isFailure())
    {
      *m_log << MSG::ERROR << "Cannot retrieve CSC Digit Container" << endreq;
      return sc;
    }
 
  if ( m_debug ) *m_log << MSG::DEBUG <<"****** csc->size() : " << csc_container->size()<<endreq;

  if (!m_cscNtuple) return StatusCode::SUCCESS;

  m_nColl = 0;
  m_nDig = 0;

  // loop over collections
  for (CscDigitContainer::const_iterator c = csc_container->begin(); 
       c != csc_container->end(); ++c)
    {
      if ( (*c)->size() > 0 ) {
        if ( m_debug ) {
           *m_log << MSG::DEBUG << "*******************************************" << endreq;
           *m_log << MSG::DEBUG << "Collection hash id = " << (unsigned int) (*c)->identifierHash() 
	          << endreq;
        }
        // loop over digits
        for (CscDigitCollection::const_iterator dig = (*c)->begin(); 
	     dig != (*c)->end(); ++dig) 
	  {
	    if ( m_debug ) *m_log << MSG::DEBUG << "Digit number " << m_nDig << endreq;
	    Identifier id = (*dig)->identify();

	    // ID information
	    m_stationName[m_nDig]  = m_cscHelper->stationName(id);
	    m_stationEta [m_nDig]  = m_cscHelper->stationEta(id);
	    m_stationPhi [m_nDig]  = m_cscHelper->stationPhi(id);
	    m_chamberLayer[m_nDig] = m_cscHelper->chamberLayer(id);
	    m_wireLayer[m_nDig]    = m_cscHelper->wireLayer(id);
	    m_measuresPhi[m_nDig]  = m_cscHelper->measuresPhi(id);
	    m_strip[m_nDig]        = m_cscHelper->strip(id); 
	    m_charge[m_nDig]       = (*dig)->charge(); 
	    if ( m_debug ) *m_log << MSG::DEBUG << "Digit = " << m_cscHelper->show_to_string(id) << " charge = " 
		                  << m_charge[m_nDig] << endreq; 
	    ++m_nDig;
	  }
        ++m_nColl;
      }
  }
 if ( m_debug ) {
    *m_log << MSG::DEBUG << "Number of collections = " << m_nColl << endreq;
    *m_log << MSG::DEBUG << "Number of digits = " << m_nDig << endreq; 
    *m_log << MSG::DEBUG << "execute() completed" 
           << endreq;
 } 
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadCscDigit::finalize()
{
  if ( m_debug ) *m_log << MSG::DEBUG << "in finalize()" << endreq;
  delete m_log; 

  return StatusCode::SUCCESS;
}


StatusCode ReadCscDigit::accessNtuple()
{

  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID ;

  //try to access it  
  NTuplePtr nt(ntupleService(), m_NtupleLocID );

  if (static_cast<int>(nt))
    {
      m_ntuplePtr=nt;
      *m_log << MSG::INFO << "Ntuple " << m_NtupleLocID 
	     << " reaccessed! " << endreq;
    } 
  else
    {
      *m_log << MSG::FATAL << "Cannot reaccess " << m_NtupleLocID << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}


