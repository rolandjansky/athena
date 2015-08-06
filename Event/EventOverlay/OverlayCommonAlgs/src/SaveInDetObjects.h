/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Ketevi A. Assamagan, October 2009

#ifndef OVERLAYCOMMONALGS_SAVEINDETOBJECTS_H
#define OVERLAYCOMMONALGS_SAVEINDETOBJECTS_H


#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"

class SaveInDetObjects : public OverlayAlgBase  {
public:

  SaveInDetObjects(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  template<class TypeToBeCopied, class Datum> void copyInDetIDCobject(StoreGateSvc* from, StoreGateSvc *to);

protected:

  ServiceHandle<StoreGateSvc> m_storeGateTemp;

private:

  bool m_sctType;

};

#include "SaveInDetObjects.icc"

#endif/*OVERLAYCOMMONALGS_COPYOBJECTS_H*/
