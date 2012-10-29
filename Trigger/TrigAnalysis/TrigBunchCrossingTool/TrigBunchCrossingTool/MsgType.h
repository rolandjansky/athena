// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgType.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_MSGTYPE_H
#define TRIGBUNCHCROSSINGTOOL_MSGTYPE_H

namespace Trig {

   /// Message type enumeration
   /**
    * Enumeration for classifying messages. Based on this classification,
    * SLogWriter can decide if/how the message should be shown.
    * (Naming taken from ATLAS offline.)
    */
   enum MsgType {
      VERBOSE = 1, /**< Type for the most detailed messages. Only for serious debugging. */
      DEBUG = 2,   /**< Type for debugging messages. A few messages per event allowed. */
      INFO = 3,    /**< Type for normal information messages. */
      WARNING = 4, /**< Type for smaller problems. */
      ERROR = 5,   /**< Type for "real" problems. */
      FATAL = 6,   /**< Type for problems that should halt the execution. */
      ALWAYS = 7   /**< Type that should always be shown. (Not really used.) */
   };

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_MSGTYPE_H
