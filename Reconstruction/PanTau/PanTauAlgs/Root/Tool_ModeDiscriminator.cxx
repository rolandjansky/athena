/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Tool_ModeDiscriminator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for PID of TauSeeds
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

//! C++
#include <string>

//! PanTau includes
#include "PanTauAlgs/Tool_ModeDiscriminator.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/TauFeature.h"
#include "PanTauAlgs/PanTauSeed.h"

//! Root
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TMVA/Reader.h"

//!Other
#include "PathResolver/PathResolver.h"


PanTau::Tool_ModeDiscriminator::Tool_ModeDiscriminator(
    const std::string& name ) :
        asg::AsgTool(name),
        m_Name_InputAlg("InvalidInputAlg"),
        m_Name_ModeCase("InvalidModeCase"),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
        m_TMVA_ReaderList()
{
    declareProperty("Name_InputAlg",            m_Name_InputAlg,            "Name of the input algorithm for this instance");
    declareProperty("Name_ModeCase",            m_Name_ModeCase,            "Name of the two modes to be distinguished for this instance");
    declareProperty("Tool_InformationStore",    m_Tool_InformationStore,    "Handle to the information store tool");
}



PanTau::Tool_ModeDiscriminator::~Tool_ModeDiscriminator() {
}



StatusCode PanTau::Tool_ModeDiscriminator::initialize() {

    ATH_MSG_DEBUG( name() << " initialize()" );
    
    ATH_MSG_DEBUG("InputAlg   : "  << m_Name_InputAlg);
    ATH_MSG_DEBUG("Mode Case  : "  << m_Name_ModeCase);
    
    ATH_CHECK(m_Tool_InformationStore.retrieve());
    
    //get the required information from the informationstore tool
    ATH_MSG_DEBUG("Get infos from information store & configure...");
    ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("ModeDiscriminator_BinEdges_Pt", m_BinEdges_Pt));
    ATH_CHECK( m_Tool_InformationStore->getInfo_String("ModeDiscriminator_ReaderOption", m_ReaderOption) );
    ATH_CHECK( m_Tool_InformationStore->getInfo_String("ModeDiscriminator_TMVAMethod", m_MethodName) );
    
    //build the name of the variable that contains the variable list for this discri tool
    std::string varNameList_Prefix  = "ModeDiscriminator_BDTVariableNames_";
    std::string varNameList_Full    = varNameList_Prefix + m_Name_InputAlg + "_" + m_Name_ModeCase;
    ATH_CHECK( m_Tool_InformationStore->getInfo_VecString(varNameList_Full, m_List_BDTVariableNames) );
    
    std::string varDefaultValueList_Prefix  = "ModeDiscriminator_BDTVariableDefaults_";
    std::string varDefaultValueList_Full    = varDefaultValueList_Prefix + m_Name_InputAlg + "_" + m_Name_ModeCase;
    ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble(varDefaultValueList_Full, m_List_BDTVariableDefaultValues) );
    
    
    //consistency check:
    // Number of feature names and feature default values has to match
    if( m_List_BDTVariableDefaultValues.size() != m_List_BDTVariableNames.size() ) {
        ATH_MSG_ERROR("Number of variable names does not match number of default values! Check jobOptions!");
        return StatusCode::FAILURE;
    }
    
    //! ////////////////////////
    //! Create list of BDT variables to link to the reader
    
    m_List_BDTVariableValues = std::vector<float>(0);
    for(unsigned int iVar=0; iVar<m_List_BDTVariableNames.size(); iVar++) {
        m_List_BDTVariableValues.push_back(0);
    }
    
    
    //! ////////////////////////
    //! Create reader for each pT Bin
    
    unsigned int nPtBins = m_BinEdges_Pt.size() - 1; // nBins =  Edges-1
    for(unsigned int iPtBin=0; iPtBin<nPtBins; iPtBin++) {
        ATH_MSG_DEBUG("PtBin " << iPtBin+1 << " / " << nPtBins);
        
        double bin_lowerVal         = m_BinEdges_Pt[iPtBin];
        double bin_upperVal         = m_BinEdges_Pt[iPtBin+1];
        
        std::string bin_lowerStr    = m_HelperFunctions.convertNumberToString(bin_lowerVal/1000.);
        std::string bin_upperStr    = m_HelperFunctions.convertNumberToString(bin_upperVal/1000.);
        
        std::string curPtBin        = "ET_" + bin_lowerStr + "_" + bin_upperStr;
        std::string curModeCase     = m_Name_ModeCase;
        
        
        //! ////////////////////////
        //! weight files
        ATH_MSG_DEBUG("\tGet the weight file");
        std::string curWeightFile = "";
        curWeightFile += "TrainModes_";
        curWeightFile += m_Name_InputAlg + "_";
        curWeightFile += curPtBin + "_";
        curWeightFile += curModeCase + "_";
        curWeightFile += m_MethodName + ".weights.xml";
        
        std::string resolvedWeightFileName = PathResolver::find_file(curWeightFile, "DATAPATH");
        if(resolvedWeightFileName == "") {
            ATH_MSG_ERROR("Weight file " << curWeightFile << " not found!");
            return StatusCode::FAILURE;
        }
        
        ATH_MSG_DEBUG("\t\tAdded weight file: " << resolvedWeightFileName);
        
        
        //! ////////////////////////
        //! TMVA Readers
        ATH_MSG_DEBUG("\tCreate TMVA::Readers");
        TMVA::Reader* curReader = new TMVA::Reader((TString)m_ReaderOption);
        if(msgLvl(MSG::DEBUG) == true) curReader->SetVerbose(true);
        
        //setup variables for reader
        for(unsigned int iVar=0; iVar<m_List_BDTVariableNames.size(); iVar++) {
            TString variableNameForReader = "tau_pantauFeature_" + m_Name_InputAlg + "_" + m_List_BDTVariableNames[iVar];
            ATH_MSG_DEBUG("\t\tAdding variable to reader: " << variableNameForReader << " var stored at: " << &(m_List_BDTVariableValues[iVar]));
            curReader->AddVariable(variableNameForReader, &(m_List_BDTVariableValues[iVar]));
        }//end loop over variables
        
        curReader->BookMVA( m_MethodName, resolvedWeightFileName);
        ATH_MSG_DEBUG("\t\tStoring new TMVA::Reader at " << curReader);
        m_TMVA_ReaderList.push_back(curReader);
        
    }//end loop over pt bins to get weight files, reference hists and TMVA Readers
    
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_ModeDiscriminator::finalize() {
    
    //delete the readers
    for(unsigned int iReader=0; iReader<m_TMVA_ReaderList.size(); iReader++) {
        TMVA::Reader* curReader = m_TMVA_ReaderList[iReader];
        if(curReader != 0) delete curReader;
    }
    
    return StatusCode::SUCCESS;
}



