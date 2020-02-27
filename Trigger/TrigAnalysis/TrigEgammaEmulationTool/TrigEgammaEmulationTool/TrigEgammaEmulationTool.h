/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationTool_H
#define TrigEgammaEmulationTool_H


#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationTool.h"


namespace Trig{

  class TrigEgammaEmulationTool: public asg::AsgTool,
                                 virtual public Trig::ITrigEgammaEmulationTool 
    
  {
    ASG_TOOL_CLASS(TrigEgammaEmulationTool, Trig::ITrigEgammaEmulationTool)
  
    public:
  
      TrigEgammaEmulationTool(const std::string& myname);
      ~TrigEgammaEmulationTool() {};
  
      StatusCode initialize() override;
  
  
  
  
  };
}//namespace

#endif
