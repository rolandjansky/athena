/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// MuPlusDsCascade.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/MuPlusDsCascade.h"
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "DerivationFrameworkBPhys/LocalVector.h"
#include "xAODBPhys/BPhysHypoHelper.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include <algorithm>
#include "HepPDT/ParticleDataTable.hh"

#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include <xAODCore/AuxContainerBase.h>

#include "AthContainers/ConstDataVector.h"


namespace DerivationFramework {
    typedef ElementLink<xAOD::VertexContainer> VertexLink;
    typedef std::vector<VertexLink> VertexLinkVector;
    typedef std::vector<const xAOD::TrackParticle*> TrackBag;
    
    typedef ElementLink<xAOD::MuonContainer> MuonsLink;
    typedef std::vector<MuonsLink> MuonsLinkVector;

    StatusCode MuPlusDsCascade::initialize() {

        // retrieving vertex Fitter
        ATH_CHECK( m_iVertexFitter.retrieve());
        
        // retrieving the V0 tools //from JpsiFinder
        ATH_CHECK( m_V0Tools.retrieve());
        
        // Get the track selector tool from ToolSvc
        if ( m_trkSelector.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSelector);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_trkSelector);
        }

        // retrieving the Cascade tools
        ATH_CHECK( m_CascadeTools.retrieve());

        // Get the beam spot service
        ATH_CHECK( m_beamSpotSvc.retrieve() );

        IPartPropSvc* partPropSvc = nullptr;
        ATH_CHECK( service("PartPropSvc", partPropSvc, true) );
        auto pdt = partPropSvc->PDT();
        
        // Ds+/-        : K K π
        // D+/-         : K π π
        // Lambda_c+/-  : K π p

        // retrieve particle masses
        if(m_vtx0MassHypo < 0.)
          m_vtx0MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::B_c_plus);
        if(m_vtx1MassHypo < 0.) {
          if(std::abs(m_Dx_pid) == 411) m_vtx1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::D_plus);
          if(std::abs(m_Dx_pid) == 431) m_vtx1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::D_s_plus);
          if(std::abs(m_Dx_pid) == 4122) m_vtx1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::Lambda_c_plus);
        }

        if(m_vtx0Daug1MassHypo < 0.) m_vtx0Daug1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::mu_minus);
        if(m_vtx1Daug1MassHypo < 0.) {
           if(std::abs(m_Dx_pid) == 431) m_vtx1Daug1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::K_plus); //Ds+
           else m_vtx1Daug1MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus); //D+, Lambda_c+
        }
        if(m_vtx1Daug2MassHypo < 0.) m_vtx1Daug2MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::K_plus);
        if(m_vtx1Daug3MassHypo < 0.) {
            if(std::abs(m_Dx_pid) == 4122) m_vtx1Daug3MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::p_plus); //Lambda_c+
            else m_vtx1Daug3MassHypo = BPhysPVCascadeTools::getParticleMass(pdt, PDG::pi_plus); //Ds+, D+
        }
        return StatusCode::SUCCESS;
    }


    StatusCode MuPlusDsCascade::addBranches() const
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

      BPhysPVCascadeTools helper(&(*m_CascadeTools), &m_beamSpotSvc);
      helper.SetMinNTracksInPV(m_PV_minNTracks);

      // Decorators for the main vertex: chi2, ndf, pt and pt error, plus the V0 vertex variables
      SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks"); 
      SG::AuxElement::Decorator<MuonsLinkVector> MuonsLinksDecor("MuonsLinks");
      SG::AuxElement::Decorator<VertexLinkVector> DxLinksDecor("DxVertexLinks"); 
      SG::AuxElement::Decorator<float> chi2_decor("ChiSquared");
      SG::AuxElement::Decorator<float> ndof_decor("NumberDoF");
      SG::AuxElement::Decorator<float> Pt_decor("Pt");
      SG::AuxElement::Decorator<float> PtErr_decor("PtErr");
      SG::AuxElement::Decorator<float> Mass_svdecor("Dx_mass");
      SG::AuxElement::Decorator<float> MassErr_svdecor("Dx_massErr");
      SG::AuxElement::Decorator<float> Pt_svdecor("Dx_Pt");
      SG::AuxElement::Decorator<float> PtErr_svdecor("Dx_PtErr");
      SG::AuxElement::Decorator<float> Lxy_svdecor("Dx_Lxy");
      SG::AuxElement::Decorator<float> LxyErr_svdecor("Dx_LxyErr");
      SG::AuxElement::Decorator<float> Tau_svdecor("Dx_Tau");
      SG::AuxElement::Decorator<float> TauErr_svdecor("Dx_TauErr");
      SG::AuxElement::Decorator<float> Dx_chi2_svdecor("Dx_chi2");
      SG::AuxElement::Decorator<float> Dx_ndof_svdecor("Dx_ndof");

      //                K X1 X2
      // Ds+/-        : K K  π
      // D+/-         : K π  π
      // Lambda_c+/-  : K π  p

      SG::AuxElement::Decorator<float> massKX1_svdecor("KX_mass");
      SG::AuxElement::Decorator<float> massKX1X2_svdecor("KXpi_mass");
      SG::AuxElement::Decorator<float> RapidityKX1X2_svdecor("KXpi_Rapidity");

      SG::AuxElement::Decorator<float> MuMass_decor("Mu_mass");
      SG::AuxElement::Decorator<float> MuPt_decor("Mu_pt");
      SG::AuxElement::Decorator<float> MuEta_decor("Mu_eta");
      SG::AuxElement::Decorator<float> MuChi2_decor("Mu_chi2");
      SG::AuxElement::Decorator<float> MunDoF_decor("Mu_nDoF");

      SG::AuxElement::Decorator<float> ChargeK_decor("K_charge");
      SG::AuxElement::Decorator<float> ChargeX1_decor("X_charge"); // K (Ds+) or pi (D+, Lambda_c+)
      SG::AuxElement::Decorator<float> ChargeX2_decor("Pi_charge"); // pi (Ds+, D+) or p (Lambda_c+)
      SG::AuxElement::Decorator<float> ChargeMu_decor("Mu_charge");

      ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer.size());

      // Get Muons container
      const xAOD::MuonContainer* muonContainer(nullptr);
      ATH_CHECK( evtStore()->retrieve(muonContainer, m_muonCollectionKey) );
      ATH_MSG_DEBUG("Muon container size "<<muonContainer->size());
        
      // Get D_(s)+/Lambda_c+ container and identify the input D_(s)+/Lambda_c+
      const xAOD::VertexContainer  *dxContainer(nullptr);
      ATH_CHECK(evtStore()->retrieve(dxContainer   , m_vertexDxContainerKey       ));

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

        if(cascadeVertices.size()!=topoN) ATH_MSG_ERROR("Incorrect number of vertices");
        if(cascadeVertices[0] == nullptr || cascadeVertices[1] == nullptr) ATH_MSG_ERROR("Error null vertex");
        // Keep vertices (bear in mind that they come in reverse order!)
        for(int i =0;i<topoN;i++) Vtxwritehandles[i]->push_back(cascadeVertices[i]);
        
        x->getSVOwnership(false); // Prevent Container from deleting vertices
        const auto mainVertex = cascadeVertices[1];   // this is the B_c+/- vertex
                                                      //and cascadeVertices[0] is Dx vertex
        const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();
          

        // Set links to cascade vertices
        std::vector<xAOD::Vertex*> verticestoLink;
        verticestoLink.push_back(cascadeVertices[0]);
        if(Vtxwritehandles[1] == nullptr) ATH_MSG_ERROR("Vtxwritehandles[1] is null");
        if(!BPhysPVCascadeTools::LinkVertices(CascadeLinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with cascade vertices");

        //----------------------------------- mu
        // Identify muons
        typedef std::vector<const xAOD::Muon*> MuonBag;
        MuonBag selectedMuons; selectedMuons.clear();

        for(const xAOD::Muon * mu : *muonContainer){
            const xAOD::TrackParticle* muonTrk = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle );
            if (muonTrk == cascadeVertices[1]->trackParticle(0)) selectedMuons.push_back(mu); //there is always only one muon
        }
        ATH_MSG_DEBUG("selectedMuon size "<<selectedMuons.size()); //always only one muon

        //-----------------------------------
        // Create link for Muon
        // create tmp vector of preceding vertex links
        MuonsLinkVector preMuLinks;

        // loop over input precedingMuons
        auto muItr = selectedMuons.begin();
        for(; muItr != selectedMuons.end(); muItr++) {
            // create element link
            MuonsLink muLink;
            muLink.setElement(*muItr);
            muLink.setStorableObject(*muonContainer);
            
            // sanity check : is the link valid?
            if( !muLink.isValid() ) continue;
               
            // link is OK, store it in the tmp vector
            preMuLinks.push_back( muLink );
          
        } // end of loop over preceding vertices
           
        // all OK: store preceding vertex links in the aux store
        MuonsLinksDecor(*cascadeVertices[1]) = preMuLinks;
        //-----------------------------------
        //----------------------------------- mu

        //----------------------------------- Dx / Lambda_c
        // Identify the input D_(s)+ or Lambda_c+
        xAOD::Vertex* dxVertex = BPhysPVCascadeTools::FindVertex<3>(dxContainer, cascadeVertices[0]);
        ATH_MSG_DEBUG("1 pt D_(s)+/Lambda_c+ tracks " << cascadeVertices[1]->trackParticle(0)->pt()<<", "
                      << cascadeVertices[0]->trackParticle(0)->pt()<< ", "<< cascadeVertices[0]->trackParticle(1)->pt()<< ", "
                      << cascadeVertices[0]->trackParticle(2)->pt());
        if (dxVertex) ATH_MSG_DEBUG("2 pt D_(s)+/Lambda_c+ tracks " << dxVertex->trackParticle(0)->pt() << ", "
                      << dxVertex->trackParticle(1)->pt() << ", " << dxVertex->trackParticle(2)->pt());
        // Set links to input vertices
        std::vector<xAOD::Vertex*> dxVerticestoLink;
        if (dxVertex) dxVerticestoLink.push_back(dxVertex);
        else ATH_MSG_WARNING("Could not find linking D_(s)+/Lambda_c+");
        if(!BPhysPVCascadeTools::LinkVertices(DxLinksDecor, dxVerticestoLink, dxContainer, cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with D_(s)+/Lambda_c+ vertices");

        // tag for D- and Lambda_c-
        bool tagDp(true);
        if (dxVertex) {
          if((std::abs(m_Dx_pid)==411 || std::abs(m_Dx_pid)==4122) && (dxVertex->trackParticle(2)->charge()==-1)) tagDp = false;
        }
        //----------------------------------- Dx / Lambda_c
          
        double mass_b = m_vtx0MassHypo;
        double mass_d = m_vtx1MassHypo;
        std::vector<double> massesMu;
        massesMu.push_back(m_vtx0Daug1MassHypo);        // µ
        std::vector<double> massesDx;
        if(tagDp){
          massesDx.push_back(m_vtx1Daug1MassHypo);      // K (Ds+/-) or π (D+, Lambda_c+)
          massesDx.push_back(m_vtx1Daug2MassHypo);      // K
        }else{ // Change the order for D- or Lambda_c-
          massesDx.push_back(m_vtx1Daug2MassHypo);      // K
          massesDx.push_back(m_vtx1Daug1MassHypo);      // π
        }
        massesDx.push_back(m_vtx1Daug3MassHypo);        // π (Ds+/-, D+-) or p (Lambda_c+)
        std::vector<double> Masses;
        Masses.push_back(m_vtx0Daug1MassHypo);          // µ
        Masses.push_back(m_vtx1MassHypo);               // Dx / Lambda_c

        // reset beamspot cache
        helper.GetBeamSpot(true);
        // loop over candidates -- Don't apply PV_minNTracks requirement here
        // because it may result in exclusion of the high-pt PV.
        // get good PVs


        xAOD::BPhysHypoHelper vtx(m_hypoName, mainVertex);

        // Get refitted track momenta from all vertices, charged tracks only
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
        Dx_chi2_svdecor(*mainVertex) = cascadeVertices[0]->chiSquared();
        Dx_ndof_svdecor(*mainVertex) = cascadeVertices[0]->numberDoF();
        
        //--------------------- mu
        float muM = 0., mupt = 0., mueta = 0.;
        if (selectedMuons.size()==1) {
          TLorentzVector  p4_mu1;
          p4_mu1.SetPtEtaPhiM(selectedMuons.at(0)->pt(),
                              selectedMuons.at(0)->eta(),
                              selectedMuons.at(0)->phi(), m_vtx0Daug1MassHypo);
          muM = p4_mu1.M();
          mupt = p4_mu1.Pt();
          mueta = p4_mu1.Eta();
        }
        MuMass_decor(*mainVertex) = muM;
        MuPt_decor(*mainVertex) = mupt;
        MuEta_decor(*mainVertex) = mueta;
        ChargeMu_decor(*mainVertex) = selectedMuons.at(0)->charge();
        MuChi2_decor(*mainVertex) = cascadeVertices[1]->trackParticle(0)->chiSquared();
        MunDoF_decor(*mainVertex) = cascadeVertices[1]->trackParticle(0)->numberDoF();
          
        //--------------------- Dx / Lambda_c
        //tagDp = true by default, for Ds+/-, D+ and Lambda_c+
        float massKX1 = 0.;
        float massKX1X2 = 0.;
        float RapidityKX1X2 = 0.;
        if (dxVertex) {
          TLorentzVector  p4_h1, p4_h2, p4_h3;
          if(tagDp){
            p4_h1.SetPtEtaPhiM(dxVertex->trackParticle(0)->pt(), 
                               dxVertex->trackParticle(0)->eta(),
                               dxVertex->trackParticle(0)->phi(), m_vtx1Daug1MassHypo);
            ChargeX1_decor(*mainVertex) = dxVertex->trackParticle(0)->charge();
            p4_h2.SetPtEtaPhiM(dxVertex->trackParticle(1)->pt(), 
                               dxVertex->trackParticle(1)->eta(),
                               dxVertex->trackParticle(1)->phi(), m_vtx1Daug2MassHypo);
            ChargeK_decor(*mainVertex) = dxVertex->trackParticle(1)->charge();
          }else{ // Change the order for D- and Lambda_c-
            p4_h1.SetPtEtaPhiM(dxVertex->trackParticle(0)->pt(), 
                               dxVertex->trackParticle(0)->eta(),
                               dxVertex->trackParticle(0)->phi(), m_vtx1Daug2MassHypo);
            ChargeK_decor(*mainVertex) = dxVertex->trackParticle(0)->charge();

            p4_h2.SetPtEtaPhiM(dxVertex->trackParticle(1)->pt(), 
                               dxVertex->trackParticle(1)->eta(),
                               dxVertex->trackParticle(1)->phi(), m_vtx1Daug1MassHypo);
            ChargeX1_decor(*mainVertex) = dxVertex->trackParticle(1)->charge(); // K (Ds+) or π (D+)
          }
            p4_h3.SetPtEtaPhiM(dxVertex->trackParticle(2)->pt(), 
                               dxVertex->trackParticle(2)->eta(),
                               dxVertex->trackParticle(2)->phi(), m_vtx1Daug3MassHypo);
            ChargeX2_decor(*mainVertex) = dxVertex->trackParticle(2)->charge(); // π (Ds+, D+) or p (Lambda_c+)
            
            massKX1 = (p4_h1 + p4_h2).M();
            massKX1X2 = (p4_h1 + p4_h2 + p4_h3).M();
            RapidityKX1X2 = (p4_h1 + p4_h2 + p4_h3).Rapidity();
        }
        massKX1_svdecor(*mainVertex) = massKX1;
        massKX1X2_svdecor(*mainVertex) = massKX1X2;
        RapidityKX1X2_svdecor(*mainVertex) = RapidityKX1X2;
          

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
        ATH_MSG_DEBUG("V0Tools mass_d " << m_V0Tools->invariantMass(cascadeVertices[0],massesDx)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[0],massesDx)
                   << " mass_J " << m_V0Tools->invariantMass(cascadeVertices[1],massesMu)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[1],massesMu));
        // masses and errors, using track masses assigned in the fit
        double Mass_B = m_CascadeTools->invariantMass(moms[1]);
        double Mass_D = m_CascadeTools->invariantMass(moms[0]);
        double Mass_B_err = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1]);
        double Mass_D_err = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0]);
        ATH_MSG_DEBUG("Mass_B " << Mass_B << " Mass_D " << Mass_D);
        ATH_MSG_DEBUG("Mass_B_err " << Mass_B_err << " Mass_D_err " << Mass_D_err);
        double mprob_B = m_CascadeTools->massProbability(mass_b,Mass_B,Mass_B_err);
        double mprob_D = m_CascadeTools->massProbability(mass_d,Mass_D,Mass_D_err);
        ATH_MSG_DEBUG("mprob_B " << mprob_B << " mprob_D " << mprob_D);
        // masses and errors, assigning user defined track masses
        ATH_MSG_DEBUG("Mass_b " << m_CascadeTools->invariantMass(moms[1],Masses)
                  << " Mass_d " << m_CascadeTools->invariantMass(moms[0],massesDx));
        ATH_MSG_DEBUG("Mass_b_err " << m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1],Masses)
                  << " Mass_d_err " << m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0],massesDx));
        ATH_MSG_DEBUG("pt_b " << m_CascadeTools->pT(moms[1])
                  << " pt_d " << m_CascadeTools->pT(moms[0])
                  << " pt_dp " << m_V0Tools->pT(cascadeVertices[0]));
        ATH_MSG_DEBUG("ptErr_b " << m_CascadeTools->pTError(moms[1],x->getCovariance()[1])
                  << " ptErr_d " << m_CascadeTools->pTError(moms[0],x->getCovariance()[0])
                  << " ptErr_dp " << m_V0Tools->pTError(cascadeVertices[0]));
        ATH_MSG_DEBUG("lxy_B " << m_V0Tools->lxy(cascadeVertices[1],primaryVertex) << " lxy_D " << m_V0Tools->lxy(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("lxy_b " << m_CascadeTools->lxy(moms[1],cascadeVertices[1],primaryVertex) << " lxy_d " << m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("lxyErr_b " << m_CascadeTools->lxyError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex)
                  << " lxyErr_d " << m_CascadeTools->lxyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1])
                  << " lxyErr_dp " << m_V0Tools->lxyError(cascadeVertices[0],cascadeVertices[1]));
        ATH_MSG_DEBUG("tau_B " << m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex,mass_b)
                   << " tau_dp " << m_V0Tools->tau(cascadeVertices[0],cascadeVertices[1],massesDx));
        ATH_MSG_DEBUG("tau_b " << m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex)
                   << " tau_d " << m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1])
                   << " tau_D " << m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1],mass_d));
        ATH_MSG_DEBUG("tauErr_b " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex)
                  << " tauErr_d " << m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1])
                  << " tauErr_dp " << m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesDx));
        ATH_MSG_DEBUG("TauErr_b " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex,mass_b)
                  << " TauErr_d " << m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1],mass_d)
                  << " TauErr_dp " << m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesDx,mass_d));

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
          
      } // loop over cascadeinfoContainer

      // Deleting cascadeinfo since this won't be stored.
      // Vertices have been kept in m_cascadeOutputs and should be owned by their container
      for (auto x : cascadeinfoContainer)  delete x;

      return StatusCode::SUCCESS;
    }


    MuPlusDsCascade::MuPlusDsCascade(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_vertexDxContainerKey(""),
    m_cascadeOutputsKeys{ "MuPlusDsCascadeVtx1", "MuPlusDsCascadeVtx2" },
    m_VxPrimaryCandidateName("PrimaryVertices"),
    m_DxMassLower(0.0),
    m_DxMassUpper(10000.0),
    m_MassLower(0.0),
    m_MassUpper(20000.0),
    m_vtx0MassHypo(-1),
    m_vtx1MassHypo(-1),
    m_vtx0Daug1MassHypo(-1),
    m_vtx1Daug1MassHypo(-1),
    m_vtx1Daug2MassHypo(-1),
    m_vtx1Daug3MassHypo(-1),
    m_Dx_pid(431),
    m_constrDx(true),
    m_chi2cut(-1.0),
    m_beamSpotSvc("BeamCondSvc",n),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_V0Tools("Trk::V0Tools"),
    m_CascadeTools("DerivationFramework::CascadeTools"),
    m_muonCollectionKey("StacoMuonCollection"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_thresholdPt(0.0),
    m_mcpCuts(true),
    m_combOnly(false),
    m_useCombMeasurement(false)
    {
       declareProperty("DxVertices",                m_vertexDxContainerKey);
       declareProperty("VxPrimaryCandidateName",    m_VxPrimaryCandidateName);
       declareProperty("RefPVContainerName",        m_refPVContainerName     = "RefittedPrimaryVertices");
       declareProperty("DxMassLowerCut",            m_DxMassLower);
       declareProperty("DxMassUpperCut",            m_DxMassUpper);
       declareProperty("MassLowerCut",              m_MassLower);
       declareProperty("MassUpperCut",              m_MassUpper);
       declareProperty("HypothesisName",            m_hypoName               = "Bc");
       declareProperty("Vtx0MassHypo",              m_vtx0MassHypo);
       declareProperty("Vtx1MassHypo",              m_vtx1MassHypo);
       declareProperty("Vtx0Daug1MassHypo",         m_vtx0Daug1MassHypo);
       declareProperty("Vtx1Daug1MassHypo",         m_vtx1Daug1MassHypo);
       declareProperty("Vtx1Daug2MassHypo",         m_vtx1Daug2MassHypo);
       declareProperty("Vtx1Daug3MassHypo",         m_vtx1Daug3MassHypo);
       declareProperty("DxHypothesis",              m_Dx_pid);
       declareProperty("ApplyDxMassConstraint",     m_constrDx);
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
       declareProperty("muonCollectionKey",         m_muonCollectionKey);
       declareProperty("TrackSelectorTool",         m_trkSelector);
       declareProperty("muonThresholdPt",           m_thresholdPt);
       declareProperty("useMCPCuts",                m_mcpCuts);
       declareProperty("combOnly",                  m_combOnly);
       declareProperty("useCombinedMeasurement",    m_useCombMeasurement);
    }

    MuPlusDsCascade::~MuPlusDsCascade(){ }

    StatusCode MuPlusDsCascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer) const
    {
        ATH_MSG_DEBUG( "MuPlusDsCascade::performSearch" );
        assert(cascadeinfoContainer!=nullptr);

        // Get TrackParticle container (for setting links to the original tracks)
        const xAOD::TrackParticleContainer  *trackContainer(nullptr);
        ATH_CHECK(evtStore()->retrieve(trackContainer   , "InDetTrackParticles"      ));

        // Get V0 container
        const xAOD::VertexContainer  *dxContainer(nullptr);
        ATH_CHECK(evtStore()->retrieve(dxContainer   , m_vertexDxContainerKey       ));

        double mass_d = m_vtx1MassHypo; 
        std::vector<const xAOD::TrackParticle*> tracksMu;
        std::vector<const xAOD::TrackParticle*> tracksDx;
        //std::vector<const xAOD::TrackParticle*> tracksBc;
        std::vector<double> massesMu;
        massesMu.push_back(m_vtx0Daug1MassHypo); // µ
        std::vector<double> massesDx;
        massesDx.push_back(m_vtx1Daug1MassHypo); // K (Ds+/-) or π (D+, Lambda_c+)
        massesDx.push_back(m_vtx1Daug2MassHypo); // K
        massesDx.push_back(m_vtx1Daug3MassHypo); // π (Ds+/-, D+) ot p (Lambda_c+)
        std::vector<double> massesDm; // Alter the order of masses for D- and Lambda_c-
        massesDm.push_back(m_vtx1Daug2MassHypo); // K
        massesDm.push_back(m_vtx1Daug1MassHypo); // π
        massesDm.push_back(m_vtx1Daug3MassHypo); // π (D-) or p (Lambda_c-)
        std::vector<double> Masses;
        Masses.push_back(m_vtx0Daug1MassHypo);   // µ
        Masses.push_back(m_vtx1MassHypo);        // Dx / Lambda_c
        
        //-------------------------------------------------------------------------------
        // Retrieving and selecting muons (taken from JpsiFinder.cxx)
        // Get the muons from StoreGate
        const xAOD::MuonContainer* importedMuonCollection;
        StatusCode sc = evtStore()->retrieve(importedMuonCollection,m_muonCollectionKey);

        if(sc.isFailure()){
            ATH_MSG_WARNING("No muon collection with key " << m_muonCollectionKey << " found in StoreGate");
            return StatusCode::SUCCESS;;
        }else{
            ATH_MSG_DEBUG("Found muon collections with key "<<m_muonCollectionKey);
        }
        ATH_MSG_DEBUG("Muon container size "<<importedMuonCollection->size());
        
        // Typedef for vectors of muons
        typedef std::vector<const xAOD::Muon*> MuonBag;

        // Select the muons
        const xAOD::Vertex* vx = 0;
        MuonBag theMuonsAfterSelection;
        for (auto mu : *importedMuonCollection) {
            if ( mu == nullptr ) continue;
            if (!mu->inDetTrackParticleLink().isValid()) continue; // No muons without ID tracks
            const xAOD::TrackParticle* muonTrk = *(mu->inDetTrackParticleLink());
            if ( muonTrk==nullptr) continue;
            if ( !m_trkSelector->decision(*muonTrk, vx) ) continue; // all ID tracks must pass basic tracking cuts
            if ( std::fabs(muonTrk->pt())<m_thresholdPt ) continue; // higher pt cut if needed
            if ( m_mcpCuts && !mu->passesIDCuts()) continue; // cuts of the MCP group recommendation
            if ( m_combOnly && mu->muonType() != xAOD::Muon::Combined ) continue; // require combined muons
            if ( mu->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon && !m_useCombMeasurement) continue;
                
            theMuonsAfterSelection.push_back(mu);
        }
        if (theMuonsAfterSelection.size() == 0) return StatusCode::SUCCESS;;
        ATH_MSG_DEBUG("Number of muons after selection: " << theMuonsAfterSelection.size());
        //-------------------------------------------------------------------------------

        //-------------------------------------------------------------------------------
        // Select the D_s+/D+/Lambda_c+ candidates before calling cascade fit
        std::vector<const xAOD::Vertex*> selectedDxCandidates;
        for(auto vxcItr : *dxContainer){

           // Check the passed flag first
           xAOD::Vertex* vtx = vxcItr;
           if(std::abs(m_Dx_pid)==431) { // D_s+/-
               SG::AuxElement::Accessor<Char_t> flagAcc1("passed_Ds");
               if(flagAcc1.isAvailable(*vtx)){
                  if(!flagAcc1(*vtx)) continue;
               }
           }

           if(std::abs(m_Dx_pid==411)) { // D+/-
               SG::AuxElement::Accessor<Char_t> flagAcc1("passed_Dp");
               SG::AuxElement::Accessor<Char_t> flagAcc2("passed_Dm");
               bool isDp(true);
               bool isDm(true);
               if(flagAcc1.isAvailable(*vtx)){
                  if(!flagAcc1(*vtx)) isDp = false;
               }
               if(flagAcc2.isAvailable(*vtx)){
                  if(!flagAcc2(*vtx)) isDm = false;
               }
               if(!(isDp||isDm)) continue;
           } 

           // Ensure the total charge is correct
           if( std::abs( vxcItr->trackParticle(0)->charge()+vxcItr->trackParticle(1)->charge()+vxcItr->trackParticle(2)->charge() ) != 1 ){
               ATH_MSG_DEBUG(" Original Dx/Lambda_c candidate rejected by the charge requirement: "
                              << vxcItr->trackParticle(0)->charge() << ", " << vxcItr->trackParticle(1)->charge() << ", " << vxcItr->trackParticle(2)->charge() );
               continue;
           }

           // Check D_(s)/Lambda_c +/- candidate invariant mass and skip if need be
           double mass_D;
           if( (std::abs(m_Dx_pid) == 411 || std::abs(m_Dx_pid) == 4122) && vxcItr->trackParticle(2)->charge()<0) // D- & Lambda_c-
               mass_D = m_V0Tools->invariantMass(vxcItr,massesDm);
           else // D+, D_s+/-, Lambda_c+
               mass_D = m_V0Tools->invariantMass(vxcItr,massesDx);
           ATH_MSG_DEBUG("D_(s)/Lambda_c mass " << mass_D);
           if(mass_D < m_DxMassLower || mass_D > m_DxMassUpper) {
               ATH_MSG_DEBUG(" Original D_(s)/Lambda_c candidate rejected by the mass cut: mass = "
                              << mass_D << " != (" << m_DxMassLower << ", " << m_DxMassUpper << ")" );
               continue;
           }

           // Add loose cut on phi(K+K-) mass for D_s->phi π
           if(std::abs(m_Dx_pid)==431){
              TLorentzVector p4Kp_in, p4Km_in;
              p4Kp_in.SetPtEtaPhiM( vxcItr->trackParticle(0)->pt(),
                                    vxcItr->trackParticle(0)->eta(),
                                    vxcItr->trackParticle(0)->phi(), m_vtx1Daug1MassHypo);
              p4Km_in.SetPtEtaPhiM( vxcItr->trackParticle(1)->pt(),
                                    vxcItr->trackParticle(1)->eta(),
                                    vxcItr->trackParticle(1)->phi(), m_vtx1Daug2MassHypo);
              double mass_phi = (p4Kp_in + p4Km_in).M();
              ATH_MSG_DEBUG("phi mass " << mass_phi);
              if(mass_phi > 1100) {
                  ATH_MSG_DEBUG(" Original phi candidate rejected by the mass cut: mass = " << mass_phi );
                  continue;
                  
              }
           }
            
           // Cut on pT(p) for Lambda_c
           if(std::abs(m_Dx_pid)==4122){
               if (vxcItr->trackParticle(2)->pt() < 2400) continue;
           }
            
           // L.G., 26.03.21: Check there are no previous D+/- combinations with inverse pion positions
           if(std::abs(m_Dx_pid)==411 && selectedDxCandidates.size()>0) {
               bool Dpmcopy(false);
               for(auto dxItr : selectedDxCandidates){
                   if(vxcItr->trackParticle(2)/*π*/->charge()>0) { // D+
                       if ( vxcItr->trackParticle(2) == dxItr->trackParticle(0) && vxcItr->trackParticle(0) == dxItr->trackParticle(2)
                           && vxcItr->trackParticle(1) == dxItr->trackParticle(1) ) Dpmcopy = true;
                   } else { // D-
                       if ( vxcItr->trackParticle(2) == dxItr->trackParticle(1) && vxcItr->trackParticle(1) == dxItr->trackParticle(2)
                           && vxcItr->trackParticle(0) == dxItr->trackParticle(0) ) Dpmcopy = true;
                   }
               }
               if (Dpmcopy) continue;
           }
            
           selectedDxCandidates.push_back(vxcItr);
        } // end for(auto vxcItr : dxContainer)
        if(selectedDxCandidates.size()<1) return StatusCode::SUCCESS;
        //-------------------------------------------------------------------------------
        
        //-------------------------------------------------------------------------------
        // Select mu D_(s)+/Lambda_c candidates
        // Iterate over muons
        for(auto muItr : theMuonsAfterSelection){
            tracksMu.clear();
            //Convert to trackParticle base
            auto TrkMuon = muItr->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
            tracksMu.push_back(TrkMuon);
            if (tracksMu.size() != 1 || massesMu.size() != 1 ) {
              ATH_MSG_WARNING("Problems with muon input");
            }

            // Iterate over D_(s)/Lambda_c +/-  vertices
            for(auto dxItr : selectedDxCandidates){
              // Check identical tracks in input
              if(std::find(tracksMu.cbegin(), tracksMu.cend(), dxItr->trackParticle(0)) != tracksMu.cend()) continue;
              if(std::find(tracksMu.cbegin(), tracksMu.cend(), dxItr->trackParticle(1)) != tracksMu.cend()) continue;
              if(std::find(tracksMu.cbegin(), tracksMu.cend(), dxItr->trackParticle(2)) != tracksMu.cend()) continue;
               
              size_t dxTrkNum = dxItr->nTrackParticles();
              tracksDx.clear();
              for( unsigned int it=0; it<dxTrkNum; it++) tracksDx.push_back(dxItr->trackParticle(it));
              if (tracksDx.size() != 3 || massesDx.size() != 3 ) {
                ATH_MSG_WARNING("Problems with D_(s)/Lambda_c +/- input");
              }

              if(std::find(tracksDx.cbegin(), tracksDx.cend(), TrkMuon) != tracksDx.cend()) continue; //looking for the muons in the D+ tracks
               
              ATH_MSG_DEBUG("Using tracks" << tracksMu[0] << ", " << tracksDx[0] << ", " << tracksDx[1] << ", " << tracksDx[2]);
              //tracksBc.clear();
              //tracksBc.push_back(TrkMuon);
              //for( unsigned int it=0; it<dxTrkNum; it++) tracksBc.push_back(dxItr->trackParticle(it));
                
              // Apply the user's settings to the fitter
              // Reset
              m_iVertexFitter->setDefault();
              // Robustness
              int robustness = 0;
              m_iVertexFitter->setRobustness(robustness);
              // Build up the topology
              // Vertex list
              std::vector<Trk::VertexID> vrtList;
              // D_(s)/Lambda_c +/- vertex
              Trk::VertexID vID;
              if (m_constrDx) {
                if((std::abs(m_Dx_pid)==411 || std::abs(m_Dx_pid)==4122 ) && dxItr->trackParticle(2)->charge()<0) // D-, Lambda_c-
                  vID = m_iVertexFitter->startVertex(tracksDx,massesDm,mass_d);
                else // D+, D_s+/-, Lambda_c+
                  vID = m_iVertexFitter->startVertex(tracksDx,massesDx,mass_d);
              } else {
                if((std::abs(m_Dx_pid)==411 || std::abs(m_Dx_pid)==4122 ) && dxItr->trackParticle(2)->charge()<0) // D-, Lambda_c-
                  vID = m_iVertexFitter->startVertex(tracksDx,massesDm);
                else // D+, D_s+/-, Lambda_c+
                  vID = m_iVertexFitter->startVertex(tracksDx,massesDx);
              }
              vrtList.push_back(vID);
               
              // B vertex including muon
              //For mass constraint, not used yet
              Trk::VertexID vID2 = m_iVertexFitter->nextVertex(tracksMu,massesMu,vrtList);
              ATH_MSG_DEBUG(vID2);
                
              // Do the work
              std::unique_ptr<Trk::VxCascadeInfo> result(m_iVertexFitter->fitCascade());

              if (result != nullptr) {
                // reset links to original tracks
                BPhysPVCascadeTools::PrepareVertexLinks(result.get(), trackContainer);
                ATH_MSG_DEBUG("storing tracks " << ((result->vertices())[0] /*D_(s)+*/)->trackParticle(0) << ", "
                                                << ((result->vertices())[0])->trackParticle(1) << ", "
                                                << ((result->vertices())[0])->trackParticle(2) << ", "
                                                << ((result->vertices())[1]/*mu*/)->trackParticle(0));
                                              
                // necessary to prevent memory leak
                result->getSVOwnership(true);
                  
                // Chi2/DOF cut
                double bChi2DOF = result->fitChi2()/result->nDoF();
                ATH_MSG_DEBUG("Candidate chi2/DOF is " << bChi2DOF);
                bool chi2CutPassed = (m_chi2cut <= 0.0 || bChi2DOF < m_chi2cut);

                const std::vector<xAOD::Vertex*> &cascadeVertices = result->vertices();
                const std::vector< std::vector<TLorentzVector> > &moms = result->getParticleMoms();
                  
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

                double mass = m_CascadeTools->invariantMass(moms[1]);
                if(chi2CutPassed) {
                  if (mass >= m_MassLower && mass <= m_MassUpper) {
                      if (m_CascadeTools->lxy(moms[1],cascadeVertices[1],primaryVertex) > 0.09){ //B_Lxy
                          if (m_CascadeTools->pT(moms[1]) > 9500){ //B_pT
                              if (m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]) > 0){ //Dx_lxy
                                  if (std::abs(m_Dx_pid)!=411) cascadeinfoContainer->push_back(result.release());
                                  else if (m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]) > 0.09){ //D+_Lxy
                                      cascadeinfoContainer->push_back(result.release());
                                  }//D+_Lxy
                              }//Dx_lxy
                          } //B_pT
                      }//B_Lxy
                  } else {
                    ATH_MSG_DEBUG("Candidate rejected by the mass cut: mass = "
                                  << mass << " != (" << m_MassLower << ", " << m_MassUpper << ")" );
                  }
                } // chi2CutPassed
                  
              } // result != nullptr

           } //Iterate over D_(s)/Lambda_c +/- vertices

        } //Iterate over muons

        ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer->size());

        return StatusCode::SUCCESS;
    }

}


