/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void plots_public();
void epara_validation_plots();
TH1* get_cumul(TH1* hist);
void ATLAS_LABEL(double x,double y);

void epara_validation_plots()
{
 
  string samplename="photons";
  
  //string dirname ="/afs/cern.ch/user/s/schaarsc/public/fastcalo/epara_storage";
  string dirname ="../../../../../../epara_storage";
  
  //get layers:
  TFile* file1=TFile::Open(Form("%s/%s/firstPCA.root",dirname.c_str(),samplename.c_str()));
  TH2I* h_layer=(TH2I*)file1->Get("h_layer");
  int pcabins=h_layer->GetNbinsX();
  vector<int> layerNr;
  for(int i=1;i<=h_layer->GetNbinsY();i++)
  {
 	 if(h_layer->GetBinContent(1,i)==1) 
 	  layerNr.push_back(h_layer->GetYaxis()->GetBinCenter(i));
  }
  vector<string> layer;
  for(unsigned int l=0;l<layerNr.size();l++)
   layer.push_back(Form("layer%i",layerNr[l]));
  layer.push_back("totalE");
  
  vector<string> name;
  vector<string> title;
  for(unsigned int l=0;l<layer.size()-1;l++)
  {
   name.push_back(layer[l].c_str());
   title.push_back(Form("E fraction in Layer %i",layerNr[l]));
  }
  name.push_back("totalE");  title.push_back("total E [MeV]");
  
  TFile* file=TFile::Open(Form("eparavalidation/%s/validation.root",samplename.c_str()));

  int use_autozoom=1;

  for(unsigned int l=0;l<layer.size();l++)
  {
   int one_canvas=0;
   double min,max,rmin,rmax;
   TH1D* h_output_lin;
   TH1D* h_input_lin;
   if(use_autozoom)
   {
   	h_output_lin=(TH1D*)file->Get(Form("h_output_zoom_%s",layer[l].c_str())); h_output_lin->SetName("h_output_lin");
   	h_input_lin =(TH1D*)file->Get(Form("h_input_zoom_%s",layer[l].c_str()));  h_input_lin->SetName("h_input_lin");
   }
   else
   {
   	h_output_lin=(TH1D*)file->Get(Form("h_output_%s",layer[l].c_str())); h_output_lin->SetName("h_output_lin");
   	h_input_lin =(TH1D*)file->Get(Form("h_input_%s",layer[l].c_str()));  h_input_lin->SetName("h_input_lin");
   }
   
   //linear:
   double kolmo=h_input_lin->KolmogorovTest(h_output_lin);
   double chi2=h_input_lin->Chi2Test(h_output_lin,"UW");
   h_input_lin->SetMarkerSize(1.0);
   h_input_lin->SetLineWidth(1);
   h_output_lin->SetLineWidth(1);
   h_output_lin->SetFillColor(7);
   h_output_lin->Scale(h_input_lin->Integral()/h_output_lin->Integral());
   h_input_lin->GetXaxis()->SetNdivisions(504,kFALSE);
   double ymax=h_input_lin->GetBinContent(h_input_lin->GetMaximumBin());
   h_input_lin->GetYaxis()->SetRangeUser(0,ymax*1.4);
   h_input_lin->GetYaxis()->SetTitle("Linear");
   h_input_lin->GetXaxis()->SetTitle(title[l].c_str());
   
   //log:
   TH1D* h_output_log=(TH1D*)h_output_lin->Clone("h_output_log");
   TH1D* h_input_log=(TH1D*)h_input_lin->Clone("h_input_log");
   h_input_log->GetYaxis()->SetRangeUser(0.1,ymax*5.0);
   h_input_log->GetYaxis()->SetTitle("Log");
      
   //cumulative:
   TH1D* h_output_cumul=(TH1D*)get_cumul(h_output_lin); h_output_cumul->SetName("h_output_cumul");
   TH1D* h_input_cumul =(TH1D*)get_cumul(h_input_lin);  h_input_cumul->SetName("h_input_cumul");
   double sf=h_input_cumul->GetBinContent(h_input_cumul->GetNbinsX());
   h_output_cumul->Scale(1.0/sf);
   h_input_cumul->Scale(1.0/sf);
   h_input_cumul->GetYaxis()->SetRangeUser(0,1.2);
   h_input_cumul->GetYaxis()->SetTitle("Cumulative");
   
   if(one_canvas)
   {
    TCanvas* can=new TCanvas("can","can",0,0,1600,600);
    can->Divide(3,1);
    can->cd(1);
    h_input_lin->Draw("e");
    h_output_lin->Draw("histsame");
    h_input_lin->Draw("esame");
    
    TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
    leg->AddEntry(h_output_lin,"Parametrisation","f");
    leg->AddEntry(h_input_lin,"G4 Input","lpe");
    leg->Draw();
    
    can->cd(2);
    h_input_log->Draw("e");
    h_output_log->Draw("histsame");
    h_input_log->Draw("esame");
    can->cd(2)->SetLogy();

    can->cd(3);
    h_input_cumul->Draw("e");
    h_output_cumul->Draw("histsame");
    h_input_cumul->Draw("esame");
    
    can->cd(1)->RedrawAxis();
    can->cd(2)->RedrawAxis();
    can->cd(3)->RedrawAxis();
    can->Print(Form("eparavalidation/%s/%s.pdf",samplename.c_str(),name[l].c_str()));
    delete can;
   } //one_canvas
   else
   {
   	TCanvas* can_lin=new TCanvas(Form("can_lin_%s",layer[l].c_str()),Form("can_lin_%s",layer[l].c_str()),0,0,800,600);
    h_input_lin ->Draw("e");
    h_output_lin->Draw("histsame");
    h_input_lin ->Draw("esame");
    {
     TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
     leg->SetBorderSize(0);
     leg->SetFillStyle(0);
     leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
     leg->AddEntry(h_output_lin,"Parametrisation","f");
     leg->AddEntry(h_input_lin,"G4 Input","lpe");
     leg->Draw();
     TLatex* la=new TLatex(0.2,0.5,"#splitline{Photons 50 GeV}{0.2 < |#eta| < 0.25}");
     la->SetNDC();
     la->Draw();
     ATLAS_LABEL(0.2,0.9);
    }
   	can_lin->RedrawAxis();
    can_lin->Print(Form("eparavalidation/%s/%s_lin.pdf",samplename.c_str(),name[l].c_str()));
    
    TCanvas* can_log=new TCanvas("can_log","can_log",0,0,800,600);
    h_input_log ->Draw("e");
    h_output_log->Draw("histsame");
    h_input_log ->Draw("esame");
    can_log->SetLogy();
    {
     TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
     leg->SetBorderSize(0);
     leg->SetFillStyle(0);
     leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
     leg->AddEntry(h_output_lin,"Parametrisation","f");
     leg->AddEntry(h_input_lin,"G4 Input","lpe");
     leg->Draw();
    }
   	can_log->RedrawAxis();
    can_log->Print(Form("eparavalidation/%s/%s_log.pdf",samplename.c_str(),name[l].c_str()));
   	
   	TCanvas* can_cum=new TCanvas("can_cum","can_cum",0,0,800,600);
    h_input_cumul ->Draw("e");
    h_output_cumul->Draw("histsame");
    h_input_cumul ->Draw("esame");
    {
     TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
     leg->SetBorderSize(0);
     leg->SetFillStyle(0);
     leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
     leg->AddEntry(h_output_lin,"Parametrisation","f");
     leg->AddEntry(h_input_lin,"G4 Input","lpe");
     leg->Draw();
    }
   	can_cum->RedrawAxis();
    can_cum->Print(Form("eparavalidation/%s/%s_cum.pdf",samplename.c_str(),name[l].c_str()));
    
   } //separate canvases
   
  } //for layer
 
}

