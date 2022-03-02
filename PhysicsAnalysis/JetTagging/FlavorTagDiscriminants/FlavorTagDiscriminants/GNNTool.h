/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GNN_TOOL_H
#define GNN_TOOL_H

// Tool includes
#include "AsgTools/AsgTool.h"
#include "FlavorTagDiscriminants/IBTagDecorator.h"
#include "FlavorTagDiscriminants/IJetTagDecorator.h"

#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/FTagDataDependencyNames.h"

// EDM includes
#include "xAODBTagging/BTaggingFwd.h"
#include "xAODJet/Jet.h"
#include "xAODBTagging/BTagging.h"

#include <memory>
#include <string>
#include <map>

#include "FlavorTagDiscriminants/DataPrepUtilities.h"
#include "FlavorTagDiscriminants/OnnxUtil.h"


namespace FlavorTagDiscriminants {
  
  struct GNNToolProperties {
    std::string nnFile;
    std::string flipTagConfig;
    std::map<std::string,std::string> variableRemapping;
    std::string trackLinkType;
  };

  //
  // Tool to to flavor tag jet/btagging object
  // using GNN based taggers
  class GNNTool : public asg::AsgTool,
                  virtual public IBTagDecorator,
                  virtual public IJetTagDecorator
  {

    ASG_TOOL_CLASS2(GNNTool, IBTagDecorator, IJetTagDecorator)
    public:
      GNNTool(const std::string& name);
      ~GNNTool();

      StatusCode initialize() override;

      virtual void decorate(const xAOD::BTagging& btag) const override;
      virtual void decorate(const xAOD::Jet& jet) const override;

      void decorate(const xAOD::Jet& jet, const SG::AuxElement& decorated) const;

      virtual std::set<std::string> getDecoratorKeys() const override;
      virtual std::set<std::string> getAuxInputKeys() const override;
      virtual std::set<std::string> getConstituentAuxInputKeys() const override;

    private:

      GNNToolProperties m_props; //!

      std::unique_ptr<OnnxUtil> m_onnxUtil;
      lwt::GraphConfig m_config;

      SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> m_jetLink;
      std::string m_input_node_name;
      std::vector<internal::VarFromBTag> m_varsFromBTag;
      std::vector<internal::VarFromJet> m_varsFromJet;
      std::vector<internal::TrackSequenceBuilder> m_trackSequenceBuilders;
      std::map<std::string, internal::OutNode> m_decorators;

      FTagDataDependencyNames m_dataDependencyNames;
  };
}
#endif
