/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include "TFile.h"
#include "TTree.h"

using namespace std;

void TauIDAnalysisExample(string filename)
{
    if (gSystem->Load("libTauDiscriminant.so") != 0)
    {
        cout << "Could not load libTauDiscriminant.so" << endl;
    }
    using namespace TauID;
    
    TFile* file = TFile::Open(filename.c_str());
    if (!file)
    {
        cout << "Could not open " << filename << endl;
        return;
    }
    TTree* tree = (TTree*)file->Get("tauPerf");
    if (!tree)
    {
        cout << "Could not find tree named tauPerf" << endl;
        return;
    }
    MethodBDT* bdt = new MethodBDT("MyBDT");
    float et;
    int numTrack;
    float emRadius;
    float centFrac;
    float trkAvgDist;
    float etOverPtLeadTrk;
    float emFractionAtEMScale;
    float massTrkSys;
    float topoInvMass;
    float trFlightPathSig;
    int numVertices;

    bdt->addVariable("numTrack",&numTrack,"I");
    bdt->addVariable("emRadius",&emRadius,"F");
    bdt->addVariable("centFrac",&centFrac,"F");
    bdt->addVariable("trkAvgDist",&trkAvgDist,"F");
    bdt->addVariable("etOverPtLeadTrk",&etOverPtLeadTrk,"F");
    bdt->addVariable("emFractionAtEMScale",&emFractionAtEMScale,"F");
    bdt->addVariable("massTrkSys",&massTrkSys,"F");
    bdt->addVariable("topoInvMass",&topoInvMass,"F");
    bdt->addVariable("trFlightPathSig",&trFlightPathSig,"F");
    bdt->addVariable("numVertices",&numVertices,"I");
    if (!bdt->build("../share/jet.BDT.bin"))
    {
        cout "Could not build BDT" << endl;
        return;
    }

    for(unsigned int ievent(0); ievent < tree->GetEntries(); ++ievent)
    {
        tree->GetEntry(ievent);
        for (unsigned int itau(0); itau < (*tree)["tau_n"]; ++itau)
        {
            et = (*tree)["tau_Et"][itau];
        }
    }
}
