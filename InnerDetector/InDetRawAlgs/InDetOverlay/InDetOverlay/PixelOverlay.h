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
  virtual StatusCode execute() override final;

private:
  SG::ReadHandleKey<PixelRDO_Container> m_bkgInputKey{ this, "BkgInputKey", "OriginalEvent_SG+PixelRDOs", "ReadHandleKey for Background Input PixelRDO_Container" };
  SG::ReadHandleKey<PixelRDO_Container> m_signalInputKey{ this, "SignalInputKey", "BkgEvent_0_SG+PixelRDOs", "ReadHandleKey for Signal Input PixelRDO_Container" };
  SG::WriteHandleKey<PixelRDO_Container> m_outputKey{ this, "OutputKey", "StoreGateSvc+PixelRDOs", "WriteHandleKey for Output PixelRDO_Container" };

  BooleanProperty m_includeBkg { this, "includeBkg", true, "Include Background RDO Container" };
};

#endif // INDETOVERLAY_PIXELOVERLAY_H
