/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ManipulatorBase_H
#define ManipulatorBase_H

#include "G4Types.hh"
#include <string>
class G4PrimaryVertex;

namespace FADS {


  class ManipulatorBase {
  public:

    ManipulatorBase(const std::string& s)
      : name(s), onSwitch(false), verboseLevel(0)
    {  }

    virtual ~ManipulatorBase() {}

    const std::string& GetName() const { return name; }

    /// @todo Eliminate the synonyms... I have no idea what they are for
    bool IsOn() const { return onSwitch; }
    bool GetStatus() const { return IsOn(); }
    void SetOnOff(bool on) { onSwitch = on; }
    void TurnOn() { SetOnOff(true); }
    void TurnOff() { SetOnOff(false); }
    void switchOn() { TurnOn(); }
    void switchOff() { TurnOff(); }

    void SetVerboseLevel(G4int i) { verboseLevel = i; }
    G4int GetVerboseLevel() const { return verboseLevel; }


  protected:

    std::string name;
    bool onSwitch;
    G4int verboseLevel;

  };


}

#endif
