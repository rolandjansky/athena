#include <TFile.h>
#include <TTree.h>
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "PixelStubletsMaker/PixelStubletsMakerAlg.h"
#include "xAODEventInfo/EventInfo.h"

// StoreGate, Athena, and Database stuff:
//#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Tracking:
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
//#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkTrack/TrackInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

// Pixels:
#include "InDetIdentifier/PixelID.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
//#include "PixelGeoModel/IBLParameterSvc.h"
//#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

// CLHEP:
#include "CLHEP/Matrix/Vector.h"

// Particle masses

// Math functions:
#include <cmath>

#include "PathResolver/PathResolver.h"

#define USE_SPACEPOINT_IN_NEWTRACKING

using namespace DEdx;

namespace {

class PixelCluster {
public:
  PixelCluster(double p, double e, double c, double pe, double ee)
  : phi(p), eta(e), DEDX(c), dphi(pe), deta(ee){};

public:
  double phi;
  double eta;
  double DEDX;
  double dphi;
  double deta;
};
}

double CalDeltaR(double, double, double, double);

PixelStubletsMakerAlg::PixelStubletsMakerAlg(const std::string &name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
  m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool"),
  //m_IBLParameterSvc("IBLParameterSvc", "name"),
  m_overflowIBLToT(0),
  //m_offlineCalibSvc("PixelOfflineCalibSvc", "name"),
  m_mydedx(0),
  m_pixelid(0),
  m_detStore(0),
  m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels")
{
  declareProperty("TrackParticleCreator", m_particleCreator);
  declareProperty("OutputContainerName" , m_outputContainerName = "InDetTwoPlusPVTrackParticles");
  declareProperty("DEdxThreshold"       , m_dEdxThreshold = 0.);
  declareProperty("ChisqThresholdPhi"   , m_chisqThresholdPhi = 10.0);
  declareProperty("ChisqThresholdEta"   , m_chisqThresholdEta = 10.0);

  declareProperty("AssociationTool"       ,m_assoTool              );
  declareProperty("UseAssociationTool"    ,m_useassoTool = false   );
  
}

StatusCode PixelStubletsMakerAlg::initialize() {
  ATH_MSG_INFO("initialize()");

  ATH_CHECK(m_particleCreator.retrieve());
  ATH_CHECK(detStore()->retrieve(m_pixelid, "PixelID"));

  m_filename       = "mcpar_signed_234.txt";
  m_readfromcool   = true;
  m_mindedxformass = 1.8;

  //m_nusedhits            = -1;
  //m_nUsedIBLOverflowHits = 0;
  const float energyPair       = 3.68e-6;  // Energy in MeV to create an electron-hole pair in silicon
  const float sidensity        = 2.329;    // silicon density in g cm^-3

  //conversion Factor
  //{.025,.023,.020}; //{Old Planars,IBL_3Ds,IBL_Planars} the sensors thickness will be take into account in dEdx calculation

  m_conversionfactor = energyPair / sidensity;

  if (m_mydedx) {
    delete m_mydedx;
    m_mydedx = 0;
  }

  // Register callback function for cache updates:
  //const DataHandle<CondAttrListCollection> aptr;
  if (m_readfromcool) {
    m_mydedx = new dEdxID;
  } else {
    std::string file_name = PathResolver::find_file(m_filename, "DATAPATH");
    if (file_name.size() == 0) {
      msg(MSG::FATAL) << "Could not find dEdx calibration file" << m_filename << endreq;
      return StatusCode::FAILURE;
    }
    m_mydedx = new dEdxID(file_name.c_str());
  }

  if( m_useassoTool ) {
    if( m_assoTool.retrieve().isFailure()) {
      msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_assoTool<<endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endreq;
    }
  }

  //m_overflowIBLToT = m_offlineCalibSvc->getIBLToToverflow();

  ATH_MSG_INFO("initialize() successful in " << name());

  return StatusCode::SUCCESS;
}

StatusCode PixelStubletsMakerAlg::finalize() {
  ATH_MSG_INFO("finalize()");

  delete m_mydedx;

  return StatusCode::SUCCESS;
}

StatusCode PixelStubletsMakerAlg::execute() {
  // Event information
  const xAOD::EventInfo *eventInfo = nullptr;
  if (StatusCode::SUCCESS != evtStore()->retrieve(eventInfo) || !eventInfo) {
    ATH_MSG_WARNING("Could not retrieve EventInfo");
    return StatusCode::SUCCESS;
  }

  // Vertices
  const xAOD::Vertex *vertex = nullptr;
  const xAOD::VertexContainer *vertexContainer;
  if (StatusCode::SUCCESS != evtStore()->retrieve(vertexContainer, "PrimaryVertices")) {
    ATH_MSG_ERROR("Cannot find VertexContainer PrimaryVertices");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_INFO("VertexPrimaryVertices: " << vertexContainer->size());
    for (size_t iVertex = 0; iVertex < vertexContainer->size(); ++iVertex) {
      // use the first vertex in this test code
      if (vertexContainer->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) {
        if (vertex != nullptr) {
          ATH_MSG_ERROR("there are more than two primary vertices");
          abort();
        }
        vertex = vertexContainer->at(iVertex);
      }
    }
    if (!vertex) return StatusCode::SUCCESS;
  }

  // Cluster
#ifdef USE_SPACEPOINT_IN_NEWTRACKING
  const SpacePointContainer *clusterContainer = 0;
  if (StatusCode::SUCCESS != evtStore()->retrieve(clusterContainer, "PixelSpacePoints") || !clusterContainer) {
    ATH_MSG_WARNING("Could not retrieve clusters");
    return StatusCode::SUCCESS;
  }
#else
  const InDet::PixelClusterContainer *clusterContainer = 0;
  if (StatusCode::SUCCESS != evtStore()->retrieve(clusterContainer, "PixelClusters") || !clusterContainer) {
    ATH_MSG_WARNING("Could not retrieve clusters");
    return StatusCode::SUCCESS;
  }
#endif
  //if (clusterContainer->size()) ATH_MSG_INFO("# of clusters = " << clusterContainer->size());

  // Get Cluster Information
  std::vector<int> becVec; // 0:barrel, +-2:endcap, +-4:DBM
  std::vector<int> layerVec;
  std::vector<int> etamoduleVec;
  std::vector<int> phimoduleVec;
  std::vector<double> posXVec;
  std::vector<double> posYVec;
  std::vector<double> posZVec;
  std::vector<double> dEdxVec;
  std::vector<double> chargeVec;
  std::vector<double> cosAlphaVec;
  std::vector<double> rphiUncVec;
  std::vector<double> zUncVec;
  std::vector<bool>   isFakeVec;
  std::vector<bool>   isAmbiguousVec;
  std::vector<bool>   isSplitVec;
  std::vector<bool>   isUsedVec;
#ifdef USE_SPACEPOINT_IN_NEWTRACKING
  for (const auto &spc : *clusterContainer) {
    // skip empty collections
    if (spc->empty()) continue;
    for (auto &sp : *spc) {
      auto cluster = (InDet::PixelCluster*)sp->clusterList().first;
#else
  for (const auto &clusterCollection : *clusterContainer) {
    // skip empty collections
    if (clusterCollection->empty()) continue;
    for (auto &cluster : *clusterCollection) {
#endif
      Identifier clusterId = cluster->identify();
      becVec.push_back(m_pixelid->barrel_ec(clusterId));
      layerVec.push_back(m_pixelid->layer_disk(clusterId));
      etamoduleVec.push_back(m_pixelid->eta_module(clusterId));
      phimoduleVec.push_back(m_pixelid->phi_module(clusterId));
      posXVec.push_back(cluster->globalPosition().x());
      posYVec.push_back(cluster->globalPosition().y());
      posZVec.push_back(cluster->globalPosition().z());
      double cosalpha;
      float charge;
      dEdxVec.push_back(dEdxCluster(cluster, vertex, &cosalpha, &charge));
      chargeVec.push_back(charge);
      cosAlphaVec.push_back(cosalpha);
      rphiUncVec.push_back(sqrt(cluster->localCovariance()(Amg::x, Amg::x)));
      zUncVec.push_back(sqrt(cluster->localCovariance()(Amg::y, Amg::y)));
      isFakeVec.push_back(cluster->isFake());
      isAmbiguousVec.push_back(cluster->isAmbiguous());
      isSplitVec.push_back(cluster->isSplit());
#ifdef USE_SPACEPOINT_IN_NEWTRACKING
      isUsedVec.push_back(m_useassoTool? m_assoTool->isUsed(*cluster): false);
#else
      isUsedVec.push_back(false);
#endif
    }
  }
  
  ATH_MSG_INFO(" # of prds = "<<dEdxVec.size());

  // Track reconstruction
  std::vector<PixelCluster> firstLayerClusters;
  std::vector<PixelCluster> secondLayerClusters;
  std::vector<PixelCluster> thirdLayerClusters;
  std::vector<PixelCluster> fourthLayerClusters;

  // const double R1 = 33.25;  // mm
  // const double R2 = 50.50;  // mm

  //const double dZ0 = 50.0e-3;  // Primary Vertex
  //const double dX0 = 11.0e-3;  // Primary Vertex
  //const double dZ1 = 60.4e-3;  // IBL
  //const double dX1 = 9.62e-3;  // IBL
  //const double dZ2 = 109.7e-3; // B-Layer
  //const double dX2 = 12.4e-3;  // B-Layer
  //const double dZ3 = 0;
  //const double dX3 = 0;
  //const double dZ4 = 0;
  //const double dX4 = 0;

  // Select large dE/dx clusters
  TVector3 dir;
  for (unsigned int iCluster = 0; iCluster < dEdxVec.size(); iCluster++) {
    if (becVec[iCluster] != 0) continue; // Reject clusters at end-cap
    if (dEdxVec[iCluster] < m_dEdxThreshold) continue;

    dir.SetX(posXVec[iCluster] - vertex->x());
    dir.SetY(posYVec[iCluster] - vertex->y());
    dir.SetZ(posZVec[iCluster] - vertex->z());
    auto Rsq = dir.X() * dir.X() + dir.Y() * dir.Y();
    auto Zsq = dir.Z() * dir.Z();
    auto errX = rphiUncVec[iCluster];
    auto errZ = zUncVec[iCluster];
    auto covXpv = vertex->covariancePosition()(Amg::x, Amg::x) * sin(dir.Phi()) * sin(dir.Phi())
                + vertex->covariancePosition()(Amg::x, Amg::y) * sin(dir.Phi()) * cos(dir.Phi()) * 2
                + vertex->covariancePosition()(Amg::y, Amg::y) * cos(dir.Phi()) * cos(dir.Phi());
    auto covZpv = vertex->covariancePosition()(Amg::z, Amg::z);
    //std::cout<<"PV error xx: "<<vertex->covariancePosition()(Amg::x, Amg::x)<<std::endl;
    //std::cout<<"PV error xy: "<<vertex->covariancePosition()(Amg::x, Amg::y)<<std::endl;
    //std::cout<<"PV error yy: "<<vertex->covariancePosition()(Amg::y, Amg::y)<<std::endl;
    //std::cout<<"PV error zz: "<<vertex->covariancePosition()(Amg::z, Amg::z)<<std::endl;
    //std::cout<<"phi        : "<<dir.Phi()<<std::endl;
    //std::cout<<"errXpv     : "<<sqrt(covXpv)<<std::endl;
    //std::cout<<"errZpv     : "<<sqrt(covZpv)<<std::endl;
    double dRsq = covXpv + errX * errX;
    double dZsq = covZpv + errZ * errZ;
    double dEta = 1 / sqrt( Rsq + Zsq ) * sqrt( Zsq / Rsq * dRsq + dZsq);
    double dPhi = sqrt(dRsq / Rsq);
    switch (layerVec[iCluster]) {
      case 0:
        firstLayerClusters.push_back(PixelCluster(dir.Phi(), -log(tan(dir.Theta() / 2)), dEdxVec[iCluster], dPhi, dEta));
        break;
      case 1:
        secondLayerClusters.push_back(PixelCluster(dir.Phi(), -log(tan(dir.Theta() / 2)), dEdxVec[iCluster], dPhi, dEta));
        break;
      case 2:
        thirdLayerClusters.push_back(PixelCluster(dir.Phi(), -log(tan(dir.Theta() / 2)), dEdxVec[iCluster], dPhi, dEta));
        break;
      case 3:
        fourthLayerClusters.push_back(PixelCluster(dir.Phi(), -log(tan(dir.Theta() / 2)), dEdxVec[iCluster], dPhi, dEta));
        break;
      default:
        break;
    }
  }
  ATH_MSG_INFO(" # of clusters at first  layer = " << firstLayerClusters.size());
  ATH_MSG_INFO(" # of clusters at second layer = " << secondLayerClusters.size());
  ATH_MSG_INFO(" # of clusters at third  layer = " << thirdLayerClusters.size());
  ATH_MSG_INFO(" # of clusters at fourth layer = " << fourthLayerClusters.size());

  // Tracking
  xAOD::TrackParticleContainer *tpc = new xAOD::TrackParticleContainer();
  if (evtStore()->record(tpc, m_outputContainerName).isFailure()) {
    ATH_MSG_ERROR("Couldn't record " << m_outputContainerName);
    return StatusCode::FAILURE;
  }
  xAOD::TrackParticleAuxContainer *tpc_aux = new xAOD::TrackParticleAuxContainer();
  if (evtStore()->record(tpc_aux, m_outputContainerName + "Aux.").isFailure()) {
    ATH_MSG_ERROR("Couldn't record " << m_outputContainerName << "Aux.");
    return StatusCode::FAILURE;
  }
  tpc->setStore(tpc_aux);

  double phi;
  double eta;
  double phiUncert;
  double etaUncert;
  double chisqNdfPhi;
  double chisqNdfEta;
  double w1, w2;
  SG::AuxElement::Decorator< double > dec_trackPhiVec("trackPhiVec");
  SG::AuxElement::Decorator< double > dec_trackEtaVec("trackEtaVec");
  SG::AuxElement::Decorator< double > dec_trackPhiUncertVec("trackPhiUncertVec");
  SG::AuxElement::Decorator< double > dec_trackEtaUncertVec("trackEtaUncertVec");
  SG::AuxElement::Decorator< double > dec_trackChisqNdfPhiVec("trackChisqNdfPhiVec");
  SG::AuxElement::Decorator< double > dec_trackChisqNdfEtaVec("trackChisqNdfEtaVec");
  SG::AuxElement::Decorator< double > dec_trackDEDX1Vec("trackDEDX1Vec");
  SG::AuxElement::Decorator< double > dec_trackDEDX2Vec("trackDEDX2Vec");
  SG::AuxElement::Decorator< double > dec_trackClusID1Vec("trackClusID1Vec");
  SG::AuxElement::Decorator< double > dec_trackClusID2Vec("trackClusID2Vec");

  SG::AuxElement::Decorator< int > dec_trackIsFakeVec("trackIsFakeVec");
  SG::AuxElement::Decorator< int > dec_trackIsAmbiguousVec("trackIsAmbiguousVec");
  SG::AuxElement::Decorator< int > dec_trackIsSplitVec("trackIsSplitVec");
  SG::AuxElement::Decorator< int > dec_trackIsUsedVec("trackIsUsedVec");

  SG::AuxElement::Decorator< double > dec_trackChisqNdfPhiThirdVec("trackChisqNdfPhiThirdVec");
  SG::AuxElement::Decorator< double > dec_trackChisqNdfEtaThirdVec("trackChisqNdfEtaThirdVec");
  SG::AuxElement::Decorator< double > dec_trackChisqNdfPhiForthVec("trackChisqNdfPhiForthVec");
  SG::AuxElement::Decorator< double > dec_trackChisqNdfEtaForthVec("trackChisqNdfEtaForthVec");

  SG::AuxElement::Decorator< int > dec_trackHitL0dR001Vec("trackHitL0dR001Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL0dR010Vec("trackHitL0dR010Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL0dR020Vec("trackHitL0dR020Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL0dR040Vec("trackHitL0dR040Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL1dR001Vec("trackHitL1dR001Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL1dR010Vec("trackHitL1dR010Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL1dR020Vec("trackHitL1dR020Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL1dR040Vec("trackHitL1dR040Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL2dR001Vec("trackHitL2dR001Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL2dR010Vec("trackHitL2dR010Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL2dR020Vec("trackHitL2dR020Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL2dR040Vec("trackHitL2dR040Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL3dR001Vec("trackHitL3dR001Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL3dR010Vec("trackHitL3dR010Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL3dR020Vec("trackHitL3dR020Vec");
  SG::AuxElement::Decorator< int > dec_trackHitL3dR040Vec("trackHitL3dR040Vec");

  for (size_t icls1 = 0; icls1< firstLayerClusters.size(); icls1++){
    auto cls1 = firstLayerClusters.at(icls1);
    for (size_t icls2 = 0; icls2< secondLayerClusters.size(); icls2++){
      auto cls2 = secondLayerClusters.at(icls2);
      // phi
      w1          = 1 / (cls1.dphi * cls1.dphi);
      w2          = 1 / (cls2.dphi * cls2.dphi);
      phi         = (w1 * cls1.phi + w2 * cls2.phi) / (w1 + w2);
      phiUncert   = 1 / sqrt(w1 + w2);
      chisqNdfPhi = ((cls1.phi - phi) * (cls1.phi - phi) * w1 +
                     (cls2.phi - phi) * (cls2.phi - phi) * w2) /
                    2;
      if (chisqNdfPhi > m_chisqThresholdPhi) continue;

      // eta
      w1          = 1 / (cls1.deta * cls1.deta);
      w2          = 1 / (cls2.deta * cls2.deta);
      eta         = (w1 * cls1.eta + w2 * cls2.eta) / (w1 + w2);
      etaUncert   = 1 / sqrt(w1 + w2);
      chisqNdfEta = ((cls1.eta - eta) * (cls1.eta - eta) * w1 +
                     (cls2.eta - eta) * (cls2.eta - eta) * w2) /
                    2;
      if (chisqNdfEta > m_chisqThresholdEta) continue;

      // count the number of clusters near the tracks
      int iHitdR001[4] = {0};
      int iHitdR010[4] = {0};
      int iHitdR020[4] = {0};
      int iHitdR040[4] = {0};
      for (auto ihit : firstLayerClusters) {
        double dR = CalDeltaR(phi, ihit.phi, eta, ihit.eta);
        if (dR < 0.01) iHitdR001[0]++;
        if (dR < 0.10) iHitdR010[0]++;
        if (dR < 0.20) iHitdR020[0]++;
        if (dR < 0.40) iHitdR040[0]++;
      }  // for L0
      for (auto ihit : secondLayerClusters) {
        double dR = CalDeltaR(phi, ihit.phi, eta, ihit.eta);
        if (dR < 0.01) iHitdR001[1]++;
        if (dR < 0.10) iHitdR010[1]++;
        if (dR < 0.20) iHitdR020[1]++;
        if (dR < 0.40) iHitdR040[1]++;
      }  // for L1
      for (auto ihit : thirdLayerClusters) {
        double dR = CalDeltaR(phi, ihit.phi, eta, ihit.eta);
        if (dR < 0.01) iHitdR001[2]++;
        if (dR < 0.10) iHitdR010[2]++;
        if (dR < 0.20) iHitdR020[2]++;
        if (dR < 0.40) iHitdR040[2]++;
      }  // for L2
      for (auto ihit : fourthLayerClusters) {
        double dR = CalDeltaR(phi, ihit.phi, eta, ihit.eta);
        if (dR < 0.01) iHitdR001[3]++;
        if (dR < 0.10) iHitdR010[3]++;
        if (dR < 0.20) iHitdR020[3]++;
        if (dR < 0.40) iHitdR040[3]++;
      }  // for L3
      
      //=== Check Nearest clusters from tracks ===//
      auto chisqNdfPhiThird = 999.;
      auto chisqNdfEtaThird = 999.;
      auto chisqNdfPhiFourth = 999.;
      auto chisqNdfEtaFourth = 999.;
      auto chisqNdfPhiTmp   = 999.;
      auto chisqNdfEtaTmp   = 999.;
      auto wtmp             = 0.;
      for (size_t icls = 0; icls< thirdLayerClusters.size(); icls++){
        auto cls = thirdLayerClusters.at(icls);
        // phi
        wtmp = 1 / (cls.dphi * cls.dphi);
        chisqNdfPhiTmp = (cls.phi - phi) * (cls.phi - phi) * wtmp;
        if (chisqNdfPhiTmp < chisqNdfPhiThird) chisqNdfPhiThird = chisqNdfPhiTmp;

        // eta
        wtmp = 1 / (cls.deta * cls.deta);
        chisqNdfEtaTmp = (cls.eta - eta) * (cls.eta - eta) * wtmp;
        if (chisqNdfEtaTmp < chisqNdfEtaThird) chisqNdfEtaThird = chisqNdfEtaTmp;
      }
      for (size_t icls = 0; icls< fourthLayerClusters.size(); icls++){
        auto cls = fourthLayerClusters.at(icls);
        // phi
        wtmp = 1 / (cls.dphi * cls.dphi);
        chisqNdfPhiTmp = (cls.phi - phi) * (cls.phi - phi) * wtmp;
        if (chisqNdfPhiTmp < chisqNdfPhiFourth) chisqNdfPhiFourth = chisqNdfPhiTmp;

        // eta
        wtmp = 1 / (cls.deta * cls.deta);
        chisqNdfEtaTmp = (cls.eta - eta) * (cls.eta - eta) * wtmp;
        if (chisqNdfEtaTmp < chisqNdfEtaFourth) chisqNdfEtaFourth = chisqNdfEtaTmp;
      }


      //  createParticle( const Perigee* perigee, const FitQuality* fq, const TrackInfo* trackInfo, const TrackSummary* summary,
      //                                                   const std::vector<const Trk::TrackParameters*>& parameters,-
      //                                                   const std::vector< xAOD::ParameterPosition>& positions,
      //                                                   xAOD::ParticleHypothesis prtOrigin,
      //                                                   xAOD::TrackParticleContainer* container ) const {
      const Amg::Vector3D gp(0, 0, 0);
      const Trk::PerigeeSurface surface(gp);
      auto theta = asin(1 / cosh(eta)) * eta/fabs(eta);
      const Trk::Perigee perigee(0., 0., phi, theta, 0., surface);// d0, z0, phi, theta, q/p, surface
      const Trk::FitQuality *fq        = nullptr;
      const Trk::TrackInfo *trackInfo  = nullptr;
      const Trk::TrackSummary *summary = nullptr;
      const std::vector<const Trk::TrackParameters *> parameters;
      const std::vector<xAOD::ParameterPosition> positions;
      m_particleCreator->createParticle(&perigee, fq, trackInfo, summary, parameters, positions, xAOD::undefined, tpc);

      dec_trackPhiVec(*tpc->back()) = phi;
      dec_trackEtaVec(*tpc->back()) = eta;
      dec_trackPhiUncertVec(*tpc->back()) = phiUncert;
      dec_trackEtaUncertVec(*tpc->back()) = etaUncert;
      dec_trackChisqNdfPhiVec(*tpc->back()) = chisqNdfPhi;
      dec_trackChisqNdfEtaVec(*tpc->back()) = chisqNdfEta;
      dec_trackDEDX1Vec(*tpc->back()) = cls1.DEDX;
      dec_trackDEDX2Vec(*tpc->back()) = cls2.DEDX;
      dec_trackClusID1Vec(*tpc->back()) = icls1;
      dec_trackClusID2Vec(*tpc->back()) = icls2;

      dec_trackIsFakeVec(*tpc->back())      = isFakeVec.at(icls1) + isFakeVec.at(icls2);
      dec_trackIsAmbiguousVec(*tpc->back()) = isAmbiguousVec.at(icls1) + isAmbiguousVec.at(icls2);
      dec_trackIsSplitVec(*tpc->back())     = isSplitVec.at(icls1) + isSplitVec.at(icls2);
      dec_trackIsUsedVec(*tpc->back())      = isUsedVec.at(icls1) + isUsedVec.at(icls2);

      dec_trackChisqNdfPhiThirdVec(*tpc->back()) = chisqNdfPhiThird;
      dec_trackChisqNdfEtaThirdVec(*tpc->back()) = chisqNdfEtaThird;
      dec_trackChisqNdfPhiForthVec(*tpc->back()) = chisqNdfPhiFourth;
      dec_trackChisqNdfEtaForthVec(*tpc->back()) = chisqNdfEtaFourth;

      dec_trackHitL0dR001Vec(*tpc->back()) = iHitdR001[0];
      dec_trackHitL0dR010Vec(*tpc->back()) = iHitdR010[0];
      dec_trackHitL0dR020Vec(*tpc->back()) = iHitdR020[0];
      dec_trackHitL0dR040Vec(*tpc->back()) = iHitdR040[0];
      dec_trackHitL1dR001Vec(*tpc->back()) = iHitdR001[1];
      dec_trackHitL1dR010Vec(*tpc->back()) = iHitdR010[1];
      dec_trackHitL1dR020Vec(*tpc->back()) = iHitdR020[1];
      dec_trackHitL1dR040Vec(*tpc->back()) = iHitdR040[1];
      dec_trackHitL2dR001Vec(*tpc->back()) = iHitdR001[2];
      dec_trackHitL2dR010Vec(*tpc->back()) = iHitdR010[2];
      dec_trackHitL2dR020Vec(*tpc->back()) = iHitdR020[2];
      dec_trackHitL2dR040Vec(*tpc->back()) = iHitdR040[2];
      dec_trackHitL3dR001Vec(*tpc->back()) = iHitdR001[3];
      dec_trackHitL3dR010Vec(*tpc->back()) = iHitdR010[3];
      dec_trackHitL3dR020Vec(*tpc->back()) = iHitdR020[3];
      dec_trackHitL3dR040Vec(*tpc->back()) = iHitdR040[3];
    }  // cls2
  }  // cls1
  ATH_MSG_INFO(" # of two hits tracks = " << tpc->size());


  //std::cout<<m_assoTool<<std::endl;
  //m_assoTool->dump(std::cout);

  return StatusCode::SUCCESS;
}

float PixelStubletsMakerAlg::dEdxCluster(const InDet::PixelCluster *cluster, const xAOD::Vertex *vertex, double *cosalpha, float *charge) {
  // ------------------------------------------------------------------------------------
  // Loop over pixel hits on track, and calculate dE/dx:
  // ------------------------------------------------------------------------------------

  float Pixel_sensorthickness      = .025;  //250 microns Pixel Planars
  float IBL_3D_sensorthickness     = .023;  //230 microns IBL 3D
  float IBL_PLANAR_sensorthickness = .020;  // 200 microns IBL Planars
  float dEdxValue                  = -1;

  Identifier clusterId = cluster->identify();
  double locx          = cluster->localPosition().x();
  double locy          = cluster->localPosition().y();
  int bec              = m_pixelid->barrel_ec(clusterId);
  int layer            = m_pixelid->layer_disk(clusterId);
  int eta_module       = m_pixelid->eta_module(clusterId);
  int phi_module       = m_pixelid->phi_module(clusterId);

  TVector3 normal, momentum;
  momentum.SetX(cluster->globalPosition().x() - vertex->x());
  momentum.SetY(cluster->globalPosition().y() - vertex->y());
  momentum.SetZ(cluster->globalPosition().z() - vertex->z());

  StatusCode sc;
  sc = SetNormal(normal,
                 bec,
                 layer,
                 phi_module);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_WARNING("Wrong argument");
    return -1;
  }

  float dotProd = momentum.Dot(normal);
  *cosalpha     = fabs(dotProd / momentum.Mag());

#if 0 /* why continue ? check Ryu */
   if (std::abs(cosalpha) < .16)
      continue;
#endif

  *charge   = cluster->totalCharge();
  dEdxValue = (*charge) * (*cosalpha);

  //keep track if this is an ibl cluster with overflow
  //int iblOverflow = 0;

  if ((bec == 0) and (layer == 0)) {  // check if IBL
    //this is IBL layer -- @todo: check using proper service (safe against geometries)
    if (((eta_module >= -10 && eta_module <= -7) || (eta_module >= 6 && eta_module <= 9)) && (fabs(locy) < 10. && (locx > -8.33 && locx < 8.3))) {  //check if IBL 3D and good cluster selection
      dEdxValue *= (m_conversionfactor / IBL_3D_sensorthickness);
      //if (iblOverflow == 1) m_nUsedIBLOverflowHits++;
    } else if ((eta_module >= -6 && eta_module <= 5) && (fabs(locy) < 20. && (locx > -8.33 && locx < 8.3))) {  //check if IBL planar and good cluster selection
      dEdxValue *= (m_conversionfactor / IBL_PLANAR_sensorthickness);
      //if (iblOverflow == 1) m_nUsedIBLOverflowHits++;
    } else {
      dEdxValue = -1;
      *charge   = 0;
    }  //end check which IBL Module
  } else if (bec == 0 && fabs(locy) < 30. && ((locx > -8.20 && locx < -0.60) || (locx > 0.50 && locx < 8.10))) {
    dEdxValue *= (m_conversionfactor / Pixel_sensorthickness);
  } else if (std::abs(bec) == 2 && fabs(locy) < 30. && ((locx > -8.15 && locx < -0.55) || (locx > 0.55 && locx < 8.15))) {
    dEdxValue *= (m_conversionfactor / Pixel_sensorthickness);
  } else {
    dEdxValue = -1;
    *charge   = 0;
  }

  return dEdxValue;
}

/* ----------------------------------------------------------------------------------- */
// Callback function to update constants from database:
/* ----------------------------------------------------------------------------------- */

StatusCode PixelStubletsMakerAlg::update(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
  ATH_MSG_INFO("Updating constants for the PixelStubletsMakerAlg! ");

  // Callback function to update HT onset parameter cache when condDB data changes:
  for (std::list<std::string>::const_iterator key = keys.begin(); key != keys.end(); ++key)
    ATH_MSG_DEBUG("IOVCALLBACK for key " << *key << " number " << I);

  const char *storeName = "/PIXEL/PixdEdx";

  const AthenaAttributeList *atrlist;

  if (StatusCode::SUCCESS == m_detStore->retrieve(atrlist, storeName) && atrlist != 0) {
    std::string mystring = (*atrlist)["data"].data<std::string>();
    //std::cout << "mystring: " << mystring << std::endl;
    std::istringstream mystringstream(mystring);

    int fit_type;
    std::string fun_type;
    std::string bb_type;
    mystringstream >> fit_type;
    mystringstream >> fun_type;
    mystringstream >> bb_type;
    //std::cout << "types: " << fit_type << " " << fun_type << " " << bb_type << std::endl;
    m_mydedx->setFun(fun_type, bb_type);

    if (fit_type == 0) {
      for (int i = 0; i < NCLASS; i++) {
        double *param = new double[NPAR];
        for (int j = 0; j < NPAR; j++) {
          mystringstream >> param[j];
          //std::cout << "m_par[" << i << "][" << j << "]: " << m_par[i][j] << std::endl;
        }
        m_mydedx->setPar(i, param);
        delete[] param;
      }
      m_mydedx->setPosNeg(false);
    } else {
      for (int i = 0; i < 2 * NCLASS; i++) {
        double *param = new double[NPAR];
        for (int j = 0; j < NPAR; j++) {
          mystringstream >> param[j];
          //std::cout << "m_par[" << i << "][" << j << "]: " << m_par[i][j] << std::endl;
        }
        m_mydedx->setPar(i, param);
        delete[] param;
      }
      m_mydedx->setPosNeg(true);
    }

  } else {
    // m_log << MSG::ERROR << "Problem reading condDB object." << endreq;
    ATH_MSG_ERROR("Problem reading condDB object. -");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelStubletsMakerAlg::SetNormal(TVector3 &norm, int bec, int layer, int phimodule) {
  const int nlayers             = 4;
  const int nmodules[nlayers]   = {14, 22, 38, 52};
  const double phiInit[nlayers] = {0.35, 0, 0, 0.06};
  double phi;

  switch (bec) {
    case -2:
    case -4:
      norm.SetX(0);
      norm.SetY(0);
      norm.SetZ(-1);
      break;
    case +2:
    case +4:
      norm.SetX(0);
      norm.SetY(0);
      norm.SetZ(+1);
      break;
    case 0:
      phi = phiInit[layer] + 2 * TMath::Pi() / nmodules[layer] * phimodule;
      phi -= 20 * TMath::DegToRad();
      norm.SetX(TMath::Cos(phi));
      norm.SetY(TMath::Sin(phi));
      norm.SetZ(0);
      break;
    default:
      norm.SetX(0);
      norm.SetY(0);
      norm.SetZ(0);
      ATH_MSG_ERROR("Wrong bec=" << bec);
      return StatusCode::FAILURE;
      break;
  }
  return StatusCode::SUCCESS;
}

double CalDeltaR(double phi1, double phi2, double eta1, double eta2) {
  double deltaphi = (TMath::Abs(phi1 - phi2) > TMath::Pi()) ? 2.0 * TMath::Pi() - TMath::Abs(phi1 - phi2) : TMath::Abs(phi1 - phi2);
  double deltaeta = TMath::Abs(eta1 - eta2);
  return TMath::Sqrt(deltaphi * deltaphi + deltaeta * deltaeta);
}
