/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "InDetBeamSpotService/IBeamCondSvc.h"
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

    bool LinkVertices_dupl3(SG::AuxElement::Decorator<VertexLinkVector> &decor, const std::vector<xAOD::Vertex*>& vertices,
                                                 const xAOD::VertexContainer* vertexContainer, xAOD::Vertex* vert){
     // create tmp vector of preceding vertex links
     VertexLinkVector precedingVertexLinks;

     // loop over input precedingVertices  
     std::vector<xAOD::Vertex*>::const_iterator precedingVerticesItr = vertices.begin();
     for(; precedingVerticesItr!=vertices.end(); ++precedingVerticesItr) {
          // sanity check 1: protect against null pointers
          if( !(*precedingVerticesItr) )
            return false;
    
       // create element link
       VertexLink vertexLink;
       vertexLink.setElement(*precedingVerticesItr);
       vertexLink.setStorableObject(*vertexContainer);
    
       // sanity check 2: is the link valid?
       if( !vertexLink.isValid() )
          return false;
    
       // link is OK, store it in the tmp vector
       precedingVertexLinks.push_back( vertexLink );
    
     } // end of loop over preceding vertices
  
       // all OK: store preceding vertex links in the aux store
      decor(*vert) = precedingVertexLinks;
      return true;
    }

    StatusCode JpsiPlusDs1Cascade::initialize() {

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

        // Get the beam spot service
        CHECK( m_beamSpotSvc.retrieve() );

        IPartPropSvc* partPropSvc = 0;
        StatusCode sc = service("PartPropSvc", partPropSvc, true);
        if (sc.isFailure()) {
          msg(MSG::ERROR) << "Could not initialize Particle Properties Service" << endmsg;
          return StatusCode::FAILURE;
        }
        m_particleDataTable = partPropSvc->PDT();

        // retrieve particle masses
        m_mass_muon     = getParticleMass(PDG::mu_minus);
        m_mass_pion     = getParticleMass(PDG::pi_plus);
        m_mass_proton   = getParticleMass(PDG::p_plus);
        m_mass_lambda   = getParticleMass(PDG::Lambda0);
        m_mass_ks       = getParticleMass(PDG::K_S0);
        m_mass_jpsi     = getParticleMass(PDG::J_psi);
        m_mass_b0       = getParticleMass(PDG::B0);
        m_mass_lambdaB  = getParticleMass(PDG::Lambda_b0);
        m_mass_kaon     = getParticleMass(PDG::K_plus);
        m_mass_D0       = getParticleMass(PDG::D0);
        m_mass_Dpst     = getParticleMass(PDG::D_star_plus);
        m_mass_Bc       = getParticleMass(PDG::B_c_plus);

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

      BPhysPVCascadeTools helper(&(*m_CascadeTools), &m_beamSpotSvc);
      helper.SetMinNTracksInPV(m_PV_minNTracks);

      // Decorators for the main vertex: chi2, ndf, pt and pt error, plus the V0 vertex variables
      SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks"); 
      SG::AuxElement::Decorator<VertexLinkVector> JpsipiLinksDecor("JpsiVertexLinks"); 
      SG::AuxElement::Decorator<VertexLinkVector> D0LinksDecor("V0VertexLinks"); 
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

      SG::AuxElement::Decorator<float> MassMumu_decor("Mumu_massOri");
      SG::AuxElement::Decorator<float> MassKpi_svdecor("Kpi_massOri");
      SG::AuxElement::Decorator<float> MassPipi_sv2decor("Pipi_massOri");

      ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer.size());

      // Get Jpsi+pi container and identify the input Jpsi+pi
      const xAOD::VertexContainer  *jpsipiContainer(nullptr);
      CHECK(evtStore()->retrieve(jpsipiContainer   , m_vertexContainerKey       ));
      // Get D0 container and identify the input D0
      const xAOD::VertexContainer  *d0Container(nullptr);
      CHECK(evtStore()->retrieve(d0Container   , m_vertexD0ContainerKey       ));
      // Get K0 container and identify the input K0
      const xAOD::VertexContainer  *k0Container(nullptr);
      CHECK(evtStore()->retrieve(k0Container   , m_vertexK0ContainerKey       ));

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
        
        x->getSVOwnership(false); // Prevent Container from deleting vertices
        const auto mainVertex = cascadeVertices[2];   // this is the Bd (Bd, Lambda_b, Lambda_bbar) vertex
        //const auto v0Vertex = cascadeVertices[0];   // this is the V0 (Kshort, Lambda, Lambdabar) vertex
        const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();

        // Set links to cascade vertices
        std::vector<xAOD::Vertex*> verticestoLink;
        verticestoLink.push_back(cascadeVertices[0]);
      //verticestoLink.push_back(cascadeVertices[1]); // ???
      //if(Vtxwritehandles[1] == nullptr) ATH_MSG_ERROR("Vtxwritehandles[1] is null");
        if(Vtxwritehandles[2] == nullptr) ATH_MSG_ERROR("Vtxwritehandles[2] is null");
      //if(!LinkVertices_dupl3(CascadeLinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[1]))
        if(!LinkVertices_dupl3(CascadeLinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with cascade vertices");

        // Identify the input Jpsi+pi
        xAOD::Vertex* jpsipiVertex = BPhysPVCascadeTools::FindVertex<3>(jpsipiContainer, cascadeVertices[2]);
        ATH_MSG_DEBUG("1 pt Jpsi+pi tracks " << cascadeVertices[2]->trackParticle(0)->pt() << ", " << cascadeVertices[2]->trackParticle(1)->pt() << ", " << cascadeVertices[2]->trackParticle(2)->pt());
        if (jpsipiVertex) ATH_MSG_DEBUG("2 pt Jpsi+pi tracks " << jpsipiVertex->trackParticle(0)->pt() << ", " << jpsipiVertex->trackParticle(1)->pt() << ", " << jpsipiVertex->trackParticle(2)->pt());

        // Identify the input D0
/*
        xAOD::Vertex* v0Vertex = BPhysPVCascadeTools::FindVertex<2>(d0Container, cascadeVertices[0]);;
        ATH_MSG_DEBUG("1 pt V0 tracks " << cascadeVertices[0]->trackParticle(0)->pt() << ", " << cascadeVertices[0]->trackParticle(1)->pt());
        if (v0Vertex) ATH_MSG_DEBUG("2 pt V0 tracks " << v0Vertex->trackParticle(0)->pt() << ", " << v0Vertex->trackParticle(1)->pt()); 
*/
        xAOD::Vertex* d0Vertex = BPhysPVCascadeTools::FindVertex<2>(d0Container, cascadeVertices[1]);;
        ATH_MSG_DEBUG("1 pt D0 tracks " << cascadeVertices[1]->trackParticle(0)->pt() << ", " << cascadeVertices[1]->trackParticle(1)->pt());
        if (d0Vertex) ATH_MSG_DEBUG("2 pt D0 tracks " << d0Vertex->trackParticle(0)->pt() << ", " << d0Vertex->trackParticle(1)->pt());

        // Identify the input K_S0
        xAOD::Vertex* k0Vertex = BPhysPVCascadeTools::FindVertex<2>(k0Container, cascadeVertices[0]);;
        ATH_MSG_DEBUG("1 pt K_S0 tracks " << cascadeVertices[0]->trackParticle(0)->pt() << ", " << cascadeVertices[0]->trackParticle(1)->pt());
        if (k0Vertex) ATH_MSG_DEBUG("2 pt D_S0 tracks " << k0Vertex->trackParticle(0)->pt() << ", " << k0Vertex->trackParticle(1)->pt());

        // Set links to input vertices
        std::vector<xAOD::Vertex*> jpsipiVerticestoLink;
        if (jpsipiVertex) jpsipiVerticestoLink.push_back(jpsipiVertex);
        else ATH_MSG_WARNING("Could not find linking Jpsi+pi");
        if(!LinkVertices_dupl3(JpsipiLinksDecor, jpsipiVerticestoLink, jpsipiContainer, cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with Jpsi+pi vertices");

        std::vector<xAOD::Vertex*> d0VerticestoLink;
        if (d0Vertex) d0VerticestoLink.push_back(d0Vertex);
        else ATH_MSG_WARNING("Could not find linking D0");
        if(!LinkVertices_dupl3(D0LinksDecor, d0VerticestoLink, d0Container, cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with D0 vertices");

        std::vector<xAOD::Vertex*> k0VerticestoLink;
        if (k0Vertex) k0VerticestoLink.push_back(k0Vertex);
        else ATH_MSG_WARNING("Could not find linking K_S0");
        if(!LinkVertices_dupl3(K0LinksDecor, k0VerticestoLink, k0Container, cascadeVertices[2]))
            ATH_MSG_ERROR("Error decorating with K_S0 vertices");

        // Collect the tracks that should be excluded from the PV
        std::vector<const xAOD::TrackParticle*> exclTrk; exclTrk.clear();
        for( unsigned int jt=0; jt<cascadeVertices.size(); jt++) {
          for( unsigned int it=0; it<cascadeVertices[jt]->vxTrackAtVertex().size(); it++) {
            ATH_MSG_DEBUG("track to exclude " << cascadeVertices[jt]->trackParticle(it) << " pt " << cascadeVertices[jt]->trackParticle(it)->pt() << " charge " << cascadeVertices[jt]->trackParticle(it)->charge() << " from vertex " << jt << " track number " << it);
           //if(cascadeVertices[jt]->trackParticle(it)->charge() != 0) exclTrk.push_back(m_V0Tools->origTrack(cascadeVertices[jt],it));
           if(cascadeVertices[jt]->trackParticle(it)->charge() != 0) exclTrk.push_back(cascadeVertices[jt]->trackParticle(it));
          }
        }


        double mass_d0 = m_mass_D0; 
      //double mass_k0 = m_mass_ks; 
        double mass_b = m_mass_Bc;
        std::vector<double> massesJpsipi(2, m_mass_muon);
        massesJpsipi.push_back(m_mass_pion);
        std::vector<double> massesD0;
        std::vector<double> massesK0;
        std::vector<double> Masses(2, m_mass_muon);
        Masses.push_back(m_mass_pion);
        if (m_v0_pid == 421) {
           massesD0.push_back(m_mass_kaon);
           massesD0.push_back(m_mass_pion);
           Masses.push_back(m_mass_D0);
        }
        if (m_k0_pid == 310) {
           massesK0.push_back(m_mass_pion);
           massesK0.push_back(m_mass_pion);
           Masses.push_back(m_mass_ks);
        } 

        // reset beamspot cache
        helper.GetBeamSpot(true);
        // loop over candidates -- Don't apply PV_minNTracks requirement here
        // because it may result in exclusion of the high-pt PV.
        // get good PVs
        const std::vector<const xAOD::Vertex*> GoodPVs = helper.GetGoodPV(pvContainer);
        ATH_MSG_DEBUG("number of good PVs " << GoodPVs.size() << " m_refitPV " << m_refitPV << " m_DoVertexType " << m_DoVertexType);
        const bool doPt   = (m_DoVertexType & 1) != 0;
        const bool doA0   = (m_DoVertexType & 2) != 0;
        const bool doZ0   = (m_DoVertexType & 4) != 0;
        const bool doZ0BA = (m_DoVertexType & 8) != 0;

        xAOD::BPhysHypoHelper vtx(m_hypoName, mainVertex);

        // Get refitted track momenta from all vertices, charged tracks only
        std::vector<float> px;
        std::vector<float> py;
        std::vector<float> pz;
        for( size_t jt=0; jt<moms.size(); jt++) {
          for( size_t it=0; it<cascadeVertices[jt]->vxTrackAtVertex().size(); it++) {
            px.push_back( moms[jt][it].Px() );
            py.push_back( moms[jt][it].Py() );
            pz.push_back( moms[jt][it].Pz() );
            ATH_MSG_DEBUG("track mass " << moms[jt][it].M());
          }
        }
        vtx.setRefTrks(px,py,pz);
        ATH_MSG_DEBUG("number of refitted tracks " << px.size());

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

        float massMumu_ori = 0.;
        if (jpsipiVertex) {
          TLorentzVector  p4Mup_ori, p4Mum_ori;
          p4Mup_ori.SetPtEtaPhiM(jpsipiVertex->trackParticle(0)->pt(), 
                                 jpsipiVertex->trackParticle(0)->eta(),
                                 jpsipiVertex->trackParticle(0)->phi(), m_mass_muon); 
          p4Mum_ori.SetPtEtaPhiM(jpsipiVertex->trackParticle(1)->pt(), 
                                 jpsipiVertex->trackParticle(1)->eta(),
                                 jpsipiVertex->trackParticle(1)->phi(), m_mass_muon); 
          massMumu_ori = (p4Mup_ori + p4Mum_ori).M();
        }
        MassMumu_decor(*mainVertex) = massMumu_ori;

        float massKpi_ori = 0.;
        if (d0Vertex) {
          TLorentzVector  p4Km_ori, p4Pip_ori;
          p4Km_ori.SetPtEtaPhiM(d0Vertex->trackParticle(0)->pt(), 
                                d0Vertex->trackParticle(0)->eta(),
                                d0Vertex->trackParticle(0)->phi(), m_mass_kaon); 
          p4Pip_ori.SetPtEtaPhiM(d0Vertex->trackParticle(1)->pt(), 
                                 d0Vertex->trackParticle(1)->eta(),
                                 d0Vertex->trackParticle(1)->phi(), m_mass_pion); 
          massKpi_ori = (p4Km_ori + p4Pip_ori).M();
        }
        MassKpi_svdecor(*mainVertex) = massKpi_ori;

        float massPipi_ori = 0.;
        if (k0Vertex) {
          TLorentzVector p4Pi1_ori, p4Pi2_ori;
          p4Pi1_ori.SetPtEtaPhiM(k0Vertex->trackParticle(0)->pt(), 
                                 k0Vertex->trackParticle(0)->eta(),
                                 k0Vertex->trackParticle(0)->phi(), m_mass_pion); 
          p4Pi2_ori.SetPtEtaPhiM(k0Vertex->trackParticle(1)->pt(), 
                                 k0Vertex->trackParticle(1)->eta(),
                                 k0Vertex->trackParticle(1)->phi(), m_mass_pion); 
          massPipi_ori = (p4Pi1_ori + p4Pi2_ori).M();
        }
        MassPipi_sv2decor(*mainVertex) = massPipi_ori;

        MassJpsi_decor(*mainVertex) = (moms[2][0] + moms[2][1]).M();
        MassPiD0_decor(*mainVertex) = (moms[2][2] + moms[2][4]).M();
        MassPiD0K0_decor(*mainVertex) = (moms[2][2] + moms[2][4] + moms[2][3]).M();

        // 2) PV dependent variables
        if (GoodPVs.empty() == false) {

          if (m_refitPV) {
            size_t pVmax =std::min((size_t)m_PV_max, GoodPVs.size());
            std::vector<const xAOD::Vertex*> refPVvertexes;
            std::vector<const xAOD::Vertex*> refPVvertexes_toDelete;
            std::vector<int> exitCode;
            refPVvertexes.reserve(pVmax);
            refPVvertexes_toDelete.reserve(pVmax);
            exitCode.reserve(pVmax);

            // Refit the primary vertex and set the related decorations.

            for (size_t i =0; i < pVmax ; i++) {
              const xAOD::Vertex* oldPV = GoodPVs.at(i);
              // when set to false this will return null when a new vertex is not required
              ATH_MSG_DEBUG("old PV x " << oldPV->x() << " y " << oldPV->y() << " z " << oldPV->z());
              const xAOD::Vertex* refPV = m_pvRefitter->refitVertex(oldPV, exclTrk, true);
              if (refPV) ATH_MSG_DEBUG("ref PV x " << refPV->x() << " y " << refPV->y() << " z " << refPV->z());
              exitCode.push_back(m_pvRefitter->getLastExitCode());
              // we want positioning to match the goodPrimaryVertices
              if (refPV == nullptr) {
                 refPVvertexes.push_back(oldPV);
                 refPVvertexes_toDelete.push_back(nullptr);
              } else {
                 refPVvertexes.push_back(refPV);
                 refPVvertexes_toDelete.push_back(refPV);
              }
            }
            LocalVector<size_t, 4> indexesUsed;
            LocalVector<std::pair<size_t, xAOD::BPhysHelper::pv_type>, 4> indexestoProcess;

            if(doPt){
               indexestoProcess.push_back(std::make_pair
                   (helper.FindHighPtIndex(refPVvertexes), xAOD::BPhysHelper::PV_MAX_SUM_PT2));
            }
            if(doA0) {
               indexestoProcess.push_back(std::make_pair( helper.FindLowA0Index(moms[2], vtx, refPVvertexes, m_PV_minNTracks),  
                  xAOD::BPhysHelper::PV_MIN_A0));
            }
            if(doZ0) {
               indexestoProcess.push_back(std::make_pair(helper.FindLowZIndex(moms[2], vtx, refPVvertexes, m_PV_minNTracks),
                       xAOD::BPhysHelper::PV_MIN_Z0));
            }
            if(doZ0BA) {
               size_t lowZBA = helper.FindLowZ0BAIndex(moms[2], vtx, refPVvertexes, m_PV_minNTracks);
               if( lowZBA < pVmax ) { 
                  indexestoProcess.push_back(std::make_pair(lowZBA, xAOD::BPhysHelper::PV_MIN_Z0_BA));
               }
               else helper.FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
            }

            ATH_MSG_DEBUG("pVMax = " << pVmax);
            ATH_MSG_DEBUG("m_PV_minNTracks = " << m_PV_minNTracks);
            for(auto x : indexestoProcess){
               ATH_MSG_DEBUG("processing vertex type " << x.second << " of index " << x.first);
            }

            for(size_t i =0 ; i<indexestoProcess.size(); i++){
                //if refitted add to refitted container
                auto index  = indexestoProcess[i].first;
                auto pvtype = indexestoProcess[i].second;
                const xAOD::VertexContainer* ParentContainer =
                    (refPVvertexes_toDelete.at(index)) ? refPvContainer : pvContainer;
                if(ParentContainer == refPvContainer && !indexesUsed.contains(index)) {
                    // store the new vertex
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(index))); 
                    indexesUsed.push_back(index);
                }
                helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, refPVvertexes[index],
                     ParentContainer, pvtype, exitCode[index]);
                vtx.setOrigPv(GoodPVs[index], pvContainer, pvtype);               
            }

            //nullify ptrs we want to keep so these won't get deleted
            //"delete null" is valid in C++ and does nothing so this is quicker than a lot of if statements
            for(size_t x : indexesUsed) refPVvertexes_toDelete[x] = nullptr;

            //Loop over toDELETE container, anything that is used or was not refitted is null
            //This cleans up all extra vertices that were created and not used
            for(const xAOD::Vertex* ptr : refPVvertexes_toDelete) delete ptr;
            refPVvertexes.clear(); // Clear lists of now dangling ptrs
            refPVvertexes_toDelete.clear();
            exitCode.clear();

          } else {

            // 2.a) the first PV with the largest sum pT.
            if(doPt) {
              size_t highPtindex = helper.FindHighPtIndex(GoodPVs); // Should be 0 in PV ordering
              helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, GoodPVs[highPtindex], pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
            }
            // 2.b) the closest in 3D:
            if(doA0) {
              size_t lowA0 =  helper.FindLowA0Index(moms[2], vtx, GoodPVs, m_PV_minNTracks);
              helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, GoodPVs[lowA0], pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
            }
            // 2.c) the closest in Z:
            if(doZ0) {
              size_t lowZ  = helper.FindLowZIndex(moms[2], vtx, GoodPVs, m_PV_minNTracks);
              helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, GoodPVs[lowZ], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
            }
            // 2.d) the closest in Z (DOCA w.r.t. beam axis):
            if(doZ0BA) {
              size_t lowZBA = helper.FindLowZ0BAIndex(moms[2], vtx, GoodPVs, m_PV_minNTracks);
              if ( lowZBA < GoodPVs.size() ) { // safety against vector index out-of-bounds
                helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, GoodPVs[lowZBA], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
              } else {
                // nothing found -- fill NULL
                helper.FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
              }
            }
          } // m_refitPV

        } else {

          if(pvContainer->empty()) return StatusCode::FAILURE;
          const xAOD::Vertex* Dummy = pvContainer->at(0);

          // 2.a) the first PV with the largest sum pT.
          if(doPt) {
            helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
          }
          // 2.b) the closest in 3D:
          if(doA0) {
            helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
          }
          // 2.c) the closest in Z:
          if(doZ0) {
            helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
          }
          // 2.d) the closest in Z (DOCA w.r.t. beam axis):
          if(doZ0BA) {
            helper.FillBPhysHelper(moms[2], x->getCovariance()[2], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
          }

        } // GoodPVs.empty()

        // 3) proper decay time and error:
        // retrieve the refitted PV (or the original one, if the PV refitting was turned off)
        if(doPt)   helper.ProcessVertex(moms[2], x->getCovariance()[2], vtx, xAOD::BPhysHelper::PV_MAX_SUM_PT2, mass_b);
        if(doA0)   helper.ProcessVertex(moms[2], x->getCovariance()[2], vtx, xAOD::BPhysHelper::PV_MIN_A0, mass_b);
        if(doZ0)   helper.ProcessVertex(moms[2], x->getCovariance()[2], vtx, xAOD::BPhysHelper::PV_MIN_Z0, mass_b);
        if(doZ0BA) helper.ProcessVertex(moms[2], x->getCovariance()[2], vtx, xAOD::BPhysHelper::PV_MIN_Z0_BA, mass_b);

        // 4) decorate the main vertex with V0 vertex mass, pt, lifetime and lxy values (plus errors) 
        // V0 points to the main vertex, so lifetime and lxy are w.r.t the main vertex
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
        ATH_MSG_DEBUG("V0Tools mass_d0 " << m_V0Tools->invariantMass(cascadeVertices[1],massesD0)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[1],massesD0)
                   << " mass_J " << m_V0Tools->invariantMass(cascadeVertices[2],massesJpsipi)
                   << " error " << m_V0Tools->invariantMassError(cascadeVertices[2],massesJpsipi));
        // masses and errors, using track masses assigned in the fit
        double Mass_B = m_CascadeTools->invariantMass(moms[2]);
        double Mass_V0 = m_CascadeTools->invariantMass(moms[1]);
        double Mass_B_err = m_CascadeTools->invariantMassError(moms[2],x->getCovariance()[2]);
        double Mass_V0_err = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1]);
        ATH_MSG_DEBUG("Mass_B " << Mass_B << " Mass_V0 " << Mass_V0);
        ATH_MSG_DEBUG("Mass_B_err " << Mass_B_err << " Mass_V0_err " << Mass_V0_err);
        double mprob_B = m_CascadeTools->massProbability(mass_b,Mass_B,Mass_B_err);
        double mprob_V0 = m_CascadeTools->massProbability(mass_d0,Mass_V0,Mass_V0_err);
        ATH_MSG_DEBUG("mprob_B " << mprob_B << " mprob_V0 " << mprob_V0);
        // masses and errors, assigning user defined track masses
        ATH_MSG_DEBUG("Mass_b " << m_CascadeTools->invariantMass(moms[2],Masses)
                  << " Mass_v0 " << m_CascadeTools->invariantMass(moms[1],massesD0));
        ATH_MSG_DEBUG("Mass_b_err " << m_CascadeTools->invariantMassError(moms[2],x->getCovariance()[2],Masses)
                  << " Mass_v0_err " << m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1],massesD0));
        ATH_MSG_DEBUG("pt_b " << m_CascadeTools->pT(moms[2])
                  << " pt_v " << m_CascadeTools->pT(moms[1])
                  << " pt_v0 " << m_V0Tools->pT(cascadeVertices[1]));
        ATH_MSG_DEBUG("ptErr_b " << m_CascadeTools->pTError(moms[2],x->getCovariance()[2])
                  << " ptErr_v " << m_CascadeTools->pTError(moms[1],x->getCovariance()[1])
                  << " ptErr_v0 " << m_V0Tools->pTError(cascadeVertices[1]));
        ATH_MSG_DEBUG("lxy_B " << m_V0Tools->lxy(cascadeVertices[2],primaryVertex) << " lxy_V " << m_V0Tools->lxy(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("lxy_b " << m_CascadeTools->lxy(moms[2],cascadeVertices[2],primaryVertex) << " lxy_v " << m_CascadeTools->lxy(moms[1],cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("lxyErr_b " << m_CascadeTools->lxyError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                  << " lxyErr_v " << m_CascadeTools->lxyError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                  << " lxyErr_v0 " << m_V0Tools->lxyError(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("tau_B " << m_CascadeTools->tau(moms[2],cascadeVertices[2],primaryVertex,mass_b)
                   << " tau_v0 " << m_V0Tools->tau(cascadeVertices[1],cascadeVertices[2],massesD0));
        ATH_MSG_DEBUG("tau_b " << m_CascadeTools->tau(moms[2],cascadeVertices[2],primaryVertex)
                   << " tau_v " << m_CascadeTools->tau(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << " tau_V " << m_CascadeTools->tau(moms[1],cascadeVertices[1],cascadeVertices[2],mass_d0));
        ATH_MSG_DEBUG("tauErr_b " << m_CascadeTools->tauError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                  << " tauErr_v " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                  << " tauErr_v0 " << m_V0Tools->tauError(cascadeVertices[1],cascadeVertices[2],massesD0));
        ATH_MSG_DEBUG("TauErr_b " << m_CascadeTools->tauError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex,mass_b)
                  << " TauErr_v " << m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2],mass_d0)
                  << " TauErr_v0 " << m_V0Tools->tauError(cascadeVertices[1],cascadeVertices[2],massesD0,mass_d0));

        ATH_MSG_DEBUG("CascadeTools main vert wrt PV " << " CascadeTools SV " << " V0Tools SV");
        ATH_MSG_DEBUG("a0z " << m_CascadeTools->a0z(moms[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0z(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0z(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0zErr " << m_CascadeTools->a0zError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0zError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0zError(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0xy " << m_CascadeTools->a0xy(moms[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0xy(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0xy(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0xyErr " << m_CascadeTools->a0xyError(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0xyError(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0xyError(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0 " << m_CascadeTools->a0(moms[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0(moms[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("a0Err " << m_CascadeTools->a0Error(moms[2],x->getCovariance()[2],cascadeVertices[2],primaryVertex)
                   << ", " << m_CascadeTools->a0Error(moms[1],x->getCovariance()[1],cascadeVertices[1],cascadeVertices[2])
                   << ", " << m_V0Tools->a0Error(cascadeVertices[1],cascadeVertices[2]));
        ATH_MSG_DEBUG("x0 " << m_V0Tools->vtx(cascadeVertices[1]).x() << " y0 " << m_V0Tools->vtx(cascadeVertices[1]).y() << " z0 " << m_V0Tools->vtx(cascadeVertices[1]).z());
        ATH_MSG_DEBUG("x1 " << m_V0Tools->vtx(cascadeVertices[2]).x() << " y1 " << m_V0Tools->vtx(cascadeVertices[2]).y() << " z1 " << m_V0Tools->vtx(cascadeVertices[2]).z());
        ATH_MSG_DEBUG("X0 " << primaryVertex->x() << " Y0 " << primaryVertex->y() << " Z0 " << primaryVertex->z());
        ATH_MSG_DEBUG("rxy0 " << m_V0Tools->rxy(cascadeVertices[1]) << " rxyErr0 " << m_V0Tools->rxyError(cascadeVertices[1]));
        ATH_MSG_DEBUG("rxy1 " << m_V0Tools->rxy(cascadeVertices[2]) << " rxyErr1 " << m_V0Tools->rxyError(cascadeVertices[2]));
        ATH_MSG_DEBUG("Rxy0 wrt PV " << m_V0Tools->rxy(cascadeVertices[1],primaryVertex) << " RxyErr0 wrt PV " << m_V0Tools->rxyError(cascadeVertices[1],primaryVertex));
        ATH_MSG_DEBUG("Rxy1 wrt PV " << m_V0Tools->rxy(cascadeVertices[2],primaryVertex) << " RxyErr1 wrt PV " << m_V0Tools->rxyError(cascadeVertices[2],primaryVertex));
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
    m_DpstMassLower(0.0),
    m_DpstMassUpper(10000.0),
    m_MassLower(0.0),
    m_MassUpper(20000.0),
    m_particleDataTable(nullptr),
    m_mass_muon   ( 0 ),
    m_mass_pion   ( 0 ),
    m_mass_proton ( 0 ),
    m_mass_lambda ( 0 ),
    m_mass_ks     ( 0 ),
    m_mass_jpsi   ( 0 ),
    m_mass_b0     ( 0 ),
    m_mass_lambdaB( 0 ),
    m_mass_kaon   ( 0 ),
    m_mass_Dpst     ( 0 ),
    m_mass_D0     ( 0 ),
    m_mass_Bc     ( 0 ),
    m_v0_pid(421),
    m_k0_pid(310),
    m_constrD0(true),
    m_constrK0(true),
    m_constrJpsi(true),
    m_beamSpotSvc("BeamCondSvc",n),
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
       declareProperty("DpstMassLowerCut", m_DpstMassLower);
       declareProperty("DpstMassUpperCut", m_DpstMassUpper);
       declareProperty("MassLowerCut", m_MassLower);
       declareProperty("MassUpperCut", m_MassUpper);
       declareProperty("HypothesisName",            m_hypoName               = "Bc");
       declareProperty("V0Hypothesis",              m_v0_pid);
       declareProperty("K0Hypothesis",              m_k0_pid);
       declareProperty("ApplyV0MassConstraint",     m_constrD0);
       declareProperty("ApplyK0MassConstraint",     m_constrK0);
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

    JpsiPlusDs1Cascade::~JpsiPlusDs1Cascade(){ }

    StatusCode JpsiPlusDs1Cascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer) const
    {
        ATH_MSG_DEBUG( "JpsiPlusDs1Cascade::performSearch" );
        assert(cascadeinfoContainer!=nullptr);

        // Get TrackParticle container (for setting links to the original tracks)
        const xAOD::TrackParticleContainer  *trackContainer(nullptr);
        CHECK(evtStore()->retrieve(trackContainer   , "InDetTrackParticles"      ));

        // Get Jpsi+pi container
        const xAOD::VertexContainer  *jpsipiContainer(nullptr);
        CHECK(evtStore()->retrieve(jpsipiContainer   , m_vertexContainerKey       ));

        // Get D0 container
        const xAOD::VertexContainer  *d0Container(nullptr);
        CHECK(evtStore()->retrieve(d0Container   , m_vertexD0ContainerKey       ));

        // Get K_S0 container
        const xAOD::VertexContainer  *k0Container(nullptr);
        CHECK(evtStore()->retrieve(k0Container   , m_vertexK0ContainerKey       ));

        double mass_d0 = m_mass_D0; 
        double mass_k0 = m_mass_ks; 
        std::vector<const xAOD::TrackParticle*> tracksJpsipi;
        std::vector<const xAOD::TrackParticle*> tracksJpsi;
        std::vector<const xAOD::TrackParticle*> tracksD0;
        std::vector<const xAOD::TrackParticle*> tracksK0;
        std::vector<const xAOD::TrackParticle*> tracksBc;
        std::vector<double> massesJpsipi(2, m_mass_muon);
        massesJpsipi.push_back(m_mass_pion); // Soft pion from D*+ decays
        std::vector<double> massesD0;
        std::vector<double> massesK0;
        std::vector<double> Masses(2, m_mass_muon);
        Masses.push_back(m_mass_pion);
        if (m_v0_pid == 421) {
           massesD0.push_back(m_mass_kaon);
           massesD0.push_back(m_mass_pion);
           Masses.push_back(m_mass_D0);
        }
        if (m_k0_pid == 310) {
           massesK0.push_back(m_mass_pion);
           massesK0.push_back(m_mass_pion);
           Masses.push_back(m_mass_ks);
        } 

        for(auto jpsipi : *jpsipiContainer) { //Iterate over Jpsi+pi vertices

           size_t jpsipiTrkNum = jpsipi->nTrackParticles();
           tracksJpsipi.clear();
           tracksJpsi.clear();
           for( unsigned int it=0; it<jpsipiTrkNum; it++) tracksJpsipi.push_back(jpsipi->trackParticle(it));
           for( unsigned int it=0; it<jpsipiTrkNum-1; it++) tracksJpsi.push_back(jpsipi->trackParticle(it));

           if (tracksJpsipi.size() != 3 || massesJpsipi.size() != 3 ) {
             ATH_MSG_INFO("problems with Jpsi+pi input");
           }
           double mass_Jpsipi = m_V0Tools->invariantMass(jpsipi,massesJpsipi);
           ATH_MSG_DEBUG("Jpsipi mass " << mass_Jpsipi);
           if (mass_Jpsipi < m_jpsipiMassLower || mass_Jpsipi > m_jpsipiMassUpper) {
             ATH_MSG_DEBUG(" Original Jpsipi candidate rejected by the mass cut: mass = "
                           << mass_Jpsipi << " != (" << m_jpsipiMassLower << ", " << m_jpsipiMassUpper << ")" );
             continue;
           }

           TLorentzVector p4Mup_in, p4Mum_in;
           p4Mup_in.SetPtEtaPhiM(jpsipi->trackParticle(0)->pt(), 
                                 jpsipi->trackParticle(0)->eta(),
                                 jpsipi->trackParticle(0)->phi(), m_mass_muon); 
           p4Mum_in.SetPtEtaPhiM(jpsipi->trackParticle(1)->pt(), 
                                 jpsipi->trackParticle(1)->eta(),
                                 jpsipi->trackParticle(1)->phi(), m_mass_muon); 
           double mass_Jpsi = (p4Mup_in + p4Mum_in).M();
           ATH_MSG_DEBUG("Jpsi mass " << mass_Jpsi);
           if (mass_Jpsi < m_jpsiMassLower || mass_Jpsi > m_jpsiMassUpper) {
             ATH_MSG_DEBUG(" Original Jpsi candidate rejected by the mass cut: mass = "
                           << mass_Jpsi << " != (" << m_jpsiMassLower << ", " << m_jpsiMassUpper << ")" );
             continue;
           }

           TLorentzVector p4PiS_in; // Momentum of soft pion
           p4PiS_in.SetPtEtaPhiM(jpsipi->trackParticle(2)->pt(), 
                                 jpsipi->trackParticle(2)->eta(),
                                 jpsipi->trackParticle(2)->phi(), m_mass_pion); 

           for(auto d0 : *d0Container) { //Iterate over V0 vertices

              size_t d0TrkNum = d0->nTrackParticles();
              tracksD0.clear();
              for( unsigned int it=0; it<d0TrkNum; it++) tracksD0.push_back(d0->trackParticle(it));
              if (tracksD0.size() != 2 || massesD0.size() != 2 ) {
                ATH_MSG_INFO("problems with D0 input");
              }
              double mass_D0 = m_V0Tools->invariantMass(d0,massesD0);
              ATH_MSG_DEBUG("D0 mass " << mass_D0);
              if (mass_D0 < m_D0MassLower || mass_D0 > m_D0MassUpper) {
                 ATH_MSG_DEBUG(" Original D0 candidate rejected by the mass cut: mass = "
                               << mass_D0 << " != (" << m_D0MassLower << ", " << m_D0MassUpper << ")" );
                continue;
              }
             
              TLorentzVector p4Km_in, p4Pip_in;
              p4Km_in.SetPtEtaPhiM( d0->trackParticle(0)->pt(), 
                                    d0->trackParticle(0)->eta(),
                                    d0->trackParticle(0)->phi(), m_mass_kaon); 
              p4Pip_in.SetPtEtaPhiM(d0->trackParticle(1)->pt(), 
                                    d0->trackParticle(1)->eta(),
                                    d0->trackParticle(1)->phi(), m_mass_pion); 
              double mass_Dpst= (p4PiS_in + p4Km_in + p4Pip_in).M();
              ATH_MSG_DEBUG("D+* mass " << mass_Dpst);
              if (mass_Dpst < m_DpstMassLower || mass_Dpst > m_DpstMassUpper) {
                ATH_MSG_DEBUG(" Original D+* candidate rejected by the mass cut: mass = "
                              << mass_Dpst << " != (" << m_DpstMassLower << ", " << m_DpstMassUpper << ")" );
                continue;
              }

              for(auto k0 : *k0Container) { //Iterate over K0 vertices
             
                 size_t k0TrkNum = k0->nTrackParticles();
                 tracksK0.clear();
                 for( unsigned int it=0; it<k0TrkNum; it++) tracksK0.push_back(k0->trackParticle(it));
                 if (tracksK0.size() != 2 || massesK0.size() != 2 ) {
                   ATH_MSG_INFO("problems with K0 input");
                 }
                 double mass_K0 = m_V0Tools->invariantMass(k0,massesK0);
                 ATH_MSG_DEBUG("K_S0 mass " << mass_K0);
                 if (mass_K0 < m_K0MassLower || mass_K0 > m_K0MassUpper) {
                    ATH_MSG_DEBUG(" Original K_S0 candidate rejected by the mass cut: mass = "
                                  << mass_K0 << " != (" << m_K0MassLower << ", " << m_K0MassUpper << ")" );
                   continue;
                 }

                 ATH_MSG_DEBUG("using tracks" << tracksJpsipi[0] << ", " << tracksJpsipi[1] << ", " << tracksJpsipi[2] << ", " << tracksD0[0] << ", " << tracksD0[1] << ", " << tracksK0[0] << ", " << tracksK0[1]);
               //if (tracksJpsipi[0] == tracksJpsipi[1] || tracksD0[0] == tracksD0[1] ||
               //    tracksJpsipi[0] == tracksD0[0] || tracksJpsipi[0] == tracksD0[1] ||
               //    tracksJpsipi[1] == tracksD0[0] || tracksJpsipi[1] == tracksD0[1]) {
               //  ATH_MSG_DEBUG("identical tracks in input");
               //  continue;
               //}
                 tracksBc.clear();
                 for( unsigned int it=0; it<jpsipiTrkNum; it++) tracksBc.push_back(jpsipi->trackParticle(it));
                 for( unsigned int it=0; it<d0TrkNum; it++) tracksBc.push_back(d0->trackParticle(it));
                 for( unsigned int it=0; it<k0TrkNum; it++) tracksBc.push_back(k0->trackParticle(it));
                 //-*-
                 bool isIdenticalTrk(false);
                 for( unsigned int it=0; it<tracksBc.size()-1; it++){
                   for( unsigned int jt=it+1; jt<tracksBc.size(); jt++){
                     if(tracksBc[it]==tracksBc[jt]) isIdenticalTrk = true;
                   }
                 }
                 //-*-
                 if( isIdenticalTrk ) {
                   ATH_MSG_DEBUG("identical tracks in input");
                   continue;
                 }
             
             
                 //if (std::find(trackContainer->begin(), trackContainer->end(), tracksJpsipi[0]) == trackContainer->end()) {
                 //   ATH_MSG_ERROR("Track is not in standard container");
                 //} else {
                 //   ATH_MSG_DEBUG("Track " << tracksJpsipi[0] << " is at position " << std::distance(trackContainer->begin(), std::find(trackContainer->begin(), trackContainer->end(), tracksJpsipi[0])) );
                 //}
                 //ATH_MSG_DEBUG("using tracks " << tracksJpsipi[0] << ", " << tracksJpsipi[1] << ", " << tracksD0[0] << ", " << tracksD0[1]);
             
                 // Apply the user's settings to the fitter
                 // Reset
                 m_iVertexFitter->setDefault();
                 // Robustness
                 int robustness = 0;
                 m_iVertexFitter->setRobustness(robustness);
                 // Build up the topology
                 // Vertex list
                 std::vector<Trk::VertexID> vrtList;
                 // K_S0 vertex
                 Trk::VertexID vK0ID;
                 if (m_constrK0) {
                   vK0ID = m_iVertexFitter->startVertex(tracksK0,massesK0,mass_k0);
                 } else {
                   vK0ID = m_iVertexFitter->startVertex(tracksK0,massesK0);
                 }
                 vrtList.push_back(vK0ID);
                 // D0 vertex
                 Trk::VertexID vD0ID;
                 if (m_constrD0) {
                   vD0ID = m_iVertexFitter->nextVertex(tracksD0,massesD0,mass_d0);
                 } else {
                   vD0ID = m_iVertexFitter->nextVertex(tracksD0,massesD0);
                 }
                 vrtList.push_back(vD0ID);
                 // B vertex including Jpsi+pi
                 Trk::VertexID vBcID = m_iVertexFitter->nextVertex(tracksJpsipi,massesJpsipi,vrtList);
                 if (m_constrJpsi) {
                   std::vector<Trk::VertexID> cnstV;
                   cnstV.clear();
                 //if ( !m_iVertexFitter->addMassConstraint(vBcID,tracksJpsipi,cnstV,m_mass_jpsi).isSuccess() ) {
                   if ( !m_iVertexFitter->addMassConstraint(vBcID,tracksJpsi,cnstV,m_mass_jpsi).isSuccess() ) {
                     ATH_MSG_WARNING("addMassConstraint failed");
                     //return StatusCode::FAILURE;
                   }
                 }
                 // Do the work
                 std::unique_ptr<Trk::VxCascadeInfo> result(m_iVertexFitter->fitCascade());
             
                 if (result != NULL) {
                   // reset links to original tracks
                   for(auto v : result->vertices()){
                     std::vector<ElementLink<DataVector<xAOD::TrackParticle> > > newLinkVector;
                     for(unsigned int i=0; i< v->trackParticleLinks().size(); i++)
                     { ElementLink<DataVector<xAOD::TrackParticle> > mylink=v->trackParticleLinks()[i]; // makes a copy (non-const) 
                     mylink.setStorableObject(*trackContainer, true);
                     newLinkVector.push_back( mylink ); }
                     v->clearTracks();
                     v->setTrackParticleLinks( newLinkVector );
                   }
                   ATH_MSG_DEBUG("storing tracks " << ((result->vertices())[0])->trackParticle(0) << ", "
                                                   << ((result->vertices())[0])->trackParticle(1) << ", "
                                                   << ((result->vertices())[1])->trackParticle(0) << ", "
                                                   << ((result->vertices())[1])->trackParticle(1) << ", "
                                                   << ((result->vertices())[2])->trackParticle(0) << ", "
                                                   << ((result->vertices())[2])->trackParticle(1) << ", "
                                                   << ((result->vertices())[2])->trackParticle(2));
                   // necessary to prevent memory leak
                   result->getSVOwnership(true);
                   const std::vector< std::vector<TLorentzVector> > moms = result->getParticleMoms();
                   double mass = m_CascadeTools->invariantMass(moms[2]);
                   if (mass >= m_MassLower && mass <= m_MassUpper) {
                     cascadeinfoContainer->push_back(result.release());
                   } else {
                     ATH_MSG_DEBUG("Candidate rejected by the mass cut: mass = "
                                   << mass << " != (" << m_MassLower << ", " << m_MassUpper << ")" );
                   }
                 }
             
              } //Iterate over K0 vertices

           } //Iterate over V0 vertices

        } //Iterate over Jpsi vertices

        ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer->size());

        return StatusCode::SUCCESS;
    }

}


