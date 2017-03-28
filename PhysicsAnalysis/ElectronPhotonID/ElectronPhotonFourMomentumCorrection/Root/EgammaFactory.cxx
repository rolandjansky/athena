/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALONE
#include <vector>
#include <cmath>
#include <string>
#include <cassert>

#include <AthLinks/ElementLink.h>
#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>
#include <xAODCaloEvent/CaloCluster.h>
#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODCaloEvent/CaloClusterAuxContainer.h>
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "PathResolver/PathResolver.h"

#include "ElectronPhotonFourMomentumCorrection/EgammaFactory.h"

void EgammaFactory::create_structure()
{
  ATH_MSG_DEBUG("Creating calo cluster container");
  m_clusters = new xAOD::CaloClusterContainer();
  m_clAux = new xAOD::CaloClusterAuxContainer();
  m_clusters->setStore(m_clAux);
  if (!m_store.record(m_clusters, "Clusters").isSuccess()) { ATH_MSG_ERROR("Cannot create cluster collection"); }
  if (!m_store.record(m_clAux, "ClustersAux.").isSuccess()) { ATH_MSG_ERROR("Canno create cluster aux collection"); };

  ATH_MSG_DEBUG("Creating vertex container");
  m_vertexes = new xAOD::VertexContainer();
  m_vxAux = new xAOD::VertexAuxContainer();
  m_vertexes->setStore(m_vxAux);
  if (!m_store.record(m_vertexes, "Vertexes").isSuccess()) { ATH_MSG_ERROR("Cannot create vertex collection"); };
  if (!m_store.record(m_vxAux, "VertexesAux.").isSuccess()) { ATH_MSG_ERROR("Cannot create vertex aux collection"); };

  ATH_MSG_DEBUG("Creating track container");
  m_tracks = new xAOD::TrackParticleContainer();
  m_tracksAux = new xAOD::TrackParticleAuxContainer();
  m_tracks->setStore(m_tracksAux);
  if (!m_store.record(m_tracks, "Tracks").isSuccess()) { ATH_MSG_ERROR("Cannot create track collection"); };
  if (!m_store.record(m_tracksAux, "TracksAux.").isSuccess()) {ATH_MSG_ERROR("Cannot create track aux collection"); };

  ATH_MSG_DEBUG("Creating photon container");
  m_photons = new xAOD::PhotonContainer();
  m_photonsAux = new xAOD::PhotonAuxContainer();
  m_photons->setStore(m_photonsAux);
  if (!m_store.record(m_photons, "Photons").isSuccess()) { ATH_MSG_ERROR("Cannot create photon collection"); };
  if (!m_store.record(m_photonsAux, "PhotonsAux.").isSuccess()) { ATH_MSG_ERROR("Cannot create photon aux collection"); };

  ATH_MSG_DEBUG("Creating electron container");
  m_electrons = new xAOD::ElectronContainer();
  m_electronsAux = new xAOD::ElectronAuxContainer();
  m_electrons->setStore(m_electronsAux);
  if (!m_store.record(m_electrons, "Electrons").isSuccess()) { ATH_MSG_ERROR("Cannot create electron collection"); };
  if (!m_store.record(m_electronsAux, "ElectronsAux.").isSuccess()) { ATH_MSG_ERROR("Cannot create electron aux collection"); };
}


