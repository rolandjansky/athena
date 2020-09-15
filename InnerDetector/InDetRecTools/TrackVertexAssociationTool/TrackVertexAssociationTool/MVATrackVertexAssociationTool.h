/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVATRACKVERTEXASSOCIATIONTOOL_H
#define MVATRACKVERTEXASSOCIATIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "AthLinks/ElementLink.h"
#include "AsgDataHandles/ReadHandleKey.h"

#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/MVAInputEvaluator.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

#include <map>
#include <string>
#include <vector>

// Forward declarations
namespace lwt {
  class LightweightNeuralNetwork;
  class LightweightGraph;
}

namespace CP {

class MVATrackVertexAssociationTool : public asg::AsgTool, virtual public ITrackVertexAssociationTool
{
  ASG_TOOL_CLASS(MVATrackVertexAssociationTool, ITrackVertexAssociationTool)
public:

  MVATrackVertexAssociationTool(const std::string& name);

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual bool isCompatible(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx) const override;

  virtual xAOD::TrackVertexAssociationMap getMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const override;
  virtual xAOD::TrackVertexAssociationMap getMatchMap(const xAOD::TrackParticleContainer& trk_cont, const xAOD::VertexContainer& vx_cont) const override;

  virtual ElementLink<xAOD::VertexContainer> getUniqueMatchVertexLink(const xAOD::TrackParticle& trk, const xAOD::VertexContainer& vx_cont) const override;
  virtual const xAOD::Vertex* getUniqueMatchVertex(const xAOD::TrackParticle& trk, std::vector<const xAOD::Vertex*>& vx_list) const override;

  virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const override;
  virtual xAOD::TrackVertexAssociationMap getUniqueMatchMap(const xAOD::TrackParticleContainer& trk_cont, const xAOD::VertexContainer& vx_cont) const override;

private:

  bool isMatch(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, float& mvaOutput) const;

  template <typename T, typename V>
  xAOD::TrackVertexAssociationMap getMatchMapInternal(const T& trk_list, const V& vx_list) const;

  template <typename T, typename V>
  xAOD::TrackVertexAssociationMap getUniqueMatchMapInternal(const T& trk_list, const V& vx_list) const;

  template <typename T>
  const xAOD::Vertex* getUniqueMatchVertexInternal(const xAOD::TrackParticle& trk, const T& vx_list) const;

  bool initializeNetwork();
  float evaluateNetwork(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt) const;
  bool finalizeNetwork();

  // Input lwtnn network file
  std::string m_fileName;

  // Vector of input variable names
  std::vector<std::string> m_inputNames;

  // Vector of input variable types
  std::vector<int> m_inputTypes;

  // Name of the output node to cut on
  std::string m_outputName;

  // Is the network sequential or functional
  bool m_isSequential;

  // TVA working point
  std::string m_wp;

  // TVA cut value on the output discriminant
  float m_cut;

  // Use the PathResolver to find our input file
  bool m_usePathResolver;

  // Input variable name/type map
  MVAInputEvaluator::InputSelectionMap m_inputMap;

  // Input variable evaluator
  MVAInputEvaluator m_inputEval;

  // EventInfo key
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo {this, "EventInfo", "EventInfo", "EventInfo key"};

  // Name of the input node (for functional modes)
  std::string m_inputNodeName = ""; //!

  // Network as implemented using lwtnn
  lwt::LightweightNeuralNetwork* m_network = nullptr; //!
  lwt::LightweightGraph*         m_graph   = nullptr; //!

};

} // namespace CP

#endif // MVATRACKVERTEXASSOCIATIONTOOL_H
