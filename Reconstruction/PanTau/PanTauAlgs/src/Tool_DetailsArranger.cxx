/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Tool_DetailsArranger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for PID of TauSeeds
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

//! C++
#include <string>
#include <vector>

#include "DataModel/ElementLink.h"

//! xAOD EDM
// #include "xAODTau/TauDefs.h"
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFOContainer.h"

//! PanTau includes
#include "PanTauAlgs/Tool_DetailsArranger.h"
#include "PanTauEvent/PanTauSeed.h"
#include "PanTauEvent/PanTauDetails.h"
#include "PanTauEvent/PanTauDetails_eflowRec.h"
#include "PanTauEvent/PanTauDetails_CellBased.h"
// #include "tauEvent/TauDetailsContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"


bool sortBDTscore(ElementLink< xAOD::PFOContainer > i, ElementLink< xAOD::PFOContainer > j){

  return ( i.cachedElement()->bdtPi0Score() > j.cachedElement()->bdtPi0Score() );

}


PanTau::Tool_DetailsArranger::Tool_DetailsArranger(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_sgSvc(0),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore")
{
    declareInterface<ITool_DetailsArranger>(this);
    declareProperty("Tool_InformationStore",            m_Tool_InformationStore,            "Tool handle to the information store tool");
}



PanTau::Tool_DetailsArranger::~Tool_DetailsArranger() {
}



StatusCode PanTau::Tool_DetailsArranger::initialize() {

    ATH_MSG_DEBUG( name() << " initialize()" );
    CHECK( AthAlgTool::initialize() );
    
    CHECK(service("StoreGateSvc", m_sgSvc));
    
    CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_Types_DeltaRCore", m_CoreCone) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_BinEdges_Eta", m_EtaBinEdges) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_Neutral_EtaBinned_EtCut", m_EtaBinnedEtCuts) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Sum",          m_varTypeName_Sum) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Ratio",        m_varTypeName_Ratio) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_EtInRing",     m_varTypeName_EtInRing) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Isolation",    m_varTypeName_Isolation) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Num",          m_varTypeName_Num) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Mean",         m_varTypeName_Mean) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_StdDev",       m_varTypeName_StdDev) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_HLV",          m_varTypeName_HLV) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Angle",        m_varTypeName_Angle) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_DeltaR",       m_varTypeName_DeltaR) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_JetMoment",    m_varTypeName_JetMoment) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Combined",     m_varTypeName_Combined) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_JetShape",     m_varTypeName_JetShape) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_ImpactParams", m_varTypeName_ImpactParams) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Basic",        m_varTypeName_Basic) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_PID",          m_varTypeName_PID) );
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Shots",        m_varTypeName_Shots) );
    
    return StatusCode::SUCCESS;
}



// StatusCode PanTau::Tool_DetailsArranger::finalize() {
//     CHECK(AlgTool::finalize());
//     
//     return StatusCode::SUCCESS;
// }





StatusCode PanTau::Tool_DetailsArranger::arrangeDetails(PanTau::PanTauSeed* inSeed) {//, Analysis::TauDetailsContainer* detailsCont) {
    std::string inputAlg = inSeed->getNameInputAlgorithm();
    
    ATH_MSG_DEBUG( "arrangeDetails called for input seed at: " << inSeed << " from inputalg: " << inputAlg);
//     ATH_MSG_DEBUG( "arrangeDetails called for container at : " << detailsCont);
    
    bool noAnyConstituents           = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoConstituentsAtAll);
    bool noSelConstituents           = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoSelectedConstituents);
    bool noValidInputTau             = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoValidInputTau);
    bool isBadSeed                   = (noAnyConstituents || noSelConstituents || noValidInputTau);
    m_expectInvalidFeatures          = isBadSeed;
    
    
    //create the general part of the details
    // relevant ID variables are taken care of in construction by passing the input seed
