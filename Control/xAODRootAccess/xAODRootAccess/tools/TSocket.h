// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_TOOLS_TSOCKET_H
#define XAODROOTACCESS_TOOLS_TSOCKET_H

// Local include(s):
#include "AsgMessaging/StatusCode.h"

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
   class TSocket {

   public:
      /// Constructor with an address and a port
      TSocket();
      /// Destructor
      ~TSocket();

      /// Do not allow object copying
      TSocket( const TSocket& ) = delete;
      /// Do not allow assignment
      TSocket& operator=( const TSocket& ) = delete;

      /// Function connecting to the specified address
      StatusCode connect( const TInetAddress& address, int port );
      /// Close the current connection
      StatusCode close();

      /// Check if the socket is connected to some address at the moment
      bool isConnected() const;

      /// Function sending a message to the connected address
      StatusCode send( const TString& payload );

   private:
      /// The underlying socket
      int m_socket;

   }; // class TSocket

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TSOCKET_H
