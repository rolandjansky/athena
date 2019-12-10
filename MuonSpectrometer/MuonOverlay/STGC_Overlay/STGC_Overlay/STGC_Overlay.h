/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Overlaying sTgcDigits
// alexandre.laurier@cern.ch - 2019-08-23

#ifndef MDTOVERLAY_H
#define MDTOVERLAY_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonOverlayBase/MuonOverlayBase.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

class sTgcIdHelper;
class IMuonDigitizationTool;


class STGC_Overlay : public MuonOverlayBase  {
public:
  
  STGC_Overlay(const std::string &name,ISvcLocator *pSvcLocator);

  /** Framework implemenrtation for the event loop */  
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  float timeIntegrationWindow() const { return m_timeIntegrationWindow; }
  
private:
  // ----------------------------------------------------------------

  ServiceHandle<StoreGateSvc> m_storeGateTemp;
  ServiceHandle<StoreGateSvc> m_storeGateTempBkg;
  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  std::string m_mainInputSTGC_Name;
  std::string m_overlayInputSTGC_Name;
 
  float m_timeIntegrationWindow;
  const sTgcIdHelper   * m_stgcHelper;
  bool m_clean_overlay_data, m_clean_overlay_signal, m_convertRdo;
  ToolHandle<IMuonDigitizationTool> m_digTool;
  ToolHandle<IMuonDigitizationTool> m_rdoTool;
};

#endif/* STGC_OVERLAY_H */
