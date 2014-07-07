/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, January 24 2004

// algorithm to decode RDO into digits

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "MuonMDT_CnvTools/IMDT_RDO_Decoder.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonTGC_CnvTools/ITGC_RDO_Decoder.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

//#include "MDTcabling/MDTcablingSvc.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "MuonRDO/MdtAmtHit.h"
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"

#include "MuonRDO/TgcRawData.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonDigitContainer/MdtDigit.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

#include "MuonDigitContainer/CscDigit.h"
#include "MuonDigitContainer/CscDigitCollection.h"
#include "MuonDigitContainer/CscDigitContainer.h"

#include "MuonDigitContainer/RpcDigit.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "MuonByteStreamCnvTest/MuonRdoToMuonDigitTool.h"

namespace std { template<typename _Tp> class auto_ptr; }

MuonRdoToMuonDigitTool::MuonRdoToMuonDigitTool(const std::string& type,const std::string& name,const IInterface* pIID) 
  : AlgTool(type, name, pIID),
    m_sgSvc("StoreGateSvc", name), 
    m_cscCalibTool("CscCalibTool"),
    m_mdtRdoDecoderTool("Muon::MdtRDO_Decoder"),
    m_cscRdoDecoderTool("Muon::CscRDO_Decoder"),
    m_rpcRdoDecoderTool("Muon::RpcRDO_Decoder"),
    m_tgcRdoDecoderTool("Muon::TgcRDO_Decoder"),
    m_mdtHelper(0), m_cscHelper(0), 
    m_rpcHelper(0), m_tgcHelper(0), 
    m_tgcCabling(0), m_mdtContainer(0), m_cscContainer(0),
    m_rpcContainer(0), m_tgcContainer(0), 
    m_is12foldTgc(true)
{

  declareInterface<IMuonDigitizationTool>(this);
  
  declareProperty("DecodeMdtRDO", m_decodeMdtRDO = true);
  declareProperty("DecodeCscRDO", m_decodeCscRDO = true);
  declareProperty("DecodeRpcRDO", m_decodeRpcRDO = true);
  declareProperty("DecodeTgcRDO", m_decodeTgcRDO = true);
  
  declareProperty("DataStore", m_sgSvc, "help"); 
  declareProperty("RetrievePrivateCopy", m_retrievePrivateCopy=false);
  declareProperty("cscCalibTool",  m_cscCalibTool);
  declareProperty("mdtRdoDecoderTool",  m_mdtRdoDecoderTool);
  declareProperty("cscRdoDecoderTool",  m_cscRdoDecoderTool);
  declareProperty("rpcRdoDecoderTool",  m_rpcRdoDecoderTool);
  declareProperty("tgcRdoDecoderTool",  m_tgcRdoDecoderTool);

  declareProperty("show_warning_level_invalid_TGC_A09_SSW6_hit", m_show_warning_level_invalid_TGC_A09_SSW6_hit = false);
}

MuonRdoToMuonDigitTool::~MuonRdoToMuonDigitTool()  
{}

