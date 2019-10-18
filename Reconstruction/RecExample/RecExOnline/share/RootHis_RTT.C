#include <iostream>
#include <string>
#include <fstream>

#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TKey.h"

void RootHis_RTT() 
{
   
   TFile *file = new TFile("Monitor.root");

//    TH1F *h1_AK4LCTTopoJetsPt= (TH1F*)gDirectory->FindObjectAny("Jets/AntiKt4LCTTopoJets/pt");
        TH1F *h1_AK4LCTTopoJetsPt= (TH1F*)gDirectory->FindObjectAny("pt");

    TCanvas *c1 = new TCanvas("c1", "GIF Canvas", 400, 300);
    h1_AK4LCTTopoJetsPt->Draw("HistE");
    c1->Print("AK4LCTTopoJetsPt.png");
       


}

