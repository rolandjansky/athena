/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"

#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"

#include "muonEvent/MuonContainer.h"
#include "VxVertex/VxCandidate.h"

#include "L1TgcTandpFillerTool.h"


using CLHEP::Hep3Vector;


const double D3PD::L1TgcTandpFillerTool::MUON_MASS = 105.6583668;  // MeV/c  ref - JPG 37, 0750211 (2010) (pdg.lbl.gov)
const double D3PD::L1TgcTandpFillerTool::JPSI_MASS = 3096.916; // MeV/c  ref - JPG 37, 075021 (2010) (pdg.lbl.gov)
const double D3PD::L1TgcTandpFillerTool::Z_MASS = 91187.6; // MeV/c  ref - JPG 37, 075021 (2010) (pdg.lbl.gov)



D3PD::L1TgcTandpFillerTool::L1TgcTandpFillerTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : BlockFillerTool<void>(type, name, parent),
    m_vertexPointEstimator("InDet::VertexPointEstimator"),
    m_conversionFinderUtils("InDet::ConversionFinderUtils"),
    m_vKalVrtFitter("Trk::TrkVKalVrtFitter"),
    m_v0VertexFitter("Trk::TrkV0VertexFitter"),
    m_v0Tools("Trk::V0Tools")
{
  declareProperty ("MuonContainer", m_muonSgKey = "StacoMuonCollection", "SG key for Muon");
  declareProperty ("TrackParticleContainer", m_trackSgKey = "TrackParticleCandidate", "SG key for TrackParticle");
  declareProperty ("PrimaryVertexContainer", m_primaryVertexSgKey = "VxPrimaryCandidate", "SG key for PrimaryVertex");
  declareProperty ("PrimaryVertexContainer", m_primaryVertexSgKey = "VxPrimaryCandidate", "SG key for PrimaryVertex");
  declareProperty ("MinimumPt", m_minPt = 2., "minimum pt for tag-and-probe tracks");
  declareProperty ("OppositeCharge", m_hasOppositeCharge = true, "requires opposite charge for tag-and-probe or not");
  declareProperty ("UseV0Fitter", m_useV0Fitter = false, "use V0VertexFitter instead of VKalVrtFitter");
  declareProperty ("OniaMassLowerLimit", m_oniaMassLowerLimit = 2000., "lower mass limit for onia in MeV");
  declareProperty ("OniaMassUpperLimit", m_oniaMassUpperLimit = 15000., "upper mass limit for onia in MeV");
  declareProperty ("ZMassLowerLimit", m_zMassLowerLimit = 50000., "lower mass limit for Z in MeV");
  declareProperty ("ZMassUpperLimit", m_zMassUpperLimit = 1.e30, "upper mass limit for Z in MeV");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode
D3PD::L1TgcTandpFillerTool::initialize()
{
  CHECK(m_vertexPointEstimator.retrieve());
  CHECK(m_conversionFinderUtils.retrieve());
  CHECK(m_vKalVrtFitter.retrieve());
  CHECK(m_v0VertexFitter.retrieve());
  CHECK(m_v0Tools.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode
D3PD::L1TgcTandpFillerTool::book()
{
  CHECK(addVariable("tag_index", m_tag_index));       // index of muon
  CHECK(addVariable("probe_index", m_probe_index));   // index of id track
  CHECK(addVariable("vertex_ndof", m_vertex_ndof));
  CHECK(addVariable("vertex_chi2", m_vertex_chi2));
  CHECK(addVariable("vertex_posX", m_vertex_posX));
  CHECK(addVariable("vertex_posY", m_vertex_posY));
  CHECK(addVariable("vertex_posZ", m_vertex_posZ));
  CHECK(addVariable("vertex_probability", m_vertex_probability));
  CHECK(addVariable("v0_mass", m_v0_mass));
  CHECK(addVariable("v0_mass_error", m_v0_mass_error));
  CHECK(addVariable("v0_mass_probability", m_v0_mass_probability));
  CHECK(addVariable("v0_px", m_v0_px));
  CHECK(addVariable("v0_py", m_v0_py));
  CHECK(addVariable("v0_pz", m_v0_pz));
  CHECK(addVariable("v0_pt", m_v0_pt));
  CHECK(addVariable("v0_pt_error", m_v0_pt_error));

  CHECK(addVariable("num_verticies", m_nVerticies));
  CHECK(addVariable("vertex_index", m_vertex_index_vec)); // index of VxCandidate
  CHECK(addVariable("lxyVec", m_lxy_vec));
  CHECK(addVariable("lxy_errorVec", m_lxy_error_vec));
  CHECK(addVariable("tauVec", m_tau_vec));
  CHECK(addVariable("tau_errorVec", m_tau_error_vec));

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTandpFillerTool::fill()
{
  const Analysis::MuonContainer* muons = 0;
  CHECK(evtStore()->retrieve(muons, m_muonSgKey));

  typedef std::map<const Rec::TrackParticle*, const Rec::TrackParticle*> TpMap;
  typedef std::map<const Rec::TrackParticle*, int> IndexMap;
  TpMap isCombined;
  IndexMap indicies;

  size_t index = 0;
  Analysis::MuonContainer::const_iterator cit_muon;
  for (cit_muon = muons->begin(); cit_muon != muons->end(); cit_muon++, index++) {
    const Analysis::Muon *muon = *cit_muon;
    if (not muon->hasCombinedMuonTrackParticle()) continue;
    if (muon->pt() < m_minPt) continue;
    const Rec::TrackParticle *tp = muon->inDetTrackParticle();
    isCombined.insert(TpMap::value_type(tp, muon->combinedMuonTrackParticle()));
    indicies.insert(IndexMap::value_type(tp, index));
  }


  const Rec::TrackParticleContainer* tracks = 0;
  CHECK(evtStore()->retrieve(tracks, m_trackSgKey));

  Rec::TrackParticleContainer::const_iterator tag, probe;
  for (tag = tracks->begin(); tag != tracks->end(); tag++) {
    TpMap::const_iterator cit_tp = isCombined.find(*tag);
    if (cit_tp == isCombined.end()) continue;
    size_t index_probe = 0;
    for (probe = tracks->begin(); probe != tracks->end(); probe++, index_probe++) {
      if (*tag == *probe) continue;
      if ((*probe)->pt() < m_minPt) continue;
      if (not isCandidate(cit_tp->second, *probe)) continue; // (combined, id) pair
      // if (not isCandidate(*tag, *probe)) continue; // (id, id) pair

      Trk::VxCandidate* vxCandidate = doVertexing(*tag, *probe);
      if (vxCandidate == 0) continue;
      std::vector<const Trk::VxCandidate*> verticies;
      CHECK(getVxCandidates(verticies));

      // fill
      std::vector<double> masses;
      masses.push_back(MUON_MASS);
      masses.push_back(MUON_MASS);

      m_tag_index->push_back(indicies.find(*tag)->second);
      m_probe_index->push_back(index_probe);

      const Trk::ExtendedVxCandidate* resonance = dynamic_cast<const Trk::ExtendedVxCandidate*>(vxCandidate);
      const Amg::Vector3D& vtx = m_v0Tools->vtx(resonance);

      m_vertex_ndof->push_back(m_v0Tools->ndof(resonance));
      m_vertex_chi2->push_back(m_v0Tools->chisq(resonance));
      m_vertex_posX->push_back(vtx[0]);
      m_vertex_posY->push_back(vtx[1]);
      m_vertex_posZ->push_back(vtx[2]);
      m_vertex_probability->push_back(m_v0Tools->vertexProbability(resonance));

      const double mass = m_v0Tools->invariantMass(resonance, masses);
      const double error = m_v0Tools->invariantMassError(resonance, masses);
      double probability = -1.e30;
      if (mass < 15000.) {
        probability = m_v0Tools->invariantMassProbability(resonance,
                                                          JPSI_MASS,
                                                          masses);
      } else {
        probability = m_v0Tools->invariantMassProbability(resonance,
                                                          Z_MASS,
                                                          masses);
      }

      m_v0_mass->push_back(mass);
      m_v0_mass_error->push_back(error);
      m_v0_mass_probability->push_back(probability);

      Amg::Vector3D v0P = m_v0Tools->V0Momentum(resonance);
      m_v0_px->push_back(v0P[0]);
      m_v0_py->push_back(v0P[1]);
      m_v0_pz->push_back(v0P[2]);
      m_v0_pt->push_back(m_v0Tools->pT(resonance));
      m_v0_pt_error->push_back(m_v0Tools->pTError(resonance));


      m_nVerticies->push_back(verticies.size());
      std::vector<const Trk::VxCandidate*>::const_iterator cit;
      std::vector<int> vertex_index_vec;
      std::vector<float> lxy_vec, lxy_error_vec, tau_vec, tau_error_vec;
      for (cit  = verticies.begin(); cit != verticies.end(); cit++) {
        const Trk::VxCandidate* candidate = *cit;
        Trk::RecVertex vx = candidate->recVertex();
        Trk::RecVertex vertex(vx) ;

        vertex_index_vec.push_back(index);
        lxy_vec.push_back(m_v0Tools->lxy(resonance, vertex));
        lxy_error_vec.push_back(m_v0Tools->lxyError(resonance, vertex));

        tau_vec.push_back(m_v0Tools->tau(resonance, vertex, JPSI_MASS));
        tau_error_vec.push_back(m_v0Tools->tauError(resonance, vertex, JPSI_MASS));

      }
      delete vxCandidate;

      m_vertex_index_vec->push_back(vertex_index_vec);
      m_lxy_vec->push_back(lxy_vec);
      m_lxy_error_vec->push_back(lxy_error_vec);
      m_tau_vec->push_back(tau_vec);
      m_tau_error_vec->push_back(tau_error_vec);
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTandpFillerTool::getVxCandidates(std::vector<const Trk::VxCandidate*>& verticies) const
{
  const VxContainer *vxContainer = 0;
  CHECK(evtStore()->retrieve(vxContainer, m_primaryVertexSgKey));

  typedef VxContainer::const_iterator cit_vx;
  cit_vx cit_end = vxContainer->end();
  for (cit_vx cit = vxContainer->begin(); cit != cit_end; cit++) {
    const Trk::VxCandidate* candidate = *cit;
//    if ((candidate->vertexType() == Trk::PriVtx) or
//        (candidate->vertexType() == Trk::PileUp)) {
    if (true) {
      verticies.push_back(candidate);
    }
  }

  return StatusCode::SUCCESS;
}



bool
D3PD::L1TgcTandpFillerTool::isCandidate(const Rec::TrackParticle* tag,
                                        const Rec::TrackParticle* probe) const
{
  if (m_hasOppositeCharge) {
    if (tag->charge() * probe->charge() > 0.) return false;
  }

  const double mass = (tag->hlv() + probe->hlv()).m();
  const bool inOniaWindow = ((m_oniaMassLowerLimit < mass) and (mass < m_oniaMassUpperLimit));
  const bool inZWindow = ((m_zMassLowerLimit < mass) and (mass < m_zMassUpperLimit));

  if (inOniaWindow or inZWindow) return true;

  return false;
}



Trk::VxCandidate*
D3PD::L1TgcTandpFillerTool::doVertexing(const Rec::TrackParticle* tag,
                                        const Rec::TrackParticle* probe) const
{
  const Trk::TrackParticleBase* tpb_tag = static_cast<const Trk::TrackParticleBase*>(tag);
  const Trk::TrackParticleBase* tpb_probe = static_cast<const Trk::TrackParticleBase*>(probe);

  const Trk::Perigee* mp_tag = tpb_tag->perigee();
  const Trk::Perigee* mp_probe = tpb_probe->perigee();

  int sflag = 0;
  int errorcode = 0;
  Amg::Vector3D startingPoint = m_vertexPointEstimator->getCirclesIntersectionPoint(mp_tag ,mp_probe, sflag, errorcode);

  if (errorcode != 0) {startingPoint.setZero();}
  Trk::Vertex vertex(startingPoint);

  const Trk::TrackParameters* pb_tag = m_conversionFinderUtils->getTrkParticleParameters(tpb_tag);
  const Trk::TrackParameters* pb_probe = m_conversionFinderUtils->getTrkParticleParameters(tpb_probe);

  std::vector<const Trk::TrackParameters*> tracks; tracks.clear();

  tracks.push_back(pb_tag);
  tracks.push_back(pb_probe);

  Trk::VxCandidate* vxCandidate = 0;

  if (m_useV0Fitter) {
    Trk::TrkV0VertexFitter* fitter = 0;
    fitter = dynamic_cast<Trk::TrkV0VertexFitter *>(&(*m_v0VertexFitter));
    if (fitter == 0) {
      ATH_MSG_FATAL("The vertex fitter passed is not a V0 Vertex Fitter");
      return 0;
    }
    vxCandidate = fitter->fit(tracks, vertex);

  } else {
    vxCandidate = m_vKalVrtFitter->fit(tracks, vertex);
  }

  return vxCandidate;
}
/* eof */
