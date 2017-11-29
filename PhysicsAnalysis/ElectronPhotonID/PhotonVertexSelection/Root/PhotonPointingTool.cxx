/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "PhotonVertexSelection/PhotonPointingTool.h"
#include "PhotonVertexSelection/PhotonVertexHelpers.h"

// EDM includes
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/EgammaDefs.h"

// Framework includes
#include "IsolationCorrections/ShowerDepthTool.h"
#include "PathResolver/PathResolver.h"

// ROOT includes
#include "TFile.h"
#include "TH1F.h"

// std includes
#include <algorithm>

namespace CP {

  //____________________________________________________________________________
  SG::AuxElement::Decorator<float> PhotonPointingTool::zvertex("zvertex");
  SG::AuxElement::Decorator<float> PhotonPointingTool::errz("errz");
  SG::AuxElement::Decorator<float> PhotonPointingTool::HPV_zvertex("HPV_zvertex");
  SG::AuxElement::Decorator<float> PhotonPointingTool::HPV_errz("HPV_errz");

  //____________________________________________________________________________
  PhotonPointingTool::PhotonPointingTool(const std::string &name)
  : asg::AsgTool(name)
  , m_showerTool(nullptr)
  , m_zCorrection(nullptr)
  { 
    declareProperty("isSimulation", m_isMC);
    declareProperty("zOscillationFileMC", m_zOscFileMC ="PhotonVertexSelection/v1/pointing_correction_mc.root");
    declareProperty("zOscillationFileData", m_zOscFileData ="PhotonVertexSelection/v1/pointing_correction_data.root");
  }

  //____________________________________________________________________________
  PhotonPointingTool::~PhotonPointingTool()
  {
    SafeDelete(m_showerTool);
    SafeDelete(m_zCorrection);
  }

