/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// JpsiPlusV0Cascade.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/JpsiPlusV0Cascade.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include <algorithm>
#include "xAODTracking/VertexContainer.h"
#include "DerivationFrameworkBPhys/LocalVector.h"
#include "HepPDT/ParticleDataTable.hh"

namespace DerivationFramework {
    typedef ElementLink<xAOD::VertexContainer> VertexLink;
    typedef std::vector<VertexLink> VertexLinkVector;
    typedef std::vector<const xAOD::TrackParticle*> TrackBag;


    StatusCode JpsiPlusV0Cascade::initialize() {

        // retrieving vertex Fitter
        if ( m_iVertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_iVertexFitter);
        }
        
        // retrieving the V0 tools
        if ( m_V0Tools.retrieve().isFailure() ) {
          ATH_MSG_FATAL("Failed to retrieve tool " << m_V0Tools);
          return StatusCode::FAILURE;
        } else {
          ATH_MSG_INFO("Retrieved tool " << m_V0Tools);
        }

        // retrieving the Cascade tools
        if ( m_CascadeTools.retrieve().isFailure() ) {
          ATH_MSG_FATAL("Failed to retrieve tool " << m_CascadeTools);
          return StatusCode::FAILURE;
        } else {
          ATH_MSG_INFO("Retrieved tool " << m_CascadeTools);
        }

        ATH_CHECK(m_beamSpotKey.initialize());

        IPartPropSvc* partPropSvc = 0;
        StatusCode sc = service("PartPropSvc", partPropSvc, true);
        if (sc.isFailure()) {
          msg(MSG::ERROR) << "Could not initialize Particle Properties Service" << endmsg;
          return StatusCode::FAILURE;
        }
        const HepPDT::ParticleDataTable* pdt = partPropSvc->PDT();

