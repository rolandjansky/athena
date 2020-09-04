// -- -*- C++ -*- 
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef __ATH_INCIDENT_FIRER_ALG_H
#define __ATH_INCIDENT_FIRER_ALG_H

#include <vector>
#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"

class IIncidentSvc;

class AthIncFirerAlg : public AthReentrantAlgorithm{
public:
  AthIncFirerAlg( const std::string& name, ISvcLocator* pSvcLocator );  
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
private:
  // Gaudi::Property<std::vector<std::string>> m_incLists{this, "Incidents", {},"List of incidents to fire"};
  // Gaudi::Property<bool> m_Serial{this, "FireSerial",false,"Whether to fire serial incidents"};
  StringArrayProperty m_incLists;
  BooleanProperty m_Serial;
  SmartIF<IIncidentSvc> m_incSvc;
};

#endif
