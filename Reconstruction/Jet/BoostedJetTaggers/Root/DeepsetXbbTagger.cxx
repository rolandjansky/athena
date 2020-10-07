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
#include <xAODTracking/Vertex.h>

// ATLAS tool 
#include <InDetTrackSelectionTool/InDetTrackSelectionTool.h>
#include <FlavorTagDiscriminants/BTagTrackAugmenter.h>
#include <xAODBTagging/SecVtxHelper.h>

// to parse the input file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

// ROOT
#include <TLorentzVector.h>

#include <fstream>
#include <regex>
#include <set>
#include <cmath>
#include <tuple>

namespace DeepsetXbbTagger {

// internal class to filter tracks in jet and return an std::vector of tracks
// that passed the selection cuts  
  class TrackSelector
  {
  public:
    TrackSelector(TrackSelectorConfig = TrackSelectorConfig());
    typedef std::vector<const xAOD::TrackParticle*> Tracks;
    Tracks get_tracks(const xAOD::Jet&) const;
  private:
    typedef SG::AuxElement AE;
    typedef std::vector<ElementLink<xAOD::TrackParticleContainer> > TrackLinks;
    AE::ConstAccessor<TrackLinks> m_track_associator;

    InDet::InDetTrackSelectionTool m_track_selector;
    bool passed_cuts(const xAOD::TrackParticle &) const;
    TrackSelectorConfig m_track_select_cfg;
  };


// internal class to read a jet and build an std::map of intputs that
// will be passed to lwtnn.
  class DexterInputBuilder
  {
  public:
    DexterInputBuilder(const std::string& config_file,
                    const std::map<std::string, double>& offsets);
    typedef std::map<std::string, std::map<std::string, double> > VMap;
    typedef std::map<std::string, std::map<std::string, std::vector<double>> > VSMap;
    std::tuple<VMap,VSMap> get_map(const xAOD::Jet&,  const xAOD::VertexContainer*, const xAOD::Vertex*) const;
    size_t n_subjets(const xAOD::Jet&) const;
  private:
    typedef std::vector<ElementLink<xAOD::IParticleContainer> > ParticleLinks;
    SG::AuxElement::ConstAccessor<ParticleLinks> m_acc_subjets;
    double m_subjet_pt_threshold;
    std::string m_node_name;
    std::string m_tracks_node_name;
    std::string m_secvtx_node_name;
    size_t m_max_tracks;
    size_t m_max_secvtx;
    typedef SG::AuxElement::ConstAccessor<float> FloatAcc;
    typedef SG::AuxElement::ConstAccessor<double> DoubleAcc;
    typedef SG::AuxElement::ConstAccessor<uint8_t> IntAcc;
    std::vector<std::pair<std::string, DoubleAcc> > m_acc_track_doubles;
    std::vector<std::pair<std::string, FloatAcc> > m_acc_track_floats;
    std::vector<std::pair<std::string, IntAcc> > m_acc_track_ints;
    std::vector<std::pair<std::string, DoubleAcc> > m_acc_secvtx_doubles;
    std::vector<std::pair<std::string, FloatAcc> > m_acc_secvtx_floats;
    std::vector<std::pair<std::string, IntAcc> > m_acc_secvtx_ints;
    std::vector<std::string> m_float_track_inputs;
    std::vector<std::string> m_int_track_inputs;
    std::vector<std::string> m_float_secvtx_inputs;
    std::vector<std::string> m_int_secvtx_inputs;    
    std::map<std::string, double> m_dummy_values;
    std::map<std::string, double> m_offsets;
    TrackSelectorConfig m_track_select_cfg;
    std::unique_ptr<BTagTrackAugmenter> m_track_augmenter;
    std::unique_ptr<TrackSelector> m_track_selector;
    std::unique_ptr<BTagSecVtxAugmenter> m_secvtx_augmenter;
  };



