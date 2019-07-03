/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2.h"
#include "FlavorTagDiscriminants/BTagTrackAugmenter.h"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"


namespace FlavorTagDiscriminants {

  // DL2
  //
  // TODO: make this work with more input nodes
  DL2::DL2(const lwt::GraphConfig& graph_config,
           const std::vector<DL2InputConfig>& inputs,
           const std::vector<DL2TrackSequenceConfig>& track_sequences,
           FlipTagConfig flipConfig,
           EDMSchema schema):
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
      auto filler = get::varFromJet(input.name, input.type,
                                        input.default_flag);
      m_varsFromJet.push_back(filler);
    }

    // set up sequence inputs
    for (const DL2TrackSequenceConfig& track_cfg: track_sequences) {
      TrackSequenceBuilder track_getter(track_cfg.order,
                                        track_cfg.selection,
                                        schema, flipConfig);
      track_getter.name = track_cfg.name;
      for (const DL2TrackInputConfig& input_cfg: track_cfg.inputs) {
        track_getter.sequencesFromTracks.push_back(
          get::seqFromTracks(input_cfg, schema));
      }
      m_trackSequenceBuilders.push_back(track_getter);
    }

    // set up outputs
    for (const auto& out_node: graph_config.outputs) {
      std::string node_name = out_node.first;
      OutNode node;
      for (const std::string& element: out_node.second.labels) {
        std::string name = node_name + "_" + element;
        // for the spring 2019 retraining campaign we're stuck with
        // doubles. Hopefully at some point we can move to using
        // floats.
        if (schema == EDMSchema::FEB_2019) {
          SG::AuxElement::Decorator<double> d(name);
          node.emplace_back(
            element, [d](const SG::AuxElement& e, double v){ d(e) = v;});
        } else {
          SG::AuxElement::Decorator<float> d(name);
          node.emplace_back(
            element, [d](const SG::AuxElement& e, double v){ d(e) = v;});
        }
      }
      m_decorators[node_name] = node;
    }
  }
  void DL2::decorate(const xAOD::Jet& jet) const {
    using namespace internal;
    std::vector<NamedVar> vvec;
    for (const auto& getter: m_varsFromJet) {
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

  DL2::TrackSequenceBuilder::TrackSequenceBuilder(SortOrder order,
                                                  TrackSelection selection,
                                                  EDMSchema schema,
                                                  FlipTagConfig flipcfg):
    tracksFromJet(order, selection, schema),
    flipFilter(internal::get::flipFilter(flipcfg, schema))
  {
  }



  // ________________________________________________________________________
  // Internal code
  namespace internal {

    // Track Getter Class
    TracksFromJet::TracksFromJet(SortOrder order, TrackSelection selection,
                             EDMSchema schema):
      m_trackAssociator("BTagTrackToJetAssociator"),
      m_trackSortVar(get::trackSortVar(order, schema)),
      m_trackFilter(get::trackFilter(selection, schema))
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
      VarFromJet varFromJet(const std::string& name, EDMType type,
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
      TrackSortVar trackSortVar(SortOrder order, EDMSchema schema) {
        typedef xAOD::TrackParticle Tp;
        typedef xAOD::Jet Jet;
        BTagTrackAugmenter aug(schema);
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
      TrackFilter trackFilter(TrackSelection selection,
                              EDMSchema schema) {
        typedef xAOD::TrackParticle Tp;
        typedef SG::AuxElement AE;
        BTagTrackAugmenter aug(schema);
        AE::ConstAccessor<unsigned char> pix_hits("numberOfPixelHits");
        AE::ConstAccessor<unsigned char> pix_holes("numberOfPixelHoles");
        AE::ConstAccessor<unsigned char> pix_shared("numberOfPixelSharedHits");
        AE::ConstAccessor<unsigned char> pix_dead("numberOfPixelDeadSensors");
        AE::ConstAccessor<unsigned char> sct_hits("numberOfSCTHits");
        AE::ConstAccessor<unsigned char> sct_holes("numberOfSCTHoles");
        AE::ConstAccessor<unsigned char> sct_shared("numberOfSCTSharedHits");
        AE::ConstAccessor<unsigned char> sct_dead("numberOfSCTDeadSensors");
        switch (selection) {
        case TrackSelection::ALL: return [](const Tp*) {return true;};
          // the following numbers come from Nicole, Dec 2018:
          // pt > 1 GeV
          // abs(d0) < 1 mm
          // abs(z0 sin(theta)) < 1.5 mm
          // >= 7 si hits
          // <= 2 si holes
          // <= 1 pix holes
        case TrackSelection::IP3D_2018:
          return [=](const Tp* tp) {
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
                 };
        default:
          throw std::logic_error("unknown track selection function");
        }
      }

      // factory for functions that build std::vector objects from
      // track sequences
      SeqFromTracks seqFromTracks(const DL2TrackInputConfig& cfg,
                                  EDMSchema s) {
        switch (cfg.type) {
        case EDMType::FLOAT: return SequenceGetter<float>(cfg.name);
        case EDMType::UCHAR: return SequenceGetter<unsigned char>(cfg.name);
        case EDMType::CUSTOM_GETTER: return customNamedSeqGetter(cfg.name, s);
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
        for (const auto* tp: tracks) {
          double sip = aug.get_signed_ip(*tp, j).ip3d_signed_d0_significance;
          if (sip < 0) filtered.push_back(tp);
        }
        return filtered;
      }
      // factory function
      TrackSequenceFilter flipFilter(FlipTagConfig cfg, EDMSchema schema) {
        namespace ph = std::placeholders;  // for _1, _2, _3
        BTagTrackAugmenter aug(schema);
        switch(cfg) {
        case FlipTagConfig::NEGATIVE_IP_ONLY:
          return std::bind(&negativeIpOnly, aug, ph::_1, ph::_2);
        case FlipTagConfig::STANDARD:
          return [](const Tracks& tr, const xAOD::Jet& ) { return tr; };
        default: {
          throw std::logic_error("Unknown flip config");
        }
        }
      }
    } // end of get namespace
  } // end of internal namespace

}
