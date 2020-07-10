/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//! PanTau includes
#include "PanTauAlgs/Tool_TauConstituentGetter.h"

#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/Tool_InputConverter.h"
//#include "TVector3.h"

//! xAOD EDM
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"
#include "xAODTracking/Vertex.h"



PanTau::Tool_TauConstituentGetter::Tool_TauConstituentGetter(
    const std::string& name ) :
        asg::AsgTool(name),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
        m_Tool_InputConverter("PanTau::Tool_InputConverter/Tool_InputConverter", this)
{
    declareProperty("Tool_InformationStore",    m_Tool_InformationStore,   "Link to tool with all information");
    declareProperty("Tool_InputConverter",      m_Tool_InputConverter,     "Link to tool to convert into TauConstituents");
    declareProperty("Tool_InformationStoreName",    m_Tool_InformationStoreName,   "Link to tool with all information");
    declareProperty("Tool_InputConverterName",      m_Tool_InputConverterName,     "Link to tool to convert into TauConstituents");
}

PanTau::Tool_TauConstituentGetter::~Tool_TauConstituentGetter() {
}

StatusCode PanTau::Tool_TauConstituentGetter::initialize() {

    ATH_MSG_INFO(" initialize()");
    m_init=true;

    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InformationStore, m_Tool_InformationStoreName ) );
    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InputConverter, m_Tool_InputConverterName ) );
    
    ATH_CHECK( m_Tool_InformationStore.retrieve() );
    ATH_CHECK( m_Tool_InputConverter.retrieve() );
    
    return StatusCode::SUCCESS;
}


// StatusCode PanTau::Tool_InformationStore::finalize() {
//     StatusCode sc = AlgTool::finalize();
//     return sc;
// }



/**
* Function to get the PFOs for a given TauJet object (Shots in each PFO etc are collected in "ConvertToTauConstituent")
*/
StatusCode PanTau::Tool_TauConstituentGetter::GetTauConstituents(const xAOD::TauJet* tauJet,
                                                                 std::vector<TauConstituent2*>& outputConstituents,
                                                                 std::string algName) const {
    ATH_MSG_DEBUG("GetTauConstituents...");
    
    bool inputAlgIsValid = false;
    // extend this if-statement once Pantau has been validated for other algs:
    if(algName == "CellBased") inputAlgIsValid = true;
    if(inputAlgIsValid == false) {
        ATH_MSG_WARNING("Unknown input algorithm: " << algName << " -> Pantau BDT Training not done for this algorithm!");
        return StatusCode::FAILURE;
    }
    
    //loop over charged PFOs
    ATH_MSG_DEBUG("Get charged PFOs");
    unsigned int nChargedPFO = 0;
    nChargedPFO = tauJet->nProtoChargedPFOs();
    ATH_MSG_DEBUG("Charged PFOs: " << nChargedPFO);

    for(unsigned int iChrgPFO=0; iChrgPFO<nChargedPFO; iChrgPFO++) {
        xAOD::PFO* curChrgPFO = 0;
        curChrgPFO = const_cast<xAOD::PFO*>(tauJet->protoChargedPFO( iChrgPFO ));

        //convert to tau constituent
        PanTau::TauConstituent2* curConst = 0;
        ATH_CHECK(m_Tool_InputConverter->ConvertToTauConstituent2(curChrgPFO, curConst, tauJet) );
        if(curConst == 0) {
            ATH_MSG_DEBUG("Problems converting charged PFO into tau constituent -> skip PFO");
            continue;
        }

	TLorentzVector v=curConst->p4();
        
        //add to list of tau constituents
        ATH_MSG_DEBUG("\tDumping contents of constituent at " << curConst);
        m_HelperFunctions.dumpTauConstituent2(curConst);
        outputConstituents.push_back(curConst);
        
    }//end loop over charged PFOs
    
    
    // Pi0 tagged PFOs are not collected!

    
    //loop over neutral PFOs
    ATH_MSG_DEBUG("Get neutral ones...");
    unsigned int nNeutPFO = 0;
    nNeutPFO = tauJet->nProtoNeutralPFOs();
    ATH_MSG_DEBUG("Neutral PFOs: " << nNeutPFO);
    
    for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++) {
        xAOD::PFO* curNeutPFO = 0;
        curNeutPFO = const_cast<xAOD::PFO*>(tauJet->protoNeutralPFO( iNeutPFO ));
        
        // Call vertex correction here
        // Will: moved to TauPi0ClusterScaler
        //m_HelperFunctions.vertexCorrection_PFOs(tauJet, curNeutPFO);
        
        //convert to tau constituent
        PanTau::TauConstituent2* curConst = 0;
        ATH_CHECK( m_Tool_InputConverter->ConvertToTauConstituent2(curNeutPFO, curConst, tauJet) );
        if(curConst == 0) {
            ATH_MSG_DEBUG("Problems converting neutral PFO into tau constituent -> skip PFO");
            continue;
        }
        
        //add to list of tau constituents
        ATH_MSG_DEBUG("\tDumping contents of constituent at " << curConst);
        m_HelperFunctions.dumpTauConstituent2(curConst);
        outputConstituents.push_back(curConst);
        
    }//end loop over charged PFOs
    
    
    return StatusCode::SUCCESS;
    
}//end PFO getter









    


