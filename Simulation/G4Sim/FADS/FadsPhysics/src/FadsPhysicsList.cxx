/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsPhysics/FadsPhysicsList.h"
#include "FadsPhysics/PhysicsListCatalog.h"

namespace FADS {

  std::string FadsPhysicsList::GetName()
  {
    return m_name;
  }
  FadsPhysicsList::FadsPhysicsList(std::string n): m_name(n) , m_cutValue(-1.)
  {
    // cout<<"c-tor called for "<<m_name<<endl;
    PhysicsListCatalog *plc=PhysicsListCatalog::GetInstance();
    // cout<<"Registering physics list "<<n<<" "<<endl;
    plc->RegisterPhysicsList(this);
  }
  void FadsPhysicsList::AddDescription(std::string s)
  {
    m_theDescription.push_back(s);
  }

} // end namespace
