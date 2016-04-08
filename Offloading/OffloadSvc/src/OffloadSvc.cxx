///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// OffloadSvc.cxx 
// Implementation file for class OffloadSvc
// Author: Sami Kama <sami.kama@cern.ch>
/////////////////////////////////////////////////////////////////// 

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

// OffloadManagerSvc includes
#include "OffloadSvc/OffloadSvc.h"
#include "APE/BufferContainer.hpp"
#include "APE/BufferAccessor.hpp"
#include "APE/ServerDefines.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <chrono>

// Constructors
////////////////
OffloadSvc::OffloadSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService ( name, pSvcLocator ){
  declareProperty( "ConnectionType",
		   m_connType = "SHM", "Connection type [SHM,PIPE,ZMQ]");
  declareProperty( "CommChannelSend",
		   m_commChannelSend = "apeSock", "Name of the APE socket for sending request");
  declareProperty( "CommChannelRecv",
		   m_commChannelRecv = "apeSock_upload", "Name of the APE socket for receiving results");
  declareProperty( "CommChannelUserUID",
		   m_useUID=true, "Whether to add uid to comm channels");
  
  m_isConnected=false;
  m_sendSock=0;
  m_recvSock=0;
  m_maxTokens=0;

  //std::cout<<__PRETTY_FUNCTION__<<"Constructing Service SAMI"<<std::endl;
}

// Destructor
///////////////
OffloadSvc::~OffloadSvc(){
}

StatusCode OffloadSvc::sendData(std::unique_ptr<APE::BufferContainer> &buff, int &token,bool requiresResponse){
  if(!m_isConnected){
    msg(MSG::ERROR)<<"APE Server communication was not open. Initiating communication"<<endreq;
    bool rc=openCommChannel();
    if(!rc){
      msg(MSG::FATAL)<<"APE Server communication Failed"<<endreq;
      return StatusCode::FAILURE;
    }
  }
  if(!buff){
    msg(MSG::ERROR)<<"BufferContainer is 0 "<<endreq;
    return StatusCode::FAILURE;
  }
  if(!m_isConnected){
    msg(MSG::ERROR)<<"Accelerator Process Extension connection is not open!"<<endreq;
    return StatusCode::FAILURE;
  }
  if(m_tokens.size()){
    token=m_tokens.front();
    m_tokens.pop();
  }else{
    token=m_maxTokens;
    m_maxTokens++;
  }
  APE::BufferAccessor::setToken(*buff,token);
  msg(MSG::DEBUG)
	   <<" Sending Data, Algorithm="<<buff->getAlgorithm()
	   <<" token="<<buff->getToken()
	   <<" module="<<buff->getModule()
	   <<" payloadSize="<<buff->getPayloadSize()
	   <<" TransferSize="<<buff->getTransferSize()
	   <<" userBuffer="<<buff->getBuffer()
	   <<endreq;
  if(requiresResponse){// will get reply
    TransferStats ts;
    ts.sendStart=std::chrono::system_clock::now();
    m_sendSock->send(APE::BufferAccessor::getRawBuffer(*buff),buff->getTransferSize());
    ts.sendEnd=std::chrono::system_clock::now();
    ts.uploadSize=buff->getTransferSize();
    ts.downloadSize = 0;
    m_stats[token]=ts;
  }else{// doesn't expect reply
    m_sendSock->send(APE::BufferAccessor::getRawBuffer(*buff),buff->getTransferSize());
    m_tokens.push(token);
  }
    return StatusCode::SUCCESS;  
}

StatusCode OffloadSvc::receiveData(std::unique_ptr<APE::BufferContainer> &buff, int token, int timeOut){
  if(!m_isConnected){
    msg(MSG::ERROR)<<"APE Server communication was not open. Initiating communication"<<endreq;
    bool rc=openCommChannel();
    if(!rc){
      msg(MSG::FATAL)<<"APE Server communication Failed"<<endreq;
      return StatusCode::FAILURE;
    }
  }
  ssize_t recvdSize=0;
  //MsgStream athenaLog(msgSvc(), name());

  void* rawBuffer=APE::BufferAccessor::getRawBuffer(*buff);
  if(timeOut>0){
    ATH_MSG_WARNING("Timeout parameter might not be implemented yet"); 
    recvdSize=m_recvSock->tryRecv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff),timeOut);
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_WARNING("Received possibly corrupt data. Trying again");
      recvdSize=m_recvSock->tryRecv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff),timeOut);      
    }
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_ERROR("Received corrupt data on both trials");      
      return StatusCode::FAILURE;
    }
  }else{
    recvdSize=m_recvSock->recv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff));
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_WARNING("Received possibly corrupt data. Trying again");
      recvdSize=m_recvSock->recv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff));
    }
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_ERROR("Received corrupt data on both trials");      
      return StatusCode::FAILURE;
    }
  }
  auto recvEnd=std::chrono::system_clock::now();
  msg(MSG::DEBUG)
	   <<" Receive Algorithm="<<buff->getAlgorithm()
	   <<" token="<<buff->getToken()
	   <<" module="<<buff->getModule()
	   <<" payloadSize="<<buff->getPayloadSize()
	   <<" TransferSize="<<buff->getTransferSize()
	   <<" userBuffer="<<buff->getBuffer()
	   <<" transferred bytes="<<recvdSize
	   <<endreq;

  auto it=m_stats.find(buff->getToken());
  if(it==m_stats.end()){
    msg(MSG::ERROR)
      <<" APE returned token "<<buff->getToken()
      <<" However this token was not sent by this process!"
      <<endreq;
    return StatusCode::FAILURE;
  }else{
    const TransferStats &ts=it->second;
    auto diff=recvEnd-it->second.sendStart;
    msg(MSG::DEBUG)<<"Send request took "<<std::chrono::duration_cast<std::chrono::microseconds>(ts.sendEnd-ts.sendStart).count()
		   <<"(usec). "
		   <<"Time between send and receive requests of token "
		   <<buff->getToken()<<" is "<<std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
		   <<"(ms). "
		   <<"Total transfer size= "<<ts.uploadSize<<" + "<<recvdSize<<"= "<< ts.uploadSize+recvdSize<<" bytes"
		   <<endreq;
    m_tokens.push(buff->getToken());
  }
  if(token!=buff->getToken()){
    ATH_MSG_ERROR("Received token "<<buff->getToken()<<" was expecting "<<token);
    return StatusCode::FAILURE;
  }
  if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
    ATH_MSG_WARNING("Receiving Failed");
    return StatusCode::FAILURE;
  }else if(recvdSize==0){
    ATH_MSG_WARNING("Received size=0");    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
// Athena Algorithm's Hooks
////////////////////////////
StatusCode OffloadSvc::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  IIncidentSvc* p_incSvc;
  auto sc=service("IncidentSvc",p_incSvc);
  if(sc!=StatusCode::SUCCESS){
    return sc;
  }
  p_incSvc->addListener(this,"PostFork",-1l);
  p_incSvc->addListener(this,"BeforeFork",-100l);
  return sc;
}

