/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGMESSAGING_ASGMESSAGING_H
#define ASGMESSAGING_ASGMESSAGING_H

// System include(s):
#include <string>

// Local include(s):
#include "AsgMessaging/MsgStream.h"
#include "AsgMessaging/MsgStreamMacros.h"

// Gaudi/Athena include(s):
#ifndef XAOD_STANDALONE
#   include "AthenaBaseComps/AthMessaging.h"
#endif // not XAOD_STANDALONE

class INamedInterface;

namespace asg {

   /// Class mimicking the AthMessaging class from the offline software
   ///
   /// This class is supposed to provide a convenient way to provide messaging
   /// capabilities for classes that are not dual-use tools themselves. Such
   /// classes are encouraged to inherit from this one, and then print output
   /// messages using the "standard" messaging macros.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 615778 $
   /// $Date: 2014-09-09 16:27:03 +0200 (Tue, 09 Sep 2014) $
   ///
   class AsgMessaging
#ifndef XAOD_STANDALONE
      : public ::AthMessaging
#endif // not XAOD_STANDALONE
   {
   public:
      /// Constructor with a name
      AsgMessaging( const std::string& name );
      /// Constructor with a tool pointer
      AsgMessaging( const INamedInterface* tool );

      /// @name Functions providing the same interface as AthMessaging
      /// @{

      /// Test the output level of the object
      ///
      /// @param lvl The message level to test against
      /// @return boolean Indicting if messages at given level will be printed
      /// @returns <code>true</code> If messages at level "lvl" will be printed
      ///
      bool msgLvl( const MSG::Level lvl ) const;

      /// The standard message stream.
      ///
      /// @returns A reference to the default message stream of this object.
      ///
      MsgStream& msg() const;

      /// The standard message stream.
      ///
      /// @param lvl The message level to set the stream to
      /// @returns A reference to the default message stream, set to level "lvl"
      ///
      MsgStream& msg( const MSG::Level lvl ) const;

      /// @}

#ifdef XAOD_STANDALONE
   private:
      mutable MsgStream m_msg; ///< Message stream to be used by the object
#endif // XAOD_STANDALONE

   }; // class AsgMessaging

} // namespace asg

#endif // ASGMESSAGING_ASGMESSAGING_H
