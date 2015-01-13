/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/PanTauSeed.h"

#include "PanTauEvent/TauConstituent.h"
// #include "tauEvent/TauJet.h"

#include <iostream>

#include "xAODTau/TauJet.h"

// default constructor
PanTau::PanTauSeed::PanTauSeed() 
    :
    P4EEtaPhiM(0., 0., 0., 0.),
    m_IsValidSeed(false),
    m_TechnicalQuality(),
    m_NameInputAlgorithm("InvalidAlg"),
    m_TauJet(0),
    m_Constituents(),
    m_TypeHLVs(),
    m_ConstituentsList_Core(),
    m_ConstituentsList_Wide(),
    m_ConstituentsList_AllSelected(),
    m_ConstituentsList_All(),
    m_DecayMode_BySubAlg(0),
    m_DecayMode_ByPanTau(0),
    m_decayModeHack_CellBasedShots(false),
    m_Features(0)
{
}



// destructor
PanTau::PanTauSeed::~PanTauSeed()
{
    if(m_Features != 0) delete m_Features;
    
    //delete the constituents
    for(unsigned int iConst=0; iConst<m_ConstituentsList_All.size(); iConst++) {
        PanTau::TauConstituent* curConst = m_ConstituentsList_All[iConst];
        if(curConst != 0) delete curConst;
    }
    m_ConstituentsList_All.clear();
    m_ConstituentsList_AllSelected.clear();
    m_ConstituentsList_Core.clear();
    m_ConstituentsList_Wide.clear();
    
    //also clear the constituent matrix
    //dont delete the entries as they point to the (at this point already deleted) constituents in m_ConstituentsList
    for(unsigned int iType=0; iType<m_Constituents.size(); iType++) {
        std::vector<PanTau::TauConstituent*> curConsts = m_Constituents[iType];
        curConsts.clear();
    }
    m_Constituents.clear();
    
}



//copy constructor
PanTau::PanTauSeed::PanTauSeed(const PanTau::PanTauSeed& rhs) 
    :
    I4Momentum(),
    P4EEtaPhiMBase(),
    P4EEtaPhiM(rhs),
    m_IsValidSeed(rhs.m_IsValidSeed),
    m_TechnicalQuality(rhs.m_TechnicalQuality),
    m_NameInputAlgorithm(rhs.m_NameInputAlgorithm),
    m_TauJet(rhs.m_TauJet),
    m_Constituents(rhs.m_Constituents),
    m_TypeHLVs(rhs.m_TypeHLVs),
    m_ConstituentsList_Core(rhs.m_ConstituentsList_Core),
    m_ConstituentsList_Wide(rhs.m_ConstituentsList_Wide),
    m_ConstituentsList_AllSelected(rhs.m_ConstituentsList_AllSelected),
    m_ConstituentsList_All(rhs.m_ConstituentsList_All),
    m_DecayMode_BySubAlg(rhs.m_DecayMode_BySubAlg),
    m_DecayMode_ByPanTau(rhs.m_DecayMode_ByPanTau),
    m_decayModeHack_CellBasedShots(rhs.m_decayModeHack_CellBasedShots),
    m_Features( (rhs.m_Features ? new PanTau::TauFeature(*rhs.m_Features) : 0) )
{
}



PanTau::PanTauSeed& PanTau::PanTauSeed::operator=(const PanTau::PanTauSeed& seed) 
    {
    if (this != &seed){
        this->set4Mom(seed);
        m_IsValidSeed           = seed.m_IsValidSeed;
        m_TechnicalQuality      = seed.m_TechnicalQuality;
        m_NameInputAlgorithm    = seed.m_NameInputAlgorithm;
        m_TauJet                = seed.m_TauJet;
        m_Constituents          = seed.m_Constituents;
        m_TypeHLVs              = seed.m_TypeHLVs;
        m_ConstituentsList_Core = seed.m_ConstituentsList_Core;
        m_ConstituentsList_Wide = seed.m_ConstituentsList_Wide;
        m_ConstituentsList_AllSelected  = seed.m_ConstituentsList_AllSelected;
        m_ConstituentsList_All          = seed.m_ConstituentsList_All;
        m_decayModeHack_CellBasedShots  = seed.m_decayModeHack_CellBasedShots;
        delete m_Features;
        m_Features              = (seed.m_Features ? new PanTau::TauFeature(*seed.m_Features) : 0);
    }
    return *this;
}



