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

#include "MuonOverlayBase/IDC_MultiHitOverlayBase.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

class TgcIdHelper;

class TgcOverlay : public IDC_MultiHitOverlayBase  {
public:

  TgcOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  /** Framework implemenrtation for the event loop */
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

private:
  // ----------------------------------------------------------------

  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  SG::ReadHandleKey<TgcDigitContainer> m_mainInputDigitKey{this,"MainInputDigitKey","OriginalEvent_SG+TGC_DIGITS","ReadHandleKey for Main Input TgcDigitContainer"};
  SG::ReadHandleKey<TgcDigitContainer> m_overlayInputDigitKey{this,"OverlayInputDigitKey","BkgEvent_0_SG+TGC_DIGITS","ReadHandleKey for Overlay Input TgcDigitContainer"};
  SG::WriteHandleKey<TgcDigitContainer> m_outputDigitKey{this,"OutputDigitKey","StoreGateSvc+TGC_DIGITS","WriteHandleKey for Output TgcDigitContainer"};

  const TgcIdHelper   * m_tgcHelper{nullptr};

};

#endif/* TGCOVERLAY_H */