  class BTagSecVtxAugmenter
  {
  public:
    BTagSecVtxAugmenter(const std::string& decorator_prefix = "secvtx");
    // this is the function that actually does the decoration
    void decorate(const xAOD::Vertex* secvtx, const xAOD::Jet& jet) const;
    void decorateWithSubjet(const xAOD::Vertex* secvtx, std::vector<const xAOD::Jet*> subjets) const;
    void decorateWithPrimary(const xAOD::Vertex* secvtx, const xAOD::Vertex* priVertex) const;
  private:
    typedef SG::AuxElement AE;
    AE::Decorator<float> m_log_mass;
    AE::Decorator<float> m_deltaEta;
    AE::Decorator<float> m_deltaPhi;
    AE::Decorator<float> m_pTfrac;
    AE::ConstAccessor<float> tc_lvt_px;
    AE::ConstAccessor<float> tc_lvt_py;
    AE::ConstAccessor<float> tc_lvt_pz;
    AE::ConstAccessor<float> tc_lvt_ee;
    // primary vertex 
    AE::Decorator<float> m_Lxy;
    AE::Decorator<float> m_Lz;
    AE::Decorator<float> m_Lxy_significance;
    AE::Decorator<float> m_Lz_significance;

  };

}

namespace {
  // constants, default configuration files
  const std::string NN_CONFIG = (
    "BoostedJetTaggers/DeepsetXbbTagger/test_config.json");
  const std::string SECVTX_NAME = (
    "SoftBVrtClusterTool_MSVTight_Vertices");  
}


Dexter::Dexter( const std::string& name ) :
  asg::AsgTool(name),
  m_neuralNetworkFile(""),
  m_configurationFile(NN_CONFIG),
  m_secvtx_collection_name(SECVTX_NAME),
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
  declareProperty( "secvtxCollection",   m_secvtx_collection_name);
  declareProperty( "tagThreshold", m_tag_threshold);  
  declareProperty( "decorationNames", m_decoration_names);
}

Dexter::~Dexter() {}

StatusCode Dexter::initialize(){

  using namespace DeepsetXbbTagger;

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
    m_decorators.emplace_back(SG::AuxElement::Decorator<double>(dec_name));
  }

  return StatusCode::SUCCESS;
}


std::map<std::string, double> Dexter::getScores(const xAOD::Jet& jet)
  const {

  using namespace DeepsetXbbTagger;

  // build the jet properties into a map
  DexterInputBuilder::VMap  inputs;
  DexterInputBuilder::VSMap input_sequences;
  
  const xAOD::VertexContainer *primVertexs = nullptr;
  if(evtStore()->retrieve(primVertexs, "PrimaryVertices").isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
  }

  const xAOD::Vertex *priVertex = getPrimaryVertex(primVertexs);
  if (!priVertex) {
      ATH_MSG_WARNING(" No PrimaryVertice Found");
  } 

  const xAOD::VertexContainer *secvtxs = nullptr;
  if(evtStore()->retrieve(secvtxs, m_secvtx_collection_name).isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
  }

  std::tie(inputs, input_sequences) = m_input_builder->get_map(jet, secvtxs, priVertex);

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
  // DexterInputBuilder::VSMap cleaned_sequences;
  // for (const auto& cleaner: m_var_cleaners) {
  //   cleaned.emplace(
  //     cleaner.first, cleaner.second->replace(inputs.at(cleaner.first)));
  // }

  auto nn_output = m_lwnn->compute(inputs, input_sequences);
  ATH_MSG_VERBOSE("Dexter Xbb light-flavor score " << nn_output.at(m_output_value_names.at(0)));
  ATH_MSG_VERBOSE("Dexter Xbb b-flavor score " << nn_output.at(m_output_value_names.at(1)));
  ATH_MSG_VERBOSE("Dexter Xbb bb-flavor score " << nn_output.at(m_output_value_names.at(2)));
  return nn_output;
}

