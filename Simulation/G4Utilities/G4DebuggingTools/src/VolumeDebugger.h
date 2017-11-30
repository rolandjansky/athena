/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_VolumeDebugger_H
#define G4DEBUGGINGTOOLS_VolumeDebugger_H

#include <string>
#include  <mutex>

class G4LogicalVolume;
class G4VPhysicalVolume;

#include "G4UserRunAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA{

  /// @class VolumeDebugger
  /// @brief a user action to debug G4 geometries
  ///
  /// It looks for the volume specified by the user in the TargetVolume parameter.
  /// (If it does not find it, it prints a very long list of possibilities, and exits without doing a check).
  /// The user can specifiy whether or not to make a GDML dump (I found it a waste of time and disk-space).
  /// It checks the specified volume, then does a recursive search through all child volumes.
  /// But it only enters into each LogVol once (no point in rechecking inside a LogVol over and over again for each PhysVol that points to it), saving quite
  /// some time. Furthermore, the user can specify a new parameter, MaxCopiesToCheck, as a double. Setting this to say 1.e20 will check all PhysVols are 
  /// inside their MotherVol, and do not collide with any sisters. Setting it to say 10. means max. 10 copies of a LogVol checked. The others are skipped. 
  /// The 10 chosen are spread out through the list of PhysVols that use the LogVol in question.
  /// Setting this parameter to say 2.0 gives a very fast check, while still checking every single type of LogVol at least once. Finds most overlaps very fast.


  static std::once_flag VolumeDebugger_DumpGeometryOnce;

  class VolumeDebugger:
  public AthMessaging, public G4UserRunAction
  {
    
  public:
    
    struct Config
    {
      std::string path="./test_volume.gdml";
      std::string targetVolume="CALO::CALO";
      int res=1000000;
      bool verbose=true;
      float tol=0.000001;
      bool volumeCheck=false; 
      double targetMaxCopiesToCheck=1.e6;
      bool dumpGDML=true;
  
    };
    
    VolumeDebugger(const Config& config);
    /// hook for G4 UA functionality
    virtual void BeginOfRunAction(const G4Run*) override;
    
  private:
    /// configuration data
    Config m_config;
    /// wrapper function for the gdml dump
    void DumpGeometry() const;
    /// volume filter
    void PullVolumes( G4LogicalVolume* v ) const;
    /// Returns true if there were overlaps
    bool recursiveCheck(G4VPhysicalVolume *pv) const;
    
  }; // class VolumeDebugger
  
  
} // namespace G4UA 



#endif //G4DEBUGGINGTOOLS_VolumeDebugger_H
