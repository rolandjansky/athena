/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigEventInfoRecorderAlg.h"

class ISvcLocator;

TrigEventInfoRecorderAlg::TrigEventInfoRecorderAlg(const std::string & name, ISvcLocator* pSvcLocator) 
: AthReentrantAlgorithm(name, pSvcLocator) 
{

}

StatusCode TrigEventInfoRecorderAlg::initialize()
{
    ATH_CHECK( m_TrigEventInfoKey.initialize() );
    
    // initialize only the ReadHandleKeys needed by the sequence that called this algorithm
    // initialize(false) for variables not used in the present instantiation
    ATH_CHECK(m_rhoKeyPF.initialize(m_decorateTLA));
    ATH_CHECK(m_rhoKeyEMT.initialize(m_decorateTLA));
    ATH_CHECK(m_PrimaryVxInputName.initialize(m_decorateTLA));

    renounce(m_rhoKeyPF);
    renounce(m_rhoKeyEMT);
    renounce(m_PrimaryVxInputName);
    
    
    return StatusCode::SUCCESS;
}


StatusCode TrigEventInfoRecorderAlg::execute(const EventContext& context) const
{
     
     // Create new TrigComposite(Aux)Container and link them together
    auto trigEventInfoContainer = std::make_unique< xAOD::TrigCompositeContainer>();
    auto trigEventInfoContainerAux  = std::make_unique< xAOD::TrigCompositeAuxContainer>();
    
    trigEventInfoContainer->setStore(trigEventInfoContainerAux.get());

    // Create TrigComposite "object" that will serve as the anchor for the decorations
    xAOD::TrigComposite * trigEI = new xAOD::TrigComposite();
    trigEventInfoContainer->push_back(trigEI);
    
    
    // Retrieve TLA variables and decorate the TrigComposite object with them
    if ( m_decorateTLA ) 
    {   
        ATH_MSG_DEBUG("Calling decorateTLA...");
        ATH_CHECK( decorateTLA(context, trigEI) );
    }
   

    SG::WriteHandle<xAOD::TrigCompositeContainer> trigEventInfoHandle(m_TrigEventInfoKey, context);

    ATH_CHECK(trigEventInfoHandle.record( std::move( trigEventInfoContainer ), std::move( trigEventInfoContainerAux ) ) );    
    
    return StatusCode::SUCCESS;
}

StatusCode TrigEventInfoRecorderAlg::decorateTLA(const EventContext& context, xAOD::TrigComposite* trigEI) const
{
    // Structure of the function: First retrieve all variables of interest from their respective containers
    // in StoreGate, then decorate the TrigComposite Object passed down from the execute
    
    double rho=0, rho_EMT = 0;
    const xAOD::EventShape * eventShape = 0;

    // Jet Densitiy
        
    SG::ReadHandle<xAOD::EventShape> rhRhoKey_PF(m_rhoKeyPF, context);
    if ( rhRhoKey_PF.isValid() )
    {
        ATH_MSG_VERBOSE("Found event density container HLT_Kt4EMPFlowEventShape");
        eventShape = rhRhoKey_PF.cptr();

        if ( !eventShape->getDensity( xAOD::EventShape::Density, rho ) ) {
            ATH_MSG_WARNING("Event density not found in handle, but handle found.");
            ATH_MSG_FATAL("Could not retrieve xAOD::EventShape::Density from xAOD::EventShape.");
            return StatusCode::FAILURE;
         }
    }
    else {
        ATH_MSG_DEBUG("EventShape handle for PFlow not found.");
    }
    
    SG::ReadHandle<xAOD::EventShape> rhRhoKey_EMT(m_rhoKeyEMT, context);
    if ( rhRhoKey_EMT.isValid() )
    {
        ATH_MSG_VERBOSE("Found event density container HLT_Kt4EMTopoEventShape");
        eventShape = rhRhoKey_EMT.cptr();
        if ( !eventShape->getDensity( xAOD::EventShape::Density, rho_EMT ) ) {
            ATH_MSG_WARNING("Event density not found in handle, but handle found.");
            ATH_MSG_FATAL("Could not retrieve xAOD::EventShape::Density from xAOD::EventShape.");
            return StatusCode::FAILURE;
        }
    }
    else {
        ATH_MSG_DEBUG("EventShape for EMTopo handle not found.");
    }

    ATH_MSG_DEBUG("Retrieved Jet Density Rho(PF): " << rho);

    // Average Interactions per bunch crossing
    float avgmu = m_lumiBlockMuTool->averageInteractionsPerCrossing();

    // Number of primary vertices: just counting the number of 'good' vertices in HLT_IDVertex_FS container
    int NPV = 0;
   

    SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_PrimaryVxInputName, context);
    const xAOD::Vertex* privtx = nullptr;
    if( vtxCont.isValid() ){
       const xAOD::VertexContainer* vertex_container = vtxCont.get();
       
       for (unsigned int i_vtx = 0; i_vtx <  vertex_container->size(); i_vtx++)
       {
        privtx = vertex_container->at(i_vtx);

        if (  !( privtx->vertexType() == xAOD::VxType::PriVtx
                && privtx->nTrackParticles() >= 2 ) ){
            privtx = nullptr;
        } else {
            ATH_MSG_DEBUG( "Primary vertex successfully retrieved" );
            NPV++;
        }
       }
    }   
    
    else {
       ATH_MSG_WARNING( "Couldn't retrieve primary vertex, keeping null privtx" );
    }


    // Now decorate the TrigComposite object with the variables retrieved above

    ATH_MSG_DEBUG("Setting PF JetDensity to " << rho);
    trigEI->setDetail("JetDensityEMPFlow", rho);
    ATH_MSG_DEBUG("Setting EMT JetDensity to " << rho_EMT);
    trigEI->setDetail("JetDensityEMTopo", rho_EMT);
    ATH_MSG_DEBUG("Setting AverageMu to " << avgmu);
    trigEI->setDetail("AvgMu", avgmu);
    ATH_MSG_DEBUG("Setting NPV to " << NPV);
    trigEI->setDetail("NumPV", NPV);

    return StatusCode::SUCCESS;
}