  //____________________________________________________________________________
  StatusCode PhotonPointingTool::initialize()
  {
    ATH_MSG_INFO("Initializing PhotonPointingTool...");

    // Shower depth tool
    m_showerTool = new CP::ShowerDepthTool();
    if (!m_showerTool->initialize()) {
      ATH_MSG_ERROR("Couldn't initialize ShowerDepthTool, failed to initialize.");
      return StatusCode::FAILURE;
    }
    

    // Get the z-oscillation correction histogram
    // FIXME: The files need to go to calib area
    #if ( defined(XAOD_STANDALONE) )
    {
        // AnalysisRelease: determine if this is data or MC
        // Cannot load the eventInfo before the first event in athena    
      const xAOD::EventInfo *eventInfo = nullptr;
      if (evtStore()->retrieve(eventInfo, "EventInfo").isFailure()) {
        ATH_MSG_WARNING("Couldn't retrieve EventInfo from TEvent, failed to initialize.");
        return StatusCode::FAILURE;
      }    
      m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
    }
    #endif
    std::string filepath = PathResolverFindCalibFile(m_isMC ? m_zOscFileMC :
                                                              m_zOscFileData);
    TFile *file = TFile::Open(filepath.c_str(), "READ");

    if (file == nullptr) {
      ATH_MSG_WARNING("Couldn't find file for z-correction: " << filepath.c_str());
      ATH_MSG_WARNING("Failed to initialize.");
      return StatusCode::FAILURE;
    }

    TH1F *temp = nullptr;
    file->GetObject("dz_trk_pointing_vs_etas2", temp);

    if (temp == nullptr) {
      ATH_MSG_WARNING("Couldn't find 'dz_trk_pointing_vs_etas2' histogram in file: " << filepath.c_str());
      ATH_MSG_WARNING("Failed to initialize.");
      return StatusCode::FAILURE;
    }

    bool status = TH1::AddDirectoryStatus();
    TH1::AddDirectory(false);
    m_zCorrection = dynamic_cast<TH1F*>(temp->Clone("zCorrection"));
    SafeDelete(file);
    TH1::AddDirectory(status);
    
    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  StatusCode PhotonPointingTool::updatePointingAuxdata(const xAOD::EgammaContainer &egammas) const
  {
    // Loop over photons and add calo pointing auxdata
    std::pair<float, float> result;
    for (auto egamma: egammas) {
      // Get calo pointing variables
      result = getCaloPointing(egamma);

      // Set photon auxdata with new value
      zvertex(*egamma) = result.first;
      errz(*egamma)    = result.second;

      // Get conv pointing variables
      if (egamma->type() == xAOD::Type::Photon) {
        const xAOD::Egamma *eg     = static_cast<const xAOD::Egamma*>(egamma);
        const xAOD::Photon *photon = dynamic_cast<const xAOD::Photon*>(eg);

        if (photon && xAOD::EgammaHelpers::numberOfSiTracks(photon))
          result = getConvPointing(photon);
      }

      // Set photon auxdata with new value
      HPV_zvertex(*egamma) = result.first;
      HPV_errz(*egamma)    = result.second;

    }

    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  float PhotonPointingTool::getCorrectedZ(float zPointing, float etas2) const
  {
    if (fabs(etas2) < 1.37) return zPointing;
    return zPointing - m_zCorrection->Interpolate(etas2);
  }

  //____________________________________________________________________________
  std::pair<float, float> PhotonPointingTool::getCaloPointing(const xAOD::Egamma *egamma) const
  {
    if (egamma == nullptr) {
      ATH_MSG_WARNING("Passed Egamma was a nullptr, returning (0,0).");
      return std::make_pair(0,0);
    }

    // Get the EventInfo
    const xAOD::EventInfo *eventInfo = nullptr;
    if (evtStore()->retrieve(eventInfo, "EventInfo").isFailure()) {
      ATH_MSG_WARNING("Couldn't retrieve EventInfo from TEvent, egamma won't be decorated.");
      return std::make_pair(0, 0);
    }

    // Beam parameters
    double d_beamSpot   = hypot(eventInfo->beamPosY(), eventInfo->beamPosX());
    double phi_beamSpot = atan2(eventInfo->beamPosY(), eventInfo->beamPosX());

    // Photon variables
    const xAOD::CaloCluster *cluster = egamma->caloCluster();
    if (cluster == nullptr) {
      ATH_MSG_WARNING("Couldn't retrieve CaloCluster from Photon, egammas won't be decorated.");
      return std::make_pair(0, 0);
    }

    float cl_e     = cluster->e();
    float etas1    = cluster->etaBE(1);
    float etas2    = cluster->etaBE(2);
    float phis2    = cluster->phiBE(2);
    float cl_theta = 2.0*atan(exp(-1.0*cluster->eta()));

    // Shower depths
    std::pair<float, float> RZ1 = m_showerTool->getRZ(etas1, 1);
    std::pair<float, float> RZ2 = m_showerTool->getRZ(etas2, 2);

    // Calo cluster pointing calculation
    double r0_with_beamSpot = d_beamSpot*cos(phis2 - phi_beamSpot);

    float zvertex = 0, errz = 0;
    if (true) { // FIXME Was only for non AuthorFwd egamma? Can only find AuthorFwdElectron now...
      zvertex = (RZ1.second*(RZ2.first - r0_with_beamSpot) - RZ2.second*(RZ1.first-r0_with_beamSpot)) / (RZ2.first - RZ1.first);
      zvertex = getCorrectedZ(zvertex, etas2);

      errz    = 0.5*(RZ2.first + RZ1.first)*(0.060/sqrt(cl_e*0.001)) / (sin(cl_theta)*sin(cl_theta));
    }

    return std::make_pair(zvertex, errz);
  }

  //____________________________________________________________________________
  std::pair<float, float> PhotonPointingTool::getConvPointing(const xAOD::Photon *photon) const
  {
    if (photon == nullptr) {
      ATH_MSG_WARNING("Passed Egamma was a nullptr, returning (0,0).");
      return std::make_pair(0,0);
    }

    // Get the EventInfo
    const xAOD::EventInfo *eventInfo = nullptr;
    if (evtStore()->retrieve(eventInfo, "EventInfo").isFailure()) {
      ATH_MSG_WARNING("Couldn't retrieve EventInfo from TEvent, photons won't be decorated.");
      return std::make_pair(0, 0);
    }

    // Beam parameters
    double d_beamSpot   = hypot(eventInfo->beamPosY(), eventInfo->beamPosX());
    double phi_beamSpot = atan2(eventInfo->beamPosY(), eventInfo->beamPosX());

    // Photon variables
    const xAOD::CaloCluster *cluster = photon->caloCluster();
    if (cluster == nullptr) {
      ATH_MSG_WARNING("Couldn't retrieve CaloCluster from Photon, photons won't be decorated.");
      return std::make_pair(0, 0);
    }

    float etas1    = cluster->etaBE(1);
    float phis2    = cluster->phiBE(2);

    const xAOD::Vertex *conv = photon->vertex();
    if (cluster == nullptr) {
      ATH_MSG_WARNING("Couldn't retrieve conversion Vertex from Photon, photons won't be decorated.");
      return std::make_pair(0, 0);
    }

    float conv_x = conv->x();
    float conv_y = conv->y();
    float conv_z = conv->z();

    // Shower depths
    std::pair<float, float> RZ1 = m_showerTool->getRZ(etas1, 1);

    // Photon conversion
    double conv_r = hypot(conv_x, conv_y);

    // HPV pointing calculation
    double phi_Calo         = atan2(sin(phis2), cos(phis2));
    double r0_with_beamSpot = d_beamSpot*cos(phi_Calo - phi_beamSpot);
    
    float zvertex = (RZ1.second*(conv_r - r0_with_beamSpot) - conv_z*(RZ1.first - r0_with_beamSpot)) / (conv_r - RZ1.first);

    float dist_vtx_to_conv = hypot(conv_r - r0_with_beamSpot, conv_z - zvertex);
    float dist_conv_to_s1 = hypot(RZ1.first - conv_r, RZ1.second - conv_z);

    float error_etaS1 = 0.001; // FIXME is there a tool which provides a better value?
    float errz = 0.0;

    if ((cluster->inBarrel() && !cluster->inEndcap()) ||
        (cluster->inBarrel() &&  cluster->inEndcap() && cluster->eSample(CaloSampling::EMB1) > cluster->eSample(CaloSampling::EME1))) {
      // Barrel case
      float error_Z_Calo_1st_Sampling_barrel = error_etaS1*RZ1.first*fabs(cosh(etas1));
      errz = error_Z_Calo_1st_Sampling_barrel*dist_vtx_to_conv/dist_conv_to_s1;
    } else { 
      // Endcap case
      float error_R_Calo_1st_Sampling_endcap = error_etaS1*cosh(etas1)*RZ1.first*RZ1.first/fabs(RZ1.second);
      errz = error_R_Calo_1st_Sampling_endcap*fabs(sinh(etas1))*dist_vtx_to_conv/dist_conv_to_s1;
    }

    return std::make_pair(zvertex, errz);
  }

  //____________________________________________________________________________
  float PhotonPointingTool::getCorrectedEta(const xAOD::CaloCluster &cluster, float PVz) const
  {
    // Get R/Z positions from sampling layer one eta
    std::pair<float, float> RZ1 = m_showerTool->getRZ(cluster.etaBE(1), 1);

    // Return corrected eta from new vertex
    return asinh((RZ1.second - PVz)/RZ1.first);
  }

  //____________________________________________________________________________
  StatusCode PhotonPointingTool::correctPrimaryVertex(xAOD::Egamma &egamma, float PVz) const
  {
    const xAOD::CaloCluster *cluster = egamma.caloCluster();
    if (cluster == nullptr) {
      ATH_MSG_WARNING("Couldn't retrieve cluster from Egamma, not correcting object.");
      return StatusCode::FAILURE;
    }

    float eta_corr = getCorrectedEta(*cluster, PVz);
    
    // Set corrected 4-vector
    egamma.setP4(egamma.e()/cosh(eta_corr),
                 eta_corr,
                 egamma.phi(),
                 egamma.m());

    return StatusCode::SUCCESS;
  }
  
  //____________________________________________________________________________
  TLorentzVector PhotonPointingTool::getCorrectedLV(const xAOD::Egamma &egamma, float PVz) const
  {
    TLorentzVector vec = egamma.p4();

    const xAOD::CaloCluster *cluster = egamma.caloCluster();
    if (cluster == nullptr) {
      ATH_MSG_WARNING("Couldn't retrieve cluster from Egamma, returning uncorrected object");
      return vec;
    }

    float eta_corr = getCorrectedEta(*cluster, PVz);
    
    // Set corrected 4-vector
    vec.SetPtEtaPhiM(egamma.e()/cosh(eta_corr),
                     eta_corr,
                     egamma.phi(),
                     egamma.m());

    return vec;
  }
  
  //____________________________________________________________________________
  float PhotonPointingTool::getCorrectedMass(const xAOD::EgammaContainer &egammas, float PVz) const
  {
    TLorentzVector v;
    for (auto eg : egammas) {
      if (!eg) {
        ATH_MSG_WARNING("Null pointer to egamma object, skipping it in mass calculation");
        continue;
      }
      v += getCorrectedLV(*eg, PVz);
    }
    return v.M();  
  }

} // namespace CP
