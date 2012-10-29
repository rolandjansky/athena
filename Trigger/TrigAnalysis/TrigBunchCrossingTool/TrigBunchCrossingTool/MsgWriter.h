// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgWriter.h 514416 2012-08-22 11:42:15Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_MSGWRITER_H
#define TRIGBUNCHCROSSINGTOOL_MSGWRITER_H

// STL include(s):
#include <string>
#include <map>

// Gaudi/Athena include(s):
#ifndef ROOTCORE
#include "GaudiKernel/MsgStream.h"
#endif // ROOTCORE

// Local include(s):
#include "MsgType.h"

namespace Trig {

   // Forward declaration(s):
   class MsgLogger;

   /**
    *  @short Singleton message writing class
    *
    *         This singleton object is used internally by MsgLogger to output
    *         all the messages. The class can be compiled in "Athena" and
    *         "standalone" mode. In Athena mode it prints all the messages
    *         using Gaudi's MsgStream. In standalone mode it uses ROOT's
    *         message printing functions.
    *
    *         This means that in both worlds the message printing can be
    *         influenced from the outside as well.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 514416 $
    * $Date: 2012-08-22 13:42:15 +0200 (Wed, 22 Aug 2012) $
    */
   class MsgWriter {

      // Declare the MsgLogger a friend of this class
      friend class MsgLogger;

   public:
      /// Function for accessing the single object
      static MsgWriter* instance();
      /// Default destructor
      ~MsgWriter();

#ifdef ROOTCORE
      /// Set the message type above which messages are printed
      void setMinType( MsgType type );
      /// Get the message type above which messages are printed
      MsgType getMinType() const;
#else
      /// Set the minimum level for the MsgStream object
      void setMinType( MSG::Level type );
      /// Set the source for the MsgStream object
      void setSource( const std::string& source );
#endif // ROOTCORE

   protected:
      /// Protected default constructor
      MsgWriter();

      /// Function writing a message to the output
      void write( MsgType type, const std::string& source,
                  const std::string& line ) const;

   private:
      static MsgWriter* m_instance; ///< Pointer to the only instance of the class

#ifdef ROOTCORE
      MsgType m_minType; ///< Minimum type to be printed
#else
      std::map< MsgType, MSG::Level > m_typeMap; ///< Message level translation map
      MsgStream*                      m_stream; ///< The MsgStream object used
#endif // ROOTCORE

   }; // class MsgWriter

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_MSGWRITER_H