//     PanTau::PanTauDetails*  Details_PanTau_General  = new PanTau::PanTauDetails(inSeed);
    
    
    //set the PFO link vector and pantau 4-vector to default values for every tau first (xAOD technicality)
    //if the tau is valid, overwrite with non-default values
    xAOD::TauJet* tauJet = const_cast<xAOD::TauJet*>(inSeed->getTauJet());
    
    ATH_MSG_DEBUG( "check for bad seed -> isBadSeed = " << isBadSeed);
    if(isBadSeed == true) {
        ATH_MSG_DEBUG("This seed is not useable for detail arraning (other than validity flag)");
        if(inputAlg == "CellBased") tauJet->setPanTauDetail(xAOD::TauJetParameters::pantau_CellBasedInput_isPanTauCandidate, 0);
        if(inputAlg == "eflowRec")  tauJet->setPanTauDetail(xAOD::TauJetParameters::pantau_eflowRecInput_isPanTauCandidate, 0);
        return StatusCode::SUCCESS;
    }
    
    
    ATH_MSG_DEBUG( "arrange for seed from inputalg: " << inputAlg);
    if(inputAlg == "CellBased") {
        
        arrangePFOLinks(inSeed, tauJet);
        
        //Basic variables
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_isPanTauCandidate",      xAOD::TauJetParameters::pantau_CellBasedInput_isPanTauCandidate, PanTau::Tool_DetailsArranger::t_Int);
//         addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_RecoMode",               xAOD::TauJetParameters::pantau_CellBasedInput_DecayModeProto, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_RecoMode_PanTau",        xAOD::TauJetParameters::pantau_CellBasedInput_DecayMode, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_1p0n_vs_1p1n",  xAOD::TauJetParameters::pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_1p1n_vs_1pXn",  xAOD::TauJetParameters::pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_3p0n_vs_3pXn",  xAOD::TauJetParameters::pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn, PanTau::Tool_DetailsArranger::t_Float);
        
        
        //Final 4-vector
        tauJet->setP4(xAOD::TauJetParameters::PanTauCellBased, inSeed->getFinalMomentum().perp(), inSeed->getFinalMomentum().eta(), inSeed->getFinalMomentum().phi(), inSeed->getFinalMomentum().m());
        
        //BDT variables
        addPanTauDetailToTauJet(inSeed, "Basic_NNeutralConsts",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, "Charged_JetMoment_EtDRxTotalEt",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Charged_StdDev_Et_WrtEtAllConsts",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_HLV_SumM",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_PID_BDTValues_BDTSort_1",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_PID_BDTValues_BDTSort_2",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_Ratio_1stBDTEtOverEtAllConsts",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_Ratio_EtOverEtAllConsts",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_Shots_NPhotonsInSeed",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Combined_DeltaR1stNeutralTo1stCharged",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Charged_HLV_SumM",
                                xAOD::TauJetParameters::pantau_CellBasedInput_BDTVar_Charged_HLV_SumM, PanTau::Tool_DetailsArranger::t_Float);
        
    }
    
    if(inputAlg == "eflowRec") {
        
        //Basic variables
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_isPanTauCandidate",      xAOD::TauJetParameters::pantau_eflowRecInput_isPanTauCandidate, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_RecoMode",               xAOD::TauJetParameters::pantau_eflowRecInput_DecayModeProto, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_RecoMode_PanTau",        xAOD::TauJetParameters::pantau_eflowRecInput_DecayMode, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_1p0n_vs_1p1n",  xAOD::TauJetParameters::pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_1p1n_vs_1pXn",  xAOD::TauJetParameters::pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_3p0n_vs_3pXn",  xAOD::TauJetParameters::pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn, PanTau::Tool_DetailsArranger::t_Float);
        
        //Proto 4-vector
//         ATH_MSG_WARNING("elfowRec: ProtoMomentumCore pt, eta, phi, m: " << inSeed->getProtoMomentumCore().perp() << ", " << inSeed->getProtoMomentumCore().eta() << ", " << inSeed->getProtoMomentumCore().phi() << ", " << inSeed->getProtoMomentumCore().m());
        tauJet->setP4(xAOD::TauJetParameters::PanTauEFlowRecProto, inSeed->getProtoMomentumCore().perp(), inSeed->getProtoMomentumCore().eta(), inSeed->getProtoMomentumCore().phi(), inSeed->getProtoMomentumCore().m() );
//         tauJet->setPtPanTauEFlowRecProto(inSeed->getProtoMomentumCore().perp());
//         tauJet->setPhiPanTauEFlowRecProto(inSeed->getProtoMomentumCore().phi());
//         tauJet->setEtaPanTauEFlowRecProto(inSeed->getProtoMomentumCore().eta());
//         tauJet->setMPanTauEFlowRecProto(inSeed->getProtoMomentumCore().m());
//         TLorentzVector eflowRecProto = tauJet->p4(xAOD::TauJetParameters::PanTauEFlowRecProto);
//         ATH_MSG_WARNING("eflowRec: Proto from xAOD TLV   : " << eflowRecProto.Pt() << ", " << eflowRecProto.Eta() << ", " << eflowRecProto.Phi() << ", " << eflowRecProto.M());
//         ATH_MSG_WARNING("eflowRec: Proto from xAOD single: " << tauJet->ptPanTauEFlowRecProto() << ", " << tauJet->etaPanTauEFlowRecProto() << ", " << tauJet->phiPanTauEFlowRecProto() << ", " << tauJet->mPanTauEFlowRecProto());
        
        //Final 4-vector
//         ATH_MSG_WARNING("elfowRec: ProtoMomentumCore pt, eta, phi, m: " << inSeed->getFinalMomentum().perp() << ", " << inSeed->getFinalMomentum().eta() << ", " << inSeed->getFinalMomentum().phi() << ", " << inSeed->getFinalMomentum().m());
        tauJet->setP4(xAOD::TauJetParameters::PanTauEFlowRec, inSeed->getFinalMomentum().perp(), inSeed->getFinalMomentum().eta(), inSeed->getFinalMomentum().phi(), inSeed->getFinalMomentum().m() );
