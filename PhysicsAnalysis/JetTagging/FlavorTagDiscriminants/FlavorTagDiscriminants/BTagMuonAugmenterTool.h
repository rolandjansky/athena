// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_MUON_AUGMENTER_TOOL_H
#define BTAG_MUON_AUGMENTER_TOOL_H

#include "AsgTools/AsgTool.h"
#include "FlavorTagDiscriminants/IBTagDecorator.h"

namespace FlavorTagDiscriminants {

  class BTagMuonAugmenter;

  class BTagMuonAugmenterTool : public asg::AsgTool, virtual public IBTagDecorator
  {
    ASG_TOOL_CLASS(BTagMuonAugmenterTool, IBTagDecorator )
  public:
    BTagMuonAugmenterTool(const std::string& name);
    ~BTagMuonAugmenterTool();

    StatusCode initialize() override;

    // returns 0 for success
    virtual void decorate(const xAOD::BTagging& btag) const override;

    virtual std::set<std::string> getDecoratorKeys() const override;
    virtual std::set<std::string> getAuxInputKeys() const override;
    virtual std::set<std::string> getConstituentAuxInputKeys() const override;
  private:
    std::unique_ptr<BTagMuonAugmenter> m_aug;
    std::string m_muonAssociationName;
    float m_muonMinDR;
    float m_muonMinpT;
    std::string m_flipTagConfig;
    // You'll probably have to add some accessors here
  };

}
#endif
