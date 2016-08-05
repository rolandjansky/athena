/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

MbtsDetDescrManager::MbtsDetDescrManager():
  m_msgSvc(0)
{
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode status = svcLoc->service( "MessageSvc", m_msgSvc);
  if(status.isFailure() || m_msgSvc==0)
    std::cerr << "CaloDetDescrManager: Could not locate the MessageSvc!\n";
}

MbtsDetDescrManager::~MbtsDetDescrManager()
{
  MbtsElements::iterator it = m_elements.begin();
  for(;it!=m_elements.end();it++) 
    delete it->second;
}

CaloDetDescrElement* MbtsDetDescrManager::get_element(const Identifier& elementId) const
{
  MbtsElements::const_iterator it = m_elements.find(elementId);
  if(it==m_elements.end())
    return 0;
  else
    return it->second;
}

void MbtsDetDescrManager::add(CaloDetDescrElement* element)
{
  MbtsElements::iterator it = m_elements.find(element->identify());
  if(it==m_elements.end()) 
    m_elements[element->identify()] = element;
  else {
    // Issue warning message and don't update map entry
    if(m_msgSvc) {
      MsgStream log(m_msgSvc, "MbtsDetDescrManager::add");
      log << MSG::DEBUG << "Attempt to update map entry for ID=" << element->identify().getString() << ". Skipping" << endmsg;
    }
  }
}
