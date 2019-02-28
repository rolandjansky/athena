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

#include <MuonDigitContainer/RpcDigitContainer.h>
#include <MuonOverlayBase/IDC_MuonOverlayBase.h>


class RpcOverlay : public IDC_MuonOverlayBase
{
public:

  RpcOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  SG::ReadHandleKey<RpcDigitContainer> m_bkgInputKey{ this, "BkgInputKey", "OriginalEvent_SG+RPC_DIGITS", "ReadHandleKey for Background Input RpcDigitContainer" };
  SG::ReadHandleKey<RpcDigitContainer> m_signalInputKey{ this, "SignalInputKey", "BkgEvent_0_SG+RPC_DIGITS", "ReadHandleKey for Signal Input RpcDigitContainer" };
  SG::WriteHandleKey<RpcDigitContainer> m_outputKey{ this, "OutputKey","StoreGateSvc+RPC_DIGITS", "WriteHandleKey for Output RpcDigitContainer" };

};

#endif/* RPCOVERLAY_H */
