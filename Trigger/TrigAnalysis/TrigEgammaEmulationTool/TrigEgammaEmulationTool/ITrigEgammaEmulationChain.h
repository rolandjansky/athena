/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef ITrigEgammaEmulationChain_h
#define ITrigEgammaEmulationChain_h

#include "AsgTools/IAsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationInfo.h"


namespace Trig{


  class ITrigEgammaEmulationChain: virtual public asg::IAsgTool
  { 

    ASG_TOOL_INTERFACE( ITrigEgammaEmulationChain )

    public:

      virtual StatusCode initialize()=0;

      virtual asg::AcceptData emulate( const Trig::TrigData &input ) const=0;

      virtual std::string getChain() const=0;
      
      virtual std::string getSignature() const=0;

      virtual std::string getSeed() const=0;



  };


}//namespace
#endif
