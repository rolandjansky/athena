/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackVertexAssociationTool/BaseTrackVertexAssociationTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

using namespace std;

namespace CP {

BaseTrackVertexAssociationTool::BaseTrackVertexAssociationTool(std::string name)
    : AsgTool(name)
{
  declareProperty("d0sig_cut", m_d0sig_cut = -1);
  declareProperty("dzSinTheta_cut", m_dzSinTheta_cut = -1);
}

StatusCode BaseTrackVertexAssociationTool::initialize()
{
  ATH_MSG_INFO("Cut on d0 significance: " << m_d0sig_cut << "\t(d0sig_cut)");
  ATH_MSG_INFO("Cut on Δz * sin θ: " << m_dzSinTheta_cut << "\t(dzSinTheta_cut)");

  ATH_MSG_WARNING("BaseTrackVertexAssociationTool is being depricated. Please use the new TrackVertexAssociationTool instead.");

  return StatusCode::SUCCESS;
}

void BaseTrackVertexAssociationTool::print() const {}

bool BaseTrackVertexAssociationTool::isCompatible(
    const xAOD::TrackParticle &trk, const xAOD::Vertex &vx) const
{
  float dzSinTheta = 0.;
  bool status = false;

  status = isMatch(trk, vx, dzSinTheta);

  return status;
}

xAOD::TrackVertexAssociationMap BaseTrackVertexAssociationTool::getMatchMap(
    std::vector<const xAOD::TrackParticle *> &trk_list,
    std::vector<const xAOD::Vertex *> &vx_list) const
{
  return getMatchMapImpl(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap BaseTrackVertexAssociationTool::getMatchMap(
    const xAOD::TrackParticleContainer &trkCont,
    const xAOD::VertexContainer &vxCont) const
{
  return getMatchMapImpl(trkCont, vxCont);
}

const xAOD::Vertex *BaseTrackVertexAssociationTool::getUniqueMatchVertex(
    const xAOD::TrackParticle &trk,
    std::vector<const xAOD::Vertex *> &vx_list) const
{
  return getUniqueMatchVertexImpl(trk, vx_list);
}

ElementLink<xAOD::VertexContainer>
BaseTrackVertexAssociationTool::getUniqueMatchVertexLink(
    const xAOD::TrackParticle &trk, const xAOD::VertexContainer &vxCont) const
{
  ElementLink<xAOD::VertexContainer> vx_link_tmp;

  const xAOD::Vertex *vx_tmp = getUniqueMatchVertexImpl(trk, vxCont);
  if (vx_tmp) {
    vx_link_tmp.toContainedElement(vxCont, vx_tmp);
  }
  return vx_link_tmp;
}

xAOD::TrackVertexAssociationMap
BaseTrackVertexAssociationTool::getUniqueMatchMap(
    std::vector<const xAOD::TrackParticle *> &trk_list,
    std::vector<const xAOD::Vertex *> &vx_list) const
{
  return getUniqueMatchMapImpl(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap
BaseTrackVertexAssociationTool::getUniqueMatchMap(
    const xAOD::TrackParticleContainer &trkCont,
    const xAOD::VertexContainer &vxCont) const
{
  return getUniqueMatchMapImpl(trkCont, vxCont);
}

// private methods

bool BaseTrackVertexAssociationTool::isMatch(const xAOD::TrackParticle &trk,
                                             const xAOD::Vertex &vx,
                                             float &dzSinTheta) const
{
  // ATH_MSG_DEBUG("<###### Enter: isMatch() function ######>");

  if (vx.vertexType() == xAOD::VxType::NoVtx) {
    // ATH_MSG_DEBUG(
    //     "The Vertex is a fake one, will not do track-vertex association");
    return false;
  }

  const xAOD::EventInfo *evt{0};
  if (evtStore()->retrieve(evt, "EventInfo").isFailure()) {
    throw std::runtime_error("Could not retrieve EventInfo");
  }

  float vx_z0 = vx.z();
  float trk_z0 = trk.z0();
  float beamspot_z0 = trk.vz();
  float theta = trk.theta();
  double d0sig = xAOD::TrackingHelpers::d0significance(
      &trk, evt->beamPosSigmaX(), evt->beamPosSigmaY(), evt->beamPosSigmaXY());

  // d0 significance cut
  if (m_d0sig_cut >= 0 && fabs(d0sig) > m_d0sig_cut)
    return false;

  // calculate Δz * sin θ
  dzSinTheta = fabs((trk_z0 - vx_z0 + beamspot_z0) * sin(theta));
  if (m_dzSinTheta_cut >= 0 && dzSinTheta > m_dzSinTheta_cut)
    return false;

  return true;
}

template <typename U, typename V>
xAOD::TrackVertexAssociationMap
BaseTrackVertexAssociationTool::getMatchMapImpl(U &trk_list, V &vx_list) const
{
  xAOD::TrackVertexAssociationMap trktovxmap;

  for (auto *vertex : vx_list) {
    xAOD::TrackVertexAssociationList trktovxlist;
    trktovxlist.clear();
    for (auto *track : trk_list) {
      if (isCompatible(*track, *vertex)) {
        trktovxlist.push_back(track);
      }
    }
    trktovxmap[vertex] = trktovxlist;
  }

  return trktovxmap;
}

template <typename T>
const xAOD::Vertex *BaseTrackVertexAssociationTool::getUniqueMatchVertexImpl(
    const xAOD::TrackParticle &trk, T &vx_list) const
{
  float mini_dz = m_dzSinTheta_cut;
  const xAOD::Vertex *mini_vertex{0};

  for (auto *vertex : vx_list) {
    float dzSinTheta = 0.;

    bool match_status = isMatch(trk, *vertex, dzSinTheta);
    if (match_status) {
      if (dzSinTheta < mini_dz) {
        mini_dz = dzSinTheta;
        mini_vertex = vertex;
      }
    }
  }

  // check if get the matched Vertex, for the tracks not used in vertex fit
  if (!mini_vertex) {
    ATH_MSG_DEBUG("Could not find any matched vertex for this track");
  }

  return mini_vertex;
}

template <typename T, typename U>
xAOD::TrackVertexAssociationMap
BaseTrackVertexAssociationTool::getUniqueMatchMapImpl(T &trk_list,
                                                      U &vx_list) const
{
  xAOD::TrackVertexAssociationMap trktovxmap;

  // initialize map
  for (auto *vertex : vx_list) {
    xAOD::TrackVertexAssociationList trktovxlist;
    trktovxlist.clear();

    trktovxmap[vertex] = trktovxlist;
  }

  for (auto *track : trk_list) {
    const xAOD::Vertex *vx_match = getUniqueMatchVertexImpl(*track, vx_list);
    if (vx_match) {
      // can find matched vertex
      trktovxmap[vx_match].push_back(track);
    }
  }

  return trktovxmap;
}

} // namespace CP

// vim: expandtab tabstop=8 shiftwidth=2 softtabstop=2
