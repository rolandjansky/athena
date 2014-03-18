/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeometryManager_H
#define GeometryManager_H

#include <map>
#include <string>
#include <vector>
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"

#include "FadsGeometry/FadsLogicalVolumeNotifier.h"
#include "FadsGeometry/FadsPhysicalVolumeNotifier.h"

namespace FADS {

  class GeometryMessenger;
  class VolumeVisualizationMessenger;

  typedef std::multimap<std::string, G4LogicalVolume*,std::less<std::string> > volumeMap;
  typedef std::multimap<std::string, G4VSolid*,std::less<std::string> > solidMap;
  typedef std::multimap<std::string, G4VPhysicalVolume*,std::less<std::string> > physicalMap;
  typedef std::multimap<std::string, G4RotationMatrix*,std::less<std::string> > rotationMap;

  class DetectorStore {
  private:
    volumeMap LVmap;
    physicalMap PVmap;
    rotationMap RMmap;
    solidMap Smap;

    friend class GeometryManager;
  };

  typedef std::map<std::string,DetectorStore*,std::less<std::string> > detectorMap;

  class GeometryManager {
  private:
    static GeometryManager *thePointer;
    GeometryManager();
    detectorMap detMap;
    std::string currentDetectorName;
    void AddDetectorStore(std::string name);
    DetectorStore* GetDetectorStore(std::string name);

    FadsLogicalVolumeNotifier* LVNotifier;
    FadsPhysicalVolumeNotifier* PVNotifier;
  public:
    ~GeometryManager();
    static GeometryManager* GetGeometryManager();
    void RegisterSolid(std::string n,G4VSolid *s);
    void RegisterVolume(G4LogicalVolume *v);
    void RegisterPhysicalVolume(G4VPhysicalVolume *v);
    std::vector<G4LogicalVolume*>& GetVolume(std::string name);
    std::vector<G4LogicalVolume*>& GetVolume(std::string detname,std::string name);
    void RegisterSolid(G4VSolid *v);
    std::vector<G4VSolid*>& GetSolid(std::string name);
    std::vector<G4VSolid*>& GetSolid(std::string detname,std::string name);
    inline void SetCurrentDetectorName(std::string s);
    inline void ResetCurrentDetectorName();
    inline std::string GetCurrentDetectorName();
    void PrintVolumes();
    void PrintSolids();
  };

  void GeometryManager::SetCurrentDetectorName(std::string s)
  {
    currentDetectorName=s;
    if (detMap.find(s)==detMap.end()) AddDetectorStore(s);
  }
  void GeometryManager::ResetCurrentDetectorName()
  {
    currentDetectorName="";
  }
  std::string GeometryManager::GetCurrentDetectorName()
  {
    return currentDetectorName;
  }

} // end namespace
#endif
