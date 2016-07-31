/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of RpcROD_Decoder class 

#include "RpcROD_Decoder.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include <algorithm> 

#include <deque>
#include <vector>
#include <utility>
#include <algorithm>
//Gaudi
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IssueSeverity.h"
#include "GaudiKernel/ListItem.h"
//#include "GaudiKernel/IIncidentSvc.h"

//niko
//#include "RpcByteStreamAccess/IRPC_ByteStreamErrorSvc.h"


//Athena
#include "eformat/Version.h"
#include "ByteStreamData/RawEvent.h"
#include "AthenaBaseComps/AthAlgTool.h"

Muon::RpcROD_Decoder::RpcROD_Decoder ( const std::string& type, const std::string& name,const IInterface* parent ) :  AthAlgTool(type,name,parent),
														      //m_hashfunc(0),
														      //m_storeGate("StoreGateSvc" ,name),
														      //niko
														      m_decodeSL(false),
														      //m_byteStreamErrSvc("RPC_ByteStreamErrorSvc",name),
														      m_cabling(0),
														      m_pRpcIdHelper(0) //,m_bench("RpcROD_Decoder")
{
    declareInterface< IRpcROD_Decoder  >( this );
    declareProperty("SpecialROBNumber",m_specialROBNumber=-1);
    declareProperty("Sector13Data",m_sector13Data=false);
    declareProperty("DataErrorPrintLimit",m_maxprinterror=1000);
    //declareProperty("RpcByteStreamErrorContainer",m_bsErrContainerName="myRpcByteStreamErrorContainer");
    
}


Muon::RpcROD_Decoder::~RpcROD_Decoder() {

}

StatusCode Muon::RpcROD_Decoder::initialize() {
 
    //   if (m_storeGate.retrieve().isFailure())
    //   {
    //       msg(MSG::ERROR) << "StoreGate not found " << endmsg;
    //       return StatusCode::FAILURE;
    //   }  
    //   if (m_storeGate.retrieve().isFailure())
    //   {
    //       msg(MSG::ERROR) << "StoreGate not found " << endmsg;
    //       return StatusCode::FAILURE;
    //   }


  ServiceHandle<IRPCcablingServerSvc> rpc_server("RPCcablingServerSvc", name());
  if (rpc_server.retrieve().isFailure()) {
	msg(MSG::FATAL) << " Can't get RPCcablingServerSvc " << endmsg;
	return StatusCode::FAILURE;
    }
  
    if (StatusCode::SUCCESS != rpc_server->giveCabling(m_cabling)) {
	msg(MSG::FATAL) << " Can't get RPCcablingSvc from Server" << endmsg;
	return StatusCode::FAILURE; 
    }

    StatusCode status=StatusCode::SUCCESS;
    /*
    try 
      {
	// needs to build differently with the id-hash 
	m_bsErrCont = new RpcByteStreamErrorContainer();
      } 
    catch(std::bad_alloc) {
      msg (MSG::FATAL)
        << "Could not create a new RPC ByteStreamErrorContainer!"<< endmsg;
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("RPC ByteStreamErrorContainer created");
    status = evtStore()->record(m_bsErrCont,m_bsErrContainerName);
    if (status!=StatusCode::SUCCESS)
      {
    	ATH_MSG_ERROR("Problems while recording RPC ByteStreamErrorContainer in SG");
      }
    else 
      ATH_MSG_INFO("RPC ByteStreamErrorContainer recorded in SG");
    */	

    //   ServiceHandle<StoreGateSvc> detStore("DetectorStore", name());
    //   if (detStore.retrieve().isFailure())
    //   {
    //       msg(MSG::ERROR) << "DetectorStore not found" << endmsg;
    //       return StatusCode::FAILURE;
    //   }

    /*  Do I have to use MuonMgr to retrieve the IdHelpers???
	if (detStore->retrieve(m_muonMgr).isFailure())
	{
	msg(MSG::ERROR) << "Cannot retrieve MuonDetectorManager" << endmsg;
	return sc;
	}
    */  

    // Get the RPC id helper from the detector store
    status = detStore()->retrieve(m_pRpcIdHelper, "RPCIDHELPER");
    if (status.isFailure()) {
	msg(MSG::FATAL) << "Could not get RpcIdHelper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	msg(MSG::VERBOSE) << " Found the RpcIdHelper. " << endmsg;
    }
  
    //m_hashfunc = new RpcPadIdHash();
  
    if (m_specialROBNumber>0) {
	msg(MSG::DEBUG) << "Setting the special ROB Number to: 0x" << MSG::hex << m_specialROBNumber
			<< MSG::dec <<endmsg;
    }

    //==LBTAG initialize vector and variables for format failure check
    for(int i=0; i<13; i++) m_RPCcheckfail[i]=0;
    m_previous=0;
    m_printerror=0;

    /*
    IIncidentSvc* incsvc;
    status = service("IncidentSvc", incsvc);
    int priority = 100;
    if( status.isSuccess() ) {
        incsvc->addListener( this, "BeginEvent", priority);
	incsvc->addListener( this, "EndEvent", priority);
    }	
    */

    // //niko
    // if (m_byteStreamErrSvc.retrieve().isFailure()) 
    // msg(MSG::FATAL) <<"Failed to get RPC_ByteStreamErrorSvc"<<endmsg;

    return StatusCode::SUCCESS;

}

//At the start of every event, create the bytestream error contatiner and store it to 
//Store-Gate but not fill here  
void Muon::RpcROD_Decoder::handle(const Incident& inc) {
  if ((inc.type() == "BeginEvent") ){
    msg(MSG::DEBUG) << "In handle::BeginEvent"<<endmsg;   
    /*
    // cleanup and record the error container 
    if (!evtStore()->contains<Muon::RpcByteStreamErrorContainer>(m_bsErrContainerName)){
      m_bsErrCont->clear();
      msg(MSG::DEBUG) << "In handle::BeginEvent after clear"<<endmsg;   
      //ServiceHandle<StoreGateSvc> sg = evtStore();
      //msg(MSG::DEBUG) << "In handle::BeginEvent after eventStore"<<endmsg;   
      // StatusCode status = evtStore()->record(m_bsErrCont,m_bsErrContainerName);
      // msg(MSG::DEBUG) << "In handle::BeginEvent after record"<<endmsg;   
      // if (status.isFailure() )
      // 	msg(MSG::ERROR) << "Failed to record BSErrors to SG"<<endmsg;   
    }
    */
  }
  
  return;
}



StatusCode Muon::RpcROD_Decoder::finalize() {
    //if(m_hashfunc) delete m_hashfunc;
  
  //==LBTAG print format failure final statistics
  printcheckformat();

    return StatusCode::SUCCESS;
} 


#include "RpcROD_Decoder_v302.C"
