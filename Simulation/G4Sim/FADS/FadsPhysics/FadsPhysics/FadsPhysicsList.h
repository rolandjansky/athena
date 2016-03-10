/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsPhysicsList_H
#define FadsPhysicsList_H

#include <iostream>
#include <vector>
#include <string>

class G4VUserPhysicsList;

namespace FADS {

  class PhysicsListCatalog;

  class FadsPhysicsList {
  private:
    std::string m_name;
    std::vector<std::string> m_theDescription;
  protected:
    double m_cutValue;
  public:
    void SetCutValue(double v) {m_cutValue=v;}
    std::string GetName();
    FadsPhysicsList(std::string n);
    virtual ~FadsPhysicsList() {}
    void AddDescription(std::string s);
    virtual G4VUserPhysicsList* GetPhysicsList() {return 0;}
    virtual void Construct()=0;
    virtual void SetCuts()=0;
  };

} // end namespace

#endif // PhysicsListEntry

