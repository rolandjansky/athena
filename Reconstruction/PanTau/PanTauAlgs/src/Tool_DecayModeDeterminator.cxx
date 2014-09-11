/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//! Gaudi includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"

//! PanTau includes
#include "PanTauAlgs/Tool_DecayModeDeterminator.h"
#include "PanTauAlgs/Tool_ModeDiscriminator.h"

#include "PanTauEvent/PanTauSeed.h"
#include "PanTauEvent/TauClassificationTypes.h"



PanTau::Tool_DecayModeDeterminator::Tool_DecayModeDeterminator(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
        m_Tool_ModeDiscriminator_1p0n_vs_1p1n("PanTau::Tool_ModeDiscriminator/Tool_ModeDiscriminator"),
        m_Tool_ModeDiscriminator_1p1n_vs_1pXn("PanTau::Tool_ModeDiscriminator/Tool_ModeDiscriminator"),
        m_Tool_ModeDiscriminator_3p0n_vs_3pXn("PanTau::Tool_ModeDiscriminator/Tool_ModeDiscriminator")
        {
    declareInterface<ITool_DecayModeDeterminator>(this);

    declareProperty("Tool_InformationStore",          m_Tool_InformationStore,          "Tool handle to the information store tool");
    declareProperty("Tool_ModeDiscriminator_1p0n_vs_1p1n",    m_Tool_ModeDiscriminator_1p0n_vs_1p1n,    "Handle to the ModeDiscriminator tool for 1p0n_vs_1p1n");
    declareProperty("Tool_ModeDiscriminator_1p1n_vs_1pXn",    m_Tool_ModeDiscriminator_1p1n_vs_1pXn,    "Handle to the ModeDiscriminator tool for 1p1n_vs_1pXn");
    declareProperty("Tool_ModeDiscriminator_3p0n_vs_3pXn",    m_Tool_ModeDiscriminator_3p0n_vs_3pXn,    "Handle to the ModeDiscriminator tool for 3p0n_vs_3pXn");
}



PanTau::Tool_DecayModeDeterminator::~Tool_DecayModeDeterminator() {
}



StatusCode PanTau::Tool_DecayModeDeterminator::initialize() {
    ATH_MSG_INFO( name() << " initialize()" );
    CHECK( AthAlgTool::initialize() );
    
    //get link to store gate
    CHECK(service("StoreGateSvc", m_sgSvc));
    
    //get the discri tools
    CHECK(m_Tool_ModeDiscriminator_1p0n_vs_1p1n.retrieve());
    CHECK(m_Tool_ModeDiscriminator_1p1n_vs_1pXn.retrieve());
    CHECK(m_Tool_ModeDiscriminator_3p0n_vs_3pXn.retrieve());
    
    //get the variable prefix
    CHECK( m_Tool_InformationStore->getInfo_String("FeatureExtractor_VarTypeName_varTypeName_Basic",        m_varTypeName_Prefix_Basic) );
    
    //get the BDT cut values
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R10X_CellBased",        m_BDTCutValue_R10X_CellBased) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R11X_CellBased",        m_BDTCutValue_R11X_CellBased) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R110_CellBased",        m_BDTCutValue_R110_CellBased) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R1XX_CellBased",        m_BDTCutValue_R1XX_CellBased) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R30X_CellBased",        m_BDTCutValue_R30X_CellBased) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R3XX_CellBased",        m_BDTCutValue_R3XX_CellBased) );
    
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R10X_eflowRec",        m_BDTCutValue_R10X_eflowRec) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R11X_eflowRec",        m_BDTCutValue_R11X_eflowRec) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R110_eflowRec",        m_BDTCutValue_R110_eflowRec) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R1XX_eflowRec",        m_BDTCutValue_R1XX_eflowRec) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R30X_eflowRec",        m_BDTCutValue_R30X_eflowRec) );
    CHECK( m_Tool_InformationStore->getInfo_Double("DecayModeDeterminator_BDTCutValue_R3XX_eflowRec",        m_BDTCutValue_R3XX_eflowRec) );
    
    return StatusCode::SUCCESS;
}



