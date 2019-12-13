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


#define nJetVar 4
#define nTauVar 4
#define nTrackVar 5
void RecExOnline_Test_Obj_His_RTT_Cpp() 
{
   
   TFile *file = new TFile("Monitor.root");

      //  For Jet variables  
      
      vector<string> JetVarstring;
      
      JetVarstring.push_back("pt");  
      JetVarstring.push_back("eta");  
      JetVarstring.push_back("phi");  
      JetVarstring.push_back("M");  


      TH1F *h_Jet[nJetVar];
      TCanvas *cjet[nJetVar];  
      
              
      string JetType;
      JetType="AntiKt4EMTopoJets";
      TDirectory* fjet =(TDirectory*)file->FindObjectAny(JetType.data()); 
    
      for(int n=0;n<nJetVar;n++)
      {
        h_Jet[n]=(TH1F*)fjet->FindObjectAny(JetVarstring[n].data());
        cjet[n]=new TCanvas(Form("cjet_%d",n),Form("JetCanvas_%d",n), 400, 300);
        cjet[n]->cd();
        h_Jet[n]->Draw("HistE");
        string savehis;
        savehis=Form("Jet_%s_%s.png",JetType.data(),JetVarstring[n].data()); 
        cjet[n]->Print(savehis.data());
        cout<<"<td><img src=\""<<savehis<<"\" alt=\""<<savehis<<"\" height=\"250\"></td>"<<endl;
      }
      
      //  For Tau variables  
      
      vector<string> TauVarstring;
      
      TauVarstring.push_back("tauEt");  
      TauVarstring.push_back("tauEta");  
      TauVarstring.push_back("tauPhi");  
      TauVarstring.push_back("tauCharge");  


      TH1F *h_Tau[nTauVar];
      TCanvas *cTau[nTauVar];  
      
              
      string TauType;
      TauType="Tau";
      TDirectory* fTau =(TDirectory*)file->FindObjectAny(TauType.data()); 
    
      for(int n=0;n<nTauVar;n++)
      {
        h_Tau[n]=(TH1F*)fTau->FindObjectAny(TauVarstring[n].data());
        cTau[n]=new TCanvas(Form("cTau_%d",n),Form("TauCanvas_%d",n), 400, 300);
        cTau[n]->cd();
        h_Tau[n]->Draw("HistE");
        string savehis;
        savehis=Form("Tau_%s_%s.png",TauType.data(),TauVarstring[n].data());
        cTau[n]->Print(savehis.data());
        cout<<"<td><img src=\""<<savehis<<"\" alt=\""<<savehis<<"\" height=\"250\"></td>"<<endl;
      }
      
      vector<string> TrackVarstring;
      
      TrackVarstring.push_back("ngtracks");  
      TrackVarstring.push_back("Nhits_per_track");  
      TrackVarstring.push_back("d0");  
      TrackVarstring.push_back("z0");  
      TrackVarstring.push_back("P");

      TH1F *h_Track[nTrackVar];
      TCanvas *cTrack[nTrackVar];  
      
              
      string TrackType;
      TrackType="run_284484/IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks";
      TDirectory* fTrack =(TDirectory*)file->Get(TrackType.data()); 

      string FindTrackDir=fTrack->GetName();
      cout<<"### Sucessfully finding Dir : "<<FindTrackDir<<endl;   
      //fTrack->Paint();  
      fTrack->Print();
      fTrack->ls();

      
      for(int n=0;n<nTrackVar;n++)
      {
        cout<<" Found "<<TrackVarstring[n].data()<<" : "<<fTrack->GetListOfKeys()->Contains(TrackVarstring[n].data())<<endl;
        cTrack[n]=new TCanvas(Form("cTrack_%d",n),Form("TrackCanvas_%d",n), 400, 300);      
        cTrack[n]->cd();
        
        if(fTrack->GetListOfKeys()->Contains(TrackVarstring[n].data()))
        {
          h_Track[n]=(TH1F*)fTrack->Get(TrackVarstring[n].data());
          h_Track[n]->Draw("HistE");
        }
        string savehis;
        savehis=Form("Track_%s_%s.png","TrackTemp",TrackVarstring[n].data());
        cTrack[n]->Print(savehis.data());
        cout<<"<td><img src=\""<<savehis<<"\" alt=\""<<savehis<<"\" height=\"250\"></td>"<<endl;
      }
 
      //  For resonance variables  
      

      TH1F *h_Res[4];
      TCanvas *cRes[4];  
      
      h_Res[0]=(TH1F*)file->Get("run_284484/IDPerfMon/Wenu/NoTriggerSelection/Wenu_transmass_sel");     
      cRes[0]=new TCanvas("wenu","wenu", 400, 300);    
      h_Res[0]->Draw("HistE");            
      cRes[0]->Print("wenu_mt.png");
 
      h_Res[1]=(TH1F*)file->Get("run_284484/IDPerfMon/Zee/NoTriggerSelection/Zee_invmass");     
      cRes[1]=new TCanvas("zee","zee", 400, 300);    
      h_Res[1]->Draw("HistE");            
      cRes[1]->Print("zee_m.png");

      h_Res[2]=(TH1F*)file->Get("run_284484/DiMuMon/Zmumu/NoTrig/Zmumu_invmass_All");     
      cRes[2]=new TCanvas("zmumu","zmumu", 400, 300);    
      h_Res[2]->Draw("HistE");            
      cRes[2]->Print("zmumu_m.png");

      h_Res[3]=(TH1F*)file->Get("run_284484/DiMuMon/Jpsi/NoTrig/Jpsi_invmass_All");     
      cRes[3]=new TCanvas("jpsi","jpsi", 400, 300);    
      h_Res[3]->Draw("HistE");            
      cRes[3]->Print("jpsi_m.png");


}

