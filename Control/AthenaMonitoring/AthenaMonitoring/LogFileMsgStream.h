/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LOGFILEMSGSTREAM_H
#define LOGFILEMSGSTREAM_H

// ********************************************************************
//
// NAME:     LogFileMsgStream.H
//
// DESCRIPTION: class for piping output to logfile and MessageSvc
//
// AUTHOR:   Rolf Seuster  27.10.2005:  1st implementation
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Message.h"

#include <fstream>
#include <string>

class LogFileMsgStream : public MsgStream {
 public:
  // constructor
  LogFileMsgStream(IMessageSvc* svc,
		   const std::string& source,
		   const std::string& filename,
		   bool startlogging ) :
    MsgStream(svc, source), m_FormatString("")
    {
      if(startlogging)
	m_logfile.open(filename.c_str(),
		       std::ofstream::out | std::ofstream::app);
    };
  
  // destructor
  ~LogFileMsgStream() {
    if(m_logfile.is_open())
      m_logfile.close();
  };
  
  /// Accept MsgStream activation using MsgStreamer operator
  // almost plain copy from MsgStream.h
  LogFileMsgStream& operator<< (MSG::Level level)  {   
    return (LogFileMsgStream&)MsgStream::report(level);
  }
  
  /// General templated stream operator
  //  template <typename T>
  //  copied from MsgStream.h, doesn't work for all types (vectors)
  //  but that deficit doesn't harm
  template <class T>
    MsgStream& operator<< (const T& arg)  {
    if(isActive()) m_stream << arg; 
    return *this;
  }
  
  // copied from MsgStream.cpp
  //  here we copy the output to a file as well
  MsgStream& doOutput() {
    Message msg(m_source,m_currLevel,m_stream.str());
    if(m_FormatString!="")
      msg.setFormat(m_FormatString);
    if(m_logfile.is_open())
      m_logfile << msg << "\n";
    return MsgStream::doOutput();
  }
  
  void setFormat(const std::string& str) {
    m_FormatString=str;
  }
  
 private:
  // default constructor, unusable !
  LogFileMsgStream();
  
  std::ofstream m_logfile;
  
  std::string m_FormatString;
  
};

#endif
