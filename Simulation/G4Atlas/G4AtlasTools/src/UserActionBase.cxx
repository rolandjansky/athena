/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasTools/UserActionBase.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "G4VUserPhysicsList.hh"
#include "G4RunManager.hh"
#include "G4EmProcessOptions.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"

UserActionBase::UserActionBase(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_fpEventManager(nullptr),
    m_stackManager(nullptr),
    m_fpSteppingManager(nullptr),
    m_fpTrackingManager(nullptr),
    m_regions()
{

  declareProperty("TargetRegions",m_regions);

}


void UserActionBase::AddRole(G4AtlasUA::Role theRole, unsigned int order){

  m_roles.insert(std::map<G4AtlasUA::Role, unsigned int>::value_type(theRole,order));

}

StatusCode
UserActionBase::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if ( riid == IUserAction::interfaceID() )
    {
      *ppvIf = (IUserAction*)this;
      addRef();
      return StatusCode::SUCCESS;
    }
  return AthAlgTool::queryInterface( riid, ppvIf );
}

void UserActionBase::printRoles(){


  ATH_MSG_INFO("Roles enabled are:");

  for(auto p: m_roles){

    ATH_MSG_INFO("   Role: "<<p.first<<" in position "<<p.second);

  }

  ATH_MSG_INFO("[BeginOfRun=0, BeginOfEvent=1, PreTracking=2, Step=3, Classification=4, NewStage=5, PrepareNewEvent=6, PostTracking=7, EndOfEvent=8, EndOfRun=9]");

}
