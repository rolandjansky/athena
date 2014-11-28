/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//! Gaudi includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"

//! PanTau includes
#include "PanTauAlgs/Tool_FourMomentumCalculator.h"

#include "PanTauEvent/PanTauSeed.h"


bool        sortNeutralTauConstituentMVA(const PanTau::TauConstituent* u, const PanTau::TauConstituent* v) {
    double uBDT = u->getBDTValue();
    double vBDT = v->getBDTValue();
    return uBDT > vBDT;
}


PanTau::Tool_FourMomentumCalculator::Tool_FourMomentumCalculator(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore")
        {
    declareInterface<ITool_FourMomentumCalculator>(this);
    
    declareProperty("Tool_InformationStore",            m_Tool_InformationStore,            "Tool handle to the information store tool");
}



PanTau::Tool_FourMomentumCalculator::~Tool_FourMomentumCalculator() {
}



StatusCode PanTau::Tool_FourMomentumCalculator::initialize() {
    ATH_MSG_INFO( name() << " initialize()" );
    CHECK( AthAlgTool::initialize() );
    
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Basic",        m_varTypeName_Basic) );
    
    return StatusCode::SUCCESS;
}



StatusCode PanTau::Tool_FourMomentumCalculator::calculateFourMomentum(PanTau::PanTauSeed* inSeed) const {
    
    ATH_MSG_DEBUG("calculateFourMomentum called for inSeed at " << inSeed);

    bool noAnyConstituents           = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoConstituentsAtAll);
    bool noSelConstituents           = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoSelectedConstituents);
    bool noValidInputTau             = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoValidInputTau);
    bool isBadSeed                   = (noAnyConstituents || noSelConstituents || noValidInputTau);
    
    if(isBadSeed == true) {
        ATH_MSG_DEBUG("Seed is not valid for four momentum calculation!");
        return StatusCode::SUCCESS;
    }
    
    ATH_MSG_DEBUG("Seed is valid for four momentum calculation!");
    
    //get variables needed later on
    std::string inputAlg = inSeed->getNameInputAlgorithm();
    
    int mode_SubAlg     = inSeed->getDecayModeBySubAlg();
    int mode_PanTau     = inSeed->getDecayModeByPanTau();
    
    bool isOK = false;
    std::vector<PanTau::TauConstituent*> consts_Charged = inSeed->getConstituentsOfType(PanTau::TauConstituent::t_Charged, isOK);
    if(isOK == false) return StatusCode::SUCCESS;
    std::vector<PanTau::TauConstituent*> consts_Pi0Neut = inSeed->getConstituentsOfType(PanTau::TauConstituent::t_Pi0Neut, isOK);
    if(isOK == false) return StatusCode::SUCCESS;
    std::vector<PanTau::TauConstituent*> consts_Neutral = inSeed->getConstituentsOfType(PanTau::TauConstituent::t_Neutral, isOK);
    if(isOK == false) return StatusCode::SUCCESS;
    
    //sort neutrals by pi0 BDT value
    std::sort(consts_Pi0Neut.begin(), consts_Pi0Neut.end(), sortNeutralTauConstituentMVA);
    std::sort(consts_Neutral.begin(), consts_Neutral.end(), sortNeutralTauConstituentMVA);
    
    unsigned int subAlg_NCharged    = consts_Charged.size();
    unsigned int subAlg_NPi0Neut    = consts_Pi0Neut.size();
    unsigned int subAlg_NNeutral    = consts_Neutral.size();
    
    
    CLHEP::HepLorentzVector hlv_CoreObjects = CLHEP::HepLorentzVector();
    for(unsigned int iConst=0; iConst<subAlg_NCharged; iConst++) hlv_CoreObjects += consts_Charged.at(iConst)->hlv();
    for(unsigned int iConst=0; iConst<subAlg_NPi0Neut; iConst++) hlv_CoreObjects += consts_Pi0Neut.at(iConst)->hlv();
    
    
    
    //!DEBUG output
    #ifndef NDEBUG
    ATH_MSG_DEBUG("Starting 4-momentum recalculation...");
    ATH_MSG_DEBUG("Original 4-vector: ");
    ATH_MSG_DEBUG("\tEt : " << hlv_CoreObjects.et());
    ATH_MSG_DEBUG("\tEta: " << hlv_CoreObjects.eta());
    ATH_MSG_DEBUG("\tPhi: " << hlv_CoreObjects.phi());
    ATH_MSG_DEBUG("\tm  : " << hlv_CoreObjects.m());
    
    ATH_MSG_DEBUG("4-vectors of charged constituents:");
    for(unsigned int iConst=0; iConst<subAlg_NCharged; iConst++) {
        CLHEP::HepLorentzVector cC = consts_Charged.at(iConst)->hlv();
        ATH_MSG_DEBUG("\tEt / Eta / Phi / M: " << cC.et() << " / " << cC.eta() << " / " << cC.phi() << " / " << cC.m());
    }
    ATH_MSG_DEBUG("4-vectors of pi0 constituents:");
    for(unsigned int iConst=0; iConst<subAlg_NPi0Neut; iConst++) {
        CLHEP::HepLorentzVector cC = consts_Pi0Neut.at(iConst)->hlv();
        ATH_MSG_DEBUG("\tEt / Eta / Phi / M: " << cC.et() << " / " << cC.eta() << " / " << cC.phi() << " / " << cC.m());
    }
    ATH_MSG_DEBUG("4-vectors of neutral constituents:");
    for(unsigned int iConst=0; iConst<subAlg_NNeutral; iConst++) {
        CLHEP::HepLorentzVector cC = consts_Neutral.at(iConst)->hlv();
        ATH_MSG_DEBUG("\tEt / Eta / Phi / M: " << cC.et() << " / " << cC.eta() << " / " << cC.phi() << " / " << cC.m());
    }
    #endif //NDEBUG
    
    
    /// ///////////////////////////
    /// Get the 4 momentum correct
    ///
    ///
    //the TLV to replace the default pantau one
    CLHEP::HepLorentzVector hlv_PanTau_Final = CLHEP::HepLorentzVector();
    
    
    //prepare Vector of pi0-neutrals to be used: updated version!
    // i.e. the list of pi0 neutrals to replace the default list from sub alg
    std::vector<CLHEP::HepLorentzVector> consts_Pi0NeutralUpdate = std::vector<CLHEP::HepLorentzVector>(0);
