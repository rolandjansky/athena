/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_STREAMLOGGER_H
#define ATHENAKERNEL_STREAMLOGGER_H 1

/** 
 * @file StreamLogger.h
 * @brief provide entry point for binding output streams.
 *
 * @author Charles Leggett
 * $Id: StreamLogger.h,v 1.2 2007-06-14 01:57:23 calaf Exp $
 */

#include "GaudiKernel/MsgStream.h"

#include <ostream>
#include <string>

class IMessageSvc;

/** 
 * @class StreamLogger
 * @brief provide entry point for binding output streams.
 * @details bind with
 * @code
 *  boost::function<void (const std::string&)> streamLog;
 * @endcode
 * for MsgSvc:
 * @code
 *  StreamLogger *logger = new StreamLogger(msgSvc(), MSG::LEVEL, MsgName);
 *  boost::bind(&StreamLogger::WriteToMsgSvc, logger, _1);
 * @endcode
 * for stderr, stdout, etc
 * @code
 *  StreamLogger *logger = new StreamLogger(std::cerr);
 *  streamLog = boost::bind(&StreamLogger::WriteToStream, logger, _1)
 * @endcode
 * for file
 * @code
 *  StreamLogger *logger = new StreamLogger(fileName);
 *  streamLog = boost::bind(&StreamLogger::WriteToStream, logger, _1);
 * @endcode
 * to write out:
 * @code
 *  streamLog("output");
 * @endcode
 */
class StreamLogger {
public:
  StreamLogger(const std::string& file);
  StreamLogger(std::ostream &ost);
  StreamLogger(IMessageSvc*, MSG::Level, 
	       const std::string& msgName="StreamLogger");
  ~StreamLogger();

  std::string name() const { return m_name; }
  
  void WriteToStream(const std::string& str) { *p_ost << str << std::endl; }
  void WriteToMsgSvc(const std::string& str) { *p_msgStr << m_level << str 
							 << endmsg; }
  
private:
  bool m_isMine;
  std::ostream *p_ost;
  MsgStream *p_msgStr;
  MSG::Level m_level;
  std::string m_name;
};


#endif
