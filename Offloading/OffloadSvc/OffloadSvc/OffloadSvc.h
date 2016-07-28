// -------------------- -*- C++ -*- --------------------

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// OffloadSvc.h 
// Header file for class OffloadSvc
// Author: Sami Kama <sami.kama@cern.ch>
// -----------------------------------------------------
#ifndef OFFLOADSERVICE_OFFLOADSVC_H 
#define OFFLOADSERVICE_OFFLOADSVC_H 

// STL includes
#include <string>
#include <queue>
#include <chrono>
#include <map>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <memory>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
// OffloadManagerSvc includes
#include "OffloadSvc/IOffloadSvc.h"
#include "yampl/SocketFactory.h"
#include "StoreGate/StoreGateSvc.h"


// Forward declaration
template <class TT> class SvcFactory;
namespace APE{
  class BufferContainer;
}
class IIncidentSvc;

class OffloadSvc : virtual public IOffloadSvc, public AthService,public virtual IIncidentListener{ 
protected:
  friend class SvcFactory<OffloadSvc>;

public: 

  /// Constructor with parameters: 
  OffloadSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~OffloadSvc(); 

  /// Gaudi Service Implementation
  //@{

  StatusCode initialize();
  StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  static const InterfaceID& interfaceID();

  virtual StatusCode sendData(std::unique_ptr<APE::BufferContainer> &buff, int &token,bool requiresResponse=true);
  virtual StatusCode receiveData(std::unique_ptr<APE::BufferContainer> &buff, int token, int timeOut=-1);
  virtual void handle(const Incident &);
  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// Default constructor: 
  OffloadSvc();
  bool openCommChannel(bool postFork=false);
  bool closeCommChannel(bool preFork=false);
  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// The quote of the day
  //StringProperty m_qotd;
  struct TransferStats{
    std::chrono::system_clock::time_point sendStart;
    std::chrono::system_clock::time_point sendEnd;
    size_t uploadSize;
    size_t downloadSize;
  };
  std::string m_connType;
  std::string m_commChannelSend;
  std::string m_commChannelRecv;
  uint m_forkDelay;
  bool m_useUID;
  bool m_isConnected;
  bool m_dumpOffloadRequests;
  bool m_dumpReplies;
  std::map<int,OffloadSvc::TransferStats> m_stats;
  std::shared_ptr<yampl::ISocket> m_sendSock,m_recvSock;
  std::queue<int> m_tokens;
  int m_maxTokens;
  std::condition_variable m_tCond;
  std::mutex m_cMutex;
  uint64_t m_currEvt;
  int m_requestNumber;
  ServiceHandle<StoreGateSvc> m_evtStore;
  yampl::ISocketFactory *m_fact;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& OffloadSvc::interfaceID() 
{ 
  return IOffloadSvc::interfaceID(); 
}

#endif 