//         tauJet->setPtPanTauEFlowRec(inSeed->getFinalMomentum().perp());
//         tauJet->setPhiPanTauEFlowRec(inSeed->getFinalMomentum().phi());
//         tauJet->setEtaPanTauEFlowRec(inSeed->getFinalMomentum().eta());
//         tauJet->setMPanTauEFlowRec(inSeed->getFinalMomentum().m());
        
        //BDT variables
        addPanTauDetailToTauJet(inSeed, "Basic_NPi0NeutConsts",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, "Basic_NNeutralConsts",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, "Charged_HLV_SumPt",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Charged_HLV_SumPt, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Charged_Ratio_EtOverEtAllConsts",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_HLV_SumM",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_PID_BDTValues_EtSort_1",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_PID_BDTValues_BDTSort_2",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_Ratio_EtOverEtAllConsts",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Neutral_Mean_DRToLeading_WrtEtAllConsts",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, "Combined_DeltaR1stNeutralTo1stCharged",
                                xAOD::TauJetParameters::pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged, PanTau::Tool_DetailsArranger::t_Float);
        
        
    }//end eflowRec details setting
    
    return StatusCode::SUCCESS;
}



void PanTau::Tool_DetailsArranger::addPanTauDetailToTauJet(PanTauSeed*                            inSeed,
                                                           std::string                            featName,
                                                           xAOD::TauJetParameters::PanTauDetails  detailEnum,
                                                           PanTauDetailsType                      detailType) const {
    
    ATH_MSG_DEBUG( "addPanTauDetailToTauJet called for feature of type " << detailType << ": " << featName);
    bool isValid;
    PanTau::TauFeature* features        = inSeed->getFeatures();
    std::string         fullFeatName    = inSeed->getNameInputAlgorithm() + "_" + featName;
    
    double theValue;
    theValue = features->value(fullFeatName, isValid);
    if(isValid == false) {
        
        if(m_expectInvalidFeatures == false) {
            ATH_MSG_DEBUG("WARNING --- Problems getting value for feature " << fullFeatName << " from map! This should not happen for this seed!");
            ATH_MSG_DEBUG("WARNING --- Did the ModeDiscriminator set features used in BDTs that were not found to their default values?");
            ATH_MSG_DEBUG("NOTE    --- This can also happen for seeds with (for example) 0 neutrals when trying to get Neutral_SumM - check seed");
        }
        theValue = -1111;
    }
    
    ATH_MSG_DEBUG( "the value is" << theValue);
    
    xAOD::TauJet* tauJet = const_cast<xAOD::TauJet*>(inSeed->getTauJet());
    int     valueToAddInt   = -1;
    float   valueToAddFloat = -1.1;
    ATH_MSG_DEBUG( "will add it to xAOD::TauJet at " << tauJet);
    switch(detailType) {
        case PanTau::Tool_DetailsArranger::t_Int:
            valueToAddInt   = (int)theValue;
            ATH_MSG_DEBUG( "Adding int feature: " << valueToAddInt);
            tauJet->setPanTauDetail(detailEnum, valueToAddInt);
            break;
        case PanTau::Tool_DetailsArranger::t_Float:
            valueToAddFloat = (float)theValue;
            ATH_MSG_DEBUG( "Adding float feature: " << valueToAddFloat);
            tauJet->setPanTauDetail(detailEnum, valueToAddFloat);
            break;
        default:
            ATH_MSG_WARNING("Unknown PanTauDetailType! Please use enum PanTauDetailsType from PanTauAlgs/Tool_DetailsArranger.h.");
            ATH_MSG_WARNING("!NOT! adding detail named " << featName);
            break;
    }
    
    return;
}





void PanTau::Tool_DetailsArranger::arrangeScalarDetail(PanTau::PanTauDetails* targetDetails, std::string featName, int featEnumFromPanTauDetails) const {
    
    bool isValid;
    PanTau::PanTauSeed* sourceSeed      = targetDetails->getPanTauSeed();
    PanTau::TauFeature* features        = targetDetails->getFeatures();
    std::string         fullFeatName    = sourceSeed->getNameInputAlgorithm() + "_" + featName;
    
    double theValue;
    theValue = features->value(fullFeatName, isValid);
    if(isValid == false) {
        
        if(m_expectInvalidFeatures == false) {
            ATH_MSG_DEBUG("WARNING --- Problems getting value for feature " << fullFeatName << " from map! This should not happen for this seed!");
            ATH_MSG_DEBUG("WARNING --- Did the ModeDiscriminator set features used in BDTs that were not found to their default values?");
            ATH_MSG_DEBUG("NOTE    --- This can also happen for seeds with (for example) 0 neutrals when trying to get Neutral_SumM - check seed");
        }
        theValue = -1111;
    }

    
    targetDetails->setScalarDetailByEnum(theValue, featEnumFromPanTauDetails);
    
    return;
}



void PanTau::Tool_DetailsArranger::arrangeVectorDetail(PanTau::PanTauDetails* targetDetails, std::string featName, int featEnumFromPanTauDetails) const {
    
    PanTau::PanTauSeed* sourceSeed      = targetDetails->getPanTauSeed();
    PanTau::TauFeature* features        = targetDetails->getFeatures();
    std::string         fullFeatName    = sourceSeed->getNameInputAlgorithm() + "_" + featName;
    
    std::vector<double> theValue;
    if(sourceSeed->getIsValidSeed() == true) {
        theValue = features->vecValue(fullFeatName);
    } else {//seed was not valid to begin with
        theValue = std::vector<double>(0);
    }
    
    #ifndef NDEBUG
    ATH_MSG_DEBUG("Tool_DetailsArranger: vector detail " << fullFeatName << " set to have entries: " << theValue.size());
    for(unsigned int iVal=0; iVal<theValue.size(); iVal++) {
        ATH_MSG_DEBUG("\t\t" << iVal+1 << ": " << theValue[iVal]);
    }
    #endif // NDEBUG
    
    
    targetDetails->setVectorDetailByEnum(theValue, featEnumFromPanTauDetails);
    
    return;
}


