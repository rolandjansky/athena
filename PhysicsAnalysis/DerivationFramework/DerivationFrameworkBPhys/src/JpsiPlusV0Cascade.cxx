/////////////////////////////////////////////////////////////////
// JpsiPlusV0Cascade.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/JpsiPlusV0Cascade.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DerivationFrameworkBPhys/CascadeTools.h"
#include "xAODTracking/VertexAuxContainer.h"
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
      SG::ReadHandle<xAOD::VertexContainer> importedVxContainer(m_VxPrimaryCandidateName);
      if (!importedVxContainer.isValid()) {
        ATH_MSG_WARNING("No VxPrimaryCandidate found in StoreGate");
        return StatusCode::RECOVERABLE;
      } else {
        ATH_MSG_DEBUG("Found " << m_VxPrimaryCandidateName.key() << " in StoreGate!");
      }

      if (importedVxContainer->size()==0){
        ATH_MSG_WARNING("You have no primary vertices: " << importedVxContainer->size());
        return StatusCode::RECOVERABLE;
      } else {
        xAOD::VertexContainer::const_iterator pvItr = importedVxContainer->begin();
        primaryVertex = *pvItr;
      }

      ATH_CHECK(performSearch(&cascadeinfoContainer));

      SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks"); 
      SG::AuxElement::Decorator<float> Mass_bdecor("Mass_B");
      SG::AuxElement::Decorator<float> Mass_berrdecor("Mass_B_err");


      ATH_MSG_INFO("cascadeinfoContainer size " << cascadeinfoContainer.size());
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

        ATH_MSG_INFO("chi2 " << chi2 << " ndf " << ndf << " chi2_1 " << chi2_1 << " chi2_2 " << chi2_2 << " vprob " << vprob);
        double m1 = m_V0Tools->invariantMass(cascadeVertices[0],massesV0);
        double m2 = m_V0Tools->invariantMass(cascadeVertices[1],massesJpsi);
        double m1err = m_V0Tools->invariantMassError(cascadeVertices[0],massesV0);
        double m2err = m_V0Tools->invariantMassError(cascadeVertices[1],massesJpsi);

        ATH_MSG_INFO("V0Tools mass_v0 " << m1 << " error " << m1err << " mass_J " << m2 << " error " << m2err);
        // masses and errors, using track masses assigned in the fit

        double Mass_B = m_CascadeTools->invariantMass(moms[1]);
        double Mass_V0 = m_CascadeTools->invariantMass(moms[0]);
        double Mass_B_err = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1]);
        double Mass_V0_err = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0]);
        ATH_MSG_INFO("Mass_B " << Mass_B << " Mass_V0 " << Mass_V0);
        ATH_MSG_INFO("Mass_B_err " << Mass_B_err << " Mass_V0_err " << Mass_V0_err);

        Mass_bdecor(*defaultVertex) = Mass_B;//Example of decoration
        Mass_berrdecor(*defaultVertex) = Mass_B_err;


        double mprob_B = m_CascadeTools->massProbability(mass_b,Mass_B,Mass_B_err);
        double mprob_V0 = m_CascadeTools->massProbability(mass_v0,Mass_V0,Mass_V0_err);
        ATH_MSG_INFO("mprob_B " << mprob_B << " mprob_V0 " << mprob_V0);
        // masses and errors, assigning user defined track masses

        double Mass_b = m_CascadeTools->invariantMass(moms[1],Masses);
        double Mass_v0 = m_CascadeTools->invariantMass(moms[0],massesV0);
        double Mass_b_err = m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1],Masses);
        double Mass_v0_err = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0],massesV0);
        ATH_MSG_INFO("Mass_b " << Mass_b << " Mass_v0 " << Mass_v0);
        ATH_MSG_INFO("Mass_b_err " << Mass_b_err << " Mass_v0_err " << Mass_v0_err);
        double pt_b = m_CascadeTools->pT(moms[1]);
        double pt_v = m_CascadeTools->pT(moms[0]);
        double pt_v0 = m_V0Tools->pT(cascadeVertices[0]);
        ATH_MSG_INFO("pt_b " << pt_b << " pt_v " << pt_v << " pt_v0 " << pt_v0);
        double ptErr_b = m_CascadeTools->pTError(moms[1],x->getCovariance()[1]);
        double ptErr_v = m_CascadeTools->pTError(moms[0],x->getCovariance()[0]);
        double ptErr_v0 = m_V0Tools->pTError(cascadeVertices[0]);
        ATH_MSG_INFO("ptErr_b " << ptErr_b << " ptErr_v " << ptErr_v << " ptErr_v0 " << ptErr_v0);
        double lxy_B = m_V0Tools->lxy(cascadeVertices[1],primaryVertex);
        double lxy_b = m_CascadeTools->lxy(moms[1],cascadeVertices[1],primaryVertex);
        double lxy_V = m_V0Tools->lxy(cascadeVertices[0],cascadeVertices[1]);
        double lxy_v = m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("lxy_B " << lxy_B << " lxy_V " << lxy_V);
        ATH_MSG_INFO("lxy_b " << lxy_b << " lxy_v " << lxy_v);
        double lxyErr_b = m_CascadeTools->lxyError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double lxyErr_v = m_CascadeTools->lxyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double lxyErr_v0 = m_V0Tools->lxyError(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("lxyErr_b " << lxyErr_b << " lxyErr_v " << lxyErr_v << " lxyErr_v0 " << lxyErr_v0);
        double tau_B = m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex,mass_b);
        double tau_b = m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex);
        double tau_v = m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double tau_V = m_V0Tools->tau(cascadeVertices[0],cascadeVertices[1],massesV0);
        ATH_MSG_INFO("tau_B " << tau_B << " tau_V " << tau_V);
        ATH_MSG_INFO("tau_b " << tau_b << " tau_v " << tau_v);
        double tauErr_b = m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double tauErr_v = m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double tauErr_v0 = m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesV0);
        ATH_MSG_INFO("tauErr_b " << tauErr_b << " tauErr_v " << tauErr_v << " tauErr_v0 " << tauErr_v0);
        double TauErr_b = m_CascadeTools->tauError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex,mass_b);
        double TauErr_v = m_CascadeTools->tauError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1],mass_v0);
        double TauErr_v0 = m_V0Tools->tauError(cascadeVertices[0],cascadeVertices[1],massesV0,mass_v0);
        ATH_MSG_INFO("TauErr_b " << TauErr_b << " TauErr_v " << TauErr_v << " TauErr_v0 " << TauErr_v0);
