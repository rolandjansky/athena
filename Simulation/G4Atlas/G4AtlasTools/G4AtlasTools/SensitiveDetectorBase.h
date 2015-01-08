/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_SENSITIVEDETECTORBASE_H
#define G4ATLASINTERFACES_SENSITIVEDETECTORBASE_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/ISensitiveDetector.h"
#include "G4VSensitiveDetector.hh"

// Members
#include "SimHelpers/AthenaHitsCollectionHelper.h"

// STL library
#include <string>
#include <vector>

class SensitiveDetectorBase : virtual public ISensitiveDetector , public G4VSensitiveDetector , public AthAlgTool {
 public:
  // Basic constructor and destructor
  SensitiveDetectorBase(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~SensitiveDetectorBase() {}

  /** Athena method, used to get out the G4 geometry and set up the SDs
    This naming is horrible, but the G4 method called Initialize is called at the beginning
    of every event.  This method is the one that is called ONLY by athena at the
    beginning of the job. */
  StatusCode initialize() override;

  /** Just the end of a G4 event.  The collection that is passed, G4HCofThisEvent,
    is not used in any ATLAS code and should not be #included anywhere (just forward
    defined).  We always use custom hit collections. */
  virtual void EndOfEvent(G4HCofThisEvent*) override;

  /** End of an athena event - store the hit collection in SG at this point */
  virtual void Gather() override;

  /** Process hits - all derived classes should (and do) override this.  The inputs
    are the current step, which is used by every SD to extract position and energy
    information, as well as particle type and process, and a touchable history, used
    in some SDs to get a more precise location inside the geometry tree */
  virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*) override;

 protected:
  std::vector<std::string> m_volumeNames;   ///!< All the volumes to which this SD is assigned
  AthenaHitsCollectionHelper m_hitCollHelp; ///!< Hit collection helper, used by many SDs for SG interfaces

};

#endif