/*
void PanTau::Tool_DetailsArranger::StripPi0ConstsFromNeutralConsts(std::vector< ElementLink< xAOD::PFOContainer > > &neutralPFOLinks, std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks){

  unsigned int neut_orig_size = neutralPFOLinks.size();

  for(unsigned int iconsts_Pi0Neut=0; iconsts_Pi0Neut < pi0PFOLinks.size(); ++iconsts_Pi0Neut){

    int neut_index=-1;
    for(unsigned int iconsts_Neutral=0; iconsts_Neutral < neutralPFOLinks.size(); ++iconsts_Neutral){

      if( neutralPFOLinks.at(iconsts_Neutral).cachedElement() == pi0PFOLinks.at(iconsts_Pi0Neut).cachedElement() ) { neut_index = iconsts_Neutral; }

    }

    if(neut_index > -1) neutralPFOLinks.erase( neutralPFOLinks.begin() + neut_index);
    else {
      ATH_MSG_WARNING("StripPi0ConstsFromNeutralConsts: No neutral found for pi0neutral with index " << iconsts_Pi0Neut);
    }

  }

  // did we find a neutral PFO to each pi0 PFO?
  if(neutralPFOLinks.size() != (neut_orig_size - pi0PFOLinks.size()) ){
    ATH_MSG_WARNING("StripPi0ConstsFromNeutralConsts: neutralPFOLinks.size() != (neut_orig_size - pi0PFOLinks.size()) -- " << neutralPFOLinks.size() << " != (" << neut_orig_size << " - " << pi0PFOLinks.size() << ")" );
  }

  return;

}
*/



void PanTau::Tool_DetailsArranger::arrangePFOLinks(PanTau::PanTauSeed* inSeed, xAOD::TauJet* tauJet) {
    
    //get the PFO links
    std::vector< ElementLink< xAOD::PFOContainer > > chrgPFOLinks       = tauJet->protoChargedPFOLinks();
    //std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks        = tauJet->protoPi0PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > neutralPFOLinks    = tauJet->protoNeutralPFOLinks();

    std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks        = CollectConstituentsAsPFOLinks( inSeed, tauJet->protoNeutralPFOLinks(), PanTau::TauConstituent::t_Pi0Neut );
    std::vector< ElementLink< xAOD::PFOContainer > > preSelected_neutralPFOLinks    = CollectConstituentsAsPFOLinks( inSeed, tauJet->protoNeutralPFOLinks(), PanTau::TauConstituent::t_Neutral );

    /*
    ATH_MSG_DEBUG("Dumping neutral pfo links from constituents");
    for(unsigned int iPFO=0; iPFO<neutralPFOLinks_tmp.size(); iPFO++) {
        const xAOD::PFO* pfo = neutralPFOLinks_tmp.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }
    ATH_MSG_DEBUG("Dumping pi0 pfo links from constituents");
    for(unsigned int iPFO=0; iPFO<pi0PFOLinks_tmp.size(); iPFO++) {
        const xAOD::PFO* pfo = pi0PFOLinks_tmp.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }
    */

    //clear the default links, just to be save
    tauJet->clearChargedPFOLinks();
    tauJet->clearNeutralPFOLinks();
    tauJet->clearPi0PFOLinks();
    
    //std::vector< ElementLink< xAOD::PFOContainer > > preLinkChrgPFOLinks       = tauJet->chargedPFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > preLinkPi0PFOLinks        = tauJet->pi0PFOLinks();
    //std::vector< ElementLink< xAOD::PFOContainer > > preLinkNeutralPFOLinks    = tauJet->neutralPFOLinks();

    // sort PFO links according to pi0 BDT score:
    std::sort (pi0PFOLinks.begin(), pi0PFOLinks.end(), sortBDTscore);
    std::sort (preSelected_neutralPFOLinks.begin(), preSelected_neutralPFOLinks.end(), sortBDTscore);
    std::sort (neutralPFOLinks.begin(), neutralPFOLinks.end(), sortBDTscore);

    // set the masses of all neutrals *and pi0 neutrals* to 0:
    SetNeutralConstituentVectorMasses(neutralPFOLinks, 0.);
    
    // arrange charged & neutral PFOs: they are not changed -> copy from cellbased
    tauJet->setChargedPFOLinks(chrgPFOLinks);
    tauJet->setNeutralPFOLinks(neutralPFOLinks);
    
    tauJet->setDetail(xAOD::TauJetParameters::nCharged, (int)chrgPFOLinks.size());


    ATH_MSG_DEBUG("Dumping preselected neutral pfo links");
    for(unsigned int iPFO=0; iPFO<preSelected_neutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = preSelected_neutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }
    /*
    std::vector< ElementLink< xAOD::PFOContainer > > preSelected_neutralPFOLinks=PreselectNeutralLinks(neutralPFOLinks, tauJet);
    ATH_MSG_DEBUG("Dumping preselected neutral pfo links");
    for(unsigned int iPFO=0; iPFO<preSelected_neutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = preSelected_neutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }


    StripPi0ConstsFromNeutralConsts(preSelected_neutralPFOLinks, pi0PFOLinks);
    ATH_MSG_DEBUG("Dumping preselected neutral pfo links after stripping pi0 neutrals");
    for(unsigned int iPFO=0; iPFO<preSelected_neutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = preSelected_neutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }
    */
    
    //arrange pi0 pfos: depends on decay mode classification
    int decayModeProto = inSeed->getDecayModeBySubAlg();
    int decayModeFinal = inSeed->getDecayModeByPanTau();
    
    if(decayModeFinal == PanTau::PanTauSeed::Mode_Other) {
        tauJet->setPi0PFOLinks(pi0PFOLinks);
        return;
    }
    
    ATH_MSG_DEBUG("Before re-linking: DecayMode Proto / Final: " << decayModeProto << " / " << decayModeFinal);
    ATH_MSG_DEBUG("Number of chrg, pi0, neut PFOs in subAlg: " << chrgPFOLinks.size() << ", " << pi0PFOLinks.size() << ", " << neutralPFOLinks.size());
    ATH_MSG_DEBUG("Number of pi0 PFOs in PanTau: " << preLinkPi0PFOLinks.size());
    
    //#ifndef NDEBUG
    ATH_MSG_DEBUG("Dumping pi0 pfos for subalg");
    for(unsigned int iPFO=0; iPFO<pi0PFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = pi0PFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score());
    }
    ATH_MSG_DEBUG("Dumping neutral pfos for subalg");
    for(unsigned int iPFO=0; iPFO<neutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = neutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }
    //#endif //NDEBUG
    
    
    //set nPi0 to 0 for neutrals that have no pi0
