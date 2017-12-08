/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_SENSITIVEDETECTORBASE_H
#define G4ATLASTOOLS_SENSITIVEDETECTORBASE_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/ISensitiveDetector.h"

// Members
#include "G4VSensitiveDetector.hh"
#ifdef G4MULTITHREADED
#  include "tbb/concurrent_unordered_map.h"
#endif

// STL library
#include <string>
#include <vector>
#include <thread>

// Forward declarations
class G4LogicalVolume;
class G4VSensitiveDetector;


/// @class SensitiveDetectorBase
/// @brief Common base class for tools that create and manage Geant4 sensitive
/// detectors.
///
/// Concrete tools must implement the makeSD method of ISensitiveDetector to
/// create an SD on demand. In multi-threading mode, SDs are created once per
/// thread by the infrastructure invoking initializeSDs.
///
/// Concrete tools can optionally implement the SetupEvent and Gather methods
/// for preparing and post-processing output collections at the beginning or
/// end of an Athena event.
///
class SensitiveDetectorBase : virtual public ISensitiveDetector, public AthAlgTool
{

 public:

  /// Standard constructor
  SensitiveDetectorBase(const std::string& type, const std::string& name,
                        const IInterface *parent);
  /// Empty virtual destructor
  virtual ~SensitiveDetectorBase() {}

  /// @brief Setup an SD in the current thread.
  ///
  /// Separate from the AthAlgTool initialize() method because it needs to be
  /// called once per worker thread in AthenaMT. Don't confuse this with the
  /// G4 SD method Initialize which is called at the beginning of every G4 event.
  StatusCode initializeSD() override;

  /// @brief Beginning of an athena event.
  /** This is where collection initialization should happen.
      If we are using a WriteHandle, then this could be empty.  */
  virtual StatusCode SetupEvent() override { return StatusCode::SUCCESS; }

  /// @brief End of an athena event.
  /** Store the output collection in SG at this point.
      If we are using a WriteHandle, then this can be empty! */
  virtual StatusCode Gather() override { return StatusCode::SUCCESS; }

  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 protected:

  /// @brief Assign SD to a list of volumes
  /** This method supports wild card matching */
  StatusCode assignSD(G4VSensitiveDetector* sd,
                      const std::vector<std::string>& volumes) const;

  /// @brief Retrieve the current SD.
  /** In AthenaMT, this means the thread-local SD.
      Otherwise, it is simply the single SD. */
  G4VSensitiveDetector* getSD();

  /// All the volumes to which this SD is assigned
  std::vector<std::string> m_volumeNames;
  /// Names of all output collections written out by this SD.
  std::vector<std::string> m_outputCollectionNames;

  /// This SensitiveDetector has no volumes associated with it.
  bool m_noVolumes;

  /// Method stolen from G4VUserDetectorConstruction in G4 10.2
  void SetSensitiveDetector(G4LogicalVolume*, G4VSensitiveDetector*) const;

 private:

  /// @brief Set the current SD.
  /** In hive, this gets assigned as the thread-local SD. */
  void setSD(G4VSensitiveDetector*);

#ifdef G4MULTITHREADED
  /// Thread-to-SD concurrent map type
  using SDThreadMap_t = tbb::concurrent_unordered_map
    < std::thread::id, G4VSensitiveDetector*, std::hash<std::thread::id> >;
  /// Concurrent map of SDs, one for each thread
  SDThreadMap_t m_sdThreadMap;
#else
  /// The sensitive detector to which this thing corresponds
  G4VSensitiveDetector* m_SD;
#endif

}; // class SensitiveDetectorBase

#endif
