/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Tool_FeatureExtractor
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to extract jet features from seed
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

//! Helper classes
#include "tauEvent/TauJet.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
// #include "FourMomUtils/Thrust.h"
// #include "FourMomUtils/FoxWolfram.h"
// #include "CLHEP/Units/SystemOfUnits.h"
// #include "CLHEP/Vector/ThreeVector.h"
// #include "Math/SpecFuncMathMore.h"
// #include "AthenaKernel/errorcheck.h"
// #include "CaloEvent/CaloCluster.h"
// #include "CaloEvent/CaloClusterMoment.h"

//! ROOT includes
#include "TMath.h"
#include "TLorentzVector.h"
#include "TVector3.h"

//! C++ includes
#include <vector>
#include <map>
#include <math.h>

//! PanTau includes
#include "PanTauAlgs/Tool_FeatureExtractor.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauEvent/TauConstituent.h"
#include "PanTauEvent/PanTauSeed.h"
#include "PanTauEvent/TauFeature.h"




bool        sortTauConstituentMVA(const PanTau::TauConstituent* u, const PanTau::TauConstituent* v) {
    double uBDT = u->getBDTValue();
    double vBDT = v->getBDTValue();
    return uBDT > vBDT;
}



bool        sortTauConstituentEt(const PanTau::TauConstituent* u, const PanTau::TauConstituent* v) {
    double uEt = u->hlv().et();
    double vEt = v->hlv().et();
    return uEt > vEt;
}





PanTau::Tool_FeatureExtractor::Tool_FeatureExtractor(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa) :
        AthAlgTool(ty, na, pa),
        m_Tool_HelperFunctions("PanTau::Tool_HelperFunctions/Tool_HelperFunctions"),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
        m_Tool_TrackToVertexIPEstimator("Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator") {
    declareInterface<ITool_FeatureExtractor>(this);
    
    declareProperty("Tool_HelperFunctions",             m_Tool_HelperFunctions,             "Tool Handle to Tool_HelperFunctions");
    declareProperty("Tool_TrackToVertexIPEstimator",    m_Tool_TrackToVertexIPEstimator,    "Tool Handle to track to vertex ip estimator tool");
    declareProperty("Tool_InformationStore",            m_Tool_InformationStore,            "Tool handle to the information store tool");
    
}



StatusCode PanTau::Tool_FeatureExtractor::initialize() {

    StatusCode sc = AlgTool::initialize();
    ATH_MSG_INFO(" initialize()");
    
    CHECK( m_Tool_HelperFunctions.retrieve() );
    CHECK( m_Tool_TrackToVertexIPEstimator.retrieve() );
    CHECK( m_Tool_InformationStore.retrieve() );
    
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
    
    CHECK( m_Tool_InformationStore->getInfo_Int("FeatureExtractor_UseEmptySeeds",                           m_Config_UseEmptySeeds) );
    
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_BinEdges_Eta",               m_Config_CellBased_BinEdges_Eta) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_1prong", m_Config_CellBased_EtaBinned_Pi0MVACut_1prong) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_3prong", m_Config_CellBased_EtaBinned_Pi0MVACut_3prong) );
    
    return sc;
}



void PanTau::Tool_FeatureExtractor::fillVariantsSeedEt(std::vector<PanTau::TauConstituent*> tauConstituents) {

    //use different approaches to calculate total energy of seed:
    m_Variants_SeedEt["EtAllConsts"] = 0.0;
    m_Variants_SeedEt["EtNeutLowA"]  = 0.0;
    m_Variants_SeedEt["EtNeutLowB"]  = 0.0;
    
    //loop over all constituents in seed
    for(unsigned int iConst = 0; iConst < tauConstituents.size(); iConst++) {
        
        //get current constituents
        PanTau::TauConstituent* curConstituent  = tauConstituents.at(iConst);
        double                  curEt           = curConstituent->hlv().et();
        
        //update the different Et definitions
        if(curConstituent->isOfType(PanTau::TauConstituent::t_Charged) == true) {
            m_Variants_SeedEt["EtAllConsts"]    += curEt;
            m_Variants_SeedEt["EtNeutLowA"]     += curEt;
            m_Variants_SeedEt["EtNeutLowB"]     += curEt;
        }
        if(curConstituent->isOfType(PanTau::TauConstituent::t_Neutral) == true) {
            m_Variants_SeedEt["EtAllConsts"]    += curEt;
        }
        
        if(curConstituent->isOfType(PanTau::TauConstituent::t_NeutLowA) == true) {
            m_Variants_SeedEt["EtNeutLowA"]     += curEt;
        }
        if(curConstituent->isOfType(PanTau::TauConstituent::t_NeutLowB) == true) {
            m_Variants_SeedEt["EtNeutLowB"]     += curEt;
        }
        
    }//end loop over constituents in seed
    
    return;
}



void    PanTau::Tool_FeatureExtractor::addFeatureWrtSeedEnergy( PanTau::TauFeature* targetMap,
                                                                std::string featName,
                                                                double numerator,
                                                                std::map<std::string, double>* denominatorMap) const {
    std::map<std::string, double>::iterator it = denominatorMap->begin();
    for(; it!=denominatorMap->end(); it++) {
        std::string FullName = featName + it->first;
        float       value    = (float)it->second;
        if(value <= 0. || isnan(value) || isinf(value) ) continue; 
        targetMap->addFeature(FullName, numerator / it->second);
    }
}



StatusCode PanTau::Tool_FeatureExtractor::calculateFeatures(PanTau::PanTauSeed* inSeed) {
    
    ATH_MSG_DEBUG("Calculating features...");
    
    
    bool noAnyConstituents           = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoConstituentsAtAll);
    bool noSelConstituents           = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoSelectedConstituents);
    bool noValidInputTau             = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoValidInputTau);
    bool isBadSeed                   = (noAnyConstituents || noSelConstituents || noValidInputTau);
    if(m_Config_UseEmptySeeds == true) isBadSeed = noValidInputTau;
    
    if(isBadSeed == true) {
        ATH_MSG_DEBUG("Seed is not valid for feature extraction (no constituents or no valid input tau) - just fill isPanTauCandidate feature");
        inSeed->getFeatures()->addFeature(inSeed->getNameInputAlgorithm() + "_" + m_varTypeName_Basic + "_isPanTauCandidate", 0);
        return StatusCode::SUCCESS;
    }
    inSeed->getFeatures()->addFeature(inSeed->getNameInputAlgorithm() + "_" + m_varTypeName_Basic + "_isPanTauCandidate", 1);
    
    
    ATH_MSG_DEBUG("Basic features");
    CHECK( calculateBasicFeatures(inSeed) );
    
    
    ATH_MSG_DEBUG("RawConstituent 4 vectors");
    CHECK( addConstituentMomenta(inSeed) );
    
    //first, calculate the Et variants for the seed
    fillVariantsSeedEt(inSeed->getConstituentsAsList_All());
    
    //loop through all types of Constituents in tau and calculate type features for them
    ATH_MSG_DEBUG("type specific features");
    //baseline
    CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_NoType) );  //=> all constituents
    CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_Charged) ); //=> charged ones in core
    CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_Neutral) ); //=> neutral ones in core
    CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_Pi0Neut) ); //=> pi0 tagged ones in core
    //for testing
    CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_NeutLowA) ); //=> same as neutral but with lower Et
    CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_NeutLowB) ); //=> same as neutral but with even lower et
    
    
    //fill the combined features
    ATH_MSG_DEBUG("combined features");
    CHECK( addCombinedFeatures(inSeed) );
    
    //fill the generic jet features
    ATH_MSG_DEBUG("generic jet features");
    CHECK( addGenericJetFeatures(inSeed) );
    
    //fill the impact paramter features
    ATH_MSG_DEBUG("impact parameter features");
    CHECK( addImpactParameterFeatures(inSeed) );
    
    ATH_MSG_DEBUG("Finished feature extraction");
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_FeatureExtractor::calculateBasicFeatures(PanTau::PanTauSeed* inSeed) {
    
    ATH_MSG_DEBUG("calculating basic features");
    PanTau::TauFeature* featureMap = inSeed->getFeatures();
    
    std::string featureAlg    = inSeed->getNameInputAlgorithm();
    std::string featurePrefix = m_varTypeName_Basic;
    
    
    
    double SumCharge = 0;
    double AbsCharge = 0;
    
    //! Loop over types to fill
    //!     - multiplicity of that type
    //!     - sum charge and abs charge
    for(int iType=0; iType<PanTau::TauConstituent::t_nTypes; iType++) {
        
        bool foundIt = false;
        std::vector<TauConstituent*> curList = inSeed->getConstituentsOfType(iType, foundIt);
        if(foundIt == false) continue;
        
        //store multiplicity of current type
        std::string     typeName        = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)iType);
        unsigned int    nConstituents   = curList.size();
        featureMap->addFeature(featureAlg + "_" + featurePrefix + "_N" + typeName + "Consts", nConstituents);
        
        
        //count charge, i.e. skip if not charged
        if(iType != (int)PanTau::TauConstituent::t_Charged) continue;
        for(unsigned int iConst=0; iConst<nConstituents; iConst++) {
            PanTau::TauConstituent* curConstituent = curList[iConst];
            SumCharge += curConstituent->getCharge();
            AbsCharge += fabs((double)curConstituent->getCharge());
        }
    }
    
    //store charge
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_SumCharge", SumCharge);
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_AbsCharge", AbsCharge);
    
    //! Fill multiplicity for any constituents
    //all constituents
    std::string                     typeNameAll         = PanTau::TauConstituent::AllConstituentsName();
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_N" + typeNameAll + "Consts", inSeed->getConstituentsAsList_Core().size() + inSeed->getConstituentsAsList_Wide().size());
    
    //! Fill the proto vector (i.e. sum momentum of constituents)
    //proto 4-vector (just the sum of all constituents)
    // will have better four momentum after mode ID
    CLHEP::HepLorentzVector hlv_ProtoMomentumCore = inSeed->getProtoMomentumCore();
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_pt", hlv_ProtoMomentumCore.perp());
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_eta", hlv_ProtoMomentumCore.eta());
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_phi", hlv_ProtoMomentumCore.phi());
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_m", hlv_ProtoMomentumCore.m());
    
    CLHEP::HepLorentzVector hlv_ProtoMomentumWide = inSeed->getProtoMomentumWide();
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_pt", hlv_ProtoMomentumWide.perp());
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_eta", hlv_ProtoMomentumWide.eta());
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_phi", hlv_ProtoMomentumWide.phi());
    featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_m", hlv_ProtoMomentumWide.m());
    
    
    return StatusCode::SUCCESS;
}