//     for(unsigned int iPFO=0; iPFO<neutralPFOLinks.size(); iPFO++) {
//         xAOD::PFO*  curPFO = const_cast<xAOD::PFO*>(neutralPFOLinks.at(iPFO).cachedElement());
//         int nPi0 = -1;
//         bool getOK = curPFO->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
//         if(getOK == false) {
//             ATH_MSG_DEBUG("Could not get nPi0Proto for this PFO -> skip setting of nPi0");
//             continue;
//         }
//         if(nPi0 < 1) curPFO->setAttribute(xAOD::PFODetails::nPi0Proto, 0);
//     }
    

    std::string inputAlg = inSeed->getNameInputAlgorithm();

    //if pantau sets the same decay mode as the substructure algorithm, just copy the links
    if(decayModeProto == decayModeFinal) {
      ATH_MSG_DEBUG("Modes are the same");
      
      if( decayModeFinal == PanTau::PanTauSeed::Mode_3pXn && pi0PFOLinks.size() > 1 ){

	// assign all neutrals but only one pi0 neutral to Pantau:
	preLinkPi0PFOLinks.push_back( pi0PFOLinks.at(0) );
	
	// set all masses correctly:
	SetNeutralConstituentVectorMasses(preLinkPi0PFOLinks, MASS_PI0);

      } else {

	// assign the same constituents to Pantau:
	preLinkPi0PFOLinks=pi0PFOLinks;
	
	// set all masses correctly:
	SetNeutralConstituentVectorMasses(preLinkPi0PFOLinks, MASS_PI0);

      }


    } else {

      // *****
      // if(preSelected_neutralPFOLinks.size() > 0) are not necessary! 
      // *****

      if( decayModeFinal == PanTau::PanTauSeed::Mode_1p1n && decayModeProto == PanTau::PanTauSeed::Mode_1p0n ){

	// add the highest BDT-score neutral from the sub-alg:
	if(preSelected_neutralPFOLinks.size() > 0) preLinkPi0PFOLinks.push_back( preSelected_neutralPFOLinks.at(0) );
	else ATH_MSG_WARNING("No neutral PFO Links although there should be!!");


	// set the mass:
	SetNeutralConstituentVectorMasses(preLinkPi0PFOLinks, MASS_PI0);

      } else if( decayModeFinal == PanTau::PanTauSeed::Mode_1p0n && decayModeProto == PanTau::PanTauSeed::Mode_1p1n ){

	// do nothing (leave the pi0 neutrals list empty)

      } else if( decayModeFinal == PanTau::PanTauSeed::Mode_1pXn && decayModeProto == PanTau::PanTauSeed::Mode_1p1n ){


	if( pi0PFOLinks.size() == 1 && HasMultPi0sInOneCluster(pi0PFOLinks.at(0).cachedElement(), decayModeProto, inputAlg) ){ 
	  //   ATH_MSG_WARNING("RecalculatePantauConstituents: Inconsistent decay mode classification! (wasAlteredByCellBasedShots=true although decay mode = " << decayModeFinal);
	  //   tauJet->setPi0PFOLinks(pi0PFOLinks);
	  //   return;
	  // }
	  
	  // assign twice the pi0 mass to the one pi0 PFO:
	  SetNeutralConstituentVectorMasses(pi0PFOLinks, 2*MASS_PI0);
	  
	  // assign the same constituents to Pantau:
	  preLinkPi0PFOLinks=pi0PFOLinks;
	  
	} else {
	  
	  // copy all (really only one) pi0s from the sub-alg and add
	  // the highest BDT-score neutral:
	  preLinkPi0PFOLinks=pi0PFOLinks;
	  if(preSelected_neutralPFOLinks.size() > 0) preLinkPi0PFOLinks.push_back( preSelected_neutralPFOLinks.at(0) );
	  else ATH_MSG_WARNING("No neutral PFO Links although there should be!!");
	  
	  // set the mass:
	  SetNeutralConstituentVectorMasses(preLinkPi0PFOLinks, MASS_PI0);

	}

      } else if( decayModeFinal == PanTau::PanTauSeed::Mode_1p1n && decayModeProto == PanTau::PanTauSeed::Mode_1pXn ){

	// copy all (two) pi0s from the sub-alg:
	preLinkPi0PFOLinks.push_back( pi0PFOLinks.at(0) );
	preLinkPi0PFOLinks.push_back( pi0PFOLinks.at(1) );

	// set both pi0neutrals to mass 0 (photon mass):
	SetNeutralConstituentVectorMasses(preLinkPi0PFOLinks, 0.);

      } else if( decayModeFinal == PanTau::PanTauSeed::Mode_3pXn && decayModeProto == PanTau::PanTauSeed::Mode_3p0n ){

	// add the highest BDT-score neutral from the sub-alg:
	if(preSelected_neutralPFOLinks.size() > 0) preLinkPi0PFOLinks.push_back( preSelected_neutralPFOLinks.at(0) );
	else ATH_MSG_WARNING("No neutral PFO Links although there should be!!");
	
	// set the mass:
	SetNeutralConstituentVectorMasses(preLinkPi0PFOLinks, MASS_PI0);

      } else if( decayModeFinal == PanTau::PanTauSeed::Mode_3p0n && decayModeProto == PanTau::PanTauSeed::Mode_3pXn ){

	// do nothing (leave the pi0 neutrals list empty)
	
      }

    }

    tauJet->setPi0PFOLinks(preLinkPi0PFOLinks);


    
    ATH_MSG_DEBUG("Done setting links");
    ATH_MSG_DEBUG("DecayMode Proto / Final: " << decayModeProto << " / " << decayModeFinal);
    ATH_MSG_DEBUG("Number of chrg, pi0, neut PFOs in subAlg: " << chrgPFOLinks.size() << ", " << pi0PFOLinks.size() << ", " << neutralPFOLinks.size());
    
    std::vector< ElementLink< xAOD::PFOContainer > > finalChrgPFOLinks       = tauJet->chargedPFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > finalPi0PFOLinks        = tauJet->pi0PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > finalNeutralPFOLinks    = tauJet->neutralPFOLinks();
    ATH_MSG_DEBUG("Number of chrg, pi0, neut PFOs in PanTau: " << finalChrgPFOLinks.size() << ", " << finalPi0PFOLinks.size() << ", " << finalNeutralPFOLinks.size());
    
    
    
    //! DEBUG output
    //#ifndef NDEBUG
    ATH_MSG_DEBUG("Dumping charged pfos for subalg");
    for(unsigned int iPFO=0; iPFO<chrgPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = chrgPFOLinks.at(iPFO).cachedElement();
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m());
    }
    ATH_MSG_DEBUG("Dumping charged pfos for pantau");
    for(unsigned int iPFO=0; iPFO<finalChrgPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = finalChrgPFOLinks.at(iPFO).cachedElement();
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m());
    }
    
    ATH_MSG_DEBUG("Dumping pi0 pfos for subalg");
    for(unsigned int iPFO=0; iPFO<pi0PFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = pi0PFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 <<  " , BDTscore: " << pfo->bdtPi0Score() );
    }
    ATH_MSG_DEBUG("Dumping pi0 pfos for pantau");
    for(unsigned int iPFO=0; iPFO<finalPi0PFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = finalPi0PFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 <<  " , BDTscore: " << pfo->bdtPi0Score() );
    }
    
    ATH_MSG_DEBUG("Dumping neutral pfos for subalg");
    for(unsigned int iPFO=0; iPFO<neutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = neutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 <<  " , BDTscore: " << pfo->bdtPi0Score() );
    }
    ATH_MSG_DEBUG("Dumping neutral pfos for pantau");
    for(unsigned int iPFO=0; iPFO<finalNeutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = finalNeutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 <<  " , BDTscore: " << pfo->bdtPi0Score() );
    }
    //#endif //NDEBUG
    
    
    
    return;
}



