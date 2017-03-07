#ifndef ISFG4GEOHELPER_H
#define ISFG4GEOHELPER_H


#include "AtlasDetDescr/AtlasRegion.h" 
#include "G4Step.hh"
#include "ISF_Interfaces/IGeoIDSvc.h"


namespace iGeant4 {

class ISFG4GeoHelper {

 public:
  ISFG4GeoHelper() = delete;

  static AtlasDetDescr::AtlasRegion nextGeoId(const G4Step* aStep, int truthVolLevel,ISF::IGeoIDSvc *geoIDSvc);
  static bool checkVolumeDepth(G4LogicalVolume* logicalVol, int volLevel, int depth=0);

 private:
};

}
#endif
