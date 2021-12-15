/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTau/TauJet.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

#include "TMath.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <vector>
#include <map>
#include <math.h>

#include "PanTauAlgs/Tool_FeatureExtractor.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/PanTauSeed.h"
#include "PanTauAlgs/TauFeature.h"
#include "PanTauAlgs/HelperFunctions.h"


bool sortTauConstituentMVA(const PanTau::TauConstituent* u, const PanTau::TauConstituent* v) {
  double uBDT = u->getBDTValue();
  double vBDT = v->getBDTValue();
  return uBDT > vBDT;
}


bool sortTauConstituentEt(const PanTau::TauConstituent* u, const PanTau::TauConstituent* v) {
  double uEt = u->p4().Et();
  double vEt = v->p4().Et();
  return uEt > vEt;
}


PanTau::Tool_FeatureExtractor::Tool_FeatureExtractor(const std::string& name) :
  asg::AsgTool(name),
  m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"){

  declareProperty("Tool_InformationStore",            m_Tool_InformationStore,            "Tool handle to the information store tool");
  declareProperty("Tool_InformationStoreName",        m_Tool_InformationStoreName,            "Tool handle to the information store tool");
    
}


StatusCode PanTau::Tool_FeatureExtractor::initialize() {

  ATH_MSG_INFO(" initialize()");
  m_init=true;
    
  ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InformationStore, m_Tool_InformationStoreName ) );
  ATH_CHECK( m_Tool_InformationStore.retrieve() );
    
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Sum",          m_varTypeName_Sum) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Ratio",        m_varTypeName_Ratio) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_EtInRing",     m_varTypeName_EtInRing) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Isolation",    m_varTypeName_Isolation) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Num",          m_varTypeName_Num) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Mean",         m_varTypeName_Mean) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_StdDev",       m_varTypeName_StdDev) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_HLV",          m_varTypeName_HLV) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Angle",        m_varTypeName_Angle) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_DeltaR",       m_varTypeName_DeltaR) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_JetMoment",    m_varTypeName_JetMoment) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Combined",     m_varTypeName_Combined) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_JetShape",     m_varTypeName_JetShape) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_ImpactParams", m_varTypeName_ImpactParams) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Basic",        m_varTypeName_Basic) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_PID",          m_varTypeName_PID) );
    
  ATH_CHECK( m_Tool_InformationStore->getInfo_Int("FeatureExtractor_UseEmptySeeds",                           m_Config_UseEmptySeeds) );
    
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_BinEdges_Eta",               m_Config_CellBased_BinEdges_Eta) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_1prong", m_Config_CellBased_EtaBinned_Pi0MVACut_1prong) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_3prong", m_Config_CellBased_EtaBinned_Pi0MVACut_3prong) );
    
  return StatusCode::SUCCESS;
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
    double                  curEt           = curConstituent->p4().Et();
        
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


void PanTau::Tool_FeatureExtractor::addFeatureWrtSeedEnergy(PanTau::TauFeature* targetMap,
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


StatusCode PanTau::Tool_FeatureExtractor::execute(PanTau::PanTauSeed* inSeed) {
    
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
    
    
  ATH_CHECK( calculateBasicFeatures(inSeed) );
    
  ATH_CHECK( addConstituentMomenta(inSeed) );
    
  //first, calculate the Et variants for the seed
  fillVariantsSeedEt(inSeed->getConstituentsAsList_All());
    
  //loop through all types of Constituents in tau and calculate type features for them
  //baseline
  ATH_CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_NoType) );  //=> all constituents
  ATH_CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_Charged) ); //=> charged ones in core
  ATH_CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_Neutral) ); //=> neutral ones in core
  ATH_CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_Pi0Neut) ); //=> pi0 tagged ones in core
  //for testing
  ATH_CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_NeutLowA) ); //=> same as neutral but with lower Et
  ATH_CHECK( calculateFeatures(inSeed, PanTau::TauConstituent::t_NeutLowB) ); //=> same as neutral but with even lower et    
    
  //fill the combined features
  ATH_CHECK( addCombinedFeatures(inSeed) );
    
  //fill the impact paramter features
  ATH_CHECK( addImpactParameterFeatures(inSeed) );
    
  return StatusCode::SUCCESS;
}


