/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Overlaying MdtDigits from two different events for MDT subdetectors.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008 

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#ifndef MDTOVERLAY_H
#define MDTOVERLAY_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonOverlayBase/MuonOverlayBase.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

class MdtIdHelper;
class IMuonDigitizationTool;


class MdtOverlay : public MuonOverlayBase  {
public:
  
  MdtOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  /** Framework implemenrtation for the event loop */  
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  float adcIntegrationWindow() const { return m_adcIntegrationWindow; }
  
private:
  // ----------------------------------------------------------------

  ServiceHandle<StoreGateSvc> m_storeGateTemp;
  ServiceHandle<StoreGateSvc> m_storeGateTempBkg;
  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  std::string m_mainInputMDT_Name;
  std::string m_overlayInputMDT_Name;
 
  float m_adcIntegrationWindow;
  const MdtIdHelper   * m_mdtHelper;
  bool m_clean_overlay_data, m_clean_overlay_signal;
  ToolHandle<IMuonDigitizationTool> m_digTool;
  ToolHandle<IMuonDigitizationTool> m_rdoTool;
};

#endif/* MDTOVERLAY_H */
