/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_RadLengthAction_H
#define G4UserActions_RadLengthAction_H

#include <map>
#include <vector>
#include <string>

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSensitiveDetector.hh"


class TTree;

namespace G4UA
{
  class RadLengthAction final : public G4UserRunAction,
                       public G4UserEventAction,
                       public G4UserSteppingAction
  {
  public:

    struct Config
    {
      // depth of volume, how many daughters should be taken
      // into account, value recieved from jobOptions
      int VolumeDepthLevel=1;
    };

    RadLengthAction(const Config& config);
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;

  private:

    Config m_config;

    // decision if muon chamber or trigger was already hit
    // set to false every BeginOfEvent and changed in Stepping
    bool MuChamberPassed;
    bool MuTriggerPassed;

    // primary variables obtained at BeginOfEvent
    double etaPrimary, phiPrimary, chargePrimary;

    // map of volumes initialized at BeginOfRun and used
    // for comparison in Stepping
    std::map<std::string,G4VPhysicalVolume*> topvolmap;

    // map of trees named like the volumes initalized during
    // BeginOfRun and filled at EndOfEvent
    std::map<std::string,TTree*> treeMap;

    // map of vector which is initialized at BeginOfRun, components
    // of the vector are referenced in the branches of the tree with
    // specific volume name, in FillVariables the vectors get values
    std::map<std::string,std::vector<double> > variables;

    // sensitive detectors retrieved at BeginOfRun and need in Stepping
    // see ./atlas/MuonSpectrometer/MuonG4/MuonG4SD/share/muonSD.mac
    G4VSensitiveDetector* m_SDMDT;
    G4VSensitiveDetector* m_SDTGC;
    G4VSensitiveDetector* m_SDCSC;
    G4VSensitiveDetector* m_SDRPC;

    // methode to fill vector stored in variables map (index volume name)
    void fillVariables(std::vector<double> varvec, const std::string& name);
  }; //class RadLengthAction

} //namespace G4UA
#endif // G4UserActions_RadLengthAction_H