/** Main constructor to be used */
PanTau::PanTauSeed::PanTauSeed( std::string                             nameInputAlgorithm,
                                const xAOD::TauJet*                     tauJet,
                                std::vector<PanTau::TauConstituent*>    tauConstituents,
                                std::vector<PanTau::TauConstituent*>    tauConstituentsAll,
                                std::vector<int>                        pantauSeed_TechnicalQuality
                              ) 
:
    P4EEtaPhiM( tauJet->ptIntermediateAxis() * cosh( tauJet->etaIntermediateAxis() ), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis() )
{
    
    m_IsValidSeed           = true;
    m_TechnicalQuality      = pantauSeed_TechnicalQuality;
    m_NameInputAlgorithm    = nameInputAlgorithm,
    m_TauJet                = tauJet;
    m_decayModeHack_CellBasedShots = false;
    m_Features              = new PanTau::TauFeature;
    
    //save a copy of the flat constituents list and also split into core/wide constituents
    m_ConstituentsList_All = tauConstituentsAll;
    m_ConstituentsList_AllSelected = tauConstituents; //all types are in here, this is the complete list
    for(unsigned int iConst=0; iConst<tauConstituents.size(); iConst++) {
        bool isCoreChrg = tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_Charged);
        bool isCoreNeut = tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_Neutral);
        bool isCorePi0  = tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_Pi0Neut);
        
        if(isCoreChrg == true || isCoreNeut == true || isCorePi0 == true) m_ConstituentsList_Core.push_back(tauConstituents[iConst]); // Core only contains the currently used objects
        
        bool isWideChrg = tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_OutChrg);
        bool isWideNeut = tauConstituents[iConst]->isOfType(PanTau::TauConstituent::t_OutNeut);
        if(isWideChrg == true || isWideNeut == true) m_ConstituentsList_Wide.push_back(tauConstituents[iConst]); // Wide contains objectsin 0.2-0.4
    }
    
    //create the constituents lists
    for(int iType=0; iType<PanTau::TauConstituent::t_nTypes; iType++) {
        m_Constituents.push_back( std::vector<PanTau::TauConstituent*>(0) );
        m_TypeHLVs.push_back( CLHEP::HepLorentzVector(0,0,0,0) );
    }
    
    //assign tauConstituents
    CLHEP::HepLorentzVector hlv_SumConstituents_Wide = CLHEP::HepLorentzVector(0,0,0,0);
    CLHEP::HepLorentzVector hlv_SumConstituents_Core = CLHEP::HepLorentzVector(0,0,0,0);
    
