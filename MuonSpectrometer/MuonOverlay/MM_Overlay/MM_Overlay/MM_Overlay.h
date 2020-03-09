/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Overlaying MmDigits from two different events for MM subdetectors.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008 
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011
// Alexandre Laurier <alexandre.laurier@cern.ch>, May 2019

#ifndef MMOVERLAY_H
#define MMOVERLAY_H

#include <string>

#include <MuonOverlayBase/IDC_MuonOverlayBase.h>
#include <MuonDigitContainer/MmDigitContainer.h>

class MM_Overlay : public IDC_MuonOverlayBase
{
public:
  MM_Overlay(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

  float timeIntegrationWindow() const { return m_timeIntegrationWindow.value(); }

private:
  SG::ReadHandleKey<MmDigitContainer> m_bkgInputKey{this, "BkgInputKey", "Bkg_MM_DIGITS", "ReadHandleKey for Background Input MmDigitContainer"};
  SG::ReadHandleKey<MmDigitContainer> m_signalInputKey{this, "SignalInputKey", "Sig_MM_DIGITS", "ReadHandleKey for Signal Input MmDigitContainer"};
  SG::WriteHandleKey<MmDigitContainer> m_outputKey{this, "OutputKey", "MM_DIGITS", "WriteHandleKey for Output MmDigitContainer"};

  FloatProperty m_timeIntegrationWindow { this, "IntegrationWindow", 100.0, "Time Integration Window" };

};

#endif/* MMOVERLAY_H */
