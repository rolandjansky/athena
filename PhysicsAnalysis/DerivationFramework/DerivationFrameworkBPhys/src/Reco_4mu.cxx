/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Reco_4mu.cxx
///////////////////////////////////////////////////////////////////
// Author: James Catmore <james.catmore@cern.ch>

#include "DerivationFrameworkBPhys/Reco_4mu.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "xAODBPhys/BPhysHypoHelper.h"

namespace DerivationFramework {
    
    Reco_4mu::Reco_4mu(const std::string& t,
                       const std::string& n,
                       const IInterface* p) :
    AthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools"),
    m_fourMuonTool("DerivationFramework::FourMuonTool"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter")
    {
        declareInterface<DerivationFramework::ISkimmingTool>(this);
        
        // Declare tools
        declareProperty("V0Tools"   , m_v0Tools);
        declareProperty("FourMuonTool", m_fourMuonTool);
        declareProperty("PVRefitter", m_pvRefitter);
        
        // Declare user-defined properties
        declareProperty("PairContainerName"      , m_pairName           = "Pairs");
        declareProperty("QuadrupletContainerName", m_quadName           = "Quadruplets");
        declareProperty("PVContainerName"        , m_pvContainerName    = "PrimaryVertices");
        declareProperty("RefPVContainerName"    , m_refPVContainerName = "RefittedPrimaryVertices");
        declareProperty("RefitPV"                , m_refitPV            = false);
        declareProperty("MaxPVrefit"             , m_PV_max             = 1);
        declareProperty("DoVertexType"           , m_DoVertexType       = 1);
    }
    
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    
    StatusCode Reco_4mu::initialize()
    {
        
        ATH_MSG_DEBUG("in initialize()");
        
        // retrieve V0 tools
        CHECK( m_v0Tools.retrieve() );
        
        // get the JpsiFinder tool
        CHECK( m_fourMuonTool.retrieve() );
        
        // get the PrimaryVertexRefitter tool
        CHECK( m_pvRefitter.retrieve() );
        
        return StatusCode::SUCCESS;
        
    }
    
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    
    StatusCode Reco_4mu::finalize()
    {
        // everything all right
        return StatusCode::SUCCESS;
    }
    
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    
    bool Reco_4mu::eventPassesFilter() const
    {
        // Output containers and its auxilliary store
        xAOD::VertexContainer*    pairContainer = NULL;
        xAOD::VertexAuxContainer* pairAuxContainer = NULL;
        xAOD::VertexContainer*    quadContainer = NULL;
        xAOD::VertexAuxContainer* quadAuxContainer = NULL;
        bool acceptEvent(false); 
        //----------------------------------------------------
        // call  finder
        //----------------------------------------------------
        if( !m_fourMuonTool->performSearch(pairContainer, pairAuxContainer, quadContainer, quadAuxContainer, acceptEvent).isSuccess() ) {
            ATH_MSG_FATAL("4mu tool (" << m_fourMuonTool << ") failed.");
            return(false);
        }
        
        //----------------------------------------------------
        // event selection
        //----------------------------------------------------
        /*if (quadContainer->size()==0) {
            if (pairContainer!=NULL) delete pairContainer;
            if (pairAuxContainer!=NULL) delete pairAuxContainer;
            if (quadContainer!=NULL) delete quadContainer;
            if (quadAuxContainer!=NULL) delete quadAuxContainer;
            return(acceptEvent); 
	    // acceptEvent based on muon selection only, not quads
        }*/
        
        //----------------------------------------------------
        // retrieve primary vertices
        //----------------------------------------------------
        const xAOD::VertexContainer*    pvContainer = NULL;
        CHECK( evtStore()->retrieve(pvContainer, m_pvContainerName) );
        
        //----------------------------------------------------
        // Refit primary vertices
        //----------------------------------------------------
        xAOD::VertexContainer*    refPvContainer = NULL;
        xAOD::VertexAuxContainer* refPvAuxContainer = NULL;
        
        if(m_refitPV) {
            refPvContainer = new xAOD::VertexContainer;
            refPvAuxContainer = new xAOD::VertexAuxContainer;
            refPvContainer->setStore(refPvAuxContainer);
        }
        
        BPhysPVTools helper(&(*m_v0Tools));//Give the helper class the ptr to v0tools to use
        
        if(m_refitPV){
            if(quadContainer->size() >0){
                StatusCode SC = helper.FillCandwithRefittedVertices(quadContainer,  pvContainer, refPvContainer, &(*m_pvRefitter) , m_PV_max, m_DoVertexType);
                if(SC.isFailure()){
                    ATH_MSG_FATAL("refitting failed - check the vertices you passed");
                    return SC;
                }
            }
            if(pairContainer->size()>0) {
                StatusCode SC = helper.FillCandwithRefittedVertices(pairContainer,  pvContainer, refPvContainer, &(*m_pvRefitter) , m_PV_max, m_DoVertexType);
                if(SC.isFailure()){
                    ATH_MSG_FATAL("refitting failed - check the vertices you passed");
                    return SC;
                }
            }
        }else{
            refPvContainer = const_cast<xAOD::VertexContainer*>(pvContainer);
            if(quadContainer->size() >0)CHECK(helper.FillCandExistingVertices(quadContainer, refPvContainer, m_DoVertexType));
            if(pairContainer->size() >0)CHECK(helper.FillCandExistingVertices(pairContainer, refPvContainer, m_DoVertexType));
        }

        //----------------------------------------------------
        // Mass-hypothesis dependent quantities
        //----------------------------------------------------
        
        std::vector<double> muonPairMasses = std::vector<double>(2, 105.658);
        std::vector<double> muonQuadMasses = std::vector<double>(4, 105.658);
        
        bool doPt   = (m_DoVertexType & 1) != 0;
        bool doA0   = (m_DoVertexType & 2) != 0;
        bool doZ0   = (m_DoVertexType & 4) != 0;
        bool doZ0BA = (m_DoVertexType & 8) != 0;
        
        // loop over pairs
        xAOD::VertexContainer::iterator pairItr = pairContainer->begin();
        ATH_MSG_DEBUG("Indices/masses of pairs follows....");
        for(; pairItr!=pairContainer->end(); ++pairItr) {
            // create BPhysHypoHelper
            xAOD::BPhysHypoHelper pairHelper("PAIR", *pairItr);
            
            //----------------------------------------------------
            // decorate the vertex
            //----------------------------------------------------
            // a) invariant mass and error
            if( !pairHelper.setMass(muonPairMasses) ) ATH_MSG_WARNING("Decoration pair.setMass failed");
            
            double massErr = m_v0Tools->invariantMassError(pairHelper.vtx(), muonPairMasses);
            if( !pairHelper.setMassErr(massErr) ) ATH_MSG_WARNING("Decoration pair.setMassErr failed");
            
            // b) proper decay time and error:
            // retrieve the refitted PV (or the original one, if the PV refitting was turned off)
            if(doPt) ProcessVertex(pairHelper, xAOD::BPhysHelper::PV_MAX_SUM_PT2, muonPairMasses);
            if(doA0) ProcessVertex(pairHelper, xAOD::BPhysHelper::PV_MIN_A0, muonPairMasses);
            if(doZ0) ProcessVertex(pairHelper, xAOD::BPhysHelper::PV_MIN_Z0, muonPairMasses);
            if(doZ0BA) ProcessVertex(pairHelper, xAOD::BPhysHelper::PV_MIN_Z0_BA, muonPairMasses);
            ATH_MSG_DEBUG((*pairItr)->auxdata<std::string>("CombinationCode") << " : " << pairHelper.mass() << " +/- " << pairHelper.massErr());
        }
        
        // loop over quadruplets
        xAOD::VertexContainer::iterator quadItr = quadContainer->begin();
        ATH_MSG_DEBUG("Indices/masses of quadruplets follows....");
        for(; quadItr!=quadContainer->end(); ++quadItr) {
            // create BPhysHypoHelper
            xAOD::BPhysHypoHelper quadHelper("QUAD", *quadItr);
            
            //----------------------------------------------------
            // decorate the vertex
            //----------------------------------------------------
            // a) invariant mass and error
            if( !quadHelper.setMass(muonQuadMasses) ) ATH_MSG_WARNING("Decoration quad.setMass failed");
            
            double massErr = m_v0Tools->invariantMassError(quadHelper.vtx(), muonQuadMasses);
            if( !quadHelper.setMassErr(massErr) ) ATH_MSG_WARNING("Decoration quad.setMassErr failed");
            
            // b) proper decay time and error:
            // retrieve the refitted PV (or the original one, if the PV refitting was turned off)
            if(doPt) ProcessVertex(quadHelper, xAOD::BPhysHelper::PV_MAX_SUM_PT2, muonQuadMasses);
            if(doA0) ProcessVertex(quadHelper, xAOD::BPhysHelper::PV_MIN_A0, muonQuadMasses);
            if(doZ0) ProcessVertex(quadHelper, xAOD::BPhysHelper::PV_MIN_Z0, muonQuadMasses);
            if(doZ0BA) ProcessVertex(quadHelper, xAOD::BPhysHelper::PV_MIN_Z0_BA, muonQuadMasses);
            ATH_MSG_DEBUG((*quadItr)->auxdata<std::string>("CombinationCode") << " : " << quadHelper.mass() << " +/- " << quadHelper.massErr());
        }
        
        //----------------------------------------------------
        // save in the StoreGate
        //----------------------------------------------------
        // Pairs
        if (!evtStore()->contains<xAOD::VertexContainer>(m_pairName))
            CHECK(evtStore()->record(pairContainer, m_pairName));
        if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_pairName+"Aux."))
            CHECK(evtStore()->record(pairAuxContainer, m_pairName+"Aux."));
        
