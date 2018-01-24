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
#include "xAODBPhys/BPhysHypoHelper.h"
#include <algorithm>
#include "xAODTracking/VertexContainer.h"
#include "DerivationFrameworkBPhys/LocalVector.h"

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

      BPhysPVCascadeTools helper(&(*m_CascadeTools), &m_beamSpotSvc);
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
        
        x->getSVOwnership(false); // Prevent Container from deleting vertices
        const auto mainVertex = cascadeVertices[1];   // this is the Bd (Bd, Lambda_b, Lambda_bbar) vertex
        //const auto v0Vertex = cascadeVertices[0];   // this is the V0 (Kshort, Lambda, Lambdabar) vertex
        const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();

        // Set links to cascade vertices
        std::vector<xAOD::Vertex*> verticestoLink;
        verticestoLink.push_back(cascadeVertices[0]);
        if(Vtxwritehandles[1] == nullptr) ATH_MSG_ERROR("Vtxwritehandles[1] is null");
        if(!LinkVertices(CascadeLinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with cascade vertices");

        // Get Jpsi container and identify the input Jpsi
        const xAOD::VertexContainer  *jpsiContainer(nullptr);
        CHECK(evtStore()->retrieve(jpsiContainer   , m_vertexContainerKey       ));
        xAOD::Vertex* jpsiVertex;
        for (auto ajpsi : *jpsiContainer) { //Iterate over Jpsi vertices
          if ((ajpsi->trackParticle(0) == cascadeVertices[1]->trackParticle(0) && ajpsi->trackParticle(1) == cascadeVertices[1]->trackParticle(1)) ||
              (ajpsi->trackParticle(0) == cascadeVertices[1]->trackParticle(1) && ajpsi->trackParticle(1) == cascadeVertices[1]->trackParticle(0))) jpsiVertex = ajpsi;
        }
        ATH_MSG_DEBUG("1 pt Jpsi tracks " << cascadeVertices[1]->trackParticle(0)->pt() << ", " << cascadeVertices[1]->trackParticle(1)->pt());
        if (jpsiVertex) ATH_MSG_DEBUG("2 pt Jpsi tracks " << jpsiVertex->trackParticle(0)->pt() << ", " << jpsiVertex->trackParticle(1)->pt());
        // Get V0 container and identify the input V0
        const xAOD::VertexContainer  *v0Container(nullptr);
        CHECK(evtStore()->retrieve(v0Container   , m_vertexV0ContainerKey       ));
        xAOD::Vertex* v0Vertex;
        for (auto av0 : *v0Container) { //Iterate over V0 vertices
          if ((av0->trackParticle(0) == cascadeVertices[0]->trackParticle(0) && av0->trackParticle(1) == cascadeVertices[0]->trackParticle(1)) ||
              (av0->trackParticle(0) == cascadeVertices[0]->trackParticle(1) && av0->trackParticle(1) == cascadeVertices[0]->trackParticle(0))) v0Vertex = av0;
        }
        ATH_MSG_DEBUG("1 pt V0 tracks " << cascadeVertices[0]->trackParticle(0)->pt() << ", " << cascadeVertices[0]->trackParticle(1)->pt());
        if (v0Vertex) ATH_MSG_DEBUG("2 pt V0 tracks " << v0Vertex->trackParticle(0)->pt() << ", " << v0Vertex->trackParticle(1)->pt());
        // Set links to input vertices
        std::vector<xAOD::Vertex*> jpsiVerticestoLink;
        if (jpsiVertex) jpsiVerticestoLink.push_back(jpsiVertex);
        if(!LinkVertices(JpsiLinksDecor, jpsiVerticestoLink, jpsiContainer, cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with Jpsi vertices");
        std::vector<xAOD::Vertex*> v0VerticestoLink;
        if (v0Vertex) v0VerticestoLink.push_back(v0Vertex);
        if(!LinkVertices(V0LinksDecor, v0VerticestoLink, v0Container, cascadeVertices[1]))
            ATH_MSG_ERROR("Error decorating with V0 vertices");

        // Collect the tracks that should be excluded from the PV
        std::vector<const xAOD::TrackParticle*> exclTrk; exclTrk.clear();
        for( unsigned int jt=0; jt<cascadeVertices.size(); jt++) {
          for( unsigned int it=0; it<cascadeVertices[jt]->vxTrackAtVertex().size(); it++) {
            ATH_MSG_DEBUG("track to exclude " << cascadeVertices[jt]->trackParticle(it) << " pt " << cascadeVertices[jt]->trackParticle(it)->pt() << " charge " << cascadeVertices[jt]->trackParticle(it)->charge() << " from vertex " << jt << " track number " << it);
           //if(cascadeVertices[jt]->trackParticle(it)->charge() != 0) exclTrk.push_back(m_V0Tools->origTrack(cascadeVertices[jt],it));
           if(cascadeVertices[jt]->trackParticle(it)->charge() != 0) exclTrk.push_back(cascadeVertices[jt]->trackParticle(it));
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
           massesV0.push_back(m_mass_pion);
           massesV0.push_back(m_mass_proton);
           Masses.push_back(m_mass_lambda);
           mass_v0 = m_mass_lambda;
           mass_b = m_mass_lambdaB;
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
        for( unsigned int jt=0; jt<moms.size(); jt++) {
          for( unsigned int it=0; it<cascadeVertices[jt]->vxTrackAtVertex().size(); it++) {
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
        BPHYS_CHECK( vtx.setMass(m_CascadeTools->invariantMass(moms[1])) );
        BPHYS_CHECK( vtx.setMassErr(m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1])) );
        // 1.b) pt and pT error (the default pt of mainVertex is != the pt of the full cascade fit!)
        Pt_decor(*mainVertex) = m_CascadeTools->pT(moms[1]);
        PtErr_decor(*mainVertex) = m_CascadeTools->pTError(moms[1],x->getCovariance()[1]);
        // 1.c) chi2 and ndof (the default chi2 of mainVertex is != the chi2 of the full cascade fit!)
        chi2_decor(*mainVertex) = x->fitChi2();
        ndof_decor(*mainVertex) = x->nDoF();

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
               indexestoProcess.push_back(std::make_pair( helper.FindLowA0Index(moms[1], vtx, refPVvertexes, m_PV_minNTracks),  
                  xAOD::BPhysHelper::PV_MIN_A0));
            }
            if(doZ0) {
               indexestoProcess.push_back(std::make_pair(helper.FindLowZIndex(moms[1], vtx, refPVvertexes, m_PV_minNTracks),
                       xAOD::BPhysHelper::PV_MIN_Z0));
            }
            if(doZ0BA) {
               size_t lowZBA = helper.FindLowZ0BAIndex(moms[1], vtx, refPVvertexes, m_PV_minNTracks);
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
                helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, refPVvertexes[index],
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
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[highPtindex], pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
            }
            // 2.b) the closest in 3D:
            if(doA0) {
              size_t lowA0 =  helper.FindLowA0Index(moms[1], vtx, GoodPVs, m_PV_minNTracks);
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[lowA0], pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
            }
            // 2.c) the closest in Z:
            if(doZ0) {
              size_t lowZ  = helper.FindLowZIndex(moms[1], vtx, GoodPVs, m_PV_minNTracks);
              helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[lowZ], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
            }
            // 2.d) the closest in Z (DOCA w.r.t. beam axis):
            if(doZ0BA) {
              size_t lowZBA = helper.FindLowZ0BAIndex(moms[1], vtx, GoodPVs, m_PV_minNTracks);
              if ( lowZBA < GoodPVs.size() ) { // safety against vector index out-of-bounds
                helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, GoodPVs[lowZBA], pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
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
            helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
          }
          // 2.b) the closest in 3D:
          if(doA0) {
            helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_A0);
          }
          // 2.c) the closest in Z:
          if(doZ0) {
            helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0);
          }
          // 2.d) the closest in Z (DOCA w.r.t. beam axis):
          if(doZ0BA) {
            helper.FillBPhysHelper(moms[1], x->getCovariance()[1], vtx, Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA, 0);
            if(m_refitPV) vtx.setOrigPv(Dummy, pvContainer, xAOD::BPhysHelper::PV_MIN_Z0_BA);
          }

        } // GoodPVs.empty()

        // 3) proper decay time and error:
        // retrieve the refitted PV (or the original one, if the PV refitting was turned off)
        if(doPt)   helper.ProcessVertex(moms[1], x->getCovariance()[1], vtx, xAOD::BPhysHelper::PV_MAX_SUM_PT2, mass_b);
        if(doA0)   helper.ProcessVertex(moms[1], x->getCovariance()[1], vtx, xAOD::BPhysHelper::PV_MIN_A0, mass_b);
        if(doZ0)   helper.ProcessVertex(moms[1], x->getCovariance()[1], vtx, xAOD::BPhysHelper::PV_MIN_Z0, mass_b);
        if(doZ0BA) helper.ProcessVertex(moms[1], x->getCovariance()[1], vtx, xAOD::BPhysHelper::PV_MIN_Z0_BA, mass_b);

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
    m_beamSpotSvc("BeamCondSvc",n),
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
              if (tracksJpsi[0] == tracksJpsi[1] || tracksV0[0] == tracksV0[1] ||
                  tracksJpsi[0] == tracksV0[0] || tracksJpsi[0] == tracksV0[1] ||
                  tracksJpsi[1] == tracksV0[0] || tracksJpsi[1] == tracksV0[1]) {
                ATH_MSG_DEBUG("identical tracks in input");
                continue;
              }

              //if (std::find(trackContainer->begin(), trackContainer->end(), tracksJpsi[0]) == trackContainer->end()) {
              //   ATH_MSG_ERROR("Track is not in standard container");
              //} else {
              //   ATH_MSG_DEBUG("Track " << tracksJpsi[0] << " is at position " << std::distance(trackContainer->begin(), std::find(trackContainer->begin(), trackContainer->end(), tracksJpsi[0])) );
              //}
              //ATH_MSG_DEBUG("using tracks " << tracksJpsi[0] << ", " << tracksJpsi[1] << ", " << tracksV0[0] << ", " << tracksV0[1]);

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
                                                << ((result->vertices())[1])->trackParticle(1));
                // necessary to prevent memory leak
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


