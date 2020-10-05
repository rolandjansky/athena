/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// nn stuff
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"
#include "lwtnn/NanReplacer.hh"

#include "BoostedJetTaggers/DeepsetXbbTagger.h"

// utilities
#include "PathResolver/PathResolver.h"

// EDM stuff
#include <xAODJet/JetContainer.h>
#include <xAODTracking/TrackParticleContainer.h>

// ATLAS tool 
#include <InDetTrackSelectionTool/InDetTrackSelectionTool.h>
#include <FlavorTagDiscriminants/BTagTrackAugmenter.h>

// to parse the input file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include <fstream>
#include <regex>
#include <set>


namespace Dexter {
// internal class to read a jet and build an std::map of intputs that
// will be passed to lwtnn.
  class DexterInputBuilder
  {
  public:
    DexterInputBuilder(const std::string& config_file,
                    const std::map<std::string, double>& offsets);
    typedef std::map<std::string, std::map<std::string, double> > VMap;
    VMap get_map(const xAOD::Jet&) const;
    size_t n_subjets(const xAOD::Jet&) const;
  private:
    typedef std::vector<ElementLink<xAOD::IParticleContainer> > ParticleLinks;
    SG::AuxElement::ConstAccessor<ParticleLinks> m_acc_subjets;
    double m_subjet_pt_threshold;
    std::string m_node_name;
    typedef SG::AuxElement::ConstAccessor<float> FloatAcc;
    typedef SG::AuxElement::ConstAccessor<double> DoubleAcc;
    typedef SG::AuxElement::ConstAccessor<int> IntAcc;
    std::vector<std::pair<std::string, FloatAcc> > m_acc_btag_floats;
    std::vector<std::pair<std::string, DoubleAcc> > m_acc_btag_doubles;
    std::vector<std::pair<std::string, IntAcc> > m_acc_btag_ints;
    std::vector<std::string> m_float_subjet_inputs;
    std::vector<std::string> m_int_subjet_inputs;
    std::map<std::string, double> m_dummy_values;
    std::map<std::string, double> m_offsets;
    TrackSelectorConfig m_track_select_cfg;
    BTagTrackAugmenter m_track_augmenter;
    std::unique_ptr<Dexter::TrackSelector> m_track_selector;

  };

  class TrackSelectorConfig
  {
    float pt_minumum = 1e3;
    float d0_maximum = 1;
    float z0_maximum = 1.5;
  };

  class TrackSelector
  {
  public:
    TrackSelector(TrackSelectorConfig = TrackSelectorConfig());
    typedef std::vector<const xAOD::TrackParticle*> Tracks;
    Tracks get_tracks(const xAOD::Jet& jet) const;

  private:
    typedef SG::AuxElement AE;
    typedef std::vector<ElementLink<xAOD::TrackParticleContainer> > TrackLinks;
    AE::ConstAccessor<TrackLinks> m_track_associator;

    InDet::InDetTrackSelectionTool m_track_selector;
    bool passed_cuts(const xAOD::TrackParticle &tp) const;
    TrackSelectorConfig m_track_select_cfg;
  };

}

namespace {
  // constants, default configuration files
  const std::string NN_CONFIG = (
    "BoostedJetTaggers/DeepsetXbbTagger/test_config.json");
}


Dexter::Dexter( const std::string& name ) :
  asg::AsgTool(name),
  m_neuralNetworkFile(""),
  m_configurationFile(NN_CONFIG),
  m_lwnn(nullptr),
  m_input_builder(nullptr),
  m_tag_threshold(1000000000.),
  m_output_value_names(),
  m_decoration_names(),
  m_decorators(),
  m_offsets()
{
  declareProperty( "neuralNetworkFile",   m_neuralNetworkFile);
  declareProperty( "configurationFile",   m_configurationFile);
  declareProperty( "tagThreshold", m_tag_threshold);
  declareProperty( "decorationNames", m_decoration_names);
}

