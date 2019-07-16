/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_H
#define DL2_H

// local includes
#include "FlavorTagDiscriminants/customGetter.h"
#include "FlavorTagDiscriminants/EDMSchemaEnums.h"
#include "FlavorTagDiscriminants/FlipTagEnums.h"

// EDM includes
#include "xAODJet/Jet.h"

// external libraries
#include "lwtnn/lightweight_network_config.hh"

// STL includes
#include <string>
#include <vector>
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
    typedef std::function<NamedVar(const Jet&)> VarFromJet;
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
        return {m_name, m_default_flag(*btag) ? NAN : m_getter(*btag)};
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
        return {m_name, m_getter(*btag)};
      }
    };

    // The track getter is responsible for getting the tracks from the
    // jet applying a selection, and then sorting the tracks.
    class TracksFromJet
    {
    public:
      TracksFromJet(SortOrder, TrackSelection, EDMSchema);
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
        EDMSchema = EDMSchema::WINTER_2018);
    void decorate(const xAOD::Jet& jet) const;
  private:
    struct TrackSequenceBuilder {
      TrackSequenceBuilder(SortOrder, TrackSelection, EDMSchema,
                           FlipTagConfig);
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
    std::vector<internal::VarFromJet> m_varsFromJet;
    std::vector<TrackSequenceBuilder> m_trackSequenceBuilders;
    std::map<std::string, OutNode> m_decorators;
  };

  //
  // Filler functions
  namespace internal {
    // factory functions to produce callable objects that build inputs
    namespace get {
      VarFromJet varFromJet(const std::string& name,
                            EDMType,
                            const std::string& defaultflag);
      TrackSortVar trackSortVar(SortOrder, EDMSchema);
      TrackFilter trackFilter(TrackSelection, EDMSchema);
      SeqFromTracks seqFromTracks(const DL2TrackInputConfig&, EDMSchema);
      TrackSequenceFilter flipFilter(FlipTagConfig, EDMSchema);
    }
  }
}
#endif
