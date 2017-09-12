/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSDOOVERLAY_H
#define INDETSDOOVERLAY_H

#include <string>

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
  std::string m_mainInputTRTKey;
  std::string m_overlayInputTRTKey;
  std::string m_outputTRTKey;

  bool m_do_SCT, m_do_SCT_background;
  std::string m_mainInputSCTKey;
  std::string m_overlayInputSCTKey;
  std::string m_outputSCTKey;

  bool m_do_Pixel, m_do_Pixel_background;
  std::string m_mainInputPixelKey;
  std::string m_overlayInputPixelKey;
  std::string m_outputPixelKey;
};

#endif // INDETSDOOVERLAY_H
