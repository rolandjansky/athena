/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_H
#define DL2_H

// local includes
#include "FlavorTagDiscriminants/customGetter.h"
#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/DL2DataDependencyNames.h"
#include "FlavorTagDiscriminants/ftagfloat_t.h"

// EDM includes
#include "xAODJet/Jet.h"

// external libraries
#include "lwtnn/lightweight_network_config.hh"

// STL includes
#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <type_traits>

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
  enum class OutputType {FLOAT, DOUBLE};

  // classes to deal with typedefs
  //
  template <typename T>
  struct EDMTypeEnum;
  template <> struct EDMTypeEnum<float> {
    const static EDMType type = EDMType::FLOAT;
  };
  template <> struct EDMTypeEnum<double> {
    const static EDMType type = EDMType::DOUBLE;
  };
  template<typename T>
  struct OutputTypeEnum;
  template<> struct OutputTypeEnum<float> {
    const static OutputType type = OutputType::FLOAT;
  };
  template<> struct OutputTypeEnum<double> {
    const static OutputType type = OutputType::DOUBLE;
  };


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
    typedef std::function<bool(const xAOD::TrackParticle*)> TrackFilter;
    typedef std::function<Tracks(const Tracks&,
                                 const xAOD::Jet&)> TrackSequenceFilter;

    // getter functions
    typedef std::function<NamedVar(const Jet&)> VarFromBTag;
    typedef std::function<NamedSeq(const Jet&, const Tracks&)> SeqFromTracks;

    // ___________________________________________________________________
    // Getter functions
    //
    // internally we want a bunch of std::functions that return pairs
    // to populate the lwtnn input map. We define a functor here to
    // deal with the b-tagging cases.
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
      BVarGetter(const std::string& name, const std::string& default_flag):
        m_getter(name),
        m_default_flag(default_flag),
        m_name(name)
        {
        }
      NamedVar operator()(const xAOD::Jet& jet) const {
        const xAOD::BTagging* btag = jet.btagging();
        if (!btag) throw std::runtime_error("can't find btagging object");
        T ret_value = m_getter(*btag);
        bool is_default = m_default_flag(*btag);
        if constexpr (std::is_floating_point<T>::value) {
          if (std::isnan(ret_value) && !is_default) {
            throw std::runtime_error(
              "Found NAN value for '" + m_name
              + "'. This is only allowed when using a default"
              " value for this input");
          }
        }
        return {m_name, is_default ? NAN : ret_value};
      }
    };

    template <typename T>
    class BVarGetterNoDefault
    {
    private:
      typedef SG::AuxElement AE;
      AE::ConstAccessor<T> m_getter;
      std::string m_name;
    public:
      BVarGetterNoDefault(const std::string& name):
        m_getter(name),
        m_name(name)
        {
        }
      NamedVar operator()(const xAOD::Jet& jet) const {
        const xAOD::BTagging* btag = jet.btagging();
        if (!btag) throw std::runtime_error("can't find btagging object");
        T ret_value = m_getter(*btag);
        if constexpr (std::is_floating_point<T>::value) {
          if (std::isnan(ret_value)) {
            throw std::runtime_error(
              "Found NAN value for '" + m_name + "'.");
          }
        }
        return {m_name, ret_value};
      }
    };

    // The track getter is responsible for getting the tracks from the
    // jet applying a selection, and then sorting the tracks.
    class TracksFromJet
    {
    public:
      TracksFromJet(SortOrder, TrackSelection);
      Tracks operator()(const xAOD::Jet& jet) const;
    private:
      typedef SG::AuxElement AE;
      typedef std::vector<ElementLink<xAOD::TrackParticleContainer>> TrackLinks;
      AE::ConstAccessor<TrackLinks> m_trackAssociator;
      TrackSortVar m_trackSortVar;
      TrackFilter m_trackFilter;
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
  } // end internal namespace
  class DL2
  {
  public:
    DL2(const lwt::GraphConfig&,
        const std::vector<DL2InputConfig>&,
        const std::vector<DL2TrackSequenceConfig>& = {},
        FlipTagConfig = FlipTagConfig::STANDARD,
        std::map<std::string, std::string> out_remap = {},
        OutputType = OutputType::DOUBLE);
    void decorate(const xAOD::Jet& jet) const;

    // functions to report data depdedencies
    DL2DataDependencyNames getDataDependencyNames() const;

  private:
    struct TrackSequenceBuilder {
      TrackSequenceBuilder(SortOrder, TrackSelection, FlipTagConfig);
      std::string name;
      internal::TracksFromJet tracksFromJet;
      internal::TrackSequenceFilter flipFilter;
      std::vector<internal::SeqFromTracks> sequencesFromTracks;
    };
    typedef std::function<void(const SG::AuxElement&, double)> OutputSetter;
    typedef std::vector<std::pair<std::string, OutputSetter > > OutNode;
    std::string m_input_node_name;
    std::unique_ptr<lwt::LightweightGraph> m_graph;
    std::unique_ptr<lwt::NanReplacer> m_variable_cleaner;
    std::vector<internal::VarFromBTag> m_varsFromBTag;
    std::vector<TrackSequenceBuilder> m_trackSequenceBuilders;
    std::map<std::string, OutNode> m_decorators;

    DL2DataDependencyNames m_dataDependencyNames;

  };


  //
  // Filler functions
  namespace internal {
    // factory functions to produce callable objects that build inputs
    namespace get {
      VarFromBTag varFromBTag(const std::string& name,
                            EDMType,
                            const std::string& defaultflag);
      TrackSortVar trackSortVar(SortOrder);
      std::pair<TrackFilter,std::set<std::string>> trackFilter(
        TrackSelection);
      std::pair<SeqFromTracks,std::set<std::string>> seqFromTracks(
        const DL2TrackInputConfig&);
      std::pair<TrackSequenceFilter,std::set<std::string>> flipFilter(
        FlipTagConfig);
    }
  }
}
#endif
