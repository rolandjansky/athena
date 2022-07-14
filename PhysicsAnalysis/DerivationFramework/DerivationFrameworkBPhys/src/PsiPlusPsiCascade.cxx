/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// PsiPlusPsiCascade.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/PsiPlusPsiCascade.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include "xAODTracking/VertexContainer.h"
#include "DerivationFrameworkBPhys/LocalVector.h"
#include "HepPDT/ParticleDataTable.hh"
#include <algorithm>

namespace DerivationFramework {
  typedef ElementLink<xAOD::VertexContainer> VertexLink;
  typedef std::vector<VertexLink> VertexLinkVector;

  StatusCode PsiPlusPsiCascade::initialize() {
    // retrieving vertex Fitter
    ATH_CHECK( m_iVertexFitter.retrieve());

    // retrieving the V0 tools
    ATH_CHECK( m_V0Tools.retrieve());

    // retrieving the Cascade tools
    ATH_CHECK( m_CascadeTools.retrieve());

    // Get the beam spot service
    ATH_CHECK( m_beamSpotSvc.retrieve() );

    IPartPropSvc* partPropSvc = nullptr;
    ATH_CHECK( service("PartPropSvc", partPropSvc, true) );
    auto pdt = partPropSvc->PDT();

    // retrieve particle masses
    if(m_massJpsi1 < 0.) m_massJpsi1 = BPhysPVCascadeTools::getParticleMass(pdt, PDG::J_psi);
    if(m_massJpsi2 < 0.) m_massJpsi2 = BPhysPVCascadeTools::getParticleMass(pdt, PDG::J_psi);
    if(m_massPsi1 < 0.) m_massPsi1 = BPhysPVCascadeTools::getParticleMass(pdt, PDG::psi_2S);
    if(m_massPsi2 < 0.) m_massPsi2 = BPhysPVCascadeTools::getParticleMass(pdt, PDG::psi_2S);

    if(m_vtx1Daug1MassHypo < 0.) m_vtx1Daug1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
    if(m_vtx1Daug2MassHypo < 0.) m_vtx1Daug2MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
    if(m_vtx1Daug3MassHypo < 0.) m_vtx1Daug3MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus);
    if(m_vtx1Daug4MassHypo < 0.) m_vtx1Daug4MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus);
    if(m_vtx2Daug1MassHypo < 0.) m_vtx2Daug1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
    if(m_vtx2Daug2MassHypo < 0.) m_vtx2Daug2MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
    if(m_vtx2Daug3MassHypo < 0.) m_vtx2Daug3MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus);
    if(m_vtx2Daug4MassHypo < 0.) m_vtx2Daug4MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus);

    return StatusCode::SUCCESS;
  }

  StatusCode PsiPlusPsiCascade::addBranches() const {
    constexpr int topoN = 2;
    std::array<std::unique_ptr<xAOD::VertexContainer>, topoN> VtxWriteHandles;
    std::array<std::unique_ptr<xAOD::VertexAuxContainer>, topoN> VtxWriteHandlesAux;
    if(m_cascadeOutputsKeys.size() !=topoN) {
      ATH_MSG_FATAL("Incorrect number of VtxContainers");
      return StatusCode::FAILURE;
    }

    for(int i =0; i<topoN;i++){
      VtxWriteHandles[i] = std::make_unique<xAOD::VertexContainer>();
      VtxWriteHandlesAux[i] = std::make_unique<xAOD::VertexAuxContainer>();
      VtxWriteHandles[i]->setStore(VtxWriteHandlesAux[i].get());
    }

    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    const xAOD::VertexContainer *pvContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(pvContainer, m_VxPrimaryCandidateName));
    ATH_MSG_DEBUG("Found " << m_VxPrimaryCandidateName << " in StoreGate!");
    if (pvContainer->size()==0) {
      ATH_MSG_WARNING("You have no primary vertices: " << pvContainer->size());
      return StatusCode::RECOVERABLE;
    }

    //----------------------------------------------------
    // Try to retrieve refitted primary vertices
    //----------------------------------------------------
    std::unique_ptr<xAOD::VertexContainer> refPvContainer;
    std::unique_ptr<xAOD::VertexAuxContainer> refPvAuxContainer;
    bool ref_record = false;
    if (m_refitPV) {
      if (evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
	// refitted PV container exists. Get it from the store gate
	xAOD::VertexContainer*    _refPvContainer    = nullptr;
	xAOD::VertexAuxContainer* _refPvAuxContainer = nullptr;
	ATH_CHECK(evtStore()->retrieve(_refPvContainer   , m_refPVContainerName       ));
	ATH_CHECK(evtStore()->retrieve(_refPvAuxContainer, m_refPVContainerName + "Aux."));
	refPvContainer.reset(_refPvContainer);
	refPvAuxContainer.reset(_refPvAuxContainer);
      } else {
	// refitted PV container does not exist. Create a new one.
	refPvContainer = std::make_unique<xAOD::VertexContainer>();
	refPvAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
	refPvContainer->setStore(refPvAuxContainer.get());
	ref_record = true;
      }
    }

    std::vector<Trk::VxCascadeInfo*> cascadeinfoContainer;
    std::vector<Trk::VxCascadeInfo*> cascadeinfoContainer_noConstr;
    ATH_CHECK(performSearch(&cascadeinfoContainer,&cascadeinfoContainer_noConstr));

    BPhysPVCascadeTools helper(&(*m_CascadeTools), &m_beamSpotSvc);
    helper.SetMinNTracksInPV(m_PV_minNTracks);

    // Decorators for the main vertex: chi2, ndf, pt and pt error, plus the V0 vertex variables
    SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks");
    SG::AuxElement::Decorator<VertexLinkVector> Psi2LinksDecor("Psi2VertexLinks");
    SG::AuxElement::Decorator<VertexLinkVector> Psi1LinksDecor("Psi1VertexLinks");
    SG::AuxElement::Decorator<float> chi2_decor("ChiSquared");
    SG::AuxElement::Decorator<int> ndof_decor("NumberDoF");
    SG::AuxElement::Decorator<float> chi2_nc_decor("ChiSquared_nc");
    SG::AuxElement::Decorator<int> ndof_nc_decor("NumberDoF_nc");
    SG::AuxElement::Decorator<float> Pt_decor("Pt");
    SG::AuxElement::Decorator<float> PtErr_decor("PtErr");

    // Get the container and identify the input Psi's
    const xAOD::VertexContainer  *psi2Container(nullptr);
    ATH_CHECK(evtStore()->retrieve(psi2Container, m_vertexPsi2ContainerKey));
    const xAOD::VertexContainer  *psi1Container(nullptr);
    ATH_CHECK(evtStore()->retrieve(psi1Container, m_vertexPsi1ContainerKey));

    for(unsigned int i=0; i<cascadeinfoContainer.size(); i++) {
      Trk::VxCascadeInfo* cascade_info_i = cascadeinfoContainer[i];
      if(cascade_info_i == nullptr) ATH_MSG_ERROR("cascadeinfoContainer is null");

      Trk::VxCascadeInfo* cascade_info_i_noConstr = cascadeinfoContainer_noConstr[i];

      const std::vector<xAOD::Vertex*> &cascadeVertices = cascade_info_i->vertices();
      if(cascadeVertices.size()!=topoN) ATH_MSG_ERROR("Incorrect number of vertices");
      if(cascadeVertices[0] == nullptr || cascadeVertices[1] == nullptr) ATH_MSG_ERROR("Error null vertex");
      // Keep vertices
      for(int i=0; i<topoN; i++) VtxWriteHandles[i]->push_back(cascadeVertices[i]);

      cascade_info_i->getSVOwnership(false); // Prevent Container from deleting vertices
      const auto mainVertex = cascadeVertices[1]; // this is the mother vertex
      const std::vector< std::vector<TLorentzVector> > &moms = cascade_info_i->getParticleMoms();

      // Set links to cascade vertices
      std::vector<xAOD::Vertex*> verticestoLink;
      verticestoLink.push_back(cascadeVertices[0]);

      // decorate the mother vertex with preceding vertex links
      // https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkBPhys/src/BPhysPVCascadeTools.cxx
      if(!BPhysPVCascadeTools::LinkVertices(CascadeLinksDecor, verticestoLink, VtxWriteHandles[0].get(), cascadeVertices[1])) ATH_MSG_ERROR("Error decorating with cascade vertices");

      // Identify the input Psi2
      xAOD::Vertex* psi2Vertex = BPhysPVCascadeTools::FindVertex<4>(psi2Container, cascadeVertices[1]);
      // Identify the input Psi1
      xAOD::Vertex* psi1Vertex = BPhysPVCascadeTools::FindVertex<4>(psi1Container, cascadeVertices[0]);

      // Set links to input vertices
      std::vector<xAOD::Vertex*> psi2VerticestoLink;
      if(psi2Vertex) psi2VerticestoLink.push_back(psi2Vertex);
      else ATH_MSG_WARNING("Could not find linking Jpsi");
      if(!BPhysPVCascadeTools::LinkVertices(Psi2LinksDecor, psi2VerticestoLink, psi2Container, cascadeVertices[1])) ATH_MSG_ERROR("Error decorating with Psi vertices");

      std::vector<xAOD::Vertex*> psi1VerticestoLink;
      if(psi1Vertex) psi1VerticestoLink.push_back(psi1Vertex);
      else ATH_MSG_WARNING("Could not find linking Psi1");
      if(!BPhysPVCascadeTools::LinkVertices(Psi1LinksDecor, psi1VerticestoLink, psi1Container, cascadeVertices[1])) ATH_MSG_ERROR("Error decorating with Psi vertices");

      // reset beamspot cache
      helper.GetBeamSpot(true);

      xAOD::BPhysHypoHelper vtx(m_hypoName, mainVertex);

      // Get refitted track momenta from all vertices, charged tracks only
      BPhysPVCascadeTools::SetVectorInfo(vtx, cascade_info_i);
      vtx.setPass(true);

      //
      // Decorate main vertex
      //
      // mass, mass error
      // https://gitlab.cern.ch/atlas/athena/-/blob/21.2/Tracking/TrkVertexFitter/TrkVKalVrtFitter/TrkVKalVrtFitter/VxCascadeInfo.h
      BPHYS_CHECK( vtx.setMass(m_CascadeTools->invariantMass(moms[1])) );
      BPHYS_CHECK( vtx.setMassErr(m_CascadeTools->invariantMassError(moms[1],cascade_info_i->getCovariance()[1])) );
      // pt and pT error (the default pt of mainVertex is != the pt of the full cascade fit!)
      Pt_decor(*mainVertex) = m_CascadeTools->pT(moms[1]);
      PtErr_decor(*mainVertex) = m_CascadeTools->pTError(moms[1],cascade_info_i->getCovariance()[1]);
      // chi2 and ndof (the default chi2 of mainVertex is != the chi2 of the full cascade fit!)
      chi2_decor(*mainVertex) = cascade_info_i->fitChi2();
      ndof_decor(*mainVertex) = cascade_info_i->nDoF();
      chi2_nc_decor(*mainVertex) = cascade_info_i_noConstr ? cascade_info_i_noConstr->fitChi2() : 999999.;
      ndof_nc_decor(*mainVertex) = cascade_info_i_noConstr ? cascade_info_i_noConstr->nDoF() : 1;

      double Mass_Moth = m_CascadeTools->invariantMass(moms[1]); // size=5
      ATH_CHECK(helper.FillCandwithRefittedVertices(m_refitPV, pvContainer, refPvContainer.get(), &(*m_pvRefitter), m_PV_max, m_DoVertexType, cascade_info_i, 1, Mass_Moth, vtx));
    } // loop over cascadeinfoContainer

    for(int i =0; i<topoN;i++){
      ATH_CHECK(evtStore()->record(std::move(VtxWriteHandles[i])   , m_cascadeOutputsKeys[i]));
      ATH_CHECK(evtStore()->record(std::move(VtxWriteHandlesAux[i]), m_cascadeOutputsKeys[i] + "Aux."));
    }
    if(ref_record) {
      ATH_CHECK(evtStore()->record(std::move(refPvContainer)   , m_refPVContainerName));
      ATH_CHECK(evtStore()->record(std::move(refPvAuxContainer), m_refPVContainerName + "Aux."));
    }

    // Deleting cascadeinfo since this won't be stored.
    // Vertices have been kept in m_cascadeOutputs and should be owned by their container
    for (auto cascade_info_i : cascadeinfoContainer) delete cascade_info_i;
    for (auto cascade_info_i_noConstr : cascadeinfoContainer_noConstr) delete cascade_info_i_noConstr;

    return StatusCode::SUCCESS;
  }

  PsiPlusPsiCascade::PsiPlusPsiCascade(const std::string& type, const std::string& name, const IInterface* parent) : AthAlgTool(type,name,parent),
    m_vertexPsi1ContainerKey(""),
    m_vertexPsi2ContainerKey(""),
    m_vertexPsi1HypoName("Psi"),
    m_vertexPsi2HypoName("Psi"),
    m_cascadeOutputsKeys{ "PsiPlusPsiCascadeVtx1", "PsiPlusPsiCascadeVtx2" },
    m_VxPrimaryCandidateName("PrimaryVertices"),
    m_jpsi1MassLower(0.0),
    m_jpsi1MassUpper(20000.0),
    m_jpsi2MassLower(0.0),
    m_jpsi2MassUpper(20000.0),
    m_psi1MassLower(0.0),
    m_psi1MassUpper(25000.0),
    m_psi2MassLower(0.0),
    m_psi2MassUpper(25000.0),
    m_MassLower(0.0),
    m_MassUpper(31000.0),
    m_vtx1Daug1MassHypo(-1),
    m_vtx1Daug2MassHypo(-1),
    m_vtx1Daug3MassHypo(-1),
    m_vtx1Daug4MassHypo(-1),
    m_vtx2Daug1MassHypo(-1),
    m_vtx2Daug2MassHypo(-1),
    m_vtx2Daug3MassHypo(-1),
    m_vtx2Daug4MassHypo(-1),
    m_massJpsi1(-1),
    m_massJpsi2(-1),
    m_massPsi1(-1),
    m_massPsi2(-1),
    m_constrPsi1(true),
    m_constrPsi2(true),
    m_constrJpsi1(true),
    m_constrJpsi2(true),
    m_chi2cut(-1.0),
    m_beamSpotSvc("BeamCondSvc",name),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_V0Tools("Trk::V0Tools"),
    m_CascadeTools("DerivationFramework::CascadeTools")
  {
    declareProperty("Psi1Vertices",             m_vertexPsi1ContainerKey);
    declareProperty("Psi2Vertices",             m_vertexPsi2ContainerKey);
    declareProperty("Psi1VtxHypoName",          m_vertexPsi1HypoName);
    declareProperty("Psi2VtxHypoName",          m_vertexPsi2HypoName);
    declareProperty("VxPrimaryCandidateName",   m_VxPrimaryCandidateName);
    declareProperty("RefPVContainerName",       m_refPVContainerName = "RefittedPrimaryVertices");
    declareProperty("Jpsi1MassLowerCut",        m_jpsi1MassLower);
    declareProperty("Jpsi1MassUpperCut",        m_jpsi1MassUpper);
    declareProperty("Jpsi2MassLowerCut",        m_jpsi2MassLower);
    declareProperty("Jpsi2MassUpperCut",        m_jpsi2MassUpper);
    declareProperty("Psi1MassLowerCut",         m_psi1MassLower);
    declareProperty("Psi1MassUpperCut",         m_psi1MassUpper);
    declareProperty("Psi2MassLowerCut",         m_psi2MassLower);
    declareProperty("Psi2MassUpperCut",         m_psi2MassUpper);
    declareProperty("MassLowerCut",             m_MassLower);
    declareProperty("MassUpperCut",             m_MassUpper);
    declareProperty("HypothesisName",           m_hypoName = "TQ");
    declareProperty("Vtx1Daug1MassHypo",        m_vtx1Daug1MassHypo);
    declareProperty("Vtx1Daug2MassHypo",        m_vtx1Daug2MassHypo);
    declareProperty("Vtx1Daug3MassHypo",        m_vtx1Daug3MassHypo);
    declareProperty("Vtx1Daug4MassHypo",        m_vtx1Daug4MassHypo);
    declareProperty("Vtx2Daug1MassHypo",        m_vtx2Daug1MassHypo);
    declareProperty("Vtx2Daug2MassHypo",        m_vtx2Daug2MassHypo);
    declareProperty("Vtx2Daug3MassHypo",        m_vtx2Daug3MassHypo);
    declareProperty("Vtx2Daug4MassHypo",        m_vtx2Daug4MassHypo);
    declareProperty("Jpsi1Mass",                m_massJpsi1);
    declareProperty("Jpsi2Mass",                m_massJpsi2);
    declareProperty("Psi1Mass",                 m_massPsi1);
    declareProperty("Psi2Mass",                 m_massPsi2);
    declareProperty("ApplyPsi1MassConstraint",  m_constrPsi1);
    declareProperty("ApplyPsi2MassConstraint",  m_constrPsi2);
    declareProperty("ApplyJpsi1MassConstraint", m_constrJpsi1);
    declareProperty("ApplyJpsi2MassConstraint", m_constrJpsi2);
    declareProperty("Chi2Cut",                  m_chi2cut);
    declareProperty("RefitPV",                  m_refitPV         = true);
    declareProperty("MaxnPV",                   m_PV_max          = 1000);
    declareProperty("MinNTracksInPV",           m_PV_minNTracks   = 0);
    declareProperty("DoVertexType",             m_DoVertexType    = 7);
    declareProperty("TrkVertexFitterTool",      m_iVertexFitter);
    declareProperty("PVRefitter",               m_pvRefitter);
    declareProperty("V0Tools",                  m_V0Tools);
    declareProperty("CascadeTools",             m_CascadeTools);
    declareProperty("CascadeVertexCollections", m_cascadeOutputsKeys);
  }

  StatusCode PsiPlusPsiCascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer, std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer_noConstr) const {
    ATH_MSG_DEBUG( "PsiPlusPsiCascade::performSearch" );
    assert(cascadeinfoContainer!=nullptr && cascadeinfoContainer_noConstr!=nullptr);

    // Get TrackParticle container (for setting links to the original tracks)
    const xAOD::TrackParticleContainer  *trackContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(trackContainer, "InDetTrackParticles"));

    std::vector<const xAOD::TrackParticle*> tracksJpsi1;
    std::vector<const xAOD::TrackParticle*> tracksJpsi2;
    std::vector<const xAOD::TrackParticle*> tracksPsi1;
    std::vector<const xAOD::TrackParticle*> tracksPsi2;
    std::vector<double> massesPsi1;
    massesPsi1.push_back(m_vtx1Daug1MassHypo);
    massesPsi1.push_back(m_vtx1Daug2MassHypo);
    massesPsi1.push_back(m_vtx1Daug3MassHypo);
    massesPsi1.push_back(m_vtx1Daug4MassHypo);
    std::vector<double> massesJpsi1;
    massesJpsi1.push_back(m_vtx1Daug1MassHypo);
    massesJpsi1.push_back(m_vtx1Daug2MassHypo);
     std::vector<double> massesPsi2;
    massesPsi2.push_back(m_vtx2Daug1MassHypo);
    massesPsi2.push_back(m_vtx2Daug2MassHypo);
    massesPsi2.push_back(m_vtx2Daug3MassHypo);
    massesPsi2.push_back(m_vtx2Daug4MassHypo);
    std::vector<double> massesJpsi2;
    massesJpsi2.push_back(m_vtx2Daug1MassHypo);
    massesJpsi2.push_back(m_vtx2Daug2MassHypo);

    // Get Psi1 container
    const xAOD::VertexContainer *psi1Container(nullptr);
    ATH_CHECK(evtStore()->retrieve(psi1Container, m_vertexPsi1ContainerKey));

    // Get Psi2 container
    const xAOD::VertexContainer *psi2Container(nullptr);
    ATH_CHECK(evtStore()->retrieve(psi2Container, m_vertexPsi2ContainerKey));

    // Select the Psi2 candidates before calling cascade fit
    std::vector<const xAOD::Vertex*> selectedPsi2Candidates;
    for(auto vxcItr=psi2Container->cbegin(); vxcItr!=psi2Container->cend(); ++vxcItr) {
      // Check the passed flag first
      xAOD::Vertex* vtx = *vxcItr;
      SG::AuxElement::Accessor<Char_t> flagAcc("passed_"+m_vertexPsi2HypoName);
      if(flagAcc.isAvailable(*vtx)){
	if(!flagAcc(*vtx)) continue;
      }

      // Check Psi2 candidate invariant mass and skip if need be
      double mass_psi2 = m_V0Tools->invariantMass(*vxcItr, massesPsi2);
      if (mass_psi2 < m_psi2MassLower || mass_psi2 > m_psi2MassUpper) continue;

      // Add loose cut on Jpsi2 mass from Psi2 -> Jpsi2 pi+ pi-
      TLorentzVector p4_mu1, p4_mu2;
      p4_mu1.SetPtEtaPhiM( (*vxcItr)->trackParticle(0)->pt(), 
			   (*vxcItr)->trackParticle(0)->eta(),
			   (*vxcItr)->trackParticle(0)->phi(), m_vtx2Daug1MassHypo);
      p4_mu2.SetPtEtaPhiM( (*vxcItr)->trackParticle(1)->pt(), 
			   (*vxcItr)->trackParticle(1)->eta(),
			   (*vxcItr)->trackParticle(1)->phi(), m_vtx2Daug2MassHypo);
      double mass_jpsi2 = (p4_mu1 + p4_mu2).M();
      if (mass_jpsi2 < m_jpsi2MassLower || mass_jpsi2 > m_jpsi2MassUpper) continue;
      selectedPsi2Candidates.push_back(*vxcItr);
    }
    if(selectedPsi2Candidates.size()<1) return StatusCode::SUCCESS;

    // Select the Psi1 candidates before calling cascade fit
    std::vector<const xAOD::Vertex*> selectedPsi1Candidates;
    for(auto vxcItr=psi1Container->cbegin(); vxcItr!=psi1Container->cend(); ++vxcItr) {
      // Check the passed flag first
      xAOD::Vertex* vtx = *vxcItr;
      SG::AuxElement::Accessor<Char_t> flagAcc("passed_"+m_vertexPsi1HypoName);
      if(flagAcc.isAvailable(*vtx)){
	if(!flagAcc(*vtx)) continue;
      }

      // Check Psi candidate invariant mass and skip if need be
      double mass_psi1 = m_V0Tools->invariantMass(*vxcItr,massesPsi1);
      if(mass_psi1 < m_psi1MassLower || mass_psi1 > m_psi1MassUpper) continue;

      // Add loose cut on Jpsi1 mass from Psi1 -> Jpsi1 pi+ pi-
      TLorentzVector p4_mu1, p4_mu2;
      p4_mu1.SetPtEtaPhiM( (*vxcItr)->trackParticle(0)->pt(), 
			   (*vxcItr)->trackParticle(0)->eta(),
			   (*vxcItr)->trackParticle(0)->phi(), m_vtx1Daug1MassHypo);
      p4_mu2.SetPtEtaPhiM( (*vxcItr)->trackParticle(1)->pt(), 
			   (*vxcItr)->trackParticle(1)->eta(),
			   (*vxcItr)->trackParticle(1)->phi(), m_vtx1Daug2MassHypo);
      double mass_jpsi1 = (p4_mu1 + p4_mu2).M();
      if (mass_jpsi1 < m_jpsi1MassLower || mass_jpsi1 > m_jpsi1MassUpper) continue;
      selectedPsi1Candidates.push_back(*vxcItr);
    }
    if(selectedPsi1Candidates.size()<1) return StatusCode::SUCCESS;

    // Select Psi1+Psi2 candidates
    // Iterate over Psi2 vertices
    for(auto psi2Itr=selectedPsi2Candidates.cbegin(); psi2Itr!=selectedPsi2Candidates.cend(); ++psi2Itr) {
      size_t psi2TrkNum = (*psi2Itr)->nTrackParticles();
      tracksPsi2.clear();
      for(size_t it=0; it<psi2TrkNum; it++) tracksPsi2.push_back((*psi2Itr)->trackParticle(it));
      if (tracksPsi2.size() != 4 || massesPsi2.size() != 4) {
	ATH_MSG_ERROR("Problems with Psi2 input: number of tracks or track mass inputs is not 4!");
      }
      tracksJpsi2.clear();
      tracksJpsi2.push_back((*psi2Itr)->trackParticle(0));
      tracksJpsi2.push_back((*psi2Itr)->trackParticle(1));

      // Iterate over Psi1 vertices
      for(auto psi1Itr=selectedPsi1Candidates.cbegin(); psi1Itr!=selectedPsi1Candidates.cend(); ++psi1Itr) {
	// Check identical tracks in input
	if(std::find(tracksPsi2.cbegin(), tracksPsi2.cend(), (*psi1Itr)->trackParticle(0)) != tracksPsi2.cend()) continue;
	if(std::find(tracksPsi2.cbegin(), tracksPsi2.cend(), (*psi1Itr)->trackParticle(1)) != tracksPsi2.cend()) continue;
	if(std::find(tracksPsi2.cbegin(), tracksPsi2.cend(), (*psi1Itr)->trackParticle(2)) != tracksPsi2.cend()) continue;
	if(std::find(tracksPsi2.cbegin(), tracksPsi2.cend(), (*psi1Itr)->trackParticle(3)) != tracksPsi2.cend()) continue;

	size_t psi1TrkNum = (*psi1Itr)->nTrackParticles();
	tracksPsi1.clear();
	for(size_t it=0; it<psi1TrkNum; it++) tracksPsi1.push_back((*psi1Itr)->trackParticle(it));
	if (tracksPsi1.size() != 4 || massesPsi1.size() != 4) {
	  ATH_MSG_ERROR("Problems with Psi1 input: number of tracks or track mass inputs is not 4!");
	}
	tracksJpsi1.clear();
	tracksJpsi1.push_back((*psi1Itr)->trackParticle(0));
	tracksJpsi1.push_back((*psi1Itr)->trackParticle(1));

	TLorentzVector p4_moth;
	TLorentzVector tmp;
        for(size_t it=0; it<psi2TrkNum; it++) {
	  tmp.SetPtEtaPhiM((*psi2Itr)->trackParticle(it)->pt(),(*psi2Itr)->trackParticle(it)->eta(),(*psi2Itr)->trackParticle(it)->phi(),massesPsi2[it]);
	  p4_moth += tmp;
	}
	for(size_t it=0; it<psi1TrkNum; it++) {
	  tmp.SetPtEtaPhiM((*psi1Itr)->trackParticle(it)->pt(),(*psi1Itr)->trackParticle(it)->eta(),(*psi1Itr)->trackParticle(it)->phi(),massesPsi1[it]);
	  p4_moth += tmp;
	}
	if (p4_moth.M() < m_MassLower || p4_moth.M() > m_MassUpper) continue;

	// Apply the user's settings to the fitter
	// Reset
	m_iVertexFitter->setDefault();
	// Robustness: http://cdsweb.cern.ch/record/685551
	int robustness = 0;
	m_iVertexFitter->setRobustness(robustness);
	// Build up the topology
	// Vertex list
	std::vector<Trk::VertexID> vrtList;
	// Psi1 vertex
	Trk::VertexID vID1;
        // https://gitlab.cern.ch/atlas/athena/-/blob/21.2/Tracking/TrkVertexFitter/TrkVKalVrtFitter/TrkVKalVrtFitter/IVertexCascadeFitter.h
	if (m_constrPsi1) {
	  vID1 = m_iVertexFitter->startVertex(tracksPsi1,massesPsi1,m_massPsi1);
	} else {
	  vID1 = m_iVertexFitter->startVertex(tracksPsi1,massesPsi1);
	}
	vrtList.push_back(vID1);
	// Mother vertex including Psi2
	Trk::VertexID vID2 = m_iVertexFitter->nextVertex(tracksPsi2,massesPsi2,vrtList);
	if (m_constrJpsi1) {
	  std::vector<Trk::VertexID> cnstV;
	  cnstV.clear();
	  if ( !m_iVertexFitter->addMassConstraint(vID1,tracksJpsi1,cnstV,m_massJpsi1).isSuccess() ) {
	    ATH_MSG_WARNING("addMassConstraint for Jpsi1 failed");
	  }
	}
	if (m_constrPsi2) {
	  std::vector<Trk::VertexID> cnstV;
	  cnstV.clear();
	  if ( !m_iVertexFitter->addMassConstraint(vID2,tracksPsi2,cnstV,m_massPsi2).isSuccess() ) {
	    ATH_MSG_WARNING("addMassConstraint for Psi2 failed");
	  }
	}
	if (m_constrJpsi2) {
	  std::vector<Trk::VertexID> cnstV;
	  cnstV.clear();
	  if ( !m_iVertexFitter->addMassConstraint(vID2,tracksJpsi2,cnstV,m_massJpsi2).isSuccess() ) {
	    ATH_MSG_WARNING("addMassConstraint for Jpsi2 failed");
	  }
	}
	// Do the work
	std::unique_ptr<Trk::VxCascadeInfo> result(m_iVertexFitter->fitCascade());

	bool pass = false;
	if (result != nullptr) {
	  // reset links to original tracks
	  BPhysPVCascadeTools::PrepareVertexLinks(result.get(), trackContainer);
	  // necessary to prevent memory leak
	  result->getSVOwnership(true);

	  // Chi2/DOF cut
	  double chi2DOF = result->fitChi2()/result->nDoF();
	  bool chi2CutPassed = (m_chi2cut <= 0.0 || chi2DOF < m_chi2cut);

	  if(chi2CutPassed) {
	    cascadeinfoContainer->push_back(result.release());
	    pass = true;
	  }
	}

	// do cascade fit again without any mass constraints
	if(pass) {
	  if(m_constrJpsi1 || m_constrPsi1 || m_constrJpsi2 || m_constrPsi2) {
	    m_iVertexFitter->setDefault();
	    m_iVertexFitter->setRobustness(robustness);
	    std::vector<Trk::VertexID> vrtList_nc;
	    // Psi1 vertex
	    Trk::VertexID vID1_nc = m_iVertexFitter->startVertex(tracksPsi1,massesPsi1);
	    vrtList_nc.push_back(vID1_nc);
	    // Mother vertex including Psi2
	    m_iVertexFitter->nextVertex(tracksPsi2,massesPsi2,vrtList_nc);
	    // Do the work
	    std::unique_ptr<Trk::VxCascadeInfo> result_nc(m_iVertexFitter->fitCascade());

	    if (result_nc != nullptr) {
	      // reset links to original tracks
	      BPhysPVCascadeTools::PrepareVertexLinks(result_nc.get(), trackContainer);
	      // necessary to prevent memory leak
	      result_nc->getSVOwnership(true);
	      cascadeinfoContainer_noConstr->push_back(result_nc.release());
	    }
	    else cascadeinfoContainer_noConstr->push_back(0);
	  }
	  else cascadeinfoContainer_noConstr->push_back(0);
	}
      } //Iterate over Psi1 vertices
    } //Iterate over Psi2 vertices
    
    return StatusCode::SUCCESS;
  }
}
