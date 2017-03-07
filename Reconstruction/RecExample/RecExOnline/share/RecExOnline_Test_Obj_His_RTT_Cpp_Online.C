#include <cstdlib>
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


#define nDQBKG 1
#define nTauVar 4
void RecExOnline_Test_Obj_His_RTT_Cpp_Online() 
{
   
   TFile *file = new TFile("monitoring.root");

      //  For Jet variables  
      
      vector<string> DQBKGstring;
      
      DQBKGstring.push_back("m_NumVertex");  

      TH1F *h_DQBKG[nDQBKG];
      TCanvas *cjet[nDQBKG];  
      
              
      string DQBKGType;
      DQBKGType="BackgroundMon";
      TDirectory* fDQBKG =(TDirectory*)file->FindObjectAny(DQBKGType.data()); 
    
      for(int n=0;n<nDQBKG;n++)
      {
        h_DQBKG[n]=(TH1F*)fDQBKG->FindObjectAny(DQBKGstring[n].data());
        cjet[n]=new TCanvas(Form("cjet_%d",n),Form("Canvas_%d",n), 400, 300);
        cjet[n]->cd();
        h_DQBKG[n]->Draw("HistE");
        string savehis;
        savehis=Form("DQBKG_%s_%s.png",DQBKGType.data(),DQBKGstring[n].data()); 
        cjet[n]->Print(savehis.data());
        cout<<"<td><img src=\""<<savehis<<"\" alt=\""<<savehis<<"\" height=\"250\"></td>"<<endl;
      }
      

}