StatusCode MuonRdoToMuonDigitTool::initialize() {

  m_log = new MsgStream(msgSvc(), name());
  m_debug = m_log->level() <= MSG::DEBUG;
  m_verbose = m_log->level() <= MSG::VERBOSE;
  if ( m_debug ) *m_log << MSG::DEBUG << " in initialize()" << endreq;

  // intitialize store gate active store
  StatusCode status = service("ActiveStoreSvc", m_acSvc);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not retrieve Active Store Service!" 
	   << endreq;
    return status;
  }
  if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved Active Store Service." << endreq;

  // intitialize transient event store
  if (m_sgSvc.retrieve().isFailure()) {
     *m_log << MSG::FATAL << "Could not retrieve  StoreGate Service !" << endreq;
     return StatusCode::FAILURE;
  }

  if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved StoreGate Service." << endreq;

  // initialize the detectore store service
  StoreGateSvc* detStore=0;
  status = serviceLocator()->service("DetectorStore", detStore);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  // initialize the identifier helpers

  status = detStore->retrieve(m_mdtHelper, "MDTIDHELPER");
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not get MdtIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the MdtIdHelper. " << endreq;
  }

  status = detStore->retrieve(m_cscHelper, "CSCIDHELPER");
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not get CscIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the CscIdHelper. " << endreq;
  }

  status = detStore->retrieve(m_rpcHelper, "RPCIDHELPER");
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not get RpcIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the RpcIdHelper. " << endreq;
  }

  status = detStore->retrieve(m_tgcHelper, "TGCIDHELPER");
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not get TgcIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the RpcIdHelper. " << endreq;
  }

  // get MDT cablingSvc
  //  status = service("MDTcablingSvc", m_mdtCabling);
  //
  //if (status.isFailure()) {
  // *m_log << MSG::WARNING << "Could not get MDTcablingSvc !" << endreq;
  //m_mdtCabling = 0;
  //return StatusCode::SUCCESS;
  //} 
  //  else {
  //if ( m_debug ) *m_log << MSG::DEBUG << " Found the MDTcablingSvc. " << endreq;
  //}

  // get RPC cablingSvc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  status = service("RPCcablingServerSvc", RpcCabGet);

  if (status.isFailure()) {
    *m_log << MSG::WARNING << "Could not get RPCcablingServiceSvc !" << endreq;
    m_rpcCabling = 0;
    return StatusCode::SUCCESS;
  }

  status = RpcCabGet->giveCabling(m_rpcCabling);

  if (status.isFailure()) {
    *m_log << MSG::WARNING << "Could not get RPCcablingSvc from the Server!" << endreq;
    m_rpcCabling = 0;
    return StatusCode::SUCCESS;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the RPCcablingSvc. " << endreq;
  }

  /** CSC calibratin tool for the Condtiions Data base access */
  status = m_cscCalibTool.retrieve();
  if (StatusCode::SUCCESS != status) {
    *m_log << MSG::FATAL << "Can't get handle on CSC calibration tools" << endreq;
    return StatusCode::FAILURE;
  }

  // create empty digit containers ready for filling

  try{
    m_mdtContainer = new MdtDigitContainer(m_mdtHelper->module_hash_max());
  } catch(std::bad_alloc){
    *m_log << MSG::FATAL
	   << "Could not create a new MdtDigitContainer!"<< endreq;
    return StatusCode::FAILURE;
  }
  m_mdtContainer->addRef();

  try{
    m_cscContainer = new CscDigitContainer(m_cscHelper->module_hash_max());
  } catch(std::bad_alloc){
    *m_log << MSG::FATAL
	   << "Could not create a new CscDigitContainer!"<< endreq;
    return StatusCode::FAILURE;
  }
  m_cscContainer->addRef();

  try{
    m_rpcContainer = new RpcDigitContainer(m_rpcHelper->module_hash_max());
  } catch(std::bad_alloc){
    *m_log << MSG::FATAL
	   << "Could not create a new RpcDigitContainer!"<< endreq;
    return StatusCode::FAILURE;
  }
  m_rpcContainer->addRef();

  try{
    m_tgcContainer = new TgcDigitContainer(m_tgcHelper->module_hash_max());
  } catch(std::bad_alloc){
    *m_log << MSG::FATAL
	   << "Could not create a new TgcDigitContainer!"<< endreq;
    return StatusCode::FAILURE;
  }
  m_tgcContainer->addRef();

  // Get MdtRdoDecoderTool
  //m_mdtRdoDecoder = new  MdtRDO_Decoder;
  //m_mdtRdoDecoder->set(m_mdtHelper, m_mdtCabling);
  // Get CscRdoDecoderTool
  if (m_mdtRdoDecoderTool.retrieve().isFailure()) {
      *m_log <<MSG::FATAL << "Failed to retrieve " << m_mdtRdoDecoderTool << endreq;
      return StatusCode::FAILURE;
  } else
      *m_log <<MSG::INFO << "Retrieved Tool " << m_mdtRdoDecoderTool << endreq;
  if (m_cscRdoDecoderTool.retrieve().isFailure()) {
      *m_log <<MSG::FATAL << "Failed to retrieve " << m_cscRdoDecoderTool << endreq;
      return StatusCode::FAILURE;
  } else
      *m_log <<MSG::INFO << "Retrieved Tool " << m_cscRdoDecoderTool << endreq;
  // Get RpcRdoDecoderTool
  if (m_rpcRdoDecoderTool.retrieve().isFailure()) {
      *m_log <<MSG::FATAL << "Failed to retrieve " << m_rpcRdoDecoderTool << endreq;
      return StatusCode::FAILURE;
  } else
      *m_log <<MSG::INFO << "Retrieved Tool " << m_rpcRdoDecoderTool << endreq;  
  // Get TgcRdoDecoderTool
  if (m_tgcRdoDecoderTool.retrieve().isFailure()) {
      *m_log <<MSG::FATAL << "Failed to retrieve " << m_tgcRdoDecoderTool << endreq;
      return StatusCode::FAILURE;
  } else
      *m_log <<MSG::INFO << "Retrieved Tool " << m_tgcRdoDecoderTool << endreq;

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::digitize() {

  if ( m_debug ) *m_log << MSG::DEBUG << " *************** in MuonRdoToMuonDigitTool::digitize()" << endreq;
  if ( m_debug ) *m_log << MSG::DEBUG << "in digitze()" << endreq;

  StatusCode status = StatusCode::SUCCESS;

  // process MDT RDO
  if (m_decodeMdtRDO) { 
    status = decodeMdtRDO();
    if (status.isFailure()) {
      *m_log << MSG::FATAL << "Unable to decode MDT RDO into MDT digits" 
	     << endreq;
      return status;
    }
  }

  // process CSC RDO
  if (m_decodeCscRDO) { 
    status = decodeCscRDO();
    if (status.isFailure()) {
      *m_log << MSG::FATAL << "Unable to decode CSC RDO into CSC digits" 
	     << endreq;
      return status;
    }
  }

  // process RPC RDO
  if (m_decodeRpcRDO && m_rpcCabling) { 
    status = decodeRpcRDO();
    if (status.isFailure()) {
      *m_log << MSG::FATAL << "Unable to decode RPC RDO into RPC digits" 
	     << endreq;
      return status;
    }
  }

  // process TGC RDO
  if(!m_tgcCabling && getTgcCabling().isFailure()) return StatusCode::FAILURE;

  if (m_decodeTgcRDO && m_tgcCabling) { 
    status = decodeTgcRDO();
    if (status.isFailure()) {
      *m_log << MSG::FATAL << "Unable to decode TGC RDO into TGC digits" 
	     << endreq;
      return status;
    }
  }

  return status;
}


