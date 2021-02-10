/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file PixelDigitization/PixelHistoConverter.h
 * @author Tomas Dado <tomas.dado@cern.ch>
 * @date February, 2020
 * @brief A helper class to conver ROOT TH* objects into simple vectors for speed up
 */

#ifndef PIXELCONDITIONSDATA_PIXELHISTOCONVRTER_H
#define PIXELCONDITIONSDATA_PIXELHISTOCONVRTER_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/StatusCode.h"

#include <vector>

/// forward class declaration
class TAxis;
class TH1;
class TH2;
class TH3;

class PixelHistoConverter{
public:
  PixelHistoConverter();
  virtual ~PixelHistoConverter() = default;

  StatusCode SetHisto1D(const TH1* histo);
  StatusCode SetHisto2D(const TH2* histo);
  StatusCode SetHisto3D(const TH3* histo);

  float GetContent(const std::size_t& x) const;
  float GetContent(const std::size_t& x, const std::size_t& y) const;
  float GetContent(const std::size_t& x, const std::size_t& y, const std::size_t& z) const;

  bool IsOverflowZ(const float value) const;
  bool IsFirstZ(const float value) const;
  float GetBinX(const float value) const;
  float GetBinY(const float value) const;
  float GetBinZ(const float value) const;

private:
  struct Axis {
    std::size_t nBins;
    float min;
    float max;
    float width;
  };

  Axis m_xAxis;
  Axis m_yAxis;
  Axis m_zAxis;

  std::vector<float> m_content;

  bool SetAxis(Axis& axis, const TAxis* rootAxis);

  std::size_t FindBin(const Axis& axis, const float value) const;

};

#endif
