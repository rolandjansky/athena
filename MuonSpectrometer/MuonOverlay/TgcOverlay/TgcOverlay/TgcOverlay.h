/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Overlaying TgcDigits from two different events for TGC subdetectors.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008 

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#ifndef TGCOVERLAY_H
#define TGCOVERLAY_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonOverlayBase/IDC_MultiHitOverlayBase.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

class TgcIdHelper;
class IMuonDigitizationTool;

class TgcOverlay : public IDC_MultiHitOverlayBase  {
public:
  
  TgcOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  /** Framework implemenrtation for the event loop */  
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

private:
  // ----------------------------------------------------------------

  ServiceHandle<StoreGateSvc> m_storeGateTemp;
  ServiceHandle<StoreGateSvc> m_storeGateTempBkg;
  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  std::string m_mainInputTGC_Name;
  std::string m_overlayInputTGC_Name;

  const TgcIdHelper   * m_tgcHelper;

  ToolHandle<IMuonDigitizationTool> m_digTool;
  ToolHandle<IMuonDigitizationTool> m_rdoTool;

};

#endif/* TGCOVERLAY_H */