/*
std::vector< ElementLink< xAOD::PFOContainer > > PanTau::Tool_DetailsArranger::PreselectNeutralLinks(std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, xAOD::TauJet* tauJet) {

    ATH_MSG_DEBUG("\tPreselectNeutralLinks called");
    // the neutral links are already ordered by BDT score

    std::vector< ElementLink< xAOD::PFOContainer > > preSelected_neutralPFOLinks;

    // unsigned int    index1stBDTScoreNeutral = 9999;
    // unsigned int    index2ndBDTScoreNeutral = 9999;
    // float           BDTScore1st             = -1111.;
    // float           BDTScore2nd             = -1111.;

    for(unsigned int iNeutral=0; iNeutral<neutralPFOLinks.size(); iNeutral++) {

        const xAOD::PFO*    curPi0PFO   = neutralPFOLinks.at(iNeutral).cachedElement();

	ATH_MSG_DEBUG("\tPreselectNeutralLinks: PFO Pt / Eta / Phi / m: " << curPi0PFO->pt() << " / " << curPi0PFO->eta() << " / " << curPi0PFO->phi() << " / " << curPi0PFO->m());

	int nPi0sPerCluster = 0;
	if( !curPi0PFO->attribute(xAOD::PFODetails::nPi0Proto, nPi0sPerCluster) ) {
	  ATH_MSG_WARNING("Could not retrieve nPi0Proto. Will set it to 0.");
	  nPi0sPerCluster = 0;
	}
        
        //check whether this neutral is in the core region
	ATH_MSG_DEBUG("\tPreselectNeutralLinks: tauJet->p4(xAOD::TauJetParameters::IntermediateAxis).DeltaR(curPi0PFO->p4()) (" << tauJet->p4(xAOD::TauJetParameters::IntermediateAxis).DeltaR(curPi0PFO->p4()) << ") > m_CoreCone (" << m_CoreCone << ")!");
        if(tauJet->p4(xAOD::TauJetParameters::IntermediateAxis).DeltaR(curPi0PFO->p4()) > m_CoreCone) {
	  //continue;
	  if(nPi0sPerCluster<1) continue; // hack (as long as dR cut doesn't work) to make sure pi0s pass
	  // TODO: Replace by shrinking cone?
        }

	ATH_MSG_DEBUG("\tPreselectNeutralLinks: Passed DR cut!");
        
        //check for et cut
        double etCut = 99999999;
        for(unsigned int iEtaBin=0; iEtaBin<m_EtaBinEdges.size()-1; iEtaBin++) {
            double curEta = fabs(curPi0PFO->p4().Eta());
            double minEta = m_EtaBinEdges.at(iEtaBin);
            double maxEta = m_EtaBinEdges.at(iEtaBin+1);
            if(curEta >= minEta && curEta < maxEta) {
                etCut = m_EtaBinnedEtCuts.at(iEtaBin);
                break;
            }
        }

	ATH_MSG_DEBUG("\tPreselectNeutralLinks: curPi0PFO->p4().Et() (" << curPi0PFO->p4().Et() << ") < etCut (" << etCut << ")?");

        if(curPi0PFO->p4().Et() < etCut) {
	  //continue;
	  if(nPi0sPerCluster<1) continue; // hack (as long as dR cut doesn't work) to make sure pi0s pass
        }
        
	ATH_MSG_DEBUG("\tPreselectNeutralLinks: Passed!");

	preSelected_neutralPFOLinks.push_back( neutralPFOLinks.at(iNeutral) );

        
        // float               curBDTValue = curPi0PFO->bdtPi0Score();
        // ATH_MSG_DEBUG("\t\tneutral " << iNeutral << ", BDT score = " << curBDTValue);
        
        
        // if(curBDTValue > BDTScore1st) {
        //     BDTScore2nd = BDTScore1st;
        //     index2ndBDTScoreNeutral = index1stBDTScoreNeutral;
        //     BDTScore1st = curBDTValue;
        //     index1stBDTScoreNeutral = iNeutral;
        // } else if(curBDTValue > BDTScore2nd) {
        //     BDTScore2nd = curBDTValue;
        //     index2ndBDTScoreNeutral = iNeutral;
        // }
        // ATH_MSG_DEBUG("\t\t  1st highestBDT score at " << index1stBDTScoreNeutral);
        // ATH_MSG_DEBUG("\t\t  2nd highestBDT score at " << index2ndBDTScoreNeutral);


    }//end loop over neutrals
    
    // std::vector<unsigned int> indexList;
    // if(nPi0ToSelects >= 1) indexList.push_back(index1stBDTScoreNeutral);
    // if(nPi0ToSelects >= 2) indexList.push_back(index2ndBDTScoreNeutral);
    
    // ATH_MSG_DEBUG("\t1st highestBDT score at " << index1stBDTScoreNeutral);
    // ATH_MSG_DEBUG("\t2nd highestBDT score at " << index2ndBDTScoreNeutral);

    // return indexList;

    return preSelected_neutralPFOLinks;

}
*/



