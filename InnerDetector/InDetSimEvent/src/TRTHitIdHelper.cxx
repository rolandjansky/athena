/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTHitIdHelper.h"

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
bool TRTHitIdHelper::isBarrel(const int& hid){
  this->SetID(hid);
  int ps = this->GetFieldValue("BarrelEndcap");
  if (ps ==0 ) return true;
  else return false;
}

bool TRTHitIdHelper::isEndcap(const int& hid){
  this->SetID(hid);
  int ps = this->GetFieldValue("BarrelEndcap");
  if (ps ==0 ) return false;
  else return true;
}  

// Positive or Negative
bool TRTHitIdHelper::isPositive(const int& hid){
  this->SetID(hid);
  int ps = this->GetFieldValue("PositiveNegative");
  if (ps ==0 ) return true;
  else return false;
}  
bool TRTHitIdHelper::isNegative(const int& hid){
  this->SetID(hid);
  int ps = this->GetFieldValue("PositiveNegative");
  if (ps ==0 ) return false;
  else return true;
}  

// Ring/Wheel
int TRTHitIdHelper::getRingWheel(const int& hid) {
  this->SetID(hid);
  return this->GetFieldValue("RingWheel");
}  

// Phi Sector
int TRTHitIdHelper::getPhiSector(const int& hid)  {
  this->SetID(hid);
  return this->GetFieldValue("PhiSector");
}

// layer/plane
int TRTHitIdHelper::getLayerPlane(const int& hid) {
  this->SetID(hid);
  return this->GetFieldValue("LayerPlane");
}
  
// straw
int TRTHitIdHelper::getStraw(const int& hid) {
  this->SetID(hid);
  return this->GetFieldValue("Straw");
}


//
// Info packing:
int TRTHitIdHelper::buildHitId(const int BrlECap, const int PosNeg, const int RingWheel,
			      const int PhiS, const int LayerPlane, const int Straw) {
  this->SetID(0);
  this->SetFieldValue("BarrelEndcap",     BrlECap);
  this->SetFieldValue("PositiveNegative", PosNeg);
  this->SetFieldValue("RingWheel",        RingWheel);
  this->SetFieldValue("PhiSector",        PhiS);
  this->SetFieldValue("LayerPlane",       LayerPlane);
  this->SetFieldValue("Straw",            Straw);
  return this->GetID();
}


