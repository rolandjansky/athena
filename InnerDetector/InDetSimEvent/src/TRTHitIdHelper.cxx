/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTHitIdHelper.h"

// This class is singleton and static method and variable are used.
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

//
// private constructor
TRTHitIdHelper::TRTHitIdHelper() :HitIdHelper() {
  Initialize();
}

TRTHitIdHelper* TRTHitIdHelper::GetHelper() {
  static TRTHitIdHelper helper;
  return &helper;
}

void TRTHitIdHelper::Initialize() {
  InitializeField("Straw",0,31);
  InitializeField("LayerPlane",0,31);
  InitializeField("PhiSector",0,31);
  InitializeField("RingWheel",0,31);
  InitializeField("PositiveNegative",0,1);
  InitializeField("BarrelEndcap",0,1);
}

  // Info retrieval:
  // Barrel or Endcap
bool TRTHitIdHelper::isBarrel(const int& hid) const {
  int ps = this->GetFieldValue("BarrelEndcap", hid);
  if (ps ==0 ) return true;
  else return false;
}

bool TRTHitIdHelper::isEndcap(const int& hid) const {
  int ps = this->GetFieldValue("BarrelEndcap", hid);
  if (ps ==0 ) return false;
  else return true;
}

// Positive or Negative
bool TRTHitIdHelper::isPositive(const int& hid) const {
  int ps = this->GetFieldValue("PositiveNegative", hid);
  if (ps ==0 ) return true;
  else return false;
}
bool TRTHitIdHelper::isNegative(const int& hid) const {
  int ps = this->GetFieldValue("PositiveNegative", hid);
  if (ps ==0 ) return false;
  else return true;
}

// Ring/Wheel
int TRTHitIdHelper::getRingWheel(const int& hid) const {
  return this->GetFieldValue("RingWheel", hid);
}

// Phi Sector
int TRTHitIdHelper::getPhiSector(const int& hid) const {
  return this->GetFieldValue("PhiSector", hid);
}

// layer/plane
int TRTHitIdHelper::getLayerPlane(const int& hid) const {
  return this->GetFieldValue("LayerPlane", hid);
}

// straw
int TRTHitIdHelper::getStraw(const int& hid) const {
  return this->GetFieldValue("Straw", hid);
}


//
// Info packing:
int TRTHitIdHelper::buildHitId(const int BrlECap, const int PosNeg, const int RingWheel,
                              const int PhiS, const int LayerPlane, const int Straw) const {
  int theID(0);
  this->SetFieldValue("BarrelEndcap",     BrlECap, theID);
  this->SetFieldValue("PositiveNegative", PosNeg, theID);
  this->SetFieldValue("RingWheel",        RingWheel, theID);
  this->SetFieldValue("PhiSector",        PhiS, theID);
  this->SetFieldValue("LayerPlane",       LayerPlane, theID);
  this->SetFieldValue("Straw",            Straw, theID);
  return theID;
}