EgammaFactory::EgammaFactory()
: asg::AsgMessaging("EgammaFactory")
{
  create_structure();

  TFile *f = TFile::Open( PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/average_layers.root").c_str(), "READ" );
  if (not f)
  {
    ATH_MSG_ERROR("cannot open file " << PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/average_layers.root").c_str() );
  }
  else { m_fave.reset(f); }
  m_histos_electron = std::array<TProfile2D*, 4> {
    static_cast<TProfile2D*>(m_fave->Get("histo_electron_ratio_Es0_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_electron_ratio_Es1_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_electron_ratio_Es2_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_electron_ratio_Es3_true_E")) };
  m_histos_conv = std::array<TProfile2D*, 4> {
    static_cast<TProfile2D*>(m_fave->Get("histo_conv_ratio_Es0_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_conv_ratio_Es1_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_conv_ratio_Es2_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_conv_ratio_Es3_true_E")) };
  m_histos_unconv = std::array<TProfile2D*, 4> {
    static_cast<TProfile2D*>(m_fave->Get("histo_unconv_ratio_Es0_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_unconv_ratio_Es1_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_unconv_ratio_Es2_true_E")),
    static_cast<TProfile2D*>(m_fave->Get("histo_unconv_ratio_Es3_true_E")) };
  m_histo_rconv = static_cast<TProfile2D*>(m_fave->Get("histo_conv_ph_Rconv"));
  m_histo_zconv = static_cast<TProfile2D*>(m_fave->Get("histo_conv_ph_zconv"));
 }

std::array<double, 4> EgammaFactory::get_layers_fraction(const std::array<TProfile2D*, 4>& prof, double eta, double pt) const
{
  std::array<double, 4> result;
  for (int i = 0; i != 4; ++i)
  {
    TProfile2D* p = prof[i];
    assert(p);
    result[i] = p->GetBinContent(p->FindBin(pt, std::abs(eta)));
  }
  return result;
}

void EgammaFactory::clear()
{
  m_store.clear();
  create_structure();
}


EgammaFactory::~EgammaFactory()
{
  m_store.clear();
}

xAOD::EventInfo* EgammaFactory::create_eventinfo(bool simulation, int runnumber, int eventnumber)
{
  xAOD::EventInfo* ei= new xAOD::EventInfo();
  ei->makePrivateStore();
  ei->setRunNumber(runnumber);
  ei->setEventNumber(eventnumber);
  ei->setEventTypeBitmask(simulation);
  return ei;
}

xAOD::CaloCluster* EgammaFactory::create_cluster(float eta, float phi, float e0, float e1, float e2, float e3, float e)
{
  ATH_MSG_DEBUG("creating cluster");
  // create cluster
  xAOD::CaloCluster* cluster = new xAOD::CaloCluster();
  cluster->makePrivateStore();

  ATH_MSG_DEBUG("setting cluster properties");
  // set cluster properties

  {
    // set eta, phi for all the layers (barrel / endcap)
    const std::set<CaloSampling::CaloSample> samplings {
      CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3,
      CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3 };
    unsigned sampling_pattern = 0;
    for (auto sample : samplings) { sampling_pattern |= 0x1U << sample; }
    ATH_MSG_DEBUG("setting sampling pattern");
    cluster->setSamplingPattern(sampling_pattern);
    ATH_MSG_DEBUG("nsamples  = " << cluster->nSamples());
    for (auto sample : samplings) {
      ATH_MSG_DEBUG("setting eta sampling");
      cluster->setEta(sample, eta);
      cluster->setPhi(sample, phi);
    }

    ATH_MSG_DEBUG("setting energies sampling");

    if (std::abs(eta) < 1.45) {
      cluster->setEnergy(CaloSampling::PreSamplerB, e0);
      cluster->setEnergy(CaloSampling::EMB1, e1);
      cluster->setEnergy(CaloSampling::EMB2, e2);
      cluster->setEnergy(CaloSampling::EMB3, e3);
    }
    else {
      cluster->setEnergy(CaloSampling::PreSamplerE, e0);
      cluster->setEnergy(CaloSampling::EME1, e1);
      cluster->setEnergy(CaloSampling::EME2, e2);
      cluster->setEnergy(CaloSampling::EME3, e3);
    }
    ATH_MSG_DEBUG("setting energy cluster");
    cluster->setE(e > 0 ? e : e0 + e1 + e2 +e3);
    ATH_MSG_DEBUG("setting eta cluster");
    cluster->setEta(eta);
    ATH_MSG_DEBUG("setting phi cluster");
    cluster->setPhi(phi);
    ATH_MSG_DEBUG("decorate cluster for etaCalo, phiCalo");
    cluster->auxdata<float>("etaCalo") = eta;
    cluster->auxdata<float>("phiCalo") = phi;
    //         void insertMoment( MomentType type, double value );
    cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME, eta);
    cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME, phi);
  }
  ATH_MSG_DEBUG("pushing cluster collection");
  m_clusters->push_back(cluster);
  return cluster;
}

xAOD::Photon* EgammaFactory::create_unconverted_photon(float eta, float phi, float e)
{
  return create_photon(eta, phi, e, 0., 0.);
}

xAOD::Photon* EgammaFactory::create_converted_photon(float eta, float phi, float e)
{
  assert(m_histo_rconv);
  assert(m_histo_zconv);
  const int bin = m_histo_rconv->FindBin(e / cosh(eta), std::abs(eta));
  if (m_histo_rconv->IsBinOverflow(bin)) { return create_photon(eta, phi, e, 0, 0); }
  else {
    const double rconv = m_histo_rconv->GetBinContent(bin);
    const double zconv = m_histo_zconv->GetBinContent(m_histo_zconv->FindBin(e / cosh(eta), std::abs(eta)));
    assert(rconv > 0);
    return create_photon(eta, phi, e, rconv, zconv);
  }
}


