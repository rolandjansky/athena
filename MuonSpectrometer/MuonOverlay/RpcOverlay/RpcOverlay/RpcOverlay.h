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

#include "MuonOverlayBase/IDC_MultiHitOverlayBase.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

class RpcIdHelper;

class RpcOverlay : public IDC_MultiHitOverlayBase  {
public:

  RpcOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  /** Framework implemenrtation for the event loop */
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

private:
  // ----------------------------------------------------------------

  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  SG::ReadHandleKey<RpcDigitContainer> m_mainInputDigitKey{this,"MainInputDigitKey","OriginalEvent_SG+RPC_DIGITS","ReadHandleKey for Main Input RpcDigitContainer"};
  SG::ReadHandleKey<RpcDigitContainer> m_overlayInputDigitKey{this,"OverlayInputDigitKey","BkgEvent_0_SG+RPC_DIGITS","ReadHandleKey for Overlay Input RpcDigitContainer"};
  SG::WriteHandleKey<RpcDigitContainer> m_outputDigitKey{this,"OutputDigitKey","StoreGateSvc+RPC_DIGITS","WriteHandleKey for Output RpcDigitContainer"};

  const RpcIdHelper   * m_rpcHelper{nullptr};

};

#endif/* RPCOVERLAY_H */