StatusCode MuonRdoToMuonDigitTool::finalize() {
    
   if ( m_debug ) *m_log << MSG::DEBUG << "finalize." << endreq;
   delete m_log;

   if(m_mdtContainer) m_mdtContainer->release();
   if(m_cscContainer) m_cscContainer->release();
   if(m_rpcContainer) m_rpcContainer->release();
   if(m_tgcContainer) m_tgcContainer->release();

   return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeMdtRDO() {

  if ( m_debug ) *m_log << MSG::DEBUG << "Decoding MDT RDO into MDT Digit" << endreq;

  // clean up the digit container
  m_mdtContainer->cleanup();

  // retrieve the collection of RDO
  Identifier oldId;
  MdtDigitCollection * collection = 0;
  const MdtCsmContainer* rdoContainer;

  if ( !m_retrievePrivateCopy ) {
     m_acSvc->setStore( &*m_sgSvc );
     StatusCode status = m_sgSvc->retrieve( rdoContainer, "MDTCSM" );
     if (status.isFailure()) {
        *m_log << MSG::WARNING << "No MDT RDO container in StoreGate!" << endreq;
        return StatusCode::SUCCESS;
     }
     if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved " << rdoContainer->size() << " MDT RDOs." 
                           << endreq;
     
     // now decode RDO into digits
     m_acSvc->setStore( &*m_sgSvc );
     MdtCsmContainer::const_iterator mdtCSM = rdoContainer->begin();
       
     for (; mdtCSM!=rdoContainer->end();++mdtCSM)
     {
         status = this->decodeMdt( *mdtCSM, collection, oldId); 
         if ( status.isFailure() ) return status;
     }
  } else {
     m_acSvc->setStore( &*m_sgSvc );
     std::auto_ptr<MdtCsmContainer> cdata(m_sgSvc->retrievePrivateCopy<MdtCsmContainer>("MDTCSM"));
     if( !cdata.get() ) {
       *m_log << MSG::WARNING << "Could not get real data MDT RDO container \"" << " MDTCSM " << "\"" << endreq;
       return StatusCode::SUCCESS;
     }

     MdtCsmContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
     MdtCsmContainer::const_iterator lastRdoColl = (cdata.get())->end();
     for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
     {
        const MdtCsm * rdoColl = *it_rdoColl;
        StatusCode status = this->decodeMdt( rdoColl, collection, oldId );
        if ( status.isFailure() ) return status;
     }
  } 

  // record the container in storeGate
  std::string key = "MDT_DIGITS";
  m_acSvc->setStore( &*m_sgSvc );
  StatusCode status = m_sgSvc->record(m_mdtContainer,key);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not record container of MDT digits" << endreq;
    return status;
  }

  return status;
}

StatusCode MuonRdoToMuonDigitTool::decodeCscRDO() {

  if ( m_debug ) *m_log << MSG::DEBUG << "Decoding CSC RDO into CSC Digit" << endreq;

  //  CscRDO_Decoder decoder;
  //  decoder.set(m_cscHelper);

  m_acSvc->setStore( &*m_sgSvc );

  /*
  const DataHandle<EventInfo> eventInfo;
  status = m_sgSvc->retrieve(eventInfo);
  if (status.isFailure()) {
    *m_log << MSG::ERROR << "Could not retrieve event info from TDS. - abort ..." << endreq;
    return status;
  }
  const std::string detdescr = eventInfo->event_type()->get_detdescr_tags();
  *m_log << MSG::DEBUG << "DetDescr tags = " << detdescr << endreq;
  */

  //  decoder.set(detdescr);

  // clean up the digit container
  m_cscContainer->cleanup();

  Identifier oldId;
  CscDigitCollection * collection = 0;
  const CscRawDataContainer* rdoContainer;

  // retrieve the collection of RDO
  if ( !m_retrievePrivateCopy ) {
     m_acSvc->setStore( &*m_sgSvc );
     StatusCode status = m_sgSvc->retrieve( rdoContainer, "CSCRDO" );
     if (status.isFailure()) {
       *m_log << MSG::WARNING << "No CSC RDO container in StoreGate!" << endreq;
       return StatusCode::SUCCESS;
     }
     if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved " << rdoContainer->size() << " CSC RDOs." 
                           << endreq;
     // now decode RDO into digits
     m_acSvc->setStore( &*m_sgSvc );
     const DataHandle<CscRawDataCollection> rdoColl;
     const DataHandle<CscRawDataCollection> lastRdoColl;
     m_acSvc->setStore( &*m_sgSvc );
     if (StatusCode::SUCCESS != m_sgSvc->retrieve(rdoColl,lastRdoColl)) {
       if ( m_debug ) *m_log << MSG::DEBUG << "Cannot retrieve CscRawDataCollection" << endreq;
     }
  
     for (; rdoColl!=lastRdoColl; ++rdoColl) {
       //        status = this->decodeCsc ( rdoColl, collection, oldId, decoder );
       status = this->decodeCsc ( rdoColl, collection, oldId);
        if ( status.isFailure() ) return status;
     }
  } else {
     m_acSvc->setStore( &*m_sgSvc );
     std::auto_ptr<CscRawDataContainer> cdata(m_sgSvc->retrievePrivateCopy<CscRawDataContainer>("CSCRDO"));
     if( !cdata.get() ) {
         *m_log << MSG::WARNING << "Could not get real data CSC RDO container \"" << " CSCRDO " << "\"" << endreq;
         return StatusCode::SUCCESS;
     }
     CscRawDataContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
     CscRawDataContainer::const_iterator lastRdoColl = (cdata.get())->end();
     for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
     {
        const CscRawDataCollection * rdoColl = *it_rdoColl;
        //        status = this->decodeCsc( rdoColl, collection, oldId, decoder );
        StatusCode status = this->decodeCsc( rdoColl, collection, oldId);
        if ( status.isFailure() ) return status;
     }
  } 
  /** record the container in storeGate */
  std::string key = "CSC_DIGITS";
  m_acSvc->setStore( &*m_sgSvc );
  StatusCode status = m_sgSvc->record(m_cscContainer,key);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not record container of CSC digits" << endreq;
    return status;
  }

  /** set the const on this container */
  status = m_sgSvc->setConst(m_cscContainer);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not setConst on container of CSC digits" << endreq;
    return status;
  }

  return status;
}

