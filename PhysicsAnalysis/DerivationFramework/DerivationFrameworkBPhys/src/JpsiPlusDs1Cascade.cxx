/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// JpsiPlusDs1Cascade.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/JpsiPlusDs1Cascade.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include <algorithm>
#include "xAODTracking/VertexContainer.h"
#include "DerivationFrameworkBPhys/LocalVector.h"

namespace DerivationFramework {
    typedef ElementLink<xAOD::VertexContainer> VertexLink;
    typedef std::vector<VertexLink> VertexLinkVector;
    typedef std::vector<const xAOD::TrackParticle*> TrackBag;

    double JpsiPlusDs1Cascade::getParticleMass(int pdgcode) const{
       auto ptr = m_particleDataTable->particle( pdgcode );
       return ptr ? ptr->mass() : 0.;
    }

    StatusCode JpsiPlusDs1Cascade::initialize() {

        // retrieving vertex Fitter
        ATH_CHECK( m_iVertexFitter.retrieve());
        
        // retrieving the V0 tools
        ATH_CHECK( m_V0Tools.retrieve());

        // retrieving the Cascade tools
        ATH_CHECK( m_CascadeTools.retrieve());

        // Get the beam spot service
        ATH_CHECK(m_beamSpotKey.initialize());

        IPartPropSvc* partPropSvc = nullptr;
        ATH_CHECK( service("PartPropSvc", partPropSvc, true) );
        m_particleDataTable = partPropSvc->PDT();

        // retrieve particle masses
        if(m_mass_jpsi < 0. ) m_mass_jpsi = getParticleMass(PDG::J_psi);
        if(m_vtx0MassHypo < 0.) m_vtx0MassHypo = getParticleMass(PDG::B_c_plus);
        if(m_vtx1MassHypo < 0.) m_vtx1MassHypo = getParticleMass(PDG::D0);
        if(m_vtx2MassHypo < 0.) m_vtx2MassHypo = getParticleMass(PDG::K_S0);

        if(m_vtx0Daug1MassHypo < 0.) m_vtx0Daug1MassHypo = getParticleMass(PDG::mu_minus);
        if(m_vtx0Daug2MassHypo < 0.) m_vtx0Daug2MassHypo = getParticleMass(PDG::mu_minus);
        if(m_vtx0Daug3MassHypo < 0.) m_vtx0Daug3MassHypo = getParticleMass(PDG::pi_plus);
        if(m_vtx1Daug1MassHypo < 0.) m_vtx1Daug1MassHypo = getParticleMass(PDG::pi_plus);
        if(m_vtx1Daug2MassHypo < 0.) m_vtx1Daug2MassHypo = getParticleMass(PDG::K_plus);
        if(m_vtx2Daug1MassHypo < 0.) m_vtx2Daug1MassHypo = getParticleMass(PDG::pi_plus);
        if(m_vtx2Daug2MassHypo < 0.) m_vtx2Daug2MassHypo = getParticleMass(PDG::pi_plus);

        return StatusCode::SUCCESS;
    }


