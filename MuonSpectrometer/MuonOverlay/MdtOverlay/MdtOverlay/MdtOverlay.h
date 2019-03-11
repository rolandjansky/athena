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

#include <MuonOverlayBase/IDC_MuonOverlayBase.h>
#include <MuonDigitContainer/MdtDigitContainer.h>

class MdtOverlay : public IDC_MuonOverlayBase
{
public:
  MdtOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

  float adcIntegrationWindow() const { return m_adcIntegrationWindow.value(); }

private:
  SG::ReadHandleKey<MdtDigitContainer> m_bkgInputKey{this, "BkgInputKey", "OriginalEvent_SG+MDT_DIGITS", "ReadHandleKey for Background Input MdtDigitContainer"};
  SG::ReadHandleKey<MdtDigitContainer> m_signalInputKey{this, "SignalInputKey", "BkgEvent_0_SG+MDT_DIGITS", "ReadHandleKey for Signal Input MdtDigitContainer"};
  SG::WriteHandleKey<MdtDigitContainer> m_outputKey{this, "OutputKey", "StoreGateSvc+MDT_DIGITS", "WriteHandleKey for Output MdtDigitContainer"};

  BooleanProperty m_includeBkg { this, "includeBkg", true, "Include Background RDO Container" };
  FloatProperty m_adcIntegrationWindow { this, "IntegrationWindow", 20.0, "ADC Integration Window" };

};

#endif/* MDTOVERLAY_H */
