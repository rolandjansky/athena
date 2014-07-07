/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Test byte stream converters, write to and read from byte stream ...

  @author Tadashi Maeno
     based on ReadRpcRDO by Ketevi A. Assamagan
*/

#include <cassert>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGate.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/INTupleSvc.h"

#include "MuonRDO/TgcRawData.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonByteStreamCnvTest/ReadTgcRDO.h"

/////////////////////////////////////////////////////////////////////////////

const int MAX_RDO = 24;
const int MAX_DATA = 1024;

ReadTgcRDO::ReadTgcRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_ntuplePtr(0), 
  m_activeStore(0), 
  m_hashFunc(0), 
  m_log(0), 
  m_debug(false), 
  m_verbose(false)
{
  // Declare the properties
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteTgcNtuple", m_tgcNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcRDO::initialize()
{
  m_log = new MsgStream(msgSvc(), name());
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;
  if(m_debug) *m_log << MSG::DEBUG << " in initialize()" << endreq;

  // Store Gate active store
  StatusCode sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS )
    {
      *m_log << (sc.isFailure() ? MSG::FATAL : MSG::ERROR) << " Cannot get ActiveStoreSvc " << endreq;
      return sc ;
    }

  if (!m_tgcNtuple) return StatusCode::SUCCESS;

  if ( accessNtuple() != StatusCode::SUCCESS || ! m_ntuplePtr )
    {
      *m_log << MSG::FATAL << "accessNtuple has failed !" << endreq;
      return StatusCode::FAILURE;
    }

  //  Add the following items to the ntuple:

  sc = m_ntuplePtr -> addItem ("tgcrod/nrdo",   m_nRdo,     0, MAX_RDO);
  sc = m_ntuplePtr -> addItem ("tgcrod/ndata",  m_nRawData, 0, MAX_DATA);
  sc = m_ntuplePtr -> addItem ("tgcrod/l1Id",   m_nRdo,        m_l1Id);
  sc = m_ntuplePtr -> addItem ("tgcrod/bcId",   m_nRdo,        m_bcId);
  sc = m_ntuplePtr -> addItem ("tgcrod/bcTag",  m_nRawData,    m_bcTag);
  sc = m_ntuplePtr -> addItem ("tgcrod/l1IdS",  m_nRawData,    m_l1IdSLB);
  sc = m_ntuplePtr -> addItem ("tgcrod/bcIdS",  m_nRawData,    m_bcIdSLB);
  sc = m_ntuplePtr -> addItem ("tgcrod/subID",  m_nRawData,    m_subId);
  sc = m_ntuplePtr -> addItem ("tgcrod/rodID",  m_nRawData,    m_rodId);
  sc = m_ntuplePtr -> addItem ("tgcrod/sswID",  m_nRawData,    m_sswId);
  sc = m_ntuplePtr -> addItem ("tgcrod/slbID",  m_nRawData,    m_slbId);
  sc = m_ntuplePtr -> addItem ("tgcrod/bitPos", m_nRawData,    m_bitPos);
  sc = m_ntuplePtr -> addItem ("tgcrod/isCoincidence",m_nRawData,m_isCoincidence);
  sc = m_ntuplePtr -> addItem ("tgcrod/type",m_nRawData,m_type);
  sc = m_ntuplePtr -> addItem ("tgcrod/index",m_nRawData,m_index);
  sc = m_ntuplePtr -> addItem ("tgcrod/pos",m_nRawData,m_pos);
  sc = m_ntuplePtr -> addItem ("tgcrod/delta",m_nRawData,m_delta);

  if ( sc == StatusCode::FAILURE )
    {
      *m_log << MSG::FATAL 
	     << "Could not add items to column wise ntuple" << endreq;
      return StatusCode::FAILURE;
    }

  m_hashFunc = new TgcRdoIdHash();
   
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcRDO::execute()
{
  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  const DataHandle<TgcRdoContainer> TgcRDO; 
  StatusCode sc_read = (*m_activeStore)->retrieve( TgcRDO, "TGCRDO" );

  if ( sc_read != SUCCESS )
    {
      *m_log << MSG::FATAL << "Could not find event" << endreq;
      return StatusCode::FAILURE;
    }

  if ( m_debug ) *m_log << MSG::DEBUG <<"****** TgcRDO->size() : " << TgcRDO->size()<<endreq;

  if (!m_tgcNtuple) return StatusCode::SUCCESS;

 // Access by Collection 

 m_nRdo = 0;
 m_nRawData = 0;

 // loop over Rdo
 TgcRdoContainer::const_iterator itR   = TgcRDO->begin(); 
 TgcRdoContainer::const_iterator itR_e = TgcRDO->end();
 for (; itR!=itR_e; ++itR)
   {
     const TgcRdo * rdo = *itR;
     assert (m_nRdo < MAX_RDO);

     if ( m_debug ) *m_log << MSG::DEBUG << " Number of RawData in this rdo " 
	                   << rdo->size() << endreq;

     if ((*itR)->size() == 0) continue;

     //m_l1Id  [m_nRdo] = (long) rdo->l1Id();
     //m_bcId  [m_nRdo] = (long) rdo->bcId();

     // for each Rdo, loop over RawData
     TgcRdo::const_iterator itD   = rdo->begin(); 
     TgcRdo::const_iterator itD_e = rdo->end();
     for (; itD!=itD_e; ++itD)
       { 
	 const TgcRawData * data = (*itD);
	 assert (m_nRawData < MAX_DATA);
	 
	 m_bcTag  [m_nRawData] = (long) data->bcTag();
	 //m_l1IdSLB[m_nRawData] = (long) data->l1Id();
	 //m_bcIdSLB[m_nRawData] = (long) data->bcId();
	 m_subId  [m_nRawData] = (long) data->subDetectorId();
	 m_rodId  [m_nRawData] = (long) data->rodId();
	 m_sswId  [m_nRawData] = (long) data->sswId();
	 m_slbId  [m_nRawData] = (long) data->slbId();
	 m_bitPos [m_nRawData] = (long) data->bitpos();
	 if (data->isCoincidence()) {
	   m_isCoincidence[m_nRawData] = 1;
	   m_type [m_nRawData] = (long) data->type();
	   m_index[m_nRawData] = (long) data->index();
	   m_pos  [m_nRawData] = (long) data->position();
	   m_delta[m_nRawData] = (long) data->delta();
	 }
	 else {
	   m_isCoincidence[m_nRawData] = 0;
	   m_type [m_nRawData] = 0;
	   m_index[m_nRawData] = 0;
	   m_pos  [m_nRawData] = 0;
	   m_delta[m_nRawData] = 0;
	 }

	 if ( m_debug ) *m_log << MSG::DEBUG << MSG::hex
	                       << " Sub : " << data->subDetectorId()
	                       << " ROD : " << data->rodId() 
	                       << " SSW : " << data->sswId()
	                       << " SLB : " << data->slbId()
	                       << " Ch  : " << data->bitpos()
	                       << endreq;

	 ++m_nRawData;
       }
     ++m_nRdo;
   }
 
 if ( m_debug ) *m_log << MSG::DEBUG << " done collecting histograms" << endreq;

 if ( m_debug ) *m_log << MSG::DEBUG << "ReadTgcRDO::execute reports success" << endreq;

 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcRDO::finalize()
{
  *m_log << MSG::INFO << "in finalize()" << endreq;
  delete m_log;

  if (m_hashFunc) delete m_hashFunc;

  return StatusCode::SUCCESS;
}


StatusCode ReadTgcRDO::accessNtuple()
{
  MsgStream log(messageService(), name());

  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID;

  //try to access it  
  NTuplePtr nt(ntupleService(), m_NtupleLocID );

  if ((int) nt)
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


