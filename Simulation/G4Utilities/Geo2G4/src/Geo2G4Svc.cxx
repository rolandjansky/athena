/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4Svc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/StoreGateSvc.h"

#include "Geo2G4SvcAccessor.h"
#include "VolumeBuilder.h"

void InitializeBuilders();

Geo2G4Svc::Geo2G4Svc(const std::string& n, ISvcLocator* svc):
  AthService(n,svc),m_pIncSvc(0),defaultBuilder(0),m_getTopTransform(true)
{
  ATH_MSG_VERBOSE ("Creating the Geo2G4Svc.");
  declareProperty("GetTopTransform", m_getTopTransform);
}
Geo2G4Svc::~Geo2G4Svc()
{;}

StatusCode Geo2G4Svc::initialize()
{
  static int initialized=0;
  if (initialized)
    ATH_MSG_VERBOSE (" Geo2G4Svc already initialized.");
  else
    {
      ATH_MSG_VERBOSE ("Initializing the Geo2G4Svc.");
      ATH_MSG_VERBOSE ("Creating all builders available.");
      InitializeBuilders();

      const std::string nameBuilder = "Extended_Parameterised_Volume_Builder"; //TODO Configurable property??
      SetDefaultBuilder(nameBuilder);
      ATH_MSG_VERBOSE (nameBuilder << " --> set as default builder" );
      ATH_MSG_VERBOSE (nameBuilder << " --> ParamOn flag = " << defaultBuilder->GetParam());
      initialized=1;
      if(msgLvl(MSG::VERBOSE)) {
        ListVolumeBuilders();
      }
    }

  return StatusCode::SUCCESS;
}

StatusCode Geo2G4Svc::finalize()
{
  ATH_MSG_VERBOSE ("Finalizing the Geo2G4Svc.");

  return StatusCode::SUCCESS;
}

StatusCode Geo2G4Svc::queryInterface(const InterfaceID& riid, void**ppvInt)
{
  const InterfaceID& iid=intID();
  if (riid==iid)
    {
      *ppvInt=dynamic_cast<IGeo2G4Svc*>(this);
      return StatusCode::SUCCESS;
    }
  return AthService::queryInterface(riid,ppvInt);
}

void Geo2G4Svc::handle(const Incident& )
{
}

void Geo2G4Svc::RegisterVolumeBuilder(VolumeBuilder* vb)
{
  std::string key(vb->GetKey());
  if (m_builders.find(key)!=m_builders.end())
    {
      ATH_MSG_DEBUG ("Trying to set an already existing builder "<<key);
      ATH_MSG_DEBUG ("\t request ignored, nothing done ");
    }
  else
    {
      m_builders[key]=vb;
      ATH_MSG_DEBUG ("Volume builder registered "<<key);
    }
}
void Geo2G4Svc::ListVolumeBuilders()
{
  BuilderMap::const_iterator it;
  std::cout<<"---- List of all Volume Builders registered with Geo2G4Svc ----"<<std::endl;
  std::cout<<"---------------------------------------------------------------"<<std::endl;
  for (it=m_builders.begin();it!=m_builders.end();it++)
    std::cout<<" Volume Builder  "<<(*it).second->GetKey()<<std::endl;
  std::cout<<"---------------------------------------------------------------"<<std::endl;
  std::cout<<" default builder is "<<defaultBuilder->GetKey()<<std::endl;
}
void Geo2G4Svc::UnregisterVolumeBuilder(VolumeBuilder* vb)
{
  const std::string key(vb->GetKey());
  if (m_builders.find(key)!=m_builders.end())
    {
      ATH_MSG_DEBUG ("Removing builder "<<key<<" from the list");
      m_builders.erase(key);
    }
  else
    {
      ATH_MSG_ERROR ("Trying to remove a not-existing builder "<<key);
      ATH_MSG_ERROR ("\t request ignored, nothing done ");
    }
}
VolumeBuilder* Geo2G4Svc::GetVolumeBuilder(std::string s)
{
  if (m_builders.find(s)!=m_builders.end())
    {
      return m_builders[s];
    }
  else
    {
      ATH_MSG_ERROR ("Trying to retrieve a not existing builder "<<s);
      ATH_MSG_ERROR ("\treturning Default Builder");
    }
  return defaultBuilder;
}
