// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_TOOL_H
#define DL2_TOOL_H

#include "AsgTools/AsgTool.h"
#include "FlavorTagDiscriminants/ISingleJetDecorator.h"

namespace FlavorTagDiscriminants {

  class DL2HighLevel;

  struct DL2Properties {
    std::string nnFile;
    std::string flipTagConfig;
  };

  class DL2Tool : public asg::AsgTool, virtual public ISingleJetDecorator
  {
    ASG_TOOL_CLASS(DL2Tool, ISingleJetDecorator )
  public:
    DL2Tool(const std::string& name);
    ~DL2Tool();

    StatusCode initialize() override;

    // returns 0 for success
    virtual void decorate(const xAOD::Jet& jet) const override;
  private:
    DL2Properties m_props; //!
    std::unique_ptr<DL2HighLevel> m_dl2; //!
  };

}
#endif