Dexter::~Dexter() {}

StatusCode Dexter::initialize(){

  using namespace Dexter;

  // Initialize the DNN tagger tool
  ATH_MSG_INFO("Initializing Dexter tool");

  // read json file for DNN weights
  ATH_MSG_INFO("Using NN file: "+ m_neuralNetworkFile);
  std::string nn_path = PathResolverFindCalibFile(m_neuralNetworkFile);
  ATH_MSG_INFO("NN file resolved to: " + nn_path);
  lwt::GraphConfig config;
  try {
    std::ifstream input_cfg( nn_path );
    config = lwt::parse_json_graph(input_cfg);
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("NN file is garbage");
    return StatusCode::FAILURE;
  }

  // setup jet inputs
  ATH_MSG_DEBUG("Dexter inputs:");
  for (auto& input_node: config.inputs) {
    ATH_MSG_DEBUG(" input node: " << input_node.name);
    for (auto& input: input_node.variables) {
      ATH_MSG_DEBUG("  " << input);
      m_offsets[input.name] = -input.offset;
      m_scales[input.name]  = input.scale;
    }
  }
  // for (const auto& node: config.inputs) {
  //   m_var_cleaners.emplace_back(
  //     node.name, std::make_unique<lwt::NanReplacer>(m_offsets, lwt::rep::all));
  // }

  // setup track/secvtx input
  ATH_MSG_DEBUG("Dexter sequences inputs:");
  for (auto& input_node: config.input_sequences) {
    ATH_MSG_DEBUG("sequences input node: " << input_node.name);
    for (auto& input: input_node.variables) {
      ATH_MSG_DEBUG("  " << input);
      m_offsets[input.name] = -input.offset;
      m_scales[input.name]  = input.scale;
    }
  }
  // for (const auto& node: config.input_sequences) {
  //   m_var_cleaners.emplace_back(
  //     node.name, std::make_unique<lwt::NanReplacer>(m_offsets, lwt::rep::all));
  // }  

  // setup outputs
  if (config.outputs.size() != 1) {
    ATH_MSG_ERROR("Graph needs one output node");
    return StatusCode::FAILURE;
  }
  auto output_node_name = config.outputs.begin()->first;
  auto out_names = config.outputs.at(output_node_name).labels;
  m_output_value_names = out_names;

  // build the network
  try {
    m_lwnn.reset(new lwt::LightweightGraph(config, output_node_name));
  } catch (lwt::NNConfigurationException& exc) {
    ATH_MSG_ERROR("NN configuration problem: " << exc.what());
    return StatusCode::FAILURE;
  }

  // setup the input builder
  ATH_MSG_INFO( "Using config file: "<< m_configurationFile );
  std::string config_file = PathResolverFindDataFile(m_configurationFile);
  ATH_MSG_INFO( "Config file resolved to: "<< config_file );
  try {
    m_input_builder.reset(new DexterInputBuilder(config_file, m_offsets));
  } catch (boost::property_tree::ptree_error& err) {
    ATH_MSG_ERROR("Config file is garbage: " << err.what());
    return StatusCode::FAILURE;
  }

  // set up the output decorators
  //
  // if there aren't any decorators given in the constructor we look
  // in the config file
  if (m_decoration_names.size() > 0) {
    ATH_MSG_INFO("Output names set in tool setup");
  } else {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(config_file, pt);
    if (pt.count("outputs") > 0) {
      for (const auto& pair: pt.get_child("outputs.decoration_map") ) {
        m_decoration_names[pair.first] = pair.second.data();
      }
      ATH_MSG_INFO("Output names read from config file");
    } else {
      ATH_MSG_INFO("Output names read from NN file ");
    }
  }
  
  // now build the decoration vectors
  for (const std::string& name: out_names) {
    std::string dec_name = name;
    // if we've given custom decoration names, insist that this output
    // has one
    if (m_decoration_names.size() > 0) {
      if (!m_decoration_names.count(name)) {
        ATH_MSG_ERROR("NN output " + name + " has no decoration name");
        //return StatusCode::FAILURE;
      }
      dec_name = m_decoration_names.at(name);
    }
    ATH_MSG_DEBUG("Adding output " + dec_name);
    m_decorators.emplace_back(SG::AuxElement::Decorator<float>(dec_name));
  }

  return StatusCode::SUCCESS;
}