StatusCode PanTau::Tool_FeatureExtractor::calculateBasicFeatures(PanTau::PanTauSeed* inSeed) {
    
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
      AbsCharge += std::abs((double)curConstituent->getCharge());
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
  TLorentzVector tlv_ProtoMomentumCore = inSeed->getProtoMomentumCore();
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_pt", tlv_ProtoMomentumCore.Perp());
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_eta", tlv_ProtoMomentumCore.Eta());
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_phi", tlv_ProtoMomentumCore.Phi());
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumCore_m", tlv_ProtoMomentumCore.M());
    
  TLorentzVector tlv_ProtoMomentumWide = inSeed->getProtoMomentumWide();
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_pt", tlv_ProtoMomentumWide.Perp());
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_eta", tlv_ProtoMomentumWide.Eta());
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_phi", tlv_ProtoMomentumWide.Phi());
  featureMap->addFeature(featureAlg + "_" + featurePrefix + "_ProtoMomentumWide_m", tlv_ProtoMomentumWide.M());    
    
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
    TLorentzVector                          tlv_TypeConstituents    = inSeed->getSubsystemHLV(iType, isOK); // summed hlv of objects of current type
    std::string                             curTypeName             = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)iType);
        
    std::vector<PanTau::TauConstituent*>    list_TypeConstituents_SortBDT = inSeed->getConstituentsOfType(iType, isOK);
    std::sort(list_TypeConstituents_SortBDT.begin(), list_TypeConstituents_SortBDT.end(), sortTauConstituentMVA);
        
    if(list_TypeConstituents.size() > 0) {
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumPt",  tlv_TypeConstituents.Perp());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumEta", tlv_TypeConstituents.Eta());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumPhi", tlv_TypeConstituents.Phi());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumM",   tlv_TypeConstituents.M());
    }
        
    //store 4-vectors of current type (et sort);
    std::vector<double> curConsts_pt    = std::vector<double>(0);
    std::vector<double> curConsts_eta   = std::vector<double>(0);
    std::vector<double> curConsts_phi   = std::vector<double>(0);
    std::vector<double> curConsts_m     = std::vector<double>(0);
    for(unsigned int iConst=0; iConst<n_Constituents_Type; iConst++) {
      TLorentzVector tlv_curConst = list_TypeConstituents[iConst]->p4();
      curConsts_pt.push_back(tlv_curConst.Perp());
      curConsts_eta.push_back(tlv_curConst.Eta());
      curConsts_phi.push_back(tlv_curConst.Phi());
      curConsts_m.push_back(tlv_curConst.M());
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
      TLorentzVector tlv_curConstBDT = list_TypeConstituents_SortBDT[iConst]->p4();
      curConstsBDT_pt.push_back(tlv_curConstBDT.Perp());
      curConstsBDT_eta.push_back(tlv_curConstBDT.Eta());
      curConstsBDT_phi.push_back(tlv_curConstBDT.Phi());
      curConstsBDT_m.push_back(tlv_curConstBDT.M());
    }
    tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_pt", curConstsBDT_pt);
    tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_eta", curConstsBDT_eta);
    tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_phi", curConstsBDT_phi);
    tauFeatureMap->addVecFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTSort_Constituents_m", curConstsBDT_m);
    
  } //end loop over constituent types
    
  return StatusCode::SUCCESS;
}


