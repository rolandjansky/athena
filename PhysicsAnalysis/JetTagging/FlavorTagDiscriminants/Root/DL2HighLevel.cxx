/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2HighLevel.h"
#include "FlavorTagDiscriminants/DL2HighLevelTools.h"
#include "FlavorTagDiscriminants/DL2.h"

#include "PathResolver/PathResolver.h"

#include "lwtnn/parse_json.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include <fstream>

namespace {
  // define a regex literal operator
  std::regex operator "" _r(const char* c, size_t /* length */) {
    return std::regex(c);
  }

}

namespace FlavorTagDiscriminants {

  DL2HighLevel::DL2HighLevel(const std::string& nn_file_name,
                             FlipTagConfig flip_config,
                             std::map<std::string,std::string> var_map):
    m_dl2(nullptr)
  {
    // get the graph
    std::string nn_path = PathResolverFindCalibFile(nn_file_name);
    if (nn_path.size() == 0) {
      throw std::runtime_error("no file found at '" + nn_file_name + "'");
    }
    std::ifstream input_stream(nn_path);
    lwt::GraphConfig config = lwt::parse_json_graph(input_stream);

    // __________________________________________________________________
    // we rewrite the inputs if we're using flip taggers
    //

    StringRegexes flip_converters {
      {"(IP[23]D)_(.*)"_r, "$1Neg_$2"},
      {"rnnip_(.*)"_r, "rnnipflip_$1"},
      {"(JetFitter|SV1|JetFitterSecondaryVertex)_(.*)"_r, "$1Flip_$2"},
      {"rnnip"_r, "rnnipflip"},
      {"^(DL1|DL1r|DL1rmu)$"_r, "$1Flip"},
      {"pt|abs_eta"_r, "$&"},
      {"(minimum|maximum|average)TrackRelativeEta"_r, "$&Flip"},
      {"softMuon.*|smt.*"_r, "$&"}
    };

    // some sequences also need to be sign-flipped. We apply this by
    // changing the input scaling and normalizations
    std::regex flip_sequences(".*signed_[dz]0.*");

    if (flip_config != FlipTagConfig::STANDARD) {
      rewriteFlipConfig(config, flip_converters);
      flipSequenceSigns(config, flip_sequences);
    }

    // __________________________________________________________________
    // build the standard inputs
    //

    // the ftagfloat_t is defined in this library (in ftagfloat_t.h)
    EDMType floattype = EDMTypeEnum<ftagfloat_t>::type;

    // type and default value-finding regexes are hardcoded for now
    TypeRegexes type_regexes = {
      {".*_isDefaults"_r, EDMType::UCHAR},
      // TODO: in the future we should migrate RNN and IPxD
      // variables to floats. This is outside the scope of the
      // current flavor tagging developments and AFT-438.
      {"IP[23]D(Neg)?_[pbc](b|c|u|tau)"_r, floattype},
      {"SV1(Flip)?_[pbc](b|c|u|tau)"_r, floattype},
      {"(rnnip|iprnn)(flip)?_p(b|c|u|tau)"_r, floattype},
      {"(minimum|maximum|average)TrackRelativeEta(Flip)?"_r, EDMType::FLOAT},
      {"(JetFitter|SV1|JetFitterSecondaryVertex)(Flip)?_[Nn].*"_r, EDMType::INT},
      {"(JetFitter|SV1|JetFitterSecondaryVertex).*"_r, EDMType::FLOAT},
      {"pt|abs_eta|eta"_r, EDMType::CUSTOM_GETTER},
      {"softMuon_p[bcu]"_r, floattype},
      {"softMuon_.*"_r, EDMType::FLOAT},
    };

    StringRegexes default_flag_regexes{
      {"IP2D_.*"_r, "IP2D_isDefaults"},
      {"IP2DNeg_.*"_r, "IP2DNeg_isDefaults"},
      {"IP3D_.*"_r, "IP3D_isDefaults"},
      {"IP3DNeg_.*"_r, "IP3DNeg_isDefaults"},
      {"SV1_.*"_r, "SV1_isDefaults"},
      {"SV1Flip_.*"_r, "SV1Flip_isDefaults"},
      {"JetFitter_.*"_r, "JetFitter_isDefaults"},
      {"JetFitterFlip_.*"_r, "JetFitterFlip_isDefaults"},
      {"JetFitterSecondaryVertex_.*"_r, "JetFitterSecondaryVertex_isDefaults"},
      {"JetFitterSecondaryVertexFlip_.*"_r, "JetFitterSecondaryVertexFlip_isDefaults"},
      {".*TrackRelativeEta"_r, "JetFitterSecondaryVertex_isDefaults"},
      {".*TrackRelativeEtaFlip"_r, "JetFitterSecondaryVertexFlip_isDefaults"},
      {"rnnip_.*"_r, "rnnip_isDefaults"},
      {"rnnipflip_.*"_r, "rnnipflip_isDefaults"},
      {"iprnn_.*"_r, ""},
      {"smt_.*"_r, "softMuon_isDefaults"},
      {"softMuon_.*"_r, "softMuon_isDefaults"},
      {"(pt|abs_eta|eta)"_r, ""}}; // no default required for custom cases

    std::vector<DL2InputConfig> input_config;
    if (config.inputs.size() == 1) {
      std::vector<std::string> input_names;
      for (const auto& var: config.inputs.at(0).variables) {
        input_names.push_back(var.name);
      }

      input_config = get_input_config(
        input_names, type_regexes, default_flag_regexes);
      // allow the user to remape some of the inputs
      remap_inputs(config.inputs.at(0).variables, input_config, var_map);
    } else if (config.inputs.size() > 1) {
      throw std::logic_error("DL2 doesn't support multiple inputs");
    }

    // ___________________________________________________________________
    // build the track inputs
    //
    std::vector<std::pair<std::string, std::vector<std::string> > > trk_names;
    for (const auto& node: config.input_sequences) {
      std::vector<std::string> names;
      for (const auto& var: node.variables) {
        names.push_back(var.name);
      }
      trk_names.emplace_back(node.name, names);
    }

    TypeRegexes trk_type_regexes {
      {"numberOf.*"_r, EDMType::UCHAR},
      {".*_(d|z)0.*"_r, EDMType::CUSTOM_GETTER},
      {"(log_)?(ptfrac|dr).*"_r, EDMType::CUSTOM_GETTER}
    };
    // We have a number of special naming conventions to sort and
    // filter tracks. The track nodes should be named according to
    //
    // tracks_<selection>_<sort-order>
    //
    SortRegexes trk_sort_regexes {
      {".*absSd0sort"_r, SortOrder::ABS_D0_SIGNIFICANCE_DESCENDING},
      {".*sd0sort"_r, SortOrder::D0_SIGNIFICANCE_DESCENDING},
      {".*ptsort"_r, SortOrder::PT_DESCENDING},
    };
    TrkSelRegexes trk_select_regexes {
      {".*_ip3d_.*"_r, TrackSelection::IP3D_2018},
      {".*_all_.*"_r, TrackSelection::ALL},
    };
    std::vector<DL2TrackSequenceConfig> trk_config = get_track_input_config(
      trk_names, trk_type_regexes, trk_sort_regexes, trk_select_regexes);

    OutputType outtype = OutputTypeEnum<ftagfloat_t>::type;
    m_dl2.reset(
      new DL2(
        config,                 // lwtnn config
        input_config,           // EDM input config
        trk_config,             // edm track input config
        flip_config,            // flip tagger configuration
        var_map,                // variable remapping
        outtype                 // output type
        ));
  }

  DL2HighLevel::~DL2HighLevel() = default;
  DL2HighLevel::DL2HighLevel(DL2HighLevel&&) = default;

  void DL2HighLevel::decorate(const xAOD::Jet& jet) const {
    m_dl2->decorate(jet);
  }

  DL2DataDependencyNames DL2HighLevel::getDataDependencyNames() const
  {
    return m_dl2->getDataDependencyNames();
  }

}