bool PanTau::Tool_DetailsArranger::HasMultPi0sInOneCluster(const xAOD::PFO* pfo, int decayModeProto, TString inputAlg){

  // this is only relevant for reco 1p1n modes, hence restrict the
  // output to these modes

  int nPi0sPerCluster = 1;

  if (inputAlg != "CellBased" ) return (nPi0sPerCluster > 1);

  // cell-based sets this to 1pXn however below this function is
  // called with the decayModeProto as evaluated by Pantau!
  if (decayModeProto != PanTau::PanTauSeed::Mode_1p1n ) return (nPi0sPerCluster > 1);
                
  if( !pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0sPerCluster) ) {
    ATH_MSG_WARNING("Could not retrieve nPi0Proto. Will set it to 1.");
    nPi0sPerCluster = 1;
  }

  return (nPi0sPerCluster > 1);

}



void PanTau::Tool_DetailsArranger::SetNeutralConstituentMass(xAOD::PFO* neutral_pfo, double mass){

  P4EEtaPhiM momentum = P4EEtaPhiM(neutral_pfo->e(), neutral_pfo->eta(), neutral_pfo->phi(), mass);
  neutral_pfo->setP4(momentum.pt(), neutral_pfo->eta(), neutral_pfo->phi(), mass);

  // double new_P=sqrt( neutral_pfo->p4().E()*neutral_pfo->p4().E() 
  // 		       + neutral_pfo->p4().M()*neutral_pfo->p4().M() 
  // 		       - mass*mass );
  // TLorentzVector neutral_hlv;
  // neutral_hlv.SetVectM( new_P*neutral_pfo->p4().Vect().Unit(), mass );
    
  // neutral_pfo->setP4( neutral_hlv );

  return;
    
}


