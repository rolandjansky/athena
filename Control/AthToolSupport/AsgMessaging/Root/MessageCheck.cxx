/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/MessageCheckLocal.h>

#include <cassert>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

#ifndef XAOD_STANDALONE
#include <GaudiKernel/Bootstrap.h>
#include <GaudiKernel/ISvcLocator.h>
#endif

//
// method implementations
//

namespace asg
{
  namespace
  {
    /// \brief the map of all package-level message streams
    ///
    /// This is mostly to allow configuring the package-level message
    /// streams centrally, i.e. they can be looked up and set by name
    /// centrally (and from python).  This delivers on one of the
    /// central promises we have for our messaging, that the message
    /// level can be adjusted at configuration time.
    ///
    /// There is a bit of an issue that users still need to know the
    /// name of the message streams they want to configure, and that
    /// there is no error if they have a typo in the names they
    /// configure, but it is better than nothing.
    std::unordered_map<std::string,MsgStream> packageMsgStreamMap;

    /// \brief the mutex for \ref packageMsgStreamMap
    ///
    /// While this code is currently (20 Aug 19) not being used in a
    /// multi-threaded environment, it is expected that it will be
    /// used in such an environment eventually.
    ///
    /// Note that the locking shouldn't happen very often, most users
    /// will store a reference to the message stream in a static
    /// variable.
    std::recursive_mutex packageMsgMutex;



#ifndef XAOD_STANDALONE
    /// Get the Athena message service
    /// TODO: Look into using AthenaKernel/MsgStreamMember.h
    IMessageSvc* getMessageSvcAthena()
    {
      static IMessageSvc* msgSvc
	= Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc");
      return msgSvc;
    }
#endif
  }



  MsgStream& packageMsgStream (const std::string& package)
  {
    std::lock_guard<std::recursive_mutex> lock {packageMsgMutex};
    auto iter = packageMsgStreamMap.find (package);
    if (iter != packageMsgStreamMap.end())
      return iter->second;
    const std::string fullName {"Package." + package};
#ifdef XAOD_STANDALONE
    auto result = packageMsgStreamMap.emplace (package, fullName);
#else
    auto result = packageMsgStreamMap.emplace
      (std::piecewise_construct,
       std::forward_as_tuple (package),
       std::forward_as_tuple (::asg::detail::getMessageSvcAthena(), fullName));
#endif
    assert (result.second);
    return result.first->second;
  }



  void setPackageMsgLevel (const std::string& package, MSG::Level level)
  {
    packageMsgStream (package).setLevel (level);
  }



  void printAllPackageMsgLevels ()
  {
    using namespace msgAsgMessaging;
    std::lock_guard<std::recursive_mutex> lock {packageMsgMutex};
    ANA_MSG_INFO ("package message levels:");
    for (auto& msgStream : packageMsgStreamMap)
    {
      ANA_MSG_INFO ("  package=" << msgStream.first << " level=" << name (msgStream.second.level()) << "(" << unsigned (msgStream.second.level()) << ")");
    }
  }



  ANA_MSG_SOURCE (msgUserCode, "UserCode")

  namespace detail
  {
    void throw_check_fail (const std::string& str)
    {
      throw std::runtime_error (str);
    }
  }
}
