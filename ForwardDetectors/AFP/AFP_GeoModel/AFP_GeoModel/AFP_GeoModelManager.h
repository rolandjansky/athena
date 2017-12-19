#ifndef AFP_GeoModelManager_h
#define AFP_GeoModelManager_h 1
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"

class AFP_GeoModelManager : public GeoVDetectorManager  {

 public:
  // Constructor
  AFP_GeoModelManager();

  // Destructor
  ~AFP_GeoModelManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  
  // Access to raw geometry:
  virtual PVConstLink getTreeTop(unsigned int i) const;


  // Add a Tree top:
  void addTreeTop(PVLink);


  private:  


  const AFP_GeoModelManager & operator=(const AFP_GeoModelManager &right);
  AFP_GeoModelManager(const AFP_GeoModelManager &right);
  std::vector<PVLink> volume;
};

// Class AFP_GeoModelManager

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(AFP_GeoModelManager, 156328970, 1)
#endif



#endif


