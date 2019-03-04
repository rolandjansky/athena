/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagJetAugmenter.h"

namespace {

#define RETURN_SCHEMA(schema) \
  if (name == std::string(#schema)) \
    return FlavorTagDiscriminants::EDMSchema::schema

  FlavorTagDiscriminants::EDMSchema get_schema(const std::string& name) {
    RETURN_SCHEMA(WINTER_2018);
    RETURN_SCHEMA(FEB_2019);
    throw std::logic_error("b-tagging name schema " + name + " unknown");
  }

#undef RETURN_SCHEMA

}

namespace FlavorTagDiscriminants {

  BTagAugmenterTool::BTagAugmenterTool(const std::string& name):
    asg::AsgTool(name),
    m_schema(""),
    m_aug(nullptr)
  {
    declareProperty("schema", m_schema);
  }
  BTagAugmenterTool::~BTagAugmenterTool() {}

  StatusCode BTagAugmenterTool::initialize() {
    m_aug.reset(new BTagJetAugmenter(get_schema(m_schema)));
    return StatusCode::SUCCESS;
  }
  StatusCode BTagAugmenterTool::finalize() {
    return StatusCode::SUCCESS;
  }

  int BTagAugmenterTool::modifyJet(xAOD::Jet& jet) const {
    m_aug->augment(jet);
    return 0; // 0 means success
  }

}
