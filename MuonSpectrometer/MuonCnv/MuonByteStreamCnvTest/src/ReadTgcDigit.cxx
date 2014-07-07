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

#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonByteStreamCnvTest/ReadTgcDigit.h"

const int maxColl = 1600;
const int maxDig  = 4096;

/////////////////////////////////////////////////////////////////////////////

ReadTgcDigit::ReadTgcDigit(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator), 
    m_ntuplePtr(0), 
    m_activeStore(0), 
    m_tgcIdHelper(0), 
    m_log(0),
    m_debug(false), 
    m_verbose(false)
{
  // Declare the properties
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteTgcNtuple", m_tgcNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcDigit::initialize()
{
  m_log = new MsgStream(msgSvc(), name());
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;
  if(m_debug) *m_log << MSG::DEBUG << " in initialize()" << endreq;

  // Store Gate active store
  StatusCode sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "Cannot get ActiveStoreSvc" << endreq;
      return sc ;
    }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "Can't locate the DetectorStore" << endreq;
      return sc;
    }

  // get TGC ID helper
  sc = detStore->retrieve( m_tgcIdHelper, "TGCIDHELPER");
  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "Could not get TgcIdHelper !" << endreq;
      return sc;
    }

  if (!m_tgcNtuple) return StatusCode::SUCCESS;

  sc = accessNtuple();

  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "accessNtuple has failed !" << endreq;
      return StatusCode::FAILURE;
    }

  // add items
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCncoll",   m_nColl, 0, maxColl);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCndig",    m_nDig,  0, maxDig);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCstation", m_nDig,  m_stationName);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCeta",     m_nDig,  m_stationEta);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCphi",     m_nDig,  m_stationPhi);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCgasgap",  m_nDig,  m_gasGap);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCisstrip", m_nDig,  m_isStrip);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCchannel", m_nDig,  m_channel);
  sc = m_ntuplePtr -> addItem ("tgcdig/TGCbctag",   m_nDig,  m_bcTag);
  
  if (sc.isFailure())
    {
      *m_log << MSG::FATAL 
	     << "Could not add items to column wise ntuple" << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcDigit::execute()
{
  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  // get TGC digit container
  const std::string key = "TGC_DIGITS";
  const DataHandle<TgcDigitContainer> tgc_container;
  StatusCode sc = (*m_activeStore)->retrieve(tgc_container, key);
  if (sc.isFailure())
    {
      *m_log << MSG::FATAL << "Cannot retrieve TGC Digit Container" << endreq;
      return sc;
    }
 
  if ( m_debug ) *m_log << MSG::DEBUG <<"****** tgc->size() : " << tgc_container->size()<<endreq;

  if (!m_tgcNtuple) return StatusCode::SUCCESS; 

  m_nColl = 0;
  m_nDig = 0;

  // loop over collections
  for (TgcDigitContainer::const_iterator c = tgc_container->begin(); 
       c != tgc_container->end(); ++c)
    {
      if ( (*c)->size() == 0 ) continue;

      // loop over digits
      std::vector<Identifier> vID;
      for (TgcDigitCollection::const_iterator dig = (*c)->begin(); 
	   dig != (*c)->end(); ++dig) 
	{
	  Identifier id = (*dig)->identify();
          uint16_t bctag  = (*dig)->bcTag();

	  // ignore duplicated hits
	  bool dupl=false;
	  std::vector<Identifier>::const_iterator it_vID  = vID.begin();	  
	  std::vector<Identifier>::const_iterator it_vIDE = vID.end();
	  for (; it_vID != it_vIDE; ++it_vID)
	    {
	      if ((*it_vID)==id)
		{ 
		  dupl=true;
		  break;
		}
	    }

	  if (dupl) continue;
	  
	  vID.push_back(id);
	  
	  if ( m_debug ) *m_log << MSG::DEBUG << "Digit number " << m_nDig << endreq;

	  // ID information
	  m_stationName[m_nDig] = m_tgcIdHelper->stationName(id);
	  m_stationEta [m_nDig] = m_tgcIdHelper->stationEta(id);
	  m_stationPhi [m_nDig] = m_tgcIdHelper->stationPhi(id);
	  m_gasGap     [m_nDig] = m_tgcIdHelper->gasGap(id);
	  m_isStrip    [m_nDig] = m_tgcIdHelper->isStrip(id);
	  m_channel    [m_nDig] = m_tgcIdHelper->channel(id); 
	  m_bcTag      [m_nDig] = bctag;

	  if ( m_debug ) *m_log << MSG::DEBUG
	                        << MSG::hex
	                        << " N_" << m_tgcIdHelper->stationName(id)
	                        << " E_" << m_tgcIdHelper->stationEta(id)
	                        << " P_" << m_tgcIdHelper->stationPhi(id)
	                        << " G_" << m_tgcIdHelper->gasGap(id)
	                        << " C_" << m_tgcIdHelper->channel(id)
	                        << endreq;

	  ++m_nDig;
	}
      ++m_nColl;
  }

 if ( m_debug ) *m_log << MSG::DEBUG << "execute() completed" 
                       << endreq;
 
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcDigit::finalize()
{
  if ( m_debug ) *m_log << MSG::DEBUG << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode ReadTgcDigit::accessNtuple()
{

  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID ;

  //try to access it  
  NTuplePtr nt(ntupleService(), m_NtupleLocID );

  if (static_cast<int>(nt))
    {
      m_ntuplePtr=nt;
      if ( m_debug ) *m_log << MSG::INFO << "Ntuple " << m_NtupleLocID 
	                    << " reaccessed! " << endreq;
    } 
  else
    {
      if ( m_debug ) *m_log << MSG::FATAL << "Cannot reaccess " << m_NtupleLocID << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}


