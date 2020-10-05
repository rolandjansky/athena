/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0ScoreCalculator.cxx
// package:     Reconstruction/tauRec
// authors:     Benedict Winter, Will Davey
// date:        2012-10-09
//-----------------------------------------------------------------------------

#include "tauRecTools/TauPi0ScoreCalculator.h"
#include "tauRecTools/HelperFunctions.h"
#include "xAODPFlow/PFO.h"

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0ScoreCalculator::TauPi0ScoreCalculator(const std::string& name) :
  TauRecToolBase(name),
  m_mvaBDT(nullptr)
{
    declareProperty("BDTWeightFile", m_weightfile);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0ScoreCalculator::~TauPi0ScoreCalculator() 
{
}

StatusCode TauPi0ScoreCalculator::initialize() 
{
  std::string weightFile = find_file(m_weightfile);

  m_mvaBDT = std::make_unique<tauRecTools::BDTHelper>();
  ATH_CHECK(m_mvaBDT->initialize(weightFile));
 
  return StatusCode::SUCCESS;
}

StatusCode TauPi0ScoreCalculator::executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer) const
{
    //---------------------------------------------------------------------
    // only run on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("ScoreCalculator: new tau. \tpt = " << pTau.pt() << "\teta = " << pTau.eta() << "\tphi = " << pTau.phi() << "\tnprongs = " << pTau.nTracks());

    //---------------------------------------------------------------------
    // retrieve neutral PFOs from tau, calculate BDT scores and store them in PFO
    //---------------------------------------------------------------------
    for( auto neutralPFO : neutralPFOContainer )
    {
      float BDTScore = calculateScore(neutralPFO);
      neutralPFO->setBDTPi0Score(BDTScore);
    }

    ATH_MSG_DEBUG("End of TauPi0ScoreCalculator::execute");

    return StatusCode::SUCCESS;
}

float TauPi0ScoreCalculator::calculateScore(const xAOD::PFO* neutralPFO) const
{
    std::map<TString, float> availableVariables; // map of the variable name to its value
    
    float Abs_FIRST_ETA = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_FIRST_ETA, Abs_FIRST_ETA) == false) {
        ATH_MSG_WARNING("Can't find FIRST_ETA. Set it to 0.");
    }
    Abs_FIRST_ETA = std::abs(Abs_FIRST_ETA);
    availableVariables.insert(std::make_pair("Pi0Cluster_Abs_FIRST_ETA", Abs_FIRST_ETA));

    float SECOND_R = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_R, SECOND_R) == false) {
        ATH_MSG_WARNING("Can't find SECOND_R. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_SECOND_R", SECOND_R));

    float Abs_DELTA_THETA = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_DELTA_THETA, Abs_DELTA_THETA) == false) {
        ATH_MSG_WARNING("Can't find DELTA_THETA. Set it to 0.");
    }
    Abs_DELTA_THETA = std::abs(Abs_DELTA_THETA);
    availableVariables.insert(std::make_pair("Pi0Cluster_Abs_DELTA_THETA", Abs_DELTA_THETA));

    float CENTER_LAMBDA_helped = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_CENTER_LAMBDA, CENTER_LAMBDA_helped) == false) {
        ATH_MSG_WARNING("Can't find CENTER_LAMBDA. Set it to 0.");
    }
    CENTER_LAMBDA_helped = fmin(CENTER_LAMBDA_helped, 1000.);
    availableVariables.insert(std::make_pair("Pi0Cluster_CENTER_LAMBDA_helped", CENTER_LAMBDA_helped));
    
    float LONGITUDINAL = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_LONGITUDINAL, LONGITUDINAL) == false) {
        ATH_MSG_WARNING("Can't find LONGITUDINAL. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_LONGITUDINAL", LONGITUDINAL));

    float ENG_FRAC_EM = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_EM, ENG_FRAC_EM) == false) {
        ATH_MSG_WARNING("Can't find ENG_FRAC_EM. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_ENG_FRAC_EM", ENG_FRAC_EM));

    float ENG_FRAC_CORE = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_CORE, ENG_FRAC_CORE) == false) { 
        ATH_MSG_WARNING("Can't find ENG_FRAC_CORE. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_ENG_FRAC_CORE", ENG_FRAC_CORE));

    float log_SECOND_ENG_DENS = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_ENG_DENS, log_SECOND_ENG_DENS) == false) { 
        ATH_MSG_WARNING("Can't find SECOND_ENG_DENS. Set it to 0.");
    }
    if(log_SECOND_ENG_DENS==0.) {
        log_SECOND_ENG_DENS=-50.;
    }
    else {
        log_SECOND_ENG_DENS = log(log_SECOND_ENG_DENS);
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_log_SECOND_ENG_DENS", log_SECOND_ENG_DENS));

    float EcoreOverEEM1 = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_EM1CoreFrac, EcoreOverEEM1) == false) { 
        ATH_MSG_WARNING("Can't find EM1CoreFrac. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_EcoreOverEEM1", EcoreOverEEM1));
    
    int NPosECells_EM1 = 0;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM1, NPosECells_EM1) == false) { 
        ATH_MSG_WARNING("Can't find NPosECells_EM1. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_NPosECells_EM1", static_cast<float>(NPosECells_EM1)));

    int NPosECells_EM2 = 0;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM2, NPosECells_EM2) == false) { 
        ATH_MSG_WARNING("Can't find NPosECells_EM2. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_NPosECells_EM2", static_cast<float>(NPosECells_EM2)));
    
    float AbsFirstEtaWRTClusterPosition_EM1 = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM1, AbsFirstEtaWRTClusterPosition_EM1) == false) { 
        ATH_MSG_WARNING("Can't find firstEtaWRTClusterPosition_EM1. Set it to 0.");
    }
    AbsFirstEtaWRTClusterPosition_EM1 = std::abs(AbsFirstEtaWRTClusterPosition_EM1);
    availableVariables.insert(std::make_pair("Pi0Cluster_AbsFirstEtaWRTClusterPosition_EM1", AbsFirstEtaWRTClusterPosition_EM1));

    float secondEtaWRTClusterPosition_EM2 = 0.;
    if(neutralPFO->attribute(xAOD::PFODetails::PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM2, secondEtaWRTClusterPosition_EM2) == false) { 
        ATH_MSG_WARNING("Can't find secondEtaWRTClusterPosition_EM2. Set it to 0.");
    }
    availableVariables.insert(std::make_pair("Pi0Cluster_secondEtaWRTClusterPosition_EM2", secondEtaWRTClusterPosition_EM2)); 

    // Calculate BDT score, will be -999 when availableVariables lack variables
    float score = m_mvaBDT->getGradBoostMVA(availableVariables);

    return score;
}
