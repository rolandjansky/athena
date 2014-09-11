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
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_RecoMode",               xAOD::TauJetParameters::pantau_CellBasedInput_DecayModeProto, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_RecoMode_PanTau",        xAOD::TauJetParameters::pantau_CellBasedInput_DecayMode, PanTau::Tool_DetailsArranger::t_Int);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_1p0n_vs_1p1n",  xAOD::TauJetParameters::pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_1p1n_vs_1pXn",  xAOD::TauJetParameters::pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn, PanTau::Tool_DetailsArranger::t_Float);
        addPanTauDetailToTauJet(inSeed, m_varTypeName_Basic + "_BDTValue_3p0n_vs_3pXn",  xAOD::TauJetParameters::pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn, PanTau::Tool_DetailsArranger::t_Float);
        
        
        
        //Proto 4-vector
        tauJet->setP4(xAOD::TauJetParameters::PanTauCellBasedProto, inSeed->getProtoMomentumCore().perp(), inSeed->getProtoMomentumCore().eta(), inSeed->getProtoMomentumCore().phi(), inSeed->getProtoMomentumCore().m());
//         tauJet->setPtPanTauCellBasedProto(inSeed->getProtoMomentumCore().perp());
//         tauJet->setPhiPanTauCellBasedProto(inSeed->getProtoMomentumCore().phi());
//         tauJet->setEtaPanTauCellBasedProto(inSeed->getProtoMomentumCore().eta());
//         tauJet->setMPanTauCellBasedProto(inSeed->getProtoMomentumCore().m());
        
        //Final 4-vector
        tauJet->setP4(xAOD::TauJetParameters::PanTauCellBased, inSeed->getFinalMomentum().perp(), inSeed->getFinalMomentum().eta(), inSeed->getFinalMomentum().phi(), inSeed->getFinalMomentum().m());
//         tauJet->setPtPanTauCellBased(inSeed->getFinalMomentum().perp());
//         tauJet->setPhiPanTauCellBased(inSeed->getFinalMomentum().phi());
//         tauJet->setEtaPanTauCellBased(inSeed->getFinalMomentum().eta());
//         tauJet->setMPanTauCellBased(inSeed->getFinalMomentum().m());
        
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