int Dexter::keep(const xAOD::Jet& jet) const {
  return getScore(jet) > m_tag_threshold;
}


std::map<std::string, double> Dexter::getScores(const xAOD::Jet& jet)
  const {

  using namespace Dexter;

  // build the jet properties into a map
  DexterInputBuilder::VMap inputs = m_input_builder->get_map(jet);

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("Dexter inputs:");
    for (auto& input_node: inputs) {
      ATH_MSG_VERBOSE(" input node: " << input_node.first);
      for (auto& input: input_node.second) {
        ATH_MSG_VERBOSE("  " << input.first << ": " << input.second);
      }
    }
  }

  // if we have any NaN or infinite values, replace them with defaults
  DexterInputBuilder::VMap cleaned;
  for (const auto& cleaner: m_var_cleaners) {
    cleaned.emplace(
      cleaner.first, cleaner.second->replace(inputs.at(cleaner.first)));
  }
    
  auto nn_output = m_lwnn->compute(cleaned);
  ATH_MSG_VERBOSE("Dexter Xbb light-flavor score " << nn_output.at(m_output_value_names.at(0)));
  ATH_MSG_VERBOSE("Dexter Xbb b-flavor score " << nn_output.at(m_output_value_names.at(1)));
  ATH_MSG_VERBOSE("Dexter Xbb bb-flavor score " << nn_output.at(m_output_value_names.at(2)));
  return nn_output;
}

double Dexter::getScore(const xAOD::Jet& jet) const {
  if (m_output_value_names.size() > 1) {
    ATH_MSG_DEBUG("asked for the first tagger score from a multi-class tagger");
  }
  auto nn_output = getScores(jet);
  return nn_output.at(m_output_value_names.at(0));
}

void Dexter::decorate(const xAOD::Jet& jet) const {
  std::map<std::string, double> scores = getScores(jet);
  size_t dec_num = 0;
  for (const auto& dec: m_decorators) {
    dec(jet) = scores.at(m_output_value_names.at(dec_num));
    dec_num++;
  }
}
void Dexter::decorateSecond(const xAOD::Jet& ref,
                                  const xAOD::Jet& target) const {
  std::map<std::string, double> scores = getScores(ref);
  size_t dec_num = 0;
  for (const auto& dec: m_decorators) {
    dec(target) = scores.at(m_output_value_names.at(dec_num));
    dec_num++;
  }
}

std::set<std::string> Dexter::decorationNames() const {
  std::set<std::string> out;
  for (const auto& pair: m_decoration_names) {
    out.insert(pair.second);
  }
  return out;
}

size_t Dexter::n_subjets(const xAOD::Jet& jet) const {
  return m_input_builder->n_subjets(jet);
}

namespace {

  // For things that we do access from the EDM, we need to know the
  // type. We use regexes to assign these.
  typedef std::vector<std::pair<std::regex, std::string> > TypeRegexes;
  std::string get_var_type(const TypeRegexes& type_regexes,
                           const std::string& var_name) {
    for (const auto& type_regex: type_regexes) {
      if (std::regex_search(var_name, type_regex.first)) {
        return type_regex.second;
      }
    }
    throw std::logic_error(
      "can't find a type match for variable " + var_name);
  }

}

namespace Dexter {

  ////////////////////////////////////////
  /// Input map builder implementation ///
  ////////////////////////////////////////

