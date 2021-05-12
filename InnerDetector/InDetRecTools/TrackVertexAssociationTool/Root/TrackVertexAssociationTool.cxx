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

TrackVertexAssociationTool::TrackVertexAssociationTool(const std::string& name) :
  AsgTool(name),
  m_wp("Nominal"),
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
  ATH_MSG_INFO(" Initializing TrackVertexAssociationTool");

  if ( m_wp == "Electron" ) {
    m_d0_cut = -1;
    m_use_d0sig = true;
    m_d0sig_cut = 5.0;
    m_dzSinTheta_cut = 0.5;
    m_doUsedInFit = false;
    m_requirePriVtx = false;
  } else if ( m_wp == "Muon" ) {
    m_d0_cut = -1;
    m_use_d0sig = true;
    m_d0sig_cut = 3.0;
    m_dzSinTheta_cut = 0.5;
    m_doUsedInFit = false;
    m_requirePriVtx = false;
  } else if ( m_wp == "Loose" ) {
    m_d0_cut = -1;
    m_use_d0sig = false;
    m_d0sig_cut = -1;
    m_dzSinTheta_cut = 3.;
    m_doUsedInFit = true;
    m_requirePriVtx = true;
  } else if ( m_wp == "Nominal" ) {
    m_d0_cut = 2.;
    m_use_d0sig = false;
    m_d0sig_cut = -1;
    m_dzSinTheta_cut = 3.;
    m_doUsedInFit = false;
    m_requirePriVtx = false;
  } else if ( m_wp == "Tight" ) {
    m_d0_cut = 0.5;
    m_use_d0sig = false;
    m_d0sig_cut = -1;
    m_dzSinTheta_cut = 0.5;
    m_doUsedInFit = false;
    m_requirePriVtx = false;
  } else if ( m_wp == "Custom" ) {
    // nothing to do here
  } else {
    ATH_MSG_ERROR("Invalid TVA working point '" << m_wp << "' - for a custom configuration, please provide 'Custom' for the 'WorkingPoint' property");
    return StatusCode::FAILURE;
  }

  if ( m_wp == "Custom" ) {
    ATH_MSG_INFO("TVA working point 'Custom' provided - tool properties are initialized to default values unless explicitly set by the user");
  } else {
    ATH_MSG_INFO("TVA working point '" << m_wp << "' provided - tool properties have been configured accordingly");
  }

  if(m_use_d0sig){
    ATH_MSG_INFO("Cut on d0 significance: " << m_d0sig_cut << "\t(d0sig_cut)");
  } else {
    ATH_MSG_INFO("Cut on d0: " << m_d0_cut << "\t(d0_cut)");
  }
  ATH_MSG_INFO("Cut on Δz * sin θ: " << m_dzSinTheta_cut << "\t(dzSinTheta_cut)");

  ATH_MSG_INFO("Allow UsedInFit MatchStatus: " << m_doUsedInFit << "\t(doUsedInFit)");
  ATH_MSG_INFO("Require VxType::PriVtx for unique match: " << m_requirePriVtx << "\t(requirePriVtx)");

  return StatusCode::SUCCESS;
}

bool TrackVertexAssociationTool::isCompatible(
    const xAOD::TrackParticle &trk, const xAOD::Vertex &vx) const
{
  float dzSinTheta = 0.;
  bool status = false;

  MatchStatus matchstatus = isMatch(trk, vx, dzSinTheta);

  if(matchstatus == TrackVertexAssociationTool::UsedInFit || matchstatus == TrackVertexAssociationTool::Matched ) {
    status = true;
  }
  else {
    status = false;
  }
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

TrackVertexAssociationTool::MatchStatus TrackVertexAssociationTool::isMatch(const xAOD::TrackParticle &trk,
                                                                            const xAOD::Vertex &vx,
                                                                            float &dzSinTheta) const
{
  // ATH_MSG_DEBUG("<###### Enter: isMatch() function ######>");

  if (vx.vertexType() == xAOD::VxType::NoVtx) {
    // ATH_MSG_DEBUG(
    //     "The Vertex is a fake one, will not do track-vertex association");
    return UnMatch;
  }

  float vx_z0 = vx.z();
  float trk_z0 = trk.z0();
  float beamspot_z0 = trk.vz();
  float theta = trk.theta();
  // calculate Δz * sin θ
  dzSinTheta = fabs((trk_z0 - vx_z0 + beamspot_z0) * sin(theta));

  // If vertex fit information is flagged to be used,
  if(m_doUsedInFit) {
    if(trk.vertex()==&vx) { // check whether the track is used for the given vertex fit
      ATH_MSG_DEBUG("This track is used to fit the vertex");
      return UsedInFit;
    } else if (trk.vertex()!=0) { // otherwise, automatically return UnMatch if it was used in another vertex fit
      return UnMatch;
    }
  }

  // Now use cuts to determine a match
  // Only arrive here if:
  // 1. vertex fit info was flagged to be used but track wasn't used in any vertex fit
  // 2. vertex fit info wasn't flagged to be used

  const xAOD::EventInfo *evt{0};
  if (evtStore()->retrieve(evt, "EventInfo").isFailure()) {
    throw std::runtime_error("Could not retrieve EventInfo");
  }

  if (m_use_d0sig) {

    double d0sig = xAOD::TrackingHelpers::d0significance(
        &trk, evt->beamPosSigmaX(), evt->beamPosSigmaY(), evt->beamPosSigmaXY());
    // d0 significance cut
    if (m_d0sig_cut >= 0 && fabs(d0sig) > m_d0sig_cut)
      return UnMatch;

  } else {

    float trk_d0=trk.d0();
    // d0 cut
    if (m_d0_cut >= 0 && fabs(trk_d0) > m_d0_cut)
      return UnMatch;

  }

  if (m_dzSinTheta_cut >= 0 && dzSinTheta > m_dzSinTheta_cut)
    return UnMatch;

  return Matched;
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
  float min_dz = m_dzSinTheta_cut;
  const xAOD::Vertex *bestMatchVertex{0};

  for (auto *vertex : vx_list) {
    float dzSinTheta = 0.;
    MatchStatus matchstatus = isMatch(trk, *vertex, dzSinTheta);
    if(matchstatus == TrackVertexAssociationTool::UsedInFit) {
      return vertex;
    }
    else {
      if ((m_requirePriVtx && vertex->vertexType()==xAOD::VxType::PriVtx) || (!m_requirePriVtx)) {
        if (matchstatus == TrackVertexAssociationTool::Matched) {
          if (dzSinTheta < min_dz) {
            min_dz = dzSinTheta;
            bestMatchVertex = vertex;
          }
        }
      }
    }
  }

  // check if get the matched Vertex, for the tracks not used in vertex fit
  if (!bestMatchVertex) {
    ATH_MSG_DEBUG("Could not find any matched vertex for this track");
  }

  return bestMatchVertex;
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
