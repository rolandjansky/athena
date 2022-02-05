/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Includes from this package
#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"

// FrameWork includes
#include "AsgDataHandles/ReadHandle.h"

// EDM includes
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackingPrimitives.h"

// ROOT includes
#include "TString.h"

// STL includes
#include <cmath>
#include <map>
#include <stdexcept>
#include <utility>

// Hidden namespace for functors
namespace {

  // Let's access the public helper typedefs/class in the TTVA tool
  using AMVFVerticesAcc = CP::TrackVertexAssociationTool::AMVFVerticesAcc;
  using AMVFWeightsAcc = CP::TrackVertexAssociationTool::AMVFWeightsAcc;
  using WorkingPoint = CP::TrackVertexAssociationTool::WorkingPoint;

  // ------------------------------------------------------------------------------------------------------------ //
  // Return the AMVF fit weight for track assuming a given vertex as well as the largest weight seen by the track //
  // ------------------------------------------------------------------------------------------------------------ //

  typedef std::pair<float, float> FitWeight; // (fit weight for a given vertex, max fit weight)

  FitWeight fitWeight(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const AMVFVerticesAcc& vtxDeco, const AMVFWeightsAcc& wgtDeco) {

    // Get our AMVF vertices/weights decorations
    const std::vector<ElementLink<xAOD::VertexContainer>> AMVFVertices = vtxDeco(*trk);
    const std::vector<float> AMVFWeights = wgtDeco(*trk);

    // Determine if the vertex matches any of the vertices the track is used in the fit of
    int leading = -1;
    for (std::size_t i = 0; i < AMVFVertices.size(); i++) {
      if (!AMVFVertices.at(i).isValid()) continue; // Skip non-valid vertex links
      if (leading < 0) leading = i; // Set the leading (i.e., highest weight) vertex equal to the first valid vertex
      if (vtx == *(AMVFVertices.at(i))) return ::FitWeight(AMVFWeights.at(i), AMVFWeights.at(leading)); // Safe to return leading element, wouldn't get here w/o >leading elements
    }

    // Default return - vertex doesn't correspond to ANY vertices the track is used in the fit of
    return ::FitWeight(-1., (leading < 0) ? -1. : AMVFWeights.at(leading));
  }

  inline float absD0Sig(const xAOD::TrackParticle* trk, const xAOD::EventInfo* evt) {
    return std::abs(xAOD::TrackingHelpers::d0significance(trk, evt->beamPosSigmaX(), evt->beamPosSigmaY(), evt->beamPosSigmaXY()));
  }

  inline float absD0(const xAOD::TrackParticle* trk) {
    return std::abs(trk->d0());
  }

  inline float absDzSinTheta(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx) {
    return std::abs((trk->z0() + trk->vz() - vtx->z()) * std::sin(trk->theta()));
  }

  // -------------------------------------- //
  // Define all of the working points below //
  // -------------------------------------- //

