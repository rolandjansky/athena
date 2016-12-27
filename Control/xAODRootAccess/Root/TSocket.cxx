/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TSocket.cxx 781412 2016-10-31 16:40:04Z krasznaa $

// System include(s):
extern "C" {
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <netdb.h>
}
#include <cstring>

// ROOT include(s):
#include <TString.h>
#include <TInetAddress.h>

// Local include(s):
#include "xAODRootAccess/tools/TSocket.h"

namespace xAOD {

   TSocket::TSocket()
      : m_socket( -1 ) {

   }

   TSocket::~TSocket() {

      // Close the socket if it's open:
      if( isConnected() ) {
         close().ignore();
      }
   }

   /// It turns out that translating a string address name into an integer
   /// address that the connect(...) function understands, is no easy feat.
   /// So instead we leave that up to ROOT. This translation just needs to
   /// be done "early enough" in the job when ROOT can still do it.
   ///
   /// @param address The address to send the data to
   /// @param port The port number to connect to
   /// #returns The usual return codes...
   ///
   TReturnCode TSocket::connect( const TInetAddress& address, int port ) {

      // If the address is invalid, give up now:
      if( ! address.IsValid() ) {
         return TReturnCode::kFailure;
      }

      // If not, translate it to a network address:
      const UInt_t adr = htonl( address.GetAddress() );

      // "Translate" the port number:
      short sport = 0;
      struct servent *sp = 0;
      if( ( sp = ::getservbyport( htons( port ), "tcp" ) ) ) {
         sport = sp->s_port;
      } else {
         sport = htons( port );
      }

      // Create a socket:
      m_socket = ::socket( AF_INET, SOCK_STREAM, 0 );
      if( m_socket < 0 ) {
         return TReturnCode::kFailure;
      }

      // Create the address that we want to connect to:
      struct sockaddr_in server;
      memset( &server, 0, sizeof( server ) );
      memcpy( &server.sin_addr, &adr, sizeof( adr ) );
      server.sin_family = address.GetFamily();
      server.sin_port = sport;

      // Connect to the address:
      if( ::connect( m_socket, ( struct sockaddr* ) &server,
                     sizeof( server ) ) < 0 ) {
         m_socket = -1;
         return TReturnCode::kFailure;
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TReturnCode TSocket::close() {

      // Check if anything needs to be done:
      if( ! isConnected() ) {
         return TReturnCode::kRecoverable;
      }

      // Close the socket:
      if( ::close( m_socket ) != 0 ) {
         return TReturnCode::kFailure;
      }

      // Return gracefully:
      m_socket = -1;
      return TReturnCode::kSuccess;
   }

   bool TSocket::isConnected() const {

      return ( m_socket != -1 );
   }

   /// This function is custom made for our use case. It sends HTML style
   /// information to the connected server.
   ///
   /// @param payload The HTML style payload to send to the server
   /// @returns The usual return codes...
   ///
   TReturnCode TSocket::send( const TString& payload ) {

      // Check if we're connected:
      if( ! isConnected() ) {
         return TReturnCode::kFailure;
      }

      // The buffer we are sending from:
      const char* buffer = payload.Data();
      // The total length of the message:
      const auto length = payload.Length();
      // Number of bytes already sent:
      int sent = 0;

      // Keep sending the message until everything is through:
      for( int i = 0; i < length; i += sent ) {
         sent = ::send( m_socket, buffer + i, length - i, 0 );
         if( sent < 0 ) {
            return TReturnCode::kFailure;
         } else if( sent == 0 ) {
            break;
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }
   
} // namespace xAOD
