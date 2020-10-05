/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/ReadRpcDigit.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonIdHelpers/RpcIdHelper.h"

static const int maxColl = 600;
static const int maxDig  = 2000;

// Algorithm constructor
ReadRpcDigit::ReadRpcDigit(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_activeStore("ActiveStoreSvc", name),
    m_ntuplePtr(nullptr)
{
  declareProperty("NtupleLocID", m_NtupleLocID);  
  declareProperty("WriteRpcNtuple", m_rpcNtuple = false);
}

StatusCode ReadRpcDigit::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_activeStore.retrieve() );
  ATH_CHECK( m_idHelperSvc.retrieve() );

  if (!m_rpcNtuple) return StatusCode::SUCCESS;  

  ATH_CHECK( accessNtuple() );
  
  if (!m_rpcNtuple) return StatusCode::SUCCESS;

  // Digits block
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/nrpccoll",   m_nColl,  0, maxColl) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/nrpcdig",    m_nDig,   0, maxDig) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rtime",      m_nDig,   m_time) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rstation",   m_nDig,   m_station) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/reta",       m_nDig,   m_eta) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rphi",       m_nDig,   m_phi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rdoublr",    m_nDig,   m_doubletR) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rdoublz",    m_nDig,   m_doubletZ) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rdoublphi",  m_nDig,   m_doubletPhi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rgasgap",    m_nDig,   m_gasGap) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rmeasphi",   m_nDig,   m_measuresPhi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("rpcdig/rstrip",     m_nDig,   m_strip) );

  return StatusCode::SUCCESS;  
}


// Execute method

StatusCode ReadRpcDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );

  std::string key = "RPC_DIGITS";

  const RpcDigitContainer* rpc_container = nullptr;
  ATH_CHECK( (*m_activeStore)->retrieve(rpc_container, key) );
  
  ATH_MSG_DEBUG("****** rpc->size() : " << rpc_container->size() );

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
	  if ( m_idHelperSvc->rpcIdHelper().valid(dig_id) ) 
	    {
	      
	      m_time[m_nDig]         = (*dig)->time();
	      m_station[m_nDig]      = m_idHelperSvc->rpcIdHelper().stationName(dig_id);
	      m_eta[m_nDig]          = m_idHelperSvc->rpcIdHelper().stationEta(dig_id);
	      m_phi[m_nDig]          = m_idHelperSvc->rpcIdHelper().stationPhi(dig_id);
	      m_doubletR[m_nDig]     = m_idHelperSvc->rpcIdHelper().doubletR(dig_id);
	      m_doubletZ[m_nDig]     = m_idHelperSvc->rpcIdHelper().doubletZ(dig_id);
	      m_doubletPhi[m_nDig]   = m_idHelperSvc->rpcIdHelper().doubletPhi(dig_id);
	      m_gasGap[m_nDig]       = m_idHelperSvc->rpcIdHelper().gasGap(dig_id); 
	      m_measuresPhi[m_nDig]  = m_idHelperSvc->rpcIdHelper().measuresPhi(dig_id);
	      m_strip[m_nDig]        = m_idHelperSvc->rpcIdHelper().strip(dig_id);
	    }

	  ++m_nDig;

	  if (m_nDig > maxDig-1) {
             ATH_MSG_WARNING( "Maximum number of RpcDigit in the ntuple reached: " 
                              << maxDig  );
	     return StatusCode::SUCCESS;
	  }
	  
	}

      ++m_nColl;
      ATH_MSG_DEBUG( " Collection number  " << m_nColl );

      if (m_nColl > maxColl-1) {
	ATH_MSG_WARNING( "Maximum number of RpcDigitCollection in the ntuple reached: " 
                         << maxColl  );
	return StatusCode::SUCCESS;
      }
    }
  }

  return StatusCode::SUCCESS;  
}

StatusCode ReadRpcDigit::accessNtuple() 
{
  //try to access it  
  NTuplePtr nt(ntupleSvc(),"/NTUPLES"+m_NtupleLocID);
  
  if (nt) 
    {
      m_ntuplePtr=nt;
      ATH_MSG_INFO( "Ntuple " <<m_NtupleLocID 
                    << " reaccessed! "  );
      return StatusCode::SUCCESS;
    } 
  else
    {
      ATH_MSG_FATAL( "Cannot reaccess " << m_NtupleLocID  );
      return StatusCode::FAILURE;
    }
}

