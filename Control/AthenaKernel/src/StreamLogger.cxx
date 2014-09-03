/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/StreamLogger.h"

#include "GaudiKernel/IMessageSvc.h"

#include <stdexcept>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////
//
// Constructor for a file

StreamLogger::StreamLogger(const std::string& file): m_isMine(1), 
						     p_ost(0), p_msgStr(0),
                                                     m_level (MSG::INFO)
{
  m_name = "file:" + file;
  p_ost = new std::ofstream(file.c_str());
  if (p_ost == 0) {
    throw std::invalid_argument("can't open file");
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Constructor for MsgStream

StreamLogger::StreamLogger(IMessageSvc *svc, MSG::Level lev, 
			   const std::string& msgName): 
  m_isMine(1), p_ost(0), p_msgStr(0) {
  p_msgStr = new MsgStream( svc, msgName);
  m_level = lev;
  m_name = "MsgStream";
}

/////////////////////////////////////////////////////////////////////////////
//
// Constructor for an ostream

StreamLogger::StreamLogger(std::ostream& ost): m_isMine(0) , p_ost(0), 
					       p_msgStr(0),
                                               m_level (MSG::INFO)
{
  p_ost = &ost;
  if (ost == std::cerr) {
    m_name = "STDERR";
  } else if ( ost == std::cout) {
    m_name = "STDOUT";
  } else {
    m_name = "unknown ostream";
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor

StreamLogger::~StreamLogger() {
  if (m_isMine && p_ost != 0) {
    delete p_ost;
  }
  if (p_msgStr != 0) {
    delete p_msgStr;
  }
}
