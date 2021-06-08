// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_AUGMENTER_TOOL_H
#define BTAG_AUGMENTER_TOOL_H


#include "AsgTools/AsgTool.h"
#include "FlavorTagDiscriminants/IBTagDecorator.h"

class BTagJetAugmenter;

namespace FlavorTagDiscriminants {

  class BTagAugmenterTool : public asg::AsgTool, virtual public IBTagDecorator
  {
    ASG_TOOL_CLASS(BTagAugmenterTool, IBTagDecorator )
  public:
    BTagAugmenterTool(const std::string& name);
    ~BTagAugmenterTool();

    StatusCode initialize() override;

    virtual void decorate(const xAOD::BTagging& btag) const override;

    virtual std::set<std::string> getDecoratorKeys() const override;
    virtual std::set<std::string> getAuxInputKeys() const override;
    virtual std::set<std::string> getConstituentAuxInputKeys() const override;
  private:
    std::string m_flipTagConfig;
    std::string m_trackAssociator;
    std::unique_ptr<BTagJetAugmenter> m_aug; //!
  };

}
#endif