StatusCode OffloadSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  bool rv=closeCommChannel();
  return (rv?StatusCode::SUCCESS:StatusCode::FAILURE);
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
OffloadSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( IOffloadSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IOffloadSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void OffloadSvc::handle(const Incident& inc){
  ATH_MSG_INFO("Got incident "<<inc.type()<<" from "<<inc.source());
  if(inc.type()=="BeginRun"){
    
  }else if(inc.type()=="BeforeFork"){
    if(m_isConnected){
      closeCommChannel(true);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }else if(inc.type()=="PostFork"){
    if(!m_isConnected){
      openCommChannel(true);
    }
  }
}

bool OffloadSvc::openCommChannel(bool postFork){
  if(!m_isConnected){
    try{
      char buff[1000];
      uid_t userUID=geteuid();
      auto chanType=yampl::LOCAL_SHM;
      if(m_connType=="PIPE"){
	chanType=yampl::LOCAL_PIPE;
      }else if(m_connType=="ZMQ"){
	chanType=yampl::DISTRIBUTED;    
      }
      if(m_commChannelSend.empty())return false;
      if(m_commChannelRecv.empty())return false;

      if(chanType!=yampl::DISTRIBUTED && m_useUID){
	snprintf(buff,1000,"%s_%u",m_commChannelSend.c_str(),userUID);
      }else{
	snprintf(buff,1000,"%s",m_commChannelSend.c_str());
      }
      ATH_MSG_INFO("Send channel = "<<buff);
      yampl::Channel channelSend(buff,chanType);
      if(chanType!=yampl::DISTRIBUTED && m_useUID){
	snprintf(buff,1000,"%s_%u",m_commChannelRecv.c_str(),userUID);
      }else{
	snprintf(buff,1000,"%s",m_commChannelRecv.c_str());
      }
      ATH_MSG_INFO("Receive channel = "<<buff);
      yampl::Channel channelRecv(buff,chanType);      
      yampl::SocketFactory factory;
      m_sendSock.reset(factory.createClientSocket(channelSend));
      m_recvSock.reset(factory.createClientSocket(channelRecv));
      std::thread *t=new std::thread([&,this](){
	  APE::BufferContainer b(sizeof(int));
	  b.setModule(SERVER_MODULE);//Openning connection;
	  if(postFork){
	    b.setAlgorithm(ALG_OPENING_AFTER_FORKING);//Openning connection;
	  }else{
	    b.setAlgorithm(ALG_OPENING_CONNECTION);//Openning connection;
	  }
	  *(int*)b.getBuffer()=(int)getpid();
	  //ATH_MSG_INFO("Trying to open connections");
	  this->m_sendSock->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
	  this->m_recvSock->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
	  this->m_isConnected=true;
	  this->m_tCond.notify_all();
	  //ATH_MSG_INFO("Connections opened");
	}
	);
      std::unique_lock<std::mutex> lock(m_cMutex);
      ATH_MSG_INFO("Waiting for connections");
      if(m_tCond.wait_for(lock,std::chrono::milliseconds(3000))==std::cv_status::timeout){
	ATH_MSG_ERROR("Error while opening connection to APE! Timeout while trying to open the connection");
	t->detach();
	delete t;
	return false;
      }else{
	ATH_MSG_INFO ("Connection successfully opened to "<<m_commChannelSend);
	if(m_isConnected){
	  t->join();
	  delete t;
	}
	return true;
      }
    }catch(std::exception &ex){
      ATH_MSG_ERROR("Error while opening connection to APE! "<<ex.what());    
      m_isConnected=false;
    }
  }
  return false;
}

bool OffloadSvc::closeCommChannel(bool preFork){
  if(m_isConnected){
    ATH_MSG_INFO ("closing connection to APE server");
    APE::BufferContainer b(sizeof(int));
    b.setModule(SERVER_MODULE);//Openning connection;
    if(preFork){
      b.setAlgorithm(ALG_CLOSING_FOR_FORKING);//Closing connection for reopen;
    }else{
      b.setAlgorithm(ALG_CLOSING_CONNECTION);//Closing connection;
    }
    *(int*)b.getBuffer()=(int)getpid();
    m_sendSock->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
    m_sendSock.reset();
    m_recvSock.reset();
    m_isConnected=false;
    return true;
  }
  return false;
}
