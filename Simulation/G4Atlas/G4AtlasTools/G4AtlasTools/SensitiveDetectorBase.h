/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_SENSITIVEDETECTORBASE_H
#define G4ATLASINTERFACES_SENSITIVEDETECTORBASE_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/ISensitiveDetector.h"

// Members
#include "G4VSensitiveDetector.hh"
#ifdef ATHENAHIVE
#  include "tbb/concurrent_unordered_map.h"
#endif

// STL library
#include <string>
#include <vector>
#include <thread>

class G4LogicalVolume;
class G4VSensitiveDetector;

class SensitiveDetectorBase : virtual public ISensitiveDetector , public AthAlgTool {
 public:
  /// Basic constructor and destructor
  SensitiveDetectorBase(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~SensitiveDetectorBase() {}

  /** Used to get out the G4 geometry and set up the SDs.  Separate from the AthAlgTool
    initialize() method because it might need to be run on several threads in Geant4MT.
    Note that the G4 method called Initialize is called at the beginning of every G4Event. */
  StatusCode initializeSD() override;

  /** Beginning of an athena event.  This is where collection initialization should happen.
    If we are using a WriteHandle, then this could be empty. */
  virtual StatusCode SetupEvent() override { return StatusCode::SUCCESS; }

  /** End of an athena event - store the output collection in SG at this point.  If we are
    using a WriteHandle, then this can be empty! */
  virtual StatusCode Gather() override { return StatusCode::SUCCESS; }

  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 protected:

  /// Retrieve the current SD. In hive, this means the thread-local SD.
  /// Otherwise, it is simply the single SD.
  G4VSensitiveDetector* getSD();

  std::vector<std::string> m_volumeNames; ///!< All the volumes to which this SD is assigned
  std::vector<std::string> m_outputCollectionNames; ///!< Names of all output collections written out by this SD.
  bool m_noVolumes;                       //!< This SensitiveDetector has no volumes associated with it.

  // Method stolen from G4VUserDetectorConstruction in G4 10.2
  void SetSensitiveDetector(G4LogicalVolume*, G4VSensitiveDetector*) const;

 private:

  /// Set the current SD. In hive, this gets assigned as the thread-local SD
  void setSD(G4VSensitiveDetector*);

#ifdef ATHENAHIVE
  /// Thread-to-SD concurrent map type
  typedef tbb::concurrent_unordered_map < std::thread::id,
                                          G4VSensitiveDetector*,
                                          std::hash<std::thread::id> > SDThreadMap_t;
  /// Concurrent map of SDs, one for each thread
  SDThreadMap_t m_sdThreadMap;
#else
  G4VSensitiveDetector* m_SD;             ///!< The sensitive detector to which this thing corresponds
#endif
};

#endif