StatusCode PanTau::Tool_FeatureExtractor::calculateFeatures(PanTau::PanTauSeed* inSeed,
                                                            int tauConstituentType) {
    
  std::string                             curTypeName                     = PanTau::TauConstituent::getTypeName( (PanTau::TauConstituent::Type)tauConstituentType );
  std::string                             curTypeName_All                 = PanTau::TauConstituent::AllConstituentsName();
  PanTau::TauFeature*                     tauFeatureMap                   = inSeed->getFeatures();
  std::string                             inputAlgName                    = inSeed->getNameInputAlgorithm();
  TLorentzVector                          tlv_Reference                   = inSeed->getProtoMomentumCore();
    
  std::vector<PanTau::TauConstituent*>    list_AllConstituents            = inSeed->getConstituentsAsList_Core();
    
  bool                                    foundIt                         = false;
  std::vector<PanTau::TauConstituent*>    list_TypeConstituents;
  if(tauConstituentType != PanTau::TauConstituent::t_NoType) list_TypeConstituents = inSeed->getConstituentsOfType(tauConstituentType, foundIt);
  if(tauConstituentType == PanTau::TauConstituent::t_NoType) list_TypeConstituents = list_AllConstituents;
  if(foundIt == false) return StatusCode::SUCCESS;

  unsigned int                            n_Constituents_All              = list_AllConstituents.size();
  unsigned int                            n_Constituents_Type             = list_TypeConstituents.size();
        
  //sort the lists by Et
  std::sort(list_AllConstituents.begin(),     list_AllConstituents.end(),     sortTauConstituentEt);
  std::sort(list_TypeConstituents.begin(),    list_TypeConstituents.end(),    sortTauConstituentEt);
    
  TLorentzVector  tlv_1st_Et;
  TLorentzVector  tlv_2nd_Et;
  TLorentzVector  tlv_3rd_Et;
    
  if(list_TypeConstituents.size() > 0) tlv_1st_Et = list_TypeConstituents[0]->p4();
  if(list_TypeConstituents.size() > 1) tlv_2nd_Et = list_TypeConstituents[1]->p4();
  if(list_TypeConstituents.size() > 2) tlv_3rd_Et = list_TypeConstituents[2]->p4();
    
    
  TLorentzVector tlv_Last_Et;
  if(list_TypeConstituents.size() > 0) tlv_Last_Et = list_TypeConstituents.back()->p4();
    
  //make an additional list of constituents, but now ordered by BDT value
  std::vector<PanTau::TauConstituent*>    list_TypeConstituents_SortBDT = list_TypeConstituents;
  std::sort(list_TypeConstituents_SortBDT.begin(), list_TypeConstituents_SortBDT.end(), sortTauConstituentMVA);
    
  TLorentzVector  tlv_1st_BDT;
  TLorentzVector  tlv_2nd_BDT;
  TLorentzVector  tlv_3rd_BDT;
    
  if(list_TypeConstituents_SortBDT.size() > 0) tlv_1st_BDT = list_TypeConstituents_SortBDT[0]->p4();
  if(list_TypeConstituents_SortBDT.size() > 1) tlv_2nd_BDT = list_TypeConstituents_SortBDT[1]->p4();
  if(list_TypeConstituents_SortBDT.size() > 2) tlv_3rd_BDT = list_TypeConstituents_SortBDT[2]->p4();
    
  //! //////////////////////////////////////////                  
  //! Prepare variables for information from eflow Objects
  //! //////////////////////////////////////////
    
  // ===> hlv for the leading EFOs and the summed HLV
  TLorentzVector              tlv_TypeConstituents;
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
    TLorentzVector tlv_curConst = list_TypeConstituents.at(iTypeConst)->p4();
        
    //final check (nan & inf)
    if (isnan(tlv_curConst.Pt()) || isinf(tlv_curConst.Pt())) continue;
        
    //ready to calc stuff
    //basically update each of the prepared sum_* and num_* variables above,
    // the sum HLV and the pointers to 1st, 2nd, 3rd leading constituents of current type
    tlv_TypeConstituents += tlv_curConst;
        
    //helpers to reduce function calls
    double hlp_Et               = tlv_curConst.Et();
    double hlp_Et2              = hlp_Et * hlp_Et;
    double hlp_E                = tlv_curConst.E();
    double hlp_E2               = hlp_E * hlp_E;
    double hlp_DeltaR           = tlv_Reference.DeltaR(tlv_curConst);
    double hlp_DeltaR2          = hlp_DeltaR * hlp_DeltaR;
    double hlp_DeltaRLeading    = (tlv_1st_Et.Pt() == 0 ? 0 : tlv_1st_Et.DeltaR(tlv_curConst));
    double hlp_DeltaR2Leading   = hlp_DeltaRLeading * hlp_DeltaRLeading;
    double hlp_DeltaRprime      = m_HelperFunctions.deltaRprime(tlv_Reference.Vect(), tlv_curConst.Vect());
    double hlp_Angle            = tlv_Reference.Angle(tlv_curConst.Vect());
        
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
    if(tauConstituentType != PanTau::TauConstituent::t_Charged) {
      double mvaCorrection = 0.0;
      double  etaCurConst = list_TypeConstituents_SortBDT[iTypeConst]->p4().Eta();
      int     etaBinIndex = m_HelperFunctions.getBinIndex(m_Config_CellBased_BinEdges_Eta, std::abs(etaCurConst));
      int     numTrack    = inSeed->getTauJet()->nTracks();
      if(numTrack == 1) { mvaCorrection = m_Config_CellBased_EtaBinned_Pi0MVACut_1prong.at(etaBinIndex); }
      else              { mvaCorrection = m_Config_CellBased_EtaBinned_Pi0MVACut_3prong.at(etaBinIndex); }

      value_BDT = value_BDT - mvaCorrection;
    }
        
    value_sumBDT_BDTSort += value_BDT;
    std::string iConst = m_HelperFunctions.convertNumberToString((double)(iTypeConst+1));
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValues_BDTSort_" + iConst, value_BDT);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValuesSum_BDTSort_" + iConst, value_sumBDT_BDTSort);
  }
    
  //sorted by highest Et
  double value_sumBDT_EtSort = 0;
  for(unsigned int iTypeConst=0; iTypeConst<n_Constituents_Type; iTypeConst++) {
        
    double value_BDT = list_TypeConstituents[iTypeConst]->getBDTValue();
    if( isnan(value_BDT) || isinf(value_BDT) ) continue;
        
    value_sumBDT_EtSort += value_BDT;
    std::string iConst = m_HelperFunctions.convertNumberToString((double)(iTypeConst+1));
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValues_EtSort_" + iConst, value_BDT);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BDTValuesSum_EtSort_" + iConst, value_sumBDT_EtSort);
  }
    
    
  //! Shot information ///////////////////////////////////////////
  prefixVARType = PanTau::Tool_FeatureExtractor::varTypeName_Shots();
  //only execute if the constituent type is neutral
  if(PanTau::TauConstituent::isNeutralType(tauConstituentType) == true) {
        
    TLorentzVector          totalTLV_SumShots       = TLorentzVector(0., 0., 0., 0.);
    unsigned int            totalPhotonsInSeed      = 0;
    unsigned int            totalShotsInSeed        = 0;
    double                  maxDeltaRSumShotToConst = -999;
    double                  minDeltaRSumShotToConst = 999;
    double                  maxDeltaRSumShotToTau   = -999;
    double                  minDeltaRSumShotToTau   = 999;
        
    std::vector<TLorentzVector> allShotTLVs = std::vector<TLorentzVector>(0);
        
    for(unsigned int iConst=0; iConst<list_TypeConstituents_SortBDT.size(); iConst++) {
            
      PanTau::TauConstituent*                 curConst            = list_TypeConstituents_SortBDT.at(iConst);
      TLorentzVector                          tlv_CurConst        = curConst->p4();
      std::vector<PanTau::TauConstituent*>    shotConstituents    = curConst->getShots();
      unsigned int                            nShots              = shotConstituents.size();
            
      unsigned int totalPhotonsInNeutral = 0;
      TLorentzVector tlv_SumShots = TLorentzVector(0., 0., 0., 0.);
            
      for(unsigned int iShot=0; iShot<nShots; iShot++) {
	PanTau::TauConstituent* curShot     = shotConstituents.at(iShot);
	totalPhotonsInNeutral += curShot->getNPhotonsInShot();
	tlv_SumShots += curShot->p4();
	allShotTLVs.push_back(curShot->p4());
      }//end loop over shots
      totalShotsInSeed    += nShots;
      totalTLV_SumShots   += tlv_SumShots;
      totalPhotonsInSeed  += totalPhotonsInNeutral;
            
      std::string iConstStr = m_HelperFunctions.convertNumberToString((double)(iConst+1));
                       
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_nPhotons_BDTSort_" + iConstStr, totalPhotonsInNeutral);
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_nShots_BDTSort_" + iConstStr, nShots);
            
      //the et/eta/phi/m of the hlv of all shots combined for this neutral-type constituent
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_Et_BDTSort_" + iConstStr, tlv_SumShots.Et());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_Eta_BDTSort_" + iConstStr, tlv_SumShots.Eta());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_Phi_BDTSort_" + iConstStr, tlv_SumShots.Phi());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SumShots_M_BDTSort_" + iConstStr, tlv_SumShots.M());
            
      //energy ratio, deltaR of sumShots and constituent
      double deltaRSumShotToConst = tlv_CurConst.DeltaR(tlv_SumShots);
      if(deltaRSumShotToConst > maxDeltaRSumShotToConst) maxDeltaRSumShotToConst = deltaRSumShotToConst;
      if(deltaRSumShotToConst < minDeltaRSumShotToConst) minDeltaRSumShotToConst = deltaRSumShotToConst;
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ConstDeltaRToSumShots_BDTSort_" + iConstStr, deltaRSumShotToConst);
      if(tlv_CurConst.Et() > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSumShotsOverConstEt_BDTSort_" + iConstStr, tlv_SumShots.Et() / tlv_CurConst.Et());
            
      //energy ratio, deltaR of shots and tauSeed
      double deltaRSumShotToTau = tlv_Reference.DeltaR(tlv_SumShots);
      if(deltaRSumShotToTau > maxDeltaRSumShotToTau) maxDeltaRSumShotToTau = deltaRSumShotToTau;
      if(deltaRSumShotToTau < minDeltaRSumShotToTau) minDeltaRSumShotToTau = deltaRSumShotToTau;
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_TauDeltaRToSumShots_BDTSort_" + iConstStr, deltaRSumShotToTau);
      if(tlv_Reference.Et() > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtSumShotsOverTauEt_BDTSort_" + iConstStr, tlv_SumShots.Et() / tlv_Reference.Et());
            
    }//end loop over constituents in tau
        
    //delta R values
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxDeltaRSumShotToConst", maxDeltaRSumShotToConst);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MinDeltaRSumShotToConst", minDeltaRSumShotToConst);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxDeltaRSumShotToTau", maxDeltaRSumShotToTau);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MinDeltaRSumShotToTau", minDeltaRSumShotToTau);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_DeltaRAllShotsToTau", tlv_Reference.DeltaR(totalTLV_SumShots));
        
    //et ratio
    if(tlv_Reference.Et() > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtAllShotsOverEtTau", totalTLV_SumShots.Et() / tlv_Reference.Et());
        
    //number of shots in seed
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_NShotsInSeed", totalShotsInSeed);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_NPhotonsInSeed", totalPhotonsInSeed);

    //build di-Shot mass
    double maxDiShotMass    = -200;
    double minDiShotMass    = 99999;
    double bestDiShotMass   = -200;
    double bestPi0Diff      = 99999;
    for(unsigned int iShot=0; iShot<allShotTLVs.size(); iShot++) {
      TLorentzVector cur_iShot = allShotTLVs.at(iShot);
            
      for(unsigned int jShot=iShot+1; jShot<allShotTLVs.size(); jShot++) {
	TLorentzVector cur_jShot = allShotTLVs.at(jShot);
                
	ATH_MSG_DEBUG("\t\tBuilding di-shot mass of shots " << iShot << " & " << jShot);
	TLorentzVector          tlv_DiShot    = cur_iShot + cur_jShot;
	double                  curDiShotMass = tlv_DiShot.M();
	double                  curpi0Diff    = std::abs(curDiShotMass - 134.98);
	ATH_MSG_DEBUG("\t\tit is: " << curDiShotMass);
	if(curpi0Diff < bestPi0Diff) bestDiShotMass = curDiShotMass;
	if(curDiShotMass > maxDiShotMass) maxDiShotMass = curDiShotMass;
	if(curDiShotMass < minDiShotMass) minDiShotMass = curDiShotMass;
      }
    }

    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_BestDiShotMass", bestDiShotMass);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxDiShotMass", maxDiShotMass);
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MinDiShotMass", minDiShotMass);
        
  }//end if check for shot info dumping
    
    
  //! Ratios ///////////////////////////////////////////
  prefixVARType = m_varTypeName_Ratio;
    
  if(curTypeName != curTypeName_All) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EtOver", sum_Et, &m_Variants_SeedEt);
    
  if(tlv_1st_Et.Pt() != 0) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stEtOver",   tlv_1st_Et.Et(), &m_Variants_SeedEt);
  if(tlv_1st_BDT.Pt() != 0) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stBDTEtOver",   tlv_1st_BDT.Et(), &m_Variants_SeedEt);
    
  if(tlv_Last_Et.Pt() != 0) addFeatureWrtSeedEnergy(tauFeatureMap, inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_SoftestEtOver",   tlv_Last_Et.Et(), &m_Variants_SeedEt);
    
  if(tlv_1st_Et.Pt() != 0 && sum_Et > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stEtOverTypeEt",    tlv_1st_Et.Et() / sum_Et);
  if(tlv_1st_BDT.Pt() != 0 && sum_Et > 0.) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stBDTEtOverTypeEt",    tlv_1st_BDT.Et() / sum_Et);    
    
  if(n_Constituents_All != 0 && curTypeName != curTypeName_All)   tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_EFOsOverTotalEFOs", (double)(((double)num_EFOs) / ((double)n_Constituents_All)));
  if(tlv_1st_Et.Pt() != 0 && tlv_2nd_Et.Pt() != 0) {
    if(tlv_1st_Et.Et() > 0. && tlv_2nd_Et.Et() > 0. ) {
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver2ndEt", TMath::Log10(tlv_1st_Et.Et() / tlv_2nd_Et.Et()));
    }
  }
  if(tlv_1st_Et.Pt() != 0 && tlv_3rd_Et.Pt() != 0) {
    if(tlv_1st_Et.Et() > 0. && tlv_3rd_Et.Et() > 0.) {
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver3rdEt", TMath::Log10(tlv_1st_Et.Et() / tlv_3rd_Et.Et()));
    }
  }
  if(tlv_2nd_Et.Pt() != 0 && tlv_3rd_Et.Pt() != 0) {
    if(tlv_2nd_Et.Et() > 0. && tlv_3rd_Et.Et() > 0.) {
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log2ndEtOver3rdEt", TMath::Log10(tlv_2nd_Et.Et() / tlv_3rd_Et.Et()));
    }
  }
    
  //and for the BDT score ordered EFOs
  if(tlv_1st_BDT.Pt() != 0 && tlv_2nd_BDT.Pt() != 0) {
    if(tlv_1st_BDT.Et() > 0. && tlv_2nd_BDT.Et() > 0. ) {
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver2ndEt_BDTSort", TMath::Log10(tlv_1st_BDT.Et() / tlv_2nd_BDT.Et()));
    }
  }
  if(tlv_1st_BDT.Pt() != 0 && tlv_3rd_BDT.Pt() != 0) {
    if(tlv_1st_BDT.Et() > 0. && tlv_3rd_BDT.Et() > 0.) {
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log1stEtOver3rdEt_BDTSort", TMath::Log10(tlv_1st_BDT.Et() / tlv_3rd_BDT.Et()));
    }
  }
  if(tlv_2nd_BDT.Pt() != 0 && tlv_3rd_BDT.Pt() != 0) {
    if(tlv_2nd_BDT.Et() > 0. && tlv_3rd_BDT.Et() > 0.) {
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_Log2ndEtOver3rdEt_BDTSort", TMath::Log10(tlv_2nd_BDT.Et() / tlv_3rd_BDT.Et()));
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

  double stddev_E             = m_HelperFunctions.stddev(sum_E2, sum_E, num_EFOs);
  double stddev_Et            = m_HelperFunctions.stddev(sum_Et2, sum_Et, num_EFOs);
  double stddev_DRToJetAxis   = m_HelperFunctions.stddev(sum_DR2ToReference, sum_DRToReference, num_EFOs);
  double stddev_DRToLeading   = m_HelperFunctions.stddev(sum_DRToLeading, sum_DR2ToLeading, num_EFOs);
    
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
    
  if(curTypeName != curTypeName_All) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ToJetAxis",   tlv_Reference.Angle(tlv_TypeConstituents.Vect()));
  if(tlv_1st_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stToJetAxis",   tlv_Reference.Angle(tlv_1st_Et.Vect()));
  if(tlv_2nd_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndToJetAxis",   tlv_Reference.Angle(tlv_2nd_Et.Vect()));
  if(tlv_3rd_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_3rdToJetAxis",   tlv_Reference.Angle(tlv_3rd_Et.Vect()));
  if(tlv_1st_Et.Pt() != 0) {
    if(tlv_2nd_Et.Pt() != 0) {
      angle_12 = tlv_1st_Et.Angle(tlv_2nd_Et.Vect());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo2nd", angle_12);
    }
    if(tlv_3rd_Et.Pt() != 0) {
      angle_13 = tlv_1st_Et.Angle(tlv_3rd_Et.Vect());
      tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo3rd", angle_13);
    }
  }
  if(tlv_2nd_Et.Pt() != 0 && tlv_3rd_Et.Pt() != 0) {
    angle_23 = tlv_2nd_Et.Angle(tlv_3rd_Et.Vect());
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndTo3rd", angle_23);
  }
  if(num_EFOs > 2 && tlv_1st_Et.Pt() != 0 && tlv_2nd_Et.Pt() != 0 && tlv_3rd_Et.Pt() != 0) {
    double angle_Planes = ( tlv_1st_Et.Vect().Cross(tlv_2nd_Et.Vect()) ).Angle( tlv_1st_Et.Vect().Cross(tlv_3rd_Et.Vect()) );
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
    
  if(curTypeName != curTypeName_All) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_ToJetAxis",      tlv_Reference.DeltaR(tlv_TypeConstituents));
  tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_MaxToJetAxis_EtSort",   max_DeltaR);
  if(tlv_1st_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stToJetAxis_EtSort",   tlv_Reference.DeltaR(tlv_1st_Et));
  if(tlv_2nd_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndToJetAxis_EtSort",   tlv_Reference.DeltaR(tlv_2nd_Et));
  if(tlv_3rd_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_3rdToJetAxis_EtSort",   tlv_Reference.DeltaR(tlv_3rd_Et));
  if(tlv_1st_Et.Pt() != 0) {
    if(tlv_2nd_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo2nd_EtSort",   tlv_1st_Et.DeltaR(tlv_2nd_Et));
    if(tlv_3rd_Et.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo3rd_EtSort",   tlv_1st_Et.DeltaR(tlv_3rd_Et));
  }
  if(tlv_2nd_Et.Pt() != 0 && tlv_3rd_Et.Pt() != 0) {
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndTo3rd_EtSort",   tlv_2nd_Et.DeltaR(tlv_3rd_Et));
  }
    
  if(tlv_1st_BDT.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stToJetAxis_BDTSort",   tlv_Reference.DeltaR(tlv_1st_BDT));
  if(tlv_2nd_BDT.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndToJetAxis_BDTSort",   tlv_Reference.DeltaR(tlv_2nd_BDT));
  if(tlv_3rd_BDT.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_3rdToJetAxis_BDTSort",   tlv_Reference.DeltaR(tlv_3rd_BDT));
  if(tlv_1st_BDT.Pt() != 0) {
    if(tlv_2nd_BDT.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo2nd_BDTSort",   tlv_1st_BDT.DeltaR(tlv_2nd_BDT));
    if(tlv_3rd_BDT.Pt() != 0) tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_1stTo3rd_BDTSort",   tlv_1st_BDT.DeltaR(tlv_3rd_BDT));
  }
  if(tlv_2nd_BDT.Pt() != 0 && tlv_3rd_BDT.Pt() != 0) {
    tauFeatureMap->addFeature(inputAlgName + "_" + curTypeName + "_" + prefixVARType + "_2ndTo3rd_BDTSort",   tlv_2nd_BDT.DeltaR(tlv_3rd_BDT));
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
  TLorentzVector     tlv_System[PanTau::TauConstituent::t_nTypes];
  TLorentzVector     tlv_1stEFO[PanTau::TauConstituent::t_nTypes];
  TLorentzVector     tlv_2ndEFO[PanTau::TauConstituent::t_nTypes];
    
  //! //////////////////////////////////////////
  //! get input objects to calc combined features
  //! //////////////////////////////////////////
  bool tlv_Sys_OK[PanTau::TauConstituent::t_nTypes];
  bool tlv_1st_OK[PanTau::TauConstituent::t_nTypes];
  bool tlv_2nd_OK[PanTau::TauConstituent::t_nTypes];  
    
  //initialize arrays with default values
  for(unsigned int iType=0; iType<(unsigned int)PanTau::TauConstituent::t_nTypes; iType++) {
    name_EFOType[iType] = "";
    num_EFOs[iType]     = 0.;
    tlv_System[iType]   = TLorentzVector();
    tlv_1stEFO[iType]   = TLorentzVector();
    tlv_2ndEFO[iType]   = TLorentzVector();
    tlv_Sys_OK[iType]   = false;
    tlv_1st_OK[iType]   = false;
    tlv_2nd_OK[iType]   = false;
  }
    
  for(int iType=0; iType<(int)PanTau::TauConstituent::t_nTypes; iType++) {
    name_EFOType[iType] = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)iType);
        
    tlv_System[iType] = inSeed->getSubsystemHLV(iType, tlv_Sys_OK[iType]);
    if(tlv_Sys_OK[iType] == false) continue;
        
    std::vector<TauConstituent*> typeConstituents = inSeed->getConstituentsOfType(iType, tlv_Sys_OK[iType]);
    if(typeConstituents.size() == 0) tlv_Sys_OK[iType] = false;
    if(tlv_Sys_OK[iType] == false) continue;
        
    num_EFOs[iType] = typeConstituents.size();
        
    if(typeConstituents.size() > 0) {
      tlv_1stEFO[iType] = typeConstituents.at(0)->p4();
      tlv_1st_OK[iType] = true;
    } else {
      tlv_1st_OK[iType] = false;
    }
        
    if(typeConstituents.size() > 1) {
      tlv_2ndEFO[iType] = typeConstituents.at(1)->p4();
      tlv_2nd_OK[iType] = true;
    } else {
      tlv_2nd_OK[iType] = false;
    }
        
  }    
    
  // From the extracted input, calc combined features
  std::string prefixVARType = m_varTypeName_Combined;
        
  // Combined-Single Features
  // Ratios of numbers (heavily spiked, just keep them for validation)
  if(tlv_Sys_OK[et_Charged] == true && tlv_Sys_OK[et_Neutral] && num_EFOs[et_Neutral] > 0.) {
    tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_NumChargedOverNumNeutral", num_EFOs[et_Charged] / num_EFOs[et_Neutral]);
  }
  if(tlv_Sys_OK[et_Charged] == true && tlv_Sys_OK[et_All] && num_EFOs[et_All] > 0.) {
    tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_NumChargedOverNumTotal",   num_EFOs[et_Charged] / num_EFOs[et_All]);
  }
    
  if(num_EFOs[et_Charged]>0. && num_EFOs[et_Neutral]>1.) {
    if(tlv_1st_OK[et_Charged] && tlv_1st_OK[et_Neutral] && tlv_2nd_OK[et_Neutral]) {
      TVector3 axis_Plane_cn1 = (tlv_1stEFO[et_Charged].Vect()).Cross( tlv_1stEFO[et_Neutral].Vect() );
      TVector3 axis_Plane_cn2 = (tlv_1stEFO[et_Charged].Vect()).Cross( tlv_2ndEFO[et_Neutral].Vect() );
      double anglePlanes = axis_Plane_cn1.Angle(axis_Plane_cn2);
      tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_AnglePlane1stCharged1st2ndNeutral", anglePlanes);
    }
  }    
    
  PanTau::TauConstituent* tauConst_NeutralLargestAngle = m_HelperFunctions.getNeutralConstWithLargestAngle(tlv_System[et_Charged],
													    list_NeutralConstituents);
  if(tauConst_NeutralLargestAngle != 0) {
    TLorentzVector tlv_NeutralLargestAngle = tauConst_NeutralLargestAngle->p4();
        
    tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_FarthestNeutral_AngleToCharged", tlv_System[et_Charged].Angle(tlv_NeutralLargestAngle.Vect()) );
    tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_FarthestNeutral_BDTScore", tauConst_NeutralLargestAngle->getBDTValue());
    if(tlv_System[et_Charged].Et() > 0) tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_FarthestNeutral_EtOverChargedEt", tlv_NeutralLargestAngle.Et() / tlv_System[et_Charged].Et());
  }
    
  // Combined Type-vs-Type Features
  // Loop over all EFO types...
  // for every type, loop over all other types (but neglect permutations, i.e.   A,B   is the same as   B,A)
  // calculate ratios, angles etc...
    
  for(int iType=0; iType<PanTau::TauConstituent::t_nTypes; iType++) {
        
    if(iType == (int)PanTau::TauConstituent::t_NoType) continue;
    int type_Denom = iType;
        
    for(int jType=0; jType<PanTau::TauConstituent::t_nTypes; jType++) {
            
      if(jType == (int)PanTau::TauConstituent::t_NoType) continue;
      int type_Nom = jType;
            
      if(jType == iType) continue;
            
      std::string typeName_Nom    = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)type_Nom);
      std::string typeName_Denom  = PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)type_Denom);
            
      double sum_Et_Nom   = 0.0;
      double sum_Et_Denom = 0.0;
      if(tlv_Sys_OK[type_Nom] && tlv_Sys_OK[type_Denom]) {
	sum_Et_Nom   = tlv_System[type_Nom].Et();
	sum_Et_Denom = tlv_System[type_Denom].Et();
      }
            
      // Fraction of leading EFO of system A wrt complete system B
      // this is not symmetric wrt system A and B, hence do this for all combinations
      if(tlv_1st_OK[type_Nom]) {
	double LeadEFO_Et_Nom = tlv_1stEFO[type_Nom].Et();
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
      if(tlv_Sys_OK[type_Nom] && tlv_Sys_OK[type_Denom]) {
	double angle_system = tlv_System[type_Nom].Angle( tlv_System[type_Denom].Vect() );
	tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Angle" + typeName_Nom + "To" + typeName_Denom, angle_system );
      }//end check for valid system pointer            
      
      if(tlv_1st_OK[type_Nom] && tlv_1st_OK[type_Denom]) {
	//Delta R between 1st and 1st EFO
	double deltaR_1st1st = tlv_1stEFO[type_Nom].DeltaR( tlv_1stEFO[type_Denom] );
	tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_DeltaR1st" + typeName_Nom + "To1st" + typeName_Denom, deltaR_1st1st );
	
	//Angles between 1st and 1st EFO
	double angle_1st1st = tlv_1stEFO[type_Nom].Angle( tlv_1stEFO[type_Denom].Vect() );
	tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Angle1st" + typeName_Nom + "To1st" + typeName_Denom, angle_1st1st );
      } //end check for valid leading efo
            
    }//end loop over system B
  }//end loop over system A
    
    
  // Combined Selected-Type Features

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
            
      if(tlv_Sys_OK[et_c]==false || tlv_Sys_OK[et_n]==false) continue;
            
      //mean Et fraction of charged+neutral system wrt total ET
      if(num_EFOs[et_c] + num_EFOs[et_n] > 0.) {
	double mean_cTypenTypeEt = ( tlv_System[et_c].Et() + tlv_System[et_n].Et() ) / (num_EFOs[et_c] + num_EFOs[et_n]);
	addFeatureWrtSeedEnergy(tauFeatures, inputAlgName + "_" + prefixVARType + "_Mean" + name_cType + name_nType + "Et_Wrt", mean_cTypenTypeEt, &m_Variants_SeedEt);
      }
            
      //invariant masses
      double mass_cTypenType = ( tlv_System[et_c]  +  tlv_System[et_n] ).M();
      tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_InvMass"  + name_cType + name_nType,   mass_cTypenType );
            
      //angle 1st charged to second neutral
      if(tlv_2nd_OK[et_n]) {
	//Angles between 1st and 2nd EFO
	double angle_1st2nd = tlv_1stEFO[et_c].Angle( tlv_2ndEFO[et_n].Vect() );
	tauFeatures->addFeature(inputAlgName + "_" + prefixVARType + "_Angle1st2nd" + name_cType + name_nType, angle_1st2nd );
      } //end check for valid 2nd EFOs
            
    }//end loop neutral types
  }//end loop charged types
    
  return StatusCode::SUCCESS;
}


StatusCode PanTau::Tool_FeatureExtractor::addImpactParameterFeatures(PanTau::PanTauSeed* inSeed) const {
    
  const xAOD::TauJet* tauJet = inSeed->getTauJet();

  const xAOD::Vertex* vtx_TauJet = tauJet->vertex();
  if(!vtx_TauJet) {
    ATH_MSG_DEBUG("Vertex of taujet points to 0! Not extracting impact parameters");
    return StatusCode::SUCCESS;
  }
    
  PanTau::TauFeature* tauFeatures = inSeed->getFeatures();
  std::string inputAlgName = inSeed->getNameInputAlgorithm();
  std::string featureNamePrefix = m_varTypeName_ImpactParams;
  std::vector<double> impactParameters(0);
  std::vector<double> impactParameterSignf(0);
    
  // get jet direction:
  TLorentzVector tlv_Tau;
  tlv_Tau.SetPtEtaPhiM(tauJet->ptIntermediateAxis(), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis());
  TVector3 vec_Tau = tlv_Tau.Vect();
  TVector3 tauDirection = TVector3(vec_Tau.X(), vec_Tau.Y(), vec_Tau.Z());
    
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
    const xAOD::TrackParticle* curTrack = list_Tracks[iTrk];
        
    double recoD0 = curTrack->d0();
    double errD02 = curTrack->definingParametersCovMatrix()(0, 0);
    double signfD0 = -999.;
    if(errD02 > 0) signfD0 = curTrack->d0() / sqrtf( errD02 );

    double recoZ0 = curTrack->z0();
    double errZ02 = curTrack->definingParametersCovMatrix()(1, 1);
    double signfZ0 = -999.;
    if(errZ02 > 0) signfZ0 = curTrack->z0() / sqrtf( errZ02 );
        
    // add to features
    if (iTrk < 4) {
      std::string indexTrk = m_HelperFunctions.convertNumberToString(iTrk+1);
      tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransIPTrack" + indexTrk + "_SortByEt", recoD0 );
      tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_LongIPTrack" + indexTrk + "_SortByEt", recoZ0 );
            
      if(isnan(signfD0) == false) tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransSignfIPTrack" + indexTrk + "_SortByEt", signfD0);
      if(isnan(signfZ0) == false) tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_LongSignfIPTrack" + indexTrk + "_SortByEt", signfZ0);

      impactParameters.push_back(std::abs(recoD0));
      impactParameterSignf.push_back(std::abs(signfD0));
    }

  }//end loop over tracks
    
  //sort impact parameters and also store sorted by value
  std::sort( impactParameters.begin(),        impactParameters.end(),     std::greater<double>() );
  std::sort( impactParameterSignf.begin(),    impactParameterSignf.end(), std::greater<double>() );
    
  for(unsigned int iIP=0; iIP<impactParameters.size(); iIP++) {
    std::string curNum = m_HelperFunctions.convertNumberToString(iIP+1);
    tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransIP" + curNum + "_SortByValue", impactParameters[iIP] );
    tauFeatures->addFeature(inputAlgName + "_" + featureNamePrefix + "_TransSignfIP" + curNum + "_SortByValue", impactParameterSignf[iIP] );
  }
    
  return StatusCode::SUCCESS;
}








