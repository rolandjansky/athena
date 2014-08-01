/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "AthenaBaseComps/AthAlgorithm.h"

  /**
   @class MboyViewServer

   This class implements a simple TCP/IP server that serves events in
   Out.Mboyview format.

   This is the simplest idea of a server. The protocol is the
   following:

   client sends "NEXT EVT"
   server sends the Out.Mboyview dump of the next event.

   The sever code is written in pure C, accepts only one connection,
   and lacks some check for figuring is client is still alive...

  @author samusog@cern.ch
  
  */

class MboyViewServer : public AthAlgorithm {
public:
  MboyViewServer(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MboyViewServer();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  int m_Port;
  int m_SockFD;
  int m_NewSockFD;
  static const int BUFSIZE=1024;
  char m_Buffer[BUFSIZE];
  int m_CurrentLine;

  StatusCode put(const std::string& message);
  StatusCode get(std::string& message);
  StatusCode putNextEvent();

};
