/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "PhotonVertexSelection/PhotonVertexSelectionTool.h"
#include "PhotonVertexSelection/PhotonPointingTool.h"
#include "PhotonVertexSelection/PhotonVertexHelpers.h"

// EDM includes
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// Framework includes
#include "PathResolver/PathResolver.h"
#include "IsolationCorrections/ShowerDepthTool.h"

// ROOT includes
#include "TMVA/Reader.h"

// std includes
#include <algorithm>

namespace CP {

  //____________________________________________________________________________
  PhotonVertexSelectionTool::PhotonVertexSelectionTool(const std::string &name)
  : asg::AsgTool(name)
  , m_pointingTool("CP::PhotonPointingTool/PhotonVertexSelection", this)
  , m_mva1(nullptr)
  , m_mva2(nullptr)
  , m_case(-1)
  {
    declareProperty("ConfigFileCase1",
        m_configFileCase1 = "PhotonVertexSelection/v1/DiphotonVertex_case1.weights.xml");
    declareProperty("ConfigFileCase2",
        m_configFileCase2 = "PhotonVertexSelection/v1/DiphotonVertex_case2.weights.xml");
    declareProperty("conversionPtCut"      , m_convPtCut       = 2e3 );    
    declareProperty("updatePointing", m_updatePointing=true, "Update pointing data?");
    declareProperty("vertexContainer", m_vertexContainerName = "PrimaryVertices");
    declareProperty("derivationPrefix", m_derivationPrefix = "");
  }

  //____________________________________________________________________________
  PhotonVertexSelectionTool::~PhotonVertexSelectionTool()
  {
    SafeDelete(m_mva1);
    SafeDelete(m_mva2);
  }

