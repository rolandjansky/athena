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

#include <MuonDigitContainer/TgcDigitContainer.h>
#include <MuonOverlayBase/IDC_MuonOverlayBase.h>


class TgcOverlay : public IDC_MuonOverlayBase
{
public:

  TgcOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  SG::ReadHandleKey<TgcDigitContainer> m_bkgInputKey{ this, "BkgInputKey", "OriginalEvent_SG+TGC_DIGITS", "ReadHandleKey for Background Input TgcDigitContainer" };
  SG::ReadHandleKey<TgcDigitContainer> m_signalInputKey{ this, "SignalInputKey", "BkgEvent_0_SG+TGC_DIGITS", "ReadHandleKey for Signal Input TgcDigitContainer" };
  SG::WriteHandleKey<TgcDigitContainer> m_outputKey{ this, "OutputKey", "StoreGateSvc+TGC_DIGITS", "WriteHandleKey for Output TgcDigitContainer" };

};

#endif/* TGCOVERLAY_H */
