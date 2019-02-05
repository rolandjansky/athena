/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Overlaying RpcDigits from two different events for RPC subdetectors.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008 

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#ifndef RPCOVERLAY_H
#define RPCOVERLAY_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonOverlayBase/IDC_MultiHitOverlayBase.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

class RpcIdHelper;
class IMuonDigitizationTool;

class RpcOverlay : public IDC_MultiHitOverlayBase  {
public:
  
  RpcOverlay(const std::string &name,ISvcLocator *pSvcLocator);

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
  std::string m_mainInputRPC_Name;
  std::string m_overlayInputRPC_Name;

  const RpcIdHelper   * m_rpcHelper;

  ToolHandle<IMuonDigitizationTool> m_digTool;
  ToolHandle<IMuonDigitizationTool> m_rdoTool;

};

#endif/* RPCOVERLAY_H */
