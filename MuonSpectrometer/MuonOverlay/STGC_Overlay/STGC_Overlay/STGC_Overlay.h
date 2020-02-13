/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Overlaying sTgcDigits from two different events for sTgc subdetectors.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008 
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011
// Alexandre Laurier <alexandre.laurier@cern.ch>, May 2019

#ifndef STGCOVERLAY_H
#define STGCOVERLAY_H

#include <string>

#include <MuonOverlayBase/IDC_MuonOverlayBase.h>
#include <MuonDigitContainer/sTgcDigitContainer.h>

class STGC_Overlay : public IDC_MuonOverlayBase
{
public:
  STGC_Overlay(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

  float timeIntegrationWindow() const { return m_timeIntegrationWindow.value(); }

private:
  SG::ReadHandleKey<sTgcDigitContainer> m_bkgInputKey{this, "BkgInputKey", "Bkg_sTGC_DIGITS", "ReadHandleKey for Background Input sTgcDigitContainer"};
  SG::ReadHandleKey<sTgcDigitContainer> m_signalInputKey{this, "SignalInputKey", "Sig_sTGC_DIGITS", "ReadHandleKey for Signal Input sTgcDigitContainer"};
  SG::WriteHandleKey<sTgcDigitContainer> m_outputKey{this, "OutputKey", "sTGC_DIGITS", "WriteHandleKey for Output sTgcDigitContainer"};

  FloatProperty m_timeIntegrationWindow { this, "IntegrationWindow", 50.0, "Time Integration Window" };

};

#endif/* STGCOVERLAY_H */
