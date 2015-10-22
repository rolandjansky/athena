/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Remove objects before adding new ones
//
// Keetvi A. Assamagan

#ifndef OVERLAYCOMMONALGS_UPDATEEVENTINFO_H
#define OVERLAYCOMMONALGS_UPDATEEVENTINFO_H

#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"

class UpdateEventInfo: public OverlayAlgBase  {
public:

  UpdateEventInfo(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  template<class TypeToBeRemoved> void checkBeforeRemove(StoreGateSvc *sg);

protected:
  ServiceHandle<StoreGateSvc> m_storeGateTemp;
  unsigned int m_count;
  std::string m_infoType;
};

#include "UpdateEventInfo.icc"

#endif/*OVERLAYCOMMONALGS_UPDATEEVENTINFO_H*/
