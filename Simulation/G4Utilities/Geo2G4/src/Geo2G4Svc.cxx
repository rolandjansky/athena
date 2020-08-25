/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4Svc.h"
#include "Geo2G4AssemblyVolume.h"
#include "ExtParameterisedVolumeBuilder.h"

// separate function not part of this class (why?)
BuilderMap InitializeBuilders(Geo2G4AssemblyFactory*);

Geo2G4Svc::Geo2G4Svc(const std::string& name, ISvcLocator* svcLocator)
  : base_class(name,svcLocator)
  , m_defaultBuilder()
  , m_getTopTransform(true)
  , m_G4AssemblyFactory(nullptr)
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
    {
      ATH_MSG_VERBOSE (" Geo2G4Svc already initialized.");
      return StatusCode::SUCCESS;
    }
  ATH_MSG_VERBOSE ("Initializing the Geo2G4Svc.");
  ATH_MSG_VERBOSE ("Creating all builders available.");
  m_G4AssemblyFactory = std::make_unique<Geo2G4AssemblyFactory>();
  m_builders = InitializeBuilders(m_G4AssemblyFactory.get());
  
  const std::string nameBuilder = "Extended_Parameterised_Volume_Builder"; //TODO Configurable property??
  this->SetDefaultBuilder(nameBuilder);
  ATH_MSG_VERBOSE (nameBuilder << " --> set as default builder" );
  ATH_MSG_VERBOSE (nameBuilder << " --> ParamOn flag = " << GetDefaultBuilder()->GetParam());
  initialized=1;
  if(msgLvl(MSG::VERBOSE))
    {
      this->ListVolumeBuilders();
    }
  return StatusCode::SUCCESS;
}

StatusCode Geo2G4Svc::finalize()
{
  ATH_MSG_VERBOSE ("Finalizing the Geo2G4Svc.");
  return StatusCode::SUCCESS;
}

void Geo2G4Svc::handle(const Incident& )
{
}

void Geo2G4Svc::ListVolumeBuilders() const
{
  ATH_MSG_INFO("---- List of all Volume Builders registered with Geo2G4Svc ----");
  ATH_MSG_INFO("---------------------------------------------------------------");
  for (const auto& builder : m_builders)
    {
      ATH_MSG_INFO(" Volume Builder: "<<builder.second->GetKey());
    }
  ATH_MSG_INFO("---------------------------------------------------------------");
  ATH_MSG_INFO(" default builder is "<<GetDefaultBuilder()->GetKey());
}

VolumeBuilder* Geo2G4Svc::GetVolumeBuilder(std::string s) const
{
  const auto builderItr(m_builders.find(s));
  if (builderItr!=m_builders.end())
    {
      return builderItr->second.get();
    }
  else
    {
      ATH_MSG_ERROR ("Trying to retrieve a not existing builder "<<s);
      ATH_MSG_ERROR ("\treturning Default Builder");
    }
  return GetDefaultBuilder();
}
