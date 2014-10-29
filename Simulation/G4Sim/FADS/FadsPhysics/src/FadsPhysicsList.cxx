/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsPhysics/FadsPhysicsList.h"
#include "FadsPhysics/PhysicsListCatalog.h"

namespace FADS {

  std::string FadsPhysicsList::GetName()
  {
    return name;
  }
  FadsPhysicsList::FadsPhysicsList(std::string n): name(n) , cutValue(-1.)
  {
    // cout<<"c-tor called for "<<name<<endl;
    PhysicsListCatalog *plc=PhysicsListCatalog::GetInstance();
    // cout<<"Registering physics list "<<n<<" "<<endl;
    plc->RegisterPhysicsList(this);
  }
  void FadsPhysicsList::AddDescription(std::string s)
  {
    theDescription.push_back(s);
  }

} // end namespace
