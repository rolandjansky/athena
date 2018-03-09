/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOMMONALGS_COPYTRUTHINFO_H
#define OVERLAYCOMMONALGS_COPYTRUTHINFO_H


#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"

class CopyTruthInfo : public OverlayAlgBase {
public:

  CopyTruthInfo(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();
};

#endif/*OVERLAYCOMMONALGS_COPYTRUTHINFO_H*/
