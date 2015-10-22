/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Remove objects before adding new ones
//
// Ketevi A. Assamagan

#ifndef OVERLAYCOMMONALGS_REMOVEOBJECTS_H
#define OVERLAYCOMMONALGS_REMOVEOBJECTS_H

#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"

class RemoveObjects : public OverlayAlgBase  {
public:

  RemoveObjects(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

private:
  bool m_removeTileMC;
  bool m_removeTriggerTowerMC;
  bool m_removeLArMC;

};

#endif/*OVERLAYCOMMONALGS_REMOVEOBJECTS_H*/
