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

//! PanTau includes
#include "PanTauAlgs/Tool_InformationStore.h"

// ROOT includes
#include "TVector2.h"


PanTau::Tool_InformationStore::Tool_InformationStore(
    const std::string& name ) :
        asg::AsgTool(name)
{

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
    
    ATH_CHECK( this->getInfo_String("Name_TauRecContainer", m_Name_Container_TauRec) );
    ATH_CHECK( this->getInfo_String("Name_TrackParticleContainer", m_Name_Container_Tracks) );
    
    return StatusCode::SUCCESS;
}



// StatusCode PanTau::Tool_InformationStore::finalize() {
//     StatusCode sc = AlgTool::finalize();
//     return sc;
// }


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
    
    ATH_MSG_DEBUG("Done dumping information in Tool_InformationStore called " << name());
    return StatusCode::SUCCESS;
}



