/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/SimpleMenu.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"

MsgStream SimpleMenu::log()
{
  if (m_log) return *m_log;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* p_msgSvc = 0;

  if (svcLocator->service("MessageSvc", p_msgSvc).isFailure() || !p_msgSvc)
    std::cout << "Trouble getting the message service.  Should never happen.  Will crash now." << std::endl;
  m_log = new MsgStream(p_msgSvc,name);
  return *m_log;
}
