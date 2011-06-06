/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
 * @file
 *
 * A resource-acquisition-in-initialization class, which can be used
 * in a code that manipulates active store to guarantee that
 * ActiveStoreSvc is set back to the state expected by the rest of the
 * program.
 *
 * @author Andrei Gaponenko, 2006
 */

#ifndef ACTIVE_STORE_SWITCHER_H
#define ACTIVE_STORE_SWITCHER_H

#include "StoreGate/ActiveStoreSvc.h"
class StoreGateSvc;

class ActiveStoreSwitcher {
  ActiveStoreSvc *m_aSvc;
  StoreGateSvc *m_oldStore;
public:
  ActiveStoreSwitcher(ActiveStoreSvc *aSvc, StoreGateSvc *newStore) 
    : m_aSvc(aSvc), m_oldStore( aSvc->operator->() ) 
  {
    m_aSvc->setStore(newStore);
  }

  ~ActiveStoreSwitcher()
  {
    m_aSvc->setStore(m_oldStore);
  }
};

#endif/*ACTIVE_STORE_SWITCHER_H*/
