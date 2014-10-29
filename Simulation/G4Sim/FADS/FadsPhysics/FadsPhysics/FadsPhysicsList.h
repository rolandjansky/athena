/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsPhysicsList_H
#define FadsPhysicsList_H

#ifdef HAVE_NEW_IOSTREAMS
#include <iostream>
#else
#include <iostream.h>
#endif

#include <vector>
#include <string>

class G4VUserPhysicsList;

namespace FADS {

  class PhysicsListCatalog;

  class FadsPhysicsList {
  private:
    std::string name;
    std::vector<std::string> theDescription;
  protected:
    double cutValue;
  public:
    void SetCutValue(double v) {cutValue=v;}
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

