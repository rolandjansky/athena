// -- -*- C++ -*- 
#ifndef __ATH_INCIDENT_FIRER_ALG_H
#define __ATH_INCIDENT_FIRER_ALG_H

#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

class IIncidentSvc;

class AthIncFirerAlg : public AthAlgorithm{
public:
  AthIncFirerAlg( const std::string& name, ISvcLocator* pSvcLocator );  
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode execute_r(const EventContext& ctx) const ;
  StatusCode finalize() override;
private:
  // Gaudi::Property<std::vector<std::string>> m_incLists{this, "Incidents", {},"List of incidents to fire"};
  // Gaudi::Property<bool> m_Serial{this, "FireSerial",false,"Whether to fire serial incidents"};
  StringArrayProperty m_incLists;
  BooleanProperty m_Serial;
  SmartIF<IIncidentSvc> m_incSvc;
};

#endif
