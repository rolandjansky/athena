// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "QatDataAnalysis/Socket.h"

class ServerSocket : private Socket
{
 public:

  ServerSocket ( int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;

  
  template <class T> const ServerSocket& operator << ( const T & query) const;
  template <class T> const ServerSocket& operator >> ( T & query) const;

  void accept ( ServerSocket& );

};

#include "QatDataAnalysis/ServerSocket.icc"

#endif
