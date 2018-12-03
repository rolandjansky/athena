/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Copy McEventCollection in overlaying jobs.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#ifndef OVERLAYCOMMONALGS_COPYMCEVENTCOLLECTION_H
#define OVERLAYCOMMONALGS_COPYMCEVENTCOLLECTION_H

#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCnvInterfaces/IEventInfoCnvTool.h"

class CopyMcEventCollection : public OverlayAlgBase  {
public:

  CopyMcEventCollection(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

protected:
  std::string m_infoType;
  bool m_realdata;
  bool m_checkeventnumbers;
  bool m_removeBkgHardScatterTruth; //flag to clean up neutrino info from premixed bkg rdos
  ServiceHandle<StoreGateSvc> m_storeGateData2;
  ToolHandle< xAODMaker::IEventInfoCnvTool > m_cnvTool;

};

#endif/*OVERLAYCOMMONALGS_COPYMCEVENTCOLLECTION_H*/
