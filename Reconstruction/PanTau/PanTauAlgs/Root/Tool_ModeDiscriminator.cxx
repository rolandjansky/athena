/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/Tool_ModeDiscriminator.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/TauFeature.h"
#include "PanTauAlgs/PanTauSeed.h"
#include "PanTauAlgs/HelperFunctions.h"
#include "PathResolver/PathResolver.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include <memory>

PanTau::Tool_ModeDiscriminator::Tool_ModeDiscriminator(const std::string& name) :
  asg::AsgTool(name),
  m_Name_InputAlg("InvalidInputAlg"),
  m_Name_ModeCase("InvalidModeCase"),
  m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
  m_MVABDT_List()
{
  declareProperty("calibFolder",              m_calib_path,               "Location of calib files in cvmfs");//sync'd with tauRecFlags.tauRecToolsCVMFSPath()
  declareProperty("Name_InputAlg",            m_Name_InputAlg,            "Name of the input algorithm for this instance");
  declareProperty("Name_ModeCase",            m_Name_ModeCase,            "Name of the two modes to be distinguished for this instance");
  declareProperty("Tool_InformationStore",    m_Tool_InformationStore,    "Handle to the information store tool");
  declareProperty("Tool_InformationStoreName",m_Tool_InformationStoreName,"Handle to the information store tool");
}


PanTau::Tool_ModeDiscriminator::~Tool_ModeDiscriminator() {
}


StatusCode PanTau::Tool_ModeDiscriminator::initialize() {

  ATH_MSG_DEBUG( name() << " initialize()" );
  m_init=true;
    
  ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InformationStore, m_Tool_InformationStoreName ) );

  ATH_CHECK(m_Tool_InformationStore.retrieve());
    
  // get the required information from the informationstore tool
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("ModeDiscriminator_BinEdges_Pt", m_BinEdges_Pt));
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("ModeDiscriminator_ReaderOption", m_ReaderOption) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("ModeDiscriminator_TMVAMethod", m_MethodName) );
    
  // build the name of the variable that contains the variable list for this discri tool
  std::string varNameList_Prefix  = "ModeDiscriminator_BDTVariableNames_";
  std::string varNameList_Full    = varNameList_Prefix + m_Name_InputAlg + "_" + m_Name_ModeCase;
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecString(varNameList_Full, m_List_BDTVariableNames) );
    
  std::string varDefaultValueList_Prefix  = "ModeDiscriminator_BDTVariableDefaults_";
  std::string varDefaultValueList_Full    = varDefaultValueList_Prefix + m_Name_InputAlg + "_" + m_Name_ModeCase;
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble(varDefaultValueList_Full, m_List_BDTVariableDefaultValues) );
    
    
  // consistency check:
  // Number of feature names and feature default values has to match
  if ( m_List_BDTVariableDefaultValues.size() != m_List_BDTVariableNames.size() ) {
    ATH_MSG_ERROR("Number of variable names does not match number of default values! Check jobOptions!");
    return StatusCode::FAILURE;
  }
  
  // Create reader for each pT Bin    
  unsigned int nPtBins = m_BinEdges_Pt.size() - 1; // nBins =  Edges-1
  for (unsigned int iPtBin=0; iPtBin<nPtBins; iPtBin++) {
        
    double bin_lowerVal         = m_BinEdges_Pt[iPtBin];
    double bin_upperVal         = m_BinEdges_Pt[iPtBin+1];
        
    std::string bin_lowerStr    = m_HelperFunctions.convertNumberToString(bin_lowerVal/1000.);
    std::string bin_upperStr    = m_HelperFunctions.convertNumberToString(bin_upperVal/1000.);
        
    std::string curPtBin        = "ET_" + bin_lowerStr + "_" + bin_upperStr;
    std::string curModeCase     = m_Name_ModeCase;
    
    // weight files
    std::string curWeightFile = m_calib_path + (m_calib_path.length() ? "/" : "");
    curWeightFile += "TrainModes_";
    curWeightFile += m_Name_InputAlg + "_";
    curWeightFile += curPtBin + "_";
    curWeightFile += curModeCase + "_";
    curWeightFile += m_MethodName + ".weights.root";

    std::string resolvedWeightFileName = PathResolverFindCalibFile(curWeightFile);

    if (resolvedWeightFileName.empty()) {
      ATH_MSG_ERROR("Weight file " << curWeightFile << " not found!");
      return StatusCode::FAILURE;
    }
    
    // MVAUtils BDT       
    std::unique_ptr<TFile> fBDT = std::make_unique<TFile>( resolvedWeightFileName.c_str() );
    TTree* tBDT = dynamic_cast<TTree*> (fBDT->Get("BDT"));
    std::unique_ptr<MVAUtils::BDT> curBDT = std::make_unique<MVAUtils::BDT>(tBDT);
    if (curBDT == nullptr) {
      ATH_MSG_ERROR( "Failed to create MVAUtils::BDT for " << resolvedWeightFileName );
      return StatusCode::FAILURE;
    }

    m_MVABDT_List.push_back(std::move(curBDT));
        
  }//end loop over pt bins to get weight files, reference hists and MVAUtils::BDT objects
    
  return StatusCode::SUCCESS;
}