StatusCode MuonRdoToMuonDigitTool::decodeRpcRDO() {

    if ( m_debug ) *m_log << MSG::DEBUG << "Decoding RPC RDO into RPC Digit" << endreq;

    // clean up the digit container
    m_rpcContainer->cleanup();

    RpcDigitCollection * collection = 0;
    const RpcPadContainer* rdoContainer;

    // retrieve the collection of RDO
    if ( !m_retrievePrivateCopy ) {
       m_acSvc->setStore( &*m_sgSvc );
       StatusCode status = m_sgSvc->retrieve( rdoContainer, "RPCPAD" );
       if (status.isFailure()) {
          *m_log << MSG::WARNING << "No RPC RDO container in StoreGate!" << endreq;
          return StatusCode::SUCCESS;
       }
       if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved " << rdoContainer->size() << " RPC RDOs." 
                             << endreq;
       // now decode RDO into digits
       m_acSvc->setStore( &*m_sgSvc );
       RpcPadContainer::const_iterator rpcPAD = rdoContainer->begin();
       
       for (; rpcPAD!=rdoContainer->end();++rpcPAD)
       {
          if ( (*rpcPAD)->size() > 0 ) {
             status = this->decodeRpc ( *rpcPAD, collection ); 
             if ( status.isFailure() ) return status;
          }
       }
    } else {
      m_acSvc->setStore( &*m_sgSvc );
      std::auto_ptr<RpcPadContainer> cdata(m_sgSvc->retrievePrivateCopy<RpcPadContainer>("RPCPAD"));
      if( !cdata.get() ) {
         *m_log << MSG::WARNING << "Could not get real data CSC RDO container \"" << " RPCPAD " << "\"" << endreq;
         return StatusCode::SUCCESS;
      }
      RpcPadContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
      RpcPadContainer::const_iterator lastRdoColl = (cdata.get())->end();
      for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
      {
         const RpcPad * rdoColl = *it_rdoColl;
         StatusCode status = this->decodeRpc( rdoColl, collection );
         if ( status.isFailure() ) return status;
      }
    } 

    // record the container in storeGate
    std::string key = "RPC_DIGITS";
    m_acSvc->setStore( &*m_sgSvc );
    StatusCode status = m_sgSvc->record(m_rpcContainer,key);
    if (status.isFailure()) {
        *m_log << MSG::FATAL << "Could not record container of RPC digits" << endreq;
        return status;
    }

    return status;
}

StatusCode MuonRdoToMuonDigitTool::decodeTgcRDO() {

  if ( m_debug ) *m_log << MSG::DEBUG << "Decoding TGC RDO into TGC Digit" << endreq;

  if(!m_tgcCabling && getTgcCabling().isFailure()) return StatusCode::FAILURE;
  
  // clean up the digit container
  m_tgcContainer->cleanup();
  
  Identifier oldElementId;

  // retrieve the collection of RDO
  if ( !m_retrievePrivateCopy ) {
     m_acSvc->setStore( &*m_sgSvc );
     const TgcRdoContainer* rdoContainer;
     StatusCode status = m_sgSvc->retrieve( rdoContainer, "TGCRDO" );
     if (status.isFailure())
     {
        *m_log << MSG::WARNING << "No TGC RDO container in StoreGate!" << endreq;
        return StatusCode::SUCCESS;
     }  
     if ( m_debug ) *m_log << MSG::DEBUG << "Retrieved " << rdoContainer->size() << " TGC RDOs." 
                           << endreq;

     m_acSvc->setStore( &*m_sgSvc );
     TgcRdoContainer::const_iterator tgcRDO = rdoContainer->begin();
       
     for (; tgcRDO!=rdoContainer->end();++tgcRDO)
     {
         if ( (*tgcRDO)->size() > 0 ) {
             status = this->decodeTgc ( *tgcRDO, oldElementId ); 
             if ( status.isFailure() ) return status;
         }
     }
     
  } else {
     m_acSvc->setStore( &*m_sgSvc );
     std::auto_ptr<TgcRdoContainer> cdata(m_sgSvc->retrievePrivateCopy<TgcRdoContainer>("TGCRDO"));
     if( !cdata.get() ) {
         *m_log << MSG::WARNING << "Could not get real data TGC RDO container \"" << " TGCRDO " << "\"" << endreq;
         return StatusCode::SUCCESS;
     }
     TgcRdoContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
     TgcRdoContainer::const_iterator lastRdoColl = (cdata.get())->end();
     for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
     {
        const TgcRdo * rdoColl = *it_rdoColl;
        StatusCode status = this->decodeTgc( rdoColl, oldElementId );
        if ( status.isFailure() ) return status;
     }
  } 
  // record the container in storeGate
  std::string key = "TGC_DIGITS";
  m_acSvc->setStore( &*m_sgSvc );
  StatusCode status = m_sgSvc->record(m_tgcContainer,key);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not record container of TGC digits" << endreq;
    return status;
  }

  return status;
}

