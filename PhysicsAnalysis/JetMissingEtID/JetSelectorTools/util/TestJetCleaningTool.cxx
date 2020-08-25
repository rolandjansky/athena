/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifdef ROOTCORE
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

#include "JetSelectorTools/JetCleaningTool.h"
#include "JetSelectorTools/Helpers.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "TString.h"
#include "TSystem.h"

const char* parseResult(const asg::AcceptData& result, const char* selection="Cleaning")
{
    return result.getCutResult(selection)==true?"true":"false";
}

void setJetToDefault(xAOD::Jet& jet)
{
    jet.setJetP4(xAOD::JetFourMom_t(100.e3,0.,0.,0.));

    jet.setAttribute<float>(xAOD::JetAttribute::EMFrac,0.1);
    jet.setAttribute<float>(xAOD::JetAttribute::HECFrac,0.1);
    jet.setAttribute<float>(xAOD::JetAttribute::LArQuality,0.1);
    jet.setAttribute<float>(xAOD::JetAttribute::HECQuality,0.1);
    jet.setAttribute<float>(xAOD::JetAttribute::FracSamplingMax,0.7);
    jet.setAttribute<float>(xAOD::JetAttribute::NegativeE,0);
    jet.setAttribute<float>(xAOD::JetAttribute::AverageLArQF,0);
    jet.setAttribute<int>(xAOD::JetAttribute::FracSamplingMaxIndex,12);
    
    // for sumPtTrk
    std::vector<float> sumPtTrkvec;
    sumPtTrkvec.push_back(50.e3);
    jet.setAttribute(xAOD::JetAttribute::SumPtTrkPt500,sumPtTrkvec);

    
    // for EnergyPerSampling
    const float jetE = jet.e();
    std::vector<float> ePerS;
    ePerS.push_back( 0   * jetE ); //  0 : PreSamplerB
    ePerS.push_back( 0.1 * jetE ); //  1 : EMB1
    ePerS.push_back( 0.1 * jetE ); //  2 : EMB2
    ePerS.push_back( 0.1 * jetE ); //  3 : EMB3
    ePerS.push_back( 0   * jetE ); //  4 : PreSamplerE
    ePerS.push_back( 0   * jetE ); //  5 : EME1
    ePerS.push_back( 0   * jetE ); //  6 : EME2
    ePerS.push_back( 0   * jetE ); //  7 : EME3
    ePerS.push_back( 0   * jetE ); //  8 : HEC0
    ePerS.push_back( 0   * jetE ); //  9 : HEC1
    ePerS.push_back( 0   * jetE ); // 10 : HEC2
    ePerS.push_back( 0   * jetE ); // 11 : HEC3
    ePerS.push_back( 0.7 * jetE ); // 12 : TileBar0
    ePerS.push_back( 0   * jetE ); // 13 : TileBar1
    ePerS.push_back( 0   * jetE ); // 14 : TileBar2
    ePerS.push_back( 0   * jetE ); // 15 : TileGap1
    ePerS.push_back( 0   * jetE ); // 16 : TileGap2
    ePerS.push_back( 0   * jetE ); // 17 : TileGap3
    ePerS.push_back( 0   * jetE ); // 18 : TileExt0
    ePerS.push_back( 0   * jetE ); // 19 : TileExt1
    ePerS.push_back( 0   * jetE ); // 20 : TileExt2
    ePerS.push_back( 0   * jetE ); // 21 : FCAL0
    ePerS.push_back( 0   * jetE ); // 22 : FCAL1
    ePerS.push_back( 0   * jetE ); // 23 : FCAL2
    jet.setAttribute("EnergyPerSampling",ePerS);
}


int main (int argc, char* argv[])
{
    // Arguments and default values
    TString cutLevel = "LooseBad";
    TString doUgly   = "true";
    TString hotCells = Form("%s/data/JetSelectorTools/HotCells2012.config",gSystem->Getenv("ROOTCOREBIN"));
    if (argc > 1)
        cutLevel = argv[1];
    if (argc > 2)
        doUgly   = argv[2];
    if (argc > 3)
        hotCells = argv[3];

    // Create and set the cleaning tool
    JetCleaningTool* tool = new JetCleaningTool("MyCleaningTool");
    if (tool->setProperty("CutLevel",cutLevel.Data()).isFailure())
    {
        printf("Failed to set CutLevel\n");
        exit(1);
    }
    if (tool->setProperty("DoUgly",doUgly.EqualTo("true",TString::kIgnoreCase)?true:false).isFailure())
    {
        printf("Failed to set DoUgly\n");
        exit(2);
    }
    if (tool->setProperty("HotCellsFile",hotCells.Data()).isFailure())
    {
        printf("Failed to set HotCellsFile\n");
        exit(3);
    }
    if (tool->initialize().isFailure())
    {
        printf("Failed to initialize the tool\n");
        exit(4);
    }

    // Prepare to build some jets to test the tool
    xAOD::TEvent event;
    xAOD::TStore store;
    xAOD::JetContainer* jets = new xAOD::JetContainer();
    jets->setStore(new xAOD::JetAuxContainer());
    jets->push_back(new xAOD::Jet());
    jets->push_back(new xAOD::Jet());
    xAOD::Jet*        jet = jets->at(0);

    // Confirm that the default jet passes
    setJetToDefault(*jet);
    printf("Test jet    <default pass>  : %s\n",parseResult(tool->accept(*jet)));

    
    // Test the isUgly criterion
    jet->setAttribute<int>(xAOD::JetAttribute::FracSamplingMaxIndex,17);
    printf("Test jet    <isUgly fail>   : %s\n",parseResult(tool->accept(*jet)));

    // Test hot cells criterion
    jet->setAttribute<int>(xAOD::JetAttribute::FracSamplingMaxIndex,13);
    jet->setJetP4(xAOD::JetFourMom_t(100.,-0.15,2.7,0));
    printf("Test jet    <hot cell fail> : %s\n",tool->containsHotCells(*jet,202660)==true?"false":"true");

    



    return 0;
}

