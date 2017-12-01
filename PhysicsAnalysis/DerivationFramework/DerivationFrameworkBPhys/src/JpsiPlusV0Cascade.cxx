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
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include <algorithm>

namespace DerivationFramework {
    typedef ElementLink<xAOD::VertexContainer> VertexLink;
    typedef std::vector<VertexLink> VertexLinkVector;
    typedef std::vector<const xAOD::TrackParticle*> TrackBag;

    double JpsiPlusV0Cascade::getParticleMass(int pdgcode) const{
       auto ptr = m_particleDataTable->particle( pdgcode );
       return ptr ? ptr->mass() : 0.;
    }

    bool LinkVertices(SG::AuxElement::Decorator<VertexLinkVector> &decor, const std::vector<xAOD::Vertex*>& vertices,
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

    StatusCode JpsiPlusV0Cascade::initialize() {

        // retrieving vertex Fitter
        if ( m_iVertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_iVertexFitter);
        }
        
        if(!m_vertexContainerKey.key().empty()) ATH_CHECK(m_vertexContainerKey.initialize());
        if(!m_vertexV0ContainerKey.key().empty()) ATH_CHECK(m_vertexV0ContainerKey.initialize());
        ATH_CHECK(m_cascadeOutputsKeys.initialize());
        ATH_CHECK(m_VxPrimaryCandidateName.initialize());

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

        return StatusCode::SUCCESS;
    }

    StatusCode JpsiPlusV0Cascade::finalize() {
        return StatusCode::SUCCESS;
    }

    StatusCode JpsiPlusV0Cascade::addBranches() const
    {
      std::vector<Trk::VxCascadeInfo*> cascadeinfoContainer;

      auto Vtxwritehandles = m_cascadeOutputsKeys.makeHandles();
      if(Vtxwritehandles.size() !=2)  { ATH_MSG_FATAL("Incorrect number of VtxContainers"); return StatusCode::FAILURE; }
      

      auto cont1 = std::make_unique<xAOD::VertexContainer>();
      auto cont1aux =  std::make_unique<xAOD::VertexAuxContainer>();
      auto cont2  = std::make_unique<xAOD::VertexContainer>();
      auto cont2aux = std::make_unique<xAOD::VertexAuxContainer>();
      cont1->setStore(cont1aux.get());
      cont2->setStore(cont2aux.get());
      ATH_CHECK( Vtxwritehandles[0].record (std::move(cont1), std::move(cont1aux) ));
      ATH_CHECK( Vtxwritehandles[1].record (std::move(cont2), std::move(cont2aux) ));

      // get PV container
      const xAOD::Vertex * primaryVertex(0);
      SG::ReadHandle<xAOD::VertexContainer> pvContainer(m_VxPrimaryCandidateName);
      if (!pvContainer.isValid()) {
        ATH_MSG_WARNING("No VxPrimaryCandidate found in StoreGate");
        return StatusCode::RECOVERABLE;
      } else {
        ATH_MSG_DEBUG("Found " << m_VxPrimaryCandidateName.key() << " in StoreGate!");
      }

      if (pvContainer->size()==0){
        ATH_MSG_WARNING("You have no primary vertices: " << pvContainer->size());
        return StatusCode::RECOVERABLE;
      } else {
        xAOD::VertexContainer::const_iterator pvItr = pvContainer->begin();
        primaryVertex = *pvItr;
      }

//    //----------------------------------------------------
//    // retrieve primary vertices
//    //----------------------------------------------------
//    const xAOD::VertexContainer* pvContainer = NULL;
//    CHECK(evtStore()->retrieve(pvContainer, m_VxPrimaryCandidateName));

      //----------------------------------------------------
      // Try to retrieve refitted primary vertices
      //----------------------------------------------------
      bool refPvExists = false;
      xAOD::VertexContainer*    refPvContainer    = NULL;
      xAOD::VertexAuxContainer* refPvAuxContainer = NULL;
      if (m_refitPV) {
        if (evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
          // refitted PV container exists. Get it from the store gate
          CHECK(evtStore()->retrieve(refPvContainer   , m_refPVContainerName       ));
          CHECK(evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName + "Aux."));
          refPvExists = true;
        } else {
          // refitted PV container does not exist. Create a new one.
          refPvContainer = new xAOD::VertexContainer;
          refPvAuxContainer = new xAOD::VertexAuxContainer;
          refPvContainer->setStore(refPvAuxContainer);
        }
      }
      if(!refPvExists && m_refitPV) {
        CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
        CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
      }

