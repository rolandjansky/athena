/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{   
#include <sstream>   
#include <string.h>

//give chamber name:
  std::string name="BOL1A01"; // BOL StationEta:1 Side:A StationPhi:01
  
//////////////////////////////////////////
  std::string multil="MDT/"+name+"/"+name+" MDT_Station_MultiLayer";  
  const char* multil_char=multil.c_str ();
  std::string layer="MDT/"+name+"/"+name+" MDT_Station_Layer";  
  const char* layer_char=layer.c_str ();
  std::string tube="MDT/"+name+"/"+name+" MDT_Station_Tube";  
  const char* tube_char=tube.c_str ();
  std::string x="MDT/"+name+"/"+name+" MDT_x";  
  const char* x_char=x.c_str ();
  std::string y="MDT/"+name+"/"+name+" MDT_y";  
  const char* y_char=y.c_str ();
  std::string z="MDT/"+name+"/"+name+" MDT_z";  
  const char* z_char=z.c_str ();
  std::string cross="MDT/"+name+"/"+name+" MDT_Station_det";  
  const char* cross_char=cross.c_str ();
  std::string longview="MDT/"+name+"/"+name+" MDT_Station_longview";  
  const char* longview_char=longview.c_str ();
  const char* name_char=name.c_str ();

  cout << "multil_char : " <<  multil_char << endl;
  
  cout << "********************************" << endl;
  cout << "**   Chamber Name : " << name << "   **" << endl;
  cout << "********************************" << endl;  
   
   gStyle->SetTitleW(0.5);
   gStyle->SetTitleH(0.08);
   gStyle->SetTitleX(0.13);
   gStyle->SetTitleY(0.98);
   gStyle->SetStatX(0.98);
   gStyle->SetStatY(0.88);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.15);
    
   TFile *histofile1= TFile::Open("rfio:/castor/cern.ch/user/m/muonprod/MuonValidation/csc100/MuonHitsVal_BL1A_csc010000_1205_mupt50GeV.histo.root", "READ");    
   TFile *histofile2= TFile::Open("rfio:/castor/cern.ch/user/m/muonprod/MuonValidation/csc110/MuonHitsVal_BL1A_csc010100_1205_mupt50GeV.histo.root", "READ");    
  
   TCanvas *c1 = new TCanvas("c1",name_char,0,0,1200,500);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);         
     
    c1->Divide(3,2);
    c1->cd(1);   	  
    TH1F *Multilayer1 = (TH1F*) histofile1.Get(multil_char);
    Multilayer1->SetTitle("Hit Occupancy per Multilayer");
    Multilayer1->Sumw2();
    Multilayer1->SetLineColor(1);
    Multilayer1->Draw();
    TH1F *Multilayer2 = (TH1F*) histofile2.Get(multil_char);
    Multilayer2->SetLineColor(2);
    Multilayer2->Sumw2();
    Multilayer2->Draw("same");


    c1->cd(2);   	  
    TH1F *Layer1 = (TH1F*) histofile1.Get(layer_char);
    Layer1->SetTitle("Hit Occupancy per Layer");
    Layer1->Sumw2();
    Layer1->SetLineColor(1);
//     Layer1->SetFillColor(0);
    Layer1->Draw();
    TH1F *Layer2 = (TH1F*) histofile2.Get(layer_char);
    Layer2->SetLineColor(2);
//     Layer2->SetFillColor(0);
    Layer2->Sumw2();
    Layer2->Draw("same");


    c1->cd(3);   	  
    TH1F *Tube1 = (TH1F*) histofile1.Get(tube_char);
    Tube1->SetTitle("Hit Occupancy per Tube");
    Tube1->Sumw2();
    Tube1->SetLineColor(1);
//     Tube1->SetFillColor(0);
    Tube1->Draw();
    TH1F *Tube2 = (TH1F*) histofile2.Get(tube_char);
    Tube2->SetLineColor(2);
//     Tube2->SetFillColor(0);
    Tube2->Sumw2();
    Tube2->Draw("same");


    c1->cd(4);   	  
    TH1F *Hitx1 = (TH1F*) histofile1.Get(x_char);
    Hitx1->SetTitle("Hit X distribution");
//     Hitx1->Sumw2();
    Hitx1->SetLineColor(1);
    Hitx1->SetFillColor(0);
    Hitx1->Draw();
    TH1F *Hitx2 = (TH1F*) histofile2.Get(x_char);
     Hitx2->SetLineColor(2);
    Hitx2->SetFillColor(0);
//     Hitx2->Sumw2();
    Hitx2->Draw("same");
 
     
    c1->cd(5);   	  
    TH1F *Hity1 = (TH1F*) histofile1.Get(y_char);
    Hity1->SetTitle("Hit Y distribution");
//     Hitx1->Sumw2();
    Hity1->SetLineColor(1);
    Hity1->SetFillColor(0);
    Hity1->Draw();
    TH1F *Hity2 = (TH1F*) histofile2.Get(y_char);
    Hity2->SetLineColor(2);
    Hity2->SetFillColor(0);
//     Hitx2->Sumw2();
    Hity2->Draw("same");
 
 
    c1->cd(6);   	  
    TH1F *Hitz1 = (TH1F*) histofile1.Get(z_char);
    Hitz1->SetTitle("Hit Z distribution");
//     Hitx1->Sumw2();
    Hitz1->SetLineColor(1);
    Hitz1->SetFillColor(0);
    Hitz1->Draw();
    TH1F *Hitz2 = (TH1F*) histofile2.Get(z_char);
    Hitz2->SetLineColor(2);
    Hitz2->SetFillColor(0);
//     Hitx2->Sumw2();
    Hitz2->Draw("same");


   TCanvas *c2 = new TCanvas("c2",name_char,0,0,1000,300);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);         
  
   
    c2->Divide(2,1);
    c2->cd(1);   	  
    TH2D *Cross1 = (TH2D*) histofile1.Get(cross_char);
    Cross1->SetTitle("x-y profile");
    Cross1->SetMarkerColor(1);
    Cross1->Draw();
    TH2D *Cross2 = (TH2D*) histofile2.Get(cross_char);
    Cross2->SetMarkerColor(2);
    Cross2->Draw("same");


    c2->cd(2);   	     	  
    TH2D *Longview1 = (TH2D*) histofile1.Get(longview_char);
    Longview1->SetTitle("r-z profile");
    Longview1->SetMarkerColor(1);
    Longview1->Draw();
    TH2D *Longview2 = (TH2D*) histofile2.Get(longview_char);
    Longview2->SetMarkerColor(2);
    Longview2->Draw("same");

}
