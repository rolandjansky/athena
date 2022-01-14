/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AtlasPhysListFactory_h
#define AtlasPhysListFactory_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class AtlasPhysListFactory
{
public:

  AtlasPhysListFactory();

  ~AtlasPhysListFactory()=default;

  G4VModularPhysicsList* GetReferencePhysList(const G4String&);
  // instantiate PhysList by name

  G4VModularPhysicsList* ReferencePhysList();
  // instantiate PhysList by environment variable "PHYSLIST"

  G4bool IsReferencePhysList(const G4String&);
  // check if the name is in the list of PhysLists names

  const std::vector<G4String>& AvailablePhysLists() const;
  // list of avalable base Phys Lists

  inline void SetVerbose(G4int val) { m_verbose = val; }

private:

  G4String m_defName;  
  std::vector<G4String> m_listnames;
  G4int m_verbose;
};

#endif
