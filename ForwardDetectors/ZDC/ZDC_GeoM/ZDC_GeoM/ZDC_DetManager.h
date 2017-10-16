/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_DETMANAGER_H
#define ZDC_DETMANAGER_H

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "AthenaKernel/MsgStreamMember.h"

class ZDC_DetManager : public GeoVDetectorManager
{
 public:
  
  ZDC_DetManager();
  ~ZDC_DetManager();
  
  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink  getTreeTop(unsigned int i) const;
  
  void addTreeTop(PVLink);
  
  MsgStream& msg     (MSG::Level lvl) const { return m_msg               << lvl; }
  bool       msgLevel(MSG::Level lvl)       { return m_msg.get().level() <= lvl; }
  
 private:
  
  mutable Athena::MsgStreamMember m_msg;

  std::vector<PVLink> m_volume;
};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ZDC_DetManager, 9886, 1)
#endif

#endif
