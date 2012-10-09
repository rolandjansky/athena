/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Remove objects before adding new ones
//
// Ketevi A. Assamagan

#ifndef REMOVEOBJECTS_H
#define REMOVEOBJECTS_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

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

#endif/*REMOVEOBJECTS_H*/