TH1* get_cumul(TH1* hist)
{
  TH1D* h_cumul=(TH1D*)hist->Clone("h_cumul");
  double sum=0;
  for(int b=1;b<=h_cumul->GetNbinsX();b++)
  {
    sum+=hist->GetBinContent(b);
    h_cumul->SetBinContent(b,sum);
  }
  return h_cumul; 
}

void ATLAS_LABEL(double x,double y) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(1);
  l.DrawLatex(x,y,"ATLAS #bf{#it{Simulation}}");
}

void Prelim_LABEL(double x,double y) 
{
  TLatex l;
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(1);
  l.DrawLatex(x,y,"#bf{#it{Preliminary}}");
}

void photon()
{
 
 int zoom=1;
 string zname="";
 if(zoom) zname="_zoom";
 
 TFile* file=TFile::Open("eparavalidation/photons/validation.root");
 TH1D* h_input=(TH1D*)file->Get(Form("h_input%s_totalE",zname.c_str()));   h_input->SetName("h_input");
 TH1D* h_output=(TH1D*)file->Get(Form("h_output%s_totalE",zname.c_str())); h_output->SetName("h_output");
 
 h_input->SetMarkerSize(1.0);
 h_input->SetLineWidth(1);
 h_output->SetLineWidth(1);
 h_output->SetFillColor(7);
 h_output->Scale(h_input->Integral()/h_output->Integral());
 h_input->GetXaxis()->SetNdivisions(506,kTRUE);
 double ymax=h_input->GetBinContent(h_input->GetMaximumBin());
 h_input->GetYaxis()->SetRangeUser(0,ymax*1.4);
 h_input->GetYaxis()->SetTitle("Events");
 h_input->GetXaxis()->SetTitle("Total cell energy [MeV]");
 
 TCanvas* can=new TCanvas("can","can",0,0,800,600);
 h_input ->Draw("e");
 h_input->GetYaxis()->SetRangeUser(0.5,10000);
 h_input->GetXaxis()->SetRangeUser(43000,52000);
 h_output->Draw("histsame");
 h_input ->Draw("esame");
 can->SetLogy();
 TLegend* leg=new TLegend(0.65,0.82,0.95,0.93);
 leg->SetBorderSize(0);
 leg->SetFillStyle(0);
 leg->AddEntry(h_output,"#bf{Parametrisation}","f");
 leg->AddEntry(h_input,"#bf{G4 Input}","lpe");
 leg->Draw();
 TLatex* la=new TLatex(0.2,0.5,"#splitline{Photons 50 GeV}{0.2 < |#eta| < 0.25}");
 la->SetNDC();
 la->Draw();
 ATLAS_LABEL(0.2,0.88);
 Prelim_LABEL(0.2,0.83);
 can->RedrawAxis();
 can->Print("eparavalidation/photon_totalE.pdf");
 
}

