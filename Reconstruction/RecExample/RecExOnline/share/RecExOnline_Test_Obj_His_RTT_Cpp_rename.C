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
#include "TLegend.h"

#define nHisVar 25
#define nJetVar 4
#define nTauVar 4
#define nTrackVar 5
void RecExOnline_Test_Obj_His_RTT_Cpp_rename() 
{
   string P1MONversion="/20.11.X.Y.Z-VAL/build/x86_64-slc6-gcc49-opt/AtlasP1MON/RecExOnline/RecExOnline_User_Offline_test_v1/";  
   string PreURL="https://atlas-rtt.cern.ch/prod/rtt/rel_";
   ofstream myfile;
   myfile.open ("P1MON_RTT_result_template_v1.html");
   myfile<<"<table border=\"2\" style=\"width:100%\"> \n";
 
   TFile *file = new TFile("Monitor.root");
  
//   TFile *file_ref = new TFile("/afs/cern.ch/user/y/yunju/working/yunju/public/GM_ref_plots/r0000284484_All_ATLAS_Global-MDA_Histogramming-Global.root");
TFile *file_ref = new TFile("/afs/cern.ch/user/y/yunju/working/yunju/public/GM_ref_plots/r0000302347_l0420_ATLAS_Global-MDA_Histogramming-Global.root");  
   string foldername_ref;
   foldername_ref= "Histogramming-Global-iss/GM-Gatherer-MIG_GlobalMonitoringSegment";   
      
   string foldername;
  //   foldername="run_284484";
  foldername="run_302347";

   vector<string> HisVarstring;      
   HisVarstring.push_back("Jets/AntiKt4EMTopoJets/pt");  
   HisVarstring.push_back("Jets/AntiKt4EMTopoJets/eta");  
   HisVarstring.push_back("Jets/AntiKt4EMTopoJets/phi");  
   HisVarstring.push_back("Jets/AntiKt4EMTopoJets/M");  
   HisVarstring.push_back("Tau/tauEt");  
   HisVarstring.push_back("Tau/tauEta");  
   HisVarstring.push_back("Tau/tauPhi");  
   HisVarstring.push_back("Tau/tauCharge");  
   HisVarstring.push_back("IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/ngtracks");  
   HisVarstring.push_back("IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/Nhits_per_track");  
   HisVarstring.push_back("IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/d0");  
   HisVarstring.push_back("IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/P");  
   HisVarstring.push_back("IDPerfMon/Wenu/NoTriggerSelection/Wenu_transmass_sel");  
   HisVarstring.push_back("IDPerfMon/Zee/NoTriggerSelection/Zee_invmass");  
   HisVarstring.push_back("DiMuMon/Zmumu/NoTrig/Zmumu_invmass_All");  
   HisVarstring.push_back("DiMuMon/Jpsi/NoTrig/Jpsi_invmass_All"); 
   HisVarstring.push_back("MuonPhysics/Muons/CBMuons/Muons_CBMuons_pt");  
   HisVarstring.push_back("MuonPhysics/Muons/CBMuons/Muons_CBMuons_eta");
   HisVarstring.push_back("MuonPhysics/Muons/CBMuons/Muons_CBMuons_phi");
   HisVarstring.push_back("egamma/photons/photonEtCbLoose");
   HisVarstring.push_back("egamma/photons/photonEtaCbLoose");
   HisVarstring.push_back("egamma/photons/photonPhiCbLoose");
   HisVarstring.push_back("egamma/electrons/electronEtLhLoose");
   HisVarstring.push_back("egamma/electrons/electronEtaLhLoose");
   HisVarstring.push_back("egamma/electrons/electronPhiLhLoose");
   


      TH1F *h_tot[nHisVar];
      TH1F *h_tot_ref[nHisVar]; 

      TCanvas *ctot[nHisVar];  

      TLegend *leg3 = new TLegend(0.22, 0.8, 0.3, 0.88);
      leg3->SetBorderSize(0);
      leg3->SetFillColor(0);
      leg3->SetTextSize(0.03);
      //leg3->SetHeader("p + p , #sqrt{s} = 8 TeV");     

      
              
    
      for(int n=0;n<nHisVar;n++)
      {
       

        //cout<<"Looking for hist: "<<Form("%s/%s",foldername.data(),HisVarstring[n].data())<<endl; 
         
        h_tot[n]=(TH1F*)file->Get(Form("%s/%s",foldername.data(),HisVarstring[n].data()));
        h_tot_ref[n]=(TH1F*)file_ref->Get(Form("%s/%s",foldername_ref.data(),HisVarstring[n].data()));
       
        ctot[n]=new TCanvas(Form("ctot_%d",n),Form("totCanvas_%d",n), 600, 400);
        ctot[n]->cd();
        h_tot[n]->Sumw2();
        h_tot_ref[n]->Sumw2();      
 
        h_tot[n]->Scale(1/h_tot[n]->Integral());
        h_tot_ref[n]->Scale(1/h_tot_ref[n]->Integral());       
        h_tot_ref[n]->SetLineColor(8);
        h_tot_ref[n]->SetFillColor(8);
        h_tot_ref[n]->SetFillStyle(3313);
        float ymax=h_tot_ref[n]->GetMaximum();
        h_tot_ref[n]->SetMaximum(ymax*1.5); 
        h_tot_ref[n]->Draw("HistE");
        
        h_tot[n]->SetLineColor(1);        
        h_tot[n]->Draw("HistEsame");
        string HisVarstring_new;
        HisVarstring_new=HisVarstring[n].data();
     
        if(n==0)
        {  
          leg3->AddEntry(h_tot[n],"P1 MON RTT","f");
          leg3->AddEntry(h_tot_ref[n],"Ref from MDA","f");
        }

        leg3->Draw("same");
                
        int f=-10; 
        do
        {
          //cout<<"start:"<<endl; 
          //cout<<"f1:"<<f<<endl;
          f = HisVarstring_new.find("/");       
          //cout<<"f2:"<<f<<endl;
          if(f!=string::npos) HisVarstring_new.replace(f, std::string("/").length(), "_");
          //cin.get();
        }while (f!=-1);        

        
        string savehis;
        savehis=Form("tot_%s_%s.png",foldername.data(),HisVarstring_new.data()); 
        ctot[n]->Print(savehis.data());
        myfile<<"<tr> \n";
        for(int rel=0;rel<7;rel++)
        {
           string PreURLRel=Form("%s%d%s%s",PreURL.data(),rel,P1MONversion.data(),savehis.data());
           string histlocation;
           //histlocation= "<td><img src=\""+PreURL<<savehis<<"\" alt=\""<<savehis<<"\" height=\"250\"></td>" 
           myfile<<"<td><img src=\""<<PreURLRel.data()<<"\" alt=\""<<savehis<<"\" height=\"250\"></td> \n";
        
        }
        myfile<<"</tr> \n";
      }
      myfile<<"</table> \n";    
myfile.close();
}

