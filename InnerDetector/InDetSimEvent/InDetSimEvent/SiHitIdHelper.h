/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMEVENT_SIHITIDHELPER
#define INDETSIMEVENT_SIHITIDHELPER

//
// This is a helper class to build an identifing integer used by 
// the simulation. It inherits from HitIdHelper, in order to get
// all the packing and shifting for free. 
// The class is a singleton and a static GetHelper() is provided 
// the constructor calls the Initialize() method which sets all the 
// field dimensions
// Methods are provided to get access to the SiTracker Geometry
// description
//


//
// Base Class
#include "HitManagement/HitIdHelper.h"


class SiHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static SiHitIdHelper* GetHelper();  
  //
  // Info retrieval:
  // Pixel, SCT, or HGTD
  bool isPixel(const int& hid);
  bool isSCT(const int& hid);
  bool isHGTD(const int& hid);
  
  // Barrel or Endcap
  int getBarrelEndcap(const int& hid);

  // Layer/Disk
  int getLayerDisk(const int& hid);
  
  // eta module
  int getEtaModule(const int& hid);

  // phi module&
  int getPhiModule(const int& hid);
  
  // side
  int getSide(const int& hid);
  //
  // Info packing:
  int buildHitId(const int, const int, const int, const int, const int, const int);

 private:
  //
  // private constructor to have a singleton
  SiHitIdHelper(); 
  //
  // Initialize the helper, only called by the constructor
  void Initialize();  
};

#endif // INDETSIMEVENT_SIHITIDHELPER
