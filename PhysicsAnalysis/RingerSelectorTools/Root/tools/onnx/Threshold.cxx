/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RingerSelectorTools/tools/onnx/Threshold.h"

namespace Ringer{

  namespace onnx{

	  Threshold::Threshold( float etmin , float etmax, float etamin, float etamax, float slope, float offset, float maxavgmu ):

          m_etmin(etmin),
          m_etmax(etmax),
          m_etamin(etamin),
          m_etamax(etamax),
          m_slope(slope),
          m_offset(offset),
          m_maxavgmu(maxavgmu)
    {;}

  } // namespace onnx
} //namespace Ringer

