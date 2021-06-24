/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef ITrigEgammaEmulationChain_h
#define ITrigEgammaEmulationChain_h

#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationInfo.h"


namespace Trig{


  class ITrigEgammaEmulationChain: virtual public asg::IAsgTool
  { 

    ASG_TOOL_INTERFACE( ITrigEgammaEmulationChain )

    public:

      virtual StatusCode initialize()=0;

      virtual asg::AcceptData emulate( const Trig::TrigData &input ) const=0;

      virtual std::string chain() const=0;
      
      virtual std::string signature() const=0;

  };


}//namespace
#endif
