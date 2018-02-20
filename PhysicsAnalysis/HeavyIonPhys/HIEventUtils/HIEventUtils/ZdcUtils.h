/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCUTILS
#define ZDCUTILS

#include <functional>
#include <vector>
#include <set>

#include "xAODForward/ZdcModule.h"
#include "xAODForward/ZdcModuleContainer.h"

namespace ZDC
{

  struct BySideTypeMod
  {
    int m_side;
    int m_type;
    int m_mod;
    BySideTypeMod(int side, int type=0, int mod=-1) : m_side(side),m_type(type),m_mod(mod) {} 
    bool operator() (const xAOD::ZdcModule* in_side)
      {return ( 
	       (in_side->side()==m_side||m_side==0) && 
	       in_side->type()==m_type &&
	       (in_side->zdcModule()==m_mod || m_mod==-1)
		);} 
  };

  BySideTypeMod sideA(1);
  BySideTypeMod sideC(-1);
  BySideTypeMod sideAC(0);

  BySideTypeMod sideA_HadPix(1,1,1);
  BySideTypeMod sideC_HadPix(-1,1,1);

  float getAmplitudeSum(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction, std::string suffix="");
  float getAmplitudeSumG0(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction, std::string suffix="");
  float getAmplitudeSumG1(const xAOD::ZdcModuleContainer* in, std::function<bool (const xAOD::ZdcModule*)> incFunction, std::string suffix="");
}
#endif //ZDCUTILS
