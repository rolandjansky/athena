/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
// Overlaying MM Digits
// alexandre.laurier@cern.ch - 2019.08.23

#ifndef MM_OVERLAY_H
#define MM_OVERLAY_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonOverlayBase/MuonOverlayBase.h"
#include "MuonDigitContainer/MmDigitContainer.h"

class MmIdHelper;
class IMuonDigitizationTool;


class MM_Overlay : public MuonOverlayBase  {
public:
  
  MM_Overlay(const std::string &name,ISvcLocator *pSvcLocator);

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
  std::string m_mainInputMM_Name;
  std::string m_overlayInputMM_Name;
 
  float m_timeIntegrationWindow;
  const MmIdHelper   * m_mmHelper;
  bool m_clean_overlay_data, m_clean_overlay_signal, m_convertRdo;
  ToolHandle<IMuonDigitizationTool> m_digTool;
  ToolHandle<IMuonDigitizationTool> m_rdoTool;
};

#endif/* MM_OVERLAY_H */

