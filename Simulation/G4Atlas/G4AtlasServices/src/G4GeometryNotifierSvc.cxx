/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4GeometryNotifierSvc.h"

#include "PVNotifier.h"
#include "LVNotifier.h"

G4GeometryNotifierSvc::G4GeometryNotifierSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : base_class(name,pSvcLocator),
    m_activateLVNotifier(false),
    m_activatePVNotifier(false),
    lvNotifier(nullptr),
    pvNotifier(nullptr)
{
  declareProperty( "ActivateLVNotifier",m_activateLVNotifier,"Toggle on/off the G4 LV notifier");
  declareProperty( "ActivatePVNotifier",m_activatePVNotifier,"Toggle on/off the G4 PV notifier");
  ATH_MSG_DEBUG( "G4GeometryNotifierSvc being created!" );
}

G4GeometryNotifierSvc::~G4GeometryNotifierSvc()
{
  if (lvNotifier) delete lvNotifier;
  if (pvNotifier) delete pvNotifier;
}

StatusCode G4GeometryNotifierSvc::initialize(){
  if (m_activateLVNotifier || m_activatePVNotifier)
    {
      ATH_MSG_DEBUG( "Notifiers being instantiated " );
      if (m_activateLVNotifier) lvNotifier=new LVNotifier(this);
      if (m_activatePVNotifier) pvNotifier=new PVNotifier(this);
    }

  ATH_MSG_DEBUG( "G4GeometryNotifierSvc initialized!!!" );
  return StatusCode::SUCCESS;
}

StatusCode G4GeometryNotifierSvc::finalize(){
  ATH_MSG_DEBUG( "G4GeometryNotifierSvc being finalized!!!" );
  return StatusCode::SUCCESS;
}