int Dexter::keep(const xAOD::Jet& jet) const {
  return getScore(jet) > m_tag_threshold;
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

xAOD::Vertex* Dexter::getPrimaryVertex(const xAOD::VertexContainer* vertexContainer) const {                                                                                                           
  for( auto vtx_itr : *vertexContainer ) {
      if(vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) { continue; }
      return vtx_itr;
    }
  return 0;
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

namespace DeepsetXbbTagger {

  ////////////////////////////////////////
  /// Input map builder implementation ///
  ////////////////////////////////////////

  DexterInputBuilder::DexterInputBuilder(
    const std::string& input_file,
    const std::map<std::string, double>& offsets):
    m_acc_subjets("catKittyCat"),
    m_offsets(offsets)
  {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(input_file, pt);

    m_node_name = pt.get<std::string>("node_name");

    auto sjets = pt.get<std::string>("subjet.collection");
    m_subjet_pt_threshold = pt.get<double>("subjet.pt_threshold");
    m_acc_subjets = SG::AuxElement::ConstAccessor<ParticleLinks>(sjets);

    m_tracks_node_name = pt.get<std::string>("track.node_name");
    m_max_tracks = pt.get<size_t>("track.maximum_number");

    // tracks selector configuration  
    m_track_select_cfg.pt_minumum = pt.get<float>("track.track_pt_minimum");
    m_track_select_cfg.d0_maximum = pt.get<float>("track.track_d0_maximum");
    m_track_select_cfg.z0_maximum = pt.get<float>("track.track_z0_maximum");
    m_track_selector.reset(new TrackSelector(m_track_select_cfg));
    m_track_augmenter.reset(new BTagTrackAugmenter());

    m_secvtx_node_name = pt.get<std::string>("sexctx.node_name");
    m_max_secvtx = pt.get<size_t>("sexctx.maximum_number");
    m_secvtx_augmenter.reset(new BTagSecVtxAugmenter());

    // build regexes to figure out variable types
    std::vector<std::pair<std::regex, std::string> > type_regexes;
    for (auto& regex_spec: pt.get_child("types.regexes")) {
      std::string regex_str = regex_spec.second.get<std::string>("regex");
      std::string type = regex_spec.second.get<std::string>("type");
      std::regex regex(regex_str);
      type_regexes.emplace_back(regex, type);
    }

    std::vector<std::string> track_var;
    for (auto& var: pt.get_child("track.variables")) {
       track_var.emplace_back(var.second.data());
    }    

    std::vector<std::string> secvtx_var;
    for (auto& var: pt.get_child("sexctx.variables")) {
       secvtx_var.emplace_back(var.second.data());
    } 

    // build input accessors
    // tracks
    std::set<std::string> float_track_inputs;
    std::set<std::string> int_track_inputs;
    for (const std::string& var_name: track_var)  { 
          std::string type = get_var_type(type_regexes, var_name);
          if (type == "double") {
            m_acc_track_doubles.emplace_back(var_name, var_name);
            float_track_inputs.insert(var_name);
          } else if (type == "int") {
            m_acc_track_ints.emplace_back(var_name, var_name);
            int_track_inputs.insert(var_name);
          } else if (type == "float") {
            m_acc_track_floats.emplace_back(var_name, var_name);
            float_track_inputs.insert(var_name);
          } else {
            throw std::logic_error("don't know how to access " + var_name);
          }
    } 
    // In cases where inputs aren't defined, we need to fill them with
    // dummy values. Keep track of all the values that need defaults
    // here.
    m_float_track_inputs.insert(
      m_float_track_inputs.end(),
      float_track_inputs.begin(), float_track_inputs.end());
    m_int_track_inputs.insert(
      m_int_track_inputs.end(),
      int_track_inputs.begin(), int_track_inputs.end());

    // secvtx
    std::set<std::string> float_secvtx_inputs;
    std::set<std::string> int_secvtx_inputs;
    for (const std::string& var_name: secvtx_var)  { 
          std::string type = get_var_type(type_regexes, var_name);
          if (type == "double") {
            m_acc_secvtx_doubles.emplace_back(var_name, var_name);
            float_secvtx_inputs.insert(var_name);
          } else if (type == "int") {
            m_acc_secvtx_ints.emplace_back(var_name, var_name);
            int_secvtx_inputs.insert(var_name);
          } else if (type == "float") {
            m_acc_secvtx_floats.emplace_back(var_name, var_name);
            float_secvtx_inputs.insert(var_name);
          } else {
            throw std::logic_error("don't know how to access " + var_name);
          }
    } 
    // In cases where inputs aren't defined, we need to fill them with
    // dummy values. Keep track of all the values that need defaults
    // here.
    m_float_secvtx_inputs.insert(
      m_float_secvtx_inputs.end(),
      float_secvtx_inputs.begin(), float_secvtx_inputs.end());
    m_int_secvtx_inputs.insert(
      m_int_secvtx_inputs.end(),
      int_secvtx_inputs.begin(), int_secvtx_inputs.end());

  }

  std::tuple<DexterInputBuilder::VMap, DexterInputBuilder::VSMap> DexterInputBuilder::get_map(
    const xAOD::Jet& jet,  const xAOD::VertexContainer* secvtxs, const xAOD::Vertex* priVertex) const {

    // inputs dict
    std::map<std::string, std::map<std::string, double> > node_input;
    std::map<std::string, double> inputs;

    // fat jet inputs
    inputs["jet_pt"] = jet.pt();  // The pt of jet is in MeV in training
    inputs["jet_eta"] = jet.eta();

    // get subjets for flight direction ref
    auto subjet_links = m_acc_subjets(jet);
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

    // input sequences
    std::map<std::string, std::map<std::string, std::vector<double>> > node_input_sequences;
    std::map<std::string, std::vector<double>> tracks_input_sequences;
    std::map<std::string, std::vector<double>> secvtx_input_sequences;

    SG::AuxElement::Decorator<float> track_deltaEta("deltaEta_subjet");
    SG::AuxElement::Decorator<float> track_deltaPhi("deltaPhi_subjet");
    SG::AuxElement::Decorator<float> track_pTfrac("pTfrac_jet");

    // get tracks
    std::vector<const xAOD::TrackParticle*> tracks;
    for (size_t subjet_n = 0; subjet_n < subjets.size(); subjet_n++) {
      auto& subjet = *subjets.at(subjet_n);
      auto temp_tracks = m_track_selector->get_tracks(subjet);
      for (const auto& track: temp_tracks) {
        m_track_augmenter->augment_with_ip(*track, subjet);
        track_deltaEta(*track) = track->p4().Eta() - subjet.p4().Eta();
        track_deltaPhi(*track) = track->p4().DeltaPhi(subjet.p4()); 
        track_pTfrac(*track) = std::log(track->pt()/jet.pt());
      }

      tracks.insert(tracks.end(), temp_tracks.begin(), temp_tracks.end());
    }

    auto signed_d0_sort = [](const xAOD::TrackParticle* t1, const xAOD::TrackParticle* t2) {
      static SG::AuxElement::ConstAccessor<float> d0_signed("IP3D_signed_d0_significance");
      return d0_signed(*t1) > d0_signed(*t2);
    };
    std::sort(tracks.begin(), tracks.end(), signed_d0_sort);

    size_t max_trk = (m_max_tracks < tracks.size()) ? m_max_tracks: tracks.size();
    for (const auto& pair: m_acc_track_doubles) {
      std::vector<double> track_input;
      for (size_t trk_n = 0; trk_n < max_trk; trk_n++) {
        const xAOD::TrackParticle* trk = tracks.at(trk_n);
        track_input.push_back(pair.second(*trk));
      }
      tracks_input_sequences[pair.first] = track_input;
    }
    for (const auto& pair: m_acc_track_floats) {
      std::vector<double> track_input;
      for (size_t trk_n = 0; trk_n < max_trk; trk_n++) {
        const xAOD::TrackParticle* trk = tracks.at(trk_n);
        track_input.push_back(pair.second(*trk));
      }
      tracks_input_sequences[pair.first] = track_input;
    }
    for (const auto& pair: m_acc_track_ints) {
      std::vector<double> track_input;
      for (size_t trk_n = 0; trk_n < max_trk; trk_n++) {
        const xAOD::TrackParticle* trk = tracks.at(trk_n);
        track_input.push_back(pair.second(*trk));
      }
      tracks_input_sequences[pair.first] = track_input;
    }

    // get secvtx  
    static SG::AuxElement::ConstAccessor<float> tc_lvt_px("tc_lvt_px");
    static SG::AuxElement::ConstAccessor<float> tc_lvt_py("tc_lvt_py");
    static SG::AuxElement::ConstAccessor<float> tc_lvt_pz("tc_lvt_pz");
    static SG::AuxElement::ConstAccessor<float> tc_lvt_ee("tc_lvt_ee");
    std::vector<const xAOD::Vertex*> msvtx;
    for(xAOD::Vertex *secvtx: *secvtxs) { 

      TLorentzVector SecVtxVector;
      SecVtxVector.SetPxPyPzE(tc_lvt_px(*secvtx), tc_lvt_py(*secvtx), tc_lvt_pz(*secvtx), tc_lvt_ee(*secvtx));
      if(SecVtxVector.DeltaR(jet.p4()) > 0.8) continue;

      m_secvtx_augmenter->decorate(secvtx, jet);
      m_secvtx_augmenter->decorateWithSubjet(secvtx, subjets);
      m_secvtx_augmenter->decorateWithPrimary(secvtx, priVertex);

      msvtx.push_back(secvtx);

    }
    auto Lxy_sig_sort = [](const xAOD::Vertex* v1, const xAOD::Vertex* v2){
      static SG::AuxElement::ConstAccessor<float> Lxy_sig("Lxy_significance");
      return Lxy_sig(*v1) > Lxy_sig(*v2);
    };
    std::sort(msvtx.begin(), msvtx.end(), Lxy_sig_sort);

    size_t max_secvtx = (m_max_secvtx < msvtx.size()) ? m_max_secvtx : msvtx.size();
    for (const auto& pair: m_acc_secvtx_doubles) {
      std::vector<double> secvtx_input;
      for (size_t vt_n = 0; vt_n < max_secvtx; vt_n++) {
        const xAOD::Vertex* vtx = msvtx.at(vt_n);
        secvtx_input.push_back(pair.second(*vtx));
      }
      secvtx_input_sequences[pair.first] = secvtx_input;
    }
    for (const auto& pair: m_acc_secvtx_floats) {
      std::vector<double> secvtx_input;
      for (size_t vt_n = 0; vt_n < max_secvtx; vt_n++) {
        const xAOD::Vertex* vtx = msvtx.at(vt_n);
        secvtx_input.push_back(pair.second(*vtx));
      }
      secvtx_input_sequences[pair.first] = secvtx_input;
    }
    for (const auto& pair: m_acc_secvtx_ints) {
      std::vector<double> secvtx_input;
      for (size_t vt_n = 0; vt_n < max_secvtx; vt_n++) {
        const xAOD::Vertex* vtx = msvtx.at(vt_n);
        secvtx_input.push_back(pair.second(*vtx));
      }
      secvtx_input_sequences[pair.first] = secvtx_input;
    }

    node_input[m_node_name] = inputs;
    node_input_sequences[m_tracks_node_name] = tracks_input_sequences;
    node_input_sequences[m_secvtx_node_name] = secvtx_input_sequences;

    return std::make_tuple(node_input, node_input_sequences);
  }

  size_t DexterInputBuilder::n_subjets(const xAOD::Jet& jet) const {
    auto subjet_links = m_acc_subjets(jet);
    size_t n_subjets = 0;
    for (const auto& el: subjet_links) {
      const auto* jet = dynamic_cast<const xAOD::Jet*>(*el);
      if (!jet) throw std::logic_error("subjet is invalid");
      if (jet->pt() < m_subjet_pt_threshold ) continue;
      n_subjets += 1;
    }
    return n_subjets;
  }

  ///////////////////////////////////////
  /// Tracks selector implementation  ///
  ///////////////////////////////////////

  TrackSelector::TrackSelector(TrackSelectorConfig config):
    m_track_associator("BTagTrackToJetAssociator"),
    m_track_selector("InDetTrackSelectionTool", "Loose"),
    m_track_select_cfg(config)
  {
    if (!m_track_selector.initialize()) {
      throw std::logic_error("can't initialize track seletor");
    }
  }
  
  TrackSelector::Tracks TrackSelector::get_tracks(const xAOD::Jet& jet) const
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

  bool TrackSelector::passed_cuts(const xAOD::TrackParticle& tp) const
  {
    static SG::AuxElement::ConstAccessor<float> d0("btagIp_d0");
    static SG::AuxElement::ConstAccessor<float> z0("btagIp_z0SinTheta");
    float pt = tp.pt();
    return ((std::abs(d0(tp)) < m_track_select_cfg.d0_maximum) && 
            (std::abs(z0(tp)) < m_track_select_cfg.z0_maximum) && 
            (pt > m_track_select_cfg.pt_minumum));
  }

  // the constructor just builds the decorator
  BTagSecVtxAugmenter::BTagSecVtxAugmenter(const std::string& prefix):
    m_log_mass(prefix + "_log_mass"),
    m_deltaEta("deltaEta_subjet"),
    m_deltaPhi("deltaPhi_subjet"),
    m_pTfrac("pTfrac_jet"),
    tc_lvt_px("tc_lvt_px"),
    tc_lvt_py("tc_lvt_py"),
    tc_lvt_pz("tc_lvt_pz"),
    tc_lvt_ee("tc_lvt_ee"),
    m_Lxy("Lxy"),
    m_Lz("Lz"),
    m_Lxy_significance("Lxy_significance"),
    m_Lz_significance("Lz_significance")
  {
  }

  void BTagSecVtxAugmenter::decorate(const xAOD::Vertex *secvtx, const xAOD::Jet& jet) const {

    TLorentzVector SecVtx;
    SecVtx.SetPxPyPzE(tc_lvt_px(*secvtx), tc_lvt_py(*secvtx), tc_lvt_pz(*secvtx), tc_lvt_ee(*secvtx));

    float log_mass = std::log(SecVtx.M());
    float pTfrac = SecVtx.Pt()/jet.pt();
    m_pTfrac(*secvtx) = pTfrac;
    m_log_mass(*secvtx) = log_mass;

  }

  void BTagSecVtxAugmenter::decorateWithSubjet(const xAOD::Vertex *secvtx, std::vector<const xAOD::Jet*> subjets) const {

    TLorentzVector SecVtx;
    SecVtx.SetPxPyPzE(tc_lvt_px(*secvtx), tc_lvt_py(*secvtx), tc_lvt_pz(*secvtx), tc_lvt_ee(*secvtx));
    if(SecVtx.DeltaR(subjets.at(0)->p4()) < SecVtx.DeltaR(subjets.at(1)->p4())) {
      float dEta = SecVtx.Eta() - subjets.at(0)->p4().Eta();
      float dPhi = SecVtx.DeltaPhi(subjets.at(0)->p4()); 
      m_deltaEta(*secvtx) = dEta;
      m_deltaPhi(*secvtx) = dPhi;
    }
    else {
      float dEta = SecVtx.Eta() - subjets.at(1)->p4().Eta();
      float dPhi = SecVtx.DeltaPhi(subjets.at(1)->p4()); 
      m_deltaEta(*secvtx) = dEta;
      m_deltaPhi(*secvtx) = dPhi;
    }

  }

  void BTagSecVtxAugmenter::decorateWithPrimary(const xAOD::Vertex* secvtx, const xAOD::Vertex* priVertex) const {

    float Lx = secvtx->position().x()-priVertex->position().x();
    float Ly = secvtx->position().y()-priVertex->position().y();
    float Lz = secvtx->position().z()-priVertex->position().z();

    AmgSymMatrix(3) covariance = secvtx->covariancePosition() + priVertex->covariancePosition(); 
    AmgSymMatrix(3) invert_covariance = covariance.inverse();

    Amg::Vector3D dL(Lx,Ly,Lz);

    const float z0_sig = fabs(Lz)/std::sqrt(covariance(2,2));
    const float Lxy = std::hypot(Lx,Ly);
    const float Lxy_sig = sqrt( Lx*Lx*invert_covariance(0,0) + 2*Lx*Ly*invert_covariance(0,1) + Ly*Ly*invert_covariance(1,1));

    m_Lxy(*secvtx) = fabs(Lxy);
    m_Lxy_significance(*secvtx) = Lxy_sig;
    m_Lz(*secvtx) = fabs(Lz);
    m_Lz_significance(*secvtx) = z0_sig;

  }


}
