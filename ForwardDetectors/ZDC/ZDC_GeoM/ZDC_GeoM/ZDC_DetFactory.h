/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_DETFACTORY_H
#define ZDC_DETFACTORY_H

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "ZDC_GeoM/ZDC_DetManager.h"
#include "AthenaKernel/MsgStreamMember.h"

class StoreGateSvc;

class ZDC_DetFactory : public GeoVDetectorFactory
{
 
 public:
  
  ZDC_DetFactory(StoreGateSvc*);
  ~ZDC_DetFactory();
  
  virtual void create(GeoPhysVol* world);
  virtual const ZDC_DetManager* getDetectorManager() const;

  MsgStream& msg     (MSG::Level lvl) const { return m_msg               << lvl; }
  bool       msgLevel(MSG::Level lvl)       { return m_msg.get().level() <= lvl; }

 private:
  
  mutable Athena::MsgStreamMember m_msg;

  int Make_Pixel_ID(int Pid = -1);

  ZDC_DetManager* m_detectorManager;
  StoreGateSvc*   m_detectorStore;
};

#endif

