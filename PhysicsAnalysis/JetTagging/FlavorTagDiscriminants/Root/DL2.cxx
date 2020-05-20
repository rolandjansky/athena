/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2.h"
#include "FlavorTagDiscriminants/BTagTrackAugmenter.h"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

namespace {
    // This is a workaround because a lot of our builds still don't
    // support C++17. It's meant to look just as ugly as it is.
  template <typename T>
  void set_merge_gcc6p2(std::set<T>& target, const std::set<T>& src) {
    target.insert(src.begin(), src.end());
  }
}


namespace FlavorTagDiscriminants {

  // DL2
  //
  // TODO: make this work with more input nodes
  DL2::DL2(const lwt::GraphConfig& graph_config,
           const std::vector<DL2InputConfig>& inputs,
           const std::vector<DL2TrackSequenceConfig>& track_sequences,
           FlipTagConfig flipConfig,
           std::map<std::string, std::string> out_remap,
           OutputType output_type):
    m_input_node_name(""),
    m_graph(new lwt::LightweightGraph(graph_config)),
    m_variable_cleaner(nullptr)
  {
    using namespace internal;
    // set up inputs
    if (graph_config.inputs.size() > 1) {
      throw std::logic_error("We don't currently support graphs with "
                             "more than one input");
    } else if (graph_config.inputs.size() == 1){
      m_input_node_name = graph_config.inputs.at(0).name;
      m_variable_cleaner.reset(new lwt::NanReplacer(
                                 graph_config.inputs.at(0).defaults,
                                 lwt::rep::all));
    }
    for (const auto& input: inputs) {
      auto filler = get::varFromBTag(input.name, input.type,
                                     input.default_flag);
      if (input.type != EDMType::CUSTOM_GETTER) {
        m_dataDependencyNames.bTagInputs.insert(input.name);
      }
      if (input.default_flag.size() > 0) {
        m_dataDependencyNames.bTagInputs.insert(input.default_flag);
      }
      m_varsFromBTag.push_back(filler);
    }

    // set up sequence inputs
    for (const DL2TrackSequenceConfig& track_cfg: track_sequences) {
      TrackSequenceBuilder track_getter(track_cfg.order,
                                        track_cfg.selection,
                                        flipConfig);
      // add the tracking data dependencies
      auto track_data_deps = get::trackFilter(track_cfg.selection).second;

      // FIXME: change this back to use std::map::merge when we
      // support C++17 everywhere
      set_merge_gcc6p2(track_data_deps, get::flipFilter(flipConfig).second);

      track_getter.name = track_cfg.name;
      for (const DL2TrackInputConfig& input_cfg: track_cfg.inputs) {
        // FIXME: change this back to
        //
        // auto [seqGetter, deps] = get::seqFromTracks(input_cfg);
        //
        // when we support C++17
        auto seqGetter_deps = get::seqFromTracks(input_cfg);
        track_getter.sequencesFromTracks.push_back(seqGetter_deps.first);
        // FIXME: change this back to use std::map::merge when we
        // support C++17 everywhere
        set_merge_gcc6p2(track_data_deps,seqGetter_deps.second);
      }
      m_trackSequenceBuilders.push_back(track_getter);
      m_dataDependencyNames.trackInputs = track_data_deps;
    }

    // set up outputs
    for (const auto& out_node: graph_config.outputs) {
      std::string node_name = out_node.first;

      OutNode node;
      for (const std::string& element: out_node.second.labels) {
        std::string name = node_name + "_" + element;

        // let user rename the output
        auto replacement_itr = out_remap.find(name);
        if (replacement_itr != out_remap.end()) {
          name = replacement_itr->second;
          out_remap.erase(replacement_itr);
        }
        m_dataDependencyNames.bTagOutputs.insert(name);

        // for the spring 2019 retraining campaign we're stuck with
        // doubles. Hopefully at some point we can move to using
        // floats.
        if (output_type == OutputType::DOUBLE) {
          SG::AuxElement::Decorator<double> d(name);
          node.emplace_back(
            element, [d](const SG::AuxElement& e, double v){ d(e) = v;});
        } else if (output_type == OutputType::FLOAT) {
          SG::AuxElement::Decorator<float> f(name);
          node.emplace_back(
            element, [f](const SG::AuxElement& e, double v){ f(e) = v;});
        } else {
          throw std::logic_error("Unknown output type");
        }
      }
      m_decorators[node_name] = node;
    }

    // we want to make sure every remapping was used
    if (out_remap.size() > 0) {
      std::string outputs;
      for (const auto& item: out_remap) {
        outputs.append(item.first);
        if (item != *out_remap.rbegin()) outputs.append(", ");
      }
      throw std::logic_error("found unused output remapping(s): " + outputs);
    }
  }

