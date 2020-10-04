/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "GeoAdaptors/GeoSiHit.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


const InDetDD::PixelDetectorManager* GeoSiHit::initPixMgr()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoSiHit");
  const InDetDD::PixelDetectorManager* pix = nullptr;
  if(detStore->retrieve(pix,"Pixel").isFailure()){
    //if Pixel retrieval fails, try ITkPixel
    if(detStore->retrieve(pix,"ITkPixel").isFailure()) 
    {
      std::abort();
    }
  }
  return pix;
}


const InDetDD::SCT_DetectorManager* GeoSiHit::initSctMgr()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoSiHit");
  const InDetDD::SCT_DetectorManager* sct = nullptr;
  if(detStore->retrieve(sct,"SCT").isFailure()){
    if(detStore->retrieve(sct,"ITkStrip").isFailure()) {
      std::abort();
    }
  }
  return sct;
}


const PixelID* GeoSiHit::initPixID()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoSiHit");
  const PixelID* pix = nullptr;
  if(detStore->retrieve(pix,"PixelID").isFailure()) {
    std::abort();
  }
  return pix;
}


const SCT_ID* GeoSiHit::initSctID()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoSiHit");
  const SCT_ID* sct = nullptr;
  if(detStore->retrieve(sct,"SCT_ID").isFailure()) {
    std::abort();
  }
  return sct;
}


const InDetDD::PixelDetectorManager* GeoSiHit::pixMgr() const
{
  const InDetDD::PixelDetectorManager* const mgr = initPixMgr();
  return mgr;
}


const InDetDD::SCT_DetectorManager*  GeoSiHit::sctMgr() const
{
  const InDetDD::SCT_DetectorManager* const mgr = initSctMgr();
  return mgr;
}


const PixelID*                       GeoSiHit::pixID()  const
{
  const PixelID* const id = initPixID();
  return id;
}


const SCT_ID*                        GeoSiHit::sctID()  const
{
  const SCT_ID* const id = initSctID();
  return id;
}

