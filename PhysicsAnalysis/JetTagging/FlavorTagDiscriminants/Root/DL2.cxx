/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2.h"
#include "FlavorTagDiscriminants/BTagTrackAugmenter.h"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

// functions that are used internally
namespace {
  template <typename T>
  T match_first(const std::vector<std::pair<std::regex, T> >& regexes,
                const std::string var_name) {
    for (const auto& pair: regexes) {
      if (std::regex_match(var_name, pair.first)) {
        return pair.second;
      }
    }
    throw std::logic_error(
      "no regex match found for input variable " + var_name);
  }
}

namespace FlavorTagDiscriminants {

  // DL2
  //
  // TODO: make this work with more input nodes
  DL2::DL2(const lwt::GraphConfig& graph_config,
           const std::vector<DL2InputConfig>& inputs,
           const std::vector<DL2TrackSequenceConfig>& track_sequences,
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
                                 graph_config.inputs.at(0).defaults));
    }
    for (const auto& input: inputs) {
      auto filler = get_filler(input.name, input.type, input.default_flag);
      m_getters.push_back(filler);
    }

    // set up sequence inputs
    for (const DL2TrackSequenceConfig& track_cfg: track_sequences) {
      TrackSequenceGetter track_getter(track_cfg.order,
                                       track_cfg.selection,
                                       schema);
      track_getter.name = track_cfg.name;
      for (const DL2TrackInputConfig& input_cfg: track_cfg.inputs) {
        track_getter.sequence_getters.push_back(get_seq_getter(input_cfg));
      }
      m_track_getters.push_back(track_getter);
    }

    // set up outputs
    for (const auto& out_node: graph_config.outputs) {
      std::string node_name = out_node.first;
      OutNode node;
      for (const std::string& element: out_node.second.labels) {
        std::string name = node_name + "_" + element;
        node.emplace_back(element, OutputDecorator(name));
      }
      m_decorators[node_name] = node;
    }
  }
  void DL2::decorate(const xAOD::Jet& jet) const {
    using namespace internal;
    std::vector<NamedVar> vvec;
    for (const auto& getter: m_getters) {
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
    for (const auto& getter: m_track_getters) {
      Tracks sorted_tracks = getter.getter(jet);
      for (const auto& seq_getter: getter.sequence_getters) {
        seqs[getter.name].insert(seq_getter(jet, sorted_tracks));
      }
    }

    // save out things
    for (const auto& dec: m_decorators) {
      // the second argument to compute(...) is for sequences
      auto out_vals = m_graph->compute(nodes, seqs, dec.first);
      for (const auto& node: dec.second) {
        node.second(*jet.btagging()) = out_vals.at(node.first);
      }
    }
  }

  DL2::TrackSequenceGetter::TrackSequenceGetter(SortOrder order,
                                                TrackSelection selection,
                                                EDMSchema schema):
    getter(order, selection, schema)
  {
  }


  // ______________________________________________________________________
  // High level configuration functions
  //
  std::vector<DL2InputConfig> get_input_config(
    const std::vector<std::string>& variable_names,
    const TypeRegexes& type_regexes,
    const StringRegexes& default_flag_regexes)
  {
    std::vector<DL2InputConfig> inputs;
    for (const auto& var: variable_names) {
      DL2InputConfig input;
      input.name = var;
      input.type = match_first(type_regexes, var);
      input.default_flag = match_first(default_flag_regexes, var);
      inputs.push_back(input);
    }
    return inputs;
  }
  std::vector<DL2TrackSequenceConfig> get_track_input_config(
    const std::vector<std::pair<std::string, std::vector<std::string>>>& names,
    const TypeRegexes& type_regexes,
    const SortRegexes& sort_regexes,
    const TrkSelRegexes& select_regexes) {
    std::vector<DL2TrackSequenceConfig> nodes;
    for (const auto& name_node: names) {
      DL2TrackSequenceConfig node;
      node.name = name_node.first;
      node.order = match_first(sort_regexes, name_node.first);
      node.selection = match_first(select_regexes, name_node.first);
      for (const auto& varname: name_node.second) {
        DL2TrackInputConfig input;
        input.name = varname;
        input.type = match_first(type_regexes, varname);
        node.inputs.push_back(input);
      }
      nodes.push_back(node);
    }
    return nodes;
  }


  // ________________________________________________________________________
  // Internal code
  namespace internal {

    // Track Getter Class
    TrackGetter::TrackGetter(SortOrder order, TrackSelection selection,
                             EDMSchema schema):
      m_track_associator("BTagTrackToJetAssociator"),
      m_sort_var_getter(get_track_sort(order, schema)),
      m_select_function(get_track_select(selection, schema))
    {
    }
    Tracks TrackGetter::operator()(const xAOD::Jet& jet) const {
      const xAOD::BTagging *btagging = jet.btagging();
      if (!btagging) throw std::runtime_error("can't find btagging object");
      std::vector<std::pair<double, const xAOD::TrackParticle*>> tracks;
      for (const auto &link : m_track_associator(*btagging)) {
        if(!link.isValid()) {
          throw std::logic_error("invalid track link");
        }
        const xAOD::TrackParticle *tp = *link;
        if (m_select_function(tp)) {
          tracks.push_back({m_sort_var_getter(tp, jet), tp});
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
    Getter get_filler(std::string name, EDMType type,
                      std::string default_flag) {
      switch (type) {
      case EDMType::INT: return BVarGetter<int>(name, default_flag);
      case EDMType::FLOAT: return BVarGetter<float>(name, default_flag);
      case EDMType::DOUBLE: return BVarGetter<double>(name, default_flag);
      case EDMType::CUSTOM_GETTER: return customGetterAndName(name);
      default: {
        throw std::logic_error("Unknown EDM type");
      }
      }
    }
    TrackSortVar get_track_sort(SortOrder order, EDMSchema schema) {
      typedef xAOD::TrackParticle Tp;
      typedef xAOD::Jet Jet;
      typedef SG::AuxElement AE;
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
    TrackSelect get_track_select(TrackSelection selection,
                                 EDMSchema schema) {
      typedef xAOD::TrackParticle Tp;
      typedef SG::AuxElement AE;
      BTagTrackAugmenter aug(schema);
      AE::ConstAccessor<unsigned char> pix_hits("numberOfPixelHits");
      AE::ConstAccessor<unsigned char> pix_holes("numberOfPixelHoles");
      AE::ConstAccessor<unsigned char> sct_hits("numberOfSCTHits");
      AE::ConstAccessor<unsigned char> sct_holes("numberOfSCTHoles");
      switch (selection) {
      case TrackSelection::ALL: return [](const Tp*) {return true;};
        // the following numbers come from Nikol, Dec 2018:
        // pt > 1 GeV
        // d0 < 1 mm
        // z0 sin(theta) < 1.5 mm
        // >= 7 si hits
        // <= 2 si holes
        // <= 1 pix holes
      case TrackSelection::IP3D_2018:
        return [=](const Tp* tp) {
                 if (tp->pt() <= 1e3) return false;
                 if (aug.d0(*tp) >= 0.1) return false;
                 if (aug.z0SinTheta(*tp) >= 0.15) return false;
                 if (pix_hits(*tp) + sct_hits(*tp) < 7) return false;
                 if (pix_holes(*tp) + sct_holes(*tp) > 2) return false;
                 if (pix_holes(*tp) > 1) return false;
                 return true;
               };
      default:
        throw std::logic_error("unknown track selection function");
      }
    }

    SeqGetter get_seq_getter(const DL2TrackInputConfig& cfg) {
      switch (cfg.type) {
      case EDMType::FLOAT: return SequenceGetter<float>(cfg.name);
      case EDMType::UCHAR: return SequenceGetter<unsigned char>(cfg.name);
      case EDMType::CUSTOM_GETTER: return customNamedSeqGetter(cfg.name);
      default: {
        throw std::logic_error("Unknown EDM type");
      }
      }
    }

  } // end of internal namespace

}
