/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Ketevi A. Assamagan, October 2009

#ifndef OVERLAYCOMMONALGS_COPYOBJECTS_H
#define OVERLAYCOMMONALGS_COPYOBJECTS_H


#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"

class CopyObjects : public OverlayAlgBase  {
public:

  CopyObjects(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  template<class TypeToBeCopied, class Datum> void copyAthenaHitObject(StoreGateSvc* from, StoreGateSvc *to);
  template<class TypeToBeCopied, class Datum> void copySDOobject(StoreGateSvc* from, StoreGateSvc *to);
  template<class TypeToBeCopied, class Datum> void copyMuonSDOobject(StoreGateSvc *from, StoreGateSvc *to, std::string k);

protected:

  ServiceHandle<StoreGateSvc> m_storeGateTemp;

private:

  bool m_sctType;
  bool m_indet;
  bool m_muon;
  bool m_truth;

};

#include "CopyObjects.icc"

#endif/*OVERLAYCOMMONALGS_COPYOBJECTS_H*/