StatusCode MuonRdoToMuonDigitTool::decodeMdt( const MdtCsm * rdoColl, MdtDigitCollection*& collection, Identifier& oldId ) {

    IdContext mdtContext = m_mdtHelper->module_context();
 
    if ( rdoColl->size() > 0 ) {
	if ( m_debug ) *m_log << MSG::DEBUG << " Number of AmtHit in this Csm " 
	                      << rdoColl->size() << endreq;
	
	uint16_t subdetId = rdoColl->SubDetId();
	uint16_t mrodId = rdoColl->MrodId();
	uint16_t csmId = rdoColl->CsmId();
	
	// for each Csm, loop over AmtHit, converter AmtHit to digit
	// retrieve/create digit collection, and insert digit into collection
	MdtCsm::const_iterator itD   = rdoColl->begin(); 
	MdtCsm::const_iterator itD_e = rdoColl->end();
	
	for (; itD!=itD_e; ++itD)
	  { 
	    const MdtAmtHit * amtHit = (*itD);
	    MdtDigit * newDigit = m_mdtRdoDecoderTool->getDigit(amtHit,subdetId,
                                                                mrodId,csmId);
	    
	    if (newDigit==NULL) {
	      *m_log << MSG::WARNING << "Error in MDT RDO decoder" << endreq;
              continue;
	    }
	
	    // find here the Proper Digit Collection identifier, using the rdo-hit id
	    // (since RDO collections are not in a 1-to-1 relation with digit collections)
	    Identifier elementId = m_mdtHelper->elementID(newDigit->identify());
	    IdentifierHash coll_hash;
	    if (m_mdtHelper->get_hash(elementId, coll_hash, &mdtContext)) {
	      *m_log << MSG::WARNING << "Unable to get MDT digit collection hash id " 
		     << "context begin_index = " << mdtContext.begin_index()
		     << " context end_index  = " << mdtContext.end_index()
		     << " the identifier is "
		     << endreq;
	      elementId.show();
	    } 

     
	    if (oldId != elementId) {
              m_acSvc->setStore( &*m_sgSvc );
              MdtDigitContainer::const_iterator it_coll = m_mdtContainer->indexFind(coll_hash);
              if (m_mdtContainer->end() ==  it_coll) {
		MdtDigitCollection * newCollection = 
		  new MdtDigitCollection(elementId,coll_hash);
		newCollection->push_back(newDigit);
		collection = newCollection;
                m_acSvc->setStore( &*m_sgSvc );
		StatusCode status = m_mdtContainer->addCollection(newCollection, coll_hash);
		if (status.isFailure())
		  *m_log << MSG::WARNING 
		         << "Couldn't record MdtDigitCollection with key=" << coll_hash 
		         << " in StoreGate!" << endreq;
	      } else {  
		MdtDigitCollection * oldCollection = const_cast<MdtDigitCollection*>( it_coll->cptr() );
		oldCollection->push_back(newDigit);
		collection = oldCollection;
	      }
	      oldId = elementId;
	    } else {
	      collection->push_back(newDigit);
	    }
	  }
    }
    return StatusCode::SUCCESS;
}  

