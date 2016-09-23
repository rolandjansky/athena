/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/ExceptionHandler.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
#include "eformat/Issue.h"

using namespace HLT;


/*
 * Exception dispatcher
 * See http://www.parashift.com/c++-faq-lite/exceptions.html#faq-17.15
 */

ErrorCode HLT::trigExceptionHandler(MsgStream* log)
{
  try {
    throw;
  }
  catch (GaudiException& ex) {
    if (log) *log << MSG::WARNING << "Caught GAUDI exception: "  << ex.what() << endmsg;
    return  GAUDI_EXCEPTION;
  }
  catch (eformat::Issue& ex) {
    if (log) *log << MSG::WARNING << "Caught eformat issue (data access error): "  << ex.what()
		  << "in: " << ex.context().file_name() << "line: " << ex.context().line_number() << endmsg;
    return EFORMAT_EXCEPTION;
  }
  catch (std::bad_alloc& ex) {
    if (log) *log << MSG::ERROR << "Memory allocation issue caught: " << ex.what() << endmsg;        
    throw;  // this is serious, re-throwing
  }
  catch (std::exception& ex) {
    if (log) *log << MSG::ERROR << "std::exception caught: " << ex.what() << endmsg;    
    return STD_EXCEPTION;
  }
  catch (...) {
    if (log) *log << MSG::ERROR << "Unknown exception caught" << endmsg;
    return UNKNOWN_EXCEPTION;
  } 
}