//     int nPi0AfterPanTau = subAlg_NPi0Neut;
    
    if(mode_PanTau == mode_SubAlg) {
        ATH_MSG_DEBUG("SubAlg mode == pantau Mode (" << PanTau::PanTauSeed::getDecayModeName(mode_PanTau) << " => use constituents as mode suggests)");
        
        if(inSeed->getModeHackedByCellBased() == true) {
            ATH_MSG_DEBUG("deal with the case that mode was alterd by cell based shots");
            //1p1n moved to 1pXn by cell based, mode not altered by pantau, set 4 vector here
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            
            CLHEP::HepLorentzVector hlv_SumPi0;
            
            if(subAlg_NPi0Neut > 0) {
                CLHEP::HepLorentzVector hlv_1st_pi0Neut = consts_Pi0Neut.at(0)->hlv();
                CLHEP::HepLorentzVector hlv_Pi0NeutA = CLHEP::HepLorentzVector(hlv_1st_pi0Neut.px(), hlv_1st_pi0Neut.py(), hlv_1st_pi0Neut.pz(), 0);
                hlv_PanTau_Final += hlv_Pi0NeutA;
                hlv_SumPi0 += hlv_Pi0NeutA;
            }
            
            if(subAlg_NPi0Neut > 1) {
                CLHEP::HepLorentzVector hlv_2nd_pi0Neut = consts_Pi0Neut.at(1)->hlv();
                CLHEP::HepLorentzVector hlv_Pi0NeutB = CLHEP::HepLorentzVector(hlv_2nd_pi0Neut.px(), hlv_2nd_pi0Neut.py(), hlv_2nd_pi0Neut.pz(), 0);
                hlv_PanTau_Final += hlv_Pi0NeutB;
                hlv_SumPi0 += hlv_Pi0NeutB;
            } else if(subAlg_NNeutral > 1) {
                CLHEP::HepLorentzVector hlv_1st_Neutral = consts_Neutral.at(1)->hlv();
                CLHEP::HepLorentzVector hlv_NeutralA = CLHEP::HepLorentzVector(hlv_1st_Neutral.px(), hlv_1st_Neutral.py(), hlv_1st_Neutral.pz(), 0);
                hlv_PanTau_Final += hlv_NeutralA;
                hlv_SumPi0 += hlv_NeutralA;
            }
            consts_Pi0NeutralUpdate.push_back(hlv_SumPi0);
//             nPi0AfterPanTau = 1;
            
        } else if(mode_PanTau != PanTau::PanTauSeed::Mode_3pXn) {
            ATH_MSG_DEBUG("Non 3pXn mode");
            //Just use the same objects as CellBased if the mode was not altered (and is not 3pXn)
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            for(unsigned int iPi0=0; iPi0<subAlg_NPi0Neut; iPi0++)       {
                hlv_PanTau_Final += consts_Pi0Neut.at(iPi0)->hlv();
                consts_Pi0NeutralUpdate.push_back(consts_Pi0Neut.at(iPi0)->hlv());
            }
        } else {
            ATH_MSG_DEBUG("3pXn mode");
            //use 2 pi0 maximum
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            if(consts_Pi0Neut.size() > 0) {
                hlv_PanTau_Final += consts_Pi0Neut.at(0)->hlv();
                consts_Pi0NeutralUpdate.push_back( consts_Pi0Neut.at(0)->hlv() );
            }
            if(consts_Pi0Neut.size() > 1) {
                hlv_PanTau_Final += consts_Pi0Neut.at(1)->hlv();
                consts_Pi0NeutralUpdate.push_back( consts_Pi0Neut.at(1)->hlv() );
            }
        }
            
    
    } 
    if(mode_PanTau != mode_SubAlg) {
        ATH_MSG_DEBUG("SubAlg mode != pantau Mode => case dependent 4-mom calculation");
        
        bool updatedMomentum = false;
        
        // 1p0n -> 1p1n
        if(mode_SubAlg == PanTau::PanTauSeed::Mode_1p0n && mode_PanTau == PanTau::PanTauSeed::Mode_1p1n) {
            ATH_MSG_DEBUG("\t1p0n -> 1p1n: Use tracks & highest pi0 score neutral");
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            if(consts_Neutral.size() > 0) {
                hlv_PanTau_Final += consts_Neutral.at(0)->hlv();
                consts_Pi0NeutralUpdate.push_back(consts_Neutral.at(0)->hlv());
                consts_Neutral.erase(consts_Neutral.begin());
            }
//             nPi0AfterPanTau = 1;
            updatedMomentum = true;
        }
        
        // 1p1n -> 1p0n
        if(mode_SubAlg == PanTau::PanTauSeed::Mode_1p1n && mode_PanTau == PanTau::PanTauSeed::Mode_1p0n) {
            ATH_MSG_DEBUG("\t1p1n -> 1p0n: Use tracks only");
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
//             nPi0AfterPanTau = 0;
            updatedMomentum = true;
        }
        
        // 1p1n -> 1pXn
        if(mode_SubAlg == PanTau::PanTauSeed::Mode_1p1n && mode_PanTau == PanTau::PanTauSeed::Mode_1pXn) {
            ATH_MSG_DEBUG("\t1p1n -> 1pXn: Use tracks, the pi0 from sub alg and highest pi0 score neutral");
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            
            if(consts_Pi0Neut.size() > 0) {
                hlv_PanTau_Final += consts_Pi0Neut.at(0)->hlv();
                consts_Pi0NeutralUpdate.push_back(consts_Pi0Neut.at(0)->hlv());
            } 
            
            if(consts_Neutral.size() > 1) {
                hlv_PanTau_Final += consts_Neutral.at(1)->hlv();
                consts_Pi0NeutralUpdate.push_back(consts_Neutral.at(1)->hlv());
                consts_Neutral.erase(consts_Neutral.begin()+1);
            }
//             nPi0AfterPanTau = 2;
            updatedMomentum = true;
        }
        
        // 1pXn -> 1p1n
        if(mode_SubAlg == PanTau::PanTauSeed::Mode_1pXn && mode_PanTau == PanTau::PanTauSeed::Mode_1p1n) {
            ATH_MSG_DEBUG("\t1pXn -> 1p1n: Use tracks & both pi0 from sub alg with mass 0");
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            
            P4EEtaPhiM hlv_Pi0NeutA;
            if(consts_Pi0Neut.size() > 0) {
                TauConstituent* curConst = consts_Pi0Neut.at(0);
                double newEnergy = sqrt(curConst->e()*curConst->e() - curConst->m()*curConst->m());
                hlv_Pi0NeutA = P4EEtaPhiM(newEnergy, curConst->eta(), curConst->phi(), 0);
                hlv_PanTau_Final += hlv_Pi0NeutA.hlv();
            }
            
            P4EEtaPhiM hlv_Pi0NeutB;
            if(consts_Pi0Neut.size() > 1) {
                TauConstituent* curConst = consts_Pi0Neut.at(1);
                double newEnergy = sqrt(curConst->e()*curConst->e() - curConst->m()*curConst->m());
                hlv_Pi0NeutB = P4EEtaPhiM(newEnergy, curConst->eta(), curConst->phi(), 0);
                hlv_PanTau_Final += hlv_Pi0NeutB.hlv();
            }
            
            consts_Pi0NeutralUpdate.push_back(hlv_Pi0NeutA.hlv() + hlv_Pi0NeutB.hlv());
            
//             nPi0AfterPanTau = 1;
            updatedMomentum = true;
        }
        
        // 3p0n -> 3pXn
        if(mode_SubAlg == PanTau::PanTauSeed::Mode_3p0n && mode_PanTau == PanTau::PanTauSeed::Mode_3pXn) {
            ATH_MSG_DEBUG("\t3p0n -> 3pXn: Use tracks & best pi0 score neutral");
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            
            if(consts_Neutral.size() > 0) {
                hlv_PanTau_Final += consts_Neutral.at(0)->hlv();
                consts_Pi0NeutralUpdate.push_back(consts_Neutral.at(0)->hlv());
                consts_Neutral.erase(consts_Neutral.begin());
            }
//             nPi0AfterPanTau = 1;
            updatedMomentum = true;
        }
        
        // 3pXn -> 3p0n
        if(mode_SubAlg == PanTau::PanTauSeed::Mode_3pXn && mode_PanTau == PanTau::PanTauSeed::Mode_3p0n) {
            ATH_MSG_DEBUG("\t3pXn -> 3p0n: Use tracks only");
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
//             nPi0AfterPanTau = 0;
            updatedMomentum = true;
        }
        
        
        if(updatedMomentum == false) {
            ATH_MSG_WARNING("\tUnknown subAlgMode and PanTauMode combination: SubAlg -> PanTau: " << mode_SubAlg << " -> " << mode_PanTau);
            ATH_MSG_WARNING("\tUse default 4-momentum and constituents");
            for(unsigned int iChrg=0; iChrg<subAlg_NCharged; iChrg++)    hlv_PanTau_Final += consts_Charged.at(iChrg)->hlv();
            for(unsigned int iPi0=0; iPi0<subAlg_NPi0Neut; iPi0++)       {
                hlv_PanTau_Final += consts_Pi0Neut.at(iPi0)->hlv();
                consts_Pi0NeutralUpdate.push_back(consts_Pi0Neut.at(iPi0)->hlv());
            }
        }
        
        
    } //end  if(PanTauMode != subAlgMode)
    
    //mode altered
    ATH_MSG_DEBUG("Mode altered: " << mode_SubAlg << " -> " << mode_PanTau);
    ATH_MSG_DEBUG("Pi0s altered: " << consts_Pi0Neut.size() << " -> " << consts_Pi0NeutralUpdate.size());
    
    
    ATH_MSG_DEBUG("Final 4-vector: ");
    ATH_MSG_DEBUG("\tEt : " << hlv_PanTau_Final.et());
    ATH_MSG_DEBUG("\tEta: " << hlv_PanTau_Final.eta());
    ATH_MSG_DEBUG("\tPhi: " << hlv_PanTau_Final.phi());
    ATH_MSG_DEBUG("\tm  : " << hlv_PanTau_Final.m());
    
    inSeed->setFinalMomentum(hlv_PanTau_Final);
    
    PanTau::TauFeature* featureMap = inSeed->getFeatures();
    featureMap->addFeature(inputAlg + "_" + m_varTypeName_Basic + "_FinalMomentumCore_pt", hlv_PanTau_Final.perp() );
    featureMap->addFeature(inputAlg + "_" + m_varTypeName_Basic + "_FinalMomentumCore_eta", hlv_PanTau_Final.eta() );
    featureMap->addFeature(inputAlg + "_" + m_varTypeName_Basic + "_FinalMomentumCore_phi", hlv_PanTau_Final.phi() );
    featureMap->addFeature(inputAlg + "_" + m_varTypeName_Basic + "_FinalMomentumCore_m", hlv_PanTau_Final.m() );
    
    
    return StatusCode::SUCCESS;
}