void    PanTau::Tool_ModeDiscriminator::updateReaderVariables(PanTau::PanTauSeed2* inSeed) {
    
    //update features used in MVA with values from current seed
    // use default value for feature if it is not present in current seed
    //NOTE! This has to be done (even if the seed pt is bad) otherwise problems with details storage
    //      [If this for loop is skipped, it is not guaranteed that all details are set to their proper default value]
    PanTau::TauFeature2* seedFeatures = inSeed->getFeatures();
    ATH_MSG_DEBUG( "Update the variables that are used in the readers...");
    for(unsigned int iVar=0; iVar<m_List_BDTVariableNames.size(); iVar++) {
        std::string curVar = m_Name_InputAlg + "_" + m_List_BDTVariableNames[iVar];
        
        bool    isValid;
        double  newValue = seedFeatures->value(curVar, isValid);
        if(isValid == false) {
            ATH_MSG_DEBUG("\tUse default value as the feature (the one below this line) was not calculated");
            newValue = m_List_BDTVariableDefaultValues[iVar];
            //add this feature with its default value for the details later
            seedFeatures->addFeature(curVar, newValue);
        }
        
        ATH_MSG_DEBUG("\tUpdate variable " << curVar << " from " << m_List_BDTVariableValues[iVar] << " to " << newValue);
        m_List_BDTVariableValues[iVar] = (float)newValue;
    }//end loop over BDT vars for update
    
    return;
}



double PanTau::Tool_ModeDiscriminator::getResponse(PanTau::PanTauSeed2* inSeed, bool& isOK) {
    
    ATH_MSG_DEBUG("get bdt response now");
    
    updateReaderVariables(inSeed);
    
    if(inSeed->isOfTechnicalQuality(PanTau::PanTauSeed2::t_BadPtValue) == true) {
        ATH_MSG_DEBUG("WARNING Seed has bad pt value! " << inSeed->getTauJet()->pt() << " MeV");
        isOK = false;
        return -2;
    }
    
    //get the pt bin of input Seed
    //NOTE: could be moved to decay mode determinator tool...
    double          seedPt  = inSeed->p4().Pt();
    int             ptBin   = -1;
    for(unsigned int iPtBin=0; iPtBin<m_BinEdges_Pt.size()-1; iPtBin++) {
        if(seedPt > m_BinEdges_Pt[iPtBin] && seedPt < m_BinEdges_Pt[iPtBin+1]) {
            ptBin = iPtBin;
            break;
        }
    }
    if(ptBin == -1) {
        ATH_MSG_WARNING("Could not find ptBin for tau seed with pt " << seedPt);
        isOK = false;
        return -2.;
    }
    
    //get mva response
    TMVA::Reader*   curReader   = m_TMVA_ReaderList[ptBin];
    if(curReader == 0) {
        ATH_MSG_ERROR("TMVA::Reader object for current tau seed points to 0");
        isOK = false;
        return -2.;
    }
    
    
//     ATH_MSG_DEBUG("Values of BDT Variables: ");
//     for(unsigned int iVar=0; iVar<m_List_BDTVariableNames.size(); iVar++) ATH_MSG_WARNING(m_List_BDTVariableNames.at(iVar) << ": " << m_List_BDTVariableValues.at(iVar) << " (stored at " << &(m_List_BDTVariableValues.at(iVar)) << ")");
    
    double  mvaResponse     = curReader->EvaluateMVA((TString)m_MethodName);
    ATH_MSG_DEBUG("MVA response from " << m_MethodName << " in " << m_Name_ModeCase << " is " << mvaResponse);
    
    isOK = true;
    return mvaResponse;
}