void PanTau::Tool_DetailsArranger::arrangePFOLinks(PanTau::PanTauSeed* inSeed, xAOD::TauJet* tauJet) {
    
    //get the PFO links
    std::vector< ElementLink< xAOD::PFOContainer > > chrgPFOLinks       = tauJet->cellBased_Charged_PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks        = tauJet->cellBased_Pi0_PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > neutralPFOLinks    = tauJet->cellBased_Neutral_PFOLinks();
    
    //clear the default links, just to be save
    tauJet->clearCharged_PFOLinks();
    tauJet->clearNeutral_PFOLinks();
    tauJet->clearPi0_PFOLinks();
    
    std::vector< ElementLink< xAOD::PFOContainer > > preLinkChrgPFOLinks       = tauJet->charged_PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > preLinkPi0PFOLinks        = tauJet->pi0_PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > preLinkNeutralPFOLinks    = tauJet->neutral_PFOLinks();
    
    // arrange charged & neutral PFOs: they are not changed -> copy from cellbased
    tauJet->setCharged_PFOLinks(chrgPFOLinks);
    tauJet->setNeutral_PFOLinks(neutralPFOLinks);
    
    tauJet->setDetail(xAOD::TauJetParameters::nCharged, (int)chrgPFOLinks.size());
    
    //arrange pi0 pfos: depends on decay mode classification
    int decayModeProto = inSeed->getDecayModeBySubAlg();
    int decayModeFinal = inSeed->getDecayModeByPanTau();
    
    
    
    ATH_MSG_DEBUG("Before re-linking: DecayMode Proto / Final: " << decayModeProto << " / " << decayModeFinal);
    ATH_MSG_DEBUG("Number of chrg, pi0, neut PFOs in subAlg: " << chrgPFOLinks.size() << ", " << pi0PFOLinks.size() << ", " << neutralPFOLinks.size());
    ATH_MSG_DEBUG("Number of chrg, pi0, neut PFOs in PanTau: " << preLinkChrgPFOLinks.size() << ", " << preLinkPi0PFOLinks.size() << ", " << preLinkNeutralPFOLinks.size());
    
    #ifndef NDEBUG
    ATH_MSG_DEBUG("Dumping pi0 pfos for subalg");
    for(unsigned int iPFO=0; iPFO<pi0PFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = pi0PFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0);
    }
    ATH_MSG_DEBUG("Dumping neutral pfos for subalg");
    for(unsigned int iPFO=0; iPFO<neutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = neutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0Proto attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0);
    }
    #endif //NDEBUG
    
    
    //set nPi0 to 0 for neutrals that have no pi0
    for(unsigned int iPFO=0; iPFO<neutralPFOLinks.size(); iPFO++) {
        xAOD::PFO*  curPFO = const_cast<xAOD::PFO*>(neutralPFOLinks.at(iPFO).cachedElement());
        int nPi0 = -1;
        bool getOK = curPFO->attribute(xAOD::PFODetails::nPi0Proto, nPi0);
        if(getOK == false) {
            ATH_MSG_DEBUG("Could not get nPi0Proto for this PFO -> skip setting of nPi0");
            continue;
        }
        if(nPi0 < 1) curPFO->setAttribute(xAOD::PFODetails::nPi0, 0);
    }
    
    int nPi0ToSet = 0;
    
    //if pantau sets the same decay mode as the substructure algorithm, just copy the links
    if(decayModeProto == decayModeFinal) {
        ATH_MSG_DEBUG("Modes are the same");
        // exception 3pXn: in this case use up to two pi0 only
        if(decayModeFinal == PanTau::PanTauSeed::t_Mode_3pXn) {
            //copy the neutrals
            int nPi0ToSelect = pi0PFOLinks.size();
            if(nPi0ToSelect > 2) nPi0ToSelect = 2;
            ATH_MSG_DEBUG("Modes is 3pXn -> copy pi0: " << nPi0ToSelect);
            nPi0ToSet = helper_CopyNeutralsAndSetPi0(tauJet, nPi0ToSelect, false);
        }//end if 3pXn
        
        //just copy the pi0s
        if(decayModeFinal != PanTau::PanTauSeed::t_Mode_3pXn) {
            
            int nMaxPi0 = pi0PFOLinks.size();
            if(nMaxPi0 == 0) {
                if(decayModeFinal == PanTau::PanTauSeed::t_Mode_1p1n) { nMaxPi0 = 1; }
                if(decayModeFinal == PanTau::PanTauSeed::t_Mode_1pXn) { nMaxPi0 = 2; }
                if(decayModeFinal == PanTau::PanTauSeed::t_Mode_3pXn) { nMaxPi0 = 1; }
            }
            if(nMaxPi0 > 0) {
                if(decayModeFinal == PanTau::PanTauSeed::t_Mode_1p0n) { nMaxPi0 = 0; }
                if(decayModeFinal == PanTau::PanTauSeed::t_Mode_3p0n) { nMaxPi0 = 0; }
            }
            ATH_MSG_DEBUG("Modes is not 3pXn -> select pi0: " << nMaxPi0);
            nPi0ToSet = helper_CopyNeutralsAndSetPi0(tauJet, nMaxPi0, false);
        }
        
    }//end proto = final mode
    
    
    if(decayModeProto != decayModeFinal) {
        ATH_MSG_DEBUG("Modes are not the same");
        int nMaxPi0 = -1;
        
        bool isSpecialCase_1pXnTo1p1n = false;
             if(decayModeProto == PanTau::PanTauSeed::t_Mode_1p0n && decayModeFinal == PanTau::PanTauSeed::t_Mode_1p1n) { nMaxPi0 = 1; }
        else if(decayModeProto == PanTau::PanTauSeed::t_Mode_1p1n && decayModeFinal == PanTau::PanTauSeed::t_Mode_1p0n) { nMaxPi0 = 0; }
        else if(decayModeProto == PanTau::PanTauSeed::t_Mode_1p1n && decayModeFinal == PanTau::PanTauSeed::t_Mode_1pXn) { nMaxPi0 = 2; }
        else if(decayModeProto == PanTau::PanTauSeed::t_Mode_1pXn && decayModeFinal == PanTau::PanTauSeed::t_Mode_1p1n) { 
            nMaxPi0 = 1;
            isSpecialCase_1pXnTo1p1n = true;
            //NOTE: See note below
        } else if(decayModeProto == PanTau::PanTauSeed::t_Mode_3p0n && decayModeFinal == PanTau::PanTauSeed::t_Mode_3pXn) { nMaxPi0 = 1; }
        else if(decayModeProto == PanTau::PanTauSeed::t_Mode_3pXn && decayModeFinal == PanTau::PanTauSeed::t_Mode_3p0n) { nMaxPi0 = 0; }
        else {
            ATH_MSG_DEBUG("Unknown decayMode combination of substructure and pantau mode: " << decayModeProto << " (from Substructure), " << decayModeFinal << " (from PanTau)");
            ATH_MSG_DEBUG("No default PFOLinks are set in the tau!!!");
        }
        
        ATH_MSG_DEBUG("Pi0 to be copied based on modes: " << nMaxPi0);
        nPi0ToSet = helper_CopyNeutralsAndSetPi0(tauJet, nMaxPi0, isSpecialCase_1pXnTo1p1n);
        
        //!Note concernint 1pXn (Proto) -> 1p1n (final)
        // FIXME: atm just flag the highest BDTScore neutral as pi0, to get numbers right
        // NOTE:  What's needed to be done is to add a new PFO to the tau, which is built from the two pi0 in proto mode
        
    }//end if proto mode != final mode
    
    tauJet->setDetail(xAOD::TauJetParameters::nPi0, nPi0ToSet);
    
    ATH_MSG_DEBUG("Done setting links");
    ATH_MSG_DEBUG("DecayMode Proto / Final: " << decayModeProto << " / " << decayModeFinal);
    ATH_MSG_DEBUG("Number of chrg, pi0, neut PFOs in subAlg: " << chrgPFOLinks.size() << ", " << pi0PFOLinks.size() << ", " << neutralPFOLinks.size());
    
    std::vector< ElementLink< xAOD::PFOContainer > > finalChrgPFOLinks       = tauJet->charged_PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > finalPi0PFOLinks        = tauJet->pi0_PFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > > finalNeutralPFOLinks    = tauJet->neutral_PFOLinks();
    ATH_MSG_DEBUG("Number of chrg, pi0, neut PFOs in PanTau: " << finalChrgPFOLinks.size() << ", " << finalPi0PFOLinks.size() << ", " << finalNeutralPFOLinks.size());
    
    
    
    //! DEBUG output
    #ifndef NDEBUG
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
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0);
    }
    ATH_MSG_DEBUG("Dumping pi0 pfos for pantau");
    for(unsigned int iPFO=0; iPFO<finalPi0PFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = finalPi0PFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0);
    }
    
    ATH_MSG_DEBUG("Dumping neutral pfos for subalg");
    for(unsigned int iPFO=0; iPFO<neutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = neutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0);
    }
    ATH_MSG_DEBUG("Dumping neutral pfos for pantau");
    for(unsigned int iPFO=0; iPFO<finalNeutralPFOLinks.size(); iPFO++) {
        const xAOD::PFO* pfo = finalNeutralPFOLinks.at(iPFO).cachedElement();
        int nPi0 = -1;
        bool getOK = pfo->attribute(xAOD::PFODetails::nPi0, nPi0);
        if(getOK == false) ATH_MSG_DEBUG("problems reading pi0 attribute");
        ATH_MSG_DEBUG("pfo " << iPFO << " pt, eta, phi, m: " << pfo->pt() << ", " << pfo->eta() << ", " << pfo->phi() << ", " << pfo->m() << "  isPi0: " << nPi0);
    }
    #endif //NDEBUG
    
    
    
    return;
}