      ATH_CHECK(performSearch(&cascadeinfoContainer));

      BPhysPVCascadeTools helper(&(*m_CascadeTools), &m_beamSpotSvc);
      helper.SetMinNTracksInPV(m_PV_minNTracks);

      SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks"); 
      SG::AuxElement::Decorator<float> chi2_decor("chiSquared");
      SG::AuxElement::Decorator<float> ndof_decor("numberDoF");
      SG::AuxElement::Decorator<float> Mass_sv1decor("SV1_mass");
      SG::AuxElement::Decorator<float> MassErr_sv1decor("SV1_massErr");
      SG::AuxElement::Decorator<float> Mass_sv2decor("SV2_mass");
      SG::AuxElement::Decorator<float> MassErr_sv2decor("SV2_massErr");
      SG::AuxElement::Decorator<float> Pt_sv1decor("SV1_Pt");
      SG::AuxElement::Decorator<float> PtErr_sv1decor("SV1_PtErr");
      SG::AuxElement::Decorator<float> Pt_sv2decor("SV2_Pt");
      SG::AuxElement::Decorator<float> PtErr_sv2decor("SV2_PtErr");
      SG::AuxElement::Decorator<float> Lxy_sv2decor("SV2_Lxy");
      SG::AuxElement::Decorator<float> LxyErr_sv2decor("SV2_LxyErr");
      SG::AuxElement::Decorator<float> Tau_sv2decor("SV2_Tau");
      SG::AuxElement::Decorator<float> TauErr_sv2decor("SV2_TauErr");

      ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer.size());
      for (Trk::VxCascadeInfo* x : cascadeinfoContainer) {
        if(x==nullptr) ATH_MSG_ERROR("x is null");

        const std::vector<xAOD::Vertex*> &cascadeVertices = x->vertices();
        if(cascadeVertices.size()!=2)
          ATH_MSG_ERROR("Incorrect number of vertices");
        if(cascadeVertices[0] == nullptr || cascadeVertices[1] == nullptr) ATH_MSG_ERROR("Error null vertex");
        //Keep vertices
        Vtxwritehandles[0]->push_back(cascadeVertices[0]);
        Vtxwritehandles[1]->push_back(cascadeVertices[1]);
        x->getSVOwnership(false);//Prevent Container from deleting vertices
        const auto defaultVertex = cascadeVertices[0];

        std::vector<xAOD::Vertex*> verticestoLink;
        verticestoLink.push_back(cascadeVertices[1]);
        if(Vtxwritehandles[1].ptr() == nullptr) ATH_MSG_ERROR("Vtxwritehandles[1].ptr() is null");
        if(!LinkVertices(CascadeLinksDecor, verticestoLink, Vtxwritehandles[1].ptr(), cascadeVertices[0])) 
            ATH_MSG_ERROR("Error decorating vertices");


        std::vector<const xAOD::TrackParticle*> exclTrk; exclTrk.clear();
        for( unsigned int jt=0; jt<cascadeVertices.size(); jt++) {
          for( unsigned int it=0; it<cascadeVertices[jt]->vxTrackAtVertex().size(); it++) {
           if(cascadeVertices[jt]->trackParticle(it)->charge() != 0) exclTrk.push_back(m_V0Tools->origTrack(cascadeVertices[jt],it));
          }
        }

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
           //massesV0.push_back(m_mass_pion);
           //massesV0.push_back(m_mass_pion);
           massesV0.push_back(m_mass_pion);
           massesV0.push_back(m_mass_proton);
           Masses.push_back(m_mass_lambda);
           mass_v0 = m_mass_lambda;
           mass_b = m_mass_lambdaB;
        }

        double chi2 = x->fitChi2();
        int ndf = x->nDoF();
        double chi2_1 = m_V0Tools->chisq(cascadeVertices[0]);
        double chi2_2 = m_V0Tools->chisq(cascadeVertices[1]);
        const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();
        double vprob = m_CascadeTools->vertexProbability(ndf,chi2);

        ATH_MSG_DEBUG("chi2 " << chi2 << " ndf " << ndf << " chi2_1 " << chi2_1 << " chi2_2 " << chi2_2 << " vprob " << vprob);
        double m1 = m_V0Tools->invariantMass(cascadeVertices[0],massesV0);
        double m2 = m_V0Tools->invariantMass(cascadeVertices[1],massesJpsi);
        double m1err = m_V0Tools->invariantMassError(cascadeVertices[0],massesV0);
        double m2err = m_V0Tools->invariantMassError(cascadeVertices[1],massesJpsi);
        ATH_MSG_DEBUG("V0Tools mass_v0 " << m1 << " error " << m1err << " mass_J " << m2 << " error " << m2err);

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
        double Mass_b = m_CascadeTools->invariantMass(moms[1],Masses);
        double Mass_v0 = m_CascadeTools->invariantMass(moms[0],massesV0);
        double Mass_b_err = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1],Masses);
        double Mass_v0_err = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0],massesV0);
        ATH_MSG_DEBUG("Mass_b " << Mass_b << " Mass_v0 " << Mass_v0);
        ATH_MSG_DEBUG("Mass_b_err " << Mass_b_err << " Mass_v0_err " << Mass_v0_err);

        double pt_b = m_CascadeTools->pT(moms[1]);
        double pt_v = m_CascadeTools->pT(moms[0]);
        double pt_v0 = m_V0Tools->pT(cascadeVertices[0]);
        ATH_MSG_DEBUG("pt_b " << pt_b << " pt_v " << pt_v << " pt_v0 " << pt_v0);
        double ptErr_b = m_CascadeTools->pTError(moms[1],x->getCovariance()[1]);
        double ptErr_v = m_CascadeTools->pTError(moms[0],x->getCovariance()[0]);
        double ptErr_v0 = m_V0Tools->pTError(cascadeVertices[0]);
        ATH_MSG_DEBUG("ptErr_b " << ptErr_b << " ptErr_v " << ptErr_v << " ptErr_v0 " << ptErr_v0);

        Mass_sv1decor(*defaultVertex) = Mass_B;//Example of decoration
        MassErr_sv1decor(*defaultVertex) = Mass_B_err;
        Mass_sv2decor(*defaultVertex) = Mass_V0;
        MassErr_sv2decor(*defaultVertex) = Mass_V0_err;
        Pt_sv1decor(*defaultVertex) = pt_b;
        PtErr_sv1decor(*defaultVertex) = ptErr_b;
        Pt_sv2decor(*defaultVertex) = pt_v;
        PtErr_sv2decor(*defaultVertex) = ptErr_v;
        chi2_decor(*defaultVertex) = chi2;
        ndof_decor(*defaultVertex) = ndf;

        double lxy_B = m_V0Tools->lxy(cascadeVertices[1],primaryVertex);
        double lxy_b = m_CascadeTools->lxy(moms[1],cascadeVertices[1],primaryVertex);
        double lxy_V = m_V0Tools->lxy(cascadeVertices[0],cascadeVertices[1]);
        double lxy_v = m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("lxy_B " << lxy_B << " lxy_V " << lxy_V);
        ATH_MSG_DEBUG("lxy_b " << lxy_b << " lxy_v " << lxy_v);
        double lxyErr_b = m_CascadeTools->lxyError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double lxyErr_v = m_CascadeTools->lxyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double lxyErr_v0 = m_V0Tools->lxyError(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("lxyErr_b " << lxyErr_b << " lxyErr_v " << lxyErr_v << " lxyErr_v0 " << lxyErr_v0);
        double tau_B = m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex,mass_b);
        double tau_b = m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex);
        double tau_v = m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double tau_V = m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1],mass_v0);
        double tau_v0 = m_V0Tools->tau(cascadeVertices[0],cascadeVertices[1],massesV0);
        ATH_MSG_DEBUG("tau_B " << tau_B << " tau_v0 " << tau_v0);
        ATH_MSG_DEBUG("tau_b " << tau_b << " tau_v " << tau_v << " tau_V " << tau_V);
        double tauErr_b = m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double tauErr_v = m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double tauErr_v0 = m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesV0);
        ATH_MSG_DEBUG("tauErr_b " << tauErr_b << " tauErr_v " << tauErr_v << " tauErr_v0 " << tauErr_v0);
        double TauErr_b = m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex,mass_b);
        double TauErr_v = m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1],mass_v0);
        double TauErr_v0 = m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesV0,mass_v0);
        ATH_MSG_DEBUG("TauErr_b " << TauErr_b << " TauErr_v " << TauErr_v << " TauErr_v0 " << TauErr_v0);