StatusCode MuonRdoToMuonDigitTool::decodeCsc( const CscRawDataCollection * rdoColl, CscDigitCollection*& collection, 
                                              Identifier& oldId){// , CscRDO_Decoder& decoder) {

      if ( m_debug ) *m_log << MSG::DEBUG << " Number of RawData in this rdo " 
	                    << rdoColl->size() << endreq;
      
      double samplingTime = rdoColl->rate();
      unsigned int numSamples   = rdoColl->numSamples();

      //      decoder.setParams(samplingTime);

      IdContext cscContext = m_cscHelper->module_context();

      /** for each Rdo, loop over RawData, converter RawData to digit
	  retrieve/create digit collection, and insert digit into collection */
      CscRawDataCollection::const_iterator itD   = rdoColl->begin(); 
      CscRawDataCollection::const_iterator itD_e = rdoColl->end();
      for (; itD!=itD_e; ++itD) { 
	const CscRawData * data = (*itD);
	uint16_t width = data->width();
        //        Identifier stationId = decoder.stationIdentifier(data);
        Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data);
	for (int j=0; j<width; ++j) {
          //          Identifier channelId = decoder.channelIdentifier(data, j);
          Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
	  std::vector<uint16_t> samples;
	  bool extractSamples = data->samples(j, numSamples, samples);
	  if (!extractSamples) {
	     *m_log << MSG::WARNING << "Unable to extract samples for strip " << j 
	            << " Online Cluster width = " << width 
	            << " for number of Samples = " << numSamples 
                    << " continuing ..." << endreq; 
	     continue;
          }	 
	  std::vector<float> charges;
	  bool adctocharge = m_cscCalibTool->adcToCharge(samples, data->hashId(), charges);
	  double charge = 0.0;
	  double time   = 0.0;
	  unsigned int samplingPhase = 0;
          int errorfindCharge = m_cscCalibTool->findCharge(samplingTime, samplingPhase, charges, charge, time);
          // WPARK: Potential problem! error code includes time out of range and aa>0
          // If this is the case, only very nice samples are converted to Digit. Sep 11, 2009
	  if ( !adctocharge || errorfindCharge ) { 
	    *m_log << MSG::WARNING << " CSC conversion ADC to Charge failed " 
	 	   << "CSC Digit not build ... " 
	           << " or charge finding failed " << " ... skipping " << std::endl;
	    continue;
	  }
	  if ( m_debug ) *m_log << MSG::DEBUG << "CSC RDO->CscDigit: " << m_cscHelper->show_to_string(channelId) 
	                        << endreq;
	  int theCharge = static_cast<int>(charge);
	  CscDigit * newDigit = new CscDigit(channelId, theCharge, time);
	  if ( m_debug ) *m_log << MSG::DEBUG << "CSC RDO->Digit: " << m_cscHelper->show_to_string(newDigit->identify()) 
	                        << " " << newDigit->charge() << " " << charge << " time= " << time << endreq; 

	  for (uint16_t i=0; i< samples.size(); ++i) {
	    if ( m_debug ) *m_log << MSG::DEBUG << "CSC RDO->Digit: " << samples[i] << endreq;
	  }
          IdentifierHash coll_hash;
          if (m_cscHelper->get_hash(stationId, coll_hash, &cscContext)) {
             *m_log << MSG::WARNING << "Unable to get CSC digiti collection hash id " 
             << "context begin_index = " << cscContext.begin_index()
             << " context end_index  = " << cscContext.end_index()
             << " the identifier is "
             << endreq;
             stationId.show();
          } 

	  if (oldId != stationId) {
            m_acSvc->setStore( &*m_sgSvc );
            CscDigitContainer::const_iterator it_coll = m_cscContainer->indexFind(coll_hash);
            if (m_cscContainer->end() ==  it_coll) {
		CscDigitCollection * newCollection = new CscDigitCollection(stationId, coll_hash);
		newCollection->push_back(newDigit);
		collection = newCollection;
                m_acSvc->setStore( &*m_sgSvc );
		StatusCode status = m_cscContainer->addCollection(newCollection, coll_hash);
		if (status.isFailure())
		  *m_log << MSG::WARNING 
		         << "Couldn't record CscDigitCollection with key=" << coll_hash 
		         << " in StoreGate!" << endreq;
	    } else {  
	      CscDigitCollection * oldCollection = const_cast<CscDigitCollection*>( it_coll->cptr() );
	      oldCollection->push_back(newDigit);
	      collection = oldCollection;
	    }
	    oldId = stationId;
	  } else {
	    collection->push_back(newDigit);
	  }
	}
      }
      return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeRpc( const RpcPad * rdoColl, RpcDigitCollection*& collection ) {

            IdContext rpcContext = m_rpcHelper->module_context();

            if ( m_debug ) *m_log << MSG::DEBUG << " Number of CMs in this Pad " 
                                  << rdoColl->size() << endreq;
	
            Identifier padOfflineId = rdoColl->identify();
            //Get pad online id and sector id
            uint16_t padId     = rdoColl->onlineId(); 
            uint16_t sectorId  = rdoColl->sector(); 

            int stationName = m_rpcHelper->stationName(padOfflineId);
            int stationEta  = m_rpcHelper->stationEta(padOfflineId);
            int stationPhi  = m_rpcHelper->stationPhi(padOfflineId);
            int doubletR    = m_rpcHelper->doubletR(padOfflineId);
	
            Identifier elementId = m_rpcHelper->elementID(stationName, stationEta,
                                                          stationPhi, doubletR);
	
            // For each pad, loop on the coincidence matrices
            RpcPad::const_iterator itCM   = rdoColl->begin();
            RpcPad::const_iterator itCM_e = rdoColl->end();
	
            for (; itCM != itCM_e ; ++itCM) {
	  
                // Get CM online Id
                uint16_t cmaId = (*itCM)->onlineId();

                // For each CM, loop on the fired channels
                RpcCoinMatrix::const_iterator itD   = (*itCM)->begin();
                RpcCoinMatrix::const_iterator itD_e = (*itCM)->end();
	  
                for (; itD != itD_e ; ++itD) {

                    const RpcFiredChannel * rpcChan = (*itD);
                    std::vector<RpcDigit*>* digitVec = 
                        m_rpcRdoDecoderTool->getDigit(rpcChan, sectorId, padId, cmaId);
	    
                    if (digitVec==NULL) {
                        *m_log << MSG::FATAL << "Error in the RPC RDO decoder " << endreq;
                        return StatusCode::FAILURE;
                    }

                    // Loop on the digits corresponding to the fired channel
                    std::vector<RpcDigit*>::const_iterator itVec   = digitVec->begin();
                    std::vector<RpcDigit*>::const_iterator itVec_e = digitVec->end();
		
                    for ( ; itVec!=itVec_e; ++itVec) 
                    {
                        collection = 0;
                        RpcDigit* newDigit = (*itVec);
                        elementId = m_rpcHelper->elementID(newDigit->identify());
                        
                        IdentifierHash coll_hash;
                        if (m_rpcHelper->get_hash(elementId, coll_hash, &rpcContext)) {
                           *m_log << MSG::WARNING << "Unable to get RPC digit collection hash id " 
                                  << "context begin_index = " << rpcContext.begin_index()
                                  << " context end_index  = " << rpcContext.end_index()
                                  << " the identifier is "
                                  << endreq;
                                  elementId.show();
                        } 

                        m_acSvc->setStore( &*m_sgSvc );
                        RpcDigitContainer::const_iterator it_coll = m_rpcContainer->indexFind(coll_hash);
                        if (m_rpcContainer->end() ==  it_coll) {
                            RpcDigitCollection * newCollection = new RpcDigitCollection(elementId, coll_hash);
                            newCollection->push_back(newDigit);
                            collection = newCollection;
                            m_acSvc->setStore( &*m_sgSvc );
                            StatusCode status = m_rpcContainer->addCollection(newCollection, coll_hash);
                            if (status.isFailure())
                                *m_log << MSG::WARNING
                                       << "Couldn't record RpcDigitCollection with key=" 
                                       << coll_hash << " in StoreGate!" << endreq;
                        }
                        else
                        {
	                   RpcDigitCollection * oldCollection = const_cast<RpcDigitCollection*>( it_coll->cptr() );
	                   oldCollection->push_back(newDigit);
                           collection = oldCollection;
                        }
                    }
                    delete digitVec; 
                }
            }
    
            return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeTgc( const TgcRdo *rdoColl, 
                                              Identifier& oldElementId ) { 
      TgcDigitCollection* collection = 0; 

      if(!m_tgcCabling && getTgcCabling().isFailure()) return StatusCode::FAILURE;

      IdContext tgcContext = m_tgcHelper->module_context();

      if ( m_debug ) *m_log << MSG::DEBUG << "Number of RawData in this rdo " 
                            << rdoColl->size() << endreq;
      // for each Rdo, loop over RawData, converter RawData to digit
      // retrieve/create digit collection, and insert digit into collection
      TgcRdo::const_iterator itD  = rdoColl->begin();
      TgcRdo::const_iterator itDe = rdoColl->end();

      std::map<std::vector<uint16_t>, uint16_t> stripMap;
      if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used 
	// TGC2 Endcap Strip OR channel treatement preparation start
	// Signals are ORed as follows: 
	// |stationEta|=5, T9, E1 - slbId=16, bit0 of stripSlbBits in this code 
	//                        /
	// |stationEta|=4, T8, E2 - slbId=17, bit1 of stripSlbBits in this code 
	//                        /
	// |stationEta|=3, T7, E3 - slbId=18, bit2 of stripSlbBits in this code 
	//                        /
	// |stationEta|=2, T6, E4 - slbId=19, bit3 of stripSlbBits in this code 
	//                        /
	// |stationEta|=1, T4, E5 - slbId=20, bit4 of stripSlbBits in this code 
	for (; itD != itDe; ++itD) {
	  if((*itD)->isCoincidence()) continue; // Require hits
	  if((*itD)->slbType()!=TgcRawData::SLB_TYPE_DOUBLET_STRIP) continue; // Require TGC2 or TGC3
	  if((*itD)->sswId()==7) continue; // Exclude Forward
	  if((*itD)->bitpos()<112 || (*itD)->bitpos()>199) continue; // Require C, D-input
	  
	  std::vector<uint16_t> stripId(5, 0);
	  stripId.at(0) = (*itD)->subDetectorId();
	  stripId.at(1) = (*itD)->rodId();
	  stripId.at(2) = (*itD)->sswId();
	  stripId.at(3) = (*itD)->bitpos();
	  stripId.at(4) = (*itD)->bcTag();
	  uint16_t stripSlbBits = 0x1 << ((*itD)->slbId()-16);
	  std::map<std::vector<uint16_t>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond channel 
	  if(itMap==stripMap.end()) { // This is new one
	    stripMap.insert(std::map<std::vector<uint16_t>, uint16_t>::value_type(stripId, stripSlbBits));
	  } else { // This already exists
	    itMap->second |= stripSlbBits;
	  }
	}
	// Covert to original hit patterns 
	std::map<std::vector<uint16_t>, uint16_t>::iterator jtMap   = stripMap.begin();
	std::map<std::vector<uint16_t>, uint16_t>::iterator jtMap_e = stripMap.end();
	for(; jtMap!=jtMap_e; jtMap++) {
	  if(jtMap->second<=31) {
	    // x    : 5-bit variable
	    // f(x) : OR function above, Digit->RDO conversion 
	    // g(x) : originalHitBits which satisfies f(g(f(x))) = f(x), RDO->Digit conversion 
	    static const uint16_t originalHitBits[32] = {
	      //  0   1   2   3   4   5   6   7
	          0,  1,  0,  3,  0,  0,  4,  7,
	      //  8   9  10  11  12  13  14  15
	          0,  0,  0,  0,  8,  9, 12, 15,
	      // 16  17  18  19  20  21  22  23
		  0,  0,  0,  0,  0,  0,  0,  0,
	      // 24  25  26  27  28  29  30  31
		 16, 17,  0, 19, 24, 25, 28, 31
	    };
	    jtMap->second = originalHitBits[jtMap->second]; 
	  } else {
	    jtMap->second = 0;
	  }
	}
	// TGC2 Endcap Strip OR channel treatement preparation end
	itD = rdoColl->begin();
      }

      for (; itD != itDe; ++itD)
        { 
	  // check Hit or Coincidence
	  if ((*itD)->isCoincidence()) continue;
	  
	  if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used 
	    // TGC2 Endcap Strip OR channel treatement start 
	    if((*itD)->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP && // Require TGC2 or TGC3
	       (*itD)->sswId()!=7 && // Exclude Forward
	       (*itD)->bitpos()>=112 && (*itD)->bitpos()<=199 // Require C, D-input
	       ) {
	      std::vector<uint16_t> stripId(5, 0);
	      stripId.at(0) = (*itD)->subDetectorId();
	      stripId.at(1) = (*itD)->rodId();
	      stripId.at(2) = (*itD)->sswId();
	      stripId.at(3) = (*itD)->bitpos();
	      stripId.at(4) = (*itD)->bcTag();
	      std::map<std::vector<uint16_t>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond hit
	      if(itMap!=stripMap.end()) {
		uint16_t stripSlbBits = 0x1 << ((*itD)->slbId()-16);
		if(!(itMap->second & stripSlbBits)) continue; // This hit is additional.
	      }
	    }
	    // TGC2 Endcap Strip OR channel treatement end 
	  }

	  // repeat two times for ORed channel
	  for (int iOr=0; iOr<2; ++iOr)
	    {

	      if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used 
		// TGC2 Endcap Strip OR channel is not converted. 
		if(iOr && (*itD)->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP) continue; 
	      }

	      bool orFlag = false;
	      
	      // check if this channel has ORed partner only when 2nd time
	      if (iOr != 0) 
		{
		  bool o_found = m_tgcCabling->isOredChannel((*itD)->subDetectorId(),
							  (*itD)->rodId(),
							  (*itD)->sswId(),
							  (*itD)->slbId(),
							  (*itD)->bitpos());
		  // set OR flag
		  if (o_found)
		    orFlag=true;
		  else
		    continue;
		}
	      
	      // get element ID
	      Identifier elementId;
	      bool e_found = m_tgcCabling->getElementIDfromReadoutID
		(elementId,(*itD)->subDetectorId(),(*itD)->rodId(),
		 (*itD)->sswId(),(*itD)->slbId(),(*itD)->bitpos(),orFlag);
	      
	      if (!e_found) {
		bool show_warning_level = true;
		
		/* One invalid channel in TGC sector A09:
		   sub=103 rod=9 ssw=6 slb=20 bitpos=151 orFlag=0
		   was always seen in 2008 data, at least run 79772 - 91800.
		   bug #48828 */
		if((*itD)->subDetectorId()==103 &&
		   (*itD)->rodId()==9 &&
		   (*itD)->sswId()==6 &&
		   (*itD)->slbId()==20 &&
		   (*itD)->bitpos()==151) {
		  show_warning_level = m_show_warning_level_invalid_TGC_A09_SSW6_hit;
		}

		if(show_warning_level || m_debug) {
		  *m_log << (show_warning_level ? MSG::WARNING : MSG::DEBUG) << "ElementID not found for "
			 << " sub=" << (*itD)->subDetectorId()
			 << " rod=" << (*itD)->rodId()
			 << " ssw=" << (*itD)->sswId()
			 << " slb=" << (*itD)->slbId()
			 << " bitpos=" << (*itD)->bitpos() 
			 << " orFlag=" << orFlag << endreq;
		}
		continue;
	      }
	      
	      // convert RawData to Digit
	      TgcDigit * newDigit = m_tgcRdoDecoderTool->getDigit(*itD,orFlag);
	      
	      // check if converted correctly
	      if (!newDigit) continue;
	      
	      // check new element or not
              IdentifierHash coll_hash;
              if (m_tgcHelper->get_hash(elementId, coll_hash, &tgcContext)) {
                 *m_log << MSG::WARNING << "Unable to get TGC digit collection hash " 
                 << "context begin_index = " << tgcContext.begin_index()
                 << " context end_index  = " << tgcContext.end_index()
                 << " the identifier is "
                 << endreq;
                 elementId.show();
              } 

	      if (elementId != oldElementId)
		{
		  // get collection
                  m_acSvc->setStore( &*m_sgSvc );
                  TgcDigitContainer::const_iterator it_coll = m_tgcContainer->indexFind(coll_hash);
                  if (m_tgcContainer->end() !=  it_coll) {
                       collection = const_cast<TgcDigitCollection*>( it_coll->cptr() );
		    }
		  else
		    {
		      // create new collection
		      if ( m_debug ) *m_log << MSG::DEBUG << "Created a new digit collection : " << coll_hash << endreq;
		      collection = new TgcDigitCollection(elementId, coll_hash);
                      m_acSvc->setStore( &*m_sgSvc );
		      StatusCode status = m_tgcContainer->addCollection(collection,coll_hash);
		      if (status.isFailure())
			{
			  *m_log << MSG::FATAL
			         << " Unable to record TGC Digit Collection in StoreGate" <<endreq;
			  delete newDigit; newDigit = 0;
			  return status;
			}
		    }
		  
		  oldElementId = elementId;
		}

	      if(!collection) {
		*m_log << MSG::WARNING << "MuonRdoToMuonDigitTool::decodeTgc TgcDigitCollection* collection is null." << endreq;
		delete newDigit;
		newDigit = 0;
		return StatusCode::SUCCESS;
	      }
	      
	      // check duplicate digits
	      TgcDigitCollection::const_iterator it_tgcDigit;
	      bool duplicate = false;
	      for (it_tgcDigit=collection->begin(); it_tgcDigit != collection->end(); it_tgcDigit++) {
		if ((newDigit->identify() == (*it_tgcDigit)->identify()) && 
		    (newDigit->bcTag()    == (*it_tgcDigit)->bcTag())) {
		  duplicate = true;
		  if ( m_debug ) *m_log << MSG::DEBUG << "Duplicate TGC Digit removed"  << endreq;
		  break;
		}
	      }
	      if (!duplicate) {
		// add the digit to the collection
		collection->push_back(newDigit);
	      } else {
		delete newDigit;
		newDigit = 0;
	      }
	    }
	}
      return StatusCode::SUCCESS;
}
 
StatusCode MuonRdoToMuonDigitTool::getTgcCabling() {
  // get TGC cablingSvc
  const ITGCcablingServerSvc* TgcCabGet=0;
  StatusCode status = service("TGCcablingServerSvc", TgcCabGet);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not get ITGCcablingSvc Server!" << endreq;
    m_tgcCabling = 0;
    return status;
  }

  status = TgcCabGet->giveCabling(m_tgcCabling);
  if (status.isFailure()) {
    *m_log << MSG::FATAL << "Could not get ITGCcablingSvc from the Server!" << endreq;
    m_tgcCabling = 0;
    return status;
  } 
  else {
    if ( m_debug ) *m_log << MSG::DEBUG << " Found the ITGCcablingSvc. " << endreq;
  }

  m_is12foldTgc = TgcCabGet->isAtlas();

  return status;
}
