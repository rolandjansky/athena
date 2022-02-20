/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaYamplTool.h"

#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IChronoStatSvc.h"

//#include "EventStorage/EventStorageRecords.h"

#include "yampl/SocketFactory.h"
#include <sstream>

namespace {
struct ShareEventHeader {
   long evtSeqNumber;
   long fileSeqNumber;
   std::size_t evtSize;
   std::size_t evtOffset;
   unsigned int evtCoreStatusFlag;
   uint32_t evtTerm1;
   uint32_t evtTerm2;
};
} // anonymous namespace

//___________________________________________________________________________
AthenaYamplTool::AthenaYamplTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)  
  : AthAlgTool(type, name, parent)
  ,  m_fileSeqNumber(0)
  ,  m_isServer(false)
  ,  m_isClient(false)
  ,  m_many2one(true)
  ,  m_chronoStatSvc("ChronoStatSvc", name)
  ,  m_incidentSvc("IncidentSvc", name)
  ,  m_socketFactory(nullptr)
  ,  m_clientSocket(nullptr)
  ,  m_serverSocket(nullptr)
{
  declareProperty("ChannelName", m_channel = name);
  declareProperty("Many2One", m_many2one);
  declareInterface<IAthenaIPCTool>(this);
}

//___________________________________________________________________________
AthenaYamplTool::~AthenaYamplTool() {
   delete m_clientSocket;
   delete m_serverSocket;
   delete m_socketFactory;
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::initialize() {
   ATH_MSG_INFO("Initializing " << name());

   // Retrieve ChronoStatSvc
   ATH_CHECK( m_chronoStatSvc.retrieve() );
   // Retrieve IncidentSvc
   ATH_CHECK( m_incidentSvc.retrieve() );

   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::finalize() {
   ATH_MSG_INFO("in finalize()");
   // Release ChronoStatSvc
   if (!m_chronoStatSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ChronoStatSvc.");
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::makeServer(int /*num*/, const std::string& /*streamPortSuffix*/) {
   ATH_MSG_DEBUG("Creating Yampl channel on the Shared Reader side");
   if (m_isServer || m_isClient) {
      ATH_MSG_ERROR("Cannot make AthenaYamplTool a Server.");
      return(StatusCode::FAILURE);
   }
   m_isServer = true;
   m_socketFactory = new yampl::SocketFactory();
   m_serverSocket = m_socketFactory->createServerSocket(yampl::Channel(m_channel.value(),yampl::LOCAL_PIPE),yampl::MOVE_DATA); 
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaYamplTool::isServer() const {
   return(m_isServer);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::makeClient(int /*num*/, std::string& /*streamPortSuffix*/) {
   ATH_MSG_DEBUG("Creating Yampl channel on the Event Processor side");
   if (m_isServer || m_isClient) {
      ATH_MSG_ERROR("Cannot make AthenaYamplTool a Client.");
      return(StatusCode::FAILURE);
   }
   m_isClient = true;
   m_socketFactory = new yampl::SocketFactory();
   if(m_many2one) {
     // We work with the shared reader directly. One shared reader in a separate process. 
     // And many Yampl Tools talking to him from different processes
     m_clientSocket = m_socketFactory->createClientSocket(yampl::Channel(m_channel.value(),yampl::LOCAL_PIPE),yampl::MOVE_DATA);       
     ATH_MSG_DEBUG("Client socket created for Many2One mode");
   }
   else {
     // We work with the TokenProcessor, which resides in the same process.
     // Thus the channel name has to be bound to the PID
     // Also, in this case the socket type is Server
     std::ostringstream pidstr;
     pidstr << getpid();
     std::string socketName = m_channel.value() + std::string("_") + pidstr.str();
     m_clientSocket = m_socketFactory->createServerSocket(yampl::Channel(socketName,yampl::LOCAL_PIPE),yampl::MOVE_DATA);
     ATH_MSG_DEBUG("Server socket created for One2One mode");
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaYamplTool::isClient() const {
   return(m_isClient);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::putEvent(long eventNumber, const void* source, std::size_t nbytes, unsigned int status) const {
   if(!m_serverSocket) {
     ATH_MSG_ERROR("putEvent called when Tool is not a Server!");
     return StatusCode::FAILURE;
   }
   if (source == nullptr && nbytes == 0) {
      ATH_MSG_DEBUG("putEvent got last Event marker");
      return(StatusCode::SUCCESS);
   }
   if (source == nullptr) {
      ATH_MSG_ERROR("putEvent got null source");
      return(StatusCode::FAILURE);
   }

   ShareEventHeader evtH;
   evtH.evtSeqNumber = eventNumber;
   evtH.fileSeqNumber = m_fileSeqNumber;
   evtH.evtSize = nbytes;
   evtH.evtOffset = 0;
   evtH.evtCoreStatusFlag = status;
   evtH.evtTerm1 = *(static_cast<const uint32_t*>(source) + nbytes / sizeof(uint32_t) - 1);
   evtH.evtTerm2 = *(static_cast<const uint32_t*>(source) + nbytes / sizeof(uint32_t) - 2);

   // Prepare message for client
   void* message = malloc(nbytes+sizeof(evtH));
   memcpy(message,(void*)&evtH,sizeof(evtH));
   memcpy((char*)message+sizeof(evtH),source,nbytes);

   // Wait for incoming request
   char *ping = nullptr; // can be something else
   m_serverSocket->recv(ping);
   m_serverSocket->send(message,nbytes+sizeof(evtH));
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::getLockedEvent(void** target, unsigned int& status) const {

  void* receive_message;

  if(m_many2one) {
    // Ping server
    std::ostringstream pidstr;
    pidstr << getpid();
    std::string ping = "Ping from " + pidstr.str();
    void* send_message = malloc(ping.size());
    memcpy(send_message,ping.data(),ping.size());
    m_clientSocket->send(send_message,ping.size());

    // And get the answer
    m_clientSocket->recv(receive_message);
  }
  else {
    m_clientSocket->recv(receive_message);
  }

  ShareEventHeader evtH;
  memcpy((void*)&evtH,receive_message,sizeof(evtH));

  if (evtH.fileSeqNumber != m_fileSeqNumber && m_fileSeqNumber > 0) {
    FileIncident endFileIncident(name(), "EndInputFile", "SHM");
    m_incidentSvc->fireIncident(endFileIncident);
    const_cast<AthenaYamplTool*>(this)->m_fileSeqNumber = evtH.fileSeqNumber;
    FileIncident beginFileIncident(name(), "BeginInputFile", "SHM");
    m_incidentSvc->fireIncident(beginFileIncident);
  }

  std::size_t nbytes = evtH.evtSize;
  char* buf = new char[nbytes];
  memcpy(buf, static_cast<char*>((char*)receive_message+sizeof(evtH))+evtH.evtOffset,nbytes);
  *target = buf;

  if(!m_many2one) {
    std::string pong("Done");
    void* send_message = malloc(pong.size());
    memcpy(send_message,pong.data(),pong.size());
    m_clientSocket->send(send_message,pong.size());
  }

  if (evtH.evtTerm1 != *(static_cast<const uint32_t*>(*target) + nbytes / sizeof(uint32_t) - 1) ||
      evtH.evtTerm2 != *(static_cast<const uint32_t*>(*target) + nbytes / sizeof(uint32_t) - 2)) {
    ATH_MSG_ERROR("Event corrupted by AthenaYamplTool");
    return(StatusCode::FAILURE);
  }
  status = evtH.evtCoreStatusFlag;
  return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::lockEvent(long) const {
  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::putObject(const void*, size_t, int) const {
  return(StatusCode::FAILURE);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::getObject(void**, size_t&, int) const {
  return(StatusCode::FAILURE);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::clearObject(const char**, int&) const {
  return(StatusCode::FAILURE);
}

//___________________________________________________________________________
StatusCode AthenaYamplTool::lockObject(const char*, int) const {
   return(StatusCode::SUCCESS);
}
