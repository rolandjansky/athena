/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOMMONALGS_COPYTIMINGS_H
#define OVERLAYCOMMONALGS_COPYTIMINGS_H


#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"

class CopyTimings : public OverlayAlgBase {
public:

  CopyTimings(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();
};

#endif/*OVERLAYCOMMONALGS_COPYTIMINGS_H*/
