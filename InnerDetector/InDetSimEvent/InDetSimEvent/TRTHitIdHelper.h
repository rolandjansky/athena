/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMEVENT_TRTHITIDHELPER
#define INDETSIMEVENT_TRTHITIDHELPER

//
// This is a helper class to build an identifing integer used by
// the simulation. It inherits from HitIdHelper, in order to get
// all the packing and shifting for free.
// The class is a singleton and a static GetHelper() is provided
// the constructor calls the Initialize() method which sets all the
// field dimensions
// Methods are provided to get access to the TRT Geometry
// description
//


//
// Base Class
#include "HitManagement/HitIdHelper.h"

// This class is singleton and static method and variable are used.
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


class TRTHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static TRTHitIdHelper* GetHelper();
  //
  // Info retrieval:
  // Barrel or Endcap
  bool isBarrel(const int& hid) const;
  bool isEndcap(const int& hid) const;
  //
  // Positive or negative side
  bool isPositive(const int& hid) const;
  bool isNegative(const int& hid) const;

  // Ring or Wheel
  int getRingWheel(const int& hid) const;

  // PhiSector
  int getPhiSector(const int& hid) const;

  // Layer Plane
  int getLayerPlane(const int& hid) const;

  // straw
  int getStraw(const int& hid) const;
  //
  // Info packing:
  int buildHitId(const int, const int, const int, const int, const int, const int) const;

 private:
  //
  // private constructor to have a singleton
  TRTHitIdHelper();
  //
  // Initialize the helper, only called by the constructor
  void Initialize();
};

#endif // INDETSIMEVENT_TRTHITIDHELPER
