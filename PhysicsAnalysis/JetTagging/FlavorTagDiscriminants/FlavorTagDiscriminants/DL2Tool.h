// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_TOOL_H
#define DL2_TOOL_H

#include "JetInterface/ISingleJetModifier.h"
#include <memory>

namespace FlavorTagDiscriminants {

  class DL2HighLevel;

  struct DL2Properties {
    std::string nnFile;
  };

  class DL2Tool : public ISingleJetModifier
  {
    ASG_TOOL_CLASS1(DL2Tool, ISingleJetModifier )
  public:
    DL2Tool(const std::string& name);
    ~DL2Tool() = default;

    StatusCode initialize();
    StatusCode finalize();

    // returns 0 for success
    int modifyJet(xAOD::Jet& jet) const;
  private:
    DL2Properties m_props;
    std::unique_ptr<DL2HighLevel> m_dl2;
  };

}
#endif