// StatusCode PanTau::Tool_DecayModeDeterminator::finalize() {
//     StatusCode sc = AlgTool::finalize();
//     return sc;
// }





StatusCode PanTau::Tool_DecayModeDeterminator::determineDecayMode(PanTau::PanTauSeed* inSeed) const {
    
    
    ATH_MSG_DEBUG("determinateDecayMode called for inSeed at " << inSeed);
    std::string         inAlgName   = inSeed->getNameInputAlgorithm();
    PanTau::TauFeature* features    = inSeed->getFeatures();
    
    
    //check for invalid input seed
    bool    noValidInput        = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoValidInputTau);
    bool    noAnyConstituents   = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoConstituentsAtAll);
    bool    noSelConstituents   = inSeed->isOfTechnicalQuality(PanTau::PanTauSeed::t_NoSelectedConstituents);
    bool    invalidForDecayMode = (noValidInput || noAnyConstituents || noSelConstituents);
    if(invalidForDecayMode) {
        ATH_MSG_DEBUG("Seed has no constituents assigned and/or input seed has no substructure info - use default mode for this");
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_RecoMode", PanTau::TauClassificationTypes::Reco_InvalidInSeed);
        return StatusCode::SUCCESS;
    }
    
    
    //get the mode as it was determined by the subalgorithm
    int decayMode_SubAlg = inSeed->getDecayModeBySubAlg();
    int decayMode_PanTau = PanTau::PanTauSeed::t_Mode_NotSet;
    
    //and calc the number of additional neutrals, not tagged as pi0
    int nCharged_SubAlg     = inSeed->getNumberOfConstituentsOfType(PanTau::TauConstituent::t_Charged);
    int nPi0Neut_SubAlg     = inSeed->getNumberOfConstituentsOfType(PanTau::TauConstituent::t_Pi0Neut);
    int nNeutrals_SubAlg    = inSeed->getNumberOfConstituentsOfType(PanTau::TauConstituent::t_Neutral);
    int AdditionalNeutrals = nNeutrals_SubAlg - nPi0Neut_SubAlg;
    
    double BDTCutValue_R10X = 0.;
    double BDTCutValue_R110 = 0.;
    double BDTCutValue_R11X = 0.;
    double BDTCutValue_R1XX = 0.;
    double BDTCutValue_R30X = 0.;
    double BDTCutValue_R3XX = 0.;
    
    if(inAlgName == "CellBased") {
        BDTCutValue_R10X = m_BDTCutValue_R10X_CellBased;
        BDTCutValue_R110 = m_BDTCutValue_R110_CellBased;
        BDTCutValue_R11X = m_BDTCutValue_R11X_CellBased;
        BDTCutValue_R1XX = m_BDTCutValue_R1XX_CellBased;
        BDTCutValue_R30X = m_BDTCutValue_R30X_CellBased;
        BDTCutValue_R3XX = m_BDTCutValue_R3XX_CellBased;
    }
    
    if(inAlgName == "eflowRec") {
        BDTCutValue_R10X = m_BDTCutValue_R10X_eflowRec;
        BDTCutValue_R110 = m_BDTCutValue_R110_eflowRec;
        BDTCutValue_R11X = m_BDTCutValue_R11X_eflowRec;
        BDTCutValue_R1XX = m_BDTCutValue_R1XX_eflowRec;
        BDTCutValue_R30X = m_BDTCutValue_R30X_eflowRec;
        BDTCutValue_R3XX = m_BDTCutValue_R3XX_eflowRec;
    }
    
    //based on the subAlg decay mode, pass to corresponding PanTau BDT...
    ATH_MSG_DEBUG("The subalg mode is set by nChrg/nPi0Neut/nAddNeut = " << nCharged_SubAlg << " / " << nPi0Neut_SubAlg << " / " << AdditionalNeutrals << " to " << decayMode_SubAlg << " - " << PanTau::PanTauSeed::getDecayModeName(decayMode_SubAlg));
    
    double          bdtResponse = -5;
    DecayModeTest   bdtTest     = t_UnknownTest;
    // 1p0n mode
    if(decayMode_SubAlg == PanTauSeed::t_Mode_1p0n) {
        
        
        //1 prong, and no other objects at all -> use 1p0n
        if(AdditionalNeutrals == 0) {
            ATH_MSG_DEBUG("Set 1p0n because there is only one object");
            decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1p0n;
            
            
        //if there are neutrals which are not pi0-tagged, check BDT to maybe set 1p1n
        } else {
            bool    isOK        = false;
            bdtResponse = m_Tool_ModeDiscriminator_1p0n_vs_1p1n->getResponse(inSeed, isOK);
            bdtTest     = t_1p0n_vs_1p1n;
            if(isOK == false) {
                ATH_MSG_DEBUG("WARNING: problems getting bdt response for 1p0n + X-neutrals. use subAlg mode");
                decayMode_PanTau = decayMode_SubAlg;
            } else {
                if(bdtResponse >  BDTCutValue_R10X) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1p0n;
                if(bdtResponse <= BDTCutValue_R10X) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1p1n;
                ATH_MSG_DEBUG("R10X: Response is: " << bdtResponse << "  Use 1p0n if > " << BDTCutValue_R10X << " -- decision: " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau) );
            }
        }
        
        ATH_MSG_DEBUG("SubAlgMode " << decayMode_SubAlg << ". Chrg/Neut: " << nCharged_SubAlg << " / " << nPi0Neut_SubAlg << ". Mode set to: " << decayMode_PanTau << " - " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau));
        
    }//end 1p0n
    
    
    
    //1p1n mode
    else if (decayMode_SubAlg == PanTau::PanTauSeed::t_Mode_1p1n) {
        
        
        //1prong, 1pi0 and no other neutrals. check 1p0n-vs-1p1n
        if(AdditionalNeutrals == 0) {
            bool    isOK        = false;
            bdtResponse = m_Tool_ModeDiscriminator_1p0n_vs_1p1n->getResponse(inSeed, isOK);
            bdtTest     = t_1p0n_vs_1p1n;
            if(isOK == false) {
                ATH_MSG_DEBUG("WARNING problems getting bdt response for 1p1n + 0-neutrals. use subAlg mode");
                decayMode_PanTau = decayMode_SubAlg;
            } else {
                if(bdtResponse <= BDTCutValue_R110) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1p1n;
                if(bdtResponse >  BDTCutValue_R110) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1p0n;
                ATH_MSG_DEBUG("R110: Response is: " << bdtResponse << "  Use 1p0n if > " << BDTCutValue_R110 << " -- decision: " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau) );
            }
        
        
        //1prong, 1pi0 and additional neutrals. check 1p1n vs 1pXn
        } else {
            //NOTE: May investigate chain of decisions here: first check 1p0n-1p1n, and then (in case of 1p1n) 1p1n-1pXn
            bool    isOK        = false;
            bdtResponse = m_Tool_ModeDiscriminator_1p1n_vs_1pXn->getResponse(inSeed, isOK);
            bdtTest     = t_1p1n_vs_1pXn;
            if(isOK == false) {
                ATH_MSG_DEBUG("WARNING problems getting bdt response for 1p1n + X-neutrals. use subAlg mode");
                decayMode_PanTau = decayMode_SubAlg;
            } else {
                if(bdtResponse >  BDTCutValue_R11X) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1p1n;
                if(bdtResponse <= BDTCutValue_R11X) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1pXn;
                ATH_MSG_DEBUG("R11X: Response is: " << bdtResponse << "  Use 1p1n if > " << BDTCutValue_R11X << " -- decision: " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau) );
            }
        }
        
        ATH_MSG_DEBUG("SubAlgMode " << decayMode_SubAlg << ". Chrg/Neut: " << nCharged_SubAlg << " / " << nPi0Neut_SubAlg << ". Mode set to: " << decayMode_PanTau << " - " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau));
    }//end 1p1n
    
    
    
    //1pXn mode
    else if (decayMode_SubAlg == PanTau::PanTauSeed::t_Mode_1pXn) {
        
        //1prong, Xpi0. check 1p1n-vs-1pXn
        bool    isOK        = false;
        bdtResponse = m_Tool_ModeDiscriminator_1p1n_vs_1pXn->getResponse(inSeed, isOK);
        bdtTest     = t_1p1n_vs_1pXn;
        if(isOK == false) {
            ATH_MSG_DEBUG("WARNING problems getting bdt response for 1p1n + X-neutrals. use subAlg mode");
            decayMode_PanTau = decayMode_SubAlg;
        } else {
            if(bdtResponse >  BDTCutValue_R1XX) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1p1n;
            if(bdtResponse <= BDTCutValue_R1XX) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1pXn;
            ATH_MSG_DEBUG("R1XX: Response is: " << bdtResponse << "  Use 1p1n if > " << BDTCutValue_R1XX << " -- decision: " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau) );
        }
        
        ATH_MSG_DEBUG("SubAlgMode " << decayMode_SubAlg << ". Chrg/Neut: " << nCharged_SubAlg << " / " << nPi0Neut_SubAlg << ". Mode set to: " << decayMode_PanTau << " - " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau));
    }//end 1pXn
    
    
    
    //3p0n mode
    else if (decayMode_SubAlg == PanTau::PanTauSeed::t_Mode_3p0n) {
        
        //no additional neutrals. 
        if(AdditionalNeutrals == 0) {
            ATH_MSG_DEBUG("Set 3p0n because there are only charged objects");
            decayMode_PanTau = PanTau::PanTauSeed::t_Mode_3p0n;
            
            
        //additional neutrals
        } else {
            
            bool    isOK        = false;
            bdtResponse = m_Tool_ModeDiscriminator_3p0n_vs_3pXn->getResponse(inSeed, isOK);
            bdtTest     = t_3p0n_vs_3pXn;
            if(isOK == false) {
                ATH_MSG_DEBUG("WARNING problems getting bdt response for 3p0n + X-neutrals. use subAlg mode");
                decayMode_PanTau = decayMode_SubAlg;
            } else {
                if(bdtResponse >  BDTCutValue_R30X) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_3p0n;
                if(bdtResponse <= BDTCutValue_R30X) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_3pXn;
                ATH_MSG_DEBUG("R30X: Response is: " << bdtResponse << "  Use 3p0n if > " << BDTCutValue_R30X << " -- decision: " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau) );
            }
            
        }
        
        ATH_MSG_DEBUG("SubAlgMode " << decayMode_SubAlg << ". Chrg/Neut: " << nCharged_SubAlg << " / " << nPi0Neut_SubAlg << ". Mode set to: " << decayMode_PanTau << " - " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau));
    }//end 3p0n
    
    
    
    //3pXn mode
    else if (decayMode_SubAlg == PanTau::PanTauSeed::t_Mode_3pXn) {
        
        //3prong, Xpi0. check 3p0n-vs-3pXn
        bool    isOK        = false;
        bdtResponse = m_Tool_ModeDiscriminator_3p0n_vs_3pXn->getResponse(inSeed, isOK);
        bdtTest     = t_3p0n_vs_3pXn;
        if(isOK == false) {
            ATH_MSG_DEBUG("WARNING problems getting bdt response for 3p0n + X-neutrals. use subAlg mode");
            decayMode_PanTau = decayMode_SubAlg;
        } else {
            if(bdtResponse >  BDTCutValue_R3XX) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_3p0n;
            if(bdtResponse <= BDTCutValue_R3XX) decayMode_PanTau = PanTau::PanTauSeed::t_Mode_3pXn;
            ATH_MSG_DEBUG("R3XX: Response is: " << bdtResponse << "  Use 3p0n if > " << BDTCutValue_R3XX << " -- decision: " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau) );
        }
        
        ATH_MSG_DEBUG("SubAlgMode " << decayMode_SubAlg << ". Chrg/Neut: " << nCharged_SubAlg << " / " << nPi0Neut_SubAlg << ". Mode set to: " << decayMode_PanTau << " - " << PanTau::PanTauSeed::getDecayModeName(decayMode_PanTau));
    } //end 3pXn
    
    
    
    //it's none of 1p0n, 1p1n, 1pXn, 3p0n, 3pXn -> set other mode
    else {
        ATH_MSG_DEBUG("WARNING SubAlg mode of tau is not known. set the 'other' mode");
        decayMode_PanTau = PanTau::PanTauSeed::t_Mode_Other;
        bdtResponse = -5;
        bdtTest     = t_UnknownTest;
    }

    if(inAlgName != "eflowRec" && inAlgName != "CellBased") {
        ATH_MSG_WARNING("Name of input algorithm not known - use mode from sub alg!");
        decayMode_PanTau = decayMode_SubAlg;
        bdtResponse = -5;
        bdtTest     = t_UnknownTest;
    }
    
    //post decay mode -HACK:
    // if cell based counted 1 charged, 1 pi0neut, and number of hits in EM1 for the pi0neut is 3 or larger, set 1pXn;
    if(inAlgName == "CellBased") {
        if(nCharged_SubAlg == 1 && nPi0Neut_SubAlg == 1) {
            ATH_MSG_DEBUG("Potential to apply hack: In case of CellBased 1p1n with >2 photons in shots -> 1pXn");
            //check for shots in EM1
            bool isOK = false;
            PanTau::TauConstituent* pi0Neut = inSeed->getConstituentsOfType(PanTau::TauConstituent::t_Pi0Neut, isOK).at(0);
            if(isOK == true) {
                double nPhotons = 0;
                std::vector<PanTau::TauConstituent*> shots = pi0Neut->getShots();
                ATH_MSG_DEBUG("There are " << shots.size() << " shots in the pi0 Neutral");
                for(unsigned int iShot=0; iShot<shots.size(); iShot++) {
                    nPhotons = nPhotons + (double)(shots.at(iShot)->getNPhotonsInShot());
                }
                ATH_MSG_DEBUG("Counted " << nPhotons << " photons in the pi0 neutral");
                if(nPhotons > 2) {
                    decayMode_SubAlg = PanTau::PanTauSeed::t_Mode_1pXn;
                    decayMode_PanTau = PanTau::PanTauSeed::t_Mode_1pXn;
                    inSeed->setModeHackedByCellBased(true);
                }
                
            //end check for pi0 neutrals were found
            } else {
                ATH_MSG_DEBUG("Could not find expected CellBased neutral -> don't hack the pantau mode");
            }
        } //end hack check for 1p1n
    }//end hack check for cell based
    
    //update mode of seed and store in features
    ATH_MSG_DEBUG("Storing decay mode in tau and features: SubAlg/PanTau = " << (double)decayMode_SubAlg << " / " << (double)decayMode_PanTau);
    inSeed->setDecayModeByPanTau(decayMode_PanTau);
    features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_RecoMode", (double)decayMode_SubAlg);
    features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_RecoMode_PanTau", (double)decayMode_PanTau);
    
    if(bdtTest == t_1p0n_vs_1p1n) {
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p0n_vs_1p1n", bdtResponse);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p1n_vs_1pXn", -5.);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_3p0n_vs_3pXn", -5.);
    }
    if(bdtTest == t_1p1n_vs_1pXn) {
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p0n_vs_1p1n", -5.);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p1n_vs_1pXn", bdtResponse);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_3p0n_vs_3pXn", -5.);
    }
    if(bdtTest == t_3p0n_vs_3pXn) {
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p0n_vs_1p1n", -5.);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p1n_vs_1pXn", -5.);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_3p0n_vs_3pXn", bdtResponse);
    }
    if(bdtTest == t_UnknownTest) {
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p0n_vs_1p1n", -5.);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_1p1n_vs_1pXn", -5.);
        features->addFeature(inAlgName + "_" + m_varTypeName_Prefix_Basic + "_BDTValue_3p0n_vs_3pXn", -5.);
    }
    
    return StatusCode::SUCCESS;
    
}
























