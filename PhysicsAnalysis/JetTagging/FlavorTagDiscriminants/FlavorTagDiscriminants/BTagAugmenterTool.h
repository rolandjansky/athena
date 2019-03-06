// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_AUGMENTER_TOOL_H
#define BTAG_AUGMENTER_TOOL_H

#include "FlavorTagDiscriminants/EDMSchemaEnums.h"

#include "AsgTools/AsgTool.h"
#include "JetInterface/ISingleJetModifier.h"

class BTagJetAugmenter;

namespace FlavorTagDiscriminants {

  class BTagAugmenterTool : public asg::AsgTool, virtual public ISingleJetModifier
  {
    ASG_TOOL_CLASS(BTagAugmenterTool, ISingleJetModifier )
  public:
    BTagAugmenterTool(const std::string& name);
    ~BTagAugmenterTool();

    StatusCode initialize();
    StatusCode finalize();

    // returns 0 for success
    int modifyJet(xAOD::Jet& jet) const;
  private:
    std::string m_schema;
    std::unique_ptr<BTagJetAugmenter> m_aug; //!
  };

}
#endif
