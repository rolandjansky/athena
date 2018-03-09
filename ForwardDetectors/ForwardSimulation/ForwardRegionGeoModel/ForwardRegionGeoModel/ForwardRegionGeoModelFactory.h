/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardRegionGeoModelFactory_h
#define ForwardRegionGeoModelFactory_h 1
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "ForwardRegionGeoModel/ForwardRegionGeoModelManager.h"

#include "ForwardRegionGeoModel/IForwardRegionProperties.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>
#include <map>

class StoreGateSvc;

typedef struct _FWD_CONFIGURATION
{
    double TCL4JawDistB1I;
    double TCL5JawDistB1I;
    double TCL6JawDistB1I;
    double TCL4JawDistB1O;
    double TCL5JawDistB1O;
    double TCL6JawDistB1O;
    double TCL4JawDistB2I;
    double TCL5JawDistB2I;
    double TCL6JawDistB2I;
    double TCL4JawDistB2O;
    double TCL5JawDistB2O;
    double TCL6JawDistB2O;
    bool buildTCL4;
    bool buildTCL6;
    bool vp1Compatibility;
    bool ALFAInNewPosition;
    double newPosB7L1;
    double newPosB7R1;
    double posAFPL1;
    double posAFPL2;
    double posAFPR1;
    double posAFPR2;

    void clear();
} FWD_CONFIGURATION, *PFWD_CONFIGURATION;

class ForwardRegionGeoModelFactory : public GeoVDetectorFactory  {

 public:
  
  // Constructor:
  ForwardRegionGeoModelFactory(StoreGateSvc *pDetStore, const PFWD_CONFIGURATION pConfig);
  
  // Destructor:
  ~ForwardRegionGeoModelFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const ForwardRegionGeoModelManager * getDetectorManager() const;
  
 private:

  StoreGateSvc             *m_detectorStore;

  // map of defined materials
  std::map<std::string,const GeoMaterial*> m_MapMaterials;

  // property configuration
  FWD_CONFIGURATION m_Config;
  FWDMg_CONFIGURATION m_MagConfig;

  ToolHandle<IForwardRegionProperties> m_properties; // properties of mag. field tool

  // initialization of the material map
  void DefineMaterials();

  // construct elements for given beam
  void constructElements(GeoPhysVol *fwrPhys, std::vector<std::vector<std::string> > loadedDataFile, int beam);

  // Elements
  GeoPhysVol* insertMagnetEnvelope(std::string name, double x, double y, double z, double rotationAngle, double diameter, double halfL, double dL, GeoPhysVol* fwrPhys);
  void insertCircularElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys);
  void insertEllipticalElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys);
  void insertXRecticircularElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys);
  void insertYRecticircularElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys);
  void insertTrousersElement(std::string name, double x, double y, double z, double rotationAngle, GeoPhysVol* fwrPhys);
  void insertTCLElement(std::string name, double x, double y, double z, GeoPhysVol* fwrPhys, double TCLJawDistO, double TCLJawDistI, bool tungstenInsteadOfCopper = false);

  // Load data from file into 2D array of strings
  std::vector<std::vector<std::string> > loadDataFile(char * fileName, int cols);

  // convert number to string
  template <class T>
  std::string num2str(T);

  // signum
  template <typename T> int sgn(T val);
  
  // Illegal operations:
  const ForwardRegionGeoModelFactory & operator=(const ForwardRegionGeoModelFactory &right);
  ForwardRegionGeoModelFactory(const ForwardRegionGeoModelFactory &right);
  
  // The manager:
  ForwardRegionGeoModelManager       *m_detectorManager;
};

// Class ForwardRegionGeoModelFactory
#endif


