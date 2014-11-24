/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//g++ -Wall -g -o StressTest.app StressTest.C -DJES_STANDALONE -I../../ `root-config --cflags --glibs` -L../../StandAlone/ -lJetUncertainties 

#include <stdio.h>
#include <stdlib.h>

#include <vector>
using std::vector;

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "../../JetUncertainties/MultijetJESUncertaintyProvider.h"

int main (int argc, char* argv[])
{
    TString mjConfig = "JES_HighMu/MultijetJES_HighMu.config";
    TString blConfig = "JES_HighMu/JESUncertaintyHighMu_Aug2013_WithNonClosure.config";
    TString jetType  = "AntiKt4LCTopo";
    TString mcType   = "HighMu";
    TString share    = "../../share";
    MultijetJESUncertaintyProvider* mj  = new MultijetJESUncertaintyProvider(mjConfig,blConfig,jetType,mcType,share);

    TString testFileName="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/STRESSTEST/stress_test.root";
    TFile* stFile = new TFile(testFileName,"READ");
    if (!stFile || stFile->IsZombie())
    {
        printf("ERROR: Could not open stress test file.\n");
        exit(1);
    }
    TTree* stTree = dynamic_cast<TTree*>(stFile->Get("physics"));
    if (!stTree)
    {
        printf("ERROR: Could not get the tree in the stress test file.\n");
        exit(2);
    }

    vector<float>* Vpt  = 0;
    TBranch*       Bpt  = 0;
    vector<float>* Veta = 0;
    TBranch*       Beta = 0;
    vector<float>* Vphi = 0;
    TBranch*       Bphi = 0;
    vector<float>* VE   = 0;
    TBranch*       BE   = 0;

    stTree->SetBranchAddress("jet_AntiKt4LCTopo_pt", &Vpt, &Bpt);
    stTree->SetBranchAddress("jet_AntiKt4LCTopo_eta",&Veta,&Beta);
    stTree->SetBranchAddress("jet_AntiKt4LCTopo_phi",&Vphi,&Bphi);
    stTree->SetBranchAddress("jet_AntiKt4LCTopo_E",  &VE,  &BE);
    
    const double NPV    = 5;
    const double mu     = 10;
    const bool   isBjet = false;
    const double dr     = 1;

    printf("Event\tjet\tpT\t\teta\t\tunc\n\n");
    for (Long64_t iEvent = 0; iEvent < stTree->GetEntries(); ++iEvent)
    {
        stTree->GetEntry(iEvent);
        for (size_t iJet = 0; iJet < Vpt->size(); ++iJet)
        {
            printf("%lld\t%zu\t%.4e\t%.4e\t%.4e\n",iEvent,iJet,Vpt->at(iJet),Veta->at(iJet),mj->getRelUncert(Vpt->at(iJet),Veta->at(iJet),dr,true,NPV,mu,isBjet));
        }
        printf("\n");
    }

    return 0;
}