  DexterInputBuilder::DexterInputBuilder(
    const std::string& input_file,
    const std::map<std::string, double>& offsets):
    m_acc_subjets("ExKt2GASubJets"),
    m_offsets(offsets)
  {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(input_file, pt);

    // tracks selector configuration  
    m_track_select_cfg.pt_minumum = pt.get<float>("track.track_pt_minimum");
    m_track_select_cfg.d0_maximum = pt.get<float>("track.track_d0_maximum");
    m_track_select_cfg.z0_maximum = pt.get<float>("track.track_z0_maximum");
    m_track_selector.reset(new TrackSelector(m_track_select_cfg));

    m_node_name = pt.get<std::string>("node_name");

    auto sjets = pt.get<std::string>("subjet.collection");
    m_subjet_pt_threshold = pt.get<double>("subjet.pt_threshold");
    m_acc_subjets = SG::AuxElement::ConstAccessor<ParticleLinks>(sjets);

    // build regexes to figure out variable types
    std::vector<std::pair<std::regex, std::string> > type_regexes;
    for (auto& regex_spec: pt.get_child("types.regexes")) {
      std::string regex_str = regex_spec.second.get<std::string>("regex");
      std::string type = regex_spec.second.get<std::string>("type");
      std::regex regex(regex_str);
      type_regexes.emplace_back(regex, type);
    }

    std::vector<std::string> subjet_var;
    for (auto& var: pt.get_child("subjet.variables")) {
       subjet_var.emplace_back(var.second.data());
    }

    // build input accessors
    std::set<std::string> float_subjet_inputs;
    std::set<std::string> int_subjet_inputs;
    for (const std::string& var_name: subjet_var)  { 
          std::string type = get_var_type(type_regexes, var_name);
          if (type == "double") {
            m_acc_btag_doubles.emplace_back(var_name, var_name);
            float_subjet_inputs.insert(var_name);
          } else if (type == "int") {
            m_acc_btag_ints.emplace_back(var_name, var_name);
            int_subjet_inputs.insert(var_name);
          } else if (type == "float") {
            m_acc_btag_floats.emplace_back(var_name, var_name);
            float_subjet_inputs.insert(var_name);
          } else {
            throw std::logic_error("don't know how to access " + var_name);
          }
      } 

    // In cases where inputs aren't defined, we need to fill them with
    // dummy values. Keep track of all the values that need defaults
    // here.
    m_float_subjet_inputs.insert(
      m_float_subjet_inputs.end(),
      float_subjet_inputs.begin(), float_subjet_inputs.end());
    m_int_subjet_inputs.insert(
      m_int_subjet_inputs.end(),
      int_subjet_inputs.begin(), int_subjet_inputs.end());
  }

