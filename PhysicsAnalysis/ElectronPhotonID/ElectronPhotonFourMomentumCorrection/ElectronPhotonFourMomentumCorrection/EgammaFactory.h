/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef EGAMMAFACTORY_H
#define EGAMMAFACTORY_H

#ifdef XAOD_STANDALONE
/**
 * This class if for testing only.
 * It has been designed only to test ElectronPhotonFourMomentumCorrection
 **/

#include <array>
#include <memory>

#include <AsgMessaging/AsgMessaging.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>
#include <xAODEgamma/Electron.h>
#include <xAODEgamma/Photon.h>
#include <xAODEgamma/Egamma.h>
#include <xAODEgamma/PhotonContainer.h>
#include <xAODEgamma/PhotonAuxContainer.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODEgamma/ElectronAuxContainer.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODCaloEvent/CaloClusterAuxContainer.h>
#include <xAODTracking/VertexContainer.h>
#include <xAODTracking/VertexAuxContainer.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTracking/TrackParticleAuxContainer.h>

#include <TProfile2D.h>
#include <TFile.h>

class EgammaFactory : public asg::AsgMessaging {
public:
  EgammaFactory();
  ~EgammaFactory();
  void clear();
  void create_structure();
  xAOD::CaloCluster* create_cluster(float eta, float phi, float e0, float e1, float e2, float e3, float e);
  xAOD::Photon* create_converted_photon(float eta, float phi, float e);
  xAOD::Photon* create_unconverted_photon(float eta, float phi, float e);
  xAOD::Photon* create_photon(float eta, float phi, float e, float rconv=0, float zconv=0);
  xAOD::Photon* create_photon(float eta, float phi, float e0, float e1, float e2, float e3, float e, float rconv=0, float zconv=0);
  xAOD::Electron* create_electron(float eta, float phi, float e);
  xAOD::Electron* create_electron(float eta, float phi, float e0, float e1, float e2, float e3, float e);
  xAOD::EventInfo* create_eventinfo(bool simulation, int runnumber, int eventnumber=1);
private:
  xAOD::TStore m_store;  //!
  xAOD::CaloClusterContainer* m_clusters;  //!
  xAOD::CaloClusterAuxContainer* m_clAux;  //!
  xAOD::VertexContainer* m_vertexes;  //!
  xAOD::VertexAuxContainer* m_vxAux;  //!
  xAOD::PhotonContainer* m_photons; //!
  xAOD::PhotonAuxContainer* m_photonsAux; //!
  xAOD::ElectronContainer* m_electrons; //!
  xAOD::ElectronAuxContainer* m_electronsAux; //!
  xAOD::TrackParticleContainer* m_tracks; //!
  xAOD::TrackParticleAuxContainer* m_tracksAux; //!

  std::unique_ptr<TFile> m_fave;  //!
  std::array<TProfile2D*, 4> m_histos_electron;  //!
  std::array<TProfile2D*, 4> m_histos_conv;  //!
  std::array<TProfile2D*, 4> m_histos_unconv;  //!
  TProfile2D* m_histo_rconv; //!
  TProfile2D* m_histo_zconv; //!
  std::array<double, 4> get_layers_fraction(const std::array<TProfile2D*, 4>& prof, double eta, double pt) const;  //!
};

#else

struct EgammaFactory {
};

#endif

#endif
