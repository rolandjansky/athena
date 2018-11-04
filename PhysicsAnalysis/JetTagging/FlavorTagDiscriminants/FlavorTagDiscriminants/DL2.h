/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_H
#define DL2_H

// local includes
#include "FlavorTagDiscriminants/customGetter.h"

// EDM includes
#include "xAODJet/Jet.h"

// external libraries
#include "lwtnn/lightweight_network_config.hh"

// STL includes
#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <exception>

// forward declarations
namespace lwt {
  class NanReplacer;
  class LightweightGraph;
}

namespace FlavorTagDiscriminants {

  enum class EDMType {INT, FLOAT, DOUBLE, CUSTOM_GETTER};

  // we define a few structures to map variable names to type, default
  // value, etc.
  typedef std::vector<std::pair<std::regex, EDMType> > TypeRegexes;
  typedef std::vector<std::pair<std::regex, std::string> > StringRegexes;

  // Structure to define a DL2 input.
  struct DL2InputConfig
  {
    std::string name;
    EDMType type;
    std::string default_flag;
  };

  // Function to map the regular expressions + the list of inputs to a
  // list of variable configurations.
  std::vector<DL2InputConfig> get_input_config(
    const std::vector<std::string>& variable_names,
    const TypeRegexes& type_regexes,
    const StringRegexes& default_flag_regexes);

  // internally we want a bunch of std::functions that return pairs to
  // populate the lwtnn input map. We define a functor here to deal
  // with the b-tagging cases.
  //
  template <typename T>
  class BVarGetter
  {
  private:
    typedef SG::AuxElement AE;
    AE::ConstAccessor<T> m_getter;
    AE::ConstAccessor<char> m_default_flag;
    std::string m_name;
  public:
    BVarGetter(std::string name, std::string default_flag):
      m_getter(name),
      m_default_flag(default_flag),
      m_name(name)
      {
      }
    std::pair<std::string, double> operator()(const xAOD::Jet& jet) const {
      const xAOD::BTagging* btag = jet.btagging();
      if (!btag) throw std::runtime_error("can't find btagging object");
      return {m_name, m_default_flag(*btag) ? NAN : m_getter(*btag)};
    }
  };

  class DL2
  {
  public:
    DL2(const lwt::GraphConfig&, const std::vector<DL2InputConfig>&);
    void decorate(const xAOD::Jet& jet) const;
  private:
    typedef xAOD::Jet Jet;
    typedef std::pair<std::string, double> Variable;
    typedef std::pair<std::string, std::vector<double> > Sequence;
    typedef std::vector<const xAOD::TrackParticle*> Tracks;
    typedef std::function<Variable(const Jet&)> Getter;
    typedef std::function<Tracks(const Jet&)> TrackGetter;
    typedef std::function<Sequence(const Jet&, const Tracks&)> SequenceGetter;
    struct TrackSequenceGetter {
      std::string name;
      TrackGetter getter;
      std::vector<SequenceGetter> sequence_getters;
    };
    typedef SG::AuxElement::Decorator<float> OutputDecorator;
    typedef std::vector<std::pair<std::string, OutputDecorator > > OutNode;
    std::string m_input_node_name;
    std::unique_ptr<lwt::LightweightGraph> m_graph;
    std::unique_ptr<lwt::NanReplacer> m_variable_cleaner;
    std::vector<Getter> m_getters;
    std::vector<TrackSequenceGetter> m_track_getters;
    std::map<std::string, OutNode> m_decorators;
  };

  //
  // Filler function
  std::function<std::pair<std::string, double>(const xAOD::Jet&)>
  get_filler(std::string name, EDMType, std::string default_flag);

}
#endif