    StatusCode JpsiPlusDs1Cascade::addBranches() const
    {
      std::vector<Trk::VxCascadeInfo*> cascadeinfoContainer;
      constexpr int topoN = 3;
      std::array<xAOD::VertexContainer*, topoN> Vtxwritehandles;
      std::array<xAOD::VertexAuxContainer*, topoN> Vtxwritehandlesaux;
      if(m_cascadeOutputsKeys.size() !=topoN)  { ATH_MSG_FATAL("Incorrect number of VtxContainers"); return StatusCode::FAILURE; }

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
      const xAOD::Vertex * primaryVertex(nullptr);
      const xAOD::VertexContainer *pvContainer(nullptr);
      ATH_CHECK(evtStore()->retrieve(pvContainer, m_VxPrimaryCandidateName));
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
          ATH_CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
        }
      }

      ATH_CHECK(performSearch(&cascadeinfoContainer));

      SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
      if(not beamSpotHandle.isValid()) ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
      BPhysPVCascadeTools helper(&(*m_CascadeTools), beamSpotHandle.cptr());
      helper.SetMinNTracksInPV(m_PV_minNTracks);

      // Decorators for the main vertex: chi2, ndf, pt and pt error, plus the D0, K0 vertex variables
      SG::AuxElement::Decorator<VertexLinkVector> CascadeV1LinksDecor("CascadeVertex1Links"); 
      SG::AuxElement::Decorator<VertexLinkVector> CascadeV2LinksDecor("CascadeVertex2Links"); 
      SG::AuxElement::Decorator<VertexLinkVector> JpsipiLinksDecor("JpsipiVertexLinks"); 
      SG::AuxElement::Decorator<VertexLinkVector> D0LinksDecor("D0VertexLinks"); 
      SG::AuxElement::Decorator<VertexLinkVector> K0LinksDecor("K0VertexLinks"); 
      SG::AuxElement::Decorator<float> chi2_decor("ChiSquared");
      SG::AuxElement::Decorator<float> ndof_decor("NumberDoF");
      SG::AuxElement::Decorator<float> Pt_decor("Pt");
      SG::AuxElement::Decorator<float> PtErr_decor("PtErr");
      SG::AuxElement::Decorator<float> Mass_svdecor("D0_mass");
      SG::AuxElement::Decorator<float> MassErr_svdecor("D0_massErr");
      SG::AuxElement::Decorator<float> Pt_svdecor("D0_Pt");
      SG::AuxElement::Decorator<float> PtErr_svdecor("D0_PtErr");
      SG::AuxElement::Decorator<float> Lxy_svdecor("D0_Lxy");
      SG::AuxElement::Decorator<float> LxyErr_svdecor("D0_LxyErr");
      SG::AuxElement::Decorator<float> Tau_svdecor("D0_Tau");
      SG::AuxElement::Decorator<float> TauErr_svdecor("D0_TauErr");

      SG::AuxElement::Decorator<float> Mass_sv2decor("K0_mass");
      SG::AuxElement::Decorator<float> MassErr_sv2decor("K0_massErr");
      SG::AuxElement::Decorator<float> Pt_sv2decor("K0_Pt");
      SG::AuxElement::Decorator<float> PtErr_sv2decor("K0_PtErr");
      SG::AuxElement::Decorator<float> Lxy_sv2decor("K0_Lxy");
      SG::AuxElement::Decorator<float> LxyErr_sv2decor("K0_LxyErr");
      SG::AuxElement::Decorator<float> Tau_sv2decor("K0_Tau");
      SG::AuxElement::Decorator<float> TauErr_sv2decor("K0_TauErr");

      SG::AuxElement::Decorator<float> MassJpsi_decor("Jpsi_mass");
      SG::AuxElement::Decorator<float> MassPiD0_decor("PiD0_mass");
      SG::AuxElement::Decorator<float> MassPiD0K0_decor("PiD0K0_mass");

      SG::AuxElement::Decorator<float> MassMumu_decor("Mumu_mass");
      SG::AuxElement::Decorator<float> MassKpi_svdecor("Kpi_mass");
      SG::AuxElement::Decorator<float> MassPipi_sv2decor("Pipi_mass");

      ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer.size());

      // Get Jpsi+pi container and identify the input Jpsi+pi
      const xAOD::VertexContainer  *jpsipiContainer(nullptr);
      ATH_CHECK(evtStore()->retrieve(jpsipiContainer   , m_vertexContainerKey       ));
      // Get D0 container and identify the input D0
      const xAOD::VertexContainer  *d0Container(nullptr);
      ATH_CHECK(evtStore()->retrieve(d0Container   , m_vertexD0ContainerKey       ));
      // Get K0 container and identify the input K0
      const xAOD::VertexContainer  *k0Container(nullptr);
      ATH_CHECK(evtStore()->retrieve(k0Container   , m_vertexK0ContainerKey       ));

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
        if(cascadeVertices[0] == nullptr || cascadeVertices[1] == nullptr || cascadeVertices[2] == nullptr) ATH_MSG_ERROR("Error null vertex");
        // Keep vertices (bear in mind that they come in reverse order!)
        for(int i =0;i<topoN;i++) Vtxwritehandles[i]->push_back(cascadeVertices[i]);
        
        x->setSVOwnership(false); // Prevent Container from deleting vertices
        const auto mainVertex = cascadeVertices[2];   // this is the B_c+/- vertex
        const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();

        // Set links to cascade vertices
        std::vector<const xAOD::Vertex*> verticestoLink;
        verticestoLink.push_back(cascadeVertices[0]);
      //if(Vtxwritehandles[1] == nullptr) ATH_MSG_ERROR("Vtxwritehandles[1] is null");
        if(Vtxwritehandles[2] == nullptr) ATH_MSG_ERROR("Vtxwritehandles[2] is null");
        if(!BPhysPVCascadeTools::LinkVertices(CascadeV1LinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with cascade vertices");

        verticestoLink.clear();
        verticestoLink.push_back(cascadeVertices[1]);
        if(!BPhysPVCascadeTools::LinkVertices(CascadeV2LinksDecor, verticestoLink, Vtxwritehandles[1], cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with cascade vertices");

        // Identify the input Jpsi+pi
        const xAOD::Vertex* jpsipiVertex = BPhysPVCascadeTools::FindVertex<3>(jpsipiContainer, cascadeVertices[2]);
        ATH_MSG_DEBUG("1 pt Jpsi+pi tracks " << cascadeVertices[2]->trackParticle(0)->pt() << ", " << cascadeVertices[2]->trackParticle(1)->pt() << ", " << cascadeVertices[2]->trackParticle(2)->pt());
        if (jpsipiVertex) ATH_MSG_DEBUG("2 pt Jpsi+pi tracks " << jpsipiVertex->trackParticle(0)->pt() << ", " << jpsipiVertex->trackParticle(1)->pt() << ", " << jpsipiVertex->trackParticle(2)->pt());

        // Identify the input D0
        const xAOD::Vertex* d0Vertex = BPhysPVCascadeTools::FindVertex<2>(d0Container, cascadeVertices[1]);;
        ATH_MSG_DEBUG("1 pt D0 tracks " << cascadeVertices[1]->trackParticle(0)->pt() << ", " << cascadeVertices[1]->trackParticle(1)->pt());
        if (d0Vertex) ATH_MSG_DEBUG("2 pt D0 tracks " << d0Vertex->trackParticle(0)->pt() << ", " << d0Vertex->trackParticle(1)->pt());

        // Identify the input K_S0
        const xAOD::Vertex* k0Vertex = BPhysPVCascadeTools::FindVertex<2>(k0Container, cascadeVertices[0]);;
        ATH_MSG_DEBUG("1 pt K_S0 tracks " << cascadeVertices[0]->trackParticle(0)->pt() << ", " << cascadeVertices[0]->trackParticle(1)->pt());
        if (k0Vertex) ATH_MSG_DEBUG("2 pt K_S0 tracks " << k0Vertex->trackParticle(0)->pt() << ", " << k0Vertex->trackParticle(1)->pt());

        // Set links to input vertices
        std::vector<const xAOD::Vertex*> jpsipiVerticestoLink;
        if (jpsipiVertex) jpsipiVerticestoLink.push_back(jpsipiVertex);
        else ATH_MSG_WARNING("Could not find linking Jpsi+pi");
        if(!BPhysPVCascadeTools::LinkVertices(JpsipiLinksDecor, jpsipiVerticestoLink, jpsipiContainer, cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with Jpsi+pi vertices");

        std::vector<const xAOD::Vertex*> d0VerticestoLink;
        if (d0Vertex) d0VerticestoLink.push_back(d0Vertex);
        else ATH_MSG_WARNING("Could not find linking D0");
        if(!BPhysPVCascadeTools::LinkVertices(D0LinksDecor, d0VerticestoLink, d0Container, cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with D0 vertices");

        std::vector<const xAOD::Vertex*> k0VerticestoLink;
        if (k0Vertex) k0VerticestoLink.push_back(k0Vertex);
        else ATH_MSG_WARNING("Could not find linking K_S0");
        if(!BPhysPVCascadeTools::LinkVertices(K0LinksDecor, k0VerticestoLink, k0Container, cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with K_S0 vertices");

        bool tagD0(true);
        if (jpsipiVertex){
          if(abs(m_Dx_pid)==421 && (jpsipiVertex->trackParticle(2)->charge()==-1)) tagD0 = false;
        }

        double mass_b = m_vtx0MassHypo;
        double mass_d0 = m_vtx1MassHypo; 
        double mass_k0 = m_vtx2MassHypo; 
        std::vector<double> massesJpsipi;
        massesJpsipi.push_back(m_vtx0Daug1MassHypo);
        massesJpsipi.push_back(m_vtx0Daug2MassHypo);
        massesJpsipi.push_back(m_vtx0Daug3MassHypo);
        std::vector<double> massesD0;
        if(tagD0){
          massesD0.push_back(m_vtx1Daug1MassHypo);
          massesD0.push_back(m_vtx1Daug2MassHypo);
        }else{ // Change the oreder of masses for D*-->D0bar pi-, D0bar->K+pi-
          massesD0.push_back(m_vtx1Daug2MassHypo);
          massesD0.push_back(m_vtx1Daug1MassHypo);
        }
        std::vector<double> massesK0;
        massesK0.push_back(m_vtx2Daug1MassHypo);
        massesK0.push_back(m_vtx2Daug2MassHypo);
        std::vector<double> Masses;
        Masses.push_back(m_vtx0Daug1MassHypo);
        Masses.push_back(m_vtx0Daug2MassHypo);
        Masses.push_back(m_vtx0Daug3MassHypo);
        Masses.push_back(m_vtx1MassHypo);
        Masses.push_back(m_vtx2MassHypo);

        // loop over candidates -- Don't apply PV_minNTracks requirement here
        // because it may result in exclusion of the high-pt PV.
        // get good PVs

        xAOD::BPhysHypoHelper vtx(m_hypoName, mainVertex);

        BPhysPVCascadeTools::SetVectorInfo(vtx, x);

        // Decorate main vertex
        //
        // 1.a) mass, mass error
        BPHYS_CHECK( vtx.setMass(m_CascadeTools->invariantMass(moms[2])) );
        BPHYS_CHECK( vtx.setMassErr(m_CascadeTools->invariantMassError(moms[2],x->getCovariance()[2])) );
        // 1.b) pt and pT error (the default pt of mainVertex is != the pt of the full cascade fit!)
        Pt_decor(*mainVertex) = m_CascadeTools->pT(moms[2]);
        PtErr_decor(*mainVertex) = m_CascadeTools->pTError(moms[2],x->getCovariance()[2]);
        // 1.c) chi2 and ndof (the default chi2 of mainVertex is != the chi2 of the full cascade fit!)
        chi2_decor(*mainVertex) = x->fitChi2();
        ndof_decor(*mainVertex) = x->nDoF();

        float massMumu = 0.;
        if (jpsipiVertex) {
          TLorentzVector  p4_mu1, p4_mu2;
          p4_mu1.SetPtEtaPhiM(jpsipiVertex->trackParticle(0)->pt(), 
                              jpsipiVertex->trackParticle(0)->eta(),
                              jpsipiVertex->trackParticle(0)->phi(), m_vtx0Daug1MassHypo); 
          p4_mu2.SetPtEtaPhiM(jpsipiVertex->trackParticle(1)->pt(), 
                              jpsipiVertex->trackParticle(1)->eta(),
                              jpsipiVertex->trackParticle(1)->phi(), m_vtx0Daug2MassHypo); 
          massMumu = (p4_mu1 + p4_mu2).M();
        }
        MassMumu_decor(*mainVertex) = massMumu;

        float massKpi = 0.;
        if (d0Vertex) {
          TLorentzVector  p4_ka, p4_pi;
          if(tagD0){
            p4_pi.SetPtEtaPhiM(d0Vertex->trackParticle(0)->pt(), 
                               d0Vertex->trackParticle(0)->eta(),
                               d0Vertex->trackParticle(0)->phi(), m_vtx1Daug1MassHypo); 
            p4_ka.SetPtEtaPhiM(d0Vertex->trackParticle(1)->pt(), 
                               d0Vertex->trackParticle(1)->eta(),
                               d0Vertex->trackParticle(1)->phi(), m_vtx1Daug2MassHypo); 
          }else{ // Change the oreder of masses for D*-->D0bar pi-, D0bar->K+pi-
            p4_pi.SetPtEtaPhiM(d0Vertex->trackParticle(1)->pt(), 
                               d0Vertex->trackParticle(1)->eta(),
                               d0Vertex->trackParticle(1)->phi(), m_vtx1Daug1MassHypo); 
            p4_ka.SetPtEtaPhiM(d0Vertex->trackParticle(0)->pt(), 
                               d0Vertex->trackParticle(0)->eta(),
                               d0Vertex->trackParticle(0)->phi(), m_vtx1Daug2MassHypo); 
          }
          massKpi = (p4_ka + p4_pi).M();
        }
        MassKpi_svdecor(*mainVertex) = massKpi;

        float massPipi = 0.;
        if (k0Vertex) {
          TLorentzVector p4_pip, p4_pim;
          p4_pip.SetPtEtaPhiM(k0Vertex->trackParticle(0)->pt(), 
                              k0Vertex->trackParticle(0)->eta(),
                              k0Vertex->trackParticle(0)->phi(), m_vtx2Daug1MassHypo); 
          p4_pim.SetPtEtaPhiM(k0Vertex->trackParticle(1)->pt(), 
                              k0Vertex->trackParticle(1)->eta(),
                              k0Vertex->trackParticle(1)->phi(), m_vtx2Daug2MassHypo); 
          massPipi = (p4_pip + p4_pim).M();
        }
        MassPipi_sv2decor(*mainVertex) = massPipi;

        MassJpsi_decor(*mainVertex) = (moms[2][0] + moms[2][1]).M();
        MassPiD0_decor(*mainVertex) = (moms[2][2] + moms[2][4]).M();
        MassPiD0K0_decor(*mainVertex) = (moms[2][2] + moms[2][4] + moms[2][3]).M();

        ATH_CHECK(helper.FillCandwithRefittedVertices(m_refitPV, pvContainer, 
                                    refPvContainer, &(*m_pvRefitter), m_PV_max, m_DoVertexType, x, 2, mass_b, vtx));

        // 4) decorate the main vertex with D0 vertex mass, pt, lifetime and lxy values (plus errors) 
        // D0 points to the main vertex, so lifetime and lxy are w.r.t the main vertex
        Mass_svdecor(*mainVertex) = m_CascadeTools->invariantMass(moms[1]);
        MassErr_svdecor(*mainVertex) = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1]);
        Pt_svdecor(*mainVertex) = m_CascadeTools->pT(moms[1]);
        PtErr_svdecor(*mainVertex) = m_CascadeTools->pTError(moms[1],x->getCovariance()[1]);
        Lxy_svdecor(*mainVertex) = m_CascadeTools->lxy(moms[1],cascadeVertices[1],cascadeVertices[2]);
        LxyErr_svdecor(*mainVertex) = m_CascadeTools->lxyError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2]);
        Tau_svdecor(*mainVertex) = m_CascadeTools->tau(moms[1],cascadeVertices[1],cascadeVertices[2]);
        TauErr_svdecor(*mainVertex) = m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2]);

        // 5) decorate the main vertex with K_S0 vertex mass, pt, lifetime and lxy values (plus errors) 
        // K_S0 points to the main vertex, so lifetime and lxy are w.r.t the main vertex
        Mass_sv2decor(*mainVertex) = m_CascadeTools->invariantMass(moms[0]);
        MassErr_sv2decor(*mainVertex) = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0]);
        Pt_sv2decor(*mainVertex) = m_CascadeTools->pT(moms[0]);
        PtErr_sv2decor(*mainVertex) = m_CascadeTools->pTError(moms[0],x->getCovariance()[0]);
        Lxy_sv2decor(*mainVertex) = m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[2]);
        LxyErr_sv2decor(*mainVertex) = m_CascadeTools->lxyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2]);
        Tau_sv2decor(*mainVertex) = m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[2]);
        TauErr_sv2decor(*mainVertex) = m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2]);

        // Some checks in DEBUG mode
        ATH_MSG_DEBUG("chi2 " << x->fitChi2()
                  << " chi2_1 " << m_V0Tools->chisq(cascadeVertices[0])
                  << " chi2_2 " << m_V0Tools->chisq(cascadeVertices[1])
                  << " chi2_3 " << m_V0Tools->chisq(cascadeVertices[2])
                  << " vprob " << m_CascadeTools->vertexProbability(x->nDoF(),x->fitChi2()));
        ATH_MSG_DEBUG("ndf " << x->nDoF() << " ndf_1 " << m_V0Tools->ndof(cascadeVertices[0]) << " ndf_2 " << m_V0Tools->ndof(cascadeVertices[1]) << " ndf_3 " << m_V0Tools->ndof(cascadeVertices[2]));
        ATH_MSG_DEBUG("V0Tools mass_k0 " << m_V0Tools->invariantMass(cascadeVertices[0],massesK0)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[0],massesK0)
                   << " mass_d0 " << m_V0Tools->invariantMass(cascadeVertices[1],massesD0)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[1],massesD0)
                   << " mass_J " << m_V0Tools->invariantMass(cascadeVertices[2],massesJpsipi)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[2],massesJpsipi));
        // masses and errors, using track masses assigned in the fit
        double Mass_B = m_CascadeTools->invariantMass(moms[2]);
        double Mass_D0 = m_CascadeTools->invariantMass(moms[1]);
        double Mass_K0 = m_CascadeTools->invariantMass(moms[0]);
        double Mass_B_err = m_CascadeTools->invariantMassError(moms[2],x->getCovariance()[2]);
        double Mass_D0_err = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1]);
        double Mass_K0_err = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0]);
        ATH_MSG_DEBUG("Mass_B " << Mass_B << " Mass_D0 " << Mass_D0 << " Mass_K0 " << Mass_K0);
        ATH_MSG_DEBUG("Mass_B_err " << Mass_B_err << " Mass_D0_err " << Mass_D0_err << " Mass_K0_err " << Mass_K0_err);
        double mprob_B = m_CascadeTools->massProbability(mass_b,Mass_B,Mass_B_err);
        double mprob_D0 = m_CascadeTools->massProbability(mass_d0,Mass_D0,Mass_D0_err);
        double mprob_K0 = m_CascadeTools->massProbability(mass_k0,Mass_K0,Mass_K0_err);
        ATH_MSG_DEBUG("mprob_B " << mprob_B << " mprob_D0 " << mprob_D0 << " mprob_K0 " << mprob_K0);
        // masses and errors, assigning user defined track masses
        ATH_MSG_DEBUG("Mass_b " << m_CascadeTools->invariantMass(moms[2],Masses)
                  << " Mass_d0 " << m_CascadeTools->invariantMass(moms[1],massesD0)
                  << " Mass_k0 " << m_CascadeTools->invariantMass(moms[0],massesD0));
        ATH_MSG_DEBUG("Mass_b_err " << m_CascadeTools->invariantMassError(moms[2],x->getCovariance()[2],Masses)
                  << " Mass_d0_err " << m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1],massesD0)
                  << " Mass_k0_err " << m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0],massesK0));
        ATH_MSG_DEBUG("pt_b " << m_CascadeTools->pT(moms[2])
                  << " pt_d " << m_CascadeTools->pT(moms[1])
                  << " pt_d0 " << m_V0Tools->pT(cascadeVertices[1])
                  << " pt_k " << m_CascadeTools->pT(moms[0])
                  << " pt_k0 " << m_V0Tools->pT(cascadeVertices[0]));
        ATH_MSG_DEBUG("ptErr_b " << m_CascadeTools->pTError(moms[2],x->getCovariance()[2])
                  << " ptErr_d " << m_CascadeTools->pTError(moms[1],x->getCovariance()[1])
                  << " ptErr_d0 " << m_V0Tools->pTError(cascadeVertices[1])
                  << " ptErr_k " << m_CascadeTools->pTError(moms[0],x->getCovariance()[0])
                  << " ptErr_k0 " << m_V0Tools->pTError(cascadeVertices[0]));
        ATH_MSG_DEBUG("lxy_B " << m_V0Tools->lxy(cascadeVertices[2],primaryVertex) << " lxy_D " << m_V0Tools->lxy(cascadeVertices[1],cascadeVertices[2]) << " lxy_K " << m_V0Tools->lxy(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("lxy_b " << m_CascadeTools->lxy(moms[2],cascadeVertices[2],primaryVertex) << " lxy_d " << m_CascadeTools->lxy(moms[1],cascadeVertices[1],cascadeVertices[2]) << " lxy_k " << m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("lxyErr_b " << m_CascadeTools->lxyError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                  << " lxyErr_d " << m_CascadeTools->lxyError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                  << " lxyErr_d0 " << m_V0Tools->lxyError(cascadeVertices[1],cascadeVertices[2])
                  << " lxyErr_k " << m_CascadeTools->lxyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2])
                  << " lxyErr_k0 " << m_V0Tools->lxyError(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("tau_B " << m_CascadeTools->tau(moms[2],cascadeVertices[2],primaryVertex,mass_b)
                   << " tau_d0 " << m_V0Tools->tau(cascadeVertices[1],cascadeVertices[2],massesD0)
                   << " tau_k0 " << m_V0Tools->tau(cascadeVertices[0],cascadeVertices[2],massesK0));
        ATH_MSG_DEBUG("tau_b " << m_CascadeTools->tau(moms[2],cascadeVertices[2],primaryVertex)
                   << " tau_d " << m_CascadeTools->tau(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << " tau_D " << m_CascadeTools->tau(moms[1],cascadeVertices[1],cascadeVertices[2],mass_d0)
                   << " tau_k " << m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[2])
                   << " tau_K " << m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[2],mass_k0));
        ATH_MSG_DEBUG("tauErr_b " << m_CascadeTools->tauError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                  << " tauErr_d " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                  << " tauErr_d0 " << m_V0Tools->tauError(cascadeVertices[1],cascadeVertices[2],massesD0)
                  << " tauErr_k " << m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2])
                  << " tauErr_k0 " << m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[2],massesK0));
        ATH_MSG_DEBUG("TauErr_b " << m_CascadeTools->tauError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex,mass_b)
                  << " TauErr_d " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2],mass_d0)
                  << " TauErr_d0 " << m_V0Tools->tauError(cascadeVertices[1],cascadeVertices[2],massesD0,mass_d0)
                  << " TauErr_k " << m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2],mass_k0)
                  << " TauErr_k0 " << m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[2],massesD0,mass_k0));

        ATH_MSG_DEBUG("CascadeTools main vert wrt PV " << " CascadeTools SV " << " V0Tools SV");
        ATH_MSG_DEBUG("a0z " << m_CascadeTools->a0z(moms[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0z(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_CascadeTools->a0z(moms[0],cascadeVertices[0],cascadeVertices[2])
                   << ", " << m_V0Tools->a0z(cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0z(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0zErr " << m_CascadeTools->a0zError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0zError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_CascadeTools->a0zError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2])
                   << ", " << m_V0Tools->a0zError(cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0zError(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0xy " << m_CascadeTools->a0xy(moms[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0xy(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_CascadeTools->a0xy(moms[0],cascadeVertices[0],cascadeVertices[2])
                   << ", " << m_V0Tools->a0xy(cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0xy(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0xyErr " << m_CascadeTools->a0xyError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0xyError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_CascadeTools->a0xyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2])
                   << ", " << m_V0Tools->a0xyError(cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0xyError(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0 " << m_CascadeTools->a0(moms[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_CascadeTools->a0(moms[0],cascadeVertices[0],cascadeVertices[2])
                   << ", " << m_V0Tools->a0(cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0Err " << m_CascadeTools->a0Error(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0Error(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_CascadeTools->a0Error(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[2])
                   << ", " << m_V0Tools->a0Error(cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0Error(cascadeVertices[0],cascadeVertices[2]));
        ATH_MSG_DEBUG("x0 " << m_V0Tools->vtx(cascadeVertices[0]).x() << " y0 " << m_V0Tools->vtx(cascadeVertices[0]).y() << " z0 " << m_V0Tools->vtx(cascadeVertices[0]).z());
        ATH_MSG_DEBUG("x1 " << m_V0Tools->vtx(cascadeVertices[1]).x() << " y1 " << m_V0Tools->vtx(cascadeVertices[1]).y() << " z1 " << m_V0Tools->vtx(cascadeVertices[1]).z());
        ATH_MSG_DEBUG("x2 " << m_V0Tools->vtx(cascadeVertices[2]).x() << " y2 " << m_V0Tools->vtx(cascadeVertices[2]).y() << " z2 " << m_V0Tools->vtx(cascadeVertices[2]).z());
        ATH_MSG_DEBUG("X0 " << primaryVertex->x() << " Y0 " << primaryVertex->y() << " Z0 " << primaryVertex->z());
        ATH_MSG_DEBUG("rxy0 " << m_V0Tools->rxy(cascadeVertices[0]) << " rxyErr0 " << m_V0Tools->rxyError(cascadeVertices[0]));
        ATH_MSG_DEBUG("rxy1 " << m_V0Tools->rxy(cascadeVertices[1]) << " rxyErr1 " << m_V0Tools->rxyError(cascadeVertices[1]));
        ATH_MSG_DEBUG("rxy2 " << m_V0Tools->rxy(cascadeVertices[2]) << " rxyErr2 " << m_V0Tools->rxyError(cascadeVertices[2]));
        ATH_MSG_DEBUG("Rxy0 wrt PV " << m_V0Tools->rxy(cascadeVertices[0],primaryVertex) << " RxyErr0 wrt PV " << m_V0Tools->rxyError(cascadeVertices[0],primaryVertex));
        ATH_MSG_DEBUG("Rxy1 wrt PV " << m_V0Tools->rxy(cascadeVertices[1],primaryVertex) << " RxyErr1 wrt PV " << m_V0Tools->rxyError(cascadeVertices[1],primaryVertex));
        ATH_MSG_DEBUG("Rxy2 wrt PV " << m_V0Tools->rxy(cascadeVertices[2],primaryVertex) << " RxyErr2 wrt PV " << m_V0Tools->rxyError(cascadeVertices[2],primaryVertex));
        ATH_MSG_DEBUG("number of covariance matrices " << (x->getCovariance()).size());
      } // loop over cascadeinfoContainer

      // Deleting cascadeinfo since this won't be stored.
      // Vertices have been kept in m_cascadeOutputs and should be owned by their container
      for (auto x : cascadeinfoContainer) delete x;

      return StatusCode::SUCCESS;
    }


    JpsiPlusDs1Cascade::JpsiPlusDs1Cascade(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_vertexContainerKey(""),
    m_vertexD0ContainerKey(""),
    m_vertexK0ContainerKey(""),
    m_cascadeOutputsKeys{ "JpsiPlusDs1CascadeVtx1", "JpsiPlusDs1CascadeVtx2", "JpsiPlusDs1CascadeVtx3" },
    m_VxPrimaryCandidateName("PrimaryVertices"),
    m_jpsiMassLower(0.0),
    m_jpsiMassUpper(10000.0),
    m_jpsipiMassLower(0.0),
    m_jpsipiMassUpper(10000.0),
    m_D0MassLower(0.0),
    m_D0MassUpper(10000.0),
    m_K0MassLower(0.0),
    m_K0MassUpper(10000.0),
    m_DstMassLower(0.0),
    m_DstMassUpper(10000.0),
    m_MassLower(0.0),
    m_MassUpper(20000.0),
    m_vtx0MassHypo(-1),
    m_vtx1MassHypo(-1),
    m_vtx2MassHypo(-1),
    m_vtx0Daug1MassHypo(-1),
    m_vtx0Daug2MassHypo(-1),
    m_vtx0Daug3MassHypo(-1),
    m_vtx1Daug1MassHypo(-1),
    m_vtx1Daug2MassHypo(-1),
    m_vtx2Daug1MassHypo(-1),
    m_vtx2Daug2MassHypo(-1),
    m_particleDataTable(nullptr),
    m_mass_jpsi(-1),
    m_Dx_pid(421),
    m_constrD0(true),
    m_constrK0(true),
    m_constrJpsi(true),
    m_chi2cut(-1.0),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_V0Tools("Trk::V0Tools"),
    m_CascadeTools("DerivationFramework::CascadeTools")
    {
       declareProperty("JpsipiVertices", m_vertexContainerKey);
       declareProperty("D0Vertices", m_vertexD0ContainerKey);
       declareProperty("K0Vertices", m_vertexK0ContainerKey);
       declareProperty("VxPrimaryCandidateName", m_VxPrimaryCandidateName);
       declareProperty("RefPVContainerName", m_refPVContainerName  = "RefittedPrimaryVertices");
       declareProperty("JpsiMassLowerCut", m_jpsiMassLower);
       declareProperty("JpsiMassUpperCut", m_jpsiMassUpper);
       declareProperty("JpsipiMassLowerCut", m_jpsipiMassLower);
       declareProperty("JpsipiMassUpperCut", m_jpsipiMassUpper);
       declareProperty("D0MassLowerCut", m_D0MassLower);
       declareProperty("D0MassUpperCut", m_D0MassUpper);
       declareProperty("K0MassLowerCut", m_K0MassLower);
       declareProperty("K0MassUpperCut", m_K0MassUpper);
       declareProperty("DstMassLowerCut", m_DstMassLower);
       declareProperty("DstMassUpperCut", m_DstMassUpper);
       declareProperty("MassLowerCut", m_MassLower);
       declareProperty("MassUpperCut", m_MassUpper);
       declareProperty("HypothesisName",            m_hypoName               = "Bc");
       declareProperty("Vtx0MassHypo",              m_vtx0MassHypo);
       declareProperty("Vtx1MassHypo",              m_vtx1MassHypo);
       declareProperty("Vtx2MassHypo",              m_vtx2MassHypo);
       declareProperty("Vtx0Daug1MassHypo",         m_vtx0Daug1MassHypo);
       declareProperty("Vtx0Daug2MassHypo",         m_vtx0Daug2MassHypo);
       declareProperty("Vtx0Daug3MassHypo",         m_vtx0Daug3MassHypo);
       declareProperty("Vtx1Daug1MassHypo",         m_vtx1Daug1MassHypo);
       declareProperty("Vtx1Daug2MassHypo",         m_vtx1Daug2MassHypo);
       declareProperty("Vtx2Daug1MassHypo",         m_vtx2Daug1MassHypo);
       declareProperty("Vtx2Daug2MassHypo",         m_vtx2Daug2MassHypo);
       declareProperty("JpsiMass",                  m_mass_jpsi);
       declareProperty("DxHypothesis",              m_Dx_pid);
       declareProperty("ApplyD0MassConstraint",     m_constrD0);
       declareProperty("ApplyK0MassConstraint",     m_constrK0);
       declareProperty("ApplyJpsiMassConstraint",   m_constrJpsi);
       declareProperty("Chi2Cut",                   m_chi2cut);
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

    JpsiPlusDs1Cascade::~JpsiPlusDs1Cascade(){ }

    StatusCode JpsiPlusDs1Cascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer) const
    {
        ATH_MSG_DEBUG( "JpsiPlusDs1Cascade::performSearch" );
        assert(cascadeinfoContainer!=nullptr);

        // Get TrackParticle container (for setting links to the original tracks)
        const xAOD::TrackParticleContainer  *trackContainer(nullptr);
        ATH_CHECK(evtStore()->retrieve(trackContainer   , "InDetTrackParticles"      ));

        // Get Jpsi+pi container
        const xAOD::VertexContainer  *jpsipiContainer(nullptr);
        ATH_CHECK(evtStore()->retrieve(jpsipiContainer   , m_vertexContainerKey       ));

        // Get D0 container
        const xAOD::VertexContainer  *d0Container(nullptr);
        ATH_CHECK(evtStore()->retrieve(d0Container   , m_vertexD0ContainerKey       ));

        // Get K_S0 container
        const xAOD::VertexContainer  *k0Container(nullptr);
        ATH_CHECK(evtStore()->retrieve(k0Container   , m_vertexK0ContainerKey       ));

        double mass_d0 = m_vtx1MassHypo; 
        double mass_k0 = m_vtx2MassHypo; 
        std::vector<const xAOD::TrackParticle*> tracksJpsipi;
        std::vector<const xAOD::TrackParticle*> tracksJpsi;
        std::vector<const xAOD::TrackParticle*> tracksD0;
        std::vector<const xAOD::TrackParticle*> tracksK0;
        std::vector<const xAOD::TrackParticle*> tracksBc;
        std::vector<double> massesJpsipi;
        massesJpsipi.push_back(m_vtx0Daug1MassHypo);
        massesJpsipi.push_back(m_vtx0Daug2MassHypo);
        massesJpsipi.push_back(m_vtx0Daug3MassHypo);
        std::vector<double> massesD0;
        massesD0.push_back(m_vtx1Daug1MassHypo);
        massesD0.push_back(m_vtx1Daug2MassHypo);
        std::vector<double> massesD0b; // Change the oreder of masses for D*-->D0bar pi-, D0bar->K+pi-
        massesD0b.push_back(m_vtx1Daug2MassHypo);
        massesD0b.push_back(m_vtx1Daug1MassHypo);
        std::vector<double> massesK0;
        massesK0.push_back(m_vtx2Daug1MassHypo);
        massesK0.push_back(m_vtx2Daug2MassHypo);
        std::vector<double> Masses;
        Masses.push_back(m_vtx0Daug1MassHypo);
        Masses.push_back(m_vtx0Daug2MassHypo);
        Masses.push_back(m_vtx0Daug3MassHypo);
        Masses.push_back(m_vtx1MassHypo);
        Masses.push_back(m_vtx2MassHypo);

        // Select J/psi pi+ candidates before calling cascade fit
        std::vector<const xAOD::Vertex*> selectedJpsipiCandidates;
        for(auto vxcItr=jpsipiContainer->cbegin(); vxcItr!=jpsipiContainer->cend(); ++vxcItr) {

           // Check the passed flag first
           const xAOD::Vertex* vtx = *vxcItr;
           SG::AuxElement::Accessor<Char_t> flagAcc1("passed_Jpsipi");
           if(flagAcc1.isAvailable(*vtx)){
              if(!flagAcc1(*vtx)) continue;
           }

           // Check J/psi candidate invariant mass and skip if need be
           TLorentzVector p4Mup_in, p4Mum_in;
           p4Mup_in.SetPtEtaPhiM((*vxcItr)->trackParticle(0)->pt(), 
                                 (*vxcItr)->trackParticle(0)->eta(),
                                 (*vxcItr)->trackParticle(0)->phi(), m_vtx0Daug1MassHypo); 
           p4Mum_in.SetPtEtaPhiM((*vxcItr)->trackParticle(1)->pt(), 
                                 (*vxcItr)->trackParticle(1)->eta(),
                                 (*vxcItr)->trackParticle(1)->phi(), m_vtx0Daug2MassHypo); 
           double mass_Jpsi = (p4Mup_in + p4Mum_in).M();
           ATH_MSG_DEBUG("Jpsi mass " << mass_Jpsi);
           if (mass_Jpsi < m_jpsiMassLower || mass_Jpsi > m_jpsiMassUpper) {
             ATH_MSG_DEBUG(" Original Jpsi candidate rejected by the mass cut: mass = "
                           << mass_Jpsi << " != (" << m_jpsiMassLower << ", " << m_jpsiMassUpper << ")" );
             continue;
           }

           // Check J/psi pi+ candidate invariant mass and skip if need be
           double mass_Jpsipi = m_V0Tools->invariantMass(*vxcItr, massesJpsipi);
           ATH_MSG_DEBUG("Jpsipi mass " << mass_Jpsipi);
           if (mass_Jpsipi < m_jpsipiMassLower || mass_Jpsipi > m_jpsipiMassUpper) {
             ATH_MSG_DEBUG(" Original Jpsipi candidate rejected by the mass cut: mass = "
                           << mass_Jpsipi << " != (" << m_jpsipiMassLower << ", " << m_jpsipiMassUpper << ")" );
             continue;
           }

           selectedJpsipiCandidates.push_back(*vxcItr);
        }
        if(selectedJpsipiCandidates.size()<1) return StatusCode::SUCCESS;

        // Select the D0/D0b candidates before calling cascade fit
        std::vector<const xAOD::Vertex*> selectedD0Candidates;
        for(auto vxcItr=d0Container->cbegin(); vxcItr!=d0Container->cend(); ++vxcItr) {

           // Check the passed flag first
           const xAOD::Vertex* vtx = *vxcItr;
           SG::AuxElement::Accessor<Char_t> flagAcc1("passed_D0");
           SG::AuxElement::Accessor<Char_t> flagAcc2("passed_D0b");
           bool isD0(true);
           bool isD0b(true);
           if(flagAcc1.isAvailable(*vtx)){
              if(!flagAcc1(*vtx)) isD0 = false;
           }
           if(flagAcc2.isAvailable(*vtx)){
              if(!flagAcc2(*vtx)) isD0b = false;
           }
           if(!(isD0||isD0b)) continue;

           // Ensure the total charge is correct
           if ((*vxcItr)->trackParticle(0)->charge() != 1 || (*vxcItr)->trackParticle(1)->charge() != -1) {
              ATH_MSG_DEBUG(" Original D0/D0-bar candidate rejected by the charge requirement: "
                              << (*vxcItr)->trackParticle(0)->charge() << ", " << (*vxcItr)->trackParticle(1)->charge() );
             continue;
           }

           // Check D0/D0bar candidate invariant mass and skip if need be
           double mass_D0 = m_V0Tools->invariantMass(*vxcItr,massesD0);
           double mass_D0b = m_V0Tools->invariantMass(*vxcItr,massesD0b);
           ATH_MSG_DEBUG("D0 mass " << mass_D0 << ", D0b mass "<<mass_D0b);
           if ((mass_D0 < m_D0MassLower || mass_D0 > m_D0MassUpper) && (mass_D0b < m_D0MassLower || mass_D0b > m_D0MassUpper)) {
              ATH_MSG_DEBUG(" Original D0 candidate rejected by the mass cut: mass = "
                            << mass_D0 << " != (" << m_D0MassLower << ", " << m_D0MassUpper << ") " 
                            << mass_D0b << " != (" << m_D0MassLower << ", " << m_D0MassUpper << ") " );
             continue;
           }

           selectedD0Candidates.push_back(*vxcItr);
        }
        if(selectedD0Candidates.size()<1) return StatusCode::SUCCESS;

        // Select the D0/D0b candidates before calling cascade fit
        std::vector<const xAOD::Vertex*> selectedK0Candidates;
        for(auto vxcItr=k0Container->cbegin(); vxcItr!=k0Container->cend(); ++vxcItr) {

           // Check the passed flag first
           const xAOD::Vertex* vtx = *vxcItr;
           SG::AuxElement::Accessor<Char_t> flagAcc1("passed_K0");
           if(flagAcc1.isAvailable(*vtx)){
              if(!flagAcc1(*vtx)) continue;
           }

           // Check K_S0 candidate invariant mass and skip if need be
           double mass_K0 = m_V0Tools->invariantMass(*vxcItr, massesK0);
           ATH_MSG_DEBUG("K_S0 mass " << mass_K0);
           if (mass_K0 < m_K0MassLower || mass_K0 > m_K0MassUpper) {
              ATH_MSG_DEBUG(" Original K_S0 candidate rejected by the mass cut: mass = "
                            << mass_K0 << " != (" << m_K0MassLower << ", " << m_K0MassUpper << ")" );
             continue;
           }

           selectedK0Candidates.push_back(*vxcItr);
        }
        if(selectedK0Candidates.size()<1) return StatusCode::SUCCESS;

        // Select J/psi D*+ candidates
        // Iterate over Jpsi+pi vertices
        for(auto jpsipiItr=selectedJpsipiCandidates.cbegin(); jpsipiItr!=selectedJpsipiCandidates.cend(); ++jpsipiItr) {

           size_t jpsipiTrkNum = (*jpsipiItr)->nTrackParticles();
           tracksJpsipi.clear();
           tracksJpsi.clear();
           for( unsigned int it=0; it<jpsipiTrkNum; it++) tracksJpsipi.push_back((*jpsipiItr)->trackParticle(it));
           for( unsigned int it=0; it<jpsipiTrkNum-1; it++) tracksJpsi.push_back((*jpsipiItr)->trackParticle(it));

           if (tracksJpsipi.size() != 3 || massesJpsipi.size() != 3 ) {
             ATH_MSG_INFO("problems with Jpsi+pi input");
           }

           bool tagD0(true);
           if(abs(m_Dx_pid)==421 && (*jpsipiItr)->trackParticle(2)->charge()==-1) tagD0 = false;

           TLorentzVector p4_pi1; // Momentum of soft pion
           p4_pi1.SetPtEtaPhiM((*jpsipiItr)->trackParticle(2)->pt(), 
                               (*jpsipiItr)->trackParticle(2)->eta(),
                               (*jpsipiItr)->trackParticle(2)->phi(), m_vtx0Daug3MassHypo); 

           // Iterate over D0/D0bar vertices
           for(auto d0Itr=selectedD0Candidates.cbegin(); d0Itr!=selectedD0Candidates.cend(); ++d0Itr) {

              // Check identical tracks in input
              if(std::find(tracksJpsipi.cbegin(), tracksJpsipi.cend(), (*d0Itr)->trackParticle(0)) != tracksJpsipi.cend()) continue; 
              if(std::find(tracksJpsipi.cbegin(), tracksJpsipi.cend(), (*d0Itr)->trackParticle(1)) != tracksJpsipi.cend()) continue; 

              TLorentzVector p4_ka, p4_pi2;
              if(tagD0){ // for D*+
                p4_pi2.SetPtEtaPhiM((*d0Itr)->trackParticle(0)->pt(), 
                                    (*d0Itr)->trackParticle(0)->eta(),
                                    (*d0Itr)->trackParticle(0)->phi(), m_vtx1Daug1MassHypo); 
                p4_ka.SetPtEtaPhiM( (*d0Itr)->trackParticle(1)->pt(), 
                                    (*d0Itr)->trackParticle(1)->eta(),
                                    (*d0Itr)->trackParticle(1)->phi(), m_vtx1Daug2MassHypo); 
              }else{ // change the order in the case of D*-
                p4_pi2.SetPtEtaPhiM((*d0Itr)->trackParticle(1)->pt(), 
                                    (*d0Itr)->trackParticle(1)->eta(),
                                    (*d0Itr)->trackParticle(1)->phi(), m_vtx1Daug1MassHypo); 
                p4_ka.SetPtEtaPhiM( (*d0Itr)->trackParticle(0)->pt(), 
                                    (*d0Itr)->trackParticle(0)->eta(),
                                    (*d0Itr)->trackParticle(0)->phi(), m_vtx1Daug2MassHypo); 
              }
              // Check D*+/- candidate invariant mass and skip if need be
              double mass_Dst= (p4_pi1 + p4_ka + p4_pi2).M();
              ATH_MSG_DEBUG("D*+/- mass " << mass_Dst);
              if (mass_Dst < m_DstMassLower || mass_Dst > m_DstMassUpper) {
                ATH_MSG_DEBUG(" Original D*+/- candidate rejected by the mass cut: mass = "
                              << mass_Dst << " != (" << m_DstMassLower << ", " << m_DstMassUpper << ")" );
                continue;
              }

              size_t d0TrkNum = (*d0Itr)->nTrackParticles();
              tracksD0.clear();
              for( unsigned int it=0; it<d0TrkNum; it++) tracksD0.push_back((*d0Itr)->trackParticle(it));
              if (tracksD0.size() != 2 || massesD0.size() != 2 ) {
                ATH_MSG_INFO("problems with D0 input");
              }
             
              // Iterate over K0 vertices
              for(auto k0Itr=selectedK0Candidates.cbegin(); k0Itr!=selectedK0Candidates.cend(); ++k0Itr) {
              
                 // Check identical tracks in input
                 if(std::find(tracksJpsipi.cbegin(), tracksJpsipi.cend(), (*k0Itr)->trackParticle(0)) != tracksJpsipi.cend()) continue; 
                 if(std::find(tracksJpsipi.cbegin(), tracksJpsipi.cend(), (*k0Itr)->trackParticle(1)) != tracksJpsipi.cend()) continue; 
                 if(std::find(tracksD0.cbegin(), tracksD0.cend(), (*k0Itr)->trackParticle(0)) != tracksD0.cend()) continue; 
                 if(std::find(tracksD0.cbegin(), tracksD0.cend(), (*k0Itr)->trackParticle(1)) != tracksD0.cend()) continue; 
             
                 size_t k0TrkNum = (*k0Itr)->nTrackParticles();
                 tracksK0.clear();
                 for( unsigned int it=0; it<k0TrkNum; it++) tracksK0.push_back((*k0Itr)->trackParticle(it));
                 if (tracksK0.size() != 2 || massesK0.size() != 2 ) {
                   ATH_MSG_INFO("problems with K0 input");
                 }

                 ATH_MSG_DEBUG("using tracks" << tracksJpsipi[0] << ", " << tracksJpsipi[1] << ", " << tracksJpsipi[2] << ", " << tracksD0[0] << ", " << tracksD0[1] << ", " << tracksK0[0] << ", " << tracksK0[1]);

                 tracksBc.clear();
                 for( unsigned int it=0; it<jpsipiTrkNum; it++) tracksBc.push_back((*jpsipiItr)->trackParticle(it));
                 for( unsigned int it=0; it<d0TrkNum; it++) tracksBc.push_back((*d0Itr)->trackParticle(it));
                 for( unsigned int it=0; it<k0TrkNum; it++) tracksBc.push_back((*k0Itr)->trackParticle(it));
                 
             
                 // Apply the user's settings to the fitter
                 // Reset
                 std::unique_ptr<Trk::IVKalState> state (m_iVertexFitter->makeState());
                 // Robustness
                 int robustness = 0;
                 m_iVertexFitter->setRobustness(robustness, *state);
                 // Build up the topology
                 // Vertex list
                 std::vector<Trk::VertexID> vrtList;
                 // K_S0 vertex
                 Trk::VertexID vK0ID;
                 if (m_constrK0) {
                   vK0ID = m_iVertexFitter->startVertex(tracksK0,massesK0, *state, mass_k0);
                 } else {
                   vK0ID = m_iVertexFitter->startVertex(tracksK0,massesK0, *state);
                 }
                 vrtList.push_back(vK0ID);
                 // D0 vertex
                 Trk::VertexID vD0ID;
                 if (m_constrD0) {
                   if(tagD0) vD0ID = m_iVertexFitter->nextVertex(tracksD0,massesD0, *state, mass_d0);
                   else vD0ID = m_iVertexFitter->nextVertex(tracksD0,massesD0b, *state, mass_d0);
                 } else {
                   if(tagD0) vD0ID = m_iVertexFitter->nextVertex(tracksD0,massesD0, *state);
                   else vD0ID = m_iVertexFitter->nextVertex(tracksD0,massesD0b, *state);
                 }
                 vrtList.push_back(vD0ID);
                 // B vertex including Jpsi+pi
                 Trk::VertexID vBcID = m_iVertexFitter->nextVertex(tracksJpsipi,massesJpsipi,vrtList, *state);
                 if (m_constrJpsi) {
                   std::vector<Trk::VertexID> cnstV;
                   cnstV.clear();
                   if ( !m_iVertexFitter->addMassConstraint(vBcID,tracksJpsi,cnstV, *state, m_mass_jpsi).isSuccess() ) {
                     ATH_MSG_WARNING("addMassConstraint failed");
                     //return StatusCode::FAILURE;
                   }
                 }
                 // Do the work
                 std::unique_ptr<Trk::VxCascadeInfo> result(m_iVertexFitter->fitCascade(*state));
             
                 if (result != nullptr) {

                   // reset links to original tracks
                   BPhysPVCascadeTools::PrepareVertexLinks(result.get(), trackContainer);
                   ATH_MSG_DEBUG("storing tracks " << ((result->vertices())[0])->trackParticle(0) << ", "
                                                   << ((result->vertices())[0])->trackParticle(1) << ", "
                                                   << ((result->vertices())[1])->trackParticle(0) << ", "
                                                   << ((result->vertices())[1])->trackParticle(1) << ", "
                                                   << ((result->vertices())[2])->trackParticle(0) << ", "
                                                   << ((result->vertices())[2])->trackParticle(1) << ", "
                                                   << ((result->vertices())[2])->trackParticle(2));
                   // necessary to prevent memory leak
                   result->setSVOwnership(true);

                   // Chi2/DOF cut
                   double bChi2DOF = result->fitChi2()/result->nDoF();
                   ATH_MSG_DEBUG("Candidate chi2/DOF is " << bChi2DOF);
                   bool chi2CutPassed = (m_chi2cut <= 0.0 || bChi2DOF < m_chi2cut);

                   const std::vector< std::vector<TLorentzVector> > &moms = result->getParticleMoms();
                   double mass = m_CascadeTools->invariantMass(moms[2]);
                   if(chi2CutPassed) {
                     if (mass >= m_MassLower && mass <= m_MassUpper) {
                       cascadeinfoContainer->push_back(result.release());
                     } else {
                       ATH_MSG_DEBUG("Candidate rejected by the mass cut: mass = "
                                     << mass << " != (" << m_MassLower << ", " << m_MassUpper << ")" );
                     }
                   }
                 }
             
              } //Iterate over K0 vertices

           } //Iterate over D0 vertices

        } //Iterate over Jpsi+pi vertices

        ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer->size());

        return StatusCode::SUCCESS;
    }

}
