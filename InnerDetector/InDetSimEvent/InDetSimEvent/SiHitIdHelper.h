/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

// This class is singleton and static method and variable are used.
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

class SiHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static SiHitIdHelper* GetHelper();
  //
  // Info retrieval:
  // Pixel or SCT
  bool isPixel(const int& hid) const;
  bool isSCT(const int& hid) const;

  // Barrel or Endcap
  int getBarrelEndcap(const int& hid) const;

  // Layer/Disk
  int getLayerDisk(const int& hid) const;

  // eta module
  int getEtaModule(const int& hid) const;

  // phi module&
  int getPhiModule(const int& hid) const;

  // side
  int getSide(const int& hid) const;
  //
  // Info packing:
  int buildHitId(const int, const int, const int, const int, const int, const int) const;

 private:
  //
  // private constructor to have a singleton
  SiHitIdHelper();
  //
  // Initialize the helper, only called by the constructor
  void Initialize();
};

#endif // INDETSIMEVENT_SIHITIDHELPER
