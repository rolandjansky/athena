/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef ITrigEgammaEmulationBaseHypoTool_h
#define ITrigEgammaEmulationBaseHypoTool_h

#include "AsgTools/IAsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "GaudiKernel/SystemOfUnits.h"


namespace Trig{


  class ITrigEgammaEmulationBaseHypoTool: virtual public asg::IAsgTool
  { 

    ASG_TOOL_INTERFACE( ITrigEgammaEmulationBaseHypoTool )

    public:

      virtual StatusCode initialize()=0;

      virtual bool emulate( const TrigData &input, bool &pass) const=0;

  };


}//namespace
#endif
