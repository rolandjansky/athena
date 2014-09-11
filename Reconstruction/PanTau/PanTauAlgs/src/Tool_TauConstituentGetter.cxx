/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//! Gaudi includes
#include "AthenaKernel/errorcheck.h"

//! PanTau includes
#include "PanTauAlgs/Tool_TauConstituentGetter.h"

#include "PanTauEvent/TauConstituent.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/Tool_InputConverter.h"
#include "PanTauAlgs/Tool_HelperFunctions.h"
#include "TVector3.h"



#include "tauEvent/TauJet.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauPi0Details.h"
#include "tauEvent/TauPi0Cluster.h"

#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEvent/eflowObject.h"

#include "Particle/TrackParticle.h"
#include "VxVertex/RecVertex.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CaloEvent/CaloCluster.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "FourMomUtils/P4Helpers.h"

//! xAOD EDM
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"
#include "xAODTracking/Vertex.h"



PanTau::Tool_TauConstituentGetter::Tool_TauConstituentGetter(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
        m_Tool_HelperFunctions("PanTau::Tool_HelperFunctions/Tool_HelperFunctions"),
        m_Tool_InputConverter("PanTau::Tool_InputConverter/Tool_InputConverter")
{
    declareInterface<ITool_TauConstituentGetter>(this);

    declareProperty("Tool_InformationStore",    m_Tool_InformationStore,   "Link to tool with all information");
    declareProperty("Tool_HelperFunctions",     m_Tool_HelperFunctions,    "Link to tool with all information");
    declareProperty("Tool_InputConverter",      m_Tool_InputConverter,     "Link to tool to convert into TauConstituents");
}

PanTau::Tool_TauConstituentGetter::~Tool_TauConstituentGetter() {
}

StatusCode PanTau::Tool_TauConstituentGetter::initialize() {

    StatusCode sc = AlgTool::initialize();
    ATH_MSG_INFO(" initialize()");
    
    CHECK( m_Tool_InformationStore.retrieve() );
    CHECK( m_Tool_HelperFunctions.retrieve() );
    CHECK( m_Tool_InputConverter.retrieve() );
    
    //FIXME: this value will depend on what the shrinking cone algorithm found out.
    //NOTE: Need to make sure the collection Delta R is available in the tauObject
    CHECK (m_Tool_InformationStore->getInfo_Double("eflowRec_Assoc_DeltaR", m_eflowRec_Assoc_DeltaR) );
    
    return sc;
}


// StatusCode PanTau::Tool_InformationStore::finalize() {
//     StatusCode sc = AlgTool::finalize();
//     return sc;
// }



/**
* Function to get the tau constituents for a given TauJet object and a given algorithm that got the substructure
* 
*/
StatusCode PanTau::Tool_TauConstituentGetter::GetTauConstituents(//const Analysis::TauJet* tauJet,
                                                                 const xAOD::TauJet* tauJet,
                                                                 std::vector<TauConstituent*>& outputConstituents,
                                                                 std::string algName) const {
    //
    ATH_MSG_DEBUG("GetTauConstituents...");
    
    int    enableXAODHacks = 0;
    CHECK (m_Tool_InformationStore->getInfo_Int("EnableXAODMigrationHacks", enableXAODHacks) );
    
    bool inputAlgIsValid = false;
    if(algName == "eflowRec")  inputAlgIsValid = true;
    if(algName == "CellBased") inputAlgIsValid = true;
    if(inputAlgIsValid == false) {
        ATH_MSG_WARNING("Unknown input algorithm: " << algName << " -> not getting constituents for this tau");
        return StatusCode::SUCCESS;
    }
    
    //loop over chared PFOs
    ATH_MSG_DEBUG("Get charged ones...");
    unsigned int nChargedPFO = 0;
    if(algName == "eflowRec")  nChargedPFO = tauJet->nEflowRec_Charged_PFOs();
    if(algName == "CellBased") nChargedPFO = tauJet->nCellBased_Charged_PFOs();
    ATH_MSG_DEBUG("Charged PFOs: " << nChargedPFO);
    
    for(unsigned int iChrgPFO=0; iChrgPFO<nChargedPFO; iChrgPFO++) {
        xAOD::PFO* curChrgPFO = 0;
        if(algName == "eflowRec")  curChrgPFO = const_cast<xAOD::PFO*>(tauJet->eflowRec_Charged_PFO( iChrgPFO ));
        if(algName == "CellBased") curChrgPFO = const_cast<xAOD::PFO*>(tauJet->cellBased_Charged_PFO( iChrgPFO ));
        
        if(enableXAODHacks == 1) {
            ATH_MSG_DEBUG("EnableXAODMigrationHacks is true -> call vertex correctio for charged PFOs to check whether it runs & does something");
            m_Tool_HelperFunctions->vertexCorrection_PFOs(tauJet, curChrgPFO);
        }
        
        //convert to tau constituent
        PanTau::TauConstituent* curConst = 0;
        CHECK(m_Tool_InputConverter->ConvertToTauConstituent(curChrgPFO, curConst, tauJet, algName) );
        if(curConst == 0) {
            ATH_MSG_DEBUG("Problems converting charged PFO into tau constituent -> skip PFO");
            continue;
        }
        
        //add to list of tau constituents
        ATH_MSG_DEBUG("\tDumping contents of constituent at " << curConst);
        m_Tool_HelperFunctions->dumpTauConstituent(curConst);
        outputConstituents.push_back(curConst);
        
    }//end loop over charged PFOs
    
    
    //loop over pi0 tagged PFOs
    
    if(enableXAODHacks == 1) {
        ATH_MSG_DEBUG("Stopping constituent selection here, because EnableXAODMigrationHacks is set to true");
        return StatusCode::SUCCESS;
    }
    
//     ATH_MSG_DEBUG("Get pi0 ones...");
    unsigned int nPi0PFO = 0;
    if(algName == "eflowRec")  nPi0PFO = tauJet->nEflowRec_Pi0_PFOs();
    if(algName == "CellBased") nPi0PFO = tauJet->nCellBased_Pi0_PFOs();
    ATH_MSG_DEBUG("Pi0 tagged PFOs in neutral PFOs: " << nPi0PFO);
//     
//     for(unsigned int iPi0PFO=0; iPi0PFO<nPi0PFO; iPi0PFO++) {
//         xAOD::PFO* curPi0PFO = 0;
//         if(algName == "eflowRec")  curPi0PFO = const_cast<xAOD::PFO*>(tauJet->eflowRec_pi0_PFO( iPi0PFO ));
//         if(algName == "CellBased") curPi0PFO = const_cast<xAOD::PFO*>(tauJet->cellbased_pi0_PFO( iPi0PFO ));
//         
//         // Call vertex correction here
//         m_Tool_HelperFunctions->vertexCorrection_PFOs(tauJet, curPi0PFO);
//         
//         //convert to tau constituent
//         PanTau::TauConstituent* curConst = 0;
//         CHECK( m_Tool_InputConverter->ConvertToTauConstituent(curPi0PFO, curConst, tauJet, algName) );
//         if(curConst == 0) {
//             ATH_MSG_DEBUG("Problems converting pi0 PFO into tau constituent -> skip PFO");
//             continue;
//         }
//         
//         //add to list of tau constituents
//         ATH_MSG_DEBUG("\tDumping contents of constituent at " << curConst);
//         m_Tool_HelperFunctions->dumpTauConstituent(curConst);
//         outputConstituents.push_back(curConst);
//         
//     }//end loop over charged PFOs
    
    
    //loop over neutral PFOs
    ATH_MSG_DEBUG("Get neutral ones...");
    unsigned int nNeutPFO = 0;
    if(algName == "eflowRec")  nNeutPFO = tauJet->nEflowRec_Neutral_PFOs();
    if(algName == "CellBased") nNeutPFO = tauJet->nCellBased_Neutral_PFOs();
    ATH_MSG_DEBUG("Neutral PFOs: " << nNeutPFO);
    
    for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++) {
        xAOD::PFO* curNeutPFO = 0;
        if(algName == "eflowRec")  curNeutPFO = const_cast<xAOD::PFO*>(tauJet->eflowRec_Neutral_PFO( iNeutPFO ));
        if(algName == "CellBased") curNeutPFO = const_cast<xAOD::PFO*>(tauJet->cellBased_Neutral_PFO( iNeutPFO ));
        
        // Call vertex correction here
        m_Tool_HelperFunctions->vertexCorrection_PFOs(tauJet, curNeutPFO);
        
        //convert to tau constituent
        PanTau::TauConstituent* curConst = 0;
        CHECK( m_Tool_InputConverter->ConvertToTauConstituent(curNeutPFO, curConst, tauJet, algName) );
        if(curConst == 0) {
            ATH_MSG_DEBUG("Problems converting neutral PFO into tau constituent -> skip PFO");
            continue;
        }
        
        //add to list of tau constituents
        ATH_MSG_DEBUG("\tDumping contents of constituent at " << curConst);
        m_Tool_HelperFunctions->dumpTauConstituent(curConst);
        outputConstituents.push_back(curConst);
        
    }//end loop over charged PFOs
    
    
    return StatusCode::SUCCESS;
    
}//end PFO getter









    


