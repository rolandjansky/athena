/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_MSGLOGGER_H
#define TRIGT1CTP_MSGLOGGER_H

// STL include(s):
#include <string>

// Gaudi include(s):
#include "GaudiKernel/IMessageSvc.h"

namespace LVL1CTP {

   // Forward declaration(s) in namespace LVL1CTP
   class MsgWriter;

   /**
    *
    * @short   This class should be used to do all printouts in the simulation.
    *
    *          One shouldn't use the standard output in the simulation,
    *          altough it's still used in some places. Instead this class
    *          should be used, as it will automatically redirect the
    *          messages to the correct output.
    *
    * @see     MsgWriter
    *
    *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
    *
    * @version \$Id: MsgLogger.h,v 1.3 2007-06-15 16:15:59 efeld Exp $
    *
    */

   class MsgLogger {

   public:
      /// Constructor with the message source defined by 'source'.
      MsgLogger( const std::string& source );
      /// default destructor
      ~MsgLogger();

      /// Send a message of type 'type' to the output.
      void send( MSG::Level type, const std::string& message ) const;

      bool 
      msgLvl (const MSG::Level lvl) const;

   private:
      const std::string m_source;       //!< name of originating class
      MsgWriter*        m_msgWriter;    //!< handle to message write class

   }; // class MsgLogger


} // namespace LVL1CTP

#endif // TRIGT1CTP_MSGLOGGER_H
