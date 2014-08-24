/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
/**
 * @file MessageSvcProvider.h
 * @author Simon George <s.george@rhul.ac.uk>
 * @author Andrew Lowe <a.j.lowe@rhul.ac.uk>
 * @brief Provides message services.
 *
 * Provides a way of using the Gaudi MessageSvc when it is not easily
 * available, e.g. for data classes.
 * Just include this file in any class that requires message services.
 */

/**
 * ==========================================
 * @warning THIS CLASS IS *NOT* THREAD-SAFE!
 * ==========================================
 */

#ifndef TRIGSTEERINGEVENT_MESSAGESVCPROVIDER_H
#define TRIGSTEERINGEVENT_MESSAGESVCPROVIDER_H

#include <iostream>
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"

/**
 * @brief OBSOLETE It is an accessor for messageSvc for objects which were not GAUDI algorithms or tools
 *
 * @warning Usage of this class is obsolete use this: IMessageSvc* Athena::getMessageSvc();
 *          See HN thread: https://hypernews.cern.ch/HyperNews/Atlas/get/offlineSWDevelopment/22/1.html
 */
class MessageSvcProvider {

public:

  MessageSvcProvider() : m_pMsgSvc(0) {}

  ~MessageSvcProvider(){}

  /**
   * @fn IMessageSvc* msgSvc() const
   *
   * @warning If this fails, without cout there is no way to tell the user!
   *
   * @return An IMessageSvc pointer.
   *
   * To use, your class needs a MessageSvcProvider as a member.
   * You can then do:
   * MsgStream log(m_MessageSvcProvider.msgSvc(), m_objectName);
   */

  IMessageSvc* msgSvc() const {
    if(!m_pMsgSvc){
      ISvcLocator* serviceLocator = Gaudi::svcLocator();
      IService* theSvc;

      StatusCode sc = serviceLocator->getService("MessageSvc", theSvc, true);
      if(sc.isFailure()) exit(1); // Service [MessageSvc] not found

      m_pMsgSvc = dynamic_cast<IMessageSvc*>(theSvc);
      if(!m_pMsgSvc) exit(1); // Dynamic cast IService --> IMessageSvc failed
    }
    return m_pMsgSvc;
  }

private:

  /**
   * @var IMessageSvc* m_pMsgSvc
   * @see msgSvc()
   */
  mutable IMessageSvc* m_pMsgSvc;
};

#endif // MESSAGESVCPROVIDER_H