StatusCode PanTau::Tool_FeatureExtractor::addConstituentMomenta(PanTau::PanTauSeed* inSeed) {
    
    std::string inputAlgName  = inSeed->getNameInputAlgorithm();
    TauFeature* tauFeatureMap = inSeed->getFeatures();
    std::string prefixVARType = m_varTypeName_HLV;
    for(int iType=0; iType<(int)PanTau::TauConstituent::t_nTypes; iType++) {
        bool isOK;
        std::vector<PanTau::TauConstituent*>    list_TypeConstituents   = inSeed->getConstituentsOfType(iType, isOK); // list of constituents of current type
        unsigned int                            n_Constituents_Type     = list_TypeConstituents.size();          // number of objects of current type
        CLHEP::HepLorentzVector                 hlv_TypeConstituents    = inSeed->getSubsystemHLV(iType, isOK); // summed hlv of objects of current type
        std::string                             curTypeName             = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)iType);
        
        std::vector<PanTau::TauConstituent*>    list_TypeConstituents_SortBDT = inSeed->getConstituentsOfType(iType, isOK);
        std::sort(list_TypeConstituents_SortBDT.begin(), list_TypeConstituents_SortBDT.end(), sortTauConstituentMVA);
        
        if(list_TypeConstituents.size() > 0) {
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumPt",  hlv_TypeConstituents.perp());
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumEta", hlv_TypeConstituents.eta());
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumPhi", hlv_TypeConstituents.phi());
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumM",   hlv_TypeConstituents.m());
        }
        
        //store 4-vectors of current type (et sort);
        std::vector<double> curConsts_pt    = std::vector<double>(0);
        std::vector<double> curConsts_eta   = std::vector<double>(0);
        std::vector<double> curConsts_phi   = std::vector<double>(0);
        std::vector<double> curConsts_m     = std::vector<double>(0);
        for(unsigned int iConst=0; iConst<n_Constituents_Type; iConst++) {
            CLHEP::HepLorentzVector hlv_curConst = list_TypeConstituents[iConst]->hlv();
            curConsts_pt.push_back(hlv_curConst.perp());
            curConsts_eta.push_back(hlv_curConst.eta());
            curConsts_phi.push_back(hlv_curConst.phi());
            curConsts_m.push_back(hlv_curConst.m());
        }
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSort_Constituents_pt", curConsts_pt);
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSort_Constituents_eta", curConsts_eta);
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSort_Constituents_phi", curConsts_phi);
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSort_Constituents_m", curConsts_m);
        
        
        //store 4-vectors of current type (bdt sort)
        std::vector<double> curConstsBDT_pt    = std::vector<double>(0);
        std::vector<double> curConstsBDT_eta   = std::vector<double>(0);
        std::vector<double> curConstsBDT_phi   = std::vector<double>(0);
        std::vector<double> curConstsBDT_m     = std::vector<double>(0);
        for(unsigned int iConst=0; iConst<n_Constituents_Type; iConst++) {
            CLHEP::HepLorentzVector hlv_curConstBDT = list_TypeConstituents_SortBDT[iConst]->hlv();
            curConstsBDT_pt.push_back(hlv_curConstBDT.perp());
            curConstsBDT_eta.push_back(hlv_curConstBDT.eta());
            curConstsBDT_phi.push_back(hlv_curConstBDT.phi());
            curConstsBDT_m.push_back(hlv_curConstBDT.m());
        }
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_pt", curConstsBDT_pt);
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_eta", curConstsBDT_eta);
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_phi", curConstsBDT_phi);
        tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_m", curConstsBDT_m);
    
    } //end loop over constituent types
    
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_FeatureExtractor::calculateFeatures(PanTau::PanTauSeed* inSeed,
                                                            int                 tauConstituentType) {
    //
    
    
    
    std::string                             curTypeName                     = PanTau::TauConstituent::getTypeName( (PanTau::TauConstituent::Type)tauConstituentType );
    std::string                             curTypeName_All                 = PanTau::TauConstituent::AllConstituentsName();
    PanTau::TauFeature*                     tauFeatureMap                   = inSeed->getFeatures();
    std::string                             inputAlgName                    = inSeed->getNameInputAlgorithm();
//     CLHEP::HepLorentzVector                 hlv_OldReference                = const_cast<Analysis::TauJet*>(inSeed->getTauJet())->getHLV(TauJetParameters::IntermediateAxis);
    CLHEP::HepLorentzVector                 hlv_Reference                   = inSeed->getProtoMomentumCore();
    
    std::vector<PanTau::TauConstituent*>    list_AllConstituents            = inSeed->getConstituentsAsList_Core();
    
    bool                                    foundIt                         = false;
    std::vector<PanTau::TauConstituent*>    list_TypeConstituents;
    if(tauConstituentType != PanTau::TauConstituent::t_NoType) list_TypeConstituents = inSeed->getConstituentsOfType(tauConstituentType, foundIt);
    if(tauConstituentType == PanTau::TauConstituent::t_NoType) list_TypeConstituents = list_AllConstituents;
    if(foundIt == false) return StatusCode::SUCCESS;

    unsigned int                            n_Constituents_All              = list_AllConstituents.size();
    unsigned int                            n_Constituents_Type             = list_TypeConstituents.size();
    
    ATH_MSG_DEBUG("Calculating features for " << list_TypeConstituents.size() << " constituents of type " << tauConstituentType << "(" << curTypeName << ")");
    
    //sort the lists by Et
    std::sort(list_AllConstituents.begin(),     list_AllConstituents.end(),     sortTauConstituentEt);
    std::sort(list_TypeConstituents.begin(),    list_TypeConstituents.end(),    sortTauConstituentEt);
    
    CLHEP::HepLorentzVector  hlv_1st_Et = CLHEP::HepLorentzVector();
    CLHEP::HepLorentzVector  hlv_2nd_Et = CLHEP::HepLorentzVector();
    CLHEP::HepLorentzVector  hlv_3rd_Et = CLHEP::HepLorentzVector();
    
    if(list_TypeConstituents.size() > 0) hlv_1st_Et = list_TypeConstituents[0]->hlv();
    if(list_TypeConstituents.size() > 1) hlv_2nd_Et = list_TypeConstituents[1]->hlv();
    if(list_TypeConstituents.size() > 2) hlv_3rd_Et = list_TypeConstituents[2]->hlv();
    
    
    CLHEP::HepLorentzVector  hlv_Last_Et = CLHEP::HepLorentzVector();
    if(list_TypeConstituents.size() > 0) hlv_Last_Et = list_TypeConstituents.back()->hlv();
    
    //make an additional list of constituents, but now ordered by BDT value
    std::vector<PanTau::TauConstituent*>    list_TypeConstituents_SortBDT = list_TypeConstituents;
    std::sort(list_TypeConstituents_SortBDT.begin(), list_TypeConstituents_SortBDT.end(), sortTauConstituentMVA);
    
    CLHEP::HepLorentzVector  hlv_1st_BDT = CLHEP::HepLorentzVector();
    CLHEP::HepLorentzVector  hlv_2nd_BDT = CLHEP::HepLorentzVector();
    CLHEP::HepLorentzVector  hlv_3rd_BDT = CLHEP::HepLorentzVector();
    
    if(list_TypeConstituents_SortBDT.size() > 0) hlv_1st_BDT = list_TypeConstituents_SortBDT[0]->hlv();
    if(list_TypeConstituents_SortBDT.size() > 1) hlv_2nd_BDT = list_TypeConstituents_SortBDT[1]->hlv();
    if(list_TypeConstituents_SortBDT.size() > 2) hlv_3rd_BDT = list_TypeConstituents_SortBDT[2]->hlv();
    
    
    
    
    
    //! //////////////////////////////////////////
    //! Prepare variables for information from eflow Objects
    //! //////////////////////////////////////////
    
    // ===> hlv for the leading EFOs and the summed HLV
    CLHEP::HepLorentzVector         hlv_TypeConstituents;
    // ===> Sum of DeltaR to jet axis
    double                      sum_DRToReference             = 0;
    double                      sum_DR2ToReference            = 0;
    double                      sum_DRToLeading             = 0;
    double                      sum_DR2ToLeading            = 0;
    // ===> Sum of Et, Et^2, E and E^2
    double                      sum_Et                      = 0;
    double                      sum_Et2                     = 0;
    double                      sum_E                       = 0;
    double                      sum_E2                      = 0;
    // ===> Sum of Et (and E) times DeltaR, DeltaR', Angle 
    double                      sum_EtxDR                   = 0;
    double                      sum_EtxDR2                  = 0;
    double                      sum_EtxDRprime              = 0;
    double                      sum_EtxAngle                = 0;
    double                      sum_ExDR                    = 0;
    double                      sum_ExDR2                   = 0;
    double                      sum_ExDRprime               = 0;
    double                      sum_ExAngle                 = 0;
    // ===> Isolation rings
    double                      sum_EtInRing00To01          = 0;
    double                      sum_EtInRing01To02          = 0;
    double                      sum_EtInRing02To03          = 0;
    double                      sum_EtInRing03To04          = 0;
    double                      sum_EtInRing04To05          = 0;
    // ===> Multiplicities
    unsigned int                num_EFOs                    = 0;
    unsigned int                num_ConstsIn00To01             = 0;
    unsigned int                num_ConstsIn01To02             = 0;
    unsigned int                num_ConstsIn02To03             = 0;
    unsigned int                num_ConstsIn03To04             = 0;
    unsigned int                num_ConstsIn04To05             = 0;
    // ===> Maximal values
    double                      max_DeltaR                  = 0;
    
    
    //! //////////////////////////////////////////
    //! Loop over selected constituents and collect information
    //! //////////////////////////////////////////
    for(unsigned int iTypeConst=0; iTypeConst<list_TypeConstituents.size(); iTypeConst++) {
        
        //get hep lorentz vector
        CLHEP::HepLorentzVector hlv_curConst = list_TypeConstituents.at(iTypeConst)->hlv();
        
        //final check (nan & inf)
        if (isnan(hlv_curConst.perp()) || isinf(hlv_curConst.perp())) continue;
        
        //ready to calc stuff
        //basically update each of the prepared sum_* and num_* variables above,
        // the sum HLV and the pointers to 1st, 2nd, 3rd leading constituents of current type
        hlv_TypeConstituents += hlv_curConst;
        
        //helpers to reduce function calls
        double hlp_Et               = hlv_curConst.et();
        double hlp_Et2              = hlp_Et * hlp_Et;
        double hlp_E                = hlv_curConst.e();
        double hlp_E2               = hlp_E * hlp_E;
        double hlp_DeltaR           = hlv_Reference.deltaR(hlv_curConst);
        double hlp_DeltaR2          = hlp_DeltaR * hlp_DeltaR;
        double hlp_DeltaRLeading    = (hlv_1st_Et.perp() == 0 ? 0 : hlv_1st_Et.deltaR(hlv_curConst));
        double hlp_DeltaR2Leading   = hlp_DeltaRLeading * hlp_DeltaRLeading;
        double hlp_DeltaRprime      = m_Tool_HelperFunctions->deltaRprime(hlv_Reference.vect(), hlv_curConst.vect());
        double hlp_Angle            = hlv_Reference.angle(hlv_curConst.vect());
        
        // update sum of DeltaR to jet axis
        sum_DRToReference           += hlp_DeltaR;
        sum_DR2ToReference          += hlp_DeltaR2;
        sum_DRToLeading             += hlp_DeltaRLeading;
        sum_DR2ToLeading            += hlp_DeltaR2Leading;
        // update Sum of Et, Et^2, E and E^2
        sum_Et                      += hlp_Et;
        sum_Et2                     += hlp_Et2;
        sum_E                       += hlp_E;
        sum_E2                      += hlp_E2;
        // update Sum of Et (and E) times DeltaR, DeltaR', Angle 
        sum_EtxDR                   += hlp_Et * hlp_DeltaR;
        sum_EtxDR2                  += hlp_Et * hlp_DeltaR2;
        sum_EtxDRprime              += hlp_Et * hlp_DeltaRprime;
        sum_EtxAngle                += hlp_Et * hlp_Angle;
        sum_ExDR                    += hlp_E  * hlp_DeltaR;
        sum_ExDR2                   += hlp_E  * hlp_DeltaR2;
        sum_ExDRprime               += hlp_E  * hlp_DeltaRprime;
        sum_ExAngle                 += hlp_E  * hlp_Angle;
        // update Isolation rings
        if(hlp_DeltaR >= 0.0 && hlp_DeltaR < 0.1) sum_EtInRing00To01 += hlp_Et;
        if(hlp_DeltaR >= 0.1 && hlp_DeltaR < 0.2) sum_EtInRing01To02 += hlp_Et;
        if(hlp_DeltaR >= 0.2 && hlp_DeltaR < 0.3) sum_EtInRing02To03 += hlp_Et;
        if(hlp_DeltaR >= 0.3 && hlp_DeltaR < 0.4) sum_EtInRing03To04 += hlp_Et;
        if(hlp_DeltaR >= 0.4 && hlp_DeltaR < 0.5) sum_EtInRing04To05 += hlp_Et;
        // update Multiplicities
        num_EFOs++;
        if(hlp_DeltaR >= 0.0 && hlp_DeltaR < 0.1) num_ConstsIn00To01++;
        if(hlp_DeltaR >= 0.1 && hlp_DeltaR < 0.2) num_ConstsIn01To02++;
        if(hlp_DeltaR >= 0.2 && hlp_DeltaR < 0.3) num_ConstsIn02To03++;
        if(hlp_DeltaR >= 0.3 && hlp_DeltaR < 0.4) num_ConstsIn03To04++;
        if(hlp_DeltaR >= 0.4 && hlp_DeltaR < 0.5) num_ConstsIn04To05++;
        // update Max values
        if(hlp_DeltaR > max_DeltaR) max_DeltaR = hlp_DeltaR;
    }//end loop over selected EFOs
    
    
    
    //! //////////////////////////////////////////
    //! Calculate & fill features
    //! //////////////////////////////////////////
    
    // naming string
    std::string prefixVARType = "";
    
    //! Multiplicities ///////////////////////////////////////////
    //if there are no EFOs of this type in the seed, all other variables cannot be calculated to have any reasonable value
    if( num_EFOs == 0 ) {
        return StatusCode::SUCCESS;
    }
    
    prefixVARType = m_varTypeName_Num;
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ConstsIn00To01", num_ConstsIn00To01);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ConstsIn01To02", num_ConstsIn01To02);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ConstsIn02To03", num_ConstsIn02To03);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ConstsIn03To04", num_ConstsIn03To04);
    
    
    
    //! Substructure particle ID features ///////////////////////////////////////////
    prefixVARType = m_varTypeName_PID;
    
    // Sorted by highest BDT score
    double value_sumBDT_BDTSort = 0;
    for(unsigned int iTypeConst=0; iTypeConst<n_Constituents_Type; iTypeConst++) {
        
        double value_BDT = list_TypeConstituents_SortBDT[iTypeConst]->getBDTValue();
        if( isnan(value_BDT) || isinf(value_BDT) ) continue;
        
        //correct BDT value based on BDT cut
        if(inputAlgName == "CellBased" && tauConstituentType != PanTau::TauConstituent::t_Charged) {
            double mvaCorrection = 0.0;
            double  etaCurConst = list_TypeConstituents_SortBDT[iTypeConst]->eta();
            int     etaBinIndex = m_Tool_HelperFunctions->getBinIndex(m_Config_CellBased_BinEdges_Eta, fabs(etaCurConst));
            bool    isOK;
            int     numTrack    = inSeed->getConstituentsOfType(PanTau::TauConstituent::t_Charged, isOK).size();
            if(numTrack == 1) { mvaCorrection = m_Config_CellBased_EtaBinned_Pi0MVACut_1prong.at(etaBinIndex); }
            else              { mvaCorrection = m_Config_CellBased_EtaBinned_Pi0MVACut_3prong.at(etaBinIndex); }
            value_BDT = value_BDT - mvaCorrection;
        }
        
        value_sumBDT_BDTSort += value_BDT;
        std::string iConst = m_Tool_HelperFunctions->convertNumberToString((double)(iTypeConst+1));
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValues_BDTSort_" + iConst, value_BDT);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValuesSum_BDTSort_" + iConst, value_sumBDT_BDTSort);
    }
    
    //sorted by highest Et
    double value_sumBDT_EtSort = 0;
    for(unsigned int iTypeConst=0; iTypeConst<n_Constituents_Type; iTypeConst++) {
        
        double value_BDT = list_TypeConstituents[iTypeConst]->getBDTValue();
        if( isnan(value_BDT) || isinf(value_BDT) ) continue;
        
        value_sumBDT_EtSort += value_BDT;
        std::string iConst = m_Tool_HelperFunctions->convertNumberToString((double)(iTypeConst+1));
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValues_EtSort_" + iConst, value_BDT);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValuesSum_EtSort_" + iConst, value_sumBDT_EtSort);
    }
    
    
    //! Shot information ///////////////////////////////////////////
    prefixVARType = PanTau::Tool_FeatureExtractor::varTypeName_Shots();
    //only execute if the inSeed was built from CellBased and the constituent type is neutral
    if(inputAlgName == "CellBased" && PanTau::TauConstituent::isNeutralType(tauConstituentType) == true) {
        ATH_MSG_DEBUG("---> Dumping shot information from " << list_TypeConstituents_SortBDT.size() << " constituents of type " << curTypeName << " in tau");
        
        CLHEP::HepLorentzVector totalHLV_SumShots       = CLHEP::HepLorentzVector(0., 0., 0., 0.);
        unsigned int            totalPhotonsInSeed      = 0;
        unsigned int            totalShotsInSeed        = 0;
        double                  maxDeltaRSumShotToConst = -999;
        double                  minDeltaRSumShotToConst = 999;
        double                  maxDeltaRSumShotToTau   = -999;
        double                  minDeltaRSumShotToTau   = 999;
        
        std::vector<CLHEP::HepLorentzVector> allShotHLVs = std::vector<CLHEP::HepLorentzVector>(0);
        
        for(unsigned int iConst=0; iConst<list_TypeConstituents_SortBDT.size(); iConst++) {
            ATH_MSG_DEBUG("\tConstituent " << iConst << " / " << list_TypeConstituents_SortBDT.size());
            
            PanTau::TauConstituent*                 curConst            = list_TypeConstituents_SortBDT.at(iConst);
            CLHEP::HepLorentzVector                 hlv_CurConst        = curConst->hlv();
            std::vector<PanTau::TauConstituent*>    shotConstituents    = curConst->getShots();
            unsigned int                            nShots              = shotConstituents.size();
            ATH_MSG_DEBUG("\t\tConstituent has Et/Eta/Phi/M: " << curConst->et() << " / " << curConst->eta() << " / " << curConst->phi() << " / " << curConst->m());
            ATH_MSG_DEBUG("\t\tShots in this constituent: " << nShots);
            
            unsigned int totalPhotonsInNeutral = 0;
            CLHEP::HepLorentzVector hlv_SumShots = CLHEP::HepLorentzVector(0., 0., 0., 0.);
            
            for(unsigned int iShot=0; iShot<nShots; iShot++) {
                PanTau::TauConstituent* curShot     = shotConstituents.at(iShot);
                unsigned int            curNPhotons = curShot->getNPhotonsInShot();
                ATH_MSG_DEBUG("\t\t\tPhotons in this shot: " << curNPhotons);
                totalPhotonsInNeutral += curShot->getNPhotonsInShot();
                ATH_MSG_DEBUG("\t\t\tEt/Eta/Phi/M of this shot: " << curShot->et() << " / " << curShot->eta() << " / " << curShot->phi() << " / " << curShot->m() );
                hlv_SumShots += curShot->hlv();
                allShotHLVs.push_back(curShot->hlv());
            }//end loop over shots
            totalShotsInSeed    += nShots;
            totalHLV_SumShots   += hlv_SumShots;
            totalPhotonsInSeed  += totalPhotonsInNeutral;
            
            ATH_MSG_DEBUG("\t\tTotal Photons: " << totalPhotonsInNeutral);
            ATH_MSG_DEBUG("\t\tEt/Eta/Phi/M of combined shots: " << hlv_SumShots.et() << " / " << hlv_SumShots.eta() << " / " << hlv_SumShots.phi() << " / " << hlv_SumShots.m() );
            
            std::string iConstStr = m_Tool_HelperFunctions->convertNumberToString((double)(iConst+1));
            ATH_MSG_DEBUG("\t\tAdded variable " << inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_nPhotons_BDTSort_" + iConstStr << " with value " << totalPhotonsInNeutral);
            
            
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_nPhotons_BDTSort_" + iConstStr, totalPhotonsInNeutral);
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_nShots_BDTSort_" + iConstStr, nShots);
            
            //the et/eta/phi/m of the hlv of all shots combined for this neutral-type constituent
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_Et_BDTSort_" + iConstStr, hlv_SumShots.et());
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_Eta_BDTSort_" + iConstStr, hlv_SumShots.eta());
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_Phi_BDTSort_" + iConstStr, hlv_SumShots.phi());
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_M_BDTSort_" + iConstStr, hlv_SumShots.m());
            
            //energy ratio, deltaR of sumShots and constituent
            double deltaRSumShotToConst = hlv_CurConst.deltaR(hlv_SumShots);
            if(deltaRSumShotToConst > maxDeltaRSumShotToConst) maxDeltaRSumShotToConst = deltaRSumShotToConst;
            if(deltaRSumShotToConst < minDeltaRSumShotToConst) minDeltaRSumShotToConst = deltaRSumShotToConst;
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ConstDeltaRToSumShots_BDTSort_" + iConstStr, deltaRSumShotToConst);
            if(hlv_CurConst.et() > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSumShotsOverConstEt_BDTSort_" + iConstStr, hlv_SumShots.et() / hlv_CurConst.et());
            
            //energy ratio, deltaR of shots and tauSeed
            double deltaRSumShotToTau = hlv_Reference.deltaR(hlv_SumShots);
            if(deltaRSumShotToTau > maxDeltaRSumShotToTau) maxDeltaRSumShotToTau = deltaRSumShotToTau;
            if(deltaRSumShotToTau < minDeltaRSumShotToTau) minDeltaRSumShotToTau = deltaRSumShotToTau;
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_TauDeltaRToSumShots_BDTSort_" + iConstStr, deltaRSumShotToTau);
            if(hlv_Reference.et() > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSumShotsOverTauEt_BDTSort_" + iConstStr, hlv_SumShots.et() / hlv_Reference.et());
            
        }//end loop over constituents in tau
        
        ATH_MSG_DEBUG("\tLoop over constituents in tau finished!");
        ATH_MSG_DEBUG("\tTotal photons from shots: " << totalPhotonsInSeed);
        
        //delta R values
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxDeltaRSumShotToConst", maxDeltaRSumShotToConst);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MinDeltaRSumShotToConst", minDeltaRSumShotToConst);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxDeltaRSumShotToTau", maxDeltaRSumShotToTau);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MinDeltaRSumShotToTau", minDeltaRSumShotToTau);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_DeltaRAllShotsToTau", hlv_Reference.deltaR(totalHLV_SumShots));
        
        //et ratio
        if(hlv_Reference.et() > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtAllShotsOverEtTau", totalHLV_SumShots.et() / hlv_Reference.et());
        
        //number of shots in seed
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_NShotsInSeed", totalShotsInSeed);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_NPhotonsInSeed", totalPhotonsInSeed);

        //build di-Shot mass
        ATH_MSG_DEBUG("\tBuild di-shot masses and check difference to pion mass");
        double maxDiShotMass    = -200;
        double minDiShotMass    = 99999;
        double bestDiShotMass   = -200;
        double bestPi0Diff      = 99999;
        for(unsigned int iShot=0; iShot<allShotHLVs.size(); iShot++) {
            CLHEP::HepLorentzVector cur_iShot = allShotHLVs.at(iShot);
            
            for(unsigned int jShot=iShot+1; jShot<allShotHLVs.size(); jShot++) {
                CLHEP::HepLorentzVector cur_jShot = allShotHLVs.at(jShot);
                
                ATH_MSG_DEBUG("\t\tBuilding di-shot mass of shots " << iShot << " & " << jShot);
                CLHEP::HepLorentzVector hlv_DiShot    = cur_iShot + cur_jShot;
                double                  curDiShotMass = hlv_DiShot.m();
                double                  curpi0Diff    = fabs(curDiShotMass - 134.98);
                ATH_MSG_DEBUG("\t\tit is: " << curDiShotMass);
                if(curpi0Diff < bestPi0Diff) bestDiShotMass = curDiShotMass;
                if(curDiShotMass > maxDiShotMass) maxDiShotMass = curDiShotMass;
                if(curDiShotMass < minDiShotMass) minDiShotMass = curDiShotMass;
            }
        }
        ATH_MSG_DEBUG("\tBest di-shot mass: " << bestDiShotMass);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BestDiShotMass", bestDiShotMass);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxDiShotMass", maxDiShotMass);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MinDiShotMass", minDiShotMass);
        
    }//end if check for shot info dumping
    
    
    //! Ratios ///////////////////////////////////////////
    prefixVARType = m_varTypeName_Ratio;
    
    if(curTypeName != curTypeName_All) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtOver", sum_Et, &m_Variants_SeedEt);
    
    if(hlv_1st_Et.perp() != 0) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stEtOver",   hlv_1st_Et.et(), &m_Variants_SeedEt);
    if(hlv_1st_BDT.perp() != 0) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stBDTEtOver",   hlv_1st_BDT.et(), &m_Variants_SeedEt);
    
    if(hlv_Last_Et.perp() != 0) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SoftestEtOver",   hlv_Last_Et.et(), &m_Variants_SeedEt);
    
    if(hlv_1st_Et.perp() != 0 && sum_Et > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stEtOverTypeEt",    hlv_1st_Et.et() / sum_Et);
    if(hlv_1st_BDT.perp() != 0 && sum_Et > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stBDTEtOverTypeEt",    hlv_1st_BDT.et() / sum_Et);
    
    
    
    if(n_Constituents_All != 0 && curTypeName != curTypeName_All)   tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EFOsOverTotalEFOs", (double)(((double)num_EFOs) / ((double)n_Constituents_All)));
    if(hlv_1st_Et.perp() != 0 && hlv_2nd_Et.perp() != 0) {
        if(hlv_1st_Et.et() > 0. && hlv_2nd_Et.et() > 0. ) {
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver2ndEt", TMath::Log10(hlv_1st_Et.et() / hlv_2nd_Et.et()));
        }
    }
    if(hlv_1st_Et.perp() != 0 && hlv_3rd_Et.perp() != 0) {
        if(hlv_1st_Et.et() > 0. && hlv_3rd_Et.et() > 0.) {
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver3rdEt", TMath::Log10(hlv_1st_Et.et() / hlv_3rd_Et.et()));
        }
    }
    if(hlv_2nd_Et.perp() != 0 && hlv_3rd_Et.perp() != 0) {
        if(hlv_2nd_Et.et() > 0. && hlv_3rd_Et.et() > 0.) {
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log2ndEtOver3rdEt", TMath::Log10(hlv_2nd_Et.et() / hlv_3rd_Et.et()));
        }
    }
    
    //and for the BDT score ordered EFOs
    if(hlv_1st_BDT.perp() != 0 && hlv_2nd_BDT.perp() != 0) {
        if(hlv_1st_BDT.et() > 0. && hlv_2nd_BDT.et() > 0. ) {
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver2ndEt_BDTSort", TMath::Log10(hlv_1st_BDT.et() / hlv_2nd_BDT.et()));
        }
    }
    if(hlv_1st_BDT.perp() != 0 && hlv_3rd_BDT.perp() != 0) {
        if(hlv_1st_BDT.et() > 0. && hlv_3rd_BDT.et() > 0.) {
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver3rdEt_BDTSort", TMath::Log10(hlv_1st_BDT.et() / hlv_3rd_BDT.et()));
        }
    }
    if(hlv_2nd_BDT.perp() != 0 && hlv_3rd_BDT.perp() != 0) {
        if(hlv_2nd_BDT.et() > 0. && hlv_3rd_BDT.et() > 0.) {
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log2ndEtOver3rdEt_BDTSort", TMath::Log10(hlv_2nd_BDT.et() / hlv_3rd_BDT.et()));
        }
    }
    
    
    //! EtRings  ///////////////////////////////////////////
    if(curTypeName == curTypeName_All) {
        prefixVARType = m_varTypeName_EtInRing;
        
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_00To01", sum_EtInRing00To01);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_01To02", sum_EtInRing01To02);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_02To03", sum_EtInRing02To03);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_03To04", sum_EtInRing03To04);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_04To05", sum_EtInRing04To05);
    }
    
    
    //! Isolations ///////////////////////////////////////////
    if(curTypeName == curTypeName_All) {
        prefixVARType = m_varTypeName_Isolation;
        
        double iso_EtIn01 = sum_EtInRing00To01;
        double iso_EtIn02 = iso_EtIn01 + sum_EtInRing01To02;
        double iso_EtIn03 = iso_EtIn02 + sum_EtInRing02To03;
        double iso_EtIn04 = iso_EtIn03 + sum_EtInRing03To04;
        
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn01Over", iso_EtIn01, &m_Variants_SeedEt);
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn02Over", iso_EtIn02, &m_Variants_SeedEt);
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn03Over", iso_EtIn03, &m_Variants_SeedEt);
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn04Over", iso_EtIn04, &m_Variants_SeedEt);
        
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn00To02Over", (sum_EtInRing00To01 + sum_EtInRing01To02), &m_Variants_SeedEt);
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn02To04Over", (sum_EtInRing02To03 + sum_EtInRing03To04), &m_Variants_SeedEt);
        
        if(iso_EtIn02>0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn01OverEtIn02", iso_EtIn01 / iso_EtIn02);
        if(iso_EtIn04>0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtIn01OverEtIn04", iso_EtIn01 / iso_EtIn04);
    }
    
    
    //! Means ///////////////////////////////////////////
    prefixVARType = m_varTypeName_Mean;
    
    if(num_EFOs > 0) {
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Et_Wrt",           (sum_Et / num_EFOs), &m_Variants_SeedEt);
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_DRToJetAxis_Wrt",  (sum_DRToReference / num_EFOs), &m_Variants_SeedEt);
        addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_DRToLeading_Wrt",  (sum_DRToLeading / num_EFOs), &m_Variants_SeedEt);
    }
    
    
    //! Standard deviations ///////////////////////////////////////////
    prefixVARType =  m_varTypeName_StdDev;

    double stddev_E             = m_Tool_HelperFunctions->stddev(sum_E2, sum_E, num_EFOs);
    double stddev_Et            = m_Tool_HelperFunctions->stddev(sum_Et2, sum_Et, num_EFOs);
    double stddev_DRToJetAxis   = m_Tool_HelperFunctions->stddev(sum_DR2ToReference, sum_DRToReference, num_EFOs);
    double stddev_DRToLeading   = m_Tool_HelperFunctions->stddev(sum_DRToLeading, sum_DR2ToLeading, num_EFOs);
    
    if(stddev_E > 0.)           tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_E",              stddev_E);
    if(stddev_Et > 0.)          tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Et",             stddev_Et);
    if(stddev_Et > 0.)          addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Et_Wrt", stddev_Et, &m_Variants_SeedEt);
    if(stddev_DRToJetAxis > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_DRToJetAxis",    stddev_DRToJetAxis);
    if(stddev_DRToLeading > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_DRToLeading",    stddev_DRToLeading);
        
    
    //! Angles ///////////////////////////////////////////
    prefixVARType = m_varTypeName_Angle;
    
    double angle_12 = 0;
    double angle_13 = 0;
    double angle_23 = 0;
    
    if(curTypeName != curTypeName_All) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ToJetAxis",   hlv_Reference.angle(hlv_TypeConstituents));
    if(hlv_1st_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stToJetAxis",   hlv_Reference.angle(hlv_1st_Et));
    if(hlv_2nd_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndToJetAxis",   hlv_Reference.angle(hlv_2nd_Et));
    if(hlv_3rd_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_3rdToJetAxis",   hlv_Reference.angle(hlv_3rd_Et));
    if(hlv_1st_Et.perp() != 0) {
        if(hlv_2nd_Et.perp() != 0) {
            angle_12 = hlv_1st_Et.angle(hlv_2nd_Et);
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo2nd", angle_12);
        }
        if(hlv_3rd_Et.perp() != 0) {
            angle_13 = hlv_1st_Et.angle(hlv_3rd_Et);
            tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo3rd", angle_13);
        }
    }
    if(hlv_2nd_Et.perp() != 0 && hlv_3rd_Et.perp() != 0) {
        angle_23 = hlv_2nd_Et.angle(hlv_3rd_Et);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndTo3rd", angle_23);
    }
    if(num_EFOs > 2 && hlv_1st_Et.perp() != 0 && hlv_2nd_Et.perp() != 0 && hlv_3rd_Et.perp() != 0) {
        double angle_Planes = ( hlv_1st_Et.vect().cross(hlv_2nd_Et.vect()) ).angle( hlv_1st_Et.vect().cross(hlv_3rd_Et.vect()) );
        double angle_max    = 0;
        if(angle_12 > angle_13) {
            if(angle_12 > angle_23) angle_max = angle_12;
            else angle_max =angle_23;
        } else {
            if(angle_13 > angle_23) angle_max =angle_13;
            else angle_max =angle_23;
        }
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxToJetAxis", angle_max);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MeanValue123", (angle_12 + angle_13 + angle_23)/3.);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Btw1213Planes", angle_Planes);
        
    }
    
    
    //! DeltaR ///////////////////////////////////////////
    prefixVARType = m_varTypeName_DeltaR;
    
    if(curTypeName != curTypeName_All) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ToJetAxis",      hlv_Reference.deltaR(hlv_TypeConstituents));
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxToJetAxis_EtSort",   max_DeltaR);
    if(hlv_1st_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stToJetAxis_EtSort",   hlv_Reference.deltaR(hlv_1st_Et));
    if(hlv_2nd_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndToJetAxis_EtSort",   hlv_Reference.deltaR(hlv_2nd_Et));
    if(hlv_3rd_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_3rdToJetAxis_EtSort",   hlv_Reference.deltaR(hlv_3rd_Et));
    if(hlv_1st_Et.perp() != 0) {
        if(hlv_2nd_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo2nd_EtSort",   hlv_1st_Et.deltaR(hlv_2nd_Et));
        if(hlv_3rd_Et.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo3rd_EtSort",   hlv_1st_Et.deltaR(hlv_3rd_Et));
    }
    if(hlv_2nd_Et.perp() != 0 && hlv_3rd_Et.perp() != 0) {
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndTo3rd_EtSort",   hlv_2nd_Et.deltaR(hlv_3rd_Et));
    }
    
    if(hlv_1st_BDT.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stToJetAxis_BDTSort",   hlv_Reference.deltaR(hlv_1st_BDT));
    if(hlv_2nd_BDT.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndToJetAxis_BDTSort",   hlv_Reference.deltaR(hlv_2nd_BDT));
    if(hlv_3rd_BDT.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_3rdToJetAxis_BDTSort",   hlv_Reference.deltaR(hlv_3rd_BDT));
    if(hlv_1st_BDT.perp() != 0) {
        if(hlv_2nd_BDT.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo2nd_BDTSort",   hlv_1st_BDT.deltaR(hlv_2nd_BDT));
        if(hlv_3rd_BDT.perp() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo3rd_BDTSort",   hlv_1st_BDT.deltaR(hlv_3rd_BDT));
    }
    if(hlv_2nd_BDT.perp() != 0 && hlv_3rd_BDT.perp() != 0) {
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndTo3rd_BDTSort",   hlv_2nd_BDT.deltaR(hlv_3rd_BDT));
    }
    
    
    //! JetMoment ///////////////////////////////////////////
    //  Moment wrt X = Sum( Et * X ) / Sum(Et)
    //  ==> Named as Eflow_EFOType_JetMoment_EtX"   when using Et as weight
    //  ==> Named as Eflow_EFOType_JetMoment_EX"    when using E  as weight
    prefixVARType = m_varTypeName_JetMoment;
    
    if(sum_Et > 0.) {
        //Using transverse energy
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtDR",            sum_EtxDR / sum_Et);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtDRprime",       sum_EtxDRprime / sum_Et);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtDR2",           sum_EtxDR2 / sum_Et);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtAngle",         sum_EtxAngle / sum_Et);
        tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtDRxTotalEt",    (sum_EtxDR / sum_Et) * m_Variants_SeedEt["EtAllConsts"]);
    }
    
    
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_FeatureExtractor::addCombinedFeatures(PanTau::PanTauSeed* inSeed) {
    
    //! //////////////////////////////////////////
    //! Prepare some short notations for variables
    //! //////////////////////////////////////////
    PanTau::TauFeature* tauFeatures     = inSeed->getFeatures();
    std::string         inputAlgName    = inSeed->getNameInputAlgorithm();
    
    
    //et: EFO Type
    int et_Charged      = PanTau::TauConstituent::t_Charged;
    int et_Pi0Neut      = PanTau::TauConstituent::t_Pi0Neut;
    int et_Neutral      = PanTau::TauConstituent::t_Neutral;
    int et_All          = PanTau::TauConstituent::t_NoType;
    
    bool foundIt;
    std::vector<PanTau::TauConstituent*>    list_NeutralConstituents = inSeed->getConstituentsOfType(et_Neutral, foundIt);
    
    //! //////////////////////////////////////////
    //! Prepare the list of names for EFO Types & 
    //! the 4 momenta of the different sub systems 
    //! (ie. charged, neutral subsystem, etc...)
    //! //////////////////////////////////////////
    std::string                 name_EFOType[PanTau::TauConstituent::t_nTypes];
    double                      num_EFOs[PanTau::TauConstituent::t_nTypes];
    CLHEP::HepLorentzVector     hlv_System[PanTau::TauConstituent::t_nTypes];
    CLHEP::HepLorentzVector     hlv_1stEFO[PanTau::TauConstituent::t_nTypes];
    CLHEP::HepLorentzVector     hlv_2ndEFO[PanTau::TauConstituent::t_nTypes];
    
    //! //////////////////////////////////////////
    //! get input objects to calc combined features
    //! //////////////////////////////////////////
    bool hlv_Sys_OK[PanTau::TauConstituent::t_nTypes];
    bool hlv_1st_OK[PanTau::TauConstituent::t_nTypes];
    bool hlv_2nd_OK[PanTau::TauConstituent::t_nTypes];
    
    
    //initialize arrays with default values
    for(unsigned int iType=0; iType<(unsigned int)PanTau::TauConstituent::t_nTypes; iType++) {
        name_EFOType[iType] = "";
        num_EFOs[iType]     = 0.;
        hlv_System[iType]   = CLHEP::HepLorentzVector();
        hlv_1stEFO[iType]   = CLHEP::HepLorentzVector();
        hlv_2ndEFO[iType]   = CLHEP::HepLorentzVector();
        hlv_Sys_OK[iType]   = false;
        hlv_1st_OK[iType]   = false;
        hlv_2nd_OK[iType]   = false;
    }
    
    for(int iType=0; iType<(int)PanTau::TauConstituent::t_nTypes; iType++) {
        name_EFOType[iType] = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)iType);
        
        hlv_System[iType] = inSeed->getSubsystemHLV(iType, hlv_Sys_OK[iType]);
        if(hlv_Sys_OK[iType] == false) continue;
        
        std::vector<TauConstituent*> typeConstituents = inSeed->getConstituentsOfType(iType, hlv_Sys_OK[iType]);
        if(typeConstituents.size() == 0) hlv_Sys_OK[iType] = false;
        if(hlv_Sys_OK[iType] == false) continue;
        
        num_EFOs[iType] = typeConstituents.size();
        
        if(typeConstituents.size() > 0) {
            hlv_1stEFO[iType] = typeConstituents.at(0)->hlv();
            hlv_1st_OK[iType] = true;
        } else {
            hlv_1st_OK[iType] = false;
        }
        
        if(typeConstituents.size() > 1) {
            hlv_2ndEFO[iType] = typeConstituents.at(1)->hlv();
            hlv_2nd_OK[iType] = true;
        } else {
            hlv_2nd_OK[iType] = false;
        }
        
    }
    
    
    //! //////////////////////////////////////////
    //! From the extracted input, calc combined features
    //! //////////////////////////////////////////
    std::string prefixVARType = m_varTypeName_Combined;
    
    
    //! Combined-Single Features ////////////////////////////////////////
    // Ratios of numbers (heavily spiked, just keep them for validation)
    if(hlv_Sys_OK[et_Charged] == true && hlv_Sys_OK[et_Neutral] && num_EFOs[et_Neutral] > 0.) {
        tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_NumChargedOverNumNeutral", num_EFOs[et_Charged] / num_EFOs[et_Neutral]);
    }
    if(hlv_Sys_OK[et_Charged] == true && hlv_Sys_OK[et_All] && num_EFOs[et_All] > 0.) {
        tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_NumChargedOverNumTotal",   num_EFOs[et_Charged] / num_EFOs[et_All]);
    }
    
    if(num_EFOs[et_Charged]>0. && num_EFOs[et_Neutral]>1.) {
        if(hlv_1st_OK[et_Charged] && hlv_1st_OK[et_Neutral] && hlv_2nd_OK[et_Neutral]) {
            CLHEP::Hep3Vector axis_Plane_cn1 = (hlv_1stEFO[et_Charged].vect()).cross( hlv_1stEFO[et_Neutral].vect() );
            CLHEP::Hep3Vector axis_Plane_cn2 = (hlv_1stEFO[et_Charged].vect()).cross( hlv_2ndEFO[et_Neutral].vect() );
            double anglePlanes = axis_Plane_cn1.angle(axis_Plane_cn2);
            tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_AnglePlane1stCharged1st2ndNeutral", anglePlanes);
        }
    }
    
    
    PanTau::TauConstituent* tauConst_NeutralLargestAngle = m_Tool_HelperFunctions->getNeutralConstWithLargestAngle(hlv_System[et_Charged],
                                                                                                                   list_NeutralConstituents);
    if(tauConst_NeutralLargestAngle != 0) {
        CLHEP::HepLorentzVector hlv_NeutralLargestAngle = tauConst_NeutralLargestAngle->hlv();
        
        tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_FarthestNeutral_AngleToCharged", hlv_System[et_Charged].angle(hlv_NeutralLargestAngle.vect()) );
        tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_FarthestNeutral_BDTScore", tauConst_NeutralLargestAngle->getBDTValue());
        if(hlv_System[et_Charged].et() > 0) tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_FarthestNeutral_EtOverChargedEt", hlv_NeutralLargestAngle.et() / hlv_System[et_Charged].et());
    }
    
    //! Combined Type-vs-Type Features ////////////////////////////////////////
    //Loop over all EFO types...
    //  for every type, loop over all other types (but neglect permutations, i.e.   A,B   is the same as   B,A)
    //     calculate ratios, angles etc...
    
    for(int iType=0; iType<PanTau::TauConstituent::t_nTypes; iType++) {
        
        if(iType == (int)PanTau::TauConstituent::t_NoType) continue;
        int type_Denom    = iType;
        
        for(int jType=0; jType<PanTau::TauConstituent::t_nTypes; jType++) {
            
            if(jType == (int)PanTau::TauConstituent::t_NoType) continue;
            int type_Nom   = jType;
            
            if(jType == iType) continue;
            
            std::string typeName_Nom    = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)type_Nom);
            std::string typeName_Denom  = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)type_Denom);
            
            double sum_Et_Nom   = 0.0;
            double sum_Et_Denom = 0.0;
            if(hlv_Sys_OK[type_Nom] && hlv_Sys_OK[type_Denom]) {
                sum_Et_Nom   = hlv_System[type_Nom].et();
                sum_Et_Denom = hlv_System[type_Denom].et();
            }
            
            //Fraction of leading EFO of system A wrt complete system B
            // this is not symmetric wrt system A and B, hence do this for all combinations
            if(hlv_1st_OK[type_Nom]) {
                double LeadEFO_Et_Nom = hlv_1stEFO[type_Nom].et();
                if(LeadEFO_Et_Nom > 0. && sum_Et_Denom > 0.) {
                    tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Log1st" + typeName_Nom + "EtOver" + typeName_Denom + "Et", TMath::Log10(LeadEFO_Et_Nom / sum_Et_Denom) );
                }
            }
            
            //following features symmetric in system A-B, hence skip multi calculation
            if(jType <= iType) continue;
            
            //Energy ratios between systems
            if(sum_Et_Denom > 0. && sum_Et_Nom > 0.) {
                tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Log" + typeName_Nom + "EtOver" + typeName_Denom + "Et",   TMath::Log10(sum_Et_Nom / sum_Et_Denom) );
            }//end check for div by zero
            
            //Angles between systems
            if(hlv_Sys_OK[type_Nom] && hlv_Sys_OK[type_Denom]) {
                double angle_system = hlv_System[type_Nom].angle( hlv_System[type_Denom] );
                m_Tool_HelperFunctions->dumpFourMomentum(hlv_System[type_Nom]);
                m_Tool_HelperFunctions->dumpFourMomentum(hlv_System[type_Denom]);
                tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Angle" + typeName_Nom + "To" + typeName_Denom, angle_system );
            }//end check for valid system pointer
            
            
            if(hlv_1st_OK[type_Nom] && hlv_1st_OK[type_Denom]) {
                //Delta R between 1st and 1st EFO
                double deltaR_1st1st = hlv_1stEFO[type_Nom].deltaR( hlv_1stEFO[type_Denom] );
                tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_DeltaR1st" + typeName_Nom + "To1st" + typeName_Denom, deltaR_1st1st );
                
                //Angles between 1st and 1st EFO
                double angle_1st1st = hlv_1stEFO[type_Nom].angle( hlv_1stEFO[type_Denom] );
                tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Angle1st" + typeName_Nom + "To1st" + typeName_Denom, angle_1st1st );
            } //end check for valid leading efo
            
        }//end loop over system B
    }//end loop over system A
    
    
    //! Combined Selected-Type Features ////////////////////////////////////////

    //setup arrays for combination of selected charged and neutral combinations
    const int cTypes = 1;
    const int nTypes = 2;
    int index_charged[cTypes] = {et_Charged};
    int index_neutral[nTypes] = {et_Pi0Neut, et_Neutral};
    
    for(int cType=0; cType<cTypes; cType++) {
        for(int nType=0; nType<nTypes; nType++) {
            
            int et_c = index_charged[cType];
            int et_n = index_neutral[nType];
            
            std::string name_cType = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)et_c);
            std::string name_nType = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)et_n);
            
            if(hlv_Sys_OK[et_c]==false || hlv_Sys_OK[et_n]==false) continue;
            
            //mean Et fraction of charged+neutral system wrt total ET
            if(num_EFOs[et_c] + num_EFOs[et_n] > 0.) {
                double mean_cTypenTypeEt = ( hlv_System[et_c].et() + hlv_System[et_n].et() ) / (num_EFOs[et_c] + num_EFOs[et_n]);
                addFeatureWrtSeedEnergy(tauFeatures, inputAlgName + "_" + prefixVARType + "_Mean" + name_cType + name_nType + "Et_Wrt", mean_cTypenTypeEt, &m_Variants_SeedEt);
            }
            
            //invariant masses
            double mass_cTypenType = ( hlv_System[et_c]  +  hlv_System[et_n] ).m();
            tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_InvMass"  + name_cType + name_nType,   mass_cTypenType );
            
            //angle 1st charged to second neutral
            if(hlv_2nd_OK[et_n]) {
                //Angles between 1st and 2nd EFO
                double angle_1st2nd = hlv_1stEFO[et_c].angle( hlv_2ndEFO[et_n] );
                tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Angle1st2nd" + name_cType + name_nType, angle_1st2nd );
            } //end check for valid 2nd EFOs
            
        }//end loop neutral types
    }//end loop charged types
    
    
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_FeatureExtractor::addGenericJetFeatures(PanTau::PanTauSeed* inSeed) const {
    
    std::string                     inputAlgName    = inSeed->getNameInputAlgorithm();
    std::vector<TauConstituent*>    allConstituents = inSeed->getConstituentsAsList_Core();
    PanTau::TauFeature*             tauFeatures     = inSeed->getFeatures();
    
    const std::string namePrefix = m_varTypeName_JetShape;
    //! Jet Thrust
    if(allConstituents.size() > 1) {
        bool thrustOK = false;
        std::vector<double> thrustValues = m_Tool_HelperFunctions->calcThrust(&allConstituents, thrustOK);
        if (thrustValues.size() == 3 && thrustOK==true) {
            const double thrust         = thrustValues[0];
            const double thrust_major   = thrustValues[1];
            const double thrust_minor   = thrustValues[2];

            tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetThrust", thrust);
            tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetThrustMajor", thrust_major);
            if (allConstituents.size() > 2) {
                tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetThrustMinor", thrust_minor);
                tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetOblateness", thrust_major - thrust_minor);
            } //end check for num. constituents for reasonable minor and oblateness
        }//end check for reasonable thrustValues
    }//end check for calculation of thrust
    
    
    //! Jet Fox Wolfram Moments
    // described here: http://cepa.fnal.gov/psm/simulation/mcgen/lund/pythia_manual/pythia6.3/pythia6301/node215.html
    bool fwOK = false;
    std::vector<double> fwValues = m_Tool_HelperFunctions->calcFWMoments(&allConstituents, fwOK);
    if(fwOK == true) {
        tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolfram1", fwValues[1]);
        tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolfram2", fwValues[2]);
        tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolfram3", fwValues[3]);
        tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolfram4", fwValues[4]);
    
        if(fwValues[1] > 0.) {
            tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolframRatioFW2OverFW1", fwValues[2] / fwValues[1]);
            tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolframRatioFW4pow4OverFW1", pow(fwValues[4], 4) / fwValues[1]);
            tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolframRatioFW234OverFW1pow4", fwValues[2]*fwValues[3]*fwValues[4] / pow(fwValues[1], 4));
        }
        if(fwValues[4] > 0.) tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetFoxWolframRatioFW1PlusFW2OverFW4", (fwValues[1] + fwValues[2]) / fwValues[4]);
    } //end check for valid calculation of FoxWolfram moments
    
    
    //! Sphericity, aplanarity and planarity
    bool sphericityOK =false;
    std::vector<double> sphValues = m_Tool_HelperFunctions->calcSphericity(&allConstituents, sphericityOK);
    if(sphericityOK == true) {
        tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetSphericity",     sphValues[0]);
        tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetAplanarity",     sphValues[1]);
        tauFeatures->addFeature(inputAlgName + "_" + namePrefix + "_JetPlanarity",      sphValues[2]);
    }
    
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_FeatureExtractor::addImpactParameterFeatures(PanTau::PanTauSeed* inSeed) const {
    
    const xAOD::TauJet*     tauJet      = inSeed->getTauJet();
//     const Trk::RecVertex*   vtx_TauJet  = 0; //inSeed->getTauJet()->origin();

    const xAOD::Vertex* vtx_TauJet = tauJet->vertexLink().cachedElement();
    if(vtx_TauJet == 0) {
        ATH_MSG_DEBUG("Trk::RecVertex of taujet (via origin()) points to 0! Not extracting impact parameters");
        return StatusCode::SUCCESS;
    }
    
    PanTau::TauFeature* tauFeatures             = inSeed->getFeatures();
    std::string         inputAlgName            = inSeed->getNameInputAlgorithm();
    std::string         featureNamePrefix       = m_varTypeName_ImpactParams;
    std::vector<double> impactParameters(0);
    std::vector<double> impactParameterSignf(0);
    
    // get jet direction:
//     CLHEP::Hep3Vector       vec_Tau         = const_cast<Analysis::TauJet*>(inSeed->getTauJet())->getHLV(TauJetParameters::IntermediateAxis).vect();
    TLorentzVector hlv_Tau;
    hlv_Tau.SetPtEtaPhiM(tauJet->ptIntermediateAxis(), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis());
    TVector3                vec_Tau         = hlv_Tau.Vect();
    const CLHEP::Hep3Vector tauDirection    = CLHEP::Hep3Vector(vec_Tau.X(), vec_Tau.Y(), vec_Tau.Z());
    
    // get a list of tracks from the inputseed
    // NOTE: if we ever have more than one charged type, may want to generalize this part to automagically get IPs from all tracks
    bool foundIt;
    std::vector<PanTau::TauConstituent*>    list_ChargedConsts = inSeed->getConstituentsOfType(PanTau::TauConstituent::t_Charged, foundIt);
    if(foundIt == false || list_ChargedConsts.size() == 0) return StatusCode::SUCCESS;
    std::sort(list_ChargedConsts.begin(),     list_ChargedConsts.end(),     sortTauConstituentEt);
    
    std::vector<const xAOD::TrackParticle*>  list_Tracks;
    for(unsigned int iChrg=0; iChrg<list_ChargedConsts.size(); iChrg++) list_Tracks.push_back( list_ChargedConsts[iChrg]->getPFO()->track(0) );
    
    // calculate the transverse impact parameter for the 4 highest momentum tracks
    for(unsigned int iTrk=0; iTrk<list_Tracks.size(); iTrk++) {
        const xAOD::TrackParticle*              curTrack        = list_Tracks[iTrk];
        
        const Trk::ImpactParametersAndSigma*    impactParameter = m_Tool_TrackToVertexIPEstimator->estimate(curTrack, vtx_TauJet);
        
        if(impactParameter == 0) {
            ATH_MSG_DEBUG("could not extract impact parameter for track at " << curTrack << " and vtx at " << vtx_TauJet);
            continue;
        }

        //get d0 value and significance
        double recoD0 = fabs(impactParameter->IPd0);
        double signfD0 = (impactParameter->sigmad0 > 0. ? fabs(recoD0/impactParameter->sigmad0) : -999. );
        if (vtx_TauJet) {
            //FIXME:
            //xAOD::TrackParticle does not return requested Trk::TrackParameters for get3DLifetimeSignOfTrack 
            double lifetimeSign = m_Tool_TrackToVertexIPEstimator->get3DLifetimeSignOfTrack(curTrack->perigeeParameters(), tauDirection, *vtx_TauJet);
            recoD0 *= lifetimeSign;
            signfD0 *= lifetimeSign;
        } else {
            ATH_MSG_WARNING( "No primary vertex, use absolute value of transverse impact parameter" );
        }
        
        //get z0 value and significance
        double recoZ0 = fabs(impactParameter->IPz0);
        double signfZ0 = (impactParameter->sigmaz0 > 0. ? fabs(recoZ0/impactParameter->sigmaz0) : -999. );
        if (vtx_TauJet) {
            //FIXME:
            //xAOD::TrackParticle does not return requested Trk::TrackParameters for getZLifetimeSignOfTrack 
            double lifetimeSign = m_Tool_TrackToVertexIPEstimator->getZLifetimeSignOfTrack(curTrack->perigeeParameters(), tauDirection, *vtx_TauJet);
            recoZ0 *= lifetimeSign;
            signfZ0 *= lifetimeSign;
        } else {
            ATH_MSG_WARNING( "No primary vertex, use absolute value of longitudinal impact parameter" );
        }
        
        // add to features
        if (iTrk < 4) {
            std::string indexTrk = m_Tool_HelperFunctions->convertNumberToString(iTrk+1);
            tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransIPTrack" + indexTrk + "_SortByEt", recoD0 );
            tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_LongIPTrack" + indexTrk + "_SortByEt", recoZ0 );
            
            if(isnan(signfD0) == false) tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransSignfIPTrack" + indexTrk + "_SortByEt", signfD0);
            if(isnan(signfZ0) == false) tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_LongSignfIPTrack" + indexTrk + "_SortByEt", signfZ0);

            impactParameters.push_back(fabs(recoD0));
            impactParameterSignf.push_back(fabs(signfD0));
        }
        
        delete impactParameter;
        impactParameter = 0;
            
    }//end loop over tracks
    
    //sort impact parameters and also store sorted by value
    std::sort( impactParameters.begin(),        impactParameters.end(),     std::greater<double>() );
    std::sort( impactParameterSignf.begin(),    impactParameterSignf.end(), std::greater<double>() );
    
    for(unsigned int iIP=0; iIP<impactParameters.size(); iIP++) {
        std::string curNum = m_Tool_HelperFunctions->convertNumberToString(iIP+1);
        tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransIP" + curNum + "_SortByValue", impactParameters[iIP] );
        tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransSignfIP" + curNum + "_SortByValue", impactParameterSignf[iIP] );
    }
    
    return StatusCode::SUCCESS;
}








