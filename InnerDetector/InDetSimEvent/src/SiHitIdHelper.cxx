/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

#include "InDetSimEvent/SiHitIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "GaudiKernel/ServiceHandle.h"

#include "G4Types.hh"
#ifdef G4MULTITHREADED
#  include "GaudiKernel/ContextSpecificPtr.h"
#endif

//
// private constructor
SiHitIdHelper::SiHitIdHelper() :HitIdHelper() {
  Initialize();
}

const SiHitIdHelper* SiHitIdHelper::GetHelper() {
  #ifdef G4MULTITHREADED
  // Context-specific singleton
  static Gaudi::Hive::ContextSpecificPtr<const SiHitIdHelper> helperPtr ATLAS_THREAD_SAFE;
  if (!helperPtr) helperPtr = new SiHitIdHelper();
  return helperPtr.get();
  #else
  static const SiHitIdHelper helper;
  return &helper;
  #endif
}

void SiHitIdHelper::Initialize() {

  // determine whether hits were created with an SLHC dictionary
  // in which case eta module field is expanded.
  const PixelID* pix = nullptr;
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "SiHitIdHelper");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->retrieve(pix, "PixelID").isFailure()) { pix = 0; }
  }
  bool isSLHC = (pix != 0 && pix->dictionaryVersion() == "SLHC");
  bool isDBM  = (pix != 0 && pix->dictionaryVersion() == "IBL-DBM");

  InitializeField("PixelSCT",0,1);
  if (isDBM) InitializeField("BarrelEndcap",-4,4);
  else InitializeField("BarrelEndcap",-2,2);
  if(!isSLHC)InitializeField("LayerDisk",0,20);
  else InitializeField("LayerDisk",0,50);
  if (isSLHC) InitializeField("EtaModule",-100,100);
  else InitializeField("EtaModule",-20,20);
  InitializeField("PhiModule",0,200);
  InitializeField("Side",0,3);
}

// Info retrieval:
// Pixel or SCT
bool SiHitIdHelper::isPixel(const int& hid) const
{
  int ps = this->GetFieldValue("PixelSCT", hid);
  if (ps ==0 ) return true;
  else return false;
}

bool SiHitIdHelper::isSCT(const int& hid) const
{
  int ps = this->GetFieldValue("PixelSCT", hid);
  if (ps ==0 ) return false;
  else return true;
}

// Barrel or Endcap
int SiHitIdHelper::getBarrelEndcap(const int& hid) const
{
  return this->GetFieldValue("BarrelEndcap", hid);
}

// Layer/Disk
int SiHitIdHelper::getLayerDisk(const int& hid) const
{
  return this->GetFieldValue("LayerDisk", hid);
}

// eta module
int SiHitIdHelper::getEtaModule(const int& hid) const
{
  return this->GetFieldValue("EtaModule", hid);
}

// phi module
int SiHitIdHelper::getPhiModule(const int& hid) const
{
  return this->GetFieldValue("PhiModule", hid);
}

// side
int SiHitIdHelper::getSide(const int& hid) const
{
  return this->GetFieldValue("Side", hid);
}


//
// Info packing:
int SiHitIdHelper::buildHitId(const int Pixel_SCT, const int BrlECap, const int LayerDisk,
                              const int etaM, const int phiM, const int side) const
{
  int theID(0);
  this->SetFieldValue("PixelSCT",       Pixel_SCT, theID);
  this->SetFieldValue("BarrelEndcap",   BrlECap, theID);
  this->SetFieldValue("LayerDisk",      LayerDisk, theID);
  this->SetFieldValue("EtaModule",      etaM, theID);
  this->SetFieldValue("PhiModule",      phiM, theID);
  this->SetFieldValue("Side",           side, theID);
  return theID;
}
