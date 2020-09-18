/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackVertexAssociationTool/MVATrackVertexAssociationTool.h"

#include "AsgDataHandles/ReadHandle.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "PathResolver/PathResolver.h"

#include "lwtnn/NNLayerConfig.hh"
#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"

#include <fstream>
#include <iterator>
#include <stdexcept>

namespace CP {

MVATrackVertexAssociationTool::MVATrackVertexAssociationTool(const std::string& name) :
  AsgTool(name)
{
  declareProperty("NetworkFileName", m_fileName,        "Name of the input lwtnn network file."                                                              );
  declareProperty("InputNames",      m_inputNames,      "Vector of the network's input variable names (std::vector<std::string>)."                           );
  declareProperty("InputTypes",      m_inputTypes,      "Vector of the network's input variable evaluator types (std::vector<CP::MVAEvaluatorInput::Input>).");
  declareProperty("OutputNodeName",  m_outputName,      "Name of the output node to cut on for TVA."                                                         );
  declareProperty("IsSequential",    m_isSequential,    "Is the network sequential (true) or functional (false)."                                            );
  declareProperty("WorkingPoint",    m_wp,              "TVA working point to apply."                                                                        );
  declareProperty("OutputCut",       m_cut,             "TVA cut value on the output value (set manually with \"Custom\" WP)."                               );
  declareProperty("UsePathResolver", m_usePathResolver, "Use the PathResolver for finding the input lwtnn network file."                                     );
}

StatusCode MVATrackVertexAssociationTool::initialize() {

  ATH_MSG_INFO("Initializing MVATrackVertexAssociationTool.");

  // Init EventInfo
  ATH_CHECK(m_eventInfo.initialize());

  // Init network
  if (!this->initializeNetwork()) {
    ATH_MSG_ERROR("Could not initialize network properly.");
    return StatusCode::FAILURE;
  }

  // Map our working point to a cut on the MVA output discriminant
  if (m_wp == "Tight") {
    m_cut = 0.85;
  }
  else if (m_wp == "Custom") {
    // Nothing to do here
  }
  else {
    ATH_MSG_ERROR("Invalid TVA working point \"" << m_wp << "\" - for a custom configuration, please provide \"Custom\" for the \"WorkingPoint\" property.");
    return StatusCode::FAILURE;
  }

  // Some extra printout for Custom
  if (m_wp == "Custom") {
    ATH_MSG_INFO("TVA working point \"Custom\" provided - tool properties are initialized to default values unless explicitly set by the user.");
  }
  else {
    ATH_MSG_INFO("TVA working point \"" << m_wp << "\" provided - tool properties have been configured accordingly.");
  }

  ATH_MSG_INFO("Cut on MVA output discriminant: " << m_cut);

  return StatusCode::SUCCESS;
}

StatusCode MVATrackVertexAssociationTool::finalize() {

  ATH_MSG_INFO("Finalizing MVATrackVertexAssociationTool.");

  // Finalize network
  if (!this->finalizeNetwork()) {
    ATH_MSG_ERROR("Could not finalize network properly.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

bool MVATrackVertexAssociationTool::isCompatible(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx) const {
  float mvaOutput;
  return isMatch(trk, vx, mvaOutput);
}

xAOD::TrackVertexAssociationMap MVATrackVertexAssociationTool::getMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const {
  return getMatchMapInternal(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap MVATrackVertexAssociationTool::getMatchMap(const xAOD::TrackParticleContainer& trkCont, const xAOD::VertexContainer& vxCont) const {
  return getMatchMapInternal(trkCont, vxCont);
}

const xAOD::Vertex* MVATrackVertexAssociationTool::getUniqueMatchVertex(const xAOD::TrackParticle& trk, std::vector<const xAOD::Vertex*>& vx_list) const {
  return getUniqueMatchVertexInternal(trk, vx_list);
}

ElementLink<xAOD::VertexContainer> MVATrackVertexAssociationTool::getUniqueMatchVertexLink(const xAOD::TrackParticle& trk, const xAOD::VertexContainer& vxCont) const {
  ElementLink<xAOD::VertexContainer> vx_link_tmp;
  const xAOD::Vertex* vx_tmp = getUniqueMatchVertexInternal(trk, vxCont);
  if (vx_tmp) {
    vx_link_tmp.toContainedElement(vxCont, vx_tmp);
  }
  return vx_link_tmp;
}

xAOD::TrackVertexAssociationMap MVATrackVertexAssociationTool::getUniqueMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const {
  return getUniqueMatchMapInternal(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap MVATrackVertexAssociationTool::getUniqueMatchMap(const xAOD::TrackParticleContainer& trkCont, const xAOD::VertexContainer& vxCont) const {
  return getUniqueMatchMapInternal(trkCont, vxCont);
}

// --------------- //
// Private methods //
// --------------- //

bool MVATrackVertexAssociationTool::isMatch(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, float& mvaOutput) const {

  // Fake vertex, return false
  if (vx.vertexType() == xAOD::VxType::NoVtx) {
    return false;
  }

  // Retrieve our EventInfo
  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfo);
  if (!evt.isValid()) {
    throw std::runtime_error("ERROR in CP::MVATrackVertexAssociationTool::isMatch : could not retrieve xAOD::EventInfo.");
  }

  // Evaluate our network and compare against our TVA cut (">= cut" := associated)
  mvaOutput = this->evaluateNetwork(trk, vx, *evt);
  return (mvaOutput >= m_cut);
}

template <typename T, typename V>
xAOD::TrackVertexAssociationMap MVATrackVertexAssociationTool::getMatchMapInternal(const T& trk_list, const V& vx_list) const {

  xAOD::TrackVertexAssociationMap trktovxmap;

  for (const auto& vertex : vx_list) {
    xAOD::TrackVertexAssociationList trktovxlist;
    trktovxlist.clear();
    for (const auto& track : trk_list) {
      if (isCompatible(*track, *vertex)) {
        trktovxlist.push_back(track);
      }
    }
    trktovxmap[vertex] = trktovxlist;
  }

  return trktovxmap;
}

template <typename T>
const xAOD::Vertex* MVATrackVertexAssociationTool::getUniqueMatchVertexInternal(const xAOD::TrackParticle& trk, const T& vx_list) const {

  bool match;
  float mvaOutput;
  float maxValue = -1.0; // MVA output ranges between 0 and 1
  const xAOD::Vertex* bestMatchVertex = nullptr;

  for (const auto& vertex : vx_list) {
    match = isMatch(trk, *vertex, mvaOutput);
    if (match && (maxValue < mvaOutput)) {
      maxValue = mvaOutput;
      bestMatchVertex = vertex;
    }
  }

  // check if get the matched Vertex, for the tracks not used in vertex fit
  if (!bestMatchVertex) {
    ATH_MSG_DEBUG("Could not find any matched vertex for this track.");
  }

  return bestMatchVertex;
}

template <typename T, typename V>
xAOD::TrackVertexAssociationMap MVATrackVertexAssociationTool::getUniqueMatchMapInternal(const T& trk_list, const V& vx_list) const {

  xAOD::TrackVertexAssociationMap trktovxmap;

  // Initialize map
  for (const auto& vertex : vx_list) {
    xAOD::TrackVertexAssociationList trktovxlist;
    trktovxlist.clear();
    trktovxmap[vertex] = trktovxlist;
  }

  // Perform matching
  for (const auto& track : trk_list) {
    const xAOD::Vertex* vx_match = getUniqueMatchVertexInternal(*track, vx_list);
    if (vx_match) {
      // Found matched vertex
      trktovxmap[vx_match].push_back(track);
    }
  }

  return trktovxmap;
}

bool MVATrackVertexAssociationTool::initializeNetwork() {

  // Load our input evaluator
  if (m_inputNames.size() != m_inputTypes.size()) {
    throw std::runtime_error("ERROR in CP::MVATrackVertexAssociationTool::initializeNetwork : size of input variable names (" + std::to_string(m_inputNames.size()) + ") does not equal size of input variable types (" + std::to_string(m_inputTypes.size()) + ").");
  }
  m_inputMap.clear();
  for (size_t i = 0; i < m_inputNames.size(); i++) {
    m_inputMap[m_inputNames[i]] = static_cast<MVAInputEvaluator::Input>(m_inputTypes[i]);
  }
  m_inputEval.load(m_inputMap);

  // Load our input file
  std::string fileName; 
  if (m_usePathResolver) {
    fileName = PathResolverFindCalibFile(m_fileName);
    if (fileName.empty()) {
      throw std::runtime_error("ERROR in CP::MVATrackVertexAssociationTool::initializeNetwork : could not find input network file: " + m_fileName);
    }
  }
  else {
    fileName = m_fileName;
  }
  std::ifstream netFile(fileName);
  if (!netFile) {
    throw std::runtime_error("ERROR in CP::MVATrackVertexAssociationTool::initializeNetwork : could not properly open input network file: " + fileName);
  }

  // For sequential:
  if (m_isSequential) {
    lwt::JSONConfig netDef = lwt::parse_json(netFile);
    m_network = new lwt::LightweightNeuralNetwork(netDef.inputs, netDef.layers, netDef.outputs);
  }
  // For functional:
  else {
    lwt::GraphConfig netDef = lwt::parse_json_graph(netFile);
    if (netDef.inputs.size() != 1) {
      throw std::runtime_error("ERROR in CP::MVATrackVertexAssociationTool::initializeNetwork : network in file \"" + fileName + "\" has more than 1 input node: # of input nodes = " + std::to_string(netDef.inputs.size()));
    }
    m_inputNodeName = netDef.inputs[0].name;
    m_graph = new lwt::LightweightGraph(netDef);
  }

  return true;
}

float MVATrackVertexAssociationTool::evaluateNetwork(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt) const {

  // Evaluate our inputs
  std::map<std::string, double> input;
  m_inputEval.eval(trk, vx, evt, input);

  // Evaluate our network
  std::map<std::string, double> output;
  // For sequential:
  if (m_isSequential) {
    output = m_network->compute(input);
  }
  // For functional:
  else {
    std::map<std::string, std::map<std::string, double>> wrappedInput;
    wrappedInput[m_inputNodeName] = input;
    output = m_graph->compute(wrappedInput);
  }

  return output[m_outputName];
}

bool MVATrackVertexAssociationTool::finalizeNetwork() {

  // Clean-up
  if (m_network) {
    delete m_network;
    m_network = nullptr;
  }
  if (m_graph) {
    delete m_graph;
    m_graph = nullptr;
  }

  return true;
}

} // namespace CP
