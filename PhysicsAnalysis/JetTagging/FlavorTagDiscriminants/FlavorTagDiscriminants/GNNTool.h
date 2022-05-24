/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GNN_TOOL_H
#define GNN_TOOL_H

// Tool includes
#include "AsgTools/AsgTool.h"
#include "FlavorTagDiscriminants/IBTagDecorator.h"
#include "FlavorTagDiscriminants/IJetTagConditionalDecorator.h"

#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/FTagDataDependencyNames.h"

// EDM includes
#include "xAODBTagging/BTaggingFwd.h"
#include "xAODJet/JetFwd.h"

#include <memory>
#include <string>
#include <map>

namespace FlavorTagDiscriminants {

  class GNN;

  struct GNNToolProperties {
    std::string nnFile;
    std::string flipTagConfig;
    std::map<std::string,std::string> variableRemapping;
    std::string trackLinkType;
    float default_output_value = NAN;
  };

  //
  // Tool to to flavor tag jet/btagging object
  // using GNN based taggers
  class GNNTool : public asg::AsgTool,
                  virtual public IBTagDecorator,
                  virtual public IJetTagConditionalDecorator
  {

    ASG_TOOL_CLASS2(GNNTool, IBTagDecorator, IJetTagConditionalDecorator)
    public:
      GNNTool(const std::string& name);
      ~GNNTool();

      StatusCode initialize() override;

      virtual void decorate(const xAOD::BTagging& btag) const override;
      virtual void decorate(const xAOD::Jet& jet) const override;
      virtual void decorateWithDefaults(const xAOD::Jet& jet) const override;
      void decorate(const xAOD::Jet& jet, const SG::AuxElement& decorated) const;

      virtual std::set<std::string> getDecoratorKeys() const override;
      virtual std::set<std::string> getAuxInputKeys() const override;
      virtual std::set<std::string> getConstituentAuxInputKeys() const override;

    private:

      GNNToolProperties m_props;
      std::unique_ptr<const GNN> m_gnn;
  };
}
#endif
