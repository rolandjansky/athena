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

#include "MuonOverlayBase/MuonOverlayBase.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

class MdtIdHelper;


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

  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  SG::ReadHandleKey<MdtDigitContainer> m_mainInputDigitKey{this,"MainInputDigitKey","OriginalEvent_SG+MDT_DIGITS","ReadHandleKey for Main Input MdtDigitContainer"};
  SG::ReadHandleKey<MdtDigitContainer> m_overlayInputDigitKey{this,"OverlayInputDigitKey","BkgEvent_0_SG+MDT_DIGITS","ReadHandleKey for Overlay Input MdtDigitContainer"};
  SG::WriteHandleKey<MdtDigitContainer> m_outputDigitKey{this,"OutputDigitKey","StoreGateSvc+MDT_DIGITS","WriteHandleKey for Output MdtDigitContainer"};

  float m_adcIntegrationWindow{20.0}; // in ns
  const MdtIdHelper   * m_mdtHelper{nullptr};
  bool m_clean_overlay_data{false};
  bool m_clean_overlay_signal{false};
};

#endif/* MDTOVERLAY_H */
