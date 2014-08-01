/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyView/MboyViewServer.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <cstdio>
#include <fstream>
using namespace std;

MboyViewServer::MboyViewServer(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {
  declareProperty("Port", m_Port = 3456);
  m_CurrentLine = 0;
  m_SockFD = 0;
  m_NewSockFD = 0;
}

MboyViewServer::~MboyViewServer() {
}

StatusCode MboyViewServer::initialize() {
  struct sockaddr_in serv_addr, cli_addr;
  m_SockFD = socket(AF_INET, SOCK_STREAM, 0);
  if (m_SockFD < 0) {
    ATH_MSG_FATAL("Failed to open socket : " << strerror(errno));
    return StatusCode::FAILURE;
  }

  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(m_Port);
  if (bind(m_SockFD, (struct sockaddr *) &serv_addr,
	   sizeof(serv_addr)) < 0) {
    ATH_MSG_FATAL("Failed to bind socket : " << strerror(errno));
    return StatusCode::FAILURE;
  } 
  listen(m_SockFD,5);
  socklen_t clilen = sizeof(cli_addr);

  ATH_MSG_INFO("MboyViewServer initialized, waiting for connection");
  m_NewSockFD = accept(m_SockFD, 
		       (struct sockaddr *) &cli_addr, 
		       &clilen);
  if (m_NewSockFD < 0){
    ATH_MSG_FATAL("Cannot accept connection from socket : " << strerror(errno));
    return StatusCode::FAILURE;
  } 


  return StatusCode::SUCCESS;
}

StatusCode MboyViewServer::get(std::string& message) {
  memset(m_Buffer, 0, BUFSIZE);
  int n = read(m_NewSockFD,m_Buffer,BUFSIZE-1);
  if (n<0) {
    ATH_MSG_FATAL("Cannot read from socket : " << strerror(errno));
    return StatusCode::FAILURE;
  }
  // TODO: if (n==BUFSIZE-1) call get() again and concatenate the
  // strings?
  message = m_Buffer;
  return StatusCode::SUCCESS;
}

StatusCode MboyViewServer::put(const std::string& message) {
  int n = write(m_NewSockFD, message.c_str(), message.size());
  if (n < 0) {
    ATH_MSG_FATAL("Cannot write to socket : " << strerror(errno));
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode MboyViewServer::putNextEvent() {
  // Quick and dirty code to retrieve the ASCII dump based on
  // files. This part has to be rewritten.
  ifstream in("Out.MboyView");
  string line;
  StatusCode sc;
  int iline=0;
  while (getline(in,line)) {
    iline++;
    if (iline<=m_CurrentLine) continue;
    sc = put(line+"\n");
    if (sc != StatusCode::SUCCESS) return sc;
    if (line.find("END EVT")==0) break;
  }
  in.close();
  m_CurrentLine = 0;
  //unlink("Out.MboyView");
  return StatusCode::SUCCESS;
}

StatusCode MboyViewServer::execute() {
  std::string message;
  StatusCode sc;
  while (true) {
    ATH_MSG_INFO("MboyViewServer waiting for command");
    sc = get(message);
    if (sc!=StatusCode::SUCCESS) return sc;
    ATH_MSG_INFO("Command received is " << message);
    // wait for the command "NEXT EVT"
    if (message.find("NEXT EVT") == 0) {
      //sc = put("hello world\n");
      sc = putNextEvent();
      if (sc != StatusCode::SUCCESS) return sc;
      return StatusCode::SUCCESS;
    }
  }
}

StatusCode MboyViewServer::finalize() {
  return StatusCode::SUCCESS;
}