//     std::cout << "PanTau::PanTauSeed\tINFO\tLoop through constituents for this seed (" << tauConstituents.size() << ") and sort them by type" << std::endl;
    int nCharged = 0;
    int nPi0Neut = 0;
    
    for(unsigned int iConst=0; iConst<tauConstituents.size(); iConst++) {
        PanTau::TauConstituent* curConst = tauConstituents.at(iConst);
        std::vector<int>        curTypes = curConst->getTypeFlags();
//         std::cout << "PanTau::PanTauSeed\tINFO\t\tConstituent " << iConst+1 << " is of " << curTypes.size() << " types" << std::endl;
        
        for(unsigned int curType=0; curType<curTypes.size(); curType++) {
            if(curTypes.at(curType) == 0) continue;
            
//             std::cout << "PanTau::PanTauSeed\tINFO\t\t\tType " << curType << " is set: " << PanTau::TauConstituent::getTypeName((PanTau::TauConstituent::Type)curType) << std::endl;
            
            if(curType == (int)PanTau::TauConstituent::t_Charged) nCharged++;
            if(curType == (int)PanTau::TauConstituent::t_Pi0Neut) nPi0Neut++;
            
            if((unsigned int)curType >= m_Constituents.size()) {
                std::cout << "PanTau::PanTauSeed\tERROR\tMore types in TauConstituent than reserved in PanTau seed constituent matrix!" << std::endl;
                continue;
            }
            
            //fill into sorted lists
            m_Constituents.at(curType).push_back(curConst);
            m_TypeHLVs.at(curType) += curConst->hlv();
            
        }//end loop over types
        
        //add each constituent to wide proto momentum
        hlv_SumConstituents_Wide += curConst->hlv();
        
        //add all charged and neutral constituents (i.e. from core region) to core proto momentum
        if(curConst->isOfType(PanTau::TauConstituent::t_Charged)) {hlv_SumConstituents_Core += curConst->hlv(); continue;}
        if(curConst->isOfType(PanTau::TauConstituent::t_Pi0Neut)) {hlv_SumConstituents_Core += curConst->hlv(); continue;}
        
    }//end loop over constituents
    
    m_ProtoMomentum_Wide = hlv_SumConstituents_Wide;
    m_ProtoMomentum_Core = hlv_SumConstituents_Core;
    
    //set mode as obtained from subalg
    m_DecayMode_BySubAlg = PanTau::PanTauSeed::getDecayMode(nCharged, nPi0Neut);
    m_DecayMode_ByPanTau = PanTau::PanTauSeed::Mode_Error;
    
    if(isOfTechnicalQuality(PanTau::PanTauSeed::t_NoValidInputTau) == true) {
        m_DecayMode_ByPanTau = PanTau::PanTauSeed::Mode_NotSet;
    }
    
//     std::cout << "PanTau::PanTauSeed\tWARNING\tProtoMomentumCore: " << m_ProtoMomentum_Core.perp() << ", " << m_ProtoMomentum_Core.eta() << ", " << m_ProtoMomentum_Core.phi() << ", " << m_ProtoMomentum_Core.m() << std::endl;
}



/** Constructor for invalid seeds */
PanTau::PanTauSeed::PanTauSeed( std::string                          nameInputAlgorithm,
                                const xAOD::TauJet*              tauJet,
                                std::vector<int>                     pantauSeed_TechnicalQuality
                              ) 
:   
    P4EEtaPhiM( tauJet->ptIntermediateAxis() * cosh( tauJet->etaIntermediateAxis() ), tauJet->etaIntermediateAxis(), tauJet->phiIntermediateAxis(), tauJet->mIntermediateAxis() ),
    m_Constituents(),
    m_TypeHLVs(),
    m_ConstituentsList_AllSelected(),
    m_ConstituentsList_All(),
    m_DecayMode_BySubAlg(0),
    m_DecayMode_ByPanTau(0),
    m_decayModeHack_CellBasedShots(false)
{
    m_IsValidSeed           = false;
    m_TechnicalQuality      = pantauSeed_TechnicalQuality;
    m_NameInputAlgorithm    = nameInputAlgorithm,
    m_TauJet                = tauJet;
    m_Features              = new PanTau::TauFeature;
}



