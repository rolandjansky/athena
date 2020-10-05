// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_AUGMENTER_TOOL_H
#define BTAG_AUGMENTER_TOOL_H


#include "AsgTools/AsgTool.h"
#include "FlavorTagDiscriminants/ISingleJetDecorator.h"

class BTagJetAugmenter;

namespace FlavorTagDiscriminants {

  class BTagAugmenterTool : public asg::AsgTool, virtual public ISingleJetDecorator
  {
    ASG_TOOL_CLASS(BTagAugmenterTool, ISingleJetDecorator )
  public:
    BTagAugmenterTool(const std::string& name);
    ~BTagAugmenterTool();

    StatusCode initialize() override;

    virtual void decorate(const xAOD::Jet& jet) const override;
  private:
    std::string m_flipTagConfig;
    std::unique_ptr<BTagJetAugmenter> m_aug; //!
  };

}
#endif
