/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/ZdcUtils.h"

namespace ZDC
{
  float getAmplitudeSumG0(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction)
  {
    float amplitude = 0;
    for (const auto zm : *in)
      {
	if (incFunction(zm)) amplitude += zm->amplitudeG0();
      }
    return amplitude;
  }

  float getAmplitudeSumG1(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction)
  {
    float amplitude = 0;
    for (const auto zm : *in)
      {
	if (incFunction(zm)) amplitude += zm->amplitudeG1();
      }
    return amplitude;
  }

  float getAmplitudeSum(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction)
  {
    float amplitude = 0;
    for (const auto zm : *in)
      {
	if (incFunction(zm)) amplitude += zm->amplitude();
      }
    return amplitude;
  }
  

}