int PanTau::PanTauSeed::getDecayMode(int nCharged, int nNeutral) {
    
    int decayMode;
    // 1 Prong modes
    if(nCharged == 1 && nNeutral == 0) decayMode = (int)PanTau::PanTauSeed::Mode_1p0n;
    else if(nCharged == 1 && nNeutral == 1) decayMode = (int)PanTau::PanTauSeed::Mode_1p1n;
    else if(nCharged == 1 && nNeutral >  1) decayMode = (int)PanTau::PanTauSeed::Mode_1pXn;
    
    // 3 prong modes
    else if(nCharged == 3 && nNeutral == 0) decayMode = (int)PanTau::PanTauSeed::Mode_3p0n;
    else if(nCharged == 3 && nNeutral >  0) decayMode = (int)PanTau::PanTauSeed::Mode_3pXn;
    
    // other mode
    else if(nCharged == 2) decayMode = (int)PanTau::PanTauSeed::Mode_Other;
    else if(nCharged == 4) decayMode = (int)PanTau::PanTauSeed::Mode_Other;
    else if(nCharged == 5) decayMode = (int)PanTau::PanTauSeed::Mode_Other;
    
    else if(nCharged == 0) decayMode = (int)PanTau::PanTauSeed::Mode_NotSet;
    else if(nCharged >= 6) decayMode = (int)PanTau::PanTauSeed::Mode_NotSet;
    
    else decayMode = (int)PanTau::PanTauSeed::Mode_Error;
    
    return decayMode;
}


std::string PanTau::PanTauSeed::getDecayModeName(int decayMode) {
    
    PanTau::PanTauSeed::DecayMode mode = (PanTau::PanTauSeed::DecayMode)decayMode;
    
    if(mode == PanTau::PanTauSeed::Mode_1p0n) return "1p0n";
    if(mode == PanTau::PanTauSeed::Mode_1p1n) return "1p1n";
    if(mode == PanTau::PanTauSeed::Mode_1pXn) return "1pXn";
    if(mode == PanTau::PanTauSeed::Mode_3p0n) return "3p0n";
    if(mode == PanTau::PanTauSeed::Mode_3pXn) return "3pXn";
    if(mode == PanTau::PanTauSeed::Mode_Other) return "Other";
    if(mode == PanTau::PanTauSeed::Mode_Error) return "Error";
    if(mode == PanTau::PanTauSeed::Mode_NotSet) return "NotSet";
    return "InvalidMode!!!";
}



bool                                                PanTau::PanTauSeed::isOfTechnicalQuality(int pantauSeed_TechnicalQuality) const {
    if(pantauSeed_TechnicalQuality > PanTau::PanTauSeed::t_nTechnicalQualities) {
        std::cout << "PanTauSeed\tERROR\tunknown technical quality value: " << pantauSeed_TechnicalQuality << ". Maximum allowed is " << PanTau::PanTauSeed::t_nTechnicalQualities << "! Check PanTauEvent/PanTauSeed.h" << std::endl;
        return false;
    }
    if(m_TechnicalQuality[pantauSeed_TechnicalQuality] == 1) return true;
    return false;
}


std::vector<PanTau::TauConstituent*>    PanTau::PanTauSeed::getConstituentsOfType(int tauConstituent_Type, bool& foundit) {
    if(tauConstituent_Type > PanTau::TauConstituent::t_nTypes) {
        foundit = false;
        std::cout << "PanTauSeed\tERROR\tunknown constituent type: " << tauConstituent_Type << " -> it's larger than the known types: " << PanTau::TauConstituent::t_nTypes << std::endl;
        return std::vector<TauConstituent*>(0);
    }
    foundit = true;
    if(tauConstituent_Type == (int)PanTau::TauConstituent::t_NoType) return m_ConstituentsList_AllSelected;
    return m_Constituents.at(tauConstituent_Type);
}



int                                     PanTau::PanTauSeed::getNumberOfConstituentsOfType(int tauConstituent_Type) {
    bool isOK = false;
    std::vector<PanTau::TauConstituent*> consts = this->getConstituentsOfType(tauConstituent_Type, isOK);
    if(isOK) {
        return (int)consts.size();
    }
    return -1;
}


CLHEP::HepLorentzVector                PanTau::PanTauSeed::getSubsystemHLV(int tauConstituent_Type, bool& foundit) {
    if(tauConstituent_Type > PanTau::TauConstituent::t_nTypes) {
        foundit = false;
        return CLHEP::HepLorentzVector(0,0,0,0);
    }
    foundit = true;
    return m_TypeHLVs.at(tauConstituent_Type);
}