  class Old_Loose
    : public WorkingPoint
  {
  public:
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo*,
      const AMVFVerticesAcc& vtxDeco, const AMVFWeightsAcc& wgtDeco) const
    {
      FitWeight weight = fitWeight(trk, vtx, vtxDeco, wgtDeco);
      return ((weight.second > 0.) ? (weight.first > 0.) : (absDzSinTheta(trk, vtx) < 3.0));
    };
  };

  class Old_Nominal
    : public WorkingPoint
  {
  public:
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo*,
      const AMVFVerticesAcc&, const AMVFWeightsAcc&) const
    {
      return (absD0(trk) < 2.0 && absDzSinTheta(trk, vtx) < 3.0);
    };
  };

  class Old_Tight
    : public WorkingPoint
  {
  public:
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo*,
      const AMVFVerticesAcc&, const AMVFWeightsAcc&) const
    {
      return (absD0(trk) < 0.5 && absDzSinTheta(trk, vtx) < 0.5);
    };
  };

  class Old_Electron
    : public WorkingPoint
  {
  public:
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo* evt,
      const AMVFVerticesAcc&, const AMVFWeightsAcc&) const
    {
      return (absD0Sig(trk, evt) < 5.0 && absDzSinTheta(trk, vtx) < 0.5);
    };
  };

  class Old_Muon
    : public WorkingPoint
  {
  public:
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo* evt,
      const AMVFVerticesAcc&, const AMVFWeightsAcc&) const
    {
      return (absD0Sig(trk, evt) < 3.0 && absDzSinTheta(trk, vtx) < 0.5);
    };
  };

  class Prompt_D0Sig
    : public WorkingPoint
  {
  public:
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo*,
      const AMVFVerticesAcc& vtxDeco, const AMVFWeightsAcc& wgtDeco) const
    {
      return (fitWeight(trk, vtx, vtxDeco, wgtDeco).first > 0.03);
    }
  };

  #define NONPROMPT_D0SIG(CLASSNAME, RADIAL_CUT)                                                                                                                        \
  class CLASSNAME                                                                                                                                                       \
    : public WorkingPoint                                                                                                                                               \
  {                                                                                                                                                                     \
  public:                                                                                                                                                               \
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo* evt,                                                             \
      const AMVFVerticesAcc& vtxDeco, const AMVFWeightsAcc& wgtDeco) const                                                                                              \
    {                                                                                                                                                                   \
      return (absD0(trk) < RADIAL_CUT && ((absD0Sig(trk, evt) < 3.0) ? (fitWeight(trk, vtx, vtxDeco, wgtDeco).first > 0.03) : (absDzSinTheta(trk, vtx) < RADIAL_CUT))); \
    }                                                                                                                                                                   \
  };                                                                                                                                                                    \

  NONPROMPT_D0SIG(Nonprompt_Hard_D0Sig,   1.0)
  NONPROMPT_D0SIG(Nonprompt_Medium_D0Sig, 2.0)
  NONPROMPT_D0SIG(Nonprompt_All_D0Sig,    5.0)

  class Prompt_MaxWeight
    : public WorkingPoint
  {
  public:
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo*,
      const AMVFVerticesAcc& vtxDeco, const AMVFWeightsAcc& wgtDeco) const
    {
      FitWeight weight = fitWeight(trk, vtx, vtxDeco, wgtDeco);
      return (weight.first > 0.03 && weight.first >= weight.second);
    }
  };

  #define NONPROMPT_MAXWEIGHT(CLASSNAME, RADIAL_CUT)                                                                                        \
  class CLASSNAME                                                                                                                           \
    : public WorkingPoint                                                                                                                   \
  {                                                                                                                                         \
  public:                                                                                                                                   \
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo*,                                     \
      const AMVFVerticesAcc& vtxDeco, const AMVFWeightsAcc& wgtDeco) const                                                                  \
    {                                                                                                                                       \
      FitWeight weight = fitWeight(trk, vtx, vtxDeco, wgtDeco);                                                                             \
      return ((weight.second > 0.) ? (weight.first >= weight.second) : (absD0(trk) < RADIAL_CUT && absDzSinTheta(trk, vtx) < RADIAL_CUT));  \
    }                                                                                                                                       \
  };                                                                                                                                        \

  NONPROMPT_MAXWEIGHT(Nonprompt_Hard_MaxWeight,   1.0)
  NONPROMPT_MAXWEIGHT(Nonprompt_Medium_MaxWeight, 2.0)
  NONPROMPT_MAXWEIGHT(Nonprompt_All_MaxWeight,    5.0)

  // Backwards compatability for Custom WP
  class Custom
    : public WorkingPoint
  {
  public:
    Custom(const float d0_cut, const bool use_d0sig, const float d0sig_cut, const float dzSinTheta_cut, const bool doUsedInFit) {
      m_d0_cut         = d0_cut;
      m_use_d0sig      = use_d0sig;
      m_d0sig_cut      = d0sig_cut;
      m_dzSinTheta_cut = dzSinTheta_cut;
      m_doUsedInFit    = doUsedInFit;
    }
    virtual bool apply(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx, const xAOD::EventInfo* evt,
      const AMVFVerticesAcc& vtxDeco, const AMVFWeightsAcc& wgtDeco) const
    {
      // If vertex fit information is flagged to be used
      if (m_doUsedInFit) {
        FitWeight weight = fitWeight(trk, vtx, vtxDeco, wgtDeco);
        if (weight.first > 0.) {
          return true;
        }
        else if (weight.second > 0.) {
          return false;
        }
      }
      // Now use cuts to determine a match
      // Only arrive here if:
      // 1. vertex fit info was flagged to be used but track wasn't used in any vertex fit
      // 2. vertex fit info wasn't flagged to be used
      if (m_use_d0sig) {
        // d0 significance cut
        if (m_d0sig_cut >= 0 && absD0Sig(trk, evt) > m_d0sig_cut) return false;
      }
      else {
        // d0 cut
        if (m_d0_cut >= 0 && absD0(trk) > m_d0_cut) return false;
      }
      return !(m_dzSinTheta_cut >= 0 && absDzSinTheta(trk, vtx) > m_dzSinTheta_cut);
    }
  private:
    float m_d0_cut;
    bool  m_use_d0sig;
    float m_d0sig_cut;
    float m_dzSinTheta_cut;
    bool  m_doUsedInFit;
  };

} // end: namespace

