/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_MSGWRITER_H
#define TRIGT1CTP_MSGWRITER_H

// STL include(s):
#include <string>

// Gaudi include(s):
#include "GaudiKernel/IMessageSvc.h"

// Forward declaration(s):
class MsgStream;

namespace LVL1CTP {

   /**
    *
    * @short   Singleton class responsible for showing messages
    *
    *          It is easier to provide a common logging facility for all the
    *          classes in the CTP simulation. This class is part of that facility.
    *
    * @see     MsgLogger
    *
    *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
    *
    * @version \$Id: MsgWriter.h,v 1.3 2007-06-15 16:15:59 efeld Exp $
    *
    */

   class MsgWriter {

   public:
      /// Singleton method for requesting the one and only object.
      static MsgWriter* instance();
      /// default destructor
      ~MsgWriter();

      /** @brief Test the output level
       *  @param lvl The message level to test against
       *  @return boolean Indicting if messages at given level will be printed
       *  @retval true Messages at level "lvl" will be printed
       */
      bool 
      msgLvl (const MSG::Level lvl) const;

      /// Write the message given in 'line' having a priority defined by 'type'.
      void write( MSG::Level type, const std::string& line );

      /// Set the name under which to send messages.
      void setName( const std::string& name );

   protected:
      /// Singleton design, the constructor is protected.
      MsgWriter();

   private:
      /// The pointer to the only instance of the object.
      static MsgWriter* m_instance;
      MsgStream* m_stream;

   }; // class MsgWriter


} // namespace LVL1CTP

#endif // TRIGT1CTP_MSGWRITER_H
