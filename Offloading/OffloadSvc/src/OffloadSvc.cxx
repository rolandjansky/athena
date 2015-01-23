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

// OffloadManagerSvc includes
#include "OffloadSvc/OffloadSvc.h"
#include "APE/BufferContainer.hpp"
#include "APE/BufferAccessor.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <chrono>

// Constructors
////////////////
OffloadSvc::OffloadSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService ( name, pSvcLocator ){
  declareProperty( "ConnectionName",
		   m_connName = "apeSock", "Name of the APE socket");
  m_isConnected=false;
  m_mySocket=0;
  m_downSock=0;
  m_maxTokens=0;

  //std::cout<<__PRETTY_FUNCTION__<<"Constructing Service SAMI"<<std::endl;
}

// Destructor
///////////////
OffloadSvc::~OffloadSvc(){
}

StatusCode OffloadSvc::sendData(std::unique_ptr<APE::BufferContainer> &buff, int &token){
  //std::cout<<__PRETTY_FUNCTION__<<"Received SendData SAMI"<<std::endl;
  // MsgStream athenaLog(msgSvc(), name());
  if(!buff){
    msg(MSG::ERROR)<<"BufferContainer is 0 "<<endreq;
    return StatusCode::FAILURE;
  }
  if(!m_isConnected){
    msg(MSG::ERROR)<<"Accelerator Process Extension connection is not open!"<<endreq;
    return StatusCode::FAILURE;
  }
  //msg(MSG::DEBUG)<<"Received sendData request with algorithm ="<<buff->getAlgorithm()<<" and payloadsize="<<buff->getPayloadSize()<<" totalSize="<<buff->getTransferSize());
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
  TransferStats ts;
  
  ts.sendStart=std::chrono::system_clock::now();
  m_mySocket->send(APE::BufferAccessor::getRawBuffer(*buff),buff->getTransferSize());
  ts.sendEnd=std::chrono::system_clock::now();
  ts.uploadSize=buff->getTransferSize();
  ts.downloadSize = 0;
  m_stats[token]=ts;
  return StatusCode::SUCCESS;  
}

StatusCode OffloadSvc::receiveData(std::unique_ptr<APE::BufferContainer> &buff, int token, int timeOut){
  ssize_t recvdSize=0;
  //MsgStream athenaLog(msgSvc(), name());

  void* rawBuffer=APE::BufferAccessor::getRawBuffer(*buff);
  if(timeOut>0){
    ATH_MSG_WARNING("Timeout parameter might not be implemented yet"); 
    recvdSize=m_downSock->tryRecv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff),timeOut);
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_WARNING("Received possibly corrupt data. Trying again");
      recvdSize=m_downSock->tryRecv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff),timeOut);      
    }
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_ERROR("Received corrupt data on both trials");      
      return StatusCode::FAILURE;
    }
  }else{
    recvdSize=m_downSock->recv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff));
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_WARNING("Received possibly corrupt data. Trying again");
      recvdSize=m_downSock->recv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff));
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
  try{
    char buff[1000];
    uid_t userUID=geteuid();
    snprintf(buff,1000,"%s_%u",m_connName.c_str(),userUID);
    ATH_MSG_INFO ("Upload connection Name=: [" << buff << "]");
    yampl::Channel apeChannel(buff,yampl::LOCAL_SHM);
    snprintf(buff,1000,"%s_%u_upload",m_connName.c_str(),userUID);
    ATH_MSG_INFO ("download connection Name=: [" << buff << "]");
    yampl::Channel downChannel(buff,yampl::LOCAL_SHM);
    yampl::SocketFactory sf;
    m_mySocket=sf.createClientSocket(apeChannel);
    m_downSock=sf.createClientSocket(downChannel);
    std::thread *t=new std::thread([&,this](){
	APE::BufferContainer b(sizeof(int));
	b.setModule(0xffffffff);//Openning connection;
	b.setAlgorithm(0xefffffff);//Openning connection;
	*(int*)b.getBuffer()=(int)getpid();
	//ATH_MSG_INFO("Trying to open connections");
	this->m_mySocket->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
	this->m_downSock->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
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
      return StatusCode::FAILURE;
    }else{
      ATH_MSG_INFO ("Connection successfully opened");
      if(m_isConnected){
	t->join();
	delete t;
      }
      return StatusCode::SUCCESS;
    }
  }catch(std::exception &ex){
    ATH_MSG_ERROR("Error while opening connection to APE! "<<ex.what());    
    m_isConnected=false;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO ("Connection successfully opened");
  return StatusCode::SUCCESS;
}

StatusCode OffloadSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  if(m_isConnected){
    ATH_MSG_INFO ("closing connection to APE server");
    APE::BufferContainer b(sizeof(int));
    b.setModule(0xffffffff);//Openning connection;
    b.setAlgorithm(0xffffffff);//Closing connection;
    *(int*)b.getBuffer()=(int)getpid();
    m_mySocket->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
    delete m_mySocket;
    delete m_downSock;
    m_mySocket=0;
    m_downSock=0;
  }
  return StatusCode::SUCCESS;
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

