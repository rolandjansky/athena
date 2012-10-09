/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Remove objects from temporary background store BkgEvent_2_SG
// before adding new ones
// Piyali.Banerjee@cern.ch February 2011

#ifndef REMOVETEMPBKGOBJECTS_H
#define REMOVETEMPBKGOBJECTS_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

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

#endif/*REMOVETEMPBKGOBJECTS_H*/
