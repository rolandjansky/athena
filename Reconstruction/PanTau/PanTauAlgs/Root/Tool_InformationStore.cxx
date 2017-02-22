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

#define GeV 1000

void PanTau::Tool_InformationStore::ABRDefaultInit(){
  #ifdef XAOD_ANALYSIS

  ATH_MSG_INFO("Setting Tool_InformationStore In environment ABR");

  // Boolean values
  MapInt m01 = {
    {"UseDefaultCellBasedConfig",1},//is this one necessary
    {"TauConstituents_UsePionMass",1},
    {"FeatureExtractor_UseEmptySeeds",0},
    {"TauConstituents_eflowRec_UseMomentumAsEnergy",0},
    {"TauConstituents_UseShrinkingCone",0}
  };
  setMapInt(m01);
  
  // double values
  MapDouble m02 = {
    {"eflowRec_Assoc_DeltaR",0.4},
    {"TauConstituents_Types_DeltaRCore",0.2},
    {"TauConstituents_MaxEta",9.9},
    {"TauConstituents_PreselectionMinEnergy",500.},
    {"TauConstituents_eflowRec_BDTThreshold_Pi0Neut",-0.04},
    // PanTau BDT Cut values --- CellBased
    {"DecayModeDeterminator_BDTCutValue_R10X_CellBased",0.52},
    {"DecayModeDeterminator_BDTCutValue_R11X_CellBased",-0.33},
    {"DecayModeDeterminator_BDTCutValue_R110_CellBased",0.47},
    {"DecayModeDeterminator_BDTCutValue_R1XX_CellBased",-0.21},
    {"DecayModeDeterminator_BDTCutValue_R30X_CellBased",-0.13},
    {"DecayModeDeterminator_BDTCutValue_R3XX_CellBased",-0.08},
    // PanTau BDT Cut values --- eflowRec
    {"DecayModeDeterminator_BDTCutValue_R10X_eflowRec",-0.15},
    {"DecayModeDeterminator_BDTCutValue_R11X_eflowRec",-0.60},
    {"DecayModeDeterminator_BDTCutValue_R110_eflowRec",-0.08},
    {"DecayModeDeterminator_BDTCutValue_R1XX_eflowRec",0.03},
    {"DecayModeDeterminator_BDTCutValue_R30X_eflowRec",-0.25},
    {"DecayModeDeterminator_BDTCutValue_R3XX_eflowRec",-0.23}
  };
  
  setMapDouble(m02);

  // String values
  MapString m03 = {
    {"Name_TauRecContainer","TauJets"},
    {"Name_eflowRecContainer","eflowObjects_tauMode"},
    {"Name_TrackParticleContainer","TrackParticleCandidate"},
    {"Name_PanTauSeedsContainer","PanTau_OutputSeeds"},
    {"ModeDiscriminator_ReaderOption","!Color:Silent"},
    {"ModeDiscriminator_TMVAMethod","BDTG"},
    {"FeatureExtractor_VarTypeName_varTypeName_Sum", "Sum"},
    {"FeatureExtractor_VarTypeName_varTypeName_Ratio", "Ratio"},
    {"FeatureExtractor_VarTypeName_varTypeName_EtInRing", "EtInRing"},
    {"FeatureExtractor_VarTypeName_varTypeName_Isolation", "Isolation"},
    {"FeatureExtractor_VarTypeName_varTypeName_Num", "Num"},
    {"FeatureExtractor_VarTypeName_varTypeName_Mean", "Mean"},
    {"FeatureExtractor_VarTypeName_varTypeName_StdDev", "StdDev"},
    {"FeatureExtractor_VarTypeName_varTypeName_HLV", "HLV"},
    {"FeatureExtractor_VarTypeName_varTypeName_Angle", "Angle"},
    {"FeatureExtractor_VarTypeName_varTypeName_DeltaR", "DeltaR"},
    {"FeatureExtractor_VarTypeName_varTypeName_JetMoment", "JetMoment"},
    {"FeatureExtractor_VarTypeName_varTypeName_Combined", "Combined"},
    {"FeatureExtractor_VarTypeName_varTypeName_JetShape", "JetShape"},
    {"FeatureExtractor_VarTypeName_varTypeName_ImpactParams", "ImpactParams"},
    {"FeatureExtractor_VarTypeName_varTypeName_Basic", "Basic"},
    {"FeatureExtractor_VarTypeName_varTypeName_PID", "PID"},
    {"FeatureExtractor_VarTypeName_varTypeName_Shots", "Shots"},
  };
  setMapString(m03);

  // vector<double> values
  // In Config_PanTau.py, this was called "vector<float> values". This was changed to "double".
  // The "Units.GeV" (import AthenaCommon.SystemOfUnits as Units) was now replaced by "GeV" (#include "CLHEP/Units/SystemOfUnits.h", using CLHEP::GeV)
  MapVecDouble m04 = {
    {"TauConstituents_BinEdges_Eta",{0.000, 0.800, 1.400, 1.500, 1.900, 9.900}},
    {"TauConstituents_Selection_Neutral_EtaBinned_EtCut",{2.1*GeV, 2.5*GeV, 2.6*GeV, 2.4*GeV, 1.9*GeV}},
    {"TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut",{2.1*GeV, 2.5*GeV, 2.6*GeV, 2.4*GeV, 1.9*GeV}},
    {"TauConstituents_Selection_Charged_EtaBinned_EtCut",{1.0*GeV, 1.0*GeV, 1.0*GeV, 1.0*GeV, 1.0*GeV}},
    {"TauConstituents_Selection_OutNeut_EtaBinned_EtCut",{1.0*GeV, 1.0*GeV, 1.0*GeV, 1.0*GeV, 1.0*GeV}},
    {"TauConstituents_Selection_OutChrg_EtaBinned_EtCut",{1.0*GeV, 1.0*GeV, 1.0*GeV, 1.0*GeV, 1.0*GeV}},
    {"TauConstituents_Selection_NeutLowA_EtaBinned_EtCut",{1.85*GeV, 2.25*GeV, 2.35*GeV, 2.15*GeV, 1.65*GeV}},
    {"TauConstituents_Selection_NeutLowB_EtaBinned_EtCut",{1.6*GeV, 2.0*GeV, 2.1*GeV, 1.9*GeV, 1.4*GeV}},
    {"eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong",{2.5*GeV, 2.5*GeV, 1.9*GeV, 2.5*GeV, 2.3*GeV}},
    {"eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong",{2.5*GeV, 2.5*GeV, 2.5*GeV, 2.5*GeV, 2.5*GeV}},
    // Eta Binned    P I 0 - B D T   C U T S
    {"CellBased_BinEdges_Eta",{0.000, 0.800, 1.400, 1.500, 1.900, 9.900}},
    {"CellBased_EtaBinned_Pi0MVACut_1prong",{0.46, 0.39, 0.51, 0.47, 0.54}},
    {"CellBased_EtaBinned_Pi0MVACut_3prong",{0.47, 0.52, 0.60, 0.55, 0.50}},
    {"eflowRec_BinEdges_Eta",{0.000, 0.800, 1.400, 1.500, 1.900, 9.900}},
    {"eflowRec_EtaBinned_Pi0MVACut_1prong",{0.09, 0.09, 0.09, 0.08, 0.05}},
    {"eflowRec_EtaBinned_Pi0MVACut_3prong",{0.09, 0.09, 0.09, 0.09, 0.07}},
    // P T   B I N S
    {"ModeDiscriminator_BinEdges_Pt",{10*GeV, 100000*GeV}},
    {"ModeDiscriminator_BDTVariableDefaults_CellBased_1p0n_vs_1p1n", {-9.0,     -0.2,    -10.0,     -0.2,     -2.0}},
    {"ModeDiscriminator_BDTVariableDefaults_CellBased_1p1n_vs_1pXn", {-9.0,      -200.0,        -0.2,        -5.0,        -2.0}},
    {"ModeDiscriminator_BDTVariableDefaults_CellBased_3p0n_vs_3pXn", {-0.2,    -9.0,       -0.2,       -2.0,     -200.0}},
  };
  setMapVecDouble(m04);

  // vector<string> values
  MapVecString m05 = {
    {"Names_InputAlgorithms",{"CellBased"}},
    {"Names_ModeCases",{"1p0n_vs_1p1n","1p1n_vs_1pXn","3p0n_vs_3pXn"}},
    // ---> eflowRec BDT variables
    {"ModeDiscriminator_BDTVariableNames_eflowRec_1p0n_vs_1p1n",{"Charged_Ratio_EtOverEtAllConsts","Basic_NPi0NeutConsts","Neutral_PID_BDTValues_EtSort_1","Combined_DeltaR1stNeutralTo1stCharged"}},
    {"ModeDiscriminator_BDTVariableNames_eflowRec_1p1n_vs_1pXn",{"Neutral_PID_BDTValues_BDTSort_2","Neutral_Ratio_EtOverEtAllConsts","Basic_NNeutralConsts","Neutral_HLV_SumM"}},
    {"ModeDiscriminator_BDTVariableNames_eflowRec_3p0n_vs_3pXn",{"Basic_NPi0NeutConsts","Neutral_PID_BDTValues_BDTSort_1","Charged_HLV_SumPt","Charged_Ratio_EtOverEtAllConsts","Neutral_Mean_DRToLeading_WrtEtAllConsts"}},
    // ---> CellBased BDT variables
    {"ModeDiscriminator_BDTVariableNames_CellBased_1p0n_vs_1p1n",{"Neutral_PID_BDTValues_BDTSort_1","Neutral_Ratio_1stBDTEtOverEtAllConsts","Combined_DeltaR1stNeutralTo1stCharged","Charged_JetMoment_EtDRxTotalEt","Neutral_Shots_NPhotonsInSeed"}},
    {"ModeDiscriminator_BDTVariableNames_CellBased_1p1n_vs_1pXn",{"Neutral_PID_BDTValues_BDTSort_2","Neutral_HLV_SumM","Neutral_Ratio_EtOverEtAllConsts","Basic_NNeutralConsts","Neutral_Shots_NPhotonsInSeed"}},
    {"ModeDiscriminator_BDTVariableNames_CellBased_3p0n_vs_3pXn",{"Neutral_Ratio_EtOverEtAllConsts","Neutral_PID_BDTValues_BDTSort_1","Charged_StdDev_Et_WrtEtAllConsts","Neutral_Shots_NPhotonsInSeed","Charged_HLV_SumM"}},
    {"ModeDiscriminator_BDTVariableTypes_CellBased_1p0n_vs_1p1n", {"F","F","F","F","F"}},
    {"ModeDiscriminator_BDTVariableTypes_CellBased_1p1n_vs_1pXn", {"F","F","F","F","F"}},
    {"ModeDiscriminator_BDTVariableTypes_CellBased_3p0n_vs_3pXn", {"F","F","F","F","F"}},
  };
  setMapVecString(m05);

  #endif
}



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
    m_init=true;

    //This function does nothing in athena
    ABRDefaultInit();

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