  void DL2::decorate(const xAOD::Jet& jet) const {
    using namespace internal;
    std::vector<NamedVar> vvec;
    for (const auto& getter: m_varsFromBTag) {
      vvec.push_back(getter(jet));
    }
    std::map<std::string, std::map<std::string, double> > nodes;
    if (m_variable_cleaner) {
      std::map<std::string, double> variables(vvec.begin(), vvec.end());
      auto cleaned = m_variable_cleaner->replace(variables);

      // Note, you can hack in more variables to `cleaned` here.

      // put the cleaned inputs into the node structure
      nodes[m_input_node_name] =  cleaned;
    }

    // add track sequences
    std::map<std::string,std::map<std::string, std::vector<double>>> seqs;
    for (const auto& builder: m_trackSequenceBuilders) {
      Tracks sorted_tracks = builder.tracksFromJet(jet);
      Tracks flipped_tracks = builder.flipFilter(sorted_tracks, jet);
      for (const auto& seq_builder: builder.sequencesFromTracks) {
        seqs[builder.name].insert(seq_builder(jet, flipped_tracks));
      }
    }


    // save out things
    for (const auto& dec: m_decorators) {
      // the second argument to compute(...) is for sequences
      auto out_vals = m_graph->compute(nodes, seqs, dec.first);
      for (const auto& node: dec.second) {
        node.second(*jet.btagging(), out_vals.at(node.first));
      }
    }
  }

  DL2DataDependencyNames DL2::getDataDependencyNames() const {
    return m_dataDependencyNames;
  }

  DL2::TrackSequenceBuilder::TrackSequenceBuilder(SortOrder order,
                                                  TrackSelection selection,
                                                  FlipTagConfig flipcfg):
    tracksFromJet(order, selection),
    flipFilter(internal::get::flipFilter(flipcfg).first)
  {
  }



  // ________________________________________________________________________
  // Internal code
  namespace internal {

    // Track Getter Class
    TracksFromJet::TracksFromJet(SortOrder order, TrackSelection selection):
      m_trackAssociator("BTagTrackToJetAssociator"),
      m_trackSortVar(get::trackSortVar(order)),
      m_trackFilter(get::trackFilter(selection).first)
    {
    }
    Tracks TracksFromJet::operator()(const xAOD::Jet& jet) const {
      const xAOD::BTagging *btagging = jet.btagging();
      if (!btagging) throw std::runtime_error("can't find btagging object");
      std::vector<std::pair<double, const xAOD::TrackParticle*>> tracks;
      for (const auto &link : m_trackAssociator(*btagging)) {
        if(!link.isValid()) {
          throw std::logic_error("invalid track link");
        }
        const xAOD::TrackParticle *tp = *link;
        if (m_trackFilter(tp)) {
          tracks.push_back({m_trackSortVar(tp, jet), tp});
        };
      }
      std::sort(tracks.begin(), tracks.end(), std::greater<>());
      std::vector<const xAOD::TrackParticle*> only_tracks;
      for (const auto& trk: tracks) only_tracks.push_back(trk.second);
      return only_tracks;
    }


    // ______________________________________________________________________
    // Internal utility functions
    //

    // The 'get' namespace is for factories that build std::function
    // objects
    namespace get {
      // factory for functions that get variables out of the b-tagging
      // object
      VarFromBTag varFromBTag(const std::string& name, EDMType type,
                            const std::string& default_flag) {
        if(default_flag.size() == 0 || name==default_flag)
        {
          switch (type) {
          case EDMType::INT: return BVarGetterNoDefault<int>(name);
          case EDMType::FLOAT: return BVarGetterNoDefault<float>(name);
          case EDMType::DOUBLE: return BVarGetterNoDefault<double>(name);
          case EDMType::UCHAR: return BVarGetterNoDefault<char>(name);
          case EDMType::CUSTOM_GETTER: return customGetterAndName(name);
          default: {
            throw std::logic_error("Unknown EDM type");
          }
          }
        }
        else{
          switch (type) {
          case EDMType::INT: return BVarGetter<int>(name, default_flag);
          case EDMType::FLOAT: return BVarGetter<float>(name, default_flag);
          case EDMType::DOUBLE: return BVarGetter<double>(name, default_flag);
          case EDMType::UCHAR: return BVarGetter<char>(name, default_flag);
          case EDMType::CUSTOM_GETTER: return customGetterAndName(name);
          default: {
            throw std::logic_error("Unknown EDM type");
          }
          }
        }
      }

      // factory for functions which return the sort variable we
      // use to order tracks
      TrackSortVar trackSortVar(SortOrder order) {
        typedef xAOD::TrackParticle Tp;
        typedef xAOD::Jet Jet;
        BTagTrackAugmenter aug;
        switch(order) {
        case SortOrder::ABS_D0_SIGNIFICANCE_DESCENDING:
          return [aug](const Tp* tp, const Jet&) {
                   return std::abs(aug.d0(*tp) / aug.d0Uncertainty(*tp));
                 };
        case SortOrder::D0_SIGNIFICANCE_DESCENDING:
          return [aug](const Tp* tp, const Jet& j) {
                   return aug.get_signed_ip(*tp, j).ip3d_signed_d0_significance;
                 };
        case SortOrder::PT_DESCENDING:
          return [](const Tp* tp, const Jet&) {return tp->pt();};
        default: {
          throw std::logic_error("Unknown sort function");
        }
        }
      } // end of track sort getter

