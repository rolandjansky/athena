/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Remove objects from temporary background store BkgEvent_2_SG
// before adding new ones
// Piyali.Banerjee@cern.ch February 2011

#ifndef OVERLAYCOMMONALGS_REMOVETEMPBKGOBJECTS_H
#define OVERLAYCOMMONALGS_REMOVETEMPBKGOBJECTS_H

#include "OverlayAlgBase/OverlayAlgBase.h"

class RemoveTempBkgObjects : public OverlayAlgBase  {

public:

  RemoveTempBkgObjects(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

private:

  ServiceHandle<StoreGateSvc> m_tempBkgStore, m_tempSigStore;

  bool m_bkgData;

  template<class TypeToBeRemoved> void checkBeforeRemove(StoreGateSvc *sg);

};

#endif/*OVERLAYCOMMONALGS_REMOVETEMPBKGOBJECTS_H*/