//This are buggy at the moment skipping for now
        continue;

        double a0z_b = m_CascadeTools->a0z(moms[1],cascadeVertices[1],primaryVertex);
        double a0z_v = m_CascadeTools->a0z(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double a0z_V = m_V0Tools->a0z(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("a0z_b " << a0z_b << " a0z_v " << a0z_v << " a0z_V " << a0z_V);
        double a0zErr_b = m_CascadeTools->a0zError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double a0zErr_v = m_CascadeTools->a0zError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double a0zErr_V = m_V0Tools->a0zError(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("a0zErr_b " << a0zErr_b << " a0zErr_v " << a0zErr_v << " a0zErr_V " << a0zErr_V);
        double a0xy_b = m_CascadeTools->a0xy(moms[1],cascadeVertices[1],primaryVertex);
        double a0xy_v = m_CascadeTools->a0xy(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double a0xy_V = m_V0Tools->a0xy(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("a0xy_b " << a0xy_b << " a0xy_v " << a0xy_v << " a0xy_V " << a0xy_V);
        double a0xyErr_b = m_CascadeTools->a0xyError(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double a0xyErr_v = m_CascadeTools->a0xyError(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double a0xyErr_V = m_V0Tools->a0xyError(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("a0xyErr_b " << a0xyErr_b << " a0xyErr_v " << a0xyErr_v << " a0xyErr_V " << a0xyErr_V);
        double a0_b = m_CascadeTools->a0(moms[1],cascadeVertices[1],primaryVertex);
        double a0_v = m_CascadeTools->a0(moms[0],cascadeVertices[0],cascadeVertices[1]);
        double a0_V = m_V0Tools->a0(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("a0_b " << a0_b << " a0_v " << a0_v << " a0_V " << a0_V);
        double a0Err_b = m_CascadeTools->a0Error(moms[1],x->getCovariance()[1],cascadeVertices[1],primaryVertex);
        double a0Err_v = m_CascadeTools->a0Error(moms[0],x->getCovariance()[0],cascadeVertices[0],cascadeVertices[1]);
        double a0Err_V = m_V0Tools->a0Error(cascadeVertices[0],cascadeVertices[1]);
        ATH_MSG_INFO("a0Err_b " << a0Err_b << " a0Err_v " << a0Err_v << " a0Err_V " << a0Err_V);
        //const Amg::MatrixX& cov30 = (cascadeVertices[0])->covariancePosition();
        //const Amg::MatrixX& cov31 = (cascadeVertices[1])->covariancePosition();
        //ATH_MSG_INFO("cov30 " << cov30);
        //ATH_MSG_INFO("cov31 " << cov31);
        double rxy0 = m_V0Tools->rxy(cascadeVertices[0]);
        double rxy1 = m_V0Tools->rxy(cascadeVertices[1]);
        double rxyErr0 = m_V0Tools->rxyError(cascadeVertices[0]);
        double rxyErr1 = m_V0Tools->rxyError(cascadeVertices[1]);
        ATH_MSG_INFO("x0 " << m_V0Tools->vtx(cascadeVertices[0]).x() << " y0 " << m_V0Tools->vtx(cascadeVertices[0]).y() << " z0 " << m_V0Tools->vtx(cascadeVertices[0]).z());
        ATH_MSG_INFO("x1 " << m_V0Tools->vtx(cascadeVertices[1]).x() << " y1 " << m_V0Tools->vtx(cascadeVertices[1]).y() << " z1 " << m_V0Tools->vtx(cascadeVertices[1]).z());
        ATH_MSG_INFO("rxy0 " << rxy0 << " rxyErr0 " << rxyErr0);
        ATH_MSG_INFO("rxy1 " << rxy1 << " rxyErr1 " << rxyErr1);
        double Rxy0 = m_V0Tools->rxy(cascadeVertices[0],primaryVertex);
        double Rxy1 = m_V0Tools->rxy(cascadeVertices[1],primaryVertex);
        double RxyErr0 = m_V0Tools->rxyError(cascadeVertices[0],primaryVertex);
        double RxyErr1 = m_V0Tools->rxyError(cascadeVertices[1],primaryVertex);
        ATH_MSG_INFO("Rxy0 " << Rxy0 << " RxyErr0 " << RxyErr0);
        ATH_MSG_INFO("Rxy1 " << Rxy1 << " RxyErr1 " << RxyErr1);
        ATH_MSG_INFO("mass_V0 " << Mass_V0 << " chi2 " << chi2 << " ndf " << ndf << " chi2_1 " << chi2_1 << " chi2_2 " << chi2_2);
      }

      //Deleting cascadeinfo since this won't be stored.
      //Vertices have been kept in m_cascadeOutputs and should be owned by their container
      for(auto x : cascadeinfoContainer) delete x;

      ATH_MSG_INFO("Exiting cascade");
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
           ATH_MSG_INFO("Jpsi mass " << mass_Jpsi);
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
              ATH_MSG_INFO("V0 mass " << mass_V0);
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


