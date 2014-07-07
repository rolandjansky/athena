/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/ReadRpcDigit.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartDataLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonIdHelpers/RpcIdHelper.h"

static const int maxColl = 600;
static const int maxDig  = 2000;

// Algorithm constructor
ReadRpcDigit::ReadRpcDigit(const std::string &name, ISvcLocator *pSvcLocator)
  : Algorithm(name, pSvcLocator)
{
  declareProperty("NtupleLocID", m_NtupleLocID);  
  declareProperty("WriteRpcNtuple", m_rpcNtuple = false);
}

ReadRpcDigit::~ReadRpcDigit() 
{ }

// Initialize method

StatusCode ReadRpcDigit::initialize()
{
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

  // Event store service
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc.isFailure()) 
    {
      *m_log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return  sc;
    }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure())   {
    *m_log << MSG::ERROR << "Can't locate the DetectorStore" << endreq; 
    return sc;
  }
  sc = detStore->retrieve(m_rpcIdHelper, "RPCIDHELPER");
  if (sc.isSuccess())
    {
      *m_log << MSG::DEBUG << "Retrieved RpdIdHelper..." << endreq;
   }
  else
    *m_log << MSG::ERROR << "Cannot retrieve RpcIdHelper!" << endreq;

  if (!m_rpcNtuple) return StatusCode::SUCCESS;  

  if ( accessNtuple() != StatusCode::SUCCESS || ! m_ntuplePtr ) {
    *m_log << MSG::ERROR << "accessNtuple has failed !" << endreq;
    return StatusCode::FAILURE;
  }
  
  if (!m_rpcNtuple) return StatusCode::SUCCESS;

  // Digits block
  sc = m_ntuplePtr -> addItem ("rpcdig/nrpccoll",   m_nColl,  0, maxColl);
  sc = m_ntuplePtr -> addItem ("rpcdig/nrpcdig",    m_nDig,   0, maxDig);
  sc = m_ntuplePtr -> addItem ("rpcdig/rtime",      m_nDig,   m_time);
  sc = m_ntuplePtr -> addItem ("rpcdig/rstation",   m_nDig,   m_station);
  sc = m_ntuplePtr -> addItem ("rpcdig/reta",       m_nDig,   m_eta);
  sc = m_ntuplePtr -> addItem ("rpcdig/rphi",       m_nDig,   m_phi);
  sc = m_ntuplePtr -> addItem ("rpcdig/rdoublr",    m_nDig,   m_doubletR);
  sc = m_ntuplePtr -> addItem ("rpcdig/rdoublz",    m_nDig,   m_doubletZ);
  sc = m_ntuplePtr -> addItem ("rpcdig/rdoublphi",  m_nDig,   m_doubletPhi);
  sc = m_ntuplePtr -> addItem ("rpcdig/rgasgap",    m_nDig,   m_gasGap);
  sc = m_ntuplePtr -> addItem ("rpcdig/rmeasphi",   m_nDig,   m_measuresPhi);
  sc = m_ntuplePtr -> addItem ("rpcdig/rstrip",     m_nDig,   m_strip);

  if (sc.isFailure())
    {
      *m_log << MSG::ERROR 
	     << "Could not add items to column wise ntuple" << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;  
}


// Execute method

StatusCode ReadRpcDigit::execute()
{
  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  std::string key = "RPC_DIGITS";

  const DataHandle<RpcDigitContainer> rpc_container;
  StatusCode sc_read = (*m_activeStore)->retrieve(rpc_container, key);
  if (sc_read.isFailure()) {
    *m_log << MSG::ERROR << " Cannot retrieve RPC Digit Container " << endreq;
    return sc_read;
  }
  
 if ( m_debug ) *m_log << MSG::DEBUG <<"****** rpc->size() : " << rpc_container->size()<<endreq;

  if (!m_rpcNtuple) return StatusCode::SUCCESS;

  m_nColl = 0;
  m_nDig = 0;

  // Access by Collection
  for (RpcDigitContainer::const_iterator c = rpc_container->begin(); 
       c != rpc_container->end(); ++c) {

    //Identifier moduleId = (*c)->identify();

    if ( (*c)->size()>0 ) {
      for (RpcDigitCollection::const_iterator dig = (*c)->begin(); 
	   dig != (*c)->end(); ++dig) 
	{
	  Identifier dig_id = (*dig)->identify();
	  if ( m_rpcIdHelper->valid(dig_id) ) 
	    {
	      
	      m_time[m_nDig]         = (*dig)->time();
	      m_station[m_nDig]      = m_rpcIdHelper->stationName(dig_id);
	      m_eta[m_nDig]          = m_rpcIdHelper->stationEta(dig_id);
	      m_phi[m_nDig]          = m_rpcIdHelper->stationPhi(dig_id);
	      m_doubletR[m_nDig]     = m_rpcIdHelper->doubletR(dig_id);
	      m_doubletZ[m_nDig]     = m_rpcIdHelper->doubletZ(dig_id);
	      m_doubletPhi[m_nDig]   = m_rpcIdHelper->doubletPhi(dig_id);
	      m_gasGap[m_nDig]       = m_rpcIdHelper->gasGap(dig_id); 
	      m_measuresPhi[m_nDig]  = m_rpcIdHelper->measuresPhi(dig_id);
	      m_strip[m_nDig]        = m_rpcIdHelper->strip(dig_id);
	    }

	  ++m_nDig;

	  if (m_nDig > maxDig-1) {
	     if ( m_debug ) *m_log << MSG::WARNING << "Maximum number of RpcDigit in the ntuple reached: " 
	                           << maxDig << endreq;
	     return StatusCode::SUCCESS;
	  }
	  
	}

      ++m_nColl;
      if ( m_debug ) *m_log << MSG::DEBUG << " Collection number  " << m_nColl<<endreq;

      if (m_nColl > maxColl-1) {
	*m_log << MSG::WARNING << "Maximum number of RpcDigitCollection in the ntuple reached: " 
	       << maxColl << endreq;
	return StatusCode::SUCCESS;
      }
      
      
    }
  }

  return StatusCode::SUCCESS;  
}


// Finalize method

StatusCode ReadRpcDigit::finalize()
{
  delete m_log;
  return StatusCode::SUCCESS;
}


StatusCode ReadRpcDigit::accessNtuple() 
{
  //try to access it  
  NTuplePtr nt(ntupleService(),"/NTUPLES"+m_NtupleLocID);
  
  if (nt) 
    {
      m_ntuplePtr=nt;
      *m_log << MSG::INFO << "Ntuple " <<m_NtupleLocID 
	  << " reaccessed! " << endreq;
      return StatusCode::SUCCESS;
    } 
  else
    {
      *m_log << MSG::FATAL << "Cannot reaccess " << m_NtupleLocID << endreq;
      return StatusCode::FAILURE;
    }
}

