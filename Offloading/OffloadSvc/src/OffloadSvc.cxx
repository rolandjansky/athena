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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <random>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "uuid/uuid.h"


// Constructors
////////////////
void nullDealloc(void*,void*){}

OffloadSvc::OffloadSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService ( name, pSvcLocator ),m_evtStore("StoreGateSvc/StoreGateSvc",name){
  declareProperty( "ConnectionType",
		   m_connType = "SHM", "Connection type [SHM,PIPE,ZMQ]");
  declareProperty( "CommChannelSend",
		   m_commChannelSend = "apeSock", "Name of the APE socket for sending request");
  declareProperty( "CommChannelRecv",
		   m_commChannelRecv = "apeSock_upload", "Name of the APE socket for receiving results");
  declareProperty( "CommChannelUserUID",
		   m_useUID=true, "Whether to add uid to comm channels");
  declareProperty( "ForkDelay",
		   m_forkDelay=0, "Maximum of random range that will be waited before openning connection to server ");
  declareProperty( "DumpOffloadRequests",
		   m_dumpOffloadRequests=false, "Whether to write offload requests to a file");
  declareProperty( "DumpReplies",
		   m_dumpReplies=false, "Whether to write responses to a file");
  m_isConnected=false;
  m_sendSock=0;
  m_recvSock=0;
  m_maxTokens=0;
  m_currEvt=0;
  //std::cout<<__PRETTY_FUNCTION__<<"Constructing Service SAMI"<<std::endl;
  m_evtStore=ServiceHandle<StoreGateSvc>("StoreGateSvc",name);
  m_fact=0;

}

// Destructor
///////////////
OffloadSvc::~OffloadSvc(){
  
}

