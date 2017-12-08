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
      : m_name(s), m_onSwitch(false), m_verboseLevel(0)
    {  }

    virtual ~ManipulatorBase() {}

    const std::string& GetName() const { return m_name; }

    /// @todo Eliminate the synonyms... I have no idea what they are for
    bool IsOn() const { return m_onSwitch; }
    bool GetStatus() const { return IsOn(); }
    void SetOnOff(bool on) { m_onSwitch = on; }
    void TurnOn() { SetOnOff(true); }
    void TurnOff() { SetOnOff(false); }
    void switchOn() { TurnOn(); }
    void switchOff() { TurnOff(); }

    void SetVerboseLevel(G4int i) { m_verboseLevel = i; }
    G4int GetVerboseLevel() const { return m_verboseLevel; }


  protected:

    std::string m_name;
    bool m_onSwitch;
    G4int m_verboseLevel;

  };


}

#endif
