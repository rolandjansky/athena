/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

using namespace std;

namespace CP {

TrackVertexAssociationTool::TrackVertexAssociationTool(std::string name) :
  AsgTool(name),
  m_wp(""),
  m_d0_cut(-1),
  m_use_d0sig(false),
  m_d0sig_cut(-1),
  m_dzSinTheta_cut(-1),
  m_doUsedInFit(false),
  m_requirePriVtx(false)
{
  declareProperty("WorkingPoint", m_wp);
  declareProperty("d0_cut", m_d0_cut);
  declareProperty("use_d0sig", m_use_d0sig);
  declareProperty("d0sig_cut", m_d0sig_cut);
  declareProperty("dzSinTheta_cut", m_dzSinTheta_cut);
  declareProperty("doUsedInFit", m_doUsedInFit);
  declareProperty("requirePriVtx", m_requirePriVtx);
}

StatusCode TrackVertexAssociationTool::initialize()
{
  ATH_MSG_INFO("Cut on d0 significance: " << m_d0sig_cut << "\t(d0sig_cut)");
  ATH_MSG_INFO("Cut on Δz * sin θ: " << m_dzSinTheta_cut << "\t(dzSinTheta_cut)");

  return StatusCode::SUCCESS;
}

void TrackVertexAssociationTool::print() const {}

bool TrackVertexAssociationTool::isCompatible(
    const xAOD::TrackParticle &trk, const xAOD::Vertex &vx) const
{
  float dzSinTheta = 0.;
  bool status = false;

  status = isMatch(trk, vx, dzSinTheta);

  return status;
}

xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getMatchMap(
    std::vector<const xAOD::TrackParticle *> &trk_list,
    std::vector<const xAOD::Vertex *> &vx_list) const
{
  return getMatchMapInternal(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getMatchMap(
    const xAOD::TrackParticleContainer &trkCont,
    const xAOD::VertexContainer &vxCont) const
{
  return getMatchMapInternal(trkCont, vxCont);
}

const xAOD::Vertex *TrackVertexAssociationTool::getUniqueMatchVertex(
    const xAOD::TrackParticle &trk,
    std::vector<const xAOD::Vertex *> &vx_list) const
{
  return getUniqueMatchVertexInternal(trk, vx_list);
}

ElementLink<xAOD::VertexContainer>
TrackVertexAssociationTool::getUniqueMatchVertexLink(
    const xAOD::TrackParticle &trk, const xAOD::VertexContainer &vxCont) const
{
  ElementLink<xAOD::VertexContainer> vx_link_tmp;

  const xAOD::Vertex *vx_tmp = getUniqueMatchVertexInternal(trk, vxCont);
  if (vx_tmp) {
    vx_link_tmp.toContainedElement(vxCont, vx_tmp);
  }
  return vx_link_tmp;
}

xAOD::TrackVertexAssociationMap
TrackVertexAssociationTool::getUniqueMatchMap(
    std::vector<const xAOD::TrackParticle *> &trk_list,
    std::vector<const xAOD::Vertex *> &vx_list) const
{
  return getUniqueMatchMapInternal(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap
TrackVertexAssociationTool::getUniqueMatchMap(
    const xAOD::TrackParticleContainer &trkCont,
    const xAOD::VertexContainer &vxCont) const
{
  return getUniqueMatchMapInternal(trkCont, vxCont);
}

// private methods

bool TrackVertexAssociationTool::isMatch(const xAOD::TrackParticle &trk,
                                         const xAOD::Vertex &vx,
                                         float &dzSinTheta) const
{
  // ATH_MSG_DEBUG("<###### Enter: isMatch() function ######>");

  if (&vx == NULL) {
    // this should never happen because vx is a reference
    ATH_MSG_DEBUG("Invalid Vertex pointer, return false");
    return false;
  }

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
TrackVertexAssociationTool::getMatchMapInternal(U &trk_list, V &vx_list) const
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
const xAOD::Vertex *TrackVertexAssociationTool::getUniqueMatchVertexInternal(
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
TrackVertexAssociationTool::getUniqueMatchMapInternal(T &trk_list,
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
    const xAOD::Vertex *vx_match = getUniqueMatchVertexInternal(*track, vx_list);
    if (vx_match) {
      // can find matched vertex
      trktovxmap[vx_match].push_back(track);
    }
  }

  return trktovxmap;
}

} // namespace CP
