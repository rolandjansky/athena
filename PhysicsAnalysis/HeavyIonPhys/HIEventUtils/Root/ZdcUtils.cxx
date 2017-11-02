/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/ZdcUtils.h"

namespace ZDC
{
  float getAmplitudeSumG0(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction, std::string suffix)
  {
    float amplitude = 0;
    float mod_amp;
    for (const auto zm : *in)
      {
	if (!incFunction(zm)) continue;

	if (suffix=="")
	  mod_amp = zm->amplitudeG0();
	else
	  mod_amp = zm->auxdecor<float>("amplitudeG0"+suffix);

	amplitude += mod_amp;
      }
    return amplitude;
  }

  float getAmplitudeSumG1(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction, std::string/* suffix*/)
  {
    float amplitude = 0;
    for (const auto zm : *in)
      {
	if (incFunction(zm)) amplitude += zm->amplitudeG1();
      }
    return amplitude;
  }

  float getAmplitudeSum(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction, std::string suffix)
  {
    float amplitude = 0;
    for (const auto zm : *in)
      {
	if (incFunction(zm)) amplitude += zm->amplitude();
      }
    return amplitude;
  }
  

}
