/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TauImpactParameterExtractionTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauSeeds to TruthTaus
///////////////////////////////////////////////////////////////////
// limbach@physik.uni-bonn.de
///////////////////////////////////////////////////////////////////

//! Gaudi includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"

//! PanTau includes
#include "PanTauAlgs/Tool_InformationStore.h"

// ROOT includes
#include "TVector2.h"

// #include "tauEvent/TauJetContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEvent/eflowObject.h"
#include "Particle/TrackParticleContainer.h"

PanTau::Tool_InformationStore::Tool_InformationStore(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_Container_eflowRec(0),
        //m_Container_eflowRecFromSG(0),
        m_Container_TauRec(0)
        {
    declareInterface<ITool_InformationStore>(this);

    declareProperty("Infos_String",     m_Infos_String, "Map with string type infos");
    declareProperty("Infos_VecString",  m_Infos_VecString, "Map with vector<string> type infos");
    declareProperty("Infos_Int",        m_Infos_Int,    "Map with int type infos");
    declareProperty("Infos_Double",     m_Infos_Double, "Map with double type infos");
    declareProperty("Infos_VecDouble",  m_Infos_VecDouble, "Map with double type infos");
}



PanTau::Tool_InformationStore::~Tool_InformationStore() {
}



StatusCode PanTau::Tool_InformationStore::initialize() {
    ATH_MSG_INFO( name() << " initialize()" );
    CHECK( AthAlgTool::initialize() );
    
    //get link to store gate
    CHECK(service("StoreGateSvc", m_sgSvc));
    
    CHECK( this->getInfo_String("Name_eflowRecContainer", m_Name_Container_eflowRec) );
    CHECK( this->getInfo_String("Name_TauRecContainer", m_Name_Container_TauRec) );
    CHECK( this->getInfo_String("Name_TrackParticleContainer", m_Name_Container_Tracks) );
    
    return StatusCode::SUCCESS;
}



// StatusCode PanTau::Tool_InformationStore::finalize() {
//     StatusCode sc = AlgTool::finalize();
//     return sc;
// }


StatusCode PanTau::Tool_InformationStore::updateInformation(std::string /*inputAlg*/) {
    
    //get the tauRec container
    StatusCode sc = m_sgSvc->retrieve(m_Container_TauRec, m_Name_Container_TauRec);
    if(sc != StatusCode::SUCCESS) {
        ATH_MSG_WARNING("Could not get container for tauRec -> skip this execution of PanTau");
        return StatusCode::RECOVERABLE;
    }
    
    return StatusCode::SUCCESS;
}



const eflowObjectContainer*             PanTau::Tool_InformationStore::getContainer_eflowRec() const {
    return m_Container_eflowRec;
}



const xAOD::TauJetContainer*      PanTau::Tool_InformationStore::getContainer_TauRec() const {
    return m_Container_TauRec;
}


const Rec::TrackParticleContainer*      PanTau::Tool_InformationStore::getContainer_TrackParticle() const {
    return m_Container_TrackParticle;
}
 


StatusCode PanTau::Tool_InformationStore::getInfo_Int(std::string varName,     int& value) {
    MapInt::const_iterator it = m_Infos_Int.find(varName);
    if(it == m_Infos_Int.end()) {
        ATH_MSG_ERROR("getInfo_Int: No integer information called " << varName << " present in InformationStore");
        return StatusCode::FAILURE;
    }
    value = it->second;
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_InformationStore::getInfo_Double(std::string varName,  double& value) {
    MapDouble::const_iterator it = m_Infos_Double.find(varName);
    if(it == m_Infos_Double.end()) {
        ATH_MSG_ERROR("getInfo_Double: No double information called " << varName << " present in InformationStore");
        return StatusCode::FAILURE;
    }
    value = it->second;
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_InformationStore::getInfo_VecDouble(std::string varName,  std::vector<double>& value) {
    MapVecDouble::const_iterator it = m_Infos_VecDouble.find(varName);
    if(it == m_Infos_VecDouble.end()) {
        ATH_MSG_ERROR("getInfo_VecDouble: No double information called " << varName << " present in InformationStore");
        return StatusCode::FAILURE;
    }
    value = it->second;
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_InformationStore::getInfo_String(std::string varName,  std::string& value) {
    MapString::const_iterator it = m_Infos_String.find(varName);
    if(it == m_Infos_String.end()) {
        ATH_MSG_ERROR("getInfo_String: No string information called " << varName << " present in InformationStore");
        return StatusCode::FAILURE;
    }
    value = it->second;
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_InformationStore::getInfo_VecString(std::string varName,  std::vector<std::string>& value) {
    MapVecString::const_iterator it = m_Infos_VecString.find(varName);
    if(it == m_Infos_VecString.end()) {
        ATH_MSG_ERROR("getInfo_VecString: No std::string information called " << varName << " present in InformationStore");
        return StatusCode::FAILURE;
    }
    value = it->second;
    return StatusCode::SUCCESS;
}



StatusCode  PanTau::Tool_InformationStore::dumpMaps() const {
    
    ATH_MSG_DEBUG("Dumping information in Tool_InformationStore called " << name());
    
    ATH_MSG_DEBUG("Integer variables: ");
    MapInt::const_iterator itI = m_Infos_Int.begin();
    for(; itI != m_Infos_Int.end(); itI++) {
        std::string key = itI->first;
        int         val = itI->second;
        ATH_MSG_DEBUG("\t" << key << ": " << val);
    }
    
    ATH_MSG_DEBUG("Double variables: ");
    MapDouble::const_iterator itD = m_Infos_Double.begin();
    for(; itD != m_Infos_Double.end(); itD++) {
        std::string key = itD->first;
        double      val = itD->second;
        ATH_MSG_DEBUG("\t" << key << ": " << val);
    }
    
    ATH_MSG_DEBUG("VecDouble variables: ");
    MapVecDouble::const_iterator itVD = m_Infos_VecDouble.begin();
    for(; itVD != m_Infos_VecDouble.end(); itVD++) {
        std::string key = itVD->first;
        std::vector<double> val = itVD->second;
        ATH_MSG_DEBUG("\t" << key << ": " << val.size());
        for(unsigned int iVal=0; iVal<val.size(); iVal++) ATH_MSG_DEBUG("\t\t" << val[iVal]);
    }
    
    ATH_MSG_DEBUG("String variables: ");
    MapString::const_iterator itS = m_Infos_String.begin();
    for(; itS != m_Infos_String.end(); itS++) {
        std::string key = itS->first;
        std::string val = itS->second;
        ATH_MSG_DEBUG("\t" << key << ": " << val);
    }
    
//     ATH_MSG_DEBUG("VecString variables: ");
//     MapVecString::const_iterator itVS = m_Infos_VecString.begin();
//     for(; itVS != m_Infos_VecString.end(); itVS++) {
//         std::string key = itVS->first;
//         std::vector<std::string> val = itVS->second;
//         ATH_MSG_DEBUG("\t" << key << ": " << val.size());
//         for(unsigned int iVal=0; iVal<val.size(); iVal++) ATH_MSG_DEBUG("\t\t" << val[iVal]);
//     }
    
    ATH_MSG_DEBUG("Done dumping information in Tool_InformationStore called " << name());
    return StatusCode::SUCCESS;
}



void PanTau::Tool_InformationStore::checkEFOContainer(const eflowObjectContainer* inputContainer, eflowObjectContainer* outputContainer) {
    
    for(unsigned int iEFO=0; iEFO<inputContainer->size(); iEFO++) {
        const eflowObject* curEFO = inputContainer->at(iEFO);
        double cur_Et  = curEFO->et();
        double cur_Eta = curEFO->eta();
        double cur_Phi = curEFO->phi();
        double cur_M   = curEFO->m();
        
        //loop over EFOs that were already added to check for duplicates
        bool isOK = true;
        for(unsigned int jEFO=0; jEFO<outputContainer->size(); jEFO++) {
            const eflowObject* goodEFO = outputContainer->at(jEFO);
            double diff_Et  = fabs(goodEFO->et()  - cur_Et);
            double diff_Eta = fabs(goodEFO->eta() - cur_Eta);
            //double diff_Phi = fabs(goodEFO->phi() - cur_Phi); // bug! shouldn't matter much but nevertheless...
	    double diff_Phi = fabs( TVector2::Phi_mpi_pi( goodEFO->phi() - cur_Phi ) );
            double diff_m   = fabs(goodEFO->m()   - cur_M);
            
            if(diff_Et < 0.000001 && diff_Eta < 0.000001  && diff_Phi < 0.000001  && diff_m < 0.000001 ) {
                ATH_MSG_DEBUG("Skipping dubplicated EFO at " << curEFO << " of charge " << curEFO->charge() << " with Et/Eta/Phi: " << cur_Et << " / " << cur_Eta << " / " << cur_Phi);
                isOK = false;
                break;
            }
        }//end loop over good EFOs
        
        if(isOK == false) continue;
        
        outputContainer->push_back( const_cast<eflowObject*>(curEFO) );
        
    }//end loop over EFOs
}


