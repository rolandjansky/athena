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

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// OffloadManagerSvc includes
#include "OffloadSvc/IOffloadSvc.h"
#include "yampl/SocketFactory.h"


// Forward declaration
template <class TT> class SvcFactory;
namespace APE{
  class BufferContainer;
}

class OffloadSvc : virtual public IOffloadSvc, public AthService{ 
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

  // /** The very important message of the day
  //  */
  // StatusCode qotd( std::string& quote );
  virtual StatusCode sendData(std::unique_ptr<APE::BufferContainer> &buff, int &token);
  virtual StatusCode receiveData(std::unique_ptr<APE::BufferContainer> &buff, int token, int timeOut=-1);
  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// Default constructor: 
  OffloadSvc();

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

  std::string m_connName;
  bool m_isConnected;
  std::map<int,OffloadSvc::TransferStats> m_stats;
  yampl::ISocket *m_mySocket,*m_downSock;
  std::queue<int> m_tokens;
  int m_maxTokens;
  std::condition_variable m_tCond;
  std::mutex m_cMutex;
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
