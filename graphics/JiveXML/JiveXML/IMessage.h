/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_IMESSAGE_H
#define JIVEXML_IMESSAGE_H

#include <GaudiKernel/IMessageSvc.h>

namespace JiveXML {

  /**
   * Pure abstract interface to provide some athena-indepandant messaging
   * Note that only MSG::Level enum is used from IMessageSvc
   */
  class IMessage {

    public:

      //Need virtual destructor
      virtual ~IMessage(){}

      /** @name Messaging services */
      //@{
      virtual void Message( const MSG::Level level, const std::string msg ) const = 0;
      /** Get the logging level **/
      virtual MSG::Level LogLevel() const = 0;
      //@}

  };
 
} //namespace

#endif
