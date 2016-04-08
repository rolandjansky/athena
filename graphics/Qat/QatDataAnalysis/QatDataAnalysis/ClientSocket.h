// Definition of the ClientSocket class

#ifndef ClientSocket_class
#define ClientSocket_class

#include "QatDataAnalysis/Socket.h"

class ClientSocket : private Socket
{
 public:

  ClientSocket ( std::string host, int port );
  virtual ~ClientSocket(){};

  const ClientSocket& operator << ( const std::string& ) const;
  const ClientSocket& operator >> ( std::string& ) const;

  template<class T> const ClientSocket& operator << ( const T & ) const;
  template<class T> const ClientSocket& operator >> ( T & ) const;

};

#include "QatDataAnalysis/ClientSocket.icc"

#endif
