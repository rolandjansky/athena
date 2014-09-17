/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/ActionsBase.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

namespace FADS {

SmartIF<IMessageSvc>& ActionsBase::msgSvc() const {
  if ( !m_MS.isValid() ) {
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h
    IService* tmpMS=0;
    StatusCode status = svcLocator->getService("MessageSvc", tmpMS );
    m_MS = SmartIF<IMessageSvc>(tmpMS);
    if( status.isFailure() || !m_MS.isValid() ) {
      throw GaudiException("Service [MessageSvc] not found", name, StatusCode::FAILURE);
    }
  }
  return m_MS;
}

}  // End namespace