StatusCode OffloadSvc::sendData(std::unique_ptr<APE::BufferContainer> &buff, int &token,bool requiresResponse){
  msg(MSG::VERBOSE)<<"Offload request! pid="<<getpid()<<endmsg;
  if(!m_isConnected){
    msg(MSG::WARNING)<<"APE Server communication was not open while offload was requested. Initiating communication!"<<endmsg;
    bool rc=openCommChannel();
    if(!rc){
      msg(MSG::FATAL)<<"APE Server communication Failed"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  if(!buff){
    msg(MSG::ERROR)<<"BufferContainer is 0 "<<endmsg;
    return StatusCode::FAILURE;
  }
  if(!m_isConnected){
    msg(MSG::ERROR)<<"Accelerator Process Extension connection is not open!"<<endmsg;
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
	   <<" needsReply="<<requiresResponse
	   <<endmsg;
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
  if(m_dumpOffloadRequests){
    const EventInfo* eventInfo;
    StatusCode scEv = m_evtStore->retrieve(eventInfo);
    uint64_t eventNumber=0;
    if (scEv.isFailure()) {
      ATH_MSG_WARNING( "Could not retrieve event info" );
    }else{
      eventNumber = eventInfo->event_ID()->event_number();
    }
    if(m_currEvt!=eventNumber){
      m_currEvt=eventNumber;
      m_requestNumber=0;
    }
    ATH_MSG_DEBUG(" Event " << eventNumber);    
    char fnamBuff[1024];
    snprintf(fnamBuff,1024,"Req-%08ld-%d-%d-%d-%d.dat",eventNumber,
	     (buff->getModule()),
	     (buff->getAlgorithm()),
	     m_requestNumber,(requiresResponse?1:0));
    int fd=open(fnamBuff,O_CREAT|O_TRUNC|O_WRONLY,(S_IRWXU^S_IXUSR)|(S_IRWXG^S_IXGRP)|(S_IRWXO^S_IXOTH));
    write(fd,APE::BufferAccessor::getRawBuffer(*buff),buff->getTransferSize());
    close(fd);
    m_requestNumber++;
  }

  return StatusCode::SUCCESS;  
}

StatusCode OffloadSvc::receiveData(std::unique_ptr<APE::BufferContainer> &buff, int token, int timeOut){
  if(!m_isConnected){
    msg(MSG::WARNING)<<"APE Server communication was not open during data request. Initiating communication"<<endmsg;
    bool rc=openCommChannel();
    if(!rc){
      msg(MSG::FATAL)<<"APE Server communication Failed"<<endmsg;
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
      ATH_MSG_WARNING("Received possibly corrupt data. Trying again"<<getpid());
      recvdSize=m_recvSock->tryRecv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff),timeOut);      
    }
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_ERROR("Received corrupt data on both trials");
      return StatusCode::FAILURE;
    }
  }else{
    recvdSize=m_recvSock->recv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff));
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_WARNING("Received possibly corrupt data. Trying again "<<getpid());
      recvdSize=m_recvSock->recv(rawBuffer,APE::BufferAccessor::getRawBufferSize(*buff));
    }
    if(recvdSize < static_cast<ssize_t>(sizeof(APE::APEHeaders))){
      ATH_MSG_ERROR("Received corrupt data on both trials "<<getpid());
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
	   <<endmsg;
  
  auto it=m_stats.find(buff->getToken());
  if(m_dumpReplies){
    const EventInfo* eventInfo;
    StatusCode scEv = m_evtStore->retrieve(eventInfo);
    uint64_t eventNumber = 0;
    if (scEv.isFailure()) {
      ATH_MSG_WARNING( "Could not retrieve event info" );
    }else{
      eventNumber=eventInfo->event_ID()->event_number();
    }
    ATH_MSG_DEBUG(" Event " << eventNumber);    
    char fnamBuff[1024];
    snprintf(fnamBuff,1024,"Resp-%08ld-%d-%d.dat",eventNumber,
	     buff->getModule(),
	     buff->getAlgorithm()
	     );
    int fd=open(fnamBuff,O_CREAT|O_TRUNC|O_WRONLY,(S_IRWXU^S_IXUSR)|(S_IRWXG^S_IXGRP)|(S_IRWXO^S_IXOTH));
    write(fd,APE::BufferAccessor::getRawBuffer(*buff),buff->getTransferSize());
    close(fd);    
  }
  if(it==m_stats.end()){
    msg(MSG::ERROR)
      <<" APE returned token "<<buff->getToken()
      <<" However this token was not sent by this process!"
      <<endmsg;
    return StatusCode::FAILURE;
  }else{
    const TransferStats &ts=it->second;
    auto diff=recvEnd-it->second.sendStart;
    msg(MSG::DEBUG)<<"Send request took "<<std::chrono::duration_cast<std::chrono::microseconds>(ts.sendEnd-ts.sendStart).count()
		   <<"(usec). "
		   <<"Time between send and receive requests of token "
		   <<buff->getToken()<<" is "<<std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
		   <<"(ms). "
		   <<"Total transfer size= "<<ts.uploadSize<<" + "<<recvdSize<<"= "<< ts.uploadSize+recvdSize
		   <<" bytes. pid="<<getpid() 
		   <<endmsg;
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
  p_incSvc->addListener(this,"PostFork",100000l);//do it first
  p_incSvc->addListener(this,"BeforeFork",-10000l);// do it last

  return sc;
}

StatusCode OffloadSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  bool rv=true;
  if(m_isConnected){
    rv=closeCommChannel();
  }
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
  ATH_MSG_INFO("Got incident "<<inc.type()<<" from "<<inc.source()<<" pid="<<getpid());
  if(inc.type()=="BeginRun"){
    
  }else if(inc.type()=="BeforeFork"){
    if(m_isConnected){
      closeCommChannel(true);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }else if(inc.type()=="PostFork"){
    if(!m_isConnected){
      if(m_forkDelay>0){
	std::random_device rgen;
	std::mt19937 rEngine(rgen());//seed with a real number possibly from /dev/urandom 
	std::uniform_int_distribution<unsigned int> uniform_dist(0,((m_forkDelay>0)?m_forkDelay:1000));
	std::this_thread::sleep_for(std::chrono::milliseconds(uniform_dist(rEngine)));
      }
      openCommChannel(true);	
    }
  }
}

bool OffloadSvc::openCommChannel(bool postFork){
  if(!m_isConnected){
    try{
      if(!m_fact)m_fact=new yampl::SocketFactory();
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
      uuid_t uuid;
      uuid_clear(uuid);
      char uuidbuff[40];
      uuid_generate(uuid);
      uuid_unparse(uuid,uuidbuff);
      uuidbuff[36]='\0';
      std::string uuids(uuidbuff);
      ATH_MSG_INFO("Send channel = "<<buff<<" uuid="<<uuids << " pid="<<getpid());
      yampl::Channel channelSend(buff,chanType);
      if(chanType!=yampl::DISTRIBUTED && m_useUID){
	snprintf(buff,1000,"%s_%u",m_commChannelRecv.c_str(),userUID);
      }else{
	snprintf(buff,1000,"%s",m_commChannelRecv.c_str());
      }
      m_sendSock.reset(m_fact->createClientSocket(channelSend,yampl::COPY_DATA,nullDealloc,uuids));
      ATH_MSG_INFO("Receive channel = "<<buff);
      yampl::Channel channelRecv(buff,chanType);      
      //yampl::SocketFactory factory;
      uuid_clear(uuid);
      uuid_generate(uuid);
      uuid_unparse(uuid,uuidbuff);
      uuidbuff[36]='\0';
      ATH_MSG_INFO("Receive channel = "<<buff<<" uuid="<<uuidbuff<<" pid="<<getpid());
      m_recvSock.reset(m_fact->createClientSocket(channelRecv,yampl::COPY_DATA,nullDealloc,std::string(uuidbuff)));
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
	  try{
	    this->m_sendSock->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
	    this->m_recvSock->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
	    this->m_isConnected=true;
	    this->m_tCond.notify_all();
	  }catch(std::exception &ex){
	    ATH_MSG_ERROR("Caught exception when trying to open connection to APE server. Pid="<<getpid()<<" "<<ex.what());
	  }
	  //ATH_MSG_INFO("Connections opened");
	}
	);
      std::unique_lock<std::mutex> lock(m_cMutex);
      ATH_MSG_INFO("Waiting for connections "<<getpid());
      if(m_tCond.wait_for(lock,std::chrono::milliseconds(5000))==std::cv_status::timeout){
	ATH_MSG_ERROR("Error while opening connection to APE! Timeout while trying to open the connection"
		      <<" pid="<<(int)getpid());
	t->detach();
	delete t;
	return false;
      }else{
	ATH_MSG_INFO ("Connection successfully opened to "<<m_commChannelSend<<" pid="<<(int)getpid());
	if(m_isConnected){
	  t->join();
	  delete t;
	}
	return true;
      }
    }catch(std::exception &ex){
      ATH_MSG_ERROR("Error while opening connection to APE! pid= "<<getpid()<<" "<<ex.what());    
      m_isConnected=false;
    }
  }
  return false;
}

bool OffloadSvc::closeCommChannel(bool preFork){
  if(m_isConnected){
    ATH_MSG_INFO ("closing connection to APE server "<<getpid());
    APE::BufferContainer b(sizeof(int));
    b.setModule(SERVER_MODULE);//Openning connection;
    if(preFork){
      b.setAlgorithm(ALG_CLOSING_FOR_FORKING);//Closing connection for reopen;
    }else{
      b.setAlgorithm(ALG_CLOSING_CONNECTION);//Closing connection;
    }
    *(int*)b.getBuffer()=(int)getpid();
    m_sendSock->send(APE::BufferAccessor::getRawBuffer(b),b.getTransferSize());
    ATH_MSG_INFO("Sleeping for 1s to ensure closing of connection ");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    m_sendSock.reset();
    m_recvSock.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    m_isConnected=false;
    delete m_fact;
    m_fact=0;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return true;
  }
  return false;
}
