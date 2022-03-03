/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

  // helper function to get the vertex of a track
  const xAOD::Vertex* getVertexFromTrack(const xAOD::TrackParticle* track,
                                         const xAOD::VertexContainer* vertices)
  {
    const xAOD::Vertex* vtxWithLargestWeight = nullptr;
    float largestWeight = 0;

    for (const auto *vtx : *vertices) {
      //Search for vertex linked to this track
      const auto& trkLinks=vtx->trackParticleLinks();
      const size_t nTrackLinks=trkLinks.size();
      for (unsigned i=0;i<nTrackLinks;++i) {
        if (trkLinks[i].isValid() && *(trkLinks[i]) == track) {//ptr comparison
          if( vtx->trackWeights()[i] > largestWeight ){
            vtxWithLargestWeight = vtx;
            largestWeight = vtx->trackWeights()[i];
          }
        }
      }
    }

    return vtxWithLargestWeight;
  }

  //____________________________________________________________________________
  PhotonVertexSelectionTool::PhotonVertexSelectionTool(const std::string &name)
  : asg::AsgTool(name)
  {
    declareProperty("ConfigFileCase1",
        m_configFileCase1 = "PhotonVertexSelection/v1/DiphotonVertex_case1.weights.xml");
    declareProperty("ConfigFileCase2",
        m_configFileCase2 = "PhotonVertexSelection/v1/DiphotonVertex_case2.weights.xml");
    declareProperty("conversionPtCut"      , m_convPtCut       = 2e3 );    
    declareProperty("updatePointing", m_updatePointing=true, "Update pointing data?");
    declareProperty("derivationPrefix", m_derivationPrefix = "");
  }

  //____________________________________________________________________________
  PhotonVertexSelectionTool::~PhotonVertexSelectionTool()
  {

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
    m_configFileCase1  = PathResolverFindCalibFile( m_configFileCase1 );
    m_configFileCase2  = PathResolverFindCalibFile( m_configFileCase2 );
    
    // Setup MVAs
    std::vector<std::string> var_names = { "deltaZ := TMath::Min(abs(PrimaryVerticesAuxDyn.z-zCommon)/zCommonError,20)", 
                                           "deltaPhi := abs(deltaPhi(PrimaryVerticesAuxDyn.phi,egamma_phi))"           , 
                                           "logSumpt := log10(PrimaryVerticesAuxDyn.sumPt)"                            , 
                                           "logSumpt2 := log10(PrimaryVerticesAuxDyn.sumPt2)" }; 
 
    auto mva1 = new TMVA::Reader(var_names, "!Silent:Color");
    mva1->BookMVA    ("MLP method"                                                                , m_configFileCase1 );
    m_mva1 = std::unique_ptr<TMVA::Reader>( std::move(mva1) );
    
    auto mva2 = std::make_unique<TMVA::Reader>(var_names, "!Silent:Color");
    mva2->BookMVA    ("MLP method"                                                                , m_configFileCase2);
    m_mva2 = std::unique_ptr<TMVA::Reader>( std::move(mva2) );

    ATH_CHECK( m_eventInfo.initialize() );
    ATH_CHECK( m_vertexContainer.initialize() );

    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  StatusCode PhotonVertexSelectionTool::decorateInputs(const xAOD::EgammaContainer &egammas, FailType* failType) const{
    auto fail = FailType::NoFail;
    
    // Update calo pointing auxdata for photons
    // FIXME: Remove once variables properly included in derivations
    if (m_updatePointing and m_pointingTool->updatePointingAuxdata(egammas).isFailure()) {
      ATH_MSG_INFO("Couldn't update photon calo pointing auxdata");
      fail = FailType::FailPointing;
    }

    static const SG::AuxElement::Decorator<float> sumPt2(m_derivationPrefix + "sumPt2");
    static const SG::AuxElement::Decorator<float> sumPt(m_derivationPrefix + "sumPt");
    static const SG::AuxElement::Decorator<float> deltaPhi(m_derivationPrefix + "deltaPhi");
    static const SG::AuxElement::Decorator<float> deltaZ(m_derivationPrefix + "deltaZ");
   
    // Get the EventInfo
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);
   
    // Find the common z-position from beam / photon pointing information
    std::pair<float, float> zCommon = xAOD::PVHelpers::getZCommonAndError(&*eventInfo, &egammas, m_convPtCut);
    // Vector sum of photons
    TLorentzVector vegamma = getEgammaVector(&egammas, fail);

    // Retrieve PV collection from TEvent
    SG::ReadHandle<xAOD::VertexContainer> vertices(m_vertexContainer);

    for (const xAOD::Vertex* vertex: *vertices) {

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

      deltaPhi(*vertex) = (fail != FailType::FailEgamVect) ? std::abs(vmom.DeltaPhi(vegamma)) : -999.;      
      deltaZ(*vertex) = std::abs((zCommon.first - vertex->z())/zCommon.second);

    } // loop over vertices

    ATH_MSG_DEBUG("DecorateInputs exit code "<< fail);
    if(failType!=nullptr)
      *failType = fail;
    return StatusCode::SUCCESS;
  }

  //____________________________________________________________________________
  std::vector<std::pair<const xAOD::Vertex*, float> >
  PhotonVertexSelectionTool::getVertex(const xAOD::EgammaContainer &egammas, bool ignoreConv, yyVtxType* vtxCasePtr, FailType* failTypePtr) const
  {
    const xAOD::Vertex *vertex = nullptr;
    std::vector<std::pair<const xAOD::Vertex*, float> > vertexMLP;
    yyVtxType vtxCase = yyVtxType::Unknown; 
    FailType failType = FailType::NoFail;
    if (getVertexImp( egammas, vertex, ignoreConv, vertexMLP, vtxCase, failType ).isSuccess()) {
      std::sort(vertexMLP.begin(), vertexMLP.end(), sortMLP);
    }
    if(vtxCasePtr!=nullptr)
      *vtxCasePtr = vtxCase;
    if(failTypePtr!=nullptr)
      *failTypePtr = failType;

    return vertexMLP;
  }

  //____________________________________________________________________________
  StatusCode PhotonVertexSelectionTool::getVertex(const xAOD::EgammaContainer &egammas,
                                                  const xAOD::Vertex* &prime_vertex,
                                                  bool ignoreConv) const
  {
    std::vector<std::pair<const xAOD::Vertex*, float> > vertexMLP;
    yyVtxType vtxcase = yyVtxType::Unknown; 
    FailType failType = FailType::NoFail;
    return getVertexImp( egammas, prime_vertex, ignoreConv, vertexMLP, vtxcase, failType );
  } 

  StatusCode PhotonVertexSelectionTool::getVertexImp(const xAOD::EgammaContainer &egammas, 
                                                     const xAOD::Vertex* &prime_vertex, 
                                                     bool ignoreConv, 
                                                     std::vector<std::pair<const xAOD::Vertex*, float> >&  vertexMLP, yyVtxType& vtxCase, FailType& fail) const
  {
    vtxCase = yyVtxType::Unknown;
    const xAOD::PhotonContainer *photons = dynamic_cast<const xAOD::PhotonContainer*>(&egammas);
    if(!photons){
      ATH_MSG_WARNING("Container is not a photon");
      return StatusCode::FAILURE;
    }

    // Retrieve PV collection from TEvent
    SG::ReadHandle<xAOD::VertexContainer> vertices(m_vertexContainer);


    if (!decorateInputs(egammas).isSuccess()){   
      return StatusCode::FAILURE;
    }


    // Check if a conversion photon has a track attached to a primary/pileup vertex
    if (!ignoreConv) { 
      prime_vertex = getPrimaryVertexFromConv(photons);
      if (prime_vertex != nullptr) {
        vtxCase = yyVtxType::ConvTrack;
        fail = FailType::MatchedTrack;
        vertexMLP.emplace_back(prime_vertex, 0.);
        return StatusCode::SUCCESS;
      }
    }

    if (fail != FailType::NoFail){
      ATH_MSG_VERBOSE("Returning hardest vertex. Fail detected (type="<< fail <<")");
      vertexMLP.clear();
      vertexMLP.emplace_back(xAOD::PVHelpers::getHardestVertex(&*vertices), 10.);
      return StatusCode::SUCCESS;
    }

  
    // Get the EventInfo
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);

    // If there are any silicon conversions passing selection, use MVA1
    TMVA::Reader *reader = m_mva2.get();
    vtxCase = yyVtxType::NoSiTracks;
    if (!ignoreConv && photons) {
      for (const auto *photon: *photons) {
        if (!photon)
        {
          ATH_MSG_WARNING("Null pointer to photon");
          return StatusCode::FAILURE;
        }
        if (xAOD::PVHelpers::passConvSelection(photon,
                                               m_convPtCut))
        {
          reader = m_mva1.get();
          vtxCase = yyVtxType::SiConvTrack;
        }
      }
    }
    ATH_MSG_DEBUG("Vtx Case: " << vtxCase);

    // Vector sum of photons
    TLorentzVector vegamma = getEgammaVector(&egammas, fail);

    SG::AuxElement::ConstAccessor<float> sumPtA(m_derivationPrefix + "sumPt");
    SG::AuxElement::ConstAccessor<float> sumPt2A(m_derivationPrefix + "sumPt2");
    SG::AuxElement::ConstAccessor<float> deltaPhiA(m_derivationPrefix + "deltaPhi");
    SG::AuxElement::ConstAccessor<float> deltaZA(m_derivationPrefix + "deltaZ");

    // Loop over vertices and find best candidate
    float mlp = 0.0, mlp_max = -99999.0;
    for (const xAOD::Vertex* vertex: *vertices) {

      // Skip dummy vertices
      if (vertex->vertexType() != xAOD::VxType::VertexType::PriVtx and
          vertex->vertexType() != xAOD::VxType::VertexType::PileUp) continue;

      // Variables used as input into TMVA::Reader
      float sumPt2, sumPt, deltaPhi, deltaZ;

      sumPt     = log10((sumPtA)(*vertex)); 
      sumPt2    = log10((sumPt2A)(*vertex)); 
      deltaPhi  = (deltaPhiA)(*vertex); 
      deltaZ    = (deltaZA)(*vertex); 
      ATH_MSG_VERBOSE("log(sumPt): " << sumPt <<
                      " log(sumPt2): " << sumPt2 <<
                      " deltaPhi: " << deltaPhi <<
                      " deltaZ: " << deltaZ);

      // Skip vertices above 10 sigma from pointing or 15 sigma from conversion (HPV)
      if ((reader == m_mva2.get() && deltaZ > 10) ||
          (reader == m_mva1.get() && deltaZ > 15)) {
        mlp = -9999.0;
      } else {
        // Get likelihood probability from MVA
        std::vector<float> mvaInput = {deltaZ,deltaPhi,sumPt,sumPt2};
        TString mvaMethod("MLP method");
        mlp = reader->EvaluateMVA(mvaInput,mvaMethod);
      }
      ATH_MSG_VERBOSE("MVA output: "  << (reader == m_mva1.get() ? "MVA1 ": "MVA2 ")<< mlp);

      vertexMLP.emplace_back(vertex, mlp);

      // Keep track of maximal likelihood vertex
      if (mlp > mlp_max) {
        mlp_max = mlp;
        prime_vertex = vertex;
      }

    } // loop over vertices

    if (mlp_max <= -9999.0) {
      ATH_MSG_DEBUG("No good vertex candidates from pointing, returning hardest vertex.");
      prime_vertex = xAOD::PVHelpers::getHardestVertex(&*vertices);
      fail = FailType::NoGdCandidate;
      vertexMLP.clear();
      vertexMLP.push_back(std::make_pair(xAOD::PVHelpers::getHardestVertex(&*vertices), 20.));    
    }
    
    ATH_MSG_VERBOSE("getVertex case "<< (int)vtxCase << " exit code "<< (int)fail);
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

    // Retrieve PV collection from TEvent
    SG::ReadHandle<xAOD::VertexContainer> all_vertices(m_vertexContainer);


    for (const auto *photon: *photons) {
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

        primary = getVertexFromTrack(tp, &*all_vertices);
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

    if (!vertices.empty()) {
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
  TLorentzVector PhotonVertexSelectionTool::getEgammaVector(const xAOD::EgammaContainer *egammas, FailType& failType) const
  {
    TLorentzVector v, v1;
    const xAOD::CaloCluster *cluster = nullptr;
    for (const xAOD::Egamma* egamma: *egammas) {
      if (egamma == nullptr) {
        ATH_MSG_DEBUG("No egamma object to get four vector");
        failType = FailType::FailEgamVect;
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
