/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "PhotonVertexSelection/PhotonVertexHelpers.h"

// EDM includes
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

// Asg tools
#include "IsolationCorrections/ShowerDepthTool.h"

namespace xAOD {
namespace PVHelpers {

//__________________________________________________________________________
const xAOD::Vertex*
getHardestVertex(const xAOD::VertexContainer* vertices)
{
  // Check PV collection

  // Check for primary vertex in collection
  for (const auto* vertex : *vertices) {
    if (vertex->vertexType() == xAOD::VxType::VertexType::PriVtx)
      return vertex;
  }

  return nullptr;
}

//__________________________________________________________________________
std::pair<float, float>
getZCommonAndError(const xAOD::EventInfo* eventInfo,
                   const xAOD::EgammaContainer* egammas,
                   float convPtCut)
{
  // Static accessors
  static const SG::AuxElement::Accessor<float> zvertex("zvertex");
  static const SG::AuxElement::Accessor<float> errz("errz");
  static const SG::AuxElement::Accessor<float> HPV_zvertex("HPV_zvertex");
  static const SG::AuxElement::Accessor<float> HPV_errz("HPV_errz");

  // Clear values
  float zCommon = 0.0, zCommonError = 0.0;

  // Beam position is the base for zCommon weighted average
  double beamPosZ = eventInfo->beamPosZ();
  double beamPosSigmaZ = eventInfo->beamPosSigmaZ();

  zCommon = beamPosZ / beamPosSigmaZ / beamPosSigmaZ;
  zCommonError = 1.0 / beamPosSigmaZ / beamPosSigmaZ;

  // Include z-position pointed at by egammas
  for (const xAOD::Egamma* egamma : *egammas) {
    if (egamma == nullptr) {
      continue;
    }

    const xAOD::Photon* photon = nullptr;
    if (egamma->type() == xAOD::Type::Photon)
      photon = dynamic_cast<const xAOD::Photon*>(egamma);

    if (photon && passConvSelection(photon, convPtCut)) {
      if (!HPV_zvertex.isAvailable(*photon) || !HPV_errz.isAvailable(*photon)) {
        continue;
      }
      if (HPV_errz(*photon) == 0.0) {
        continue;
      }
      zCommon += HPV_zvertex(*photon) / HPV_errz(*photon) / HPV_errz(*photon);
      zCommonError += 1.0 / HPV_errz(*photon) / HPV_errz(*photon);
    } else {
      if (!zvertex.isAvailable(*egamma) || !errz.isAvailable(*egamma)) {
        continue;
      }
      if (errz(*egamma) == 0.0) {
        continue;
      }
      zCommon += zvertex(*egamma) / errz(*egamma) / errz(*egamma);
      zCommonError += 1.0 / errz(*egamma) / errz(*egamma);
    }
  }

  // Normalize by error (weighted average)
  zCommon /= zCommonError;
  zCommonError = 1.0 / sqrt(zCommonError);

  return std::make_pair(zCommon, zCommonError);
}

//____________________________________________________________________________
bool
passConvSelection(const xAOD::Vertex& conversionVertex,
                  size_t i,
                  float convPtCut)
{
  const xAOD::TrackParticle* tp = conversionVertex.trackParticle(i);

  if (!xAOD::EgammaHelpers::numberOfSiHits(tp))
    return false;
  // pt1,pt2 is only set for 2-track vertices
  if (conversionVertex.nTrackParticles() == 1)
    return xAOD::EgammaHelpers::momentumAtVertex(conversionVertex).perp() >
           convPtCut;

  std::string s = Form("pt%lu", i + 1);
  return (conversionVertex.isAvailable<float>(s) &&
          conversionVertex.auxdata<float>(s) > convPtCut);
}

//____________________________________________________________________________
bool
passConvSelection(const xAOD::Photon* photon, float convPtCut)
{
  const xAOD::Vertex* conversionVertex = photon->vertex();
  if (conversionVertex == nullptr)
    return false;

  size_t NumberOfTracks = conversionVertex->nTrackParticles();
  for (size_t i = 0; i < NumberOfTracks; ++i) {
    if (passConvSelection(*conversionVertex, i, convPtCut))
      return true;
  }

  return false;
}

//__________________________________________________________________________
TLorentzVector
getTrackAtFirstMeasurement(const xAOD::TrackParticle* tp)
{

  static const SG::AuxElement::ConstAccessor<std::vector<float>> accParameterPX(
    "parameterPX");
  static const SG::AuxElement::ConstAccessor<std::vector<float>> accParameterPY(
    "parameterPY");

  TLorentzVector v;
  v.SetPtEtaPhiM(tp->pt(), tp->eta(), tp->phi(), 0.);
  if (!accParameterPX.isAvailable(*tp) || !accParameterPY.isAvailable(*tp)) {
    return v;
  }

  for (unsigned int i = 0; i < accParameterPX(*tp).size(); ++i) {
    if (tp->parameterPosition(i) == xAOD::FirstMeasurement) {
      v.SetPxPyPzE(accParameterPX(*tp)[i], accParameterPY(*tp)[i], 0, 0);
      return v;
    }
  }
  return v;
}

//__________________________________________________________________________
TLorentzVector
getVertexMomentum(const xAOD::Vertex* vertex,
                  bool useAux /* true */,
                  const std::string& derivationPrefix /* "" */)
{
  TLorentzVector v;

  SG::AuxElement::ConstAccessor<float> pt(derivationPrefix + "pt");
  SG::AuxElement::ConstAccessor<float> eta(derivationPrefix + "eta");
  SG::AuxElement::ConstAccessor<float> phi(derivationPrefix + "phi");

  if (useAux and pt.isAvailable(*vertex) and eta.isAvailable(*vertex) and
      phi.isAvailable(*vertex)) {
    v.SetPtEtaPhiM(pt(*vertex), eta(*vertex), phi(*vertex), 0.0);
    return v;
  }

  // Sum the 4-momenta of all track particles at the vertex
  const xAOD::TrackParticle* tp = nullptr;
  for (size_t i = 0; i < vertex->nTrackParticles(); ++i) {
    tp = vertex->trackParticle(i);
    if (tp == nullptr) {
      continue; // protect against thinned tracks
    }
    v += tp->p4();
  }
  return v;
}

//__________________________________________________________________________
float
getVertexSumPt(const xAOD::Vertex* vertex, int power, bool useAux /* = true */)
{

  std::string pw = (power == 1) ? "sumPt" : Form("sumPt%d", power);
  if (useAux and vertex->isAvailable<float>(pw))
    return vertex->auxdata<float>(pw);

  // Loop over all track particles, sum up their pt
  float pt = 0.0;
  const xAOD::TrackParticle* tp = nullptr;
  for (size_t i = 0; i < vertex->nTrackParticles(); ++i) {
    tp = vertex->trackParticle(i);
    if (tp == nullptr)
      continue; // protect against slimmed tracks

    pt += pow(tp->pt() / 1e3, power);
  }
  return pt;
}

} // namespace PVHelpers
} // namespace xAOD
