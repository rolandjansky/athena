/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ILOGGEDMESSAGESVC_H
#define ATHENAKERNEL_ILOGGEDMESSAGESVC_H 1

// Include files
#include "GaudiKernel/IMessageSvc.h"
#include <string>
#include <vector>
#include <functional>


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ILoggedMessageSvc(31, 1 , 1); 


/** @class ILoggedMessageSvc ILoggedMessageSvc.h AthenaKernel/ILoggedMessageSvc.h

    Extends IMessageSvc to get logged messages

    @author Charles Leggett
*/
class ILoggedMessageSvc : virtual public IMessageSvc {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILoggedMessageSvc; }

  struct LoggedMessage {
    int level;
    std::string source;
    std::string message;

    LoggedMessage(int l, const std::string& s, const std::string& m):
      level(l),source(s),message(m){};
  };
    

  virtual const std::vector<std::pair<std::string, std::string> >& getMessages( MSG::Level ) const = 0;
  virtual const std::vector< LoggedMessage >& getKeyMessages() const = 0;

  virtual ~ILoggedMessageSvc();


};

#endif // ATHENAKERNEL_ILOGGEDMESSAGESVC_H