  //____________________________________________________________________________
  StatusCode PhotonVertexSelectionTool::initialize()
  {
    ATH_MSG_INFO("Initializing PhotonVertexSelectionTool...");

    // Retrieve PhotonPointingTool
    if (m_pointingTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Couldn't retrieve PhotonPointingTool.");
      return StatusCode::FAILURE;
    }

    // Get full path of configuration files for MVA
    m_configFileCase1  = PathResolverFindCalibFile(m_configFileCase1 );
    m_configFileCase2  = PathResolverFindCalibFile(m_configFileCase2);

    // Setup MVAs
    SafeDelete(m_mva1);
    m_mva1 = new TMVA::Reader("!Color:Silent");
    m_mva1->AddVariable("deltaZ := TMath::Min(abs(PrimaryVerticesAuxDyn.z-zCommon)/zCommonError,20)", &m_deltaZ         );
    m_mva1->AddVariable("deltaPhi := abs(deltaPhi(PrimaryVerticesAuxDyn.phi,egamma_phi))"           , &m_deltaPhi       );
    m_mva1->AddVariable("logSumpt := log10(PrimaryVerticesAuxDyn.sumPt)"                            , &m_sumPt          );
    m_mva1->AddVariable("logSumpt2 := log10(PrimaryVerticesAuxDyn.sumPt2)"                          , &m_sumPt2         );
    m_mva1->BookMVA    ("MLP method"                                                                , m_configFileCase1 );

    SafeDelete(m_mva2);
    m_mva2 = new TMVA::Reader("!Color:Silent");
    m_mva2->AddVariable("deltaZ := TMath::Min(abs(PrimaryVerticesAuxDyn.z-zCommon)/zCommonError,20)", &m_deltaZ        );
    m_mva2->AddVariable("deltaPhi := abs(deltaPhi(PrimaryVerticesAuxDyn.phi,egamma_phi))"           , &m_deltaPhi      );
    m_mva2->AddVariable("logSumpt := log10(PrimaryVerticesAuxDyn.sumPt)"                            , &m_sumPt         );
    m_mva2->AddVariable("logSumpt2 := log10(PrimaryVerticesAuxDyn.sumPt2)"                          , &m_sumPt2        );
    m_mva2->BookMVA    ("MLP method"                                                                , m_configFileCase2);

    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  std::vector<std::pair<const xAOD::Vertex*, float> >&
  PhotonVertexSelectionTool::getVertex(const xAOD::EgammaContainer &egammas, bool ignoreConv)
  {
    const xAOD::Vertex *vertex = nullptr;
    if (getVertex(egammas, vertex, ignoreConv).isSuccess()) {
      std::sort(m_vertexMLP.begin(), m_vertexMLP.end(), sortMLP);
    }

    return m_vertexMLP;
  }

  //____________________________________________________________________________
  StatusCode PhotonVertexSelectionTool::getVertex(const xAOD::EgammaContainer &egammas,
                                                  const xAOD::Vertex* &prime_vertex,
                                                  bool ignoreConv)
  {
    m_vertexMLP.clear();
    m_case = -1;
    const xAOD::PhotonContainer *photons = dynamic_cast<const xAOD::PhotonContainer*>(&egammas);

    // Retrieve PV collection from TEvent
    const xAOD::VertexContainer* vertices = 0;
    if (evtStore()->retrieve(vertices, m_vertexContainerName).isFailure()) {
      ATH_MSG_WARNING("Couldn't retrieve " << m_vertexContainerName << " from TEvent, returning nullptr.");
      prime_vertex = nullptr;
      return StatusCode::FAILURE;
    }

    // Check if a conversion photon has a track attached to a primary/pileup vertex
    if (!ignoreConv) { 
      prime_vertex = getPrimaryVertexFromConv(photons);
      if (prime_vertex != nullptr) {
        m_case = 0;
        m_vertexMLP.push_back(std::make_pair(prime_vertex, 0.));
        return StatusCode::SUCCESS;
      }
    }

    // Update calo pointing auxdata for photons
    // FIXME: Remove once variables properly included in derivations
    if (m_updatePointing and m_pointingTool->updatePointingAuxdata(egammas).isFailure()) {
      ATH_MSG_WARNING("Couldn't update photon calo pointing auxdata, returning hardest vertex.");
      prime_vertex = xAOD::PVHelpers::getHardestVertex(vertices);
      return StatusCode::FAILURE;
    }
  
    // Get the EventInfo
    const xAOD::EventInfo *eventInfo = nullptr;
    if (evtStore()->retrieve(eventInfo, "EventInfo").isFailure()) {
      ATH_MSG_WARNING("Couldn't retrieve EventInfo from TEvent, returning hardest vertex.");
      prime_vertex = xAOD::PVHelpers::getHardestVertex(vertices);
      return StatusCode::FAILURE;
    }

    // Find the common z-position from beam / photon pointing information
    std::pair<float, float> zCommon = xAOD::PVHelpers::getZCommonAndError(eventInfo, &egammas, m_convPtCut);

    // If there are any silicon conversions passing selection, use MVA1
    TMVA::Reader *reader = m_mva2;
    m_case = 2;
    if (!ignoreConv && photons) {
      for (auto photon: *photons) {
        if (!photon)
        {
          ATH_MSG_WARNING("Null pointer to photon");
          return StatusCode::FAILURE;
        }
        if (xAOD::PVHelpers::passConvSelection(photon,
                                               m_convPtCut))
        {
          reader = m_mva1;
          m_case = 1;
        }
      }
    }
    ATH_MSG_DEBUG("Case: " << m_case);

    // Vector sum of photons
    TLorentzVector vegamma = getEgammaVector(&egammas);

    // Loop over vertices and find best candidate
    float mlp = 0.0, mlp_max = -99999.0;
    for (auto vertex: *vertices) {

      // Skip dummy vertices
      if (vertex->vertexType() != xAOD::VxType::VertexType::PriVtx and
          vertex->vertexType() != xAOD::VxType::VertexType::PileUp) continue;

      // Get momentum vector of vertex
      TLorentzVector vmom = xAOD::PVHelpers::getVertexMomentum(vertex, true, m_derivationPrefix);

      // Set input variables for MVA
      static SG::AuxElement::ConstAccessor<float> sumPt(m_derivationPrefix + "sumPt");
      if (sumPt.isAvailable(*vertex)) {
        m_sumPt = log10(sumPt(*vertex));
      } else {
        m_sumPt  = log10(xAOD::PVHelpers::getVertexSumPt(vertex));
      }

      static SG::AuxElement::ConstAccessor<float> sumPt2(m_derivationPrefix + "sumPt2");
      if (sumPt2.isAvailable(*vertex))
        m_sumPt2 = log10(sumPt2(*vertex));
      else
        m_sumPt2 = log10(xAOD::PVHelpers::getVertexSumPt(vertex, 2));

      m_deltaPhi     = fabs(vmom.DeltaPhi(vegamma));
      m_deltaZ       = fabs((zCommon.first - vertex->z())/zCommon.second);
      ATH_MSG_VERBOSE("log(sumPt): " << m_sumPt <<
                      " log(sumPt2): " << m_sumPt2 <<
                      " deltaPhi: " << m_deltaPhi <<
                      " deltaZ: " << m_deltaZ);

      // Skip vertices above 10 sigma from pointing or 15 sigma from conversion (HPV)
      if ((reader == m_mva2 && m_deltaZ > 10) ||
          (reader == m_mva1 && m_deltaZ > 15)) {
        mlp = -9999.0;
      } else {
        // Get likelihood probability from MVA
        mlp = reader->EvaluateMVA("MLP method");
      }
      ATH_MSG_VERBOSE("MVA output: " << mlp);

      m_vertexMLP.push_back(std::make_pair(vertex, mlp));

      // Keep track of maximal likelihood vertex
      if (mlp > mlp_max) {
        mlp_max = mlp;
        prime_vertex = vertex;
      }

    } // loop over vertices

    if (mlp_max <= -9999.0) {
      ATH_MSG_DEBUG("No good vertex candidates from pointing, returning hardest vertex.");
      prime_vertex = xAOD::PVHelpers::getHardestVertex(vertices);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  bool PhotonVertexSelectionTool::sortMLP(const std::pair<const xAOD::Vertex*, float> &a,
                                          const std::pair<const xAOD::Vertex*, float> &b)
  { return a.second > b.second; }

  //____________________________________________________________________________
  const xAOD::Vertex* PhotonVertexSelectionTool::getPrimaryVertexFromConv(const xAOD::PhotonContainer *photons) const
  {
    if (photons == nullptr) {
      ATH_MSG_WARNING("Passed nullptr photon container, returning nullptr vertex from getPrimaryVertexFromConv");
      return nullptr;
    }

    std::vector<const xAOD::Vertex*> vertices;
    const xAOD::Vertex *conversionVertex = nullptr, *primary = nullptr;
    const xAOD::TrackParticle *tp = nullptr;
    size_t NumberOfTracks = 0;

    for (auto photon: *photons) {
      conversionVertex = photon->vertex();
      if (conversionVertex == nullptr) continue;

      NumberOfTracks = conversionVertex->nTrackParticles();
      for (size_t i = 0; i < NumberOfTracks; ++i) {
        // Get trackParticle in GSF collection
        const auto *gsfTp = conversionVertex->trackParticle(i);
        if (gsfTp == nullptr) continue;
        if (!xAOD::PVHelpers::passConvSelection(*conversionVertex, i, m_convPtCut)) continue;
        
        // Get trackParticle in InDet collection
        tp = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(gsfTp);
        if (tp == nullptr) continue;


        primary = tp->vertex();
        if (primary == nullptr) continue;

        if (primary->vertexType() == xAOD::VxType::VertexType::PriVtx ||
            primary->vertexType() == xAOD::VxType::VertexType::PileUp) {
          if (std::find(vertices.begin(), vertices.end(), primary) == vertices.end()) {
            vertices.push_back(primary);
            continue;
          }
        }

      }
    }

    if (vertices.size() > 0) {
      if (vertices.size() > 1)
        ATH_MSG_WARNING("Photons associated to different vertices! Returning lead photon association.");
      return vertices[0];
    }

    return nullptr;
  }

  //____________________________________________________________________________
  const CP::IPhotonPointingTool* PhotonVertexSelectionTool::photonPointingTool() const
  {
    return &*m_pointingTool;
  }

  //____________________________________________________________________________
  TLorentzVector PhotonVertexSelectionTool::getEgammaVector(const xAOD::EgammaContainer *egammas) const
  {
    TLorentzVector v, v1;
    const xAOD::CaloCluster *cluster = nullptr;
    for (auto egamma: *egammas) {
      cluster = egamma->caloCluster();
      if (cluster == nullptr) {
        ATH_MSG_WARNING("No cluster associated to egamma, not adding to 4-vector.");
        continue;
      }

      v1.SetPtEtaPhiM(egamma->e()/cosh(cluster->etaBE(2)),
                      cluster->etaBE(2),
                      cluster->phiBE(2),
                      0.0);
      v += v1;
    }
    return v;
  }

} // namespace CP
