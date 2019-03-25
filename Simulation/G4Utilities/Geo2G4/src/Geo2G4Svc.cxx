/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4Svc.h"
#include "VolumeBuilder.h"
#include "ExtParameterisedVolumeBuilder.h"

void InitializeBuilders(Geo2G4AssemblyFactory*);

Geo2G4Svc::Geo2G4Svc(const std::string& name, ISvcLocator* svcLocator)
  : base_class(name,svcLocator)
  , m_defaultBuilder(nullptr)
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
  InitializeBuilders(m_G4AssemblyFactory.get()); // separate function not part of this class
  
  const std::string nameBuilder = "Extended_Parameterised_Volume_Builder"; //TODO Configurable property??
  this->SetDefaultBuilder(nameBuilder);
  ATH_MSG_VERBOSE (nameBuilder << " --> set as default builder" );
  ATH_MSG_VERBOSE (nameBuilder << " --> ParamOn flag = " << m_defaultBuilder->GetParam());
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
  
  // clear builders...
  // TODO: replace with std::unique_ptr
  for (auto &x : m_builders) {
      if (x.second)
          delete x.second;
  }

  return StatusCode::SUCCESS;
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

void Geo2G4Svc::ListVolumeBuilders() const
{
  ATH_MSG_INFO("---- List of all Volume Builders registered with Geo2G4Svc ----");
  ATH_MSG_INFO("---------------------------------------------------------------");
  for (const auto& builder : m_builders)
    {
      ATH_MSG_INFO(" Volume Builder: "<<builder.second->GetKey());
    }
  ATH_MSG_INFO("---------------------------------------------------------------");
  ATH_MSG_INFO(" default builder is "<<m_defaultBuilder->GetKey());
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

VolumeBuilder* Geo2G4Svc::GetVolumeBuilder(std::string s) const
{
  const auto builderItr(m_builders.find(s));
  if (builderItr!=m_builders.end())
    {
      return builderItr->second;
    }
  else
    {
      ATH_MSG_ERROR ("Trying to retrieve a not existing builder "<<s);
      ATH_MSG_ERROR ("\treturning Default Builder");
    }
  return m_defaultBuilder;
}