  DexterInputBuilder::VMap DexterInputBuilder::get_map(
    const xAOD::Jet& jet) const {

    // inputs dict
    std::map<std::string, std::map<std::string, float> > node_input;
    std::map<std::string, float> inputs;

    // fat jet inputs
    inputs["jet_pt"] = jet.pt();  // The pt of fat jet is in MeV in training
    inputs["jet_eta"] = jet.eta();

    // get subjets for flight direction ref
    auto subjet_links = m_acc_subjets(*jet);
    std::vector<const xAOD::Jet*> subjets;
    for (const auto& el: subjet_links) {
      const auto* jet = dynamic_cast<const xAOD::Jet*>(*el);
      if (!jet) throw std::logic_error("subjet is invalid");
      if (jet->pt() < m_subjet_pt_threshold ) continue;
      subjets.push_back(jet);
    }
    auto pt_sort = [](auto& sj1, auto& sj2 ){
      return sj1->pt() > sj2->pt();
    };
    std::sort(subjets.begin(), subjets.end(), pt_sort);

    // get tracks
    std::vector<const xAOD::TrackParticle*> tracks;
    for (size_t subjet_n = 0; subjet_n < subjets.size(); subjet_n++) {
      auto& subjet = *subjets.at(subjet_n);
      auto temp_tracks = m_track_selector.get_tracks(subjet);
      for (const auto& track: temp_tracks) {
        m_track_augmenter.augment_with_ip(*track, subjet);
      }

      tracks.insert(tracks.end(), temp_tracks.begin(), temp_tracks.end());
    }
    auto by_sd0(const xAOD::TrackParticle* t1,
                const xAOD::TrackParticle* t2) {
      static SG::AuxElement::ConstAccessor<float> d0("btagIp_d0");
      static SG::AuxElement::ConstAccessor<float> d0s("btagIp_d0Uncertainty");
      return std::abs(d0(*t1) / d0s(*t1)) > std::abs(d0(*t2) / d0s(*t2));
    }
    std::sort(tracks.begin(), tracks.end(), by_sd0);



    // get secvtx 
    std::vector<const xAOD::Vertex*> sectxs;


    for (size_t subjet_n = 0; subjet_n < 3; subjet_n++) {
      bool have_jet = subjet_n < subjets.size();
      std::map<std::string, double> subjet_inputs;
      std::string order = "_" + std::to_string(subjet_n+1);
      if (have_jet && subjets.at(subjet_n)->pt() > m_subjet_pt_threshold) {
        const xAOD::Jet* subjet = subjets.at(subjet_n);
        const auto* btag = subjet->btagging();
        for (const auto& pair: m_acc_btag_floats) {
          inputs[pair.first + order] = pair.second(*btag);
        }
        for (const auto& pair: m_acc_btag_doubles) {
          inputs[pair.first + order] = pair.second(*btag);
        }
        for (const auto& pair: m_acc_btag_ints) {
          inputs[pair.first + order] = pair.second(*btag);
        }
      } else {
        for (const std::string& input_name: m_float_subjet_inputs) {
          inputs[input_name + order] = m_offsets.at(input_name + order);
        }
        for (const std::string& input_name: m_int_subjet_inputs) {
          inputs[input_name + order] = m_offsets.at(input_name + order);
        }
      }
    }
    
    node_input[m_node_name] = inputs;

    return node_input;
  }

  size_t DexterInputBuilder::n_subjets(const xAOD::Jet& jet) const {
    auto subjet_links = m_acc_subjets(*jet);
    return subjet_links.size();
  }

  ///////////////////////////////////////
  /// Tracks selector implementation  ///
  ///////////////////////////////////////

  TrackSelector(TrackSelectorConfig config):
    m_track_associator("BTagTrackToJetAssociator"),
    m_track_selector("InDetTrackSelectionTool", "Loose"),
    m_track_select_cfg(config)
  {
    if (!m_track_selector.initialize()) {
      throw std::logic_error("can't initialize track seletor");
    }
  }
  
  Tracks TrackSelector::get_tracks(const xAOD::Jet& jet) const
  {
    const xAOD::BTagging *btagging = jet.btagging();
    std::vector<const xAOD::TrackParticle*> tracks;
    for (const auto &link : m_track_associator(*btagging)) {
      if(link.isValid()) {
        const xAOD::TrackParticle *tp = *link;
        if (m_track_selector.accept(tp) && passed_cuts(*tp)) {
          tracks.push_back(tp);
        }
      } else {
        throw std::logic_error("invalid track link");
      }
    }
    return tracks;
  }

  bool passed_cuts(const xAOD::TrackParticle& tp) const
  {
    static SG::AuxElement::ConstAccessor<float> d0("btagIp_d0");
    static SG::AuxElement::ConstAccessor<float> z0("btagIp_z0SinTheta");
    float pt = tp.pt();
    return ((std::abs(d0(tp)) < m_track_select_cfg.d0_maximum) && 
            (std::abs(z0(tp)) < m_track_select_cfg.z0_maximum) && 
            (pt > m_track_select_cfg.pt_minumum));
  }

}
