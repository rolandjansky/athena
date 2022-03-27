/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

#include "InDetSimEvent/SiHitIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "GaudiKernel/ServiceHandle.h"


//
// private constructor
SiHitIdHelper::SiHitIdHelper() :HitIdHelper() {
  Initialize();
}

const SiHitIdHelper* SiHitIdHelper::GetHelper() {
  static const SiHitIdHelper helper;
  return &helper;
}

void SiHitIdHelper::Initialize() {

  const PixelID* pix = nullptr;
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "SiHitIdHelper");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->retrieve(pix, "PixelID").isFailure()) { pix = nullptr; }
  }

  bool isDBM  = (pix != nullptr && pix->dictionaryVersion() == "IBL-DBM");
  // check for ITk and HGTD
  bool isITkHGTD = (pix !=nullptr &&  pix->dictionaryVersion() == "ITkHGTD");
  // we might include PLR as well, then we have to increase endcap range to +/- 4
  bool isITkHGTDPLR = (pix !=nullptr &&  pix->dictionaryVersion() == "ITkHGTDPLR");
  // cache the HL-LHC decision
  m_isITkHGTD = isITkHGTD || isITkHGTDPLR;

  if (m_isITkHGTD) InitializeField("Part",0,2);
  else InitializeField("Part",0,1);
  if (isDBM || isITkHGTDPLR) InitializeField("BarrelEndcap",-4,4);
  else InitializeField("BarrelEndcap",-2,2);
  InitializeField("LayerDisk",0,20);
  if (m_isITkHGTD) InitializeField("EtaModule",-100,100);
  else InitializeField("EtaModule",-20,20);
  InitializeField("PhiModule",0,200);
  InitializeField("Side",0,3);

}

// Info retrieval:
// Pixel, SCT, HGTD, or PLR
bool SiHitIdHelper::isPixel(const int& hid) const
{
  int psh = this->GetFieldValue("Part", hid);
  if (psh ==0 ) return true;
  else return false;
}

bool SiHitIdHelper::isSCT(const int& hid) const
{
  int psh = this->GetFieldValue("Part", hid);
  if (psh ==1 ) return true;
  else return false;
}

bool SiHitIdHelper::isHGTD(const int& hid) const
{
  int psh = this->GetFieldValue("Part", hid);
  if (psh ==2 ) return true;
  else return false;
}

bool SiHitIdHelper::isPLR(const int& hid) const
{
  if (!m_isITkHGTD) return false;

  int psh = this->GetFieldValue("BarrelEndcap", hid);
  if (std::abs(psh) == 4) return true;
  else return false;
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
int SiHitIdHelper::buildHitId(const int Part, const int BrlECap, const int LayerDisk,
                              const int etaM, const int phiM, const int side) const
{
  int theID(0);
  this->SetFieldValue("Part",           Part, theID);
  this->SetFieldValue("BarrelEndcap",   BrlECap, theID);
  this->SetFieldValue("LayerDisk",      LayerDisk, theID);
  this->SetFieldValue("EtaModule",      etaM, theID);
  this->SetFieldValue("PhiModule",      phiM, theID);
  this->SetFieldValue("Side",           side, theID);
  return theID;
}
