/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PIXELBARRELSURVEYUTILS_H__
#define __PIXELBARRELSURVEYUTILS_H__

#include <iostream>
#include <string>
#include <CLHEP/Geometry/Transform3D.h>

namespace PixelBarrelSurveyUtils {

#define ModulesOnStave 13

struct moduleStruct {
  long long int serialNumber;
  std::string offlineID;
  unsigned int hashID;
  double xcenter[3];
  double angles[3];
  double disto[3];
};

typedef struct moduleStruct ModuleStruct;

struct staveStruct {
  long long int serialNumber;
  std::string position;
  double sphereA[3];   // A side sphere coordinates in ATLAS frame
  double sphereC[3];   // C side sphere coordinates in ATLAS frame
  double sphereA1[3];  // A side sphere coordinates in Stave frame
  double sphereC1[3];  // C side sphere coordinates in Stave frame
  double plane[4];     // stave plane (y=0 in Stave frame) in ATLAS frame
  ModuleStruct module[ModulesOnStave];
};

typedef struct staveStruct StaveStruct;

// Read one module line. 
// Return 0 if error or eof is encountered while reading data
ModuleStruct* ReadModule(std::istream&);
// Read one stave information. 
// Return 0 if error or eof is encountered while reading data
StaveStruct*  ReadStave(std::istream&);
// Write the distorsion parameter of a module.
bool OutputDistorsion(std::ostream&, ModuleStruct&);


HepGeom::Transform3D GetModuleTransform(ModuleStruct&);
HepGeom::Transform3D GetStaveTransform(StaveStruct&);
// eta_index goes from -6 (module M6C) to +6 (module M6A)
HepGeom::Transform3D GetModuleTransform(StaveStruct&,int eta_index);

}
#endif