        // Quads
        if (!evtStore()->contains<xAOD::VertexContainer>(m_quadName))
            CHECK(evtStore()->record(quadContainer, m_quadName));
        if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_quadName+"Aux."))
            CHECK(evtStore()->record(quadAuxContainer, m_quadName+"Aux."));
        
        // Refitted PVs
        if(m_refitPV) {
            CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
            CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
        }
        
        return(acceptEvent);
    }
    
    
    void Reco_4mu::ProcessVertex(xAOD::BPhysHypoHelper &hypoHelper, xAOD::BPhysHelper::pv_type pv_t, std::vector<double> trackMasses) const{
        
        const xAOD::Vertex* pv = hypoHelper.pv(pv_t);
        if(pv) {
            // decorate the vertex.
            
            BPHYS_CHECK( hypoHelper.setTau( m_v0Tools->tau(hypoHelper.vtx(), pv,  trackMasses),
                                           pv_t,
                                           xAOD::BPhysHypoHelper::TAU_INV_MASS) );
            
            BPHYS_CHECK( hypoHelper.setTauErr( m_v0Tools->tauError(hypoHelper.vtx(), pv,  trackMasses),
                                              pv_t,
                                              xAOD::BPhysHypoHelper::TAU_INV_MASS) );
            
            //enum pv_type {PV_MAX_SUM_PT2, PV_MIN_A0, PV_MIN_Z0, PV_MIN_Z0BA};
        }else{
            
            const float errConst = -9999999;
            BPHYS_CHECK( hypoHelper.setTau( errConst,
                                           pv_t,
                                           xAOD::BPhysHypoHelper::TAU_INV_MASS) );
            
            BPHYS_CHECK( hypoHelper.setTauErr( errConst,
                                              pv_t,
                                              xAOD::BPhysHypoHelper::TAU_INV_MASS) );
        }
        
        return;
    }
    
}