      // factory for functions that return true for tracks we want to
      // use, false for those we don't want
      std::pair<TrackFilter,std::set<std::string>> trackFilter(
        TrackSelection selection) {

        typedef xAOD::TrackParticle Tp;
        typedef SG::AuxElement AE;
        BTagTrackAugmenter aug;
        auto data_deps = aug.getTrackIpDataDependencyNames();

        // make sure we record accessors as data dependencies
        auto addAccessor = [&data_deps](const std::string& n) {
                             AE::ConstAccessor<unsigned char> a(n);
                             data_deps.insert(n);
                             return a;
                           };
        auto pix_hits = addAccessor("numberOfPixelHits");
        auto pix_holes = addAccessor("numberOfPixelHoles");
        auto pix_shared = addAccessor("numberOfPixelSharedHits");
        auto pix_dead = addAccessor("numberOfPixelDeadSensors");
        auto sct_hits = addAccessor("numberOfSCTHits");
        auto sct_holes = addAccessor("numberOfSCTHoles");
        auto sct_shared = addAccessor("numberOfSCTSharedHits");
        auto sct_dead = addAccessor("numberOfSCTDeadSensors");

        switch (selection) {
        case TrackSelection::ALL: return {[](const Tp*) {return true;}, {} };
          // the following numbers come from Nicole, Dec 2018:
          // pt > 1 GeV
          // abs(d0) < 1 mm
          // abs(z0 sin(theta)) < 1.5 mm
          // >= 7 si hits
          // <= 2 si holes
          // <= 1 pix holes
        case TrackSelection::IP3D_2018:
          return {
            [=](const Tp* tp) {
              // from the track selector tool
              if (std::abs(tp->eta()) > 2.5) return false;
              double n_module_shared = (
                pix_shared(*tp) + sct_shared(*tp) / 2);
              if (n_module_shared > 1) return false;
              if (tp->pt() <= 1e3) return false;
              if (std::abs(aug.d0(*tp)) >= 1.0) return false;
              if (std::abs(aug.z0SinTheta(*tp)) >= 1.5) return false;
              if (pix_hits(*tp) + pix_dead(*tp) + sct_hits(*tp)
                  + sct_dead(*tp) < 7) return false;
              if ((pix_holes(*tp) + sct_holes(*tp)) > 2) return false;
              if (pix_holes(*tp) > 1) return false;
              return true;
            }, data_deps
          };
        default:
          throw std::logic_error("unknown track selection function");
        }
      }

      // factory for functions that build std::vector objects from
      // track sequences
      std::pair<SeqFromTracks,std::set<std::string>> seqFromTracks(
        const DL2TrackInputConfig& cfg)
      {
        switch (cfg.type) {
        case EDMType::FLOAT: return {
            SequenceGetter<float>(cfg.name), {cfg.name}
          };
        case EDMType::UCHAR: return {
            SequenceGetter<unsigned char>(cfg.name), {cfg.name}
          };
        case EDMType::CUSTOM_GETTER: return {
            customNamedSeqGetter(cfg.name),
            BTagTrackAugmenter().getTrackIpDataDependencyNames()
          };
        default: {
          throw std::logic_error("Unknown EDM type");
        }
        }
      }

      // here we define filters for the "flip" taggers
      //
      // start by defining the raw functions, there's a factory
      // function below to convert the configuration enums to a
      // std::function
      Tracks negativeIpOnly(BTagTrackAugmenter& aug,
                            const Tracks& tracks,
                            const xAOD::Jet& j) {
        Tracks filtered;
        // we want to reverse the order of the tracks as part of the
        // flipping
        for (auto ti = tracks.crbegin(); ti != tracks.crend(); ti++) {
          const xAOD::TrackParticle* tp = *ti;
          double sip = aug.get_signed_ip(*tp, j).ip3d_signed_d0_significance;
          if (sip < 0) filtered.push_back(tp);
        }
        return filtered;
      }

      // factory function
      std::pair<TrackSequenceFilter,std::set<std::string>> flipFilter(
        FlipTagConfig cfg)
      {
        namespace ph = std::placeholders;  // for _1, _2, _3
        BTagTrackAugmenter aug;
        switch(cfg) {
        case FlipTagConfig::NEGATIVE_IP_ONLY:
          // flips order and removes tracks with negative IP
          return {
            std::bind(&negativeIpOnly, aug, ph::_1, ph::_2),
            aug.getTrackIpDataDependencyNames()
          };
        case FlipTagConfig::FLIP_SIGN:
          // Just flips the order
          return {
            [](const Tracks& tr, const xAOD::Jet& ) {
              return Tracks(tr.crbegin(), tr.crend());},
            {}
          };
        case FlipTagConfig::STANDARD:
          return {[](const Tracks& tr, const xAOD::Jet& ) { return tr; }, {}};
        default: {
          throw std::logic_error("Unknown flip config");
        }
        }
      }
    } // end of get namespace
  } // end of internal namespace

}