namespace CP {

TrackVertexAssociationTool::TrackVertexAssociationTool(const std::string& name) :
  AsgTool(name),
  m_wp("Old_Nominal"),
  m_d0_cut(-1),
  m_use_d0sig(false),
  m_d0sig_cut(-1),
  m_dzSinTheta_cut(-1),
  m_doUsedInFit(false),
  m_doPVPriority(false),
  m_requirePriVtx(false),
  m_hardScatterDeco("hardScatterVertexLink"),
  m_trkContName("InDetTrackParticles")
{
  declareProperty("WorkingPoint",        m_wp,              "Working point to operate on.");
  declareProperty("d0_cut",              m_d0_cut,          "Cut on d0. Not applied if set to -1.");
  declareProperty("use_d0sig",           m_use_d0sig,       "Flag to cut on d0sig instead of d0.");
  declareProperty("d0sig_cut",           m_d0sig_cut,       "Cut on d0Sig. Not applied if set to -1.");
  declareProperty("dzSinTheta_cut",      m_dzSinTheta_cut,  "Cut on |dz*sinTheta| (in mm). Not applied if set to -1.");
  declareProperty("doUsedInFit",         m_doUsedInFit,     "Control whether to allow for a MatchStatus of UsedInFit.");
  declareProperty("doPVPriority",        m_doPVPriority,    "Control whether to give priority to matching to PV instead of closest vertex.");
  declareProperty("requirePriVtx",       m_requirePriVtx,   "Control whether a vertex must be VxType::PriVtx in order for a track (not UsedInFit) to be uniquely matched to it.");
  declareProperty("HardScatterLinkDeco", m_hardScatterDeco, "The decoration name of the ElementLink to the hardscatter vertex (found on xAOD::EventInfo)");
  declareProperty("TrackContName",       m_trkContName,     "The name of the xAOD::TrackParticleContainer to access the AMVF vertices+weights for (not actually read).");
  declareProperty("AMVFVerticesDeco",    m_vtxDecoKey = "TTVA_AMVFVertices",     "The per-track decoration name of the vector of AMVF used-in-fit vertex ElementLinks.");
  declareProperty("AMVFWeightsDeco",     m_wgtDecoKey = "TTVA_AMVFWeights",     "The per-track decoration name of the vector of AMVF used-in-fit annealing weights.");
}

#define IF_WORKING_POINT(WORKING_POINT, DO_USED_IN_FIT, REQUIRE_PRI_VTX)                               \
if (m_wp == #WORKING_POINT) {                                                                          \
  m_applicator = std::unique_ptr<CP::TrackVertexAssociationTool::WorkingPoint>(new ::WORKING_POINT()); \
  m_doUsedInFit = DO_USED_IN_FIT;                                                                      \
  m_requirePriVtx = REQUIRE_PRI_VTX;                                                                   \
}                                                                                                      \

StatusCode TrackVertexAssociationTool::initialize()
{
  ATH_MSG_INFO("Initializing TrackVertexAssociationTool.");

  std::vector<std::string> run_2_wps = {"Loose", "Nominal", "Tight", "Electron", "Muon", "Old_Loose", "Old_Nominal", "Old_Tight", "Old_Electron", "Old_Muon"};
  if (std::find(run_2_wps.begin(), run_2_wps.end(), m_wp) != run_2_wps.end()) {
    std::string prefix = "Old_";
    if (m_wp.compare(0, prefix.size(), prefix) == 0) {
      ATH_MSG_WARNING("WorkingPoint '" << m_wp << "' corresponds to a Run 2 working point and is not recommended.");
    }
    else {
      ATH_MSG_WARNING("WorkingPoint '" << m_wp << "' corresponds to a Run 2 working point and is not recommended - remapping to 'Old_" << m_wp << "' (same definition, however).");
      m_wp.insert(0, "Old_");
    }
  }

  IF_WORKING_POINT(Old_Loose, true, true)
  else IF_WORKING_POINT(Old_Nominal, false, false)
  else IF_WORKING_POINT(Old_Tight, false, false)
  else IF_WORKING_POINT(Old_Electron, false, false)
  else IF_WORKING_POINT(Old_Muon, false, false)
  else IF_WORKING_POINT(Prompt_D0Sig, true, false)
  else IF_WORKING_POINT(Nonprompt_Hard_D0Sig, true, false)
  else IF_WORKING_POINT(Nonprompt_Medium_D0Sig, true, false)
  else IF_WORKING_POINT(Nonprompt_All_D0Sig, true, false)
  else IF_WORKING_POINT(Prompt_MaxWeight, true, false)
  else IF_WORKING_POINT(Nonprompt_Hard_MaxWeight, true, false)
  else IF_WORKING_POINT(Nonprompt_Medium_MaxWeight, true, false)
  else IF_WORKING_POINT(Nonprompt_All_MaxWeight, true, false)
  // Backwards compatability for Custom WP
  else if (m_wp == "Custom") {
    m_applicator = std::unique_ptr<CP::TrackVertexAssociationTool::WorkingPoint>(
      new ::Custom(m_d0_cut, m_use_d0sig, m_d0sig_cut, m_dzSinTheta_cut, m_doUsedInFit));
  }
  else {
    ATH_MSG_ERROR("Invalid TVA working point '" << m_wp << "' - for a custom configuration, please provide 'Custom' for the 'WorkingPoint' property.");
    return StatusCode::FAILURE;
  }

  if (m_wp == "Custom") {
    ATH_MSG_INFO("TVA working point 'Custom' provided - tool properties are initialized to default values unless explicitly set by the user.");
  }
  else {
    ATH_MSG_INFO("TVA working point '" << m_wp << "' provided - tool properties have been configured accordingly.");
  }

  if (m_use_d0sig) {
    ATH_MSG_INFO("(For Custom WP:) cut on d0 significance: " << m_d0sig_cut << "\t(d0sig_cut).");
  }
  else {
    ATH_MSG_INFO("(For Custom WP:) cut on d0: " << m_d0_cut << "\t(d0_cut).");
  }
  ATH_MSG_INFO("(For Custom WP:) cut on Δz * sin θ: " << m_dzSinTheta_cut << "\t(dzSinTheta_cut).");
  ATH_MSG_INFO("(For Custom WP:) allow UsedInFit MatchStatus: " << m_doUsedInFit << "\t(doUsedInFit).");
  ATH_MSG_INFO("Require VxType::PriVtx for unique match: " << m_requirePriVtx << "\t(requirePriVtx).");

  // Initialize our EventInfo container and decoration reads
  ATH_CHECK(m_eventInfo.initialize());
  m_hardScatterDecoKey = SG::ReadDecorHandleKey<xAOD::EventInfo>(m_eventInfo.key() + "." + m_hardScatterDeco);
  ATH_CHECK(m_hardScatterDecoKey.initialize());
  m_vtxDecoAcc = std::make_unique<AMVFVerticesAcc>(m_vtxDecoKey.key());
  m_vtxDecoKey = SG::ReadDecorHandleKey<xAOD::TrackParticleContainer>(m_trkContName + "." + m_vtxDecoKey.key());
  ATH_CHECK(m_vtxDecoKey.initialize());
  m_wgtDecoAcc = std::make_unique<AMVFWeightsAcc>(m_wgtDecoKey.key());
  m_wgtDecoKey = SG::ReadDecorHandleKey<xAOD::TrackParticleContainer>(m_trkContName + "." + m_wgtDecoKey.key());
  ATH_CHECK(m_wgtDecoKey.initialize());

  return StatusCode::SUCCESS;
}

bool TrackVertexAssociationTool::isCompatible(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx) const
{
  ATH_MSG_DEBUG("In TrackVertexAssociationTool::isCompatible function.");
  return isMatch(trk, vx);
}

bool TrackVertexAssociationTool::isCompatible(const xAOD::TrackParticle& trk) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfo, ctx);
  if (!evt.isValid()) {
    throw std::runtime_error("ERROR in CP::TrackVertexAssociationTool::isCompatible : could not retrieve xAOD::EventInfo!");
  }
  SG::ReadDecorHandle<xAOD::EventInfo, ElementLink<xAOD::VertexContainer>> hardScatterDeco(m_hardScatterDecoKey, ctx);
  ElementLink<xAOD::VertexContainer> vtxLink = hardScatterDeco(*evt);
  if (!vtxLink.isValid()) {
    throw std::runtime_error("ERROR in CP::TrackVertexAssociationTool::isCompatible : hardscatter vertex link is not valid!");
  }
  return isMatch(trk, **vtxLink, evt.get());
}

xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const
{
  return getMatchMapInternal(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getMatchMap(const xAOD::TrackParticleContainer& trkCont, const xAOD::VertexContainer& vxCont) const
{
  return getMatchMapInternal(trkCont, vxCont);
}

const xAOD::Vertex* TrackVertexAssociationTool::getUniqueMatchVertex(const xAOD::TrackParticle& trk, std::vector<const xAOD::Vertex*>& vx_list) const
{
  return getUniqueMatchVertexInternal(trk, vx_list);
}

ElementLink<xAOD::VertexContainer> TrackVertexAssociationTool::getUniqueMatchVertexLink(const xAOD::TrackParticle& trk, const xAOD::VertexContainer& vxCont) const
{
  ElementLink<xAOD::VertexContainer> vx_link_tmp;
  const xAOD::Vertex* vx_tmp = getUniqueMatchVertexInternal(trk, vxCont);
  if (vx_tmp) {
    vx_link_tmp.toContainedElement(vxCont, vx_tmp);
  }
  return vx_link_tmp;
}

xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getUniqueMatchMap(std::vector<const xAOD::TrackParticle*>& trk_list, std::vector<const xAOD::Vertex*>& vx_list) const
{
  return getUniqueMatchMapInternal(trk_list, vx_list);
}

xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getUniqueMatchMap(const xAOD::TrackParticleContainer& trkCont, const xAOD::VertexContainer& vxCont) const
{
  return getUniqueMatchMapInternal(trkCont, vxCont);
}

/////////////////////
// Private methods //
/////////////////////

bool TrackVertexAssociationTool::isMatch(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo* evtInfo) const {

  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Return false for fake vertices
  if (vx.vertexType() == xAOD::VxType::NoVtx) {
    return false;
  }

  // Read our EventInfo
  const xAOD::EventInfo* evt = nullptr;
  if (!evtInfo) {
    SG::ReadHandle<xAOD::EventInfo> evttmp(m_eventInfo, ctx);
    if (!evttmp.isValid()) {
      throw std::runtime_error("ERROR in CP::TrackVertexAssociationTool::isMatch : could not retrieve xAOD::EventInfo!");
    }
    evt = evttmp.get();
  }
  else {
    evt = evtInfo;
  }

  // Apply the working point
  return m_applicator->apply(&trk, &vx, evt, *m_vtxDecoAcc, *m_wgtDecoAcc);

}

template <typename U, typename V>
xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getMatchMapInternal(U& trk_list, V& vx_list) const
{
  xAOD::TrackVertexAssociationMap trktovxmap;

  for (const auto& vertex : vx_list) {
    xAOD::TrackVertexAssociationList trktovxlist;
    trktovxlist.clear();
    trktovxlist.reserve(trk_list.size());
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
const xAOD::Vertex* TrackVertexAssociationTool::getUniqueMatchVertexInternal(const xAOD::TrackParticle& trk, T& vx_list) const
{
  FitWeight weight;
  float dzSinTheta;
  float min_dz = ((m_dzSinTheta_cut >= 0) ? m_dzSinTheta_cut : +999.0);
  const xAOD::Vertex* bestMatchVertex = nullptr;

  for (const auto& vertex : vx_list) {
    weight = ::fitWeight(&trk, vertex, *m_vtxDecoAcc, *m_wgtDecoAcc);
    if (m_doUsedInFit && weight.first > 0.0 && weight.first >= weight.second) {
      bestMatchVertex = vertex;
      break;
    }
    else {
      if (m_requirePriVtx && vertex->vertexType() != xAOD::VxType::PriVtx) continue;
      if (isCompatible(trk, *vertex)) {
        dzSinTheta = ::absDzSinTheta(&trk, vertex);
        if (dzSinTheta < min_dz) {
          min_dz = dzSinTheta;
          bestMatchVertex = vertex;
          if(m_doPVPriority) break; //This will stop the iteration on the vertices. This works since the PV should always be the first entry in the vertex collection
        }
      }
    }
  }

  // Check if get the matched vertex
  if (!bestMatchVertex) {
    ATH_MSG_DEBUG("Could not find any matched vertex for this track!");
  }

  return bestMatchVertex;
}

template <typename T, typename U>
xAOD::TrackVertexAssociationMap TrackVertexAssociationTool::getUniqueMatchMapInternal(T& trk_list, U& vx_list) const
{
  xAOD::TrackVertexAssociationMap trktovxmap;

  // Initialize our map
  for (const auto& vertex : vx_list) {
    xAOD::TrackVertexAssociationList trktovxlist;
    trktovxlist.clear();
    trktovxlist.reserve(trk_list.size());
    trktovxmap[vertex] = trktovxlist;
  }

  for (const auto& track : trk_list) {
    const xAOD::Vertex* vx_match = getUniqueMatchVertexInternal(*track, vx_list);
    if (vx_match) trktovxmap[vx_match].push_back(track); // Found matched vertex
  }

  return trktovxmap;
}

} // namespace CP
