/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// JpsiPlusPsiCascade.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/JpsiPlusPsiCascade.h"
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

  StatusCode JpsiPlusPsiCascade::initialize() {
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
    if(m_mass_jpsi < 0.) m_mass_jpsi = BPhysPVCascadeTools::getParticleMass(pdt, PDG::J_psi);
    if(m_mass_jpsi2 < 0.) m_mass_jpsi2 = BPhysPVCascadeTools::getParticleMass(pdt, PDG::J_psi);
    if(m_mass_psi < 0.) m_mass_psi = BPhysPVCascadeTools::getParticleMass(pdt, PDG::psi_2S);

    if(m_vtx1Daug1MassHypo < 0.) m_vtx1Daug1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
    if(m_vtx1Daug2MassHypo < 0.) m_vtx1Daug2MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
    if(m_vtx1Daug3MassHypo < 0.) m_vtx1Daug3MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus);
    if(m_vtx1Daug4MassHypo < 0.) m_vtx1Daug4MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus);
    if(m_vtx2Daug1MassHypo < 0.) m_vtx2Daug1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
    if(m_vtx2Daug2MassHypo < 0.) m_vtx2Daug2MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);

    return StatusCode::SUCCESS;
  }

  StatusCode JpsiPlusPsiCascade::addBranches() const {
    constexpr int topoN = 2;
    std::array<xAOD::VertexContainer*, topoN> Vtxwritehandles;
    std::array<xAOD::VertexAuxContainer*, topoN> Vtxwritehandlesaux;
    if(m_cascadeOutputsKeys.size() !=topoN) {
      ATH_MSG_FATAL("Incorrect number of VtxContainers");
      return StatusCode::FAILURE;
    }

    for(int i =0; i<topoN;i++){
      Vtxwritehandles[i] = new xAOD::VertexContainer();
      Vtxwritehandlesaux[i] = new xAOD::VertexAuxContainer();
      Vtxwritehandles[i]->setStore(Vtxwritehandlesaux[i]);
      ATH_CHECK(evtStore()->record(Vtxwritehandles[i]   , m_cascadeOutputsKeys[i]       ));
      ATH_CHECK(evtStore()->record(Vtxwritehandlesaux[i], m_cascadeOutputsKeys[i] + "Aux."));
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
    xAOD::VertexContainer*    refPvContainer    = nullptr;
    xAOD::VertexAuxContainer* refPvAuxContainer = nullptr;
    if (m_refitPV) {
      if (evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
	// refitted PV container exists. Get it from the store gate
	ATH_CHECK(evtStore()->retrieve(refPvContainer   , m_refPVContainerName       ));
	ATH_CHECK(evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName + "Aux."));
      } else {
	// refitted PV container does not exist. Create a new one.
	refPvContainer = new xAOD::VertexContainer;
	refPvAuxContainer = new xAOD::VertexAuxContainer;
	refPvContainer->setStore(refPvAuxContainer);
	ATH_CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
	ATH_CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName + "Aux."));
      }
    }

    std::vector<Trk::VxCascadeInfo*> cascadeinfoContainer;
    std::vector<Trk::VxCascadeInfo*> cascadeinfoContainer_noConstr;
    ATH_CHECK(performSearch(&cascadeinfoContainer,&cascadeinfoContainer_noConstr));

    BPhysPVCascadeTools helper(&(*m_CascadeTools), &m_beamSpotSvc);
    helper.SetMinNTracksInPV(m_PV_minNTracks);

    // Decorators for the main vertex: chi2, ndf, pt and pt error, plus the V0 vertex variables
    SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks"); 
    SG::AuxElement::Decorator<VertexLinkVector> JpsiLinksDecor("JpsiVertexLinks"); 
    SG::AuxElement::Decorator<VertexLinkVector> PsiLinksDecor("PsiVertexLinks"); 
    SG::AuxElement::Decorator<float> chi2_decor("ChiSquared");
    SG::AuxElement::Decorator<int> ndof_decor("NumberDoF");
    SG::AuxElement::Decorator<float> chi2_nc_decor("ChiSquared_nc");
    SG::AuxElement::Decorator<int> ndof_nc_decor("NumberDoF_nc");
    SG::AuxElement::Decorator<float> Pt_decor("Pt");
    SG::AuxElement::Decorator<float> PtErr_decor("PtErr");

    // Get Jpsi container and identify the input Jpsi
    const xAOD::VertexContainer  *jpsiContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(jpsiContainer, m_vertexContainerKey));
    const xAOD::VertexContainer  *psiContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(psiContainer, m_vertexPsiContainerKey));

    for(unsigned int i=0; i<cascadeinfoContainer.size(); i++) {
      Trk::VxCascadeInfo* x = cascadeinfoContainer[i];
      if(x==nullptr) ATH_MSG_ERROR("cascadeinfoContainer is null");

      Trk::VxCascadeInfo* y = cascadeinfoContainer_noConstr[i];

      const std::vector<xAOD::Vertex*> &cascadeVertices = x->vertices();
      if(cascadeVertices.size()!=topoN) ATH_MSG_ERROR("Incorrect number of vertices");
      if(cascadeVertices[0] == nullptr || cascadeVertices[1] == nullptr) ATH_MSG_ERROR("Error null vertex");
      // Keep vertices
      for(int i=0; i<topoN; i++) Vtxwritehandles[i]->push_back(cascadeVertices[i]);

      x->getSVOwnership(false); // Prevent Container from deleting vertices
      const auto mainVertex = cascadeVertices[1]; // this is the mother vertex
      const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();

      // Set links to cascade vertices
      std::vector<xAOD::Vertex*> verticestoLink;
      verticestoLink.push_back(cascadeVertices[0]);

      // decorate the mother vertex with preceding vertex links
      // https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkBPhys/src/BPhysPVCascadeTools.cxx
      if(!BPhysPVCascadeTools::LinkVertices(CascadeLinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[1])) ATH_MSG_ERROR("Error decorating with cascade vertices");

      // Identify the input Jpsi
      xAOD::Vertex* jpsiVertex2 = BPhysPVCascadeTools::FindVertex<2>(jpsiContainer, cascadeVertices[1]);
      // Identify the input Psi
      xAOD::Vertex* psiVertex = BPhysPVCascadeTools::FindVertex<4>(psiContainer, cascadeVertices[0]);

      // Set links to input vertices
      std::vector<xAOD::Vertex*> jpsiVerticestoLink;
      if(jpsiVertex2) jpsiVerticestoLink.push_back(jpsiVertex2);
      else ATH_MSG_WARNING("Could not find linking Jpsi");
      if(!BPhysPVCascadeTools::LinkVertices(JpsiLinksDecor, jpsiVerticestoLink, jpsiContainer, cascadeVertices[1])) ATH_MSG_ERROR("Error decorating with Jpsi vertices");

      std::vector<xAOD::Vertex*> psiVerticestoLink;
      if(psiVertex) psiVerticestoLink.push_back(psiVertex);
      else ATH_MSG_WARNING("Could not find linking Psi");
      if(!BPhysPVCascadeTools::LinkVertices(PsiLinksDecor, psiVerticestoLink, psiContainer, cascadeVertices[1])) ATH_MSG_ERROR("Error decorating with Psi vertices");

      // reset beamspot cache
      helper.GetBeamSpot(true);

      xAOD::BPhysHypoHelper vtx(m_hypoName, mainVertex);

      // Get refitted track momenta from all vertices, charged tracks only
      BPhysPVCascadeTools::SetVectorInfo(vtx, x);
      vtx.setPass(true);

      //
      // Decorate main vertex
      //
      // mass, mass error
      // https://gitlab.cern.ch/atlas/athena/-/blob/21.2/Tracking/TrkVertexFitter/TrkVKalVrtFitter/TrkVKalVrtFitter/VxCascadeInfo.h
      BPHYS_CHECK( vtx.setMass(m_CascadeTools->invariantMass(moms[1])) );
      BPHYS_CHECK( vtx.setMassErr(m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1])) );
      // pt and pT error (the default pt of mainVertex is != the pt of the full cascade fit!)
      Pt_decor(*mainVertex) = m_CascadeTools->pT(moms[1]);
      PtErr_decor(*mainVertex) = m_CascadeTools->pTError(moms[1],x->getCovariance()[1]);
      // chi2 and ndof (the default chi2 of mainVertex is != the chi2 of the full cascade fit!)
      chi2_decor(*mainVertex) = x->fitChi2();
      ndof_decor(*mainVertex) = x->nDoF();
      chi2_nc_decor(*mainVertex) = y ? y->fitChi2() : 999999.;
      ndof_nc_decor(*mainVertex) = y ? y->nDoF() : 1;

      double Mass_Moth = m_CascadeTools->invariantMass(moms[1]); // size=3
      ATH_CHECK(helper.FillCandwithRefittedVertices(m_refitPV, pvContainer, refPvContainer, &(*m_pvRefitter), m_PV_max, m_DoVertexType, x, 1, Mass_Moth, vtx));
    } // loop over cascadeinfoContainer

    // Deleting cascadeinfo since this won't be stored.
    // Vertices have been kept in m_cascadeOutputs and should be owned by their container
    for (auto x : cascadeinfoContainer) delete x;
    for (auto y : cascadeinfoContainer_noConstr) delete y;

    return StatusCode::SUCCESS;
  }

  JpsiPlusPsiCascade::JpsiPlusPsiCascade(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p),
    m_vertexContainerKey(""),
    m_vertexPsiContainerKey(""),
    m_vertexJpsiHypoName("Jpsi"),
    m_vertexPsiHypoName("Psi"),
    m_cascadeOutputsKeys{ "JpsiPlusPsiCascadeVtx1", "JpsiPlusPsiCascadeVtx2" },
    m_VxPrimaryCandidateName("PrimaryVertices"),
    m_jpsiMassLower(0.0),
    m_jpsiMassUpper(20000.0),
    m_jpsi2MassLower(0.0),
    m_jpsi2MassUpper(20000.0),
    m_psiMassLower(0.0),
    m_psiMassUpper(20000.0),
    m_MassLower(0.0),
    m_MassUpper(20000.0),
    m_vtx1Daug1MassHypo(-1),
    m_vtx1Daug2MassHypo(-1),
    m_vtx1Daug3MassHypo(-1),
    m_vtx1Daug4MassHypo(-1),
    m_vtx2Daug1MassHypo(-1),
    m_vtx2Daug2MassHypo(-1),
    m_mass_jpsi(-1),
    m_mass_jpsi2(-1),
    m_mass_psi(-1),
    m_constrPsi(true),
    m_constrJpsi(true),
    m_constrJpsi2(true),
    m_chi2cut(-1.0),
    m_beamSpotSvc("BeamCondSvc",n),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_V0Tools("Trk::V0Tools"),
    m_CascadeTools("DerivationFramework::CascadeTools")
  {
    declareProperty("JpsiVertices",             m_vertexContainerKey);
    declareProperty("PsiVertices",              m_vertexPsiContainerKey);
    declareProperty("JpsiVtxHypoName",          m_vertexJpsiHypoName);
    declareProperty("PsiVtxHypoName",           m_vertexPsiHypoName);
    declareProperty("VxPrimaryCandidateName",   m_VxPrimaryCandidateName);
    declareProperty("RefPVContainerName",       m_refPVContainerName = "RefittedPrimaryVertices");
    declareProperty("Jpsi1MassLowerCut",        m_jpsiMassLower);
    declareProperty("Jpsi1MassUpperCut",        m_jpsiMassUpper);
    declareProperty("Jpsi2MassLowerCut",        m_jpsi2MassLower);
    declareProperty("Jpsi2MassUpperCut",        m_jpsi2MassUpper);
    declareProperty("PsiMassLowerCut",          m_psiMassLower);
    declareProperty("PsiMassUpperCut",          m_psiMassUpper);
    declareProperty("MassLowerCut",             m_MassLower);
    declareProperty("MassUpperCut",             m_MassUpper);
    declareProperty("HypothesisName",           m_hypoName = "TQ");
    declareProperty("Vtx1Daug1MassHypo",        m_vtx1Daug1MassHypo);
    declareProperty("Vtx1Daug2MassHypo",        m_vtx1Daug2MassHypo);
    declareProperty("Vtx1Daug3MassHypo",        m_vtx1Daug3MassHypo);
    declareProperty("Vtx1Daug4MassHypo",        m_vtx1Daug4MassHypo);
    declareProperty("Vtx2Daug1MassHypo",        m_vtx2Daug1MassHypo);
    declareProperty("Vtx2Daug2MassHypo",        m_vtx2Daug2MassHypo);
    declareProperty("Jpsi1Mass",                m_mass_jpsi);
    declareProperty("Jpsi2Mass",                m_mass_jpsi2);
    declareProperty("PsiMass",                  m_mass_psi);
    declareProperty("ApplyPsiMassConstraint",   m_constrPsi);
    declareProperty("ApplyJpsi1MassConstraint", m_constrJpsi);
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

  JpsiPlusPsiCascade::~JpsiPlusPsiCascade(){ }

  StatusCode JpsiPlusPsiCascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer, std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer_noConstr) const {
    ATH_MSG_DEBUG( "JpsiPlusPsiCascade::performSearch" );
    assert(cascadeinfoContainer!=nullptr);

    // Get TrackParticle container (for setting links to the original tracks)
    const xAOD::TrackParticleContainer  *trackContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(trackContainer, "InDetTrackParticles"));

    std::vector<const xAOD::TrackParticle*> tracksJpsi;
    std::vector<const xAOD::TrackParticle*> tracksJpsi2;
    std::vector<const xAOD::TrackParticle*> tracksPsi;
    std::vector<double> massesPsi;
    massesPsi.push_back(m_vtx1Daug1MassHypo);
    massesPsi.push_back(m_vtx1Daug2MassHypo);
    massesPsi.push_back(m_vtx1Daug3MassHypo);
    massesPsi.push_back(m_vtx1Daug4MassHypo);
    std::vector<double> massesJpsi;
    massesJpsi.push_back(m_vtx1Daug1MassHypo);
    massesJpsi.push_back(m_vtx1Daug2MassHypo);
    std::vector<double> massesJpsi2;
    massesJpsi2.push_back(m_vtx2Daug1MassHypo);
    massesJpsi2.push_back(m_vtx2Daug2MassHypo);

    // Get Jpsi container
    const xAOD::VertexContainer  *jpsiContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(jpsiContainer, m_vertexContainerKey));

    // Get Psi container
    const xAOD::VertexContainer  *psiContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(psiContainer, m_vertexPsiContainerKey));

    // Select the J/psi candidates before calling cascade fit
    std::vector<const xAOD::Vertex*> selectedJpsiCandidates;
    for(auto vxcItr=jpsiContainer->cbegin(); vxcItr!=jpsiContainer->cend(); ++vxcItr) {
      // Check the passed flag first
      xAOD::Vertex* vtx = *vxcItr;
      SG::AuxElement::Accessor<Char_t> flagAcc("passed_"+m_vertexJpsiHypoName);
      if(flagAcc.isAvailable(*vtx)){
	if(!flagAcc(*vtx)) continue;
      }

      // Check Jpsi candidate invariant mass and skip if need be
      double mass_jpsi2 = m_V0Tools->invariantMass(*vxcItr, massesJpsi2);
      if (mass_jpsi2 < m_jpsi2MassLower || mass_jpsi2 > m_jpsi2MassUpper) continue;
      selectedJpsiCandidates.push_back(*vxcItr);
    }
    if(selectedJpsiCandidates.size()<1) return StatusCode::SUCCESS;

    // Select the Psi candidates before calling cascade fit
    std::vector<const xAOD::Vertex*> selectedPsiCandidates;
    for(auto vxcItr=psiContainer->cbegin(); vxcItr!=psiContainer->cend(); ++vxcItr) {
      // Check the passed flag first
      xAOD::Vertex* vtx = *vxcItr;
      SG::AuxElement::Accessor<Char_t> flagAcc("passed_"+m_vertexPsiHypoName);
      if(flagAcc.isAvailable(*vtx)){
	if(!flagAcc(*vtx)) continue;
      }

      // Check Psi candidate invariant mass and skip if need be
      double mass_psi = m_V0Tools->invariantMass(*vxcItr,massesPsi);
      if(mass_psi < m_psiMassLower || mass_psi > m_psiMassUpper) continue;

      // Add loose cut on Jpsi mass from Psi -> Jpsi pi+ pi-
      TLorentzVector p4_mu1, p4_mu2;
      p4_mu1.SetPtEtaPhiM( (*vxcItr)->trackParticle(0)->pt(), 
			   (*vxcItr)->trackParticle(0)->eta(),
			   (*vxcItr)->trackParticle(0)->phi(), m_vtx1Daug1MassHypo);
      p4_mu2.SetPtEtaPhiM( (*vxcItr)->trackParticle(1)->pt(), 
			   (*vxcItr)->trackParticle(1)->eta(),
			   (*vxcItr)->trackParticle(1)->phi(), m_vtx1Daug2MassHypo);
      double mass_jpsi = (p4_mu1 + p4_mu2).M();
      if (mass_jpsi < m_jpsiMassLower || mass_jpsi > m_jpsiMassUpper) continue;
      selectedPsiCandidates.push_back(*vxcItr);
    }
    if(selectedPsiCandidates.size()<1) return StatusCode::SUCCESS;

    // Select Jpsi+Psi candidates
    // Iterate over Jpsi vertices
    for(auto jpsiItr=selectedJpsiCandidates.cbegin(); jpsiItr!=selectedJpsiCandidates.cend(); ++jpsiItr) {
      size_t jpsiTrkNum = (*jpsiItr)->nTrackParticles();
      tracksJpsi2.clear();
      for(size_t it=0; it<jpsiTrkNum; it++) tracksJpsi2.push_back((*jpsiItr)->trackParticle(it));
      if (tracksJpsi2.size() != 2 || massesJpsi2.size() != 2) {
	ATH_MSG_ERROR("problems with Jpsi input");
      }

      // Iterate over Psi vertices
      for(auto psiItr=selectedPsiCandidates.cbegin(); psiItr!=selectedPsiCandidates.cend(); ++psiItr) {
	// Check identical tracks in input
	if(std::find(tracksJpsi2.cbegin(), tracksJpsi2.cend(), (*psiItr)->trackParticle(0)) != tracksJpsi2.cend()) continue;
	if(std::find(tracksJpsi2.cbegin(), tracksJpsi2.cend(), (*psiItr)->trackParticle(1)) != tracksJpsi2.cend()) continue;
	if(std::find(tracksJpsi2.cbegin(), tracksJpsi2.cend(), (*psiItr)->trackParticle(2)) != tracksJpsi2.cend()) continue;
	if(std::find(tracksJpsi2.cbegin(), tracksJpsi2.cend(), (*psiItr)->trackParticle(3)) != tracksJpsi2.cend()) continue;

	size_t psiTrkNum = (*psiItr)->nTrackParticles();
	tracksPsi.clear();
	for(size_t it=0; it<psiTrkNum; it++) tracksPsi.push_back((*psiItr)->trackParticle(it));
	if (tracksPsi.size() != 4 || massesPsi.size() != 4 ) {
	  ATH_MSG_ERROR("problems with Psi input");
	}
	tracksJpsi.clear();
	tracksJpsi.push_back((*psiItr)->trackParticle(0));
	tracksJpsi.push_back((*psiItr)->trackParticle(1));

	TLorentzVector p4_moth;
	TLorentzVector tmp;
        for(size_t it=0; it<jpsiTrkNum; it++) {
	  tmp.SetPtEtaPhiM((*jpsiItr)->trackParticle(it)->pt(),(*jpsiItr)->trackParticle(it)->eta(),(*jpsiItr)->trackParticle(it)->phi(),massesJpsi2[it]);
	  p4_moth += tmp;
	}
	for(size_t it=0; it<psiTrkNum; it++) {
	  tmp.SetPtEtaPhiM((*psiItr)->trackParticle(it)->pt(),(*psiItr)->trackParticle(it)->eta(),(*psiItr)->trackParticle(it)->phi(),massesPsi[it]);
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
	// Psi vertex
	Trk::VertexID vID;
        // https://gitlab.cern.ch/atlas/athena/-/blob/21.2/Tracking/TrkVertexFitter/TrkVKalVrtFitter/TrkVKalVrtFitter/IVertexCascadeFitter.h
	if (m_constrPsi) {
	  vID = m_iVertexFitter->startVertex(tracksPsi,massesPsi,m_mass_psi);
	} else {
	  vID = m_iVertexFitter->startVertex(tracksPsi,massesPsi);
	}
	vrtList.push_back(vID);
	// Mother vertex including Jpsi
	Trk::VertexID vID2 = m_iVertexFitter->nextVertex(tracksJpsi2,massesJpsi,vrtList);
	if (m_constrJpsi) {
	  std::vector<Trk::VertexID> cnstV;
	  cnstV.clear();
	  if ( !m_iVertexFitter->addMassConstraint(vID,tracksJpsi,cnstV,m_mass_jpsi).isSuccess() ) {
	    ATH_MSG_WARNING("addMassConstraint for Jpsi2 failed");
	  }
	}
	if (m_constrJpsi2) {
	  std::vector<Trk::VertexID> cnstV;
	  cnstV.clear();
	  if ( !m_iVertexFitter->addMassConstraint(vID2,tracksJpsi2,cnstV,m_mass_jpsi2).isSuccess() ) {
	    ATH_MSG_WARNING("addMassConstraint for Jpsi failed");
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
	  if(m_constrJpsi || m_constrPsi || m_constrJpsi2) {
	    m_iVertexFitter->setDefault();
	    m_iVertexFitter->setRobustness(robustness);
	    std::vector<Trk::VertexID> vrtList_nc;
	    // Psi vertex
	    Trk::VertexID vID_nc = m_iVertexFitter->startVertex(tracksPsi,massesPsi);
	    vrtList_nc.push_back(vID_nc);
	    // Mother vertex including Jpsi
	    m_iVertexFitter->nextVertex(tracksJpsi2,massesJpsi,vrtList_nc);
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
      } //Iterate over Psi vertices
    } //Iterate over Jpsi vertices

    return StatusCode::SUCCESS;
  }
}