void PanTau::Tool_ModeDiscriminator::updateReaderVariables(PanTau::PanTauSeed* inSeed, std::vector<float>& list_BDTVariableValues) const {
    
  //update features used in MVA with values from current seed
  // use default value for feature if it is not present in current seed
  //NOTE! This has to be done (even if the seed pt is bad) otherwise problems with details storage
  //      [If this for loop is skipped, it is not guaranteed that all details are set to their proper default value]
  PanTau::TauFeature* seedFeatures = inSeed->getFeatures();

  for (unsigned int iVar=0; iVar<m_List_BDTVariableNames.size(); iVar++) {
    std::string curVar = m_Name_InputAlg + "_" + m_List_BDTVariableNames[iVar];
        
    bool isValid;
    double newValue = seedFeatures->value(curVar, isValid);
    if (!isValid) {
      ATH_MSG_DEBUG("\tUse default value as the feature (the one below this line) was not calculated");
      newValue = m_List_BDTVariableDefaultValues[iVar];
      //add this feature with its default value for the details later
      seedFeatures->addFeature(curVar, newValue);
    }
        
    list_BDTVariableValues[iVar] = static_cast<float>(newValue);
  }//end loop over BDT vars
    
  return;
}


double PanTau::Tool_ModeDiscriminator::getResponse(PanTau::PanTauSeed* inSeed, bool& isOK) const {
    
  std::vector<float> list_BDTVariableValues(m_List_BDTVariableNames.size());

  updateReaderVariables(inSeed, list_BDTVariableValues);
    
  if (inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_BadPtValue)) {
    ATH_MSG_DEBUG("WARNING Seed has bad pt value! " << inSeed->getTauJet()->pt() << " MeV");
    isOK = false;
    return -2;
  }
    
  //get the pt bin of input Seed
  //NOTE: could be moved to decay mode determinator tool...
  double seedPt = inSeed->p4().Pt();
  int ptBin = -1;
  for (unsigned int iPtBin=0; iPtBin<m_BinEdges_Pt.size()-1; iPtBin++) {
    if (seedPt > m_BinEdges_Pt[iPtBin] && seedPt < m_BinEdges_Pt[iPtBin+1]) {
      ptBin = iPtBin;
      break;
    }
  }
  if (ptBin == -1) {
    ATH_MSG_WARNING("Could not find ptBin for tau seed with pt " << seedPt);
    isOK = false;
    return -2.;
  }
    
  //get mva response
  double mvaResponse = m_MVABDT_List[ptBin]->GetGradBoostMVA(list_BDTVariableValues);
    
  isOK = true;
  return mvaResponse;
}
