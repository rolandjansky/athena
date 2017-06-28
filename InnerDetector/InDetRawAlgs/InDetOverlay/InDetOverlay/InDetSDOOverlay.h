/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSDOOVERLAY_H
#define INDETSDOOVERLAY_H

#include <string>

#include "GaudiKernel/ServiceHandle.h"

#include "InDetSimData/InDetSimDataCollection.h"

#include "OverlayAlgBase/OverlayAlgBase.h"

class InDetSDOOverlay : public OverlayAlgBase
{
public:
  InDetSDOOverlay(const std::string &name, ISvcLocator *pSvcLocator);
  
  virtual StatusCode overlayInitialize() override;
  virtual StatusCode overlayExecute() override;
  virtual StatusCode overlayFinalize() override;

private:
  bool m_do_TRT, m_do_TRT_background;
  SG::ReadHandleKey<InDetSimDataCollection> m_mainInputTRTKey;
  SG::ReadHandleKey<InDetSimDataCollection> m_overlayInputTRTKey;
  SG::WriteHandleKey<InDetSimDataCollection> m_outputTRTKey;

  bool m_do_SCT, m_do_SCT_background;
  SG::ReadHandleKey<InDetSimDataCollection> m_mainInputSCTKey;
  SG::ReadHandleKey<InDetSimDataCollection> m_overlayInputSCTKey;
  SG::WriteHandleKey<InDetSimDataCollection> m_outputSCTKey;

  bool m_do_Pixel, m_do_Pixel_background;
  SG::ReadHandleKey<InDetSimDataCollection> m_mainInputPixelKey;
  SG::ReadHandleKey<InDetSimDataCollection> m_overlayInputPixelKey;
  SG::WriteHandleKey<InDetSimDataCollection> m_outputPixelKey;
};

#endif // INDETSDOOVERLAY_H
