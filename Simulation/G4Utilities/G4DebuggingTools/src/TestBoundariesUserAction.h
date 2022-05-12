/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_TESTBOUNDARIESUSERACTION_H
#define G4DEBUGGINGTOOLS_TESTBOUNDARIESUSERACTION_H

#include <string>
#include <map>

#include "TMath.h"

#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

class TTree;
class TFile;

namespace G4UA
{

  class TestBoundariesUserAction:
    public AthMessaging, public G4UserRunAction, public G4UserSteppingAction
  {

  public:
    TestBoundariesUserAction();
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;

  private:

    typedef std::map<std::string,int> SMap;
    SMap m_sel;

    TFile * m_file{};
    TTree * m_tree{};

    struct Data
    {
      Float_t x,y,z;
      Int_t volume;
      Bool_t enter;
      Bool_t exit;
      Bool_t leave;

      /// Data constructor
      Data() : x(0.),y(0.),z(0.), volume(0),enter(false),exit(false),leave(false) {}

      /// Reset the data
      void Reset() {
        enter=exit=leave=false;
      }

      /// Set the data
      void Set(Float_t _x, Float_t _y, Float_t _z,
               Int_t _vol)
      {
        x=_x; y=_y; z=_z;
        volume=_vol;
      }

    } m_data;

  }; // class TestBoundariesUserAction

} // namespace G4UA

#endif //  G4DEBUGGINGTOOLS_TESTBOUNDARIESUSERACTION_H