void PanTau::Tool_DetailsArranger::SetNeutralConstituentVectorMasses(std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, double mass){
    
  for(unsigned int iNeutral=0; iNeutral<neutralPFOLinks.size(); iNeutral++) {
    ElementLink<xAOD::PFOContainer> curNeutralPFOLink   = neutralPFOLinks.at(iNeutral);
    xAOD::PFO*                      curNeutralPFO       = const_cast<xAOD::PFO*>(curNeutralPFOLink.cachedElement());
      
    SetNeutralConstituentMass(curNeutralPFO, mass);
      
  }
    
  return;
    
}



std::vector< ElementLink< xAOD::PFOContainer > > PanTau::Tool_DetailsArranger::CollectConstituentsAsPFOLinks( PanTau::PanTauSeed* inSeed, std::vector< ElementLink< xAOD::PFOContainer > > cellbased_neutralPFOLinks, PanTau::TauConstituent::Type type ){
  // collect element links from tau constituents in the Pantau
  // seed of type "type". cellbased_neutralPFOLinks is only used
  // to obtain the ElementLinks.

  std::vector< ElementLink< xAOD::PFOContainer > > new_links;

  unsigned int nConstsOfType=0;
  bool foundIt=false;
  std::vector<PanTau::TauConstituent*> tauConstituents=inSeed->getConstituentsOfType(type,foundIt);

  if( (type != PanTau::TauConstituent::t_Neutral && type != PanTau::TauConstituent::t_Pi0Neut) || !foundIt){
    ATH_MSG_WARNING("CollectConstituentsAsPFOLinks: Function was called with type = " << type << " , however it was only designed for types t_Pi0Neut and t_Neutral! Returning...");
    return new_links;
  }

  for(unsigned int iConst=0; iConst<tauConstituents.size(); iConst++) {
    //bool isCoreNeut = tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_Neutral);
    //bool isCorePi0  = tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_Pi0Neut);
    bool isOfType = tauConstituents[iConst]->isOfType(type);
    if(!isOfType) continue;

    // if the requested type is t_Neutral then exclude any t_Pi0Neut
    // from the list (note: tau constituents that are t_Pi0Neut are
    // also t_Neutral at the same time):
    if(type==PanTau::TauConstituent::t_Neutral && tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_Pi0Neut) ) continue;
    ++nConstsOfType;

    for(unsigned int iPFO=0; iPFO<cellbased_neutralPFOLinks.size(); iPFO++) {
      const xAOD::PFO* pfo = cellbased_neutralPFOLinks.at(iPFO).cachedElement();

      if( tauConstituents[iConst]->getPFO() != pfo ) continue;

      new_links.push_back( cellbased_neutralPFOLinks.at(iPFO) );

    }

  }


  if( nConstsOfType != new_links.size() ){
    ATH_MSG_WARNING("CollectConstituentsAsPFOLinks: Couldn't find PFOLinks " << new_links.size() << " for all tau constituents (" << tauConstituents.size() << ")!");

    ATH_MSG_DEBUG("Dumping neutral pfo links from all constituents of type " << type);
    for(unsigned int iConst=0; iConst<tauConstituents.size(); iConst++) {
      const xAOD::PFO* pfo = tauConstituents[iConst]->getPFO();
      int nPi0 = -1;
      bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
      if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
      ATH_MSG_DEBUG("pfo " << iConst << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }

    ATH_MSG_DEBUG("Dumping all cellbased neutral pfo links");
    for(unsigned int iPFO=0; iPFO<cellbased_neutralPFOLinks.size(); iPFO++) {
      const xAOD::PFO* pfo = cellbased_neutralPFOLinks.at(iPFO).cachedElement();
      int nPi0 = -1;
      bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
      if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
      ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0 << " , BDTscore: " << pfo->bdtPi0Score() );
    }
  }

  return new_links;

}