xAOD::Photon* EgammaFactory::create_photon(float eta, float phi, float e, float rconv, float zconv)
{
  const bool isconv = (rconv > 0 and rconv < 800);
  const auto l = get_layers_fraction(isconv ? m_histos_conv : m_histos_unconv, eta, e / cosh(eta));
  return create_photon(eta, phi, l[0] * e, l[1] * e, l[2] * e, l[3] * e, e, rconv, zconv);
}

xAOD::Electron* EgammaFactory::create_electron(float eta, float phi, float e)
{
  const auto l = get_layers_fraction(m_histos_electron, eta, e / cosh(eta));
  return create_electron(eta, phi, l[0] * e, l[1] * e, l[2] * e, l[3] * e, e);
}

xAOD::Photon* EgammaFactory::create_photon(float eta, float phi, float e0, float e1, float e2, float e3, float e, float rconv, float zconv)
{
  xAOD::CaloCluster* cluster = create_cluster(eta, phi, e0, e1, e2, e3, e);
  // create Vertex
  xAOD::Vertex* vertex = nullptr;
  if (rconv > 0 and rconv < 800) {
    ATH_MSG_DEBUG("creating vertex");
    vertex = new xAOD::Vertex();
    vertex->makePrivateStore();
    vertex->setZ(zconv);
    vertex->setX(rconv);
    vertex->setY(0);
    // decorate with pt1, pt2
    vertex->auxdata<float>("pt1") = e / cosh(eta) * 0.7;
    vertex->auxdata<float>("pt2") = e / cosh(eta) * 0.3;
    m_vertexes->push_back(vertex);
  }

  ATH_MSG_DEBUG("creating photon");
  xAOD::Photon* ph = new xAOD::Photon();
  ph->makePrivateStore();
  m_photons->push_back(ph);

  ATH_MSG_DEBUG("link cluster to photon");
  // set link to clusters
  std::vector< ElementLink< xAOD::CaloClusterContainer > > links_clusters;
  ATH_MSG_DEBUG("push back cluster = " << cluster);
  links_clusters.push_back(ElementLink< xAOD::CaloClusterContainer >( cluster, *m_clusters ));
  ATH_MSG_DEBUG("set link");
  ph->setCaloClusterLinks(links_clusters);

  // set link to vertex
  if (vertex)
  {
    ATH_MSG_DEBUG("link vertex to photon");
    std::vector<ElementLink<xAOD::VertexContainer>> links_vertexes;
    links_vertexes.push_back(ElementLink<xAOD::VertexContainer>(vertex, *m_vertexes));
    ph->setVertexLinks(links_vertexes);
  }
  else {
    ATH_MSG_DEBUG("not converted");
  }

  // set particle properties

  ph->setEta(eta);
  ph->setPhi(phi);
  ph->setM(0);
  ph->setPt(e / cosh(eta));

  return ph;
}


xAOD::Electron* EgammaFactory::create_electron(float eta, float phi, float e0, float e1, float e2, float e3, float e)
{
  ATH_MSG_DEBUG("creating cluster");
  xAOD::CaloCluster* cluster = create_cluster(eta, phi, e0, e1, e2, e3, e);

  ATH_MSG_DEBUG("creating track");
  xAOD::TrackParticle* track = new xAOD::TrackParticle();
  track->makePrivateStore();
  track->setDefiningParameters(0., 0., phi, 2 * atan(exp(-eta)), 1.);
  m_tracks->push_back(track);

  ATH_MSG_DEBUG("creating electron");
  xAOD::Electron* el = new xAOD::Electron();
  el->makePrivateStore();
  m_electrons->push_back(el);

  ATH_MSG_DEBUG("link track to electron");
  std::vector<ElementLink<xAOD::TrackParticleContainer>> links_tracks;
  links_tracks.push_back(ElementLink<xAOD::TrackParticleContainer>(track, *m_tracks));
  el->setTrackParticleLinks(links_tracks);

  ATH_MSG_DEBUG("link cluster to electron");
  std::vector< ElementLink< xAOD::CaloClusterContainer > > links_clusters;
  ATH_MSG_DEBUG("push back cluster = " << cluster);
  links_clusters.push_back(ElementLink< xAOD::CaloClusterContainer >( cluster, *m_clusters ));
  ATH_MSG_DEBUG("set link");
  el->setCaloClusterLinks(links_clusters);

  // set particle properties

  el->setEta(eta);
  el->setPhi(phi);
  el->setM(0);
  el->setPt(e / cosh(eta));

  return el;
}
#endif
