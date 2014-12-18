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

#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "MuonByteStreamCnvTest/ReadMdtRDO.h"

using namespace std;


static const int maxCsm =   1200;//?????
static const int maxAmt =   5000;//?????

/////////////////////////////////////////////////////////////////////////////

ReadMdtRDO::ReadMdtRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_ntuplePtr(0), m_activeStore(0), m_log(0),
  m_debug(false), m_verbose(false) {
  
  // Declare the properties

  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteMdtNtuple",m_mdtNtuple = false);
  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtRDO::initialize(){

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

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  if ( accessNtuple() != StatusCode::SUCCESS || ! m_ntuplePtr ) {
    *m_log << MSG::ERROR << "accessNtuple has failed !" << endreq;
    return StatusCode::FAILURE;
  }


  sc = m_ntuplePtr -> addItem ("mdtrod/ncsm",   m_nCsm,          0, maxCsm);
  sc = m_ntuplePtr -> addItem ("mdtrod/namt",   m_nAmt,           0, maxAmt);
  sc = m_ntuplePtr -> addItem ("mdtrod/subid",  m_nCsm,          m_subId);
  sc = m_ntuplePtr -> addItem ("mdtrod/rodid",  m_nCsm,          m_rodId);
  sc = m_ntuplePtr -> addItem ("mdtrod/csmid",  m_nCsm,         m_csmId);
  sc = m_ntuplePtr -> addItem ("mdtrod/tdcid",  m_nAmt,          m_tdcId);
  sc = m_ntuplePtr -> addItem ("mdtrod/chanid",  m_nAmt,          m_channelId);
  sc = m_ntuplePtr -> addItem ("mdtrod/fine",    m_nAmt,          m_fine);
  sc = m_ntuplePtr -> addItem ("mdtrod/coarse",  m_nAmt,          m_coarse);
  sc = m_ntuplePtr -> addItem ("mdtrod/width",  m_nAmt,          m_width);


  if ( sc == StatusCode::FAILURE ) {
       
    *m_log << MSG::ERROR 
           << "Could not add items to column wise ntuple" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtRDO::execute() {

  if ( m_debug ) *m_log << MSG::DEBUG << "in execute()" << endreq;

  const DataHandle<MdtCsmContainer> MdtRDO; 
  StatusCode sc_read = (*m_activeStore)->retrieve( MdtRDO, "MDTCSM" );

  if ( sc_read != SUCCESS ) {
    *m_log << MSG::FATAL << "Could not find event" << endreq;
    return StatusCode::FAILURE;
  }

 // Access by Collection 

 if ( m_debug ) *m_log << MSG::DEBUG <<"****** MdtRDO->size() : " << MdtRDO->size()<<endreq;

 if (!m_mdtNtuple) return StatusCode::SUCCESS;

 m_nCsm = 0;
 m_nAmt = 0;

 MdtCsmContainer::const_iterator it1 = MdtRDO->begin(); 
 MdtCsmContainer::const_iterator it2 = MdtRDO->end();

 for (; it1!=it2; ++it1){

   // loop over pads
   if ( (*it1)->size() > 0 ) {

     const MdtCsm* csm = (*it1);

     m_subId[m_nCsm]    = (long) csm->SubDetId();
     m_csmId[m_nCsm]    = (long) csm->CsmId();
     m_rodId[m_nCsm]    = (long) csm->MrodId();
     
     if ( m_debug ) {      
        *m_log << MSG::DEBUG << "ncsm : " << m_nCsm << " Subdet : " << csm->SubDetId()
	       << " Mrodid : " << csm->MrodId()
	       << " Csmid : " <<  csm->CsmId() << endreq;
        *m_log << MSG::DEBUG << "Subdet ntuple: " << m_subId[m_nCsm] 
	       << " Mrodid ntuple: " << m_rodId[m_nCsm]
	       << " Csmid ntuple: " << m_csmId[m_nCsm] << endreq;
        *m_log << MSG::DEBUG << " Number of Amt in this Csm " << (*it1)->size() << endreq;
     }
 
     // for each cms, loop over amt
     MdtCsm::const_iterator it3 = (*it1)->begin(); 
     MdtCsm::const_iterator it4 = (*it1)->end();
     for (; it3!=it4; ++it3) { 
       const MdtAmtHit * amt = (*it3);
       m_tdcId[m_nAmt] = (long)amt->tdcId();
       m_channelId[m_nAmt] = (long)amt->channelId();
       m_fine[m_nAmt] = (long)amt->fine();
       m_coarse[m_nAmt] = (long)amt->coarse();
       m_width[m_nAmt]= (long)amt->width();
       m_nAmt++;

       if (m_nAmt > maxAmt-1) {
	 *m_log << MSG::WARNING << "Maximum number of AmtHit in the ntuple reached: " 
	        << maxAmt << endreq;
	 return StatusCode::SUCCESS;
       }

     }
     m_nCsm++;
     if ( m_nCsm > maxCsm-1 ) {
       *m_log << MSG::WARNING << "Maximum number of MdtCsm in the ntuple reached: " 
	      << maxCsm << endreq;
       return StatusCode::SUCCESS;
     }
   }
 }
 if ( m_debug ) {
   *m_log << MSG::DEBUG << " done collecting histograms" << endreq;
 
   *m_log << MSG::DEBUG
          << "ReadMdtRDO::execute reports success" 
          << endreq;
 }
 return StatusCode::SUCCESS;
 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtRDO::finalize() {

  *m_log << MSG::INFO << "in finalize()" << endreq;
  delete m_log;

  return StatusCode::SUCCESS;
}


StatusCode ReadMdtRDO::accessNtuple() {

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


