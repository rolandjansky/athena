#include <iostream>
#include <string>
#include <fstream>

#include "TROOT.h"
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TKey.h"
#include "TFolder.h"
#include "TDirectory.h"
void RecExOnline_Test_Obj_His_RTT() 
{
   
   TFile *file = new TFile("Monitor.root");

//    TH1F *h1_AK4LCTTopoJetsPt= (TH1F*)gDirectory->FindObjectAny("Jets/AntiKt4LCTTopoJets/pt");
      //  TH1F *h1_AK4LCTTopoJetsPt= (TH1F*)gDirectory->FindObjectAny("run_284484/Jets/AntiKt4EMTopoJets/pt");
     
      TDirectory* fjet =(TDirectory*)file->FindObjectAny("AntiKt4EMTopoJets"); 
      TH1F *h1_JetsPt= (TH1F*)fjet->FindObjectAny("pt");
      
      TDirectory* ftau =(TDirectory*)file->FindObjectAny("Tau"); 
      TH1F *h2_TauPt= (TH1F*)ftau->FindObjectAny("tauEt");
         

    TCanvas *c1 = new TCanvas("c1", " Canvas", 400, 300);
 
    h1_JetsPt->Draw("HistE");
    c1->Print("AntiKt4EMTopoPt.png");
       
    TCanvas *c2 = new TCanvas("c21", "Canvas 2", 400, 300);
 
    h2_TauPt->Draw("HistE");
    c2->Print("TauPt.png");


}

