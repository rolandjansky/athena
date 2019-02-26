/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_H
#define DL2_H

// local includes
#include "FlavorTagDiscriminants/customGetter.h"
#include "FlavorTagDiscriminants/EDMSchemaEnums.h"

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

  enum class EDMType {UCHAR, INT, FLOAT, DOUBLE, CUSTOM_GETTER};
  enum class SortOrder {
    ABS_D0_SIGNIFICANCE_DESCENDING, D0_SIGNIFICANCE_DESCENDING, PT_DESCENDING};
  enum class TrackSelection {ALL, IP3D_2018};

  // Structures to define DL2 input.
  //
  struct DL2InputConfig
  {
    std::string name;
    EDMType type;
    std::string default_flag;
  };
  struct DL2TrackInputConfig
  {
    std::string name;
    EDMType type;
  };
  struct DL2TrackSequenceConfig
  {
    std::string name;
    SortOrder order;
    TrackSelection selection;
    std::vector<DL2TrackInputConfig> inputs;
  };

  // ____________________________________________________________________
  // High level adapter stuff
  //
  // We define a few structures to map variable names to type, default
  // value, etc. These are only used by the high level interface.
  //
  // TODO: move this stuff into another file, i.e. either DL2HighLevel
  // or some utility file that is also included in DL2HighLevel.cxx
  typedef std::vector<std::pair<std::regex, EDMType> > TypeRegexes;
  typedef std::vector<std::pair<std::regex, std::string> > StringRegexes;
  typedef std::vector<std::pair<std::regex, SortOrder> > SortRegexes;
  typedef std::vector<std::pair<std::regex, TrackSelection> > TrkSelRegexes;

  // Function to map the regular expressions + the list of inputs to a
  // list of variable configurations.
  std::vector<DL2InputConfig> get_input_config(
    const std::vector<std::string>& variable_names,
    const TypeRegexes& type_regexes,
    const StringRegexes& default_flag_regexes);
  std::vector<DL2TrackSequenceConfig> get_track_input_config(
    const std::vector<std::pair<std::string, std::vector<std::string>>>& names,
    const TypeRegexes& type_regexes,
    const SortRegexes& sort_regexes,
    const TrkSelRegexes& select_regexes);


  // _____________________________________________________________________
  // Internal code

  namespace internal {
    // typedefs
    typedef std::pair<std::string, double> NamedVar;
    typedef std::pair<std::string, std::vector<double> > NamedSeq;
    typedef xAOD::Jet Jet;
    typedef std::vector<const xAOD::TrackParticle*> Tracks;
    typedef std::function<double(const xAOD::TrackParticle*,
                                 const xAOD::Jet&)> TrackSortVar;
    typedef std::function<bool(const xAOD::TrackParticle*)> TrackSelect;

    // getter functions
    typedef std::function<NamedVar(const Jet&)> Getter;
    typedef std::function<NamedSeq(const Jet&, const Tracks&)> SeqGetter;

    // ___________________________________________________________________
    // Getter functions
    //
    // internally we want a bunch of std::functions that return pairs
    // to populate the lwtnn input map. We define a functor here to
    // deal with the b-tagging cases.
    //
    template <typename T, bool NoDefault = false>
    class BVarGetter
    {
    private:
      typedef SG::AuxElement AE;
      AE::ConstAccessor<T> m_getter;
      AE::ConstAccessor<char> m_default_flag;
      std::string m_name;
    public:
      BVarGetter(const std::string& name, const std::string& default_flag):
        m_getter(name),
        m_default_flag(NoDefault ? "dummyDefault" : default_flag),
        m_name(name)
        {
        }
      NamedVar operator()(const xAOD::Jet& jet) const {
        const xAOD::BTagging* btag = jet.btagging();
        if (!btag) throw std::runtime_error("can't find btagging object");
        return {m_name, NoDefault || m_default_flag(*btag) ? NAN : m_getter(*btag)};
      }
    };

    // The track getter is responsible for getting the tracks from the
    // jet applying a selection, and then sorting the tracks.
    class TrackGetter
    {
    public:
      TrackGetter(SortOrder, TrackSelection, EDMSchema);
      Tracks operator()(const xAOD::Jet& jet) const;
    private:
      typedef SG::AuxElement AE;
      typedef std::vector<ElementLink<xAOD::TrackParticleContainer>> TrackLinks;
      AE::ConstAccessor<TrackLinks> m_track_associator;
      TrackSortVar m_sort_var_getter;
      TrackSelect m_select_function;
    };

    // The sequence getter takes in tracks and calculates arrays of
    // values which are better suited for inputs to the NNs
    template <typename T>
    class SequenceGetter
    {
    private:
      SG::AuxElement::ConstAccessor<T> m_getter;
      std::string m_name;
    public:
      SequenceGetter(const std::string& name):
        m_getter(name),
        m_name(name)
        {
        }
      NamedSeq operator()(const xAOD::Jet&, const Tracks& trks) const {
        std::vector<double> seq;
        for (const xAOD::TrackParticle* track: trks) {
          seq.push_back(m_getter(*track));
        }
        return {m_name, seq};
      }
    };
  }
  class DL2
  {
  public:
    DL2(const lwt::GraphConfig&,
        const std::vector<DL2InputConfig>&,
        const std::vector<DL2TrackSequenceConfig>& = {},
        EDMSchema = EDMSchema::WINTER_2018);
    void decorate(const xAOD::Jet& jet) const;
  private:
    struct TrackSequenceGetter {
      TrackSequenceGetter(SortOrder, TrackSelection, EDMSchema);
      std::string name;
      internal::TrackGetter getter;
      std::vector<internal::SeqGetter> sequence_getters;
    };
    typedef SG::AuxElement::Decorator<float> OutputDecorator;
    typedef std::vector<std::pair<std::string, OutputDecorator > > OutNode;
    std::string m_input_node_name;
    std::unique_ptr<lwt::LightweightGraph> m_graph;
    std::unique_ptr<lwt::NanReplacer> m_variable_cleaner;
    std::vector<internal::Getter> m_getters;
    std::vector<TrackSequenceGetter> m_track_getters;
    std::map<std::string, OutNode> m_decorators;
  };

  //
  // Filler functions
  namespace internal {
    Getter get_filler(std::string name, EDMType, std::string default_flag);
    TrackSortVar get_track_sort(SortOrder, EDMSchema);
    TrackSelect get_track_select(TrackSelection, EDMSchema);
    SeqGetter get_seq_getter(const DL2TrackInputConfig&);
  }
}
#endif
