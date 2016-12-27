// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TSocket.h 781356 2016-10-31 14:03:28Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TSOCKET_H
#define XAODROOTACCESS_TOOLS_TSOCKET_H

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"

// Forward declaration(s):
class TString;
class TInetAddress;

namespace xAOD {

   /// Very simple wrapper around POSIX sockets
   ///
   /// Since ROOT's TSocket is not fit to be used in TFileAccessTracer's
   /// destructor (as some ROOT infrastructure may not be available by the time
   /// the destructor is called), we need our own simple implementation instead.
   ///
   /// Most of the implementation is simply a copy of the code from TUnixSystem.
   /// Slightly simplified in some cases where we don't need the flexibility of
   /// the ROOT implementation.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 781356 $
   /// $Date: 2016-10-31 15:03:28 +0100 (Mon, 31 Oct 2016) $
   ///
   class TSocket {

   public:
      /// Constructor with an address and a port
      TSocket();
      /// Destructor
      ~TSocket();

      /// Function connecting to the specified address
      TReturnCode connect( const TInetAddress& address, int port );
      /// Close the current connection
      TReturnCode close();

      /// Check if the socket is connected to some address at the moment
      bool isConnected() const;

      /// Function sending a message to the connected address
      TReturnCode send( const TString& payload );

   private:
      /// The underlying socket
      int m_socket;

   }; // class TSocket

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TSOCKET_H
