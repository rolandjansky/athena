/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//    Use gaudi messaging service when in Athena; use cout when "stand-alone".
//

#ifndef OUTPUTDIRECTOR_H
#define OUTPUTDIRECTOR_H

#ifdef GEOMODEL_STANDALONE_GMX
  #include <iostream>
  #include <string>

  namespace MSG {
    // enum((VERBOSE=1, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS))}
    const std::string PKG_NAME("GeoModelXML-");
    const std::string VERBOSE(PKG_NAME + "VERBOSE: ");
    const std::string DEBUG(PKG_NAME + "DEBUG: ");
    const std::string INFO(PKG_NAME + "INFO: ");
    const std::string WARNING(PKG_NAME + "WARNING: ");
    const std::string ERROR(PKG_NAME + "ERROR: ");
    const std::string FATAL(PKG_NAME + "FATAL: ");
    const std::string ALWAYS(PKG_NAME + "ALWAYS: ");
  }
  // Consider changing this to a longer, less clashing sort of name, in the code
  #define msglog std::cout
  #define endmsg std::endl
  // Nothing to do for stand-alone output; cout is already available.
  #define OUTPUT_STREAM 
#else
  #include "GaudiKernel/ServiceHandle.h"
  #include "GaudiKernel/MsgStream.h"
  #include "GaudiKernel/IMessageSvc.h"
  // Macro OUTPUT_STREAM opens a MsgStream called log; can then write to it.
  // Cannot get ServiceHandle directly into the macro; hide it with a typedef...
  typedef ServiceHandle<IMessageSvc> SvcHndl;
  #define OUTPUT_STREAM SvcHndl msgh("MessageSvc", "GeoModelXml"); MsgStream msglog(&(*msgh), "GeoModelXml")
#endif

#endif // OUTPUTDIRECTOR_H
