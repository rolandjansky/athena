/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETOVERLAY_PIXELOVERLAY_H
#define INDETOVERLAY_PIXELOVERLAY_H

#include "IDC_OverlayBase/IDC_OverlayBase.h"
#include "InDetRawData/PixelRDO_Container.h"

class PixelOverlay : public IDC_OverlayBase
{
public:

  PixelOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  SG::ReadHandleKey<PixelRDO_Container> m_bkgInputKey{ this, "BkgInputKey", "Bkg_PixelRDOs", "ReadHandleKey for Background Input PixelRDO_Container" };
  SG::ReadHandleKey<PixelRDO_Container> m_signalInputKey{ this, "SignalInputKey", "Sig_PixelRDOs", "ReadHandleKey for Signal Input PixelRDO_Container" };
  SG::WriteHandleKey<PixelRDO_Container> m_outputKey{ this, "OutputKey", "PixelRDOs", "WriteHandleKey for Output PixelRDO_Container" };
};

#endif // INDETOVERLAY_PIXELOVERLAY_H