//This are buggy at the moment skipping for now
        //continue;
        Lxy_sv2decor(*defaultVertex) = lxy_v;
        LxyErr_sv2decor(*defaultVertex) = lxyErr_v;
        Tau_sv2decor(*defaultVertex) = tau_v;
        TauErr_sv2decor(*defaultVertex) = tauErr_v;

        double a0z_b = m_CascadeTools->a0z(moms[1],cascadeVertices[1],primaryVertex);
        double a0z_v = m_CascadeTools->a0z(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double a0z_V = m_V0Tools->a0z(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("a0z_b " << a0z_b << " a0z_v " << a0z_v << " a0z_V " << a0z_V);
        double a0zErr_b = m_CascadeTools->a0zError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double a0zErr_v = m_CascadeTools->a0zError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double a0zErr_V = m_V0Tools->a0zError(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("a0zErr_b " << a0zErr_b << " a0zErr_v " << a0zErr_v << " a0zErr_V " << a0zErr_V);
        double a0xy_b = m_CascadeTools->a0xy(moms[1],cascadeVertices[1],primaryVertex);
        double a0xy_v = m_CascadeTools->a0xy(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double a0xy_V = m_V0Tools->a0xy(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("a0xy_b " << a0xy_b << " a0xy_v " << a0xy_v << " a0xy_V " << a0xy_V);
        double a0xyErr_b = m_CascadeTools->a0xyError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double a0xyErr_v = m_CascadeTools->a0xyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double a0xyErr_V = m_V0Tools->a0xyError(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("a0xyErr_b " << a0xyErr_b << " a0xyErr_v " << a0xyErr_v << " a0xyErr_V " << a0xyErr_V);
        double a0_b = m_CascadeTools->a0(moms[1],cascadeVertices[1],primaryVertex);
        double a0_v = m_CascadeTools->a0(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double a0_V = m_V0Tools->a0(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("a0_b " << a0_b << " a0_v " << a0_v << " a0_V " << a0_V);
        double a0Err_b = m_CascadeTools->a0Error(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double a0Err_v = m_CascadeTools->a0Error(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double a0Err_V = m_V0Tools->a0Error(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_DEBUG("a0Err_b " << a0Err_b << " a0Err_v " << a0Err_v << " a0Err_V " << a0Err_V);
        //const Amg::MatrixX& cov30 = (cascadeVertices[0])->covariancePosition();
        //const Amg::MatrixX& cov31 = (cascadeVertices[1])->covariancePosition();
        //ATH_MSG_DEBUG("cov30 " << cov30);
        //ATH_MSG_DEBUG("cov31 " << cov31);
        double rxy0 = m_V0Tools->rxy(cascadeVertices[0]);
        double rxy1 = m_V0Tools->rxy(cascadeVertices[1]);
        double rxyErr0 = m_V0Tools->rxyError(cascadeVertices[0]);
        double rxyErr1 = m_V0Tools->rxyError(cascadeVertices[1]);
        ATH_MSG_DEBUG("x0 " << m_V0Tools->vtx(cascadeVertices[0]).x() << " y0 " << m_V0Tools->vtx(cascadeVertices[0]).y() << " z0 " << m_V0Tools->vtx(cascadeVertices[0]).z());
        ATH_MSG_DEBUG("x1 " << m_V0Tools->vtx(cascadeVertices[1]).x() << " y1 " << m_V0Tools->vtx(cascadeVertices[1]).y() << " z1 " << m_V0Tools->vtx(cascadeVertices[1]).z());
        ATH_MSG_DEBUG("rxy0 " << rxy0 << " rxyErr0 " << rxyErr0);
        ATH_MSG_DEBUG("rxy1 " << rxy1 << " rxyErr1 " << rxyErr1);
        double Rxy0 = m_V0Tools->rxy(cascadeVertices[0],primaryVertex);
        double Rxy1 = m_V0Tools->rxy(cascadeVertices[1],primaryVertex);
        double RxyErr0 = m_V0Tools->rxyError(cascadeVertices[0],primaryVertex);
        double RxyErr1 = m_V0Tools->rxyError(cascadeVertices[1],primaryVertex);
        ATH_MSG_DEBUG("Rxy0 " << Rxy0 << " RxyErr0 " << RxyErr0);
        ATH_MSG_DEBUG("Rxy1 " << Rxy1 << " RxyErr1 " << RxyErr1);
        ATH_MSG_DEBUG("mass_V0 " << Mass_V0 << " chi2 " << chi2 << " ndf " << ndf << " chi2_1 " << chi2_1 << " chi2_2 " << chi2_2);

        // reset beamspot cache
        helper.GetBeamSpot(true);
        // loop over candidates -- Don't apply PV_minNTracks requirement here
        // because it may result in exclusion of the high-pt PV.
        const std::vector<const xAOD::Vertex*> GoodPVs = helper.GetGoodPV(pvContainer.cptr());
        ATH_MSG_DEBUG("number of good PVs " << GoodPVs.size());
        ATH_MSG_INFO("number of good PVs " << GoodPVs.size() << " m_refitPV " << m_refitPV << " m_DoVertexType " << m_DoVertexType);
        const bool doPt   = (m_DoVertexType & 1) != 0;
        const bool doA0   = (m_DoVertexType & 2) != 0;
        const bool doZ0   = (m_DoVertexType & 4) != 0;
        const bool doZ0BA = (m_DoVertexType & 8) != 0;
        xAOD::BPhysHelper vtx(defaultVertex);
        if (m_refitPV) {

          if (GoodPVs.empty() == false) {
            size_t pVmax =std::min((size_t)m_PV_max, GoodPVs.size());
            std::vector<const xAOD::Vertex*> refPVvertexes;
            std::vector<const xAOD::Vertex*> refPVvertexes_toDelete;
            std::vector<int> exitCode;
            refPVvertexes.reserve(pVmax);
            refPVvertexes_toDelete.reserve(pVmax);
            exitCode.reserve(pVmax);
            //const bool doPt   = (m_DoVertexType & 1) != 0;
            //const bool doA0   = (m_DoVertexType & 2) != 0;
            //const bool doZ0   = (m_DoVertexType & 4) != 0;
            //const bool doZ0BA = (m_DoVertexType & 8) != 0;
            //xAOD::BPhysHelper vtx(defaultVertex);

            // 1) pT error
            BPHYS_CHECK( vtx.setPtErr(ptErr_b) );

            // 2) refit the primary vertex and set the related decorations.
            for (size_t i =0; i < pVmax ; i++) {
                const xAOD::Vertex* oldPV = GoodPVs.at(i);
                //when set to false this will return null when a new vertex is not required
                const xAOD::Vertex* refPV = m_pvRefitter->refitVertex(oldPV, exclTrk, false);
                exitCode.push_back(m_pvRefitter->getLastExitCode());
                //I want positioning to match the goodPrimaryVertices
                if (refPV == nullptr) {
                   refPVvertexes.push_back(oldPV);
                   refPVvertexes_toDelete.push_back(nullptr);
                } else {
                   refPVvertexes.push_back(refPV);
                   refPVvertexes_toDelete.push_back(refPV);
                }
            }

            size_t highPtindex = doPt ? helper.FindHighPtIndex(refPVvertexes) : 9999999; //Should be 0 in PV ordering
            size_t lowA0 = doA0 ?
              helper.FindLowA0Index(moms[1], vtx, refPVvertexes, m_PV_minNTracks) : 9999998;
            size_t lowZ  = doZ0 ?
              helper.FindLowZIndex(moms[1], vtx, refPVvertexes, m_PV_minNTracks) : 9999997;
            size_t lowZBA = doZ0BA ?
              helper.FindLowZ0BAIndex(moms[1], vtx, refPVvertexes, m_PV_minNTracks) : 9999996;
            //ATH_MSG_INFO("EVA: pVMax = " << pVmax);
            //ATH_MSG_INFO("EVA: m_PV_minNTracks = " << m_PV_minNTracks);
            //ATH_MSG_INFO("EVA: hPt,lowA0/Z/ZBA = " << highPtindex << ", " << lowA0 << ", " << lowZ << ", " << lowZBA << " "
            //     << (lowA0 != lowZ   ? "1!" : "  ")
            //     << (lowA0 != lowZBA ? "2!" : "  ")
            //     << (lowZ  != lowZBA ? "3!" : "  ")
            //     << (highPtindex != lowA0  ? "4!" : "  ")
            //     << (highPtindex != lowZ   ? "5!" : "  ")
            //     << (highPtindex != lowZBA ? "6!" : "  ")
            //     );

            if(doPt) {
                //Choose old PV container if not refitted
                const xAOD::VertexContainer* ParentContainer =
                    (refPVvertexes_toDelete.at(highPtindex)) ? refPvContainer : pvContainer.cptr();
                if(ParentContainer == refPvContainer) //if refitted add to refitted container
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(highPtindex))); // store the new vertex

                helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, refPVvertexes[highPtindex],
                     ParentContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, exitCode[highPtindex]);
                vtx.setOrigPv(GoodPVs[highPtindex], pvContainer.cptr(), xAOD::BPhysHelper::PV_MAX_SUM_PT2);
            }
            if(doA0) {
                const xAOD::VertexContainer* ParentContainer =
                     (refPVvertexes_toDelete.at(lowA0)) ? refPvContainer : pvContainer.cptr();
                if(ParentContainer == refPvContainer && highPtindex!=lowA0)
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(lowA0))); // store the new vertex
                helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, refPVvertexes[lowA0],
                      ParentContainer, xAOD::BPhysHelper::PV_MIN_A0, exitCode[lowA0]);
                vtx.setOrigPv(GoodPVs[lowA0], pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_A0);
            }
            // 2.c) the closest in Z:
            if(doZ0) {
                const xAOD::VertexContainer* ParentContainer =
                    (refPVvertexes_toDelete.at(lowZ)) ? refPvContainer : pvContainer.cptr();
                if(ParentContainer == refPvContainer && highPtindex!=lowZ && lowZ!=lowA0)
                    refPvContainer->push_back(const_cast<xAOD::Vertex*>(refPVvertexes.at(lowZ))); // store the new vertex
                helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, refPVvertexes[lowZ],
                      ParentContainer, xAOD::BPhysHelper::PV_MIN_Z0, exitCode[lowZ]);
                vtx.setOrigPv(GoodPVs[lowZ], pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0);
            }
            // 2.d) the closest in Z (DOCA w.r.t. beam axis):
            if (doZ0BA) {
              if ( lowZBA < pVmax ) { // safety for vector indices
                const xAOD::VertexContainer* ParentContainer =
                  (refPVvertexes_toDelete.at(lowZBA)) ?
                  refPvContainer : pvContainer.cptr();
                if (ParentContainer == refPvContainer && highPtindex!=lowZBA
                    && lowZBA!=lowA0 && lowZBA != lowZ) {
                  // store the new vertex
                  refPvContainer->push_back(const_cast<xAOD::Vertex*>
                                            (refPVvertexes.at(lowZBA)));
                }
                helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, refPVvertexes[lowZBA],
                                ParentContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA,
                                exitCode[lowZBA]);
                vtx.setOrigPv(GoodPVs[lowZBA], pvContainer.cptr(),
                              xAOD::BPhysHelper::PV_MIN_Z0_BA);
              } else {
                // nothing found -- fill NULL
                const xAOD::Vertex* PV = NULL;
                BPHYS_CHECK( vtx.setPv     ( PV, pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                const float errConst = std::numeric_limits<float>::lowest();
                // set variables claculated from PV
                BPHYS_CHECK( vtx.setLxy    ( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setLxyErr ( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setA0     ( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setA0Err  ( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setA0xy   ( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setA0xyErr( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setZ0     ( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setZ0Err  ( errConst, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );
                BPHYS_CHECK( vtx.setRefitPVStatus ( 0, xAOD::BPhysHelper::PV_MIN_Z0_BA ) );

                ////FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
                // nothing found -- fill dummy vertex (type-0 vertex)
                // if(pvContainer->empty()) return StatusCode::FAILURE;
                // const xAOD::Vertex* dummy = pvContainer->at(pvContainer->size()-1);  //No good vertices so last vertex must be dummy
                // FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
                // vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
              }
            }

            //nullify ptrs we want to keep so these won't get deleted
            //"delete null" is valid in C++ and does nothing so this is quicker than a lot of if statements
            if(doPt)                     refPVvertexes_toDelete[highPtindex] = nullptr;
            if(doA0)                     refPVvertexes_toDelete[lowA0]       = nullptr;
            if(doZ0)                     refPVvertexes_toDelete[lowZ]        = nullptr;
            if(doZ0BA && lowZBA < pVmax) refPVvertexes_toDelete[lowZBA]      = nullptr;
            //Loop over toDELETE container, anything that is used or was not refitted is null
            //This cleans up all extra vertices that were created and not used
            for(const xAOD::Vertex* ptr : refPVvertexes_toDelete) {
                delete ptr;
            }
            refPVvertexes.clear();// Clear lists of now dangling ptrs
            refPVvertexes_toDelete.clear();
            exitCode.clear();

          } else {

            const xAOD::Vertex* Dummy = pvContainer->at(0);
            // 1) pT error
            BPHYS_CHECK( vtx.setPtErr(ptErr_b) );
            if(doPt) {
              // 2.a) the first PV with the largest sum pT.
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
              //if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MAX_SUM_PT2);
                //helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, refPVvertexes[highPtindex],
                //     ParentContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, exitCode[highPtindex]);
                //vtx.setOrigPv(GoodPVs[highPtindex], pvContainer.cptr(), xAOD::BPhysHelper::PV_MAX_SUM_PT2);
            }
            if(doA0) {
              // 2.b) the closest in 3D:
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_A0, 0);
              //if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_A0);
            }
            if(doZ0) {
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0, 0);
              //if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0);
            }
            if(doZ0BA) {
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
              //if(SetOrignal) vtx.setOrigPv(Dummy, pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0_BA);
            }


          }

        } else { // m_refitPV
          if (GoodPVs.empty() == false) {
            //const bool doPt   = (m_DoVertexType & 1) != 0;
            //const bool doA0   = (m_DoVertexType & 2) != 0;
            //const bool doZ0   = (m_DoVertexType & 4) != 0;
            //const bool doZ0BA = (m_DoVertexType & 8) != 0;

            // 1) pT error
            BPHYS_CHECK( vtx.setPtErr(ptErr_b) );

            // 2) refit the primary vertex and set the related decorations.
            if(doPt) {
              size_t highPtindex = helper.FindHighPtIndex(GoodPVs); //Should be 0 in PV ordering
              // 2.a) the first PV with the largest sum pT.
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[highPtindex], pvContainer.cptr(), xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
            }
            if(doA0) {
              // 2.b) the closest in 3D:
              size_t lowA0 =  helper.FindLowA0Index(moms[1], vtx, GoodPVs, m_PV_minNTracks);
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[lowA0], pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_A0, 0);
            }
            if(doZ0) {
              size_t lowZ  = helper.FindLowZIndex(moms[1], vtx, GoodPVs, m_PV_minNTracks);
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[lowZ], pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0, 0);
            }
            if(doZ0BA) {
              size_t lowZBA = helper.FindLowZ0BAIndex(moms[1], vtx, GoodPVs, m_PV_minNTracks);
              if ( lowZBA < GoodPVs.size() ) { // safety against vector index out-of-bounds
                helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[lowZBA], pvContainer.cptr(), xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
              } else {
                // nothing found -- fill NULL
      ////          helper.FillBPhysHelperNULL(vtx, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
              }
            }

          } else {
          //cout << "Warning: DerivationFramework::BPhysPVCascadeTools::FillCandExistingVertices No Primary Vertices Found trying to decorate wilth dummy \n";
          ////if(pvContainer->empty()) return StatusCode::FAILURE;
          ////const xAOD::Vertex* dummy = pvContainer->at(0);  //No good vertices so last vertex must be dummy
          ////DecorateWithDummyVertex(moms[1], x->getCovariance()[1], vtxContainer, pvContainer, dummy, DoVertexType, false);
          }
        } // m_refitPV

      } // loop over cascadeinfoContainer

      //Deleting cascadeinfo since this won't be stored.
      //Vertices have been kept in m_cascadeOutputs and should be owned by their container
      for(auto x : cascadeinfoContainer) delete x;

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
    m_particleDataTable(0),
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
    //m_pvConstraint(false),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_beamSpotSvc("BeamCondSvc",n),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_V0Tools("Trk::V0Tools"),
    m_CascadeTools("DerivationFramework::CascadeTools")
    {
       declareProperty("JpsiVertices", m_vertexContainerKey);
       declareProperty("V0Vertices", m_vertexV0ContainerKey);
       declareProperty("VxPrimaryCandidateName", m_VxPrimaryCandidateName);
       declareProperty("JpsiMassLowerCut", m_jpsiMassLower);
       declareProperty("JpsiMassUpperCut", m_jpsiMassUpper);
       declareProperty("V0MassLowerCut", m_V0MassLower);
       declareProperty("V0MassUpperCut", m_V0MassUpper);
       declareProperty("MassLowerCut", m_MassLower);
       declareProperty("MassUpperCut", m_MassUpper);
       declareProperty("V0Hypothesis",              m_v0_pid);
       declareProperty("ApplyV0MassConstraint",     m_constrV0);
       declareProperty("ApplyJpsiMassConstraint",   m_constrJpsi);
       declareProperty("PVRefitter",                m_pvRefitter);
       declareProperty("RefPVContainerName",        m_refPVContainerName     = "RefittedPrimaryVertices");
       declareProperty("RefitPV",                   m_refitPV                = true);
       declareProperty("MaxnPV",                    m_PV_max                 = 999);
       declareProperty("MinNTracksInPV",            m_PV_minNTracks          = 0);
       declareProperty("DoVertexType",              m_DoVertexType           = 7);
       declareProperty("TrkVertexFitterTool",       m_iVertexFitter);
       declareProperty("V0Tools",                   m_V0Tools);
       declareProperty("CascadeTools",              m_CascadeTools);
       declareProperty("CascadeVertexCollections",              m_cascadeOutputsKeys);
    }

    JpsiPlusV0Cascade::~JpsiPlusV0Cascade(){ }

    StatusCode JpsiPlusV0Cascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer) const
    {
        ATH_MSG_DEBUG( "JpsiPlusV0Cascade::performSearch" );
        assert(cascadeinfoContainer!=nullptr);

        // Get Jpsi container
        SG::ReadHandle<xAOD::VertexContainer>  jpsiContainer(m_vertexContainerKey);
        if(!jpsiContainer.isValid()){
            ATH_MSG_ERROR("No VertexContainer with key " << m_vertexContainerKey.key() << " found in StoreGate. BCandidates will be EMPTY!");
            return StatusCode::FAILURE;
        }

        // Get V0 container
        SG::ReadHandle<xAOD::VertexContainer>  v0Container(m_vertexV0ContainerKey);
        if(!v0Container.isValid()){
            ATH_MSG_ERROR("No V0Container with key " << m_vertexV0ContainerKey.key() << " found in StoreGate. BCandidates will be EMPTY!");
            return StatusCode::FAILURE;
        }

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
                //ATH_MSG_FATAL("problems with V0 input");
                //return StatusCode::FAILURE;
              }
              double mass_V0 = m_V0Tools->invariantMass(v0,massesV0);
              ATH_MSG_DEBUG("V0 mass " << mass_V0);
              if (mass_V0 < m_V0MassLower || mass_V0 > m_V0MassUpper) {
                 ATH_MSG_DEBUG(" Original V0 candidate rejected by the mass cut: mass = "
                               << mass_V0 << " != (" << m_V0MassLower << ", " << m_V0MassUpper << ")" );
                continue;
              }

              if (tracksJpsi[0] == tracksJpsi[1] || tracksV0[0] == tracksV0[1] ||
                  tracksJpsi[0] == tracksV0[0] || tracksJpsi[0] == tracksV0[1] ||
                  tracksJpsi[1] == tracksV0[0] || tracksJpsi[1] == tracksV0[1]) {
                ATH_MSG_DEBUG("identical tracks in input");
                continue;
              }

              // Apply the user's settings to the fitter
              // Reset
              m_iVertexFitter->setDefault();
              // Robustness
              int robustness = 0;
              m_iVertexFitter->setRobustness(robustness);
              // Build up the topology
              // Vertex list
              std::vector<Trk::VertexID> vrtList;
              // V0 vertex
              Trk::VertexID vID;
              if (m_constrV0) {
                vID = m_iVertexFitter->startVertex(tracksV0,massesV0,mass_v0);
              } else {
                vID = m_iVertexFitter->startVertex(tracksV0,massesV0);
              }
              vrtList.push_back(vID);
              // B vertex including Jpsi
              Trk::VertexID vID2 = m_iVertexFitter->nextVertex(tracksJpsi,massesJpsi,vrtList);
              if (m_constrJpsi) {
                std::vector<Trk::VertexID> cnstV;
                cnstV.clear();
                if ( !m_iVertexFitter->addMassConstraint(vID2,tracksJpsi,cnstV,m_mass_jpsi).isSuccess() ) {
                  ATH_MSG_WARNING("addMassConstraint failed");
                  //return StatusCode::FAILURE;
                }
              }
              // Do the work
              std::unique_ptr<Trk::VxCascadeInfo> result(m_iVertexFitter->fitCascade());

              if (result != NULL) {
                //necessary to prevent memory leak
                result->getSVOwnership(true);
                const std::vector< std::vector<TLorentzVector> > moms = result->getParticleMoms();
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


