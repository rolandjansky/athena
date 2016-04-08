/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


class TRTHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static TRTHitIdHelper* GetHelper();  
  //
  // Info retrieval:
  // Barrel or Endcap
  bool isBarrel(const int& hid);
  bool isEndcap(const int& hid);
  //
  // Positive or negative side 
  bool isPositive(const int& hid);
  bool isNegative(const int& hid);

  // Ring or Wheel
  int getRingWheel(const int& hid);

  // PhiSector
  int getPhiSector(const int& hid);
  
  // Layer Plane
  int getLayerPlane(const int& hid);

  // straw
  int getStraw(const int& hid);
  //
  // Info packing:
  int buildHitId(const int, const int, const int, const int, const int, const int);

 private:
  //
  // private constructor to have a singleton
  TRTHitIdHelper(); 
  //
  // Initialize the helper, only called by the constructor
  void Initialize();  
};

#endif // INDETSIMEVENT_TRTHITIDHELPER
