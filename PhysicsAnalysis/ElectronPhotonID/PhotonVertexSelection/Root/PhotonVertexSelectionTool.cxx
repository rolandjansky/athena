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
  , m_vertices(nullptr)
  , m_acc_sumPt(nullptr)
  , m_acc_sumPt2(nullptr)
  , m_acc_deltaPhi(nullptr)
  , m_acc_deltaZ(nullptr)
  , m_mva1(nullptr)
  , m_mva2(nullptr)
  , m_case(-1)
  , m_varsDecorated(false)
  , m_fail(FailType::Unkown)
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
  PhotonVertexSelectionTool::~PhotonVertexSelectionTool() = default;

  //____________________________________________________________________________
  StatusCode PhotonVertexSelectionTool::initialize()
  {
    ATH_MSG_INFO("Initializing PhotonVertexSelectionTool...");

    // Retrieve PhotonPointingTool
    if (m_pointingTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Couldn't retrieve PhotonPointingTool.");
      return StatusCode::FAILURE;
    }

    // Initialize accessors
    m_acc_sumPt = std::make_unique<const SG::AuxElement::ConstAccessor<float> >(m_derivationPrefix + "sumPt");
    m_acc_sumPt2 = std::make_unique<const SG::AuxElement::ConstAccessor<float> >(m_derivationPrefix + "sumPt2");
    m_acc_deltaPhi = std::make_unique<const SG::AuxElement::ConstAccessor<float> >(m_derivationPrefix + "fabs_deltaPhi");
    m_acc_deltaZ = std::make_unique<const SG::AuxElement::ConstAccessor<float> >(m_derivationPrefix + "fabs_deltaZ");


    // Get full path of configuration files for MVA
    m_configFileCase1  = PathResolverFindCalibFile(m_configFileCase1 );
    m_configFileCase2  = PathResolverFindCalibFile(m_configFileCase2);

    // Setup MVAs
    m_mva1 = std::make_shared<TMVA::Reader>("!Color:Silent");
    m_mva1->AddVariable("deltaZ := TMath::Min(abs(PrimaryVerticesAuxDyn.z-zCommon)/zCommonError,20)", &m_deltaZ         );
    m_mva1->AddVariable("deltaPhi := abs(deltaPhi(PrimaryVerticesAuxDyn.phi,egamma_phi))"           , &m_deltaPhi       );
    m_mva1->AddVariable("logSumpt := log10(PrimaryVerticesAuxDyn.sumPt)"                            , &m_sumPt          );
    m_mva1->AddVariable("logSumpt2 := log10(PrimaryVerticesAuxDyn.sumPt2)"                          , &m_sumPt2         );
    m_mva1->BookMVA    ("MLP method"                                                                , m_configFileCase1 );

    m_mva2 = std::make_shared<TMVA::Reader>("!Color:Silent");
    m_mva2->AddVariable("deltaZ := TMath::Min(abs(PrimaryVerticesAuxDyn.z-zCommon)/zCommonError,20)", &m_deltaZ        );
    m_mva2->AddVariable("deltaPhi := abs(deltaPhi(PrimaryVerticesAuxDyn.phi,egamma_phi))"           , &m_deltaPhi      );
    m_mva2->AddVariable("logSumpt := log10(PrimaryVerticesAuxDyn.sumPt)"                            , &m_sumPt         );
    m_mva2->AddVariable("logSumpt2 := log10(PrimaryVerticesAuxDyn.sumPt2)"                          , &m_sumPt2        );
    m_mva2->BookMVA    ("MLP method"                                                                , m_configFileCase2);

    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  StatusCode PhotonVertexSelectionTool::decorateInputs(const xAOD::EgammaContainer &egammas){

    m_varsDecorated = false;
    m_fail = FailType::NoFail;

    // Retrieve PV collection from TEvent
    if (evtStore()->retrieve(m_vertices, m_vertexContainerName).isFailure()) { // TODO: forbidden in Rel22, to be fixed
      ATH_MSG_ERROR("Couldn't retrieve " << m_vertexContainerName << " from TEvent");
      m_fail = FailType::NoVxCont;
      return StatusCode::FAILURE;
    }

    // Get the EventInfo
    const xAOD::EventInfo *eventInfo = nullptr;
    if (evtStore()->retrieve(eventInfo, "EventInfo").isFailure()) {
      ATH_MSG_ERROR("Couldn't retrieve EventInfo from TEvent");
      m_fail = FailType::NoEventInfo;
      return StatusCode::FAILURE;
    }

    // Update calo pointing auxdata for photons
    // FIXME: Remove once variables properly included in derivations
    if (m_updatePointing and m_pointingTool->updatePointingAuxdata(egammas).isFailure()) {
      ATH_MSG_ERROR("Couldn't update photon calo pointing auxdata");
      m_fail = FailType::FailPointing;
    }

    const SG::AuxElement::Decorator<float> sumPt2(m_derivationPrefix + "sumPt2");
    const SG::AuxElement::Decorator<float> sumPt(m_derivationPrefix + "sumPt");
    const SG::AuxElement::Decorator<float> deltaPhi(m_derivationPrefix + "fabs_deltaPhi");
    const SG::AuxElement::Decorator<float> deltaZ(m_derivationPrefix + "fabs_deltaZ");

    // Find the common z-position from beam / photon pointing information
    std::pair<float, float> zCommon = xAOD::PVHelpers::getZCommonAndError(eventInfo, &egammas, m_convPtCut);

    // Vector sum of photons
    TLorentzVector vegamma = getEgammaVector(&egammas);

    for (xAOD::Vertex* vertex: *m_vertices) {

      // Skip dummy vertices
      if (vertex->vertexType() != xAOD::VxType::VertexType::PriVtx and
          vertex->vertexType() != xAOD::VxType::VertexType::PileUp) continue;

      // Set input variables for MVA
      sumPt(*vertex) = xAOD::PVHelpers::getVertexSumPt(vertex, 1, false);

      if (not sumPt2.isAvailable(*vertex)) {
        sumPt2(*vertex) = xAOD::PVHelpers::getVertexSumPt(vertex, 2);
      }

      // Get momentum vector of vertex
      TLorentzVector vmom = xAOD::PVHelpers::getVertexMomentum(vertex, true, m_derivationPrefix);

      deltaPhi(*vertex) = (m_fail != FailType::FailEgamVect) ? fabs(vmom.DeltaPhi(vegamma)) : -999.;      
      deltaZ(*vertex) = fabs((zCommon.first - vertex->z())/zCommon.second);

    } // loop over vertices

    m_varsDecorated = true;
    ATH_MSG_DEBUG("DecorateInputs exit code "<< m_fail);
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

    // re-initialize for next getVertex call
    m_varsDecorated = false;

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

    if (not m_varsDecorated){
      if (!decorateInputs(egammas).isSuccess()){
        return StatusCode::FAILURE;
      }
    }

    // Check if a conversion photon has a track attached to a primary/pileup vertex
    if (!ignoreConv) { 
      prime_vertex = getPrimaryVertexFromConv(photons);
      if (prime_vertex != nullptr) {
        m_case = 0;
        m_fail = FailType::MatchedTrack;
        m_vertexMLP.push_back(std::make_pair(prime_vertex, 5.));
        return StatusCode::SUCCESS;
      }
    }

    if (m_fail){
      ATH_MSG_ERROR("Returning hardest vertex. Fail detected (type="<< m_fail <<")");
      m_vertexMLP.push_back(std::make_pair(xAOD::PVHelpers::getHardestVertex(m_vertices), 10.));
      return StatusCode::FAILURE;
    }

    // If there are any silicon conversions passing selection, use MVA1
    std::shared_ptr<TMVA::Reader> reader(m_mva2);
    m_case = 2;
    if (!ignoreConv && photons) {
      for (const xAOD::Photon* photon: *photons) {
        if (!photon){
          ATH_MSG_ERROR("Null pointer to photon");
          return StatusCode::FAILURE;
        }
        
        if (xAOD::PVHelpers::passConvSelection(photon, m_convPtCut)){
          reader = m_mva1;
          m_case = 1;
        }
      }
    }
    ATH_MSG_DEBUG("Case: " << m_case);

    // Loop over vertices and find best candidate
    float mlp = 0.0, mlp_max = -99999.0;
    for (xAOD::Vertex* vertex: *m_vertices) {

      // Skip dummy vertices
      if (vertex->vertexType() != xAOD::VxType::VertexType::PriVtx and
          vertex->vertexType() != xAOD::VxType::VertexType::PileUp) continue;

      m_sumPt     = log10((*m_acc_sumPt)(*vertex)); // TODO: forbidden in Rel22, to be fixed
      m_sumPt2    = log10((*m_acc_sumPt2)(*vertex)); // TODO: forbidden in Rel22, to be fixed
      m_deltaPhi  = (*m_acc_deltaPhi)(*vertex); // TODO: forbidden in Rel22, to be fixed
      m_deltaZ    = (*m_acc_deltaZ)(*vertex); // TODO: forbidden in Rel22, to be fixed

      ATH_MSG_VERBOSE(" log(sumPt): "  << m_sumPt    <<
                      " log(sumPt2): " << m_sumPt2   <<
                      " deltaPhi: "    << m_deltaPhi <<
                      " deltaZ: "      << m_deltaZ);

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
      ATH_MSG_ERROR("No good vertex candidates from pointing, returning hardest vertex.");
      m_fail = FailType::NoGdCandidate;
      m_vertexMLP.clear();
      m_vertexMLP.push_back(std::make_pair(xAOD::PVHelpers::getHardestVertex(m_vertices), 20.));
      return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("getVertex case "<<m_case << " exit code "<< m_fail);
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

    for (const xAOD::Photon* photon: *photons) {
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
  TLorentzVector PhotonVertexSelectionTool::getEgammaVector(const xAOD::EgammaContainer *egammas)
  {
    TLorentzVector v, v1;
    const xAOD::CaloCluster *cluster = nullptr;
    for (const xAOD::Egamma* egamma: *egammas) {
      if (egamma == nullptr) {
        ATH_MSG_WARNING("No egamma object to get four vector");
        m_fail = FailType::FailEgamVect;
        continue;

      }
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