        // retrieve particle masses
        m_mass_muon     = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
        m_mass_pion     = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus);
        m_mass_proton   = BPhysPVCascadeTools::getParticleMass(pdt, PDG::p_plus);
        m_mass_lambda   = BPhysPVCascadeTools::getParticleMass(pdt, PDG::Lambda0);
        m_mass_ks       = BPhysPVCascadeTools::getParticleMass(pdt, PDG::K_S0);
        m_mass_jpsi     = BPhysPVCascadeTools::getParticleMass(pdt, PDG::J_psi);
        m_mass_b0       = BPhysPVCascadeTools::getParticleMass(pdt, PDG::B0);
        m_mass_lambdaB  = BPhysPVCascadeTools::getParticleMass(pdt, PDG::Lambda_b0);

        return StatusCode::SUCCESS;
    }


    StatusCode JpsiPlusV0Cascade::addBranches() const
    {
      std::vector<Trk::VxCascadeInfo*> cascadeinfoContainer;
      constexpr int topoN = 2;
      std::array<xAOD::VertexContainer*, topoN> Vtxwritehandles;
      std::array<xAOD::VertexAuxContainer*, topoN> Vtxwritehandlesaux;
      if(m_cascadeOutputsKeys.size() !=topoN)  { ATH_MSG_FATAL("Incorrect number of VtxContainers"); return StatusCode::FAILURE; }

      for(int i =0; i<topoN;i++){
         Vtxwritehandles[i] = new xAOD::VertexContainer();
         Vtxwritehandlesaux[i] = new xAOD::VertexAuxContainer();
         Vtxwritehandles[i]->setStore(Vtxwritehandlesaux[i]);
         CHECK(evtStore()->record(Vtxwritehandles[i]   , m_cascadeOutputsKeys[i]       ));
         CHECK(evtStore()->record(Vtxwritehandlesaux[i], m_cascadeOutputsKeys[i] + "Aux."));
      }

      //----------------------------------------------------
      // retrieve primary vertices
      //----------------------------------------------------
      const xAOD::Vertex * primaryVertex(nullptr);
      const xAOD::VertexContainer *pvContainer(nullptr);
      CHECK(evtStore()->retrieve(pvContainer, m_VxPrimaryCandidateName));
      ATH_MSG_DEBUG("Found " << m_VxPrimaryCandidateName << " in StoreGate!");

      if (pvContainer->size()==0){
        ATH_MSG_WARNING("You have no primary vertices: " << pvContainer->size());
        return StatusCode::RECOVERABLE;
      } else {
        primaryVertex = (*pvContainer)[0];
      }

      //----------------------------------------------------
      // Try to retrieve refitted primary vertices
      //----------------------------------------------------
      xAOD::VertexContainer*    refPvContainer    = NULL;
      xAOD::VertexAuxContainer* refPvAuxContainer = NULL;
      if (m_refitPV) {
        if (evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
          // refitted PV container exists. Get it from the store gate
          CHECK(evtStore()->retrieve(refPvContainer   , m_refPVContainerName       ));
          CHECK(evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName + "Aux."));
        } else {
          // refitted PV container does not exist. Create a new one.
          refPvContainer = new xAOD::VertexContainer;
          refPvAuxContainer = new xAOD::VertexAuxContainer;
          refPvContainer->setStore(refPvAuxContainer);
          CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
          CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
        }
      }

      ATH_CHECK(performSearch(&cascadeinfoContainer));

      SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
      if(not beamSpotHandle.isValid()) ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
      BPhysPVCascadeTools helper(&(*m_CascadeTools), beamSpotHandle.cptr());
      helper.SetMinNTracksInPV(m_PV_minNTracks);

      // Decorators for the main vertex: chi2, ndf, pt and pt error, plus the V0 vertex variables
      SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks"); 
      SG::AuxElement::Decorator<VertexLinkVector> JpsiLinksDecor("JpsiVertexLinks"); 
      SG::AuxElement::Decorator<VertexLinkVector> V0LinksDecor("V0VertexLinks"); 
      SG::AuxElement::Decorator<float> chi2_decor("ChiSquared");
      SG::AuxElement::Decorator<float> ndof_decor("NumberDoF");
      SG::AuxElement::Decorator<float> Pt_decor("Pt");
      SG::AuxElement::Decorator<float> PtErr_decor("PtErr");
      SG::AuxElement::Decorator<float> Mass_svdecor("V0_mass");
      SG::AuxElement::Decorator<float> MassErr_svdecor("V0_massErr");
      SG::AuxElement::Decorator<float> Pt_svdecor("V0_Pt");
      SG::AuxElement::Decorator<float> PtErr_svdecor("V0_PtErr");
      SG::AuxElement::Decorator<float> Lxy_svdecor("V0_Lxy");
      SG::AuxElement::Decorator<float> LxyErr_svdecor("V0_LxyErr");
      SG::AuxElement::Decorator<float> Tau_svdecor("V0_Tau");
      SG::AuxElement::Decorator<float> TauErr_svdecor("V0_TauErr");

      ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer.size());

      // Get Jpsi container and identify the input Jpsi
      const xAOD::VertexContainer  *jpsiContainer(nullptr);
      CHECK(evtStore()->retrieve(jpsiContainer   , m_vertexContainerKey       ));
      const xAOD::VertexContainer  *v0Container(nullptr);
      CHECK(evtStore()->retrieve(v0Container   , m_vertexV0ContainerKey       ));

      for (Trk::VxCascadeInfo* x : cascadeinfoContainer) {
        if(x==nullptr) ATH_MSG_ERROR("cascadeinfoContainer is null");

        // the cascade fitter returns:
        // std::vector<xAOD::Vertex*>, each xAOD::Vertex contains the refitted track parameters (perigee at the vertex position)
        //   vertices[iv]              the links to the original TPs and a covariance of size 3+5*NTRK; the chi2 of the total fit
        //                             is split between the cascade vertices as per track contribution
        // std::vector< std::vector<TLorentzVector> >, each std::vector<TLorentzVector> contains the refitted momenta (TLorentzVector)
        //   momenta[iv][...]          of all tracks in the corresponding vertex, including any pseudotracks (from cascade vertices)
        //                             originating in this vertex; the masses are as assigned in the cascade fit
        // std::vector<Amg::MatrixX>,  the corresponding covariance matrices in momentum space
        //   covariance[iv]
        // int nDoF, double Chi2
        //
        // the invariant mass, pt, lifetime etc. errors should be calculated using the covariance matrices in momentum space as these
        // take into account the full track-track and track-vertex correlations
        //
        // in the case of Jpsi+V0: vertices[0] is the V0 vertex, vertices[1] is the B/Lambda_b(bar) vertex, containing the 2 Jpsi tracks.
        // The covariance terms between the two vertices are not stored. In momentum space momenta[0] contains the 2 V0 tracks,
        // their momenta add up to the momentum of the 3rd track in momenta[1], the first two being the Jpsi tracks

        const std::vector<xAOD::Vertex*> &cascadeVertices = x->vertices();
        if(cascadeVertices.size()!=topoN)
          ATH_MSG_ERROR("Incorrect number of vertices");
        if(cascadeVertices[0] == nullptr || cascadeVertices[1] == nullptr) ATH_MSG_ERROR("Error null vertex");
        // Keep vertices (bear in mind that they come in reverse order!)
        for(int i =0;i<topoN;i++) Vtxwritehandles[i]->push_back(cascadeVertices[i]);
        
        x->setSVOwnership(false); // Prevent Container from deleting vertices
        const auto mainVertex = cascadeVertices[1];   // this is the Bd (Bd, Lambda_b, Lambda_bbar) vertex
        //const auto v0Vertex = cascadeVertices[0];   // this is the V0 (Kshort, Lambda, Lambdabar) vertex
        const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();

        // Set links to cascade vertices
        std::vector<const xAOD::Vertex*> verticestoLink;
        verticestoLink.push_back(cascadeVertices[0]);
        if(Vtxwritehandles[1] == nullptr) ATH_MSG_ERROR("Vtxwritehandles[1] is null");
        if(!BPhysPVCascadeTools::LinkVertices(CascadeLinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with cascade vertices");

        // Identify the input Jpsi
        const xAOD::Vertex* jpsiVertex = BPhysPVCascadeTools::FindVertex<2>(jpsiContainer, cascadeVertices[1]);
        ATH_MSG_DEBUG("1 pt Jpsi tracks " << cascadeVertices[1]->trackParticle(0)->pt() << ", " << cascadeVertices[1]->trackParticle(1)->pt());
        if (jpsiVertex) ATH_MSG_DEBUG("2 pt Jpsi tracks " << jpsiVertex->trackParticle(0)->pt() << ", " << jpsiVertex->trackParticle(1)->pt());

        // Identify the input V0
        const xAOD::Vertex* v0Vertex = BPhysPVCascadeTools::FindVertex<2>(v0Container, cascadeVertices[0]);;
        ATH_MSG_DEBUG("1 pt V0 tracks " << cascadeVertices[0]->trackParticle(0)->pt() << ", " << cascadeVertices[0]->trackParticle(1)->pt());
        if (v0Vertex) ATH_MSG_DEBUG("2 pt V0 tracks " << v0Vertex->trackParticle(0)->pt() << ", " << v0Vertex->trackParticle(1)->pt());

        // Set links to input vertices
        std::vector<const xAOD::Vertex*> jpsiVerticestoLink;
        if (jpsiVertex) jpsiVerticestoLink.push_back(jpsiVertex);
        else ATH_MSG_WARNING("Could not find linking Jpsi");
        if(!BPhysPVCascadeTools::LinkVertices(JpsiLinksDecor, jpsiVerticestoLink, jpsiContainer, cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with Jpsi vertices");

        std::vector<const xAOD::Vertex*> v0VerticestoLink;
        if (v0Vertex) v0VerticestoLink.push_back(v0Vertex);
        else ATH_MSG_WARNING("Could not find linking V0");
        if(!BPhysPVCascadeTools::LinkVertices(V0LinksDecor, v0VerticestoLink, v0Container, cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with V0 vertices");

        double mass_v0 = m_mass_ks; 
        double mass_b = m_mass_b0;
        std::vector<double> massesJpsi(2, m_mass_muon);
        std::vector<double> massesV0;
        std::vector<double> Masses(2, m_mass_muon);
        if (m_v0_pid == 310) {
           massesV0.push_back(m_mass_pion);
           massesV0.push_back(m_mass_pion);
           Masses.push_back(m_mass_ks);
        } else if (m_v0_pid == 3122) {
           massesV0.push_back(m_mass_proton);
           massesV0.push_back(m_mass_pion);
           Masses.push_back(m_mass_lambda);
           mass_v0 = m_mass_lambda;
           mass_b = m_mass_lambdaB;
        } else if (m_v0_pid == -3122) {
           massesV0.push_back(m_mass_pion);
           massesV0.push_back(m_mass_proton);
           Masses.push_back(m_mass_lambda);
           mass_v0 = m_mass_lambda;
           mass_b = m_mass_lambdaB;
        }

        // loop over candidates -- Don't apply PV_minNTracks requirement here
        // because it may result in exclusion of the high-pt PV.
        // get good PVs

        xAOD::BPhysHypoHelper vtx(m_hypoName, mainVertex);

        BPhysPVCascadeTools::SetVectorInfo(vtx, x);
        

        // Decorate main vertex
        //
        // 1.a) mass, mass error
        BPHYS_CHECK( vtx.setMass(m_CascadeTools->invariantMass(moms[1])) );
        BPHYS_CHECK( vtx.setMassErr(m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1])) );
        // 1.b) pt and pT error (the default pt of mainVertex is != the pt of the full cascade fit!)
        Pt_decor(*mainVertex) = m_CascadeTools->pT(moms[1]);
        PtErr_decor(*mainVertex) = m_CascadeTools->pTError(moms[1],x->getCovariance()[1]);
        // 1.c) chi2 and ndof (the default chi2 of mainVertex is != the chi2 of the full cascade fit!)
        chi2_decor(*mainVertex) = x->fitChi2();
        ndof_decor(*mainVertex) = x->nDoF();

        ATH_CHECK(helper.FillCandwithRefittedVertices(m_refitPV, pvContainer, 
                                    refPvContainer, &(*m_pvRefitter), m_PV_max, m_DoVertexType, x, 1, mass_b, vtx));

        // 4) decorate the main vertex with V0 vertex mass, pt, lifetime and lxy values (plus errors) 
        // V0 points to the main vertex, so lifetime and lxy are w.r.t the main vertex
        Mass_svdecor(*mainVertex) = m_CascadeTools->invariantMass(moms[0]);
        MassErr_svdecor(*mainVertex) = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0]);
        Pt_svdecor(*mainVertex) = m_CascadeTools->pT(moms[0]);
        PtErr_svdecor(*mainVertex) = m_CascadeTools->pTError(moms[0],x->getCovariance()[0]);
        Lxy_svdecor(*mainVertex) = m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]);
        LxyErr_svdecor(*mainVertex) = m_CascadeTools->lxyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        Tau_svdecor(*mainVertex) = m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1]);
        TauErr_svdecor(*mainVertex) = m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);

        // Some checks in DEBUG mode
        ATH_MSG_DEBUG("chi2 " << x->fitChi2()
                  << " chi2_1 " << m_V0Tools->chisq(cascadeVertices[0])
                  << " chi2_2 " << m_V0Tools->chisq(cascadeVertices[1])
                  << " vprob " << m_CascadeTools->vertexProbability(x->nDoF(),x->fitChi2()));
        ATH_MSG_DEBUG("ndf " << x->nDoF() << " ndf_1 " << m_V0Tools->ndof(cascadeVertices[0]) << " ndf_2 " << m_V0Tools->ndof(cascadeVertices[1]));
        ATH_MSG_DEBUG("V0Tools mass_v0 " << m_V0Tools->invariantMass(cascadeVertices[0],massesV0)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[0],massesV0)
                   << " mass_J " << m_V0Tools->invariantMass(cascadeVertices[1],massesJpsi)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[1],massesJpsi));
        // masses and errors, using track masses assigned in the fit
        double Mass_B = m_CascadeTools->invariantMass(moms[1]);
        double Mass_V0 = m_CascadeTools->invariantMass(moms[0]);
        double Mass_B_err = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1]);
        double Mass_V0_err = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0]);
        ATH_MSG_DEBUG("Mass_B " << Mass_B << " Mass_V0 " << Mass_V0);
        ATH_MSG_DEBUG("Mass_B_err " << Mass_B_err << " Mass_V0_err " << Mass_V0_err);
        double mprob_B = m_CascadeTools->massProbability(mass_b,Mass_B,Mass_B_err);
        double mprob_V0 = m_CascadeTools->massProbability(mass_v0,Mass_V0,Mass_V0_err);
        ATH_MSG_DEBUG("mprob_B " << mprob_B << " mprob_V0 " << mprob_V0);
        // masses and errors, assigning user defined track masses
        ATH_MSG_DEBUG("Mass_b " << m_CascadeTools->invariantMass(moms[1],Masses)
                  << " Mass_v0 " << m_CascadeTools->invariantMass(moms[0],massesV0));
        ATH_MSG_DEBUG("Mass_b_err " << m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1],Masses)
                  << " Mass_v0_err " << m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0],massesV0));
        ATH_MSG_DEBUG("pt_b " << m_CascadeTools->pT(moms[1])
                  << " pt_v " << m_CascadeTools->pT(moms[0])
                  << " pt_v0 " << m_V0Tools->pT(cascadeVertices[0]));
        ATH_MSG_DEBUG("ptErr_b " << m_CascadeTools->pTError(moms[1],x->getCovariance()[1])
                  << " ptErr_v " << m_CascadeTools->pTError(moms[0],x->getCovariance()[0])
                  << " ptErr_v0 " << m_V0Tools->pTError(cascadeVertices[0]));
        ATH_MSG_DEBUG("lxy_B " << m_V0Tools->lxy(cascadeVertices[1],primaryVertex) << " lxy_V " << m_V0Tools->lxy(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("lxy_b " << m_CascadeTools->lxy(moms[1],cascadeVertices[1],primaryVertex) << " lxy_v " << m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("lxyErr_b " << m_CascadeTools->lxyError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex)
                  << " lxyErr_v " << m_CascadeTools->lxyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1])
                  << " lxyErr_v0 " << m_V0Tools->lxyError(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("tau_B " << m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex,mass_b)
                   << " tau_v0 " << m_V0Tools->tau(cascadeVertices[0],cascadeVertices[1],massesV0));
        ATH_MSG_DEBUG("tau_b " << m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex)
                   << " tau_v " << m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1])
                   << " tau_V " << m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1],mass_v0));
        ATH_MSG_DEBUG("tauErr_b " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex)
                  << " tauErr_v " << m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1])
                  << " tauErr_v0 " << m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesV0));
        ATH_MSG_DEBUG("TauErr_b " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex,mass_b)
                  << " TauErr_v " << m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1],mass_v0)
                  << " TauErr_v0 " << m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesV0,mass_v0));

        ATH_MSG_DEBUG("CascadeTools main vert wrt PV " << " CascadeTools SV " << " V0Tools SV");
        ATH_MSG_DEBUG("a0z " << m_CascadeTools->a0z(moms[1],cascadeVertices[1],primaryVertex)
                   << ", " << m_CascadeTools->a0z(moms[0],cascadeVertices[0],cascadeVertices[1])
                   << ", " << m_V0Tools->a0z(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("a0zErr " << m_CascadeTools->a0zError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex)
                   << ", " << m_CascadeTools->a0zError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1])
                   << ", " << m_V0Tools->a0zError(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("a0xy " << m_CascadeTools->a0xy(moms[1],cascadeVertices[1],primaryVertex)
                   << ", " << m_CascadeTools->a0xy(moms[0],cascadeVertices[0],cascadeVertices[1])
                   << ", " << m_V0Tools->a0xy(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("a0xyErr " << m_CascadeTools->a0xyError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex)
                   << ", " << m_CascadeTools->a0xyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1])
                   << ", " << m_V0Tools->a0xyError(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("a0 " << m_CascadeTools->a0(moms[1],cascadeVertices[1],primaryVertex)
                   << ", " << m_CascadeTools->a0(moms[0],cascadeVertices[0],cascadeVertices[1])
                   << ", " << m_V0Tools->a0(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("a0Err " << m_CascadeTools->a0Error(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex)
                   << ", " << m_CascadeTools->a0Error(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1])
                   << ", " << m_V0Tools->a0Error(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("x0 " << m_V0Tools->vtx(cascadeVertices[0]).x() << " y0 " << m_V0Tools->vtx(cascadeVertices[0]).y() << " z0 " << m_V0Tools->vtx(cascadeVertices[0]).z());
        ATH_MSG_DEBUG("x1 " << m_V0Tools->vtx(cascadeVertices[1]).x() << " y1 " << m_V0Tools->vtx(cascadeVertices[1]).y() << " z1 " << m_V0Tools->vtx(cascadeVertices[1]).z());
        ATH_MSG_DEBUG("X0 " << primaryVertex->x() << " Y0 " << primaryVertex->y() << " Z0 " << primaryVertex->z());
        ATH_MSG_DEBUG("rxy0 " << m_V0Tools->rxy(cascadeVertices[0]) << " rxyErr0 " << m_V0Tools->rxyError(cascadeVertices[0]));
        ATH_MSG_DEBUG("rxy1 " << m_V0Tools->rxy(cascadeVertices[1]) << " rxyErr1 " << m_V0Tools->rxyError(cascadeVertices[1]));
        ATH_MSG_DEBUG("Rxy0 wrt PV " << m_V0Tools->rxy(cascadeVertices[0],primaryVertex) << " RxyErr0 wrt PV " << m_V0Tools->rxyError(cascadeVertices[0],primaryVertex));
        ATH_MSG_DEBUG("Rxy1 wrt PV " << m_V0Tools->rxy(cascadeVertices[1],primaryVertex) << " RxyErr1 wrt PV " << m_V0Tools->rxyError(cascadeVertices[1],primaryVertex));
        ATH_MSG_DEBUG("number of covariance matrices " << (x->getCovariance()).size());
        //const Amg::MatrixX& cov30 = (cascadeVertices[0])->covariancePosition();
        //const Amg::MatrixX& cov31 = (cascadeVertices[1])->covariancePosition();
        //ATH_MSG_DEBUG("cov30 " << cov30);
        //ATH_MSG_DEBUG("cov31 " << cov31);


      } // loop over cascadeinfoContainer

      // Deleting cascadeinfo since this won't be stored.
      // Vertices have been kept in m_cascadeOutputs and should be owned by their container
      for (auto x : cascadeinfoContainer) delete x;

      return StatusCode::SUCCESS;
    }


    JpsiPlusV0Cascade::JpsiPlusV0Cascade(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_vertexContainerKey(""),
    m_vertexV0ContainerKey(""),
    m_cascadeOutputsKeys{ "JpsiPlusV0CascadeVtx1", "JpsiPlusV0CascadeVtx2" },
    m_VxPrimaryCandidateName("PrimaryVertices"),
    m_jpsiMassLower(0.0),
    m_jpsiMassUpper(10000.0),
    m_V0MassLower(0.0),
    m_V0MassUpper(10000.0),
    m_MassLower(0.0),
    m_MassUpper(20000.0),
    m_mass_muon   ( 0 ),
    m_mass_pion   ( 0 ),
    m_mass_proton ( 0 ),
    m_mass_lambda ( 0 ),
    m_mass_ks     ( 0 ),
    m_mass_jpsi   ( 0 ),
    m_mass_b0     ( 0 ),
    m_mass_lambdaB( 0 ),
    m_v0_pid(310),
    m_constrV0(true),
    m_constrJpsi(true),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_V0Tools("Trk::V0Tools"),
    m_CascadeTools("DerivationFramework::CascadeTools")
    {
       declareProperty("JpsiVertices", m_vertexContainerKey);
       declareProperty("V0Vertices", m_vertexV0ContainerKey);
       declareProperty("VxPrimaryCandidateName", m_VxPrimaryCandidateName);
       declareProperty("RefPVContainerName", m_refPVContainerName  = "RefittedPrimaryVertices");
       declareProperty("JpsiMassLowerCut", m_jpsiMassLower);
       declareProperty("JpsiMassUpperCut", m_jpsiMassUpper);
       declareProperty("V0MassLowerCut", m_V0MassLower);
       declareProperty("V0MassUpperCut", m_V0MassUpper);
       declareProperty("MassLowerCut", m_MassLower);
       declareProperty("MassUpperCut", m_MassUpper);
       declareProperty("HypothesisName",            m_hypoName               = "Bd");
       declareProperty("V0Hypothesis",              m_v0_pid);
       declareProperty("ApplyV0MassConstraint",     m_constrV0);
       declareProperty("ApplyJpsiMassConstraint",   m_constrJpsi);
       declareProperty("RefitPV",                   m_refitPV                = true);
       declareProperty("MaxnPV",                    m_PV_max                 = 999);
       declareProperty("MinNTracksInPV",            m_PV_minNTracks          = 0);
       declareProperty("DoVertexType",              m_DoVertexType           = 7);
       declareProperty("TrkVertexFitterTool",       m_iVertexFitter);
       declareProperty("PVRefitter",                m_pvRefitter);
       declareProperty("V0Tools",                   m_V0Tools);
       declareProperty("CascadeTools",              m_CascadeTools);
       declareProperty("CascadeVertexCollections",  m_cascadeOutputsKeys);
    }

    JpsiPlusV0Cascade::~JpsiPlusV0Cascade(){ }

    StatusCode JpsiPlusV0Cascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer) const
    {
        ATH_MSG_DEBUG( "JpsiPlusV0Cascade::performSearch" );
        assert(cascadeinfoContainer!=nullptr);

        // Get TrackParticle container (for setting links to the original tracks)
        const xAOD::TrackParticleContainer  *trackContainer(nullptr);
        CHECK(evtStore()->retrieve(trackContainer   , "InDetTrackParticles"      ));

        // Get Jpsi container
        const xAOD::VertexContainer  *jpsiContainer(nullptr);
        CHECK(evtStore()->retrieve(jpsiContainer   , m_vertexContainerKey       ));

        // Get V0 container
        const xAOD::VertexContainer  *v0Container(nullptr);
        CHECK(evtStore()->retrieve(v0Container   , m_vertexV0ContainerKey       ));

        double mass_v0 = m_mass_ks; 
        std::vector<const xAOD::TrackParticle*> tracksJpsi;
        std::vector<const xAOD::TrackParticle*> tracksV0;
        std::vector<double> massesJpsi(2, m_mass_muon);
        std::vector<double> massesV0;
        std::vector<double> Masses(2, m_mass_muon);
        if (m_v0_pid == 310) {
           massesV0.push_back(m_mass_pion);
           massesV0.push_back(m_mass_pion);
           Masses.push_back(m_mass_ks);
        } else if (m_v0_pid == 3122) {
           massesV0.push_back(m_mass_proton);
           massesV0.push_back(m_mass_pion);
           mass_v0 = m_mass_lambda;
           Masses.push_back(m_mass_lambda);
        } else if (m_v0_pid == -3122) {
           massesV0.push_back(m_mass_pion);
           massesV0.push_back(m_mass_proton);
           mass_v0 = m_mass_lambda;
           Masses.push_back(m_mass_lambda);
        }

        for(auto jpsi : *jpsiContainer) { //Iterate over Jpsi vertices

           size_t jpsiTrkNum = jpsi->nTrackParticles();
           tracksJpsi.clear();
           for( unsigned int it=0; it<jpsiTrkNum; it++) tracksJpsi.push_back(jpsi->trackParticle(it));

           if (tracksJpsi.size() != 2 || massesJpsi.size() != 2 ) {
             ATH_MSG_INFO("problems with Jpsi input");
           }
           double mass_Jpsi = m_V0Tools->invariantMass(jpsi,massesJpsi);
           ATH_MSG_DEBUG("Jpsi mass " << mass_Jpsi);
           if (mass_Jpsi < m_jpsiMassLower || mass_Jpsi > m_jpsiMassUpper) {
             ATH_MSG_DEBUG(" Original Jpsi candidate rejected by the mass cut: mass = "
                           << mass_Jpsi << " != (" << m_jpsiMassLower << ", " << m_jpsiMassUpper << ")" );
             continue;
           }

           for(auto v0 : *v0Container) { //Iterate over V0 vertices

              size_t v0TrkNum = v0->nTrackParticles();
              tracksV0.clear();
              for( unsigned int it=0; it<v0TrkNum; it++) tracksV0.push_back(v0->trackParticle(it));
              if (tracksV0.size() != 2 || massesV0.size() != 2 ) {
                ATH_MSG_INFO("problems with V0 input");
              }
              double mass_V0 = m_V0Tools->invariantMass(v0,massesV0);
              ATH_MSG_DEBUG("V0 mass " << mass_V0);
              if (mass_V0 < m_V0MassLower || mass_V0 > m_V0MassUpper) {
                 ATH_MSG_DEBUG(" Original V0 candidate rejected by the mass cut: mass = "
                               << mass_V0 << " != (" << m_V0MassLower << ", " << m_V0MassUpper << ")" );
                continue;
              }
              ATH_MSG_DEBUG("using tracks" << tracksJpsi[0] << ", " << tracksJpsi[1] << ", " << tracksV0[0] << ", " << tracksV0[1]);
              if(!BPhysPVCascadeTools::uniqueCollection(tracksJpsi, tracksV0)) continue;


              //if (std::find(trackContainer->begin(), trackContainer->end(), tracksJpsi[0]) == trackContainer->end()) {
              //   ATH_MSG_ERROR("Track is not in standard container");
              //} else {
              //   ATH_MSG_DEBUG("Track " << tracksJpsi[0] << " is at position " << std::distance(trackContainer->begin(), std::find(trackContainer->begin(), trackContainer->end(), tracksJpsi[0])) );
              //}
              //ATH_MSG_DEBUG("using tracks " << tracksJpsi[0] << ", " << tracksJpsi[1] << ", " << tracksV0[0] << ", " << tracksV0[1]);

              // Apply the user's settings to the fitter
              // Reset
              std::unique_ptr<Trk::IVKalState> state = m_iVertexFitter->makeState();
              // Robustness
              int robustness = 0;
              m_iVertexFitter->setRobustness(robustness, *state);
              // Build up the topology
              // Vertex list
              std::vector<Trk::VertexID> vrtList;
              // V0 vertex
              Trk::VertexID vID;
              if (m_constrV0) {
                vID = m_iVertexFitter->startVertex(tracksV0,massesV0,*state, mass_v0);
              } else {
                vID = m_iVertexFitter->startVertex(tracksV0,massesV0, *state);
              }
              vrtList.push_back(vID);
              // B vertex including Jpsi
              Trk::VertexID vID2 = m_iVertexFitter->nextVertex(tracksJpsi,massesJpsi,vrtList, *state);
              if (m_constrJpsi) {
                std::vector<Trk::VertexID> cnstV;
                cnstV.clear();
                if ( !m_iVertexFitter->addMassConstraint(vID2,tracksJpsi,cnstV,*state, m_mass_jpsi).isSuccess() ) {
                  ATH_MSG_WARNING("addMassConstraint failed");
                  //return StatusCode::FAILURE;
                }
              }
              // Do the work
              std::unique_ptr<Trk::VxCascadeInfo> result(m_iVertexFitter->fitCascade(*state));

              if (result != NULL) {
                // reset links to original tracks
                BPhysPVCascadeTools::PrepareVertexLinks(result.get(), trackContainer);

                ATH_MSG_DEBUG("storing tracks " << ((result->vertices())[0])->trackParticle(0) << ", "
                                                << ((result->vertices())[0])->trackParticle(1) << ", "
                                                << ((result->vertices())[1])->trackParticle(0) << ", "
                                                << ((result->vertices())[1])->trackParticle(1));

                // necessary to prevent memory leak
                result->setSVOwnership(true);
                const std::vector< std::vector<TLorentzVector> > &moms = result->getParticleMoms();
                if(moms.size() < 2){
                    ATH_MSG_FATAL("Incorrect size " << __FILE__ << __LINE__ );
                    return StatusCode::FAILURE;
                }
                double mass = m_CascadeTools->invariantMass(moms[1]);
                if (mass >= m_MassLower && mass <= m_MassUpper) {

                  cascadeinfoContainer->push_back(result.release());
                } else {
                  ATH_MSG_DEBUG("Candidate rejected by the mass cut: mass = "
                                << mass << " != (" << m_MassLower << ", " << m_MassUpper << ")" );
                }
              }

           } //Iterate over V0 vertices

        } //Iterate over Jpsi vertices

        ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer->size());

        return StatusCode::SUCCESS;
    }

}


