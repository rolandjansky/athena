/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

#include "InDetSimEvent/SiHitIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "InDetIdentifier/PixelID.h"

#ifdef G4MULTITHREADED
#  include "GaudiKernel/ContextSpecificPtr.h"
#endif

static std::mutex _sgMutex;

//
// private constructor
SiHitIdHelper::SiHitIdHelper() :HitIdHelper() {
  Initialize();
}

SiHitIdHelper* SiHitIdHelper::GetHelper() {
#ifdef G4MULTITHREADED
  // Context-specific singleton
  static Gaudi::Hive::ContextSpecificPtr<SiHitIdHelper> helperPtr;
  if(!helperPtr) helperPtr = new SiHitIdHelper();
  return helperPtr.get();
#else
  static SiHitIdHelper helper;
  return &helper;
#endif
}

void SiHitIdHelper::Initialize() {

  // determine whether hits were created with an SLHC dictionary
  // in which case eta module field is expanded.
  // Need to lock this thread-unsafe retrieval
  const PixelID* pix;
  StoreGateSvc* detStore(nullptr);
  {
    std::lock_guard<std::mutex> lock(_sgMutex);
    detStore = StoreGate::pointer("DetectorStore");
    if (detStore->retrieve(pix, "PixelID").isFailure()) { pix = 0; }
  }
  bool isSLHC = (pix != 0 && pix->dictionaryVersion() == "SLHC");
  bool isDBM  = (pix != 0 && pix->dictionaryVersion() == "IBL-DBM");
  bool isInclinedAlternative = (pix !=0 &&  pix->dictionaryVersion() == "SLHC_InclinedAlternative");

  if (isSLHC) InitializeField("Part",0,2);
  else InitializeField("Part",0,1);
  if (isDBM) InitializeField("BarrelEndcap",-4,4);
  else InitializeField("BarrelEndcap",-2,2);
  if(isInclinedAlternative) InitializeField("LayerDisk",0,32);
  else InitializeField("LayerDisk",0,20);
  if (isSLHC) InitializeField("EtaModule",-100,100);
  else InitializeField("EtaModule",-20,20);
  InitializeField("PhiModule",0,200);
  InitializeField("Side",0,3);
}

// Info retrieval:
// Pixel or SCT
bool SiHitIdHelper::isPixel(const int& hid){
  this->SetID(hid);
  int ps = this->GetFieldValue("Part");
  if (ps ==0 ) return true;
  else return false;
}

bool SiHitIdHelper::isSCT(const int& hid){
  this->SetID(hid);
  int ps = this->GetFieldValue("Part");
  if (ps ==1 ) return true;
  else return false;
}

bool SiHitIdHelper::isHGTD(const int& hid){
  this->SetID(hid);
  int ps = this->GetFieldValue("Part");
  if (ps ==2 ) return true;
  else return false;
}

// Barrel or Endcap
int SiHitIdHelper::getBarrelEndcap(const int& hid){
  this->SetID(hid);
  return this->GetFieldValue("BarrelEndcap");
}

// Layer/Disk
int SiHitIdHelper::getLayerDisk(const int& hid) {
  this->SetID(hid);
  return this->GetFieldValue("LayerDisk");
}

// eta module
int SiHitIdHelper::getEtaModule(const int& hid)  {
  this->SetID(hid);
  return this->GetFieldValue("EtaModule");
}

// phi module
int SiHitIdHelper::getPhiModule(const int& hid) {
  this->SetID(hid);
  return this->GetFieldValue("PhiModule");
}

// side
int SiHitIdHelper::getSide(const int& hid) {
  this->SetID(hid);
  return this->GetFieldValue("Side");
}


//
// Info packing:
int SiHitIdHelper::buildHitId(const int Part, const int BrlECap, const int LayerDisk,
                              const int etaM, const int phiM, const int side) {
  this->SetID(0);
  this->SetFieldValue("Part", Part);
  this->SetFieldValue("BarrelEndcap",   BrlECap);
  this->SetFieldValue("LayerDisk",      LayerDisk);
  this->SetFieldValue("EtaModule",      etaM);
  this->SetFieldValue("PhiModule",      phiM);
  this->SetFieldValue("Side",           side);
  return this->GetID();
}
