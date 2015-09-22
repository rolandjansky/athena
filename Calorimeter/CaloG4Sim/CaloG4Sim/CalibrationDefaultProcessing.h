/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloG4::CalibrationDefaultProcessing
// 04-Mar-2004 William Seligman

// 21-Sep-2004 WGS: Moved to CaloG4Sim.

// The calibration studies rely on every volume in the simulation
// being made into a sensitive detector.  There is a practical
// problem: What if we're still in the middle of developing code, and
// not every volume has been made sensitive yet?  What if we've
// overlooked a volume?

// This class provides a "default behavior" for all energy deposits
// that are not made in a volume that's been made sensitive for
// calibration studies.

// Since we need to examine the particles at every G4Step, this class
// makes use of a UserAction interface class.

#ifndef CaloG4_CalibrationDefaultProcessing_h
#define CaloG4_CalibrationDefaultProcessing_h

// The Athena and stand-alone G4 setups have different UserAction
// classes (though they accomplish the same thing.

#include "G4AtlasTools/UserActionBase.h"


// Forward declarations
class G4Run;
class G4Step;
class G4VSensitiveDetector;

namespace CaloG4 {

  class CalibrationDefaultProcessing final : public UserActionBase {

  public:

    CalibrationDefaultProcessing(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~CalibrationDefaultProcessing();

    virtual void Step(const G4Step*) override;
    virtual void BeginOfEvent(const G4Event*) override;
    
    // Make the default sensitive detector available to other routines.
    G4VSensitiveDetector* GetDefaultSD() { return m_defaultSD; }
    //static void SetDefaultSD( G4VSensitiveDetector* );

    virtual StatusCode queryInterface(const InterfaceID&, void**) override;

    

  private:

    // The default sensitive detector to be applied to all G4Steps
    // in volumes without a CalibrationSensitiveDetector.
    G4VSensitiveDetector* m_defaultSD;
    std::string m_SDname;

  };

} // namespace CaloG4

#endif // CaloG4_CalibrationDefaultProcessing_h
