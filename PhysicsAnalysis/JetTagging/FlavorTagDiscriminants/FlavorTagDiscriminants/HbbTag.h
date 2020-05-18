/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HBB_TAG_H
#define HBB_TAG_H

namespace lwt {
  class LightweightGraph;
}

#include "FlavorTagDiscriminants/HbbTagConfig.h"

#include "xAODJet/JetContainer.h"

#include <functional>
#include <memory>
#include <map>

namespace FlavorTagDiscriminants {

  class HbbTag
  {
  public:
    HbbTag(const HbbTagConfig& config);
    ~HbbTag();
    void decorate(const xAOD::Jet& jet) const;

  private:
    // reading stuff
    using JetLink = ElementLink<xAOD::JetContainer>;
    SG::AuxElement::ConstAccessor<JetLink> m_parent_link;
    using PartLink = std::vector<ElementLink<xAOD::IParticleContainer>>;
    SG::AuxElement::ConstAccessor<PartLink> m_subjet_link_getter;
    using Pg = std::function<std::pair<std::string, double>(const xAOD::Jet&)>;
    std::vector<Pg> m_fat_jet_getters;
    std::vector<Pg> m_subjet_getters;
    size_t m_n_subjets;
    double m_min_subjet_pt;

    // calculating stuff
    std::unique_ptr<lwt::LightweightGraph> m_graph;
    std::map<std::string, std::map<std::string,double>> m_defaults;

    // writeing stuff
    template <typename T>
    using Decorator = SG::AuxElement::Decorator<T>;
    using NodeWriter = std::vector<std::pair<std::string, Decorator<float>>>;
    std::vector<std::pair<std::string, NodeWriter>> m_outputs;
  };

}

#endif
