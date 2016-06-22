/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/Helpers.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"
    
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h" 
#include "PATInterfaces/SystematicsUtil.h"

int main (int argc, char* argv[])
{
    bool isLargeR = false;
    if (argc == 2)
        isLargeR = TString(argv[1]).EqualTo("true",TString::kIgnoreCase) || TString(argv[1]).EqualTo("largeR",TString::kIgnoreCase) || TString(argv[1]).EqualTo("large-R",TString::kIgnoreCase);
    
    StatusCode::enableFailure();

    JetUncertaintiesTool* tool = new JetUncertaintiesTool("JESprov");
    //tool->msg().setLevel(MSG::DEBUG);
    if (!isLargeR && tool->setProperty("JetDefinition","AntiKt4EMTopo").isFailure())
        exit(1);
    if (isLargeR && tool->setProperty("JetDefinition","AntiKt10LCTopoTrimmedPtFrac5SmallR20").isFailure())
        exit(1);

    if (tool->setProperty("MCType","MC15").isFailure())
        exit(1);
    
    //if (!isLargeR && tool->setProperty("ConfigFile","JES_2015/Prerec/PrerecJES2015_AllNuisanceParameters_25ns.config").isFailure())
    if (!isLargeR && tool->setProperty("ConfigFile","JES_2015/Moriond2016/JES2015_AllNuisanceParameters.config").isFailure())
        exit(1);
    //if (isLargeR && tool->setProperty("ConfigFile","UJ_2015/Prerec/Prerec2015_WZTagging.config").isFailure())
    if (isLargeR && tool->setProperty("ConfigFile","UJ_2015/Moriond2016/UJ2015_WZTagging_medium.config").isFailure())
        exit(1);

    if (tool->setScaleToGeV().isFailure())
        exit(1);
    if (tool->initialize().isFailure())
        exit(1);
    

    // Build a jet container and a jet for us to manipulate later
    xAOD::TEvent event;
    xAOD::TStore store;
    xAOD::JetContainer* jets = new xAOD::JetContainer();
    jets->setStore(new xAOD::JetAuxContainer());
    jets->push_back(new xAOD::Jet());
    xAOD::Jet* jet = jets->at(0);
        
    // Build an EventInfo object for us to manipulate later
    xAOD::EventInfoContainer* eInfos = new xAOD::EventInfoContainer();
    eInfos->setStore(new xAOD::EventInfoAuxContainer());
    eInfos->push_back(new xAOD::EventInfo());
    xAOD::EventInfo* eInfo = eInfos->at(0);
        

    
    // General tests

    // Print the list of affecting systematics
    //CP::SystematicSet affectingSystematics = tool->affectingSystematics();
    //for (CP::SystematicSet::iterator iter = affectingSystematics.begin(); iter != affectingSystematics.end(); ++iter)
    //    std::cout << iter->basename() << std::endl;


    // Get the active categories and print components per category
    std::vector<std::string> categories = tool->getComponentCategories();
    for (size_t iCat = 0; iCat < categories.size(); ++iCat)
    {
        printf("Category %s:\n",categories.at(iCat).c_str());
        std::vector<std::string> compInCategory = tool->getComponentNamesInCategory(categories.at(iCat));
        for (size_t iComp = 0; iComp < compInCategory.size(); ++iComp)
            printf("\t\t%s\n",compInCategory.at(iComp).c_str());
    }

    // Get the list of recommended systematics
    // https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/SoftwareTutorialxAODAnalysisInROOT
    const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
    std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recommendedSystematics); 
    printf("\nRecommended systematics (%zux2+1=%zu%s):\n",recommendedSystematics.size(),sysList.size(),recommendedSystematics.size()*2+1==sysList.size()?"":" FAILED");
    for (size_t iSyst = 0; iSyst < sysList.size(); ++iSyst)
        if (sysList.at(iSyst).name() == "")
            printf("\t%40s\n","NOMINAL");
        else
            printf("\t%40s%s",sysList.at(iSyst).name().c_str(),iSyst%2==0?"\n":"");
    printf("\n");


    // small-R tests
    if (!isLargeR)
    {

        // Confirm that the systematic recognition is working
        printf("Recognizes JET_Flavor_Response__continuous: %s\n",tool->isAffectedBySystematic(CP::SystematicVariation("JET_Flavor_Response__continuous"))?"true":"false");
        printf("Recognizes JET_Flavor_Response: %s\n",tool->isAffectedBySystematic(CP::SystematicVariation("JET_Flavor_Response"))?"true":"false");

        //printf("Full: %s, base: %s\n",CP::SystematicVariation("JET_Flavor_Response__continuous",-1.5).name().c_str(),CP::SystematicVariation("JET_Flavor_Response__continuous",-1.5).basename().c_str());
        //printf("Full: %s, base: %s\n",CP::SystematicVariation("JET_Flavor_Response",-1.5).name().c_str(),CP::SystematicVariation("JET_Flavor_Response",-1.5).basename().c_str());
        
        
        // Add Nsegments information
        // 25 segments is about average for jets receiving a correction
        SG::AuxElement::Accessor<int> Nsegments("GhostMuonSegmentCount");
        SG::AuxElement::Accessor<char> IsBjet("IsBjet");
        Nsegments(*jet) = 0;
        IsBjet(*jet) = false;
        
        // Add mu and NPV information
        SG::AuxElement::Accessor<float> mu("averageInteractionsPerCrossing");
        SG::AuxElement::Accessor<float> NPV("NPV");
        mu(*eInfo)  = 22;
        NPV(*eInfo) = 11;
        
        
        
        // Specify which systematics to vary
        printf("Trying (Flavor_Response)x(-1.5) + (PunchThrough_MC15)x(+1.5)\n");
        CP::SystematicSet* syst = new CP::SystematicSet();
        syst->insert(CP::SystematicVariation("JET_Flavor_Response",-1.5));
        syst->insert(CP::SystematicVariation("JET_PunchThrough_MC15",+1.5));
        if (tool->applySystematicVariation(*syst) != CP::SystematicCode::Ok)
            exit(2);
        
        // Try below eta boundary of flavour
        jet->setJetP4(xAOD::JetFourMom_t(100,-4.5,0.,0.));
        printf("%20s: (pT,eta) = (%.0f,%.1f)","below eta boundary",jet->pt(),jet->eta());
        if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            exit(3);
        printf(" --> (%.0f,%.1f)\n",jet->pt(),jet->eta());

        // Now try with opposite shift
        delete syst;
        syst = new CP::SystematicSet();
        syst->insert(CP::SystematicVariation("JET_Flavor_Response",+1.5));
        syst->insert(CP::SystematicVariation("JET_PunchThrough_MC15",+1.5));
        if (tool->applySystematicVariation(*syst) != CP::SystematicCode::Ok)
            exit(2);
        
        // Re-try below eta boundary of flavour
        jet->setJetP4(xAOD::JetFourMom_t(100,-4.5,0.,0.));
        printf("%20s: (pT,eta) = (%.0f,%.1f)","(Flavor)x(+1.5)",jet->pt(),jet->eta());
        if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            exit(3);
        printf(" --> (%.0f,%.1f)\n",jet->pt(),jet->eta());
        
        // Try exact eta boundary of flavour
        jet->setJetP4(xAOD::JetFourMom_t(100,-4.4,0.,0.));
        printf("%20s: (pT,eta) = (%.0f,%.1f)","exact eta boundary",jet->pt(),jet->eta());
        if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            exit(3);
        printf(" --> (%.0f,%.1f)\n",jet->pt(),jet->eta());
        
        // Try Nseg value without punch-through
        jet->setJetP4(xAOD::JetFourMom_t(500,1.7,0.,0.));
        printf("%20s: (pT,eta) = (%.0f,%.1f)","Nsegments=0",jet->pt(),jet->eta());
        if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            exit(3);
        printf(" --> (%.0f,%.1f)\n",jet->pt(),jet->eta());

        // Try Nseg value with punch-through
        jet->setJetP4(xAOD::JetFourMom_t(500,1.7,0.,0.));
        Nsegments(*jet) = 20;
        printf("%20s: (pT,eta) = (%.0f,%.1f)","Nsegments=20",jet->pt(),jet->eta());
        if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            exit(3);
        Nsegments(*jet) = 0;
        printf(" --> (%.0f,%.1f)\n",jet->pt(),jet->eta());
    }
    // large-R tests
    else
    {
        // Confirm that the systematic recognition is working
        printf("Recognizes JET_Rtrk_Baseline__continuous: %s\n",tool->isAffectedBySystematic(CP::SystematicVariation("JET_Rtrk_Baseline__continuous"))?"true":"false");
        printf("Recognizes JET_Rtrk_Baseline: %s\n",tool->isAffectedBySystematic(CP::SystematicVariation("JET_Rtrk_Baseline"))?"true":"false");
        //printf("Recognizes JET_WZ_Run1__continuous: %s\n",tool->isAffectedBySystematic(CP::SystematicVariation("JET_WZ_Run1__continuous"))?"true":"false");
        //printf("Recognizes JET_WZ_Run1: %s\n",tool->isAffectedBySystematic(CP::SystematicVariation("JET_WZ_Run1"))?"true":"false");
        
        // Specify which systematics to vary
        printf("Trying (Rtrk_Baseline)x(-1.5) + (Rtrk_Modelling)x(+1.5)\n");
        CP::SystematicSet* syst = new CP::SystematicSet();
        syst->insert(CP::SystematicVariation("JET_Rtrk_Baseline",-1.5));
        syst->insert(CP::SystematicVariation("JET_Rtrk_Modelling",+1.5));
        if (tool->applySystematicVariation(*syst) != CP::SystematicCode::Ok)
            exit(2);
        
        // Add D2 information
        // 25 segments is about average for jets receiving a correction
        SG::AuxElement::Accessor<float> accD2("D2");
        accD2(*jet) = 1.;
        
        // Try a normal high pT W/Z boson
        jet->setJetP4(xAOD::JetFourMom_t(1000,1.0,0.,85));
        printf("%20s: (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f)","Normal W/Z jet",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
        if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            exit(3);
        printf(" --> (%.0f,%.1f,%.1f,%.0f,%0.1f)\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));

        // Try outside of validity
        //jet->setJetP4(xAOD::JetFourMom_t(4000,1.0,0.,85));
        //printf("%20s: (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f)","Too high of pT",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
        //if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
        //    exit(3);
        //printf(" --> (%.0f,%.1f,%.1f,%.0f,%0.1f)\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
        
        //jet->setJetP4(xAOD::JetFourMom_t(2000,3.0,0.,85));
        //printf("%20s: (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f)","Too high of eta",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
        //if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
        //    exit(3);
        //printf(" --> (%.0f,%.1f,%.1f,%.0f,%0.1f)\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));

        
        // Try validity borders
        printf("\n");
        jet->setJetP4(xAOD::JetFourMom_t(3000,2,0,0));
        printf("Validity of (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f) : %s\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet), (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::Ok) ? "ok" : (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::OutOfValidityRange) ? "invalid" : "ERROR");
        jet->setJetP4(xAOD::JetFourMom_t(2999,2,0,0));
        printf("Validity of (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f) : %s\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet), (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::Ok) ? "ok" : (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::OutOfValidityRange) ? "invalid" : "ERROR");
        jet->setJetP4(xAOD::JetFourMom_t(2999,1.99,0,0));
        printf("Validity of (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f) : %s\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet), (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::Ok) ? "ok" : (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::OutOfValidityRange) ? "invalid" : "ERROR");
        jet->setJetP4(xAOD::JetFourMom_t(3000,1.99,0,0));
        printf("Validity of (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f) : %s\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet), (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::Ok) ? "ok" : (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::OutOfValidityRange) ? "invalid" : "ERROR");
        jet->setJetP4(xAOD::JetFourMom_t(500,0,0,0.79*500));
        printf("Validity of (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f) : %s\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet), (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::Ok) ? "ok" : (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::OutOfValidityRange) ? "invalid" : "ERROR");
        jet->setJetP4(xAOD::JetFourMom_t(500,0,0,0.81*500));
        printf("Validity of (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f) : %s\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet), (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::Ok) ? "ok" : (tool->applyCorrection(*jet,*eInfo) == CP::CorrectionCode::OutOfValidityRange) ? "invalid" : "ERROR");


        // Border of validity
        //jet->setJetP4(xAOD::JetFourMom_t(3000,2,0,0));
        //printf("%20s: (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f)","Validity borders 1",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
        //if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
        //    exit(3);
        //printf(" --> (%.0f,%.1f,%.1f,%.0f,%0.1f)\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
        //
        //jet->setJetP4(xAOD::JetFourMom_t(150,0,0,150));
        //printf("%20s: (pT,eta,phi,m,D2) = (%.0f,%.1f,%.1f,%.0f,%.1f)","Validity borders 2",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
        //if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
        //    exit(3);
        //printf(" --> (%.0f,%.1f,%.1f,%.0f,%0.1f)\n",jet->pt(),jet->eta(),jet->phi(),jet->m(),accD2(*jet));
    }

    return 0;
}