std::vector<unsigned> PanTau::Tool_DetailsArranger::helper_IndicesOfNeutralsToBePi0( std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks, int nMaxPi0s) {
    ATH_MSG_DEBUG("\tIndicesOfNeutralsToBePi0 called for nMaxPi0 = " << nMaxPi0s);
    unsigned int    index1stBDTScoreNeutral = 9999;
    unsigned int    index2ndBDTScoreNeutral = 9999;
    float           BDTScore1st             = -1111.;
    float           BDTScore2nd             = -1111.;
    for(unsigned int iNeutral=0; iNeutral<neutralPFOLinks.size(); iNeutral++) {
        const xAOD::PFO*    curPi0PFO   = neutralPFOLinks.at(iNeutral).cachedElement();
        float               curBDTValue = curPi0PFO->bdtPi0Score();
        ATH_MSG_DEBUG("\t\tneutral " << iNeutral << ", BDT score = " << curBDTValue);
        if(curBDTValue > BDTScore1st) {
            BDTScore2nd = BDTScore1st;
            index2ndBDTScoreNeutral = index1stBDTScoreNeutral;
            BDTScore1st = curBDTValue;
            index1stBDTScoreNeutral = iNeutral;
        } else if(curBDTValue > BDTScore2nd) {
            BDTScore2nd = curBDTValue;
            index2ndBDTScoreNeutral = iNeutral;
        }
        ATH_MSG_DEBUG("\t\t  1st highestBDT score at " << index1stBDTScoreNeutral);
        ATH_MSG_DEBUG("\t\t  2nd highestBDT score at " << index2ndBDTScoreNeutral);
    }//end loop over neutrals
    
    std::vector<unsigned int> indexList;
    if(nMaxPi0s >= 1) indexList.push_back(index1stBDTScoreNeutral);
    if(nMaxPi0s >= 2) indexList.push_back(index2ndBDTScoreNeutral);
    
    ATH_MSG_DEBUG("\t1st highestBDT score at " << index1stBDTScoreNeutral);
    ATH_MSG_DEBUG("\t2nd highestBDT score at " << index2ndBDTScoreNeutral);
    return indexList;
}


