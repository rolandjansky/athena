// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HBB_TAG_TOOL_H
#define HBB_TAG_TOOL_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetDecorator.h"

namespace FlavorTagDiscriminants {

  class HbbTag;

  struct HbbTagProperties {
    std::string nnFile;
  };

  class HbbTagTool : public asg::AsgTool, virtual public IJetDecorator
  {
    ASG_TOOL_CLASS(HbbTagTool, IJetDecorator )
  public:
    HbbTagTool(const std::string& name);
    ~HbbTagTool();

    StatusCode initialize();
    StatusCode finalize();

    // returns 0 for success
    int decorate(const xAOD::JetContainer& jets) const;
  private:
    HbbTagProperties m_props; //!
    std::unique_ptr<HbbTag> m_hbb; //!
  };

}
#endif
