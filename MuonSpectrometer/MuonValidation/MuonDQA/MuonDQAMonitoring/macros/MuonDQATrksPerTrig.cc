/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <fstream>
#include <vector>

void MuonDQATrksPerTrig (string runNumber)
{
  const char* MBoy  = "ConvertedMBoy";
  const char* Moore = "Moore";
  
  vector<string> trigStreams;
  vector<string> filenames;
  string trigStream;
  string filename;
  int N=0; //number of streams;

  ifstream fin;
  fin.open("streamsfiles.txt");
  if(fin.fail()) {cout<< "ERROR! Could not read from streamsfiles.txt ..."<<endl; return; }
  while (!fin.eof()) {
    fin>>trigStream>>filename;
    if(!fin.eof()){
      trigStreams.push_back(trigStream);
      filenames.push_back(filename);
      N++;
    }
  }
  if (N==0) {cout<<"ERROR! No files found...\n"; return;}
  fin.close();

  std::string pdfTag=runNumber+".pdf";
  TCanvas *canv=new TCanvas("canv","Number Of Tracks Per Trigger Stream",0,0,850,650);
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  
  string dirname1 = "run_"+runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonMonTrkSummary/ConvertedMBoyTracks/";
  string dirname2 = "run_"+runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonMonTrkSummary/MooreTracks/";
  string histname = "muon_ntracks";
  
  TH1F *h1 = new TH1F("h1","Number_Of_Reconstructed_Tracks_Per_Trigger_Stream",1,0,1);
  TH1F *h2 = new TH1F("h2","Number_Of_Reconstructed_Tracks_Per_Trigger_Stream",1,0,1);

  for (int i=0; i<N; i++) {
    TFile::Open(filenames[i].c_str());
    cout<<"--->" << filenames[i]<<endl;
    
    TDirectory *dir1=gFile->GetDirectory(dirname1.c_str());
    TDirectory *dir2=gFile->GetDirectory(dirname2.c_str());    
    
    if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
    TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
    TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
    TLegend *leg = new TLegend(0.8,0.92,1,1); 
    int nE1=hist1->GetEntries();
    int nE2=hist2->GetEntries();
    h1->Fill(trigStreams[i].c_str(),nE1);
    h2->Fill(trigStreams[i].c_str(),nE2);
    h1->SetLineColor(kBlue);
    h2->SetLineColor(kRed);
    int max=h1->GetMaximum();
    int min=h2->GetMinimum();
    int max2=h2->GetMaximum();
    int min2=h2->GetMinimum();
    if ( max< max2 )  max=max2;
    if ( min< min2 )  min=min2;
    if (min>0) min=0;
    h1->GetYaxis()->SetRangeUser(1.2*min,1.1*max);
    char t1[100];
    char t2[100];
    sprintf(t1,"ConvertedMBoy");
    sprintf(t2,"Moore");
    leg->AddEntry(h1,t1,"l");
    leg->AddEntry(h2,t2,"l");
    h1->LabelsDeflate("X");
    h2->LabelsDeflate("X");
    h1->Draw();
    h2->Draw("same");
    leg->Draw();
    canv->Update();
  }

  TPDF* pdf;
  stringstream pdfName;
  pdfName<<"MuonDqaTrksPerTrig_"<<pdfTag;
  pdf=new TPDF(pdfName.str().c_str());
  // canv->SetLogy();
  h1->Draw(); h2->Draw("same"); leg->Draw();
  canv->Update();
  pdf->Close();

  return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