void pion()
{
 
 int zoom=1;
 string zname="";
 if(zoom) zname="_zoom";
 
 TFile* file=TFile::Open("eparavalidation/pions/validation.root");
 TH1D* h_input=(TH1D*)file->Get(Form("h_input%s_layer12",zname.c_str()));   h_input->SetName("h_input");
 TH1D* h_output=(TH1D*)file->Get(Form("h_output%s_layer12",zname.c_str())); h_output->SetName("h_output");
 
 h_input->SetMarkerSize(1.0);
 h_input->SetLineWidth(1);
 h_output->SetLineWidth(1);
 h_output->SetFillColor(7);
 h_output->Scale(h_input->Integral()/h_output->Integral());
 h_input->GetXaxis()->SetNdivisions(506,kTRUE);
 double ymax=h_input->GetBinContent(h_input->GetMaximumBin());
 h_input->GetYaxis()->SetRangeUser(0,ymax*1.4);
 h_input->GetYaxis()->SetTitle("Events");
 h_input->GetXaxis()->SetTitle("Fraction of energy deposited in TileBarrel1");
 
 TCanvas* can=new TCanvas("can","can",0,0,800,600);
 h_input ->Draw("e");
 h_output->Draw("histsame");
 h_input ->Draw("esame");
 TLegend* leg=new TLegend(0.65,0.82,0.95,0.93);
 leg->SetBorderSize(0);
 leg->SetFillStyle(0);
 leg->AddEntry(h_output,"#bf{Parametrisation}","f");
 leg->AddEntry(h_input,"#bf{G4 Input}","lpe");
 leg->Draw();
 TLatex* la=new TLatex(0.6,0.5,"#splitline{Pions 50 GeV}{0.2 < |#eta| < 0.25}");
 la->SetNDC();
 la->Draw();
 ATLAS_LABEL(0.2,0.88);
 Prelim_LABEL(0.2,0.83);
 can->RedrawAxis();
 can->Print("eparavalidation/pion_layer12.pdf");
 
}