int    PanTau::Tool_DetailsArranger::helper_CopyNeutralsAndSetPi0(xAOD::TauJet* tauJet, int nMaxPi0s, bool isSpecialCase_1pXnTo1p1n) {
    int result_numberOfPi0Set = 0;
    
    ATH_MSG_DEBUG("CopyNeutralsAndSetPi0 for nMaxPi0: " << nMaxPi0s);
    std::vector< ElementLink<xAOD::PFOContainer> > neutralPFOLinks = tauJet->cellBased_Neutral_PFOLinks();
    std::vector<unsigned> indicesOfNeutralsToBePi0 = helper_IndicesOfNeutralsToBePi0(neutralPFOLinks, nMaxPi0s);
    
    ATH_MSG_DEBUG("Lenght of indices of neutrals to be pi0 " << indicesOfNeutralsToBePi0.size());
    ATH_MSG_DEBUG("Loop over neutrals to set links. Neutrals: " << neutralPFOLinks.size());
    for(unsigned int iNeutral=0; iNeutral<neutralPFOLinks.size(); iNeutral++) {
        ElementLink<xAOD::PFOContainer> curNeutralPFOLink   = neutralPFOLinks.at(iNeutral);
        xAOD::PFO*                      curNeutralPFO       = const_cast<xAOD::PFO*>(curNeutralPFOLink.cachedElement());
        ATH_MSG_DEBUG("\tNeutral " << iNeutral);
        
        //loop over list of indices of neutrals to be pi0
        bool promoteThisNeutralToPi0 = false;
        for(unsigned int iIndex=0; iIndex<indicesOfNeutralsToBePi0.size(); iIndex++) {
            if(iNeutral == indicesOfNeutralsToBePi0.at(iIndex)) {
                promoteThisNeutralToPi0 = true;
                break;
            }
        }
        
//         //Copy nPi0Proto to nPi0  -> removed, nPi0Proto and nPi0 are set in CellBased now
//         
//             curNeutralPFO->setAttribute(xAOD::PFODetails::nPi0Proto, ProtoNPi0);
//         }
        
        //promote neutral to be a pi0
        if(promoteThisNeutralToPi0 == true) {
            ATH_MSG_DEBUG("\t\tIts a  pi0, add to pi0s");
            
            //set final number of Pi0 for this PFO
            int nPi0ToBeSet = 1;
            if(isSpecialCase_1pXnTo1p1n == true) nPi0ToBeSet = 2;
            curNeutralPFO->setAttribute(xAOD::PFODetails::nPi0, nPi0ToBeSet);
            
            //set final momentum for this PFO (set pion mass)
            P4EEtaPhiM  momentum    = P4EEtaPhiM(curNeutralPFO->e(), curNeutralPFO->eta(), curNeutralPFO->phi(), 134.98);
            curNeutralPFO->setP4(momentum.pt(), curNeutralPFO->eta(), curNeutralPFO->phi(), 134.98);
            
            //add to pi0 links
            tauJet->addPi0_PFOLink( curNeutralPFOLink );
            
            //increase number of neutral pions
            result_numberOfPi0Set++;
            
        } else {
            ATH_MSG_DEBUG("\t\tIts no pi0, do not add to pi0 neutrals");
            curNeutralPFO->setAttribute(xAOD::PFODetails::nPi0, 0);
            //note: the neutral links have been copied before
        }
    }
    
    return result_numberOfPi0Set;
}







