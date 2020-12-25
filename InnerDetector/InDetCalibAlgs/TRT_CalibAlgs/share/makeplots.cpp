#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <exception>
#include <TNtuple.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TEventList.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TPaletteAxis.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TError.h>
#include <TVectorD.h>
#include <map>
#include <string>
#include <vector>
using namespace std;

bool do_expert = false;


char detlet[3]={'C','-','A'};

void SetZAxis (TH2F* reshist0) {
  double zmin =  100000000;
  double zmax = -100000000;
  double temp = 0;
  for (int ix = 0; ix < reshist0->GetNbinsX(); ix++) {
        for (int iy =  0; iy < reshist0->GetNbinsY(); iy++) {
                temp = reshist0->GetBinContent(ix,iy);
                if (temp > zmax && temp !=0 ) zmax = temp;
                if (temp < zmin && temp !=0 ) zmin = temp;
        }
  }
  reshist0->GetZaxis()->SetRangeUser(zmin,zmax)             ;
}

void ResizePalette(TH2* hist){
  
  TPaletteAxis *palette = (TPaletteAxis*) hist->GetListOfFunctions()->FindObject("palette");
  palette->SetX2NDC(0.92);
  palette->Paint();
  palette->SetLabelSize(0.03);

}

string trim(string s){
  while(s.find_first_of(" ")==0) s=s.substr(1,s.size());
  while(s.find_last_of(" ")==s.size()-1) s=s.substr(0,(s.size())-1);
  return s;
}


class TextPage: public TCanvas{
public:
  TextPage(string);
};

TextPage::TextPage(string text){
  this->cd();
  TPaveText* pt= new TPaveText(.1,.1,0.9,0.9);
  pt->SetTextAlign(22);
  pt->AddText(text.data());
  pt->Draw();  
}


//================================================================

class NHitStack: public THStack{
public: 
  NHitStack(TNtuple*, TLegend*, bool, int, int);
};


NHitStack::NHitStack(TNtuple* tuple, TLegend* leg, bool doleg, int mint0, int minrt){

  int min=(int)tuple->GetMinimum("nt0");  
  int max=(int)tuple->GetMaximum("nt0");
  float bsize=ceil(((float)(max-min)/50));
  int hmax=min+(int)(50*bsize);
  //cout << min << " " << max << " " << bsize << " " << hmax << endl;
  TH1F* hits1 = new TH1F(Form("%s_hits1",tuple->GetName()),"test1",50,min,hmax);
  TH1F* hits2 = new TH1F(Form("%s_hits2",tuple->GetName()),"test2",50,min,hmax);
  TH1F* hits3 = new TH1F(Form("%s_hits3",tuple->GetName()),"test3",50,min,hmax);
  //TH1F* hits2 = new TH1F("hits2","test2",50,min,hmax);
  //TH1F* hits3 = new TH1F("hits3","test3",50,min,hmax);  
  tuple->Draw(Form("nt0>>%s_hits1",tuple->GetName()),Form("nt0<=%i",mint0));
  tuple->Draw(Form("nt0>>%s_hits2",tuple->GetName()),Form("nt0>%i && nt0<=%i",mint0,minrt));
  tuple->Draw(Form("nt0>>%s_hits3",tuple->GetName()),Form("nt0>%i",minrt));
  hits1->SetFillColor(46);
  hits2->SetFillColor(8);
  hits3->SetFillColor(38);

  //gStyle->SetOptStat("e");

  this->Add(hits1);
  this->Add(hits2);
  this->Add(hits3);

  if (doleg){
    leg->AddEntry(hits1,"no calibration");
    leg->AddEntry(hits2,"enough for T0 calib");
    leg->AddEntry(hits3,"enough for T0+RT calib");
    leg->SetTextSize(0.04);
  }

}


//================================================================

class HitStatistics: public TCanvas{
public:
  HitStatistics(char*, bool);
};

HitStatistics::HitStatistics(char* infile, bool isAr = false){

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* Strawtuple   =NULL;
  TNtuple* Chiptuple    =NULL;
  TNtuple* Boardtuple   =NULL;
  TNtuple* Moduletuple  =NULL;
  TNtuple* Layertuple   =NULL;
  TNtuple* Detectortuple=NULL;
  TNtuple* conftup      =NULL;
 
 if(!isAr){
  Strawtuple=(TNtuple*)file->Get("Strawtuple");
  Chiptuple=(TNtuple*)file->Get("Chiptuple");
  Boardtuple=(TNtuple*)file->Get("Boardtuple");
  Moduletuple=(TNtuple*)file->Get("Moduletuple");
  Layertuple=(TNtuple*)file->Get("Layertuple");
  Detectortuple=(TNtuple*)file->Get("Detectortuple");
  conftup=(TNtuple*)file->Get("configs");
  }
  else {
  Strawtuple=(TNtuple*)file->Get("Straw_Artuple");
  Chiptuple=(TNtuple*)file->Get("Chip_Artuple");
  Boardtuple=(TNtuple*)file->Get("Board_Artuple");
  Moduletuple=(TNtuple*)file->Get("Module_Artuple");
  Layertuple=(TNtuple*)file->Get("Layer_Artuple");
  Detectortuple=(TNtuple*)file->Get("Detector_Artuple");
  conftup=(TNtuple*)file->Get("configs");
  }

  float mint0=500,minrt=10000;
  if (conftup) {
    conftup->SetBranchAddress("mint0",&mint0);
    conftup->SetBranchAddress("minrt",&minrt);
    conftup->GetEntry(0);
  }
  //minrt=(int)Detectortuple->GetMaximum("nt0");  

  this->SetName("c1");
  this->cd();
  this->Divide(3,2,0.01,0.01);

  ((TPad*) this->GetPrimitive("c1_1"))->SetFrameFillColor(0);  
  ((TPad*) this->GetPrimitive("c1_2"))->SetFrameFillColor(0);  
  ((TPad*) this->GetPrimitive("c1_3"))->SetFrameFillColor(0);  
  ((TPad*) this->GetPrimitive("c1_4"))->SetFrameFillColor(0);  
  ((TPad*) this->GetPrimitive("c1_5"))->SetFrameFillColor(0);  
  ((TPad*) this->GetPrimitive("c1_6"))->SetFrameFillColor(0);  


  ((TPad*) this->GetPrimitive("c1_1"))->SetLogy();  
  ((TPad*) this->GetPrimitive("c1_2"))->SetLogy();  
  ((TPad*) this->GetPrimitive("c1_3"))->SetLogy();  
  ((TPad*) this->GetPrimitive("c1_4"))->SetLogy();  
  ((TPad*) this->GetPrimitive("c1_5"))->SetLogy();  
  ((TPad*) this->GetPrimitive("c1_6"))->SetLogy();  

  TLegend* leg = new TLegend(0.95,0.6,0.35,0.35);

  //TGaxis::SetMaxDigits(5);
  //gStyle->SetStripDecimals(kTRUE);

  this->cd(1);
  this->SetLogx();
  NHitStack* hsStraw = new NHitStack(Strawtuple,leg,false,(int)mint0,(int)minrt);
  hsStraw->SetTitle("Hit Statistics (Straw)");
  if (isAr) hsStraw->SetTitle("Hit Statistics (Argon Straw)");
  hsStraw->Draw();
  hsStraw->GetHistogram()->GetXaxis()->SetTitle("No. Hits");
  hsStraw->GetHistogram()->GetYaxis()->SetTitle("No. Straws");
  hsStraw->GetHistogram()->GetXaxis()->SetNdivisions(-4);
  this->cd(2);
  this->SetLogx();
  NHitStack* hsChip = new NHitStack(Chiptuple,leg,false,(int)mint0,(int)minrt);
  hsChip->SetTitle("Hit Statistics (Chip)");
  if (isAr)  hsChip->SetTitle("Hit Statistics (Argon chip)");
  hsChip->Draw();
  hsChip->GetHistogram()->GetXaxis()->SetTitle("No. Hits");
  hsChip->GetHistogram()->GetYaxis()->SetTitle("No. Chips");
  hsChip->GetHistogram()->GetXaxis()->SetNdivisions(-4);
  this->cd(3);
  this->SetLogx();
  NHitStack* hsBoard = new NHitStack(Boardtuple,leg,false,(int)mint0,(int)minrt);
  hsBoard->SetTitle("Hit Statistics (Board)");
  if (isAr)  hsBoard->SetTitle("Hit Statistics (Argon Board)");
  hsBoard->Draw();
  hsBoard->GetHistogram()->GetXaxis()->SetTitle("No. Hits");
  hsBoard->GetHistogram()->GetYaxis()->SetTitle("No. Boards");
  hsBoard->GetHistogram()->GetXaxis()->SetNdivisions(-4);
  this->cd(4);
  this->SetLogx();
  NHitStack* hsModule = new NHitStack(Moduletuple,leg,false,(int)mint0,(int)minrt);
  hsModule->SetTitle("Hit Statistics (Phi Sector)");
  if (isAr)  hsModule->SetTitle("Hit Statistics (Argon Phi Sector)");
  hsModule->Draw();
  hsModule->GetHistogram()->GetXaxis()->SetTitle("No. Hits");
  hsModule->GetHistogram()->GetYaxis()->SetTitle("No. Phi Sectors");
  hsModule->GetHistogram()->GetXaxis()->SetNdivisions(-4);
  this->cd(5);
  this->SetLogx();
  NHitStack* hsLayer = new NHitStack(Layertuple,leg,false,(int)mint0,(int)minrt);
  hsLayer->SetTitle("Hit Statistics (Layer/wheel)");
  if (isAr)  hsLayer->SetTitle("Hit Statistics (Argon Layer/wheel)");
  hsLayer->Draw();
  hsLayer->GetHistogram()->GetXaxis()->SetTitle("No. Hits");
  hsLayer->GetHistogram()->GetYaxis()->SetTitle("No. Layers/wheel");
  hsLayer->GetHistogram()->GetXaxis()->SetNdivisions(-4);
  this->cd(6);
  this->SetLogx();
  NHitStack* hsDetector = new NHitStack(Detectortuple,leg,true,(int)mint0,(int)minrt);
  hsDetector->SetTitle("Hit Statistics (Detector)");
  if (isAr)  hsDetector->SetTitle("Argon Hit Statistics (Detector)");
  hsDetector->Draw();
  hsDetector->GetHistogram()->GetXaxis()->SetTitle("No. Hits");
  hsDetector->GetHistogram()->GetYaxis()->SetTitle("No. Detectors");
  hsDetector->GetHistogram()->GetXaxis()->SetNdivisions(-4);

  leg->Draw();

}


//================================================================

class XYMaps: public TCanvas{
public:
  XYMaps(char*, char*, bool) throw(string);
};

XYMaps::XYMaps(char* infile, char* variable, bool isAr = false) throw(string){

  char selectionA[500];  sprintf(selectionA,"%s*(det==1)",variable);
  char selectionC[500];  sprintf(selectionC,"%s*(det==-1)",variable);
  if (variable=="nt0") variable = "Nhits";
  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* Chiptuple    =       NULL;
  TNtuple* Boardtuple   =       NULL;
  TNtuple* Moduletuple  =       NULL;

  if(!isAr){
    Chiptuple=(TNtuple*)        file->Get("Chiptuple");
    Boardtuple=(TNtuple*)       file->Get("Boardtuple");
    Moduletuple=(TNtuple*)      file->Get("Moduletuple");
  }
  else{
    Chiptuple=(TNtuple*)        file->Get("Chip_Artuple");
    Boardtuple=(TNtuple*)       file->Get("Board_Artuple");
    Moduletuple=(TNtuple*)      file->Get("Module_Artuple");
  }

  this->SetName("c1");
  this->cd();
  this->Divide(3,2,0.01,0.01);
  ((TPad*) this->GetPrimitive("c1_1"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_2"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_3"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_4"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_5"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_6"))->SetFrameFillColor(1);  

  gStyle->SetPalette(1);

  this->cd(1);
  Moduletuple->Draw("y:x>>reshist10(40,-1200,1200,40,-1200,1200)",selectionA,"colz");
  TH2F *reshist10 = (TH2F*)gPad->GetPrimitive("reshist10");
  if (!reshist10) throw string("Variable not found!");
  if (variable == "res"){
    reshist10->GetZaxis()->SetRangeUser(0.135, 0.150);
  }
  else if (variable == "abs(resMean)"){
    reshist10->GetZaxis()->SetRangeUser(0.0, 0.007);
  }
  else if (variable == "t0"){
    reshist10->GetZaxis()->SetRangeUser(9.2, 10.8); 
  }
  else if (variable == "abs(t0-oldt0)"){
    reshist10->GetZaxis()->SetRangeUser(0.0, 0.5);
  }
  else if (variable == "(tres)"){
    reshist10->GetZaxis()->SetRangeUser(3.1, 3.5);
  }
  this->Update();
  if(reshist10->GetEntries()>1)   ResizePalette(reshist10);
  //SetZAxis(reshist10);
  char title1[500];  sprintf(title1,"Module %s (barrel side A)",variable);
  if (isAr) 	sprintf(title1,"Argon Module %s (barrel side A)",variable);
  reshist10->SetTitle(title1);
  reshist10->SetStats(0);
  reshist10->GetXaxis()->SetLabelOffset(1000);
  reshist10->GetYaxis()->SetLabelOffset(1000);

  this->cd(2);
  Boardtuple->Draw("y:x>>reshist0(100,-1200,1200,100,-1200,1200)",selectionA,"colz");
  TH2F *reshist0 = (TH2F*)gPad->GetPrimitive("reshist0");
  if (!reshist0) throw string("Variable not found!");

 if (variable == "res"){
    reshist0->GetZaxis()->SetRangeUser(0.130, 0.160);
  }
  else if (variable == "abs(resMean)"){
    reshist0->GetZaxis()->SetRangeUser(0.0, 0.010);
  }
  else if (variable == "t0"){
    reshist0->GetZaxis()->SetRangeUser(9.0, 11); 
  }
  else if (variable == "abs(t0-oldt0)"){
    reshist0->GetZaxis()->SetRangeUser(0.0, 0.7);
  }
  else if (variable == "(tres)"){
    reshist0->GetZaxis()->SetRangeUser(2.9, 4);
  }
  this->Update();
  if(reshist0->GetEntries()>1)   ResizePalette(reshist0);
  //SetZAxis(reshist0);
  char title2[500];  sprintf(title2,"Board %s (barrel side A)",variable);
  if(isAr) sprintf(title2,"Argon Board %s (barrel side A)",variable);
  reshist0->SetTitle(title2);
  reshist0->SetStats(0);
  reshist0->GetXaxis()->SetLabelOffset(1000);
  reshist0->GetYaxis()->SetLabelOffset(1000);

  this->cd(3);
  Chiptuple->Draw("y:x>>t0hist0(300,-1200,1200,300,-1200,1200)",selectionA,"colz");
  TH2F *t0hist0 = (TH2F*)gPad->GetPrimitive("t0hist0");
  if (!t0hist0) throw string("Variable not found!");
  char title3[500];  sprintf(title3,"Chip %s (barrel side A)",variable);
  if(isAr) 	sprintf(title3,"Argon Chip %s (barrel side A)",variable);
  t0hist0->SetTitle(title3);

 if (variable == "res"){
    t0hist0->GetZaxis()->SetRangeUser(0.115, 0.190);
  }
  else if (variable == "abs(resMean)"){
    t0hist0->GetZaxis()->SetRangeUser(0.0, 0.02);
  }
  else if (variable == "t0"){
    t0hist0->GetZaxis()->SetRangeUser(5.0, 13); 
  }
  else if (variable == "abs(t0-oldt0)"){
    t0hist0->GetZaxis()->SetRangeUser(0.0, 1.5);
  }
  else if (variable == "(tres)"){
    t0hist0->GetZaxis()->SetRangeUser(2.5, 4.5);
  }
  this->Update();
  if(t0hist0->GetEntries()>1)   ResizePalette(t0hist0);
  //SetZAxis(t0hist0);
  t0hist0->SetStats(0);
  t0hist0->GetXaxis()->SetLabelOffset(1000);
  t0hist0->GetYaxis()->SetLabelOffset(1000);

  this->cd(4);
  Moduletuple->Draw("y:x>>reshist11(40,-1200,1200,40,-1200,1200)",selectionC,"colz");
  TH2F *reshist11 = (TH2F*)gPad->GetPrimitive("reshist11");
  if (!reshist11) throw string("Variable not found!");
  if (variable == "res"){
    reshist11->GetZaxis()->SetRangeUser(0.135, 0.150);
  }
  else if (variable == "abs(resMean)"){
    reshist11->GetZaxis()->SetRangeUser(0.0, 0.007);
  }
  else if (variable == "t0"){
    reshist11->GetZaxis()->SetRangeUser(9.2, 10.8); 
  }
  else if (variable == "abs(t0-oldt0)"){
    reshist11->GetZaxis()->SetRangeUser(0.0, 0.5);
  }
  else if (variable == "(tres)"){
    reshist11->GetZaxis()->SetRangeUser(3.1, 3.5);
  }
  this->Update();
  if(reshist11->GetEntries()>1)   ResizePalette(reshist11);
  //SetZAxis(reshist11);
  char title4[500];  sprintf(title4,"Module %s (barrel side C)",variable);
  if(isAr) sprintf(title4,"Argon Module %s (barrel side C)",variable);
  reshist11->SetTitle(title4);
  reshist11->SetStats(0);
  reshist11->GetXaxis()->SetLabelOffset(1000);
  reshist11->GetYaxis()->SetLabelOffset(1000);

  this->cd(5);
  Boardtuple->Draw("y:x>>reshist1(100,-1200,1200,100,-1200,1200)",selectionC,"colz");
  TH2F *reshist1 = (TH2F*)gPad->GetPrimitive("reshist1");
  if (!reshist1) throw string("Variable not found!");
 if (variable == "res"){
    reshist1->GetZaxis()->SetRangeUser(0.130, 0.160);
  }
  else if (variable == "abs(resMean)"){
    reshist1->GetZaxis()->SetRangeUser(0.0, 0.01);
  }
  else if (variable == "t0"){
    reshist1->GetZaxis()->SetRangeUser(9.0, 11); 
  }
  else if (variable == "abs(t0-oldt0)"){
    reshist1->GetZaxis()->SetRangeUser(0.0, 0.7);
  }
  else if (variable == "(tres)"){
    reshist1->GetZaxis()->SetRangeUser(2.9, 4.0);
  }
  this->Update();
  if(reshist1->GetEntries()>1)   ResizePalette(reshist1);
  //SetZAxis(reshist1);
  char title5[500];  sprintf(title5,"Board %s (barrel side C)",variable);
  if(isAr)	sprintf(title5,"Argon Board %s (barrel side C)",variable);
  reshist1->SetTitle(title5);
  reshist1->SetStats(0);
  reshist1->GetXaxis()->SetLabelOffset(1000);
  reshist1->GetYaxis()->SetLabelOffset(1000);

  this->cd(6);
  Chiptuple->Draw("y:x>>t0hist1(300,-1200,1200,300,-1200,1200)",selectionC,"colz");
  TH2F *t0hist1 = (TH2F*)gPad->GetPrimitive("t0hist1");
  if (!t0hist1) throw string("Variable not found!");
 if (variable == "res"){
    t0hist1->GetZaxis()->SetRangeUser(0.115, 0.190);
  }
  else if (variable == "abs(resMean)"){
    t0hist1->GetZaxis()->SetRangeUser(0.0, 0.020);
  }
  else if (variable == "t0"){
    t0hist1->GetZaxis()->SetRangeUser(5.0, 13); 
  }
  else if (variable == "abs(t0-oldt0)"){
    t0hist1->GetZaxis()->SetRangeUser(0.0, 1.5);
  }
  else if (variable == "(tres)"){
    t0hist1->GetZaxis()->SetRangeUser(2.5, 4.5);
  }
  this->Update();
  if(t0hist1->GetEntries()>1)   ResizePalette(t0hist1);
  //SetZAxis(t0hist1);
  char title6[500];  sprintf(title6,"Chip %s (barrel side C)",variable);
  if(isAr)	 sprintf(title6,"Argon Chip %s (barrel side C)",variable);
  t0hist1->SetTitle(title6);
  t0hist1->SetStats(0);
  t0hist1->GetXaxis()->SetLabelOffset(1000);
  t0hist1->GetYaxis()->SetLabelOffset(1000);

}


//================================================================

class T0CalTypeXY{
public:
  T0CalTypeXY(TNtuple*, int, float);
  void Draw(char*,bool);
  TGraph** graph;  
  TEventList** elist;
};

T0CalTypeXY::T0CalTypeXY(TNtuple* Levtuple, int detector, float markersize){
  
  float x,y,nt0,chp,brd,lay,mod,det;
  int color[7]={0,46,38,8,16,1,2};

  graph = new TGraph*[6];
  elist = new TEventList*[6];

  Levtuple->SetBranchAddress("x",&x);
  Levtuple->SetBranchAddress("y",&y);
  Levtuple->SetBranchAddress("nt0",&nt0);
  Levtuple->SetBranchAddress("chp",&chp);
  Levtuple->SetBranchAddress("brd",&brd);
  Levtuple->SetBranchAddress("lay",&lay);
  Levtuple->SetBranchAddress("mod",&mod);
  Levtuple->SetBranchAddress("det",&det);
  
  Levtuple->Draw(">>tmplist0",Form("nt0*(det==%i)",detector));
  Levtuple->Draw(">>tmplist1",Form("nt0*(ftype==1 && det==%i)",detector));
  Levtuple->Draw(">>tmplist2",Form("nt0*(ftype==2 && det==%i)",detector));
  Levtuple->Draw(">>tmplist3",Form("nt0*(ftype==3 && det==%i)",detector));
  Levtuple->Draw(">>tmplist4",Form("nt0*(ftype==4 && det==%i)",detector));
  Levtuple->Draw(">>tmplist5",Form("nt0*(ftype==5 && det==%i)",detector));
  Levtuple->Draw(">>tmplist6",Form("nt0*(ftype==6 && det==%i)",detector));
  elist[0] = (TEventList*)gDirectory->Get("tmplist0");      
  elist[1] = (TEventList*)gDirectory->Get("tmplist1");      
  elist[2] = (TEventList*)gDirectory->Get("tmplist2");      
  elist[3] = (TEventList*)gDirectory->Get("tmplist3");      
  elist[4] = (TEventList*)gDirectory->Get("tmplist4");      
  elist[5] = (TEventList*)gDirectory->Get("tmplist5");      
  elist[6] = (TEventList*)gDirectory->Get("tmplist6");      

  for (int igr=0;igr<=6;igr++){
    graph[igr]=new TGraph();
    graph[igr]->SetMarkerStyle(20);
    graph[igr]->SetMarkerSize(markersize);
    graph[igr]->SetMarkerColor(color[igr]);
    int ipnt=0;
    for (int ib=0;ib<elist[igr]->GetN();ib++){
      ipnt++;      
      Levtuple->GetEntry(elist[igr]->GetEntry(ib));
      //if (igr==1) cout << "MEANSHIFT" << " " << chp << " " << brd << " " << mod << " " << lay << " " << det << endl;
      graph[igr]->SetPoint(ipnt,x,y);
    }
    graph[igr]->RemovePoint(0);   
  }
  graph[0]->SetMarkerSize(0.1);

}

void T0CalTypeXY::Draw(char* title, bool plotleg){
  graph[0]->GetHistogram()->GetXaxis()->SetLabelOffset(1000);
  graph[0]->GetHistogram()->GetYaxis()->SetLabelOffset(1000);
  if (graph[0]->GetN()>0) graph[0]->Draw("ap");
  if (graph[1]->GetN()>0) graph[1]->Draw("p");
  if (graph[2]->GetN()>0) graph[2]->Draw("p");
  if (graph[3]->GetN()>0) graph[3]->Draw("p");
  if (graph[4]->GetN()>0) graph[4]->Draw("p");
  if (graph[5]->GetN()>0) graph[5]->Draw("p");
  if (graph[6]->GetN()>0) graph[6]->Draw("p");
  //cout << graph[0]->GetN() << endl;
  //cout << graph[1]->GetN() << endl;
  //cout << graph[2]->GetN() << endl;
  //cout << graph[3]->GetN() << endl;
  //cout << graph[4]->GetN() << endl;
  graph[0]->Clear();
  graph[0]->SetTitle(title);

  if (plotleg) {
    TLegend* leg = new TLegend(0.99,0.35,0.55,0.01);
    leg->AddEntry(graph[2],"normal fit");
    leg->AddEntry(graph[3],"used chip ref");
    leg->AddEntry(graph[4],"Low stat,T0");
    leg->AddEntry(graph[1],"|mean|>5");
    leg->AddEntry(graph[5],"No T0 Cal");
    leg->AddEntry(graph[6],"Wrong fit, RMS and Mean used");
    leg->SetTextSize(0.04);
    
    leg->Draw();
  }
}


//================================================================

class T0CalibTypeXY: public TCanvas{
public:
  T0CalibTypeXY(char*,bool);
};

T0CalibTypeXY::T0CalibTypeXY(char* infile, bool isAr = false){

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* Chiptuple    =       NULL;
  TNtuple* Boardtuple   =       NULL;
  TNtuple* Moduletuple  =       NULL;

  if(!isAr){
    Chiptuple=(TNtuple*)        file->Get("Chiptuple");
    Boardtuple=(TNtuple*)       file->Get("Boardtuple");
    Moduletuple=(TNtuple*)      file->Get("Moduletuple");
  }
  else{
    Chiptuple=(TNtuple*)        file->Get("Chip_Artuple");
    Boardtuple=(TNtuple*)       file->Get("Board_Artuple");
    Moduletuple=(TNtuple*)      file->Get("Module_Artuple");
  }

  
  T0CalTypeXY* hxy1 = new T0CalTypeXY(Chiptuple,-1,0.2);
  T0CalTypeXY* hxy2 = new T0CalTypeXY(Boardtuple,-1,0.5);
  T0CalTypeXY* hxy3 = new T0CalTypeXY(Moduletuple,-1,0.8);
  T0CalTypeXY* hxy4 = new T0CalTypeXY(Chiptuple,1,0.2);
  T0CalTypeXY* hxy5 = new T0CalTypeXY(Boardtuple,1,0.5);
  T0CalTypeXY* hxy6 = new T0CalTypeXY(Moduletuple,1,0.8);

  this->SetName("c1");
  this->cd();
  this->Divide(3,2,0.01,0.01);
  ((TPad*) this->GetPrimitive("c1_1"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_2"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_3"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_4"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_5"))->SetFrameFillColor(1);  
  ((TPad*) this->GetPrimitive("c1_6"))->SetFrameFillColor(1);  
  if(!isAr){
	  this->cd(6);
	  hxy1->Draw("T0 calib type (Chip, side C)",false);
	  this->cd(5);
	  hxy2->Draw("T0 calib type (Board, side C)",false);
	  this->cd(4);
	  hxy3->Draw("T0 calib type (Module, side C)",false);
	  this->cd(3);
	  hxy4->Draw("T0 calib type (Chip, side A)",false);
	  this->cd(2);
	  hxy5->Draw("T0 calib type (Board, side A)",false);
	  this->cd(1);
	  hxy6->Draw("T0 calib type (Module, side A)",true);
 }
  else{
	  this->cd(6);
	  hxy1->Draw("Argon T0 calib type (Chip, side C)",false);
	  this->cd(5);
	  hxy2->Draw("Argon T0 calib type (Board, side C)",false);
	  this->cd(4);
	  hxy3->Draw("Argon T0 calib type (Module, side C)",false);
	  this->cd(3);
	  hxy4->Draw("Argon T0 calib type (Chip, side A)",false);
	  this->cd(2);
	  hxy5->Draw("Argon T0 calib type (Board, side A)",false);
	  this->cd(1);
	  hxy6->Draw("Argon T0 calib type (Module, side A)",true);
 }


}


//================================================================

class ChipVariations: public TProfile{
public:
  ChipVariations(char*, int);
  void DrawLines(float,float);
};

ChipVariations::ChipVariations(char* infile, int detector){

  ofstream txtfile(Form("finedelays_%i.txt",detector),ios::out);

  float t0o,chip,dt0,rt0;
  int nchip[9]={10,11,15,9,9,11,12,13,14};

  this->SetBins(104,0.5,104.5);
  this->SetName(Form("ChipT0Var_%i",detector));

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* Chiptuple=(TNtuple*)file->Get("Chiptuple");

  Chiptuple->SetBranchAddress("rt0",&rt0);
  Chiptuple->SetBranchAddress("t0offset",&t0o);
  Chiptuple->SetBranchAddress("chp",&chip);
  Chiptuple->SetBranchAddress("dt0",&dt0);

  //TGraphErrors* chg = new TGraphErrors();
  //pro = new TProfile("","",104,0.5,104.5);

  int ic=0;
  for (int ibrd=0;ibrd<9;ibrd++){
    
    Chiptuple->Draw(">>tmplist",Form("brd==%i && t0offset!=0 && ftype>1 && det==%i",ibrd,detector));    
    TEventList *elist = (TEventList*)gDirectory->Get("tmplist");
    
    int mchip=0;
    for (int ib=0;ib<elist->GetN();ib++){
      ic++;
      Chiptuple->GetEntry(elist->GetEntry(ib));
      //cout << elist->GetEntry(ib) << " " << t0o << " " << mchip << endl;
      if (ibrd<2) mchip=(int)chip;
      if (ibrd>=2 && ibrd<5) mchip=(int)chip+21;
      if (ibrd>=5) mchip=(int)chip+21+33;
      //chg->SetPoint(ic,mchip,t0o);
      //chg->SetPointError(ic,0, sqrt((2*dt0)*(2*dt0)+(2*0.05)*(2*0.05)) );
      //cout << mchip << " " << t0o << endl;
      this->Fill(mchip,t0o);
    }
  }

  ic=0;
  for (int ibrd=0;ibrd<9;ibrd++){
    for (int ichp=0;ichp<nchip[ibrd];ichp++){      
      int mchip=0;
      if (ibrd==0) mchip=ichp+1;
      if (ibrd==1) mchip=ichp+nchip[ibrd-1]+1;
      if (ibrd==2) mchip=ichp+1;
      if (ibrd==3) mchip=ichp+nchip[ibrd-1]+1;
      if (ibrd==4) mchip=ichp+nchip[ibrd-1]+nchip[ibrd-2]+1;
      if (ibrd==5) mchip=ichp+1;
      if (ibrd==6) mchip=ichp+nchip[ibrd-1]+1;
      if (ibrd==7) mchip=ichp+nchip[ibrd-1]+nchip[ibrd-2]+1;
      if (ibrd==8) mchip=ichp+nchip[ibrd-1]+nchip[ibrd-2]+nchip[ibrd-3]+1;
      txtfile << ibrd << " " << mchip << " " << detector << " " << ic << " " << this->GetBinContent(ic+1) << endl;
      ic++;
    }
  }


  //chg->RemovePoint(0);
  this->SetTitle(Form("Chip T0 offset (phi-mod average, %c-side)",detlet[detector+1]));
  this->SetStats(kFALSE);
  this->SetLineColor(2);
  this->GetYaxis()->SetTitle("T0_{board}-T0_{chip} / ns");
  this->GetXaxis()->SetTitle("chip #");
    
  txtfile.close();
  file->Close();
  delete file;
}

void ChipVariations::DrawLines(float lowe, float upe){
  TLine* lin1 = new TLine(10.5,lowe,10.5,upe); lin1->SetLineStyle(2); lin1->Draw();
  TLine* lin2 = new TLine(21.5,lowe,21.5,upe); lin2->SetLineStyle(2); lin2->Draw();
  TLine* lin3 = new TLine(36.5,lowe,36.5,upe); lin3->SetLineStyle(2); lin3->Draw();
  TLine* lin4 = new TLine(45.5,lowe,45.5,upe); lin4->SetLineStyle(2); lin4->Draw();
  TLine* lin5 = new TLine(54.5,lowe,54.5,upe); lin5->SetLineStyle(2); lin5->Draw();
  TLine* lin6 = new TLine(65.5,lowe,65.5,upe); lin6->SetLineStyle(2); lin6->Draw();
  TLine* lin7 = new TLine(77.5,lowe,77.5,upe); lin7->SetLineStyle(2); lin7->Draw();
  TLine* lin8 = new TLine(90.5,lowe,90.5,upe); lin8->SetLineStyle(2); lin8->Draw();
}


//================================================================

class BoardVariations: public TGraphErrors{
public:
  BoardVariations(char*, int);
  void DrawLines();
  int GetNpoints() {return this->fNpoints;}  
  double* GetX() {return this->fX;}  
  double* GetY() {return this->fY;}  
  float maxx,maxy,minx,miny;
};

BoardVariations::BoardVariations(char* infile, int det){
  
  //cout << infile << endl;
  
  ofstream txtfile(Form("BoardVar_%i.txt",det),ios::out);

  float t0,dt0,bindex;

  minx=0; maxx=289; miny=100; maxy=-25;

  this->SetName(Form("BoardT0Var_%i",det));

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* Boardtuple=(TNtuple*)file->Get("Boardtuple");

  Boardtuple->SetBranchAddress("t0",&t0);
  Boardtuple->SetBranchAddress("dt0",&dt0);

  int ipnt=0;
  for (int imod=0;imod<32;imod++){
    for (int ibrd=0;ibrd<9;ibrd++){

      bindex=9*imod+ibrd;

      Boardtuple->Draw(">>tmplist",Form("brd==%i && det==%i && mod==%i",ibrd,det,imod));
      TEventList *elist = (TEventList*)gDirectory->Get("tmplist");      
      for (int ib=0;ib<elist->GetN();ib++){
	ipnt++;      
	Boardtuple->GetEntry(elist->GetEntry(ib));
	//cout << bindex+1 << " " << t0 << " " << dt0 << endl;
	if (t0<miny) miny = t0;
        if (t0>maxy) maxy = t0;
	this->SetPoint(ipnt,bindex+1,t0);
	this->SetPointError(ipnt,0,dt0);     
	txtfile << det << " " << bindex << " " << t0 << endl;
      }
      
    }
  }

  this->fMaximum=maxy;
  this->fMinimum=miny;
  this->SetTitle(Form("Board T0s (%c-side)",detlet[det+1]));
  this->SetLineColor(4);
  this->RemovePoint(0);

  file->Close();
  delete file;
  txtfile.close();
}

void BoardVariations::DrawLines(){

  this->GetYaxis()->SetTitle("T0_{board} / ns");
  this->GetXaxis()->SetTitle("board #");
  this->GetYaxis()->SetRangeUser(-0.1+miny,0.1+maxy);
  this->GetYaxis()->SetRangeUser(9.0, 10.7);
	miny = 9.0;
	maxy = 10.7;
  this->GetXaxis()->SetRangeUser(minx,maxx);
  this->Draw("AL");
  for (int imod=1;imod<=32;imod++){
    TLine* lin = new TLine((float)imod*9+0.5,miny,(float)imod*9+0.5,maxy); lin->SetLineStyle(2); lin->Draw();
  }

}


//================================================================

class BoardVariationsDiff: public TGraphErrors{
public:
  BoardVariationsDiff(char*,char*, int);
  void DrawLines();
  int GetNpoints() {return this->fNpoints;}  
  double* GetX() {return this->fX;}  
  double* GetY() {return this->fY;}  
  float maxx,maxy,minx,miny;
};

BoardVariationsDiff::BoardVariationsDiff(char* infile_data, char* infile_ref, int det){
  
  //cout << infile << endl;
  
  ofstream txtfile(Form("BoardVarDiff_%i.txt",det),ios::out);

  float t0,dt0,oldt0,bindex;

  minx=0; maxx=289; miny=1000; maxy=-1000;

  this->SetName(Form("BoardT0VarDiff_%i",det));

  TFile* filedata = new TFile(infile_data);
  TFile* fileref = new TFile(infile_ref);
  //cout << infile << endl;
  filedata->cd();
  TNtuple* Boardtuple_data=(TNtuple*)filedata->Get("Boardtuple");
  fileref->cd();
  TNtuple* Boardtuple_ref=(TNtuple*)fileref->Get("Boardtuple");

  Boardtuple_data->SetBranchAddress("t0",&t0);
  Boardtuple_data->SetBranchAddress("dt0",&dt0);
  Boardtuple_ref->SetBranchAddress("oldt0",&oldt0);

  int ipnt=0;
  for (int imod=0;imod<32;imod++){
    for (int ibrd=0;ibrd<9;ibrd++){

      bindex=9*imod+ibrd;

      Boardtuple_data->Draw(">>tmplist",Form("brd==%i && det==%i && mod==%i",ibrd,det,imod));
      TEventList *elist = (TEventList*)gDirectory->Get("tmplist");      
      for (int ib=0;ib<elist->GetN();ib++){
	ipnt++;      
	Boardtuple_data->GetEntry(elist->GetEntry(ib));
	Boardtuple_ref->GetEntry(elist->GetEntry(ib));
	//cout << bindex+1 << " " << t0-oldt0 << " " << miny << " " << maxy << endl;
	if ((t0-oldt0)<miny) miny = t0-oldt0;
        if ((t0-oldt0)>maxy) maxy = t0-oldt0;
	this->SetPoint(ipnt,bindex+1,t0-oldt0);
	this->SetPointError(ipnt,0,dt0);     
	txtfile << det << " " << bindex << " " << t0-oldt0 << endl;
      }
      
    }
  }

  this->fMaximum=maxy+0.1;
  this->fMinimum=miny-0.1;
  this->SetTitle(Form("Board T0 - ref (%c-side)",detlet[det+1]));
  this->SetLineColor(4);
  this->RemovePoint(0);

  filedata->Close();
  delete filedata;
  txtfile.close();
}

void BoardVariationsDiff::DrawLines(){

  this->GetYaxis()->SetTitle("T0_{board}-ref / ns");
  this->GetXaxis()->SetTitle("board #");
  this->GetYaxis()->SetRangeUser(-0.1+miny,0.1+maxy);
  this->GetXaxis()->SetRangeUser(minx,maxx);
  this->Draw("AP");
  TLine* lin = new TLine(minx,0,maxx,0); lin->SetLineColor(2); lin->Draw();
  for (int imod=1;imod<=32;imod++){
    TLine* lin = new TLine((float)imod*9+0.5,this->fMinimum,(float)imod*9+0.5,this->fMaximum); lin->SetLineStyle(2); lin->Draw();
  }

}


//================================================================

class BoardVariationsOldT0: public TGraph{
public:
  BoardVariationsOldT0(char*, int);
  void DrawLines();
  int GetNpoints() {return this->fNpoints;}  
  double* GetX() {return this->fX;}  
  double* GetY() {return this->fY;}  
  float maxx,maxy,minx,miny;
};

BoardVariationsOldT0::BoardVariationsOldT0(char* infile, int det){
  
  //cout << infile << endl;
  
  ofstream txtfile(Form("BoardOldT0Var_%i.txt",det),ios::out);

  float oldt0,bindex;

  minx=0; maxx=289; miny=100; maxy=-25;

  //this->fMaximum=maxy;
  //this->fMinimum=miny;
  this->SetName(Form("BoardOldT0Var_%i",det));

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* Boardtuple=(TNtuple*)file->Get("Boardtuple");

  Boardtuple->SetBranchAddress("oldt0",&oldt0);


  int ipnt=0;
  for (int imod=0;imod<32;imod++){
    for (int ibrd=0;ibrd<9;ibrd++){

      bindex=9*imod+ibrd;

      Boardtuple->Draw(">>tmplist",Form("brd==%i && det==%i && mod==%i",ibrd,det,imod));
      TEventList *elist = (TEventList*)gDirectory->Get("tmplist");      
      for (int ib=0;ib<elist->GetN();ib++){
	ipnt++;      
	Boardtuple->GetEntry(elist->GetEntry(ib));
	//cout << bindex+1 << " " << oldt0 << " " << dt0 << endl;
	if (oldt0<miny) miny = oldt0;
        if (oldt0>maxy) maxy = oldt0;
	this->SetPoint(ipnt,bindex+1,oldt0);
	txtfile << det << " " << bindex << " " << oldt0 << endl;
      }
      
    }
  }

  this->fMaximum=maxy;
  this->fMinimum=miny;
  this->SetTitle(Form("Board T0s (%c-side)",detlet[det+1]));
  this->SetLineColor(4);
  this->RemovePoint(0);
  
  this->GetYaxis()->SetRangeUser(9,10.7);

  file->Close();
  delete file;
  txtfile.close();
}

void BoardVariationsOldT0::DrawLines(){

  this->GetYaxis()->SetTitle("T0_{board} / ns");
  this->GetXaxis()->SetTitle("board #");
   miny = 9.0;
   maxy = 10.6;
  this->GetYaxis()->SetRangeUser( miny,maxy);
  this->GetXaxis()->SetRangeUser(minx,maxx);
  this->Draw("AL");
  for (int imod=1;imod<=32;imod++){
    TLine* lin = new TLine((float)imod*9+0.5,miny,(float)imod*9+0.5,maxy); lin->SetLineStyle(2); lin->Draw();
  }

}


//================================================================
// Board distributions for time residual

class BoardVariationsRes: public TGraphErrors{
public:
  BoardVariationsRes(char*, int);
  void DrawLines();
  int GetNpoints() {return this->fNpoints;}
  double* GetX() {return this->fX;}
  double* GetY() {return this->fY;}
  float maxx,maxy,minx,miny;
};

BoardVariationsRes::BoardVariationsRes(char* infile, int det){

  ofstream txtfile(Form("BoardVarRes_%i.txt",det),ios::out);

  float t0,dt0,bindex;

  minx=0; maxx=289; miny=100; maxy=-100;

  this->SetName(Form("BoardT0Var_%i",det));

  TFile* file = new TFile(infile);
  //cout << infile << endl;

  file->cd();
  TNtuple* BoardtupleRes=(TNtuple*)file->Get("Boardtuple");

  BoardtupleRes->SetBranchAddress("res",&t0);
  BoardtupleRes->SetBranchAddress("dres",&dt0);

  int ipnt=0;
  for (int imod=0;imod<32;imod++){
    for (int ibrd=0;ibrd<9;ibrd++){

      bindex=9*imod+ibrd;

      BoardtupleRes->Draw(">>tmplist",Form("brd==%i && det==%i && mod==%i",ibrd,det,imod));
      TEventList *elist = (TEventList*)gDirectory->Get("tmplist");
      for (int ib=0;ib<elist->GetN();ib++){
        ipnt++;
        BoardtupleRes->GetEntry(elist->GetEntry(ib));
        //cout << bindex+1 << " " << t0 << " " << dt0 << endl;
        if (t0<miny) miny = t0;
        if (t0>maxy) maxy = t0;
        this->SetPoint(ipnt,bindex+1,t0);
        this->SetPointError(ipnt,0,dt0);
	txtfile << det << " " << bindex << " " << t0 << endl;
      }

    }
  }

  this->fMaximum=0.01+maxy;
  this->fMinimum=-0.01+miny;
  this->SetTitle(Form("Board Residuals (%c-side)",detlet[det+1]));
  this->SetLineColor(4);
  this->RemovePoint(0);
  this->GetYaxis()->SetRangeUser(0.125, 0.165);

  file->Close();
  delete file;
  txtfile.close();
}

void BoardVariationsRes::DrawLines(){

  this->GetYaxis()->SetTitle("Residual{board} / um");
  this->GetXaxis()->SetTitle("board #");
	miny = 0.124;
	maxy = 0.166;
  this->GetYaxis()->SetRangeUser( miny, maxy);
  this->GetXaxis()->SetRangeUser(minx,maxx);
  this->Draw("AL");
  for (int imod=1;imod<=32;imod++){
    TLine* lin = new TLine((float)imod*9+0.5,miny,(float)imod*9+0.5,maxy); lin->SetLineStyle(2); lin->Draw();
  }

}


//================================================================
// DO board distributions for Sigma time residual

class BoardVariationsTRes1: public TGraphErrors{
public:
  BoardVariationsTRes1(char*, int);
  void DrawLines();
  int GetNpoints() {return this->fNpoints;}
  double* GetX() {return this->fX;}
  double* GetY() {return this->fY;}
  float maxx,maxy,minx,miny;
};

BoardVariationsTRes1::BoardVariationsTRes1(char* infile, int det){

  ofstream txtfile(Form("BoardVarTRes1_%i.txt",det),ios::out);

  float t0,oldt0,dt0,bindex,t0offset ;

  minx=0; maxx=289; miny=100; maxy=-100;

  this->SetName(Form("BoardT0Var_%i",det));

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* BoardtupleTRes=(TNtuple*)file->Get("Boardtuple");

  BoardtupleTRes->SetBranchAddress("t0",&t0);
  BoardtupleTRes->SetBranchAddress("oldt0",&oldt0);
  BoardtupleTRes->SetBranchAddress("dt0",&dt0);
  BoardtupleTRes->SetBranchAddress("t0offset",&t0offset);

  int ipnt=0;
  for (int imod=0;imod<32;imod++){
    for (int ibrd=0;ibrd<9;ibrd++){

      bindex=9*imod+ibrd;

      BoardtupleTRes->Draw(">>tmplist",Form("brd==%i && det==%i && mod==%i",ibrd,det,imod));
      TEventList *elist = (TEventList*)gDirectory->Get("tmplist");
      for (int ib=0;ib<elist->GetN();ib++){
        ipnt++;
        BoardtupleTRes->GetEntry(elist->GetEntry(ib));
        if ((t0-oldt0)<miny) miny = t0-oldt0;
        if ((t0-oldt0)>maxy) maxy = t0-oldt0;
        //cout << bindex+1 << " " << t0 << " " << dt0 << endl;
//        this->SetPoint(ipnt,bindex+1,t0-oldt0+t0offset);
        this->SetPoint(ipnt,bindex+1,t0-oldt0);
        this->SetPointError(ipnt,0,dt0);
	txtfile << det << " " << bindex << " " << t0-oldt0 << endl;
      }

    }
  }

  this->fMaximum=0.1+maxy;
  this->fMinimum=-0.1+miny;
  this->SetTitle(Form("Board T0 Difference (%c-side)",detlet[det+1]));
  this->SetLineColor(4);
  this->RemovePoint(0);
  this->GetYaxis()->SetRangeUser(9,10.7);

  file->Close();
  delete file;
  txtfile.close();
}

void BoardVariationsTRes1::DrawLines(){
  
  this->GetYaxis()->SetTitle("Board T0 Difference / ns");
  this->GetXaxis()->SetTitle("board #");
	miny = -0.5;
	maxy = 0.5;
  this->GetYaxis()->SetRangeUser( miny, maxy);
  this->GetXaxis()->SetRangeUser(minx,maxx);
  this->Draw("AL");
  TLine* lin = new TLine(minx,0,maxx,0); lin->Draw();
  TLine* lin1 = new TLine(minx,0.1,maxx,0.1); lin1->SetLineStyle(2); lin1->Draw();
  TLine* lin2 = new TLine(minx,-0.1,maxx,-0.1); lin2->SetLineStyle(2); lin2->Draw();
  for (int imod=1;imod<=32;imod++){
    TLine* lin = new TLine((float)imod*9+0.5,miny,(float)imod*9+0.5,maxy); lin->SetLineStyle(2); lin->Draw();
    //TLine* lin = new TLine((float)imod*9+0.5,this->fMinimum,(float)imod*9+0.5,this->fMaximum); lin->SetLineStyle(2); lin->Draw();
  }

}


//================================================================
// DO board distributions for Sigma time residual

class BoardVariationsTRes: public TGraphErrors{
public:
  BoardVariationsTRes(char*, int);
  void DrawLines();
  int GetNpoints() {return this->fNpoints;}
  double* GetX() {return this->fX;}
  double* GetY() {return this->fY;}
  float maxx,maxy,minx,miny;
};

BoardVariationsTRes::BoardVariationsTRes(char* infile, int det){

  ofstream txtfile(Form("BoardVarTRes_%i.txt",det),ios::out);

  float t0,dt0,bindex;

  minx=0; maxx=289; miny=100; maxy=-100;

  this->SetName(Form("BoardT0Var_%i",det));

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* BoardtupleTRes=(TNtuple*)file->Get("Boardtuple");

  BoardtupleTRes->SetBranchAddress("tres",&t0);
  BoardtupleTRes->SetBranchAddress("dres",&dt0);

  int ipnt=0;
  for (int imod=0;imod<32;imod++){
    for (int ibrd=0;ibrd<9;ibrd++){

      bindex=9*imod+ibrd;

      BoardtupleTRes->Draw(">>tmplist",Form("brd==%i && det==%i && mod==%i",ibrd,det,imod));
      TEventList *elist = (TEventList*)gDirectory->Get("tmplist");
      for (int ib=0;ib<elist->GetN();ib++){
        ipnt++;
        BoardtupleTRes->GetEntry(elist->GetEntry(ib));
        //cout << bindex+1 << " " << t0 << " " << dt0 << endl;
        this->SetPoint(ipnt,bindex+1,t0);
        this->SetPointError(ipnt,0,dt0);
        if (t0<miny) miny = t0;
        if (t0>maxy) maxy = t0;
	txtfile << det << " " << bindex << " " << t0 << endl;
      }

    }
  }

  this->fMaximum=maxy;
  this->fMinimum=miny;
  this->SetTitle(Form("Board Sigma Time Residuals (%c-side)",detlet[det+1]));
  this->SetLineColor(4);
  this->RemovePoint(0);

  file->Close();
  delete file;
  txtfile.close();
}

void BoardVariationsTRes::DrawLines(){

  this->GetYaxis()->SetTitle("Board Sigma Time Residuals/ ns");
  this->GetXaxis()->SetTitle("board #");
	miny = 3.0;
	maxy = 3.7;
  this->GetYaxis()->SetRangeUser(miny,maxy);
  this->GetXaxis()->SetRangeUser(minx,maxx);
  this->Draw("ALP");
  TLine* lin = new TLine(minx,0,maxx,0); lin->Draw();
  TLine* lin1 = new TLine(minx,0.1,maxx,0.1); lin1->SetLineStyle(2); lin1->Draw();
  TLine* lin2 = new TLine(minx,-0.1,maxx,-0.1); lin2->SetLineStyle(2); lin2->Draw();
  for (int imod=1;imod<=32;imod++){
    //TLine* lin = new TLine((float)imod*9+0.5,this->fMinimum,(float)imod*9+0.5,this->fMaximum); lin->SetLineStyle(2); lin->Draw();
    TLine* lin = new TLine((float)imod*9+0.5,miny,(float)imod*9+0.5,maxy); lin->SetLineStyle(2); lin->Draw();

  }

}


//================================================================

class DvGraph: public TGraph{
public: 
  DvGraph(char*,char*, char*,int,int,bool);
  //TH1F* hist;
  TGraph* rtgraph;
};

//DvGraph::DvGraph(char* infile, char* path,int det, int lay, bool isinverted){
DvGraph::DvGraph(char* infile, char* path, char* folder, int det, int lay, bool isinverted){
  
  this->SetName(Form("Dv_%i_%i",det,lay));
  
  vector<string> levels;
  string dum=string(path);
  while (true){
    if((int)dum.find(",")<0) break;
    levels.push_back(dum.substr(0,dum.find(",")));
    dum=dum.substr(dum.find(",")+1,dum.size());
  }
  levels.push_back(dum.substr(0,dum.find(",")));
  //for (int i=0; i<levels.size(); i++) cout << " " << levels.at(i);
  //cout << endl;
  
  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  
  TDirectory* trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
  if (folder!=""){
    TDirectory* det = (TDirectory*) trt->FindKey(folder)->ReadObj();
    //cout << "PLOT FOR "<< folder <<endl;
    if (det->FindKey("rtgraph")){
      rtgraph= (TGraphErrors*) det  ->FindKey("rtgraph")->ReadObj();
    }
    else
      rtgraph=NULL;
  }
  
  else if (trt->FindKey("rtgraph")){
    rtgraph= (TGraphErrors*) trt  ->FindKey("rtgraph")->ReadObj();
  }
  else{
    rtgraph=NULL;
  }
  
  /*
    TDirectory* trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
    TDirectory* det1 = (TDirectory*) trt->FindKey("Detector_-1")->ReadObj();
    TDirectory* det2 = (TDirectory*) trt->FindKey("Detector_1")->ReadObj();
    TDirectory* lay10 = (TDirectory*) det1->FindKey("Layer_-1_0")->ReadObj();
    TDirectory* lay11 = (TDirectory*) det1->FindKey("Layer_-1_1")->ReadObj();
    TDirectory* lay12 = (TDirectory*) det1->FindKey("Layer_-1_2")->ReadObj();
    TDirectory* lay20 = (TDirectory*) det2->FindKey("Layer_1_0")->ReadObj();
    TDirectory* lay21 = (TDirectory*) det2->FindKey("Layer_1_1")->ReadObj();
    TDirectory* lay22 = (TDirectory*) det2->FindKey("Layer_1_2")->ReadObj();
    
    TGraph* trgraph=NULL;
    if (det==-2 && lay==-1) {
    if (trt  ->FindKey("rtgraph")) trgraph = (TGraph*) trt->FindKey("trgraph")->ReadObj(); 
    }
    
    if (det==-2 && lay==-1) {if (trt  ->FindKey("rtgraph")) rtgraph= (TGraph*) trt  ->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det==-1 && lay==-1) {if (det1 ->FindKey("rtgraph")) rtgraph= (TGraph*) det1 ->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det== 1 && lay==-1) {if (det2 ->FindKey("rtgraph")) rtgraph= (TGraph*) det2 ->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det==-1 && lay== 0) {if (lay10->FindKey("rtgraph")) rtgraph= (TGraph*) lay10->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det==-1 && lay== 1) {if (lay11->FindKey("rtgraph")) rtgraph= (TGraph*) lay11->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det==-1 && lay== 2) {if (lay12->FindKey("rtgraph")) rtgraph= (TGraph*) lay12->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det== 1 && lay== 0) {if (lay20->FindKey("rtgraph")) rtgraph= (TGraph*) lay20->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det== 1 && lay== 1) {if (lay21->FindKey("rtgraph")) rtgraph= (TGraph*) lay21->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
    if (det== 1 && lay== 2) {if (lay22->FindKey("rtgraph")) rtgraph= (TGraph*) lay22->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  */
  if(rtgraph){
    
    TF1* rtfunc;
    if (rtgraph) rtfunc = (TF1*)rtgraph->GetListOfFunctions()->First();
    
    double p0=rtfunc->GetParameter(0);
    double p1=rtfunc->GetParameter(1);
    double p2=rtfunc->GetParameter(2);
    double p3=rtfunc->GetParameter(3);
    
    //cout << "  R-t PARAMETERS: " << p0 << " " << p1 << " " << p2 << " " << p3 << endl; 
    
    bool isdines=false;
    //bool isdines=(string(trgraph->GetTitle()).find("Dines")!=string::npos);
    //cout << string(trgraph->GetTitle()) << endl;
    
    if(isdines){
      //cout << "  USING DINES R-t RELATION" << endl;  
      //char* trrelation = "[1] - abs([2]*[0]) + [2] * sqrt([0]*[0] + x*x)";
      //char* rtrelation = "sqrt( ( (x-([1]-abs([2]*[0])))*(x-([1]-abs([2]*[0])))/([2]*[2]) ) - [0]*[0])";
      //char* vtrelation = "(x-[1]+abs([2]*[0]))/( [2]*[2]*sqrt( ( (x-([1]-abs([2]*[0])))*(x-([1]-abs([2]*[0])))/([2]*[2]) ) - [0]*[0])  )";
      char* vrrelation;
      if (isinverted) vrrelation = "x*[2]/sqrt([0]*[0]+x*x)";
      else vrrelation = "sqrt([0]*[0]+x*x)/(x*[2])";
      
      TF1* vr1 = new TF1("vr-relation",vrrelation,0.0001,2.0);
      vr1->SetParameters(p0,p1,p2);
      
      for (int i=0; i<=100; i++) {
	float r=0.1+(2.0-0.1)*(float)i/100;
	float v1=vr1->Eval(r,0,0,0);
	this->SetPoint(i,r,v1);
	//cout << i << " " << r << " " << v1 << endl; 
      }
      
      vr1->Delete();
      
    } else {
      //cout << "  USING POLYNOMIAL R-t RELATION" << endl;  
      
      char* rtrelation="[0]+x*([1]+x*([2]+x*[3]))";
      char* vtrelation;
      if (isinverted) vtrelation="1/([0]+2*x*[1]+3*x*x*[2])";
      else  vtrelation="[0]+2*x*[1]+3*x*x*[2]";
      
      TF1* rt1 = new TF1("rt-relation",rtrelation,-10000,10000);
      rt1->SetParameters(p0,p1,p2,p3);
      TF1* vt1 = new TF1("vt-relation",vtrelation,-10000,10000);
      vt1->SetParameters(p1,p2,p3);
      
      for (int i=0; i<=100; i++) {
	float r=0.1+(2.0-0.1)*(float)i/100;      
	float t1=rt1->GetX(r,-10000,10000); 
	float v1=vt1->Eval(t1,0,0,0);
	this->SetPoint(i,r,v1);
	//cout << i << " " << r << " " << t1 << " " << v1 << endl; 
      }
      
      rt1->Delete();  
      vt1->Delete();
      
    }
  }
}


//================================================================

class DGraph: public TGraph{
public: 
  DGraph(char*,char*, char*,int,int,bool);
  //TH1F* hist;
  TGraph* rtgraph;
};

//DvGraph::DvGraph(char* infile, char* path,int det, int lay, bool isinverted){
DGraph::DGraph(char* infile, char* path, char* folder, int det, int lay, bool isinverted){
  
  this->SetName(Form("Dv_%i_%i",det,lay));
  
  vector<string> levels;
  string dum=string(path);
  while (true){
    if((int)dum.find(",")<0) break;
    levels.push_back(dum.substr(0,dum.find(",")));
    dum=dum.substr(dum.find(",")+1,dum.size());
  }
  levels.push_back(dum.substr(0,dum.find(",")));

  TFile* file = new TFile(infile);
  file->cd();

TDirectory* trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
if (folder!=""){
  TDirectory* det = (TDirectory*) trt->FindKey(folder)->ReadObj();
  //cout << "PLOT FOR "<< folder <<endl;
                        if (det->FindKey("rtgraph")){
                                rtgraph= (TGraphErrors*) det  ->FindKey("rtgraph")->ReadObj();
                        }
                        else
                                rtgraph=NULL;
   }

else if (trt->FindKey("rtgraph")){
                                rtgraph= (TGraphErrors*) trt  ->FindKey("rtgraph")->ReadObj();
 }
else{
    rtgraph=NULL;
}

  if(rtgraph){

    TF1* rtfunc;
    if (rtgraph) rtfunc = (TF1*)rtgraph->GetListOfFunctions()->First();
    
    double p0=rtfunc->GetParameter(0);
    double p1=rtfunc->GetParameter(1);
    double p2=rtfunc->GetParameter(2);
    double p3=rtfunc->GetParameter(3);
    
    //cout << "  R-t PARAMETERS: " << p0 << " " << p1 << " " << p2 << " " << p3 << endl; 
    
    bool isdines=false;
    //bool isdines=(string(trgraph->GetTitle()).find("Dines")!=string::npos);
    //cout << string(trgraph->GetTitle()) << endl;
    
    if(isdines){
      //cout << "  USING DINES R-t RELATION" << endl;  
      //char* trrelation = "[1] - abs([2]*[0]) + [2] * sqrt([0]*[0] + x*x)";
      //char* rtrelation = "sqrt( ( (x-([1]-abs([2]*[0])))*(x-([1]-abs([2]*[0])))/([2]*[2]) ) - [0]*[0])";
      //char* vtrelation = "(x-[1]+abs([2]*[0]))/( [2]*[2]*sqrt( ( (x-([1]-abs([2]*[0])))*(x-([1]-abs([2]*[0])))/([2]*[2]) ) - [0]*[0])  )";
      char* vrrelation;
      if (isinverted) vrrelation = "x*[2]/sqrt([0]*[0]+x*x)";
      else vrrelation = "sqrt([0]*[0]+x*x)/(x*[2])";
      
      TF1* vr1 = new TF1("vr-relation",vrrelation,0.0001,2.0);
      vr1->SetParameters(p0,p1,p2);
      
      for (int i=0; i<=100; i++) {
	float r=0.1+(2.0-0.1)*(float)i/100;
	float v1=vr1->Eval(r,0,0,0);
	this->SetPoint(i,r,v1);
	//cout << i << " " << r << " " << v1 << endl; 
      }
      
      vr1->Delete();
      
    } else {
      //cout << "  USING POLYNOMIAL R-t RELATION" << endl;  
      
      char* rtrelation="[0]+x*([1]+x*([2]+x*[3]))";
      char* vtrelation;
      if (isinverted) vtrelation="1/([0]+2*x*[1]+3*x*x*[2])";
      else  vtrelation="[0]+2*x*[1]+3*x*x*[2]";
      
      TF1* rt1 = new TF1("rt-relation",rtrelation,-10000,10000);
      rt1->SetParameters(p0,p1,p2,p3);
      TF1* vt1 = new TF1("vt-relation",vtrelation,-10000,10000);
      vt1->SetParameters(p1,p2,p3);
     /* 
      for (int i=0; i<=100; i++) {
	float r=0.1+(2.0-0.1)*(float)i/100;      
	float t1=rt1->GetX(r,-10000,10000); 
	float v1=vt1->Eval(t1,0,0,0);
	this->SetPoint(i,r,v1);
	//this->SetPoint(i,r,rt1);
	//cout << i << " " << r << " " << t1 << " " << v1 << endl; 
      }
*/
      for (int i=0; i<=1000; i++) {
       	float t1= -5+ i * 0.05 ; 
        float r1=rt1->Eval(t1,0,0,0);
        this->SetPoint(i,t1,r1);
        //this->SetPoint(i,r,rt1);
        //cout << i << " " <<  t1 << " " << r1 << endl; 
      }
      
      rt1->Delete();  
      vt1->Delete();
      
    }
  }
}


//================================================================

class RtGraphs: public TCanvas{
public:
 // RtGraphs(char*,int,int);
  RtGraphs(char*,char*,bool) throw(string);
  TH2F* rthist;
  TGraphErrors* rtgraph;
  TGraphErrors* trgraph;
  TF1* oldrtfunc;
  //  map<double,double> rtmap;
};

RtGraphs::RtGraphs(char* infile, char* folder, bool isAr = false) throw(string){


  //this->SetName(Form("Rt_%i_%i",det,lay));
  this->SetName(Form("Rt_%is",folder));

  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TDirectory* trt = NULL;

  map<string,string> titlemap;
  if(!isAr){
          trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
          titlemap["WholeBarrel_1"]="whole barrel";
          titlemap["Detector_-1"]="barrel C";
          titlemap["Detector_1"]="barrel A";
          titlemap["Detector_-2"]="end-cap C";
          titlemap["Detector_2"]="end-cap A";
  }

  else{
          trt = (TDirectory*) file->FindKey("TRT_Ar_all")->ReadObj();
          titlemap["WholeBarrel_Ar_1"]  ="whole Ar barrel";
          titlemap["Detector_Ar_-1"]    ="barrel Ar C";
          titlemap["Detector_Ar_1"]     ="barrel Ar A";
          titlemap["Detector_Ar_-2"]    ="end-cap Ar C";
          titlemap["Detector_Ar_2"]     ="end-cap Ar A";
  }


  if (folder!=""){
    //cout << "  PLOT FOR "<< folder <<endl;
    TDirectory* det = (TDirectory*) trt->FindKey(folder)->ReadObj();
    if (det->FindKey("rt-relation")){
      rthist= (TH2F*) det->FindKey("rt-relation")->ReadObj();
      this->cd();
      rthist->GetXaxis()->SetRangeUser(0,40);
      char name[500];  sprintf(name,"rt (%s)",titlemap[string(folder)].data());
      rthist->SetTitle(name);
      if (det->FindKey("rtgraph")){
	rtgraph= (TGraphErrors*) det  ->FindKey("rtgraph")->ReadObj();
      }
      else 
	rtgraph=NULL;
      if (det->FindKey("trgraph")){
	trgraph= (TGraphErrors*) det  ->FindKey("trgraph")->ReadObj();
      }
      else
	trgraph=NULL;
      if (det->FindKey("oldrtfunc")){
	oldrtfunc= (TF1*) det  ->FindKey("oldrtfunc")->ReadObj();
      }
      else
	oldrtfunc=NULL;
    }
    else {
      throw string("  NO RT OBJECTS FOUND");
    }

  }
  else if (trt->FindKey("rt-relation")){
    //cout << "  PLOT FOR TRT"<< endl;
    rthist= (TH2F*) trt->FindKey("rt-relation")->ReadObj();
    this->cd();
    rthist->GetXaxis()->SetRangeUser(0,40);
    rthist->SetTitle("r(t) for whole TRT");
    if (trt->FindKey("rtgraph")){
      rtgraph= (TGraphErrors*) trt  ->FindKey("rtgraph")->ReadObj();
    } 
    else 
      rtgraph = NULL;
    if (trt->FindKey("trgraph")){
      trgraph= (TGraphErrors*) trt  ->FindKey("trgraph")->ReadObj();
    }
    else
      trgraph=NULL; 
    if (trt->FindKey("oldrtfunc")){
      oldrtfunc= (TF1*) trt  ->FindKey("oldrtfunc")->ReadObj();
    }
    else 
      oldrtfunc=NULL;
  }
  else{
    rthist=NULL;
    rtgraph=NULL;
    oldrtfunc=NULL;
  }
  

  /*
    TDirectory* trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
    TDirectory* det1 = (TDirectory*) trt->FindKey("Detector_-1")->ReadObj();
  TDirectory* det2 = (TDirectory*) trt->FindKey("Detector_1")->ReadObj();
  TDirectory* lay10 = (TDirectory*) det1->FindKey("Layer_-1_0")->ReadObj();
  TDirectory* lay11 = (TDirectory*) det1->FindKey("Layer_-1_1")->ReadObj();
  TDirectory* lay12 = (TDirectory*) det1->FindKey("Layer_-1_2")->ReadObj();
  TDirectory* lay20 = (TDirectory*) det2->FindKey("Layer_1_0")->ReadObj();
  TDirectory* lay21 = (TDirectory*) det2->FindKey("Layer_1_1")->ReadObj();
  TDirectory* lay22 = (TDirectory*) det2->FindKey("Layer_1_2")->ReadObj();
  
  if (det==-2 && lay==-1) {if (trt  ->FindKey("rt-relation")) rthist= (TH1F*) trt  ->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det==-1 && lay==-1) {if (det1 ->FindKey("rt-relation")) rthist= (TH1F*) det1 ->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det== 1 && lay==-1) {if (det2 ->FindKey("rt-relation")) rthist= (TH1F*) det2 ->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det==-1 && lay== 0) {if (lay10->FindKey("rt-relation")) rthist= (TH1F*) lay10->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det==-1 && lay== 1) {if (lay11->FindKey("rt-relation")) rthist= (TH1F*) lay11->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det==-1 && lay== 2) {if (lay12->FindKey("rt-relation")) rthist= (TH1F*) lay12->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det== 1 && lay== 0) {if (lay20->FindKey("rt-relation")) rthist= (TH1F*) lay20->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det== 1 && lay== 1) {if (lay21->FindKey("rt-relation")) rthist= (TH1F*) lay21->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}
  if (det== 1 && lay== 2) {if (lay22->FindKey("rt-relation")) rthist= (TH1F*) lay22->FindKey("rt-relation")->ReadObj(); else rthist=NULL;}  

  if (det==-2 && lay==-1) {if (trt  ->FindKey("rtgraph")) rtgraph= (TGraphErrors*) trt  ->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det==-1 && lay==-1) {if (det1 ->FindKey("rtgraph")) rtgraph= (TGraphErrors*) det1 ->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det== 1 && lay==-1) {if (det2 ->FindKey("rtgraph")) rtgraph= (TGraphErrors*) det2 ->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det==-1 && lay== 0) {if (lay10->FindKey("rtgraph")) rtgraph= (TGraphErrors*) lay10->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det==-1 && lay== 1) {if (lay11->FindKey("rtgraph")) rtgraph= (TGraphErrors*) lay11->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det==-1 && lay== 2) {if (lay12->FindKey("rtgraph")) rtgraph= (TGraphErrors*) lay12->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det== 1 && lay== 0) {if (lay20->FindKey("rtgraph")) rtgraph= (TGraphErrors*) lay20->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det== 1 && lay== 1) {if (lay21->FindKey("rtgraph")) rtgraph= (TGraphErrors*) lay21->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}
  if (det== 1 && lay== 2) {if (lay22->FindKey("rtgraph")) rtgraph= (TGraphErrors*) lay22->FindKey("rtgraph")->ReadObj(); else rtgraph=NULL;}  

  if (det==-2 && lay==-1) {if (trt  ->FindKey("trgraph")) trgraph= (TGraphErrors*) trt  ->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det==-1 && lay==-1) {if (det1 ->FindKey("trgraph")) trgraph= (TGraphErrors*) det1 ->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det== 1 && lay==-1) {if (det2 ->FindKey("trgraph")) trgraph= (TGraphErrors*) det2 ->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det==-1 && lay== 0) {if (lay10->FindKey("trgraph")) trgraph= (TGraphErrors*) lay10->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det==-1 && lay== 1) {if (lay11->FindKey("trgraph")) trgraph= (TGraphErrors*) lay11->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det==-1 && lay== 2) {if (lay12->FindKey("trgraph")) trgraph= (TGraphErrors*) lay12->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det== 1 && lay== 0) {if (lay20->FindKey("trgraph")) trgraph= (TGraphErrors*) lay20->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det== 1 && lay== 1) {if (lay21->FindKey("trgraph")) trgraph= (TGraphErrors*) lay21->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}
  if (det== 1 && lay== 2) {if (lay22->FindKey("trgraph")) trgraph= (TGraphErrors*) lay22->FindKey("trgraph")->ReadObj(); else trgraph=NULL;}  

  if (det==-2 && lay==-1) {if (trt  ->FindKey("oldrtfunc")) oldrtfunc= (TF1*) trt  ->FindKey("oldrtfunc")->ReadObj(); else trgraph=NULL;}
*/
  
  this->cd();
  this->Divide(2,1,0.01,0.01);
  this->cd(1);
  
  TPad*  c1_3 = new TPad("c1_3", "newpad",0.01,0.33,0.99,0.99);
  c1_3->Draw();
  c1_3->cd();
  c1_3->SetTopMargin(0.1);
  c1_3->SetBottomMargin(0.01);
  c1_3->SetRightMargin(0.1);
  c1_3->SetFillStyle(0);
  c1_3->SetGrid();
  

  if(rtgraph){
    
    double rgraph,tgraph;
    //for (int ipnt=0; ipnt<=rtgraph->GetN(); ipnt++){
    //  rtgraph->GetPoint(ipnt,rgraph,tgraph);
    //  rtmap[tgraph]=rgraph;
    //}

  
    //gStyle->SetOptFit(0);    
    if (folder == "" )    rtgraph->SetTitle("r(t) fit (whole TRT)");
    else{
      char name[500];  sprintf(name,"r(t) (%s)",titlemap[string(folder)].data());
      rtgraph->SetTitle(name);
    }
    rtgraph->GetXaxis()->SetTitle("t-T0/ns");
    rtgraph->GetYaxis()->SetTitle("|r|_{track}/mm");
    //rtgraph->GetYaxis()->SetRangeUser(0,2.7);
    rtgraph->GetXaxis()->SetTitleSize(0.06);
    rtgraph->GetXaxis()->SetLabelSize(0.06);
    

    oldrtfunc->SetLineColor(1);
    oldrtfunc->SetLineWidth(1);
    oldrtfunc->SetLineStyle(2);
    //Blue line too wide, extract and plot again:
    TF1* newRT;
    if(rtgraph->GetFunction("rtfunc")) newRT = rtgraph->GetFunction("rtfunc");
    else newRT = rtgraph->GetFunction("rtfunc2");
    newRT->SetLineWidth(1);
    newRT->Draw();
    rtgraph->SetMarkerStyle(20);
    rtgraph->SetMarkerSize(0.5);
    rtgraph->GetYaxis()->SetRangeUser(-0.05,2.6);
    rtgraph->Draw("ap");
    oldrtfunc->Draw("same");
    //Legend

    TLegend *leg = new TLegend(0.7155172,0.1038136,0.8965517,0.2542373,NULL,"brNDC");
    leg->AddEntry(rtgraph,"Data","p");
    leg->AddEntry(newRT,"Fit","l");
    leg->AddEntry(oldrtfunc,"Fit old rt","l");
    leg->SetTextSize(0.03);
    leg->Draw();
    
    

    //Plot the difference between old and new rt:
    if(oldrtfunc){
      this->cd();
      this->cd(1);
      TPad*  c1_4 = new TPad("c1_4", "newpad",0.01,0,0.99,0.32);
      c1_4->Draw();
      c1_4->cd();
      c1_4->SetTopMargin(0.01);
      c1_4->SetBottomMargin(0.3);
      c1_4->SetRightMargin(0.1);
      c1_4->SetFillStyle(0);
      c1_4->SetGrid();
      
      Double_t Xmax =  rtgraph->GetXaxis()->GetXmax();
      Double_t Xmin =  rtgraph->GetXaxis()->GetXmin();
      
      Int_t n = 300;
      Double_t x[n], y[n], dx;
      dx = (Xmax-Xmin)/n;
      
      Double_t Ymax = -10;
      Double_t Ymin = 10;
      
      
      for (Int_t i=0; i<n; i++) {
	x[i] = Xmin+i*dx;
	//y[i] = (newRT->Eval(x[i]) - oldrtfunc->Eval(x[i]));
	y[i] = ( oldrtfunc->Eval(x[i]) - newRT->Eval(x[i]));
	if (y[i] > Ymax ) Ymax = y[i];
	if (y[i] < Ymin ) Ymin = y[i];
      }
      
      TGraph *gr1 = new TGraph (n, x, y);
      
      //gStyle->SetOptTitle(0);
      gr1->SetTitle("");
      gr1->GetXaxis()->SetTitle("t-T0/ns");
      gr1->GetXaxis()->SetRangeUser(Xmin,Xmax);
      gr1->GetYaxis()->SetTitle("Old r(t) - New r(t) /mm");
      gr1->GetYaxis()->SetRangeUser(-0.1,0.1);
      //gr1->GetYaxis()->SetRangeUser(1.1*Ymin,1.1*Ymax);
      gr1->GetYaxis()->SetNdivisions(5);
      gr1->GetXaxis()->SetTitleSize(0.06);
      gr1->GetXaxis()->SetLabelSize(0.06);
      gr1->GetYaxis()->SetLabelSize(0.08);
      gr1->GetYaxis()->SetTitleOffset(0.8);
      gr1->GetYaxis()->SetTitleSize(0.06);
      gr1->SetLineStyle(2);
      
    
      //Also add the difference between the fit and the points of the fit
      
      Int_t n1 = rtgraph->GetN();
      Double_t x1[n1], y1[n1],ex1[n1], ey1[n1], X, Y;
      
      for (Int_t i=0; i<n1; i++) {
	X = 0;
	Y = 0;
	rtgraph->GetPoint(i, X, Y) ;
	x1[i] = X;
	//y1[i] = newRT->Eval(X)  - Y;
	y1[i] = Y - newRT->Eval(X);
	ex1[i] = 0;
	ey1[i] = rtgraph->GetErrorY(i);
	if (y1[i] > Ymax ) Ymax = y1[i];
	if (y1[i] < Ymin ) Ymin = y1[i];
	
      }
      //gr1->Draw("APL");
      
      
      TGraphErrors *gr2 = new TGraphErrors (n1, x1, y1, ex1,ey1);
      gr2->SetMarkerColor(2);
      gr2->SetMarkerStyle(20);
      gr2->SetTitle("");
      gr2->GetYaxis()->SetTitle("Old r(t) - New r(t) /mm");

      //gr2->GetYaxis()->SetRangeUser(1.1*Ymin,1.1*Ymax);
      gr2->GetYaxis()->SetRangeUser(-0.2,0.2);
      gr2->GetXaxis()->SetRangeUser(-5,50);

      gr2->Draw("ap");
      gr1->Draw("l");
            
      TLine* line0 = new TLine(Xmin,0,Xmax,0); line0->SetLineColor(4); line0->Draw();

      
    }
    //TPaveStats *st = (TPaveStats*)rtgraph->GetListOfFunctions()->FindObject("stats");
    //cout << "stat " << st << endl;
    //st->SetX1NDC(0.5);
    //st->SetX2NDC(0.5);
  }
  

  this->cd(2);
  if(trgraph){
    //gStyle->SetOptFit(0001);    
    if (folder == "" )    trgraph->SetTitle("t(r) fit (whole TRT)");
    else{
      char name[500];  sprintf(name,"t(r) fit (%s)",titlemap[string(folder)].data());
      trgraph->SetTitle(name);
    }
    
    
    //trgraph->SetTitle("t(r) fit (whole TRT)");
    trgraph->GetYaxis()->SetTitle("t-T0/ns");
    trgraph->GetXaxis()->SetTitle("|r|_{track}/mm");
    //trgraph->GetYaxis()->SetRangeUser(0,32);
    trgraph->SetMarkerStyle(20);
    trgraph->SetMarkerSize(0.5);
    trgraph->Draw("ap");
    
    //TPaveStats *st = (TPaveStats*)rtgraph->GetListOfFunctions()->FindObject("stats");
    //cout << "stat " << st << endl;
    //st->SetX1NDC(0.5);
    //st->SetX2NDC(0.5);
    
  }
  
  //  if(rthist){
  //    gStyle->SetPalette(1);
  //    rthist->GetXaxis()->SetRangeUser(-10,50);
  //    rthist->Draw("colz");
  //  }

  gStyle->SetOptStat(1);

}


//=================================================================

class XYMapsEC: public TCanvas{
public:
  XYMapsEC(char*, char*, bool);
};

XYMapsEC::XYMapsEC(char* infile, char* variable,bool isAr = false){

  char selectionA[500];  sprintf(selectionA,"%s*(det==2)",variable);
  char selectionC[500];  sprintf(selectionC,"%s*(det==-2)",variable);
  if (variable=="nt0") variable = "n hits";
  TFile* file = new TFile(infile);
  //cout << infile << endl;
  file->cd();
  TNtuple* Chiptuple    =       NULL;
  TNtuple* Boardtuple   =       NULL;
  TNtuple* Moduletuple  =       NULL;

  if(!isAr){
    Chiptuple=(TNtuple*)        file->Get("Chiptuple");
    Boardtuple=(TNtuple*)       file->Get("Boardtuple");
    Moduletuple=(TNtuple*)      file->Get("Moduletuple");
  }
  else{
    Chiptuple=(TNtuple*)        file->Get("Chip_Artuple");
    Boardtuple=(TNtuple*)       file->Get("Board_Artuple");
    Moduletuple=(TNtuple*)      file->Get("Module_Artuple");
  }

  this->SetName("c1");
  this->cd();
  this->Divide(2,2,0.01,0.01);
  ((TPad*) this->GetPrimitive("c1_1"))->SetFrameFillColor(1);
  ((TPad*) this->GetPrimitive("c1_2"))->SetFrameFillColor(1);
  ((TPad*) this->GetPrimitive("c1_3"))->SetFrameFillColor(1);
  ((TPad*) this->GetPrimitive("c1_4"))->SetFrameFillColor(1);

  gStyle->SetPalette(1);

  this->cd(1);
  //Moduletuple->Draw("y:x>>reshist10(40,-1200,1200,40,-1200,1200)",selectionA,"colz");
  Moduletuple->Draw("mod:lay>>reshist10(14,0,14,32,0,32)",selectionA,"colz");
  TH2F *reshist10 = (TH2F*)gPad->GetPrimitive("reshist10");
  this->Update();
  if(reshist10->GetEntries()>1)   ResizePalette(reshist10);
  SetZAxis(reshist10);
  char title1[500];  sprintf(title1,"Phi Sector VS Wheel %s (EC side A)",variable);
  if(isAr)	sprintf(title1,"Argon Phi Sector VS Wheel %s (EC side A)",variable);
  reshist10->SetTitle(title1);
  reshist10->SetStats(0);
  reshist10->GetXaxis()->SetLabelOffset(1000);
  reshist10->GetYaxis()->SetLabelOffset(1000);
  reshist10->GetXaxis()->SetTitle("Wheel (Z)");
  reshist10->GetYaxis()->SetTitle("Phi sector");
  if (variable == "ftype"){
    reshist10->GetZaxis()->SetRangeUser(1,6);
    reshist10->GetZaxis()->SetNdivisions(6);
  }
  else if (variable == "res"){
    reshist10->GetZaxis()->SetRangeUser(0.125, 0.180);
  }
  else if (variable == "abs(resMean)"){
    reshist10->GetZaxis()->SetRangeUser(0.0, 0.010);
  }
  else if (variable == "t0"){
    reshist10->GetZaxis()->SetRangeUser(9.0, 11.0); 
  }
  else if (variable == "abs(t0-oldt0)"){
    reshist10->GetZaxis()->SetRangeUser(0.0, 0.6);
  }
  else if (variable == "tres"){
    reshist10->GetZaxis()->SetRangeUser(2.9, 3.9);
  }

  this->cd(2);
  
  //cout << selectionA << endl;
  //cout << selectionC << endl;

  Chiptuple->Draw("mod:chp>>histC(248,84,332,32,0,32)",selectionA,"colz");
  TH2F *histC = (TH2F*)gPad->GetPrimitive("histC");
  char title3[500];  sprintf(title3,"Chip %s (EC side A)",variable);
  if(isAr)	sprintf(title3,"Argon Chip %s (EC side A)",variable);
  histC->SetTitle(title3);
  this->Update();
  if(histC->GetEntries()>1)   ResizePalette(histC);
  SetZAxis(histC);
  histC->SetStats(0);
  histC->GetXaxis()->SetLabelOffset(1000);
  histC->GetYaxis()->SetLabelOffset(1000);
  histC->GetXaxis()->SetTitle("Chip number");
  histC->GetYaxis()->SetTitle("Phi sector");
    if (variable == "ftype"){
    histC->GetZaxis()->SetRangeUser(1,6);
    histC->GetZaxis()->SetNdivisions(6);
  }
  else if (variable == "res"){
    histC->GetZaxis()->SetRangeUser(0.120, 0.200);
  }
  else if (variable == "abs(resMean)"){
    histC->GetZaxis()->SetRangeUser(0.0, 0.05);
  }
  else if (variable == "t0"){
    histC->GetZaxis()->SetRangeUser(5, 18.0); 
  }
  else if (variable == "abs(t0-oldt0)"){
    histC->GetZaxis()->SetRangeUser(0.0, 1.6);
  }
  else if (variable == "(tres)"){
    histC->GetZaxis()->SetRangeUser(2.5, 4.5);
  }
  
  float lowe      = 0;
  float upe       = 32;

  TLine* lin11 = new TLine(84,lowe,84,upe); lin11->SetLineStyle(2); lin11->Draw();
  TLine* lin12 = new TLine(107,lowe,107,upe); lin12->SetLineStyle(2); lin12->Draw();
  TLine* lin13 = new TLine(131,lowe,131,upe); lin13->SetLineStyle(2); lin13->Draw();
  TLine* lin14 = new TLine(155,lowe,155,upe); lin14->SetLineStyle(2); lin14->Draw();
  TLine* lin15 = new TLine(179,lowe,179,upe); lin15->SetLineStyle(2); lin15->Draw();
  TLine* lin16 = new TLine(203,lowe,203,upe); lin16->SetLineStyle(2); lin16->Draw();
  TLine* lin17 = new TLine(227,lowe,227,upe); lin17->SetLineStyle(2); lin17->Draw();
  TLine* lin18 = new TLine(239,lowe,239,upe); lin18->SetLineStyle(2); lin18->Draw();
  this->Update();

  this->cd(3);
  Moduletuple->Draw("mod:lay>>reshist11(14,0,14,32,0,32)",selectionC,"colz");
  TH2F *reshist11 = (TH2F*)gPad->GetPrimitive("reshist11");
  this->Update();
  if(reshist11->GetEntries()>1)   ResizePalette(reshist11);
  SetZAxis(reshist11);
  char title4[500];  sprintf(title4,"Phi Sector VS Wheel %s (EC side C)",variable);
  if(isAr) 	sprintf(title4,"Argon Phi Sector VS Wheel %s (EC side C)",variable);
  reshist11->SetTitle(title4);
  reshist11->SetMarkerStyle(21);
  reshist11->SetStats(0);
  reshist11->GetXaxis()->SetLabelOffset(1000);
  reshist11->GetYaxis()->SetLabelOffset(1000);
  reshist11->GetXaxis()->SetTitle("Layer (Z)");
  reshist11->GetYaxis()->SetTitle("Phi sector");
  if (variable == "ftype"){
    reshist11->GetZaxis()->SetRangeUser(1,6);
    reshist11->GetZaxis()->SetNdivisions(6);

//   TH2F *reshist121 = (TH2F*)gPad->GetPrimitive("reshist11");
//   TH2F *reshist131 = (TH2F*)gPad->GetPrimitive("reshist11");
//   TH2F *reshist141 = (TH2F*)gPad->GetPrimitive("reshist11");
//   TH2F *reshist151 = (TH2F*)gPad->GetPrimitive("reshist11");
//   TH2F *reshist161 = (TH2F*)gPad->GetPrimitive("reshist11");

    TLegend* leg = new TLegend(0.99,0.35,0.55,0.01,"T0 fit type");    

	TH2F *h1 = new TH2F("h1","h1 title",40,0,4,30,-3,3); 
  	h1->SetMarkerColor(54);
  	h1->SetMarkerStyle(21);

        TH2F *h2 = new TH2F("h2","h2 title",40,0,4,30,-3,3);
        h2->SetMarkerColor(4);
  	h2->SetMarkerStyle(21);

        TH2F *h3 = new TH2F("h3","h3 title",40,0,4,30,-3,3);
        h3->SetMarkerColor(7);
  	h3->SetMarkerStyle(21);

        TH2F *h4 = new TH2F("h4","h4 title",40,0,4,30,-3,3);
        h4->SetMarkerColor(81);
  	h4->SetMarkerStyle(21);

        TH2F *h5 = new TH2F("h5","h5 title",40,0,4,30,-3,3);
        h5->SetMarkerColor(kYellow);
  	h5->SetMarkerStyle(21);

        TH2F *h6 = new TH2F("h6","h6 title",40,0,4,30,-3,3);
        h6->SetMarkerColor(kRed);
  	h6->SetMarkerStyle(21);


    leg->AddEntry(h1,"1: |mean|>5","P");
    leg->AddEntry(h2,"2: normal fit","P");
    leg->AddEntry(h3,"3: used chip ref","P");
    leg->AddEntry(h4,"4: Low stat","P");
    leg->AddEntry(h5,"5: No T0 Cal","P");
    leg->AddEntry(h6,"6: Wrong fit. Mean used","P");
    leg->SetTextSize(0.04);
    leg->Draw();
  }
  else if (variable == "res"){
    reshist11->GetZaxis()->SetRangeUser(0.125, 0.180);
  }
  else if (variable == "abs(resMean)"){
    reshist11->GetZaxis()->SetRangeUser(0.0, 0.010);
  }
  else if (variable == "t0"){
    reshist11->GetZaxis()->SetRangeUser(9.0, 11.0); 
  }
  else if (variable == "abs(t0-oldt0)"){
    reshist11->GetZaxis()->SetRangeUser(0.0, 0.6);
  }
  else if (variable == "tres"){
    reshist11->GetZaxis()->SetRangeUser(2.9, 3.9);
  }


  this->cd(4);
  Chiptuple->Draw("mod:chp>>histC1(248,84,332,32,0,32)",selectionC,"colz");
  TH2F *histC1 = (TH2F*)gPad->GetPrimitive("histC1");
  char title5[500];  sprintf(title5,"Chip %s (EC side C)",variable);
  if(isAr) 	sprintf(title5,"Argon Chip %s (EC side C)",variable);
  histC1->SetTitle(title5);
  this->Update();
  if(histC1->GetEntries()>1)   ResizePalette(histC1);
  SetZAxis(histC1);
  histC1->SetStats(0);
  histC1->GetXaxis()->SetLabelOffset(1000);
  histC1->GetYaxis()->SetLabelOffset(1000);
  histC1->GetXaxis()->SetTitle("Chip number");
  histC1->GetYaxis()->SetTitle("Phi sector");
  if (variable == "ftype"){
    histC1->GetZaxis()->SetRangeUser(1,6);
    histC1->GetZaxis()->SetNdivisions(6);
  }
  else if (variable == "res"){
    histC1->GetZaxis()->SetRangeUser(0.120, 0.200);
  }
  else if (variable == "abs(resMean)"){
    histC1->GetZaxis()->SetRangeUser(0.0, 0.05);
  }
  else if (variable == "t0"){
    histC1->GetZaxis()->SetRangeUser(5, 18.0); 
  }
  else if (variable == "abs(t0-oldt0)"){
    histC1->GetZaxis()->SetRangeUser(0.0, 1.6);
  }
  else if (variable == "(tres)"){
    histC1->GetZaxis()->SetRangeUser(2.5, 4.5);
  }

  TLine* lin1 = new TLine(84,lowe,84,upe);   lin1->SetLineStyle(2); lin1->Draw();
  TLine* lin2 = new TLine(108,lowe,108,upe); lin2->SetLineStyle(2); lin2->Draw();
  TLine* lin3 = new TLine(132,lowe,132,upe); lin3->SetLineStyle(2); lin3->Draw();
  TLine* lin4 = new TLine(156,lowe,156,upe); lin4->SetLineStyle(2); lin4->Draw();
  TLine* lin5 = new TLine(180,lowe,180,upe); lin5->SetLineStyle(2); lin5->Draw();
  TLine* lin6 = new TLine(204,lowe,204,upe); lin6->SetLineStyle(2); lin6->Draw();
  TLine* lin7 = new TLine(228,lowe,228,upe); lin7->SetLineStyle(2); lin7->Draw();
  TLine* lin8 = new TLine(240,lowe,239,upe); lin8->SetLineStyle(2); lin8->Draw();



  this->Update();


}


//================================================================

class RtColor: public TCanvas{
public:
  RtColor(char*, char*, bool) throw(string);
  TH2F* hist;
  TGraphErrors* rtgraph;
  TF1* oldrtfunc;
  TF1* newRT;
};

RtColor::RtColor(char* infile, char* folder, bool isAr = false) throw(string){ 
 
  map<string,string> titlemap;
  titlemap["WholeBarrel_1"]="whole barrel";
  titlemap["Detector_-1"]="barrel C";
  titlemap["Detector_1"]="barrel A";
  titlemap["Detector_-2"]="end-cap C";
  titlemap["Detector_2"]="end-cap A";

  titlemap["WholeBarrel_Ar_1"]="whole barrel";
  titlemap["Detector_Ar_-1"]="barrel C";
  titlemap["Detector_Ar_1"]="barrel A";
  titlemap["Detector_Ar_-2"]="end-cap C";
  titlemap["Detector_Ar-2"]="end-cap A";

  TFile* file = new TFile(infile);
  //cout << infile << endl;

  TPad*  c1_3 = new TPad("c1_3", "newpad",0.01,0.33,0.99,0.99);
  c1_3->Draw();
  c1_3->cd();
  c1_3->SetTopMargin(0.1);
  c1_3->SetBottomMargin(0.01);
  c1_3->SetRightMargin(0.1);
  c1_3->SetFillStyle(0);
  c1_3->SetGrid();

  file->cd();
  
  //gStyle->SetOptFit(0001);    
  TDirectory* trt = NULL;
  if (!isAr)    trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
  else          trt = (TDirectory*) file->FindKey("TRT_Ar_all")->ReadObj();
  
  if (folder!=""){
    TDirectory* det = (TDirectory*) trt->FindKey(folder)->ReadObj();
    //cout << "  PLOT FOR "<< folder <<endl;
    if (det->FindKey("rt-relation")){
      hist= (TH2F*) det->FindKey("rt-relation")->ReadObj();
//      this->cd();
      hist->GetXaxis()->SetRangeUser(0,40);
      hist->GetYaxis()->SetRangeUser(0,2.4);
      char name[500];  sprintf(name,"r(t) for %s",titlemap[(string)folder].data());
      if(isAr) 	 sprintf(name,"Argon r(t) for %s",titlemap[(string)folder].data());
      hist->SetTitle(name);
      hist->GetXaxis()->SetRangeUser(0,50);
      hist->Draw("colz");
      if (det->FindKey("rtgraph")){
	rtgraph= (TGraphErrors*) det  ->FindKey("rtgraph")->ReadObj();
	rtgraph->SetMarkerStyle(20);
	rtgraph->SetMarkerColor(1);
	rtgraph->SetLineColor(1);
	rtgraph->GetYaxis()->SetRangeUser(0,2.4);
	rtgraph->Draw("p");
    	if(rtgraph->GetFunction("rtfunc")) newRT = rtgraph->GetFunction("rtfunc");
    	else newRT = rtgraph->GetFunction("rtfunc2");
      }
      if (det->FindKey("oldrtfunc")){
	oldrtfunc= (TF1*) det  ->FindKey("oldrtfunc")->ReadObj();
	oldrtfunc->SetRange(0,50);
	oldrtfunc->Draw("same");
      }
    }
    else {  
      throw string("  NO RT OBJECTS FOUND");
    }  
  }
  else if (trt->FindKey("rt-relation")){
    //cout << "  PLOT FOR TRT"<< endl;
    hist= (TH2F*) trt->FindKey("rt-relation")->ReadObj(); 
    //this->cd();
    hist->GetXaxis()->SetRangeUser(0,40);
    hist->SetTitle("r(t) for whole TRT");
    if(isAr) hist->SetTitle("Argon r(t) for whole TRT");
    hist->GetYaxis()->SetRangeUser(0,2.4);
    hist->GetXaxis()->SetRangeUser(0,50);
    hist->Draw("colz");
    if (trt->FindKey("rtgraph")){ 
      rtgraph= (TGraphErrors*) trt  ->FindKey("rtgraph")->ReadObj();
      rtgraph->SetMarkerStyle(20);
      rtgraph->SetLineColor(1);
      rtgraph->SetMarkerColor(1);
      rtgraph->GetYaxis()->SetRangeUser(0,2.4);
      if(rtgraph->GetFunction("rtfunc")) newRT = rtgraph->GetFunction("rtfunc");
      else newRT = rtgraph->GetFunction("rtfunc2");
      rtgraph->Draw("p");
    }
    if (trt->FindKey("oldrtfunc")){
      oldrtfunc= (TF1*) trt  ->FindKey("oldrtfunc")->ReadObj();
      oldrtfunc->SetRange(0,50);
      oldrtfunc->Draw("same");
    } 
  }
  else 
    hist=NULL;

	// Draw the ration plots just below:

    //Plot the difference between old and new rt:
    if(oldrtfunc){
      this->cd();
      this->cd(1);
      TPad*  c1_4 = new TPad("c1_4", "newpad",0.01,0,0.99,0.32);
      c1_4->Draw();
      c1_4->cd();
      c1_4->SetTopMargin(0.01);
      c1_4->SetBottomMargin(0.3);
      c1_4->SetRightMargin(0.1);
      c1_4->SetFillStyle(0);
      c1_4->SetGrid();
      
      Double_t Xmax =  rtgraph->GetXaxis()->GetXmax();
      Double_t Xmin =  rtgraph->GetXaxis()->GetXmin();
      
      Int_t n = 300;
      Double_t x[n], y[n], dx;
      dx = (Xmax-Xmin)/n;
      
      Double_t Ymax = -10;
      Double_t Ymin = 10;
     

	newRT->SetRange(-10.,60.); 
	oldrtfunc->SetRange(-10.,60.); 
      
      for (Int_t i=0; i<n; i++) {
	x[i] = Xmin+i*dx;
	y[i] = ( oldrtfunc->Eval(x[i]) - newRT->Eval(x[i]));
	if (y[i] > Ymax ) Ymax = y[i];
	if (y[i] < Ymin ) Ymin = y[i];
      }
      
      TGraph *gr1 = new TGraph (n, x, y);
      
      //gStyle->SetOptTitle(0);
      gr1->SetTitle("");
      gr1->GetXaxis()->SetTitle("t-T0/ns");
      gr1->GetXaxis()->SetRangeUser(Xmin,Xmax);
      gr1->GetYaxis()->SetTitle("Old r(t) - New r(t) /mm");
      gr1->GetYaxis()->SetRangeUser(-0.1,0.1);
      //gr1->GetYaxis()->SetRangeUser(1.1*Ymin,1.1*Ymax);
      gr1->GetYaxis()->SetNdivisions(5);
      gr1->GetXaxis()->SetTitleSize(0.06);
      gr1->GetXaxis()->SetLabelSize(0.06);
      gr1->GetYaxis()->SetLabelSize(0.08);
      gr1->GetYaxis()->SetTitleOffset(0.8);
      gr1->GetYaxis()->SetTitleSize(0.06);
      gr1->SetLineStyle(2);
      
    
      //Also add the difference between the fit and the points of the fit
      
      Int_t n1 = rtgraph->GetN();
      Double_t x1[n1], y1[n1],ex1[n1], ey1[n1], X, Y;
      
      for (Int_t i=0; i<n1; i++) {
	X = 0;
	Y = 0;
	rtgraph->GetPoint(i, X, Y) ;
	x1[i] = X;
	//y1[i] = newRT->Eval(X)  - Y;
	y1[i] = Y - newRT->Eval(X);
	ex1[i] = 0;
	ey1[i] = rtgraph->GetErrorY(i);
	if (y1[i] > Ymax ) Ymax = y1[i];
	if (y1[i] < Ymin ) Ymin = y1[i];
	
      }
      //gr1->Draw("APL");
      
      
      TGraphErrors *gr2 = new TGraphErrors (n1, x1, y1, ex1,ey1);
      gr2->SetMarkerColor(kBlack);
      gr2->SetMarkerStyle(20);
      gr2->SetTitle("");
      gr2->GetYaxis()->SetTitle("Old r(t) - New r(t) /mm");

      //gr2->GetYaxis()->SetRangeUser(1.1*Ymin,1.1*Ymax);
      gr2->GetYaxis()->SetRangeUser(-0.1,0.1);
      gr2->GetXaxis()->SetRangeUser(0,50);

      gr2->Draw("ap");
      gr1->Draw("l");
            
      TLine* line0 = new TLine(0,0,50,0); line0->SetLineColor(4); line0->Draw();

      
    }



}


//================================================================

class ResidualPlots: public TCanvas{
public:
  ResidualPlots(TFile*, bool) throw(string);
};

ResidualPlots::ResidualPlots(TFile* file, bool isAr = false)  throw(string){

  this->SetName("resplots");

  file->cd();

  TDirectory* trt  =    NULL;
  TDirectory* det1 =    NULL;
  TDirectory* det2 =    NULL;
  TDirectory* det3 =    NULL;
  TDirectory* det4 =    NULL;

  if(!isAr){
   trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
   det1 = (TDirectory*) trt->FindKey("Detector_-1")->ReadObj();
   det2 = (TDirectory*) trt->FindKey("Detector_1")->ReadObj();
   det3 = (TDirectory*) trt->FindKey("Detector_-2")->ReadObj();
   det4 = (TDirectory*) trt->FindKey("Detector_2")->ReadObj();
  }
  else {
   if (file->FindKey("TRT_Ar_all"))     trt  = (TDirectory*) file->FindKey("TRT_Ar_all")->ReadObj();
   if (trt ->FindKey("Detector_Ar_-1")) det1 = (TDirectory*) trt->FindKey("Detector_Ar_-1")->ReadObj();
   if (trt ->FindKey("Detector_Ar_1"))  det2 = (TDirectory*) trt->FindKey("Detector_Ar_1")->ReadObj();
   if (trt ->FindKey("Detector_Ar_-2")) det3 = (TDirectory*) trt->FindKey("Detector_Ar_-2")->ReadObj();
   if (trt ->FindKey("Detector_Ar_2"))  det4 = (TDirectory*) trt->FindKey("Detector_Ar_2")->ReadObj();
  }

  TH2F* reshist1=NULL;
  TH2F* reshist2=NULL;
  TH2F* reshist3=NULL;
  TH2F* reshist4=NULL;
  TH2F* reshist5=NULL;
  TH2F* treshist1=NULL;
  TH2F* treshist2=NULL;
  TH2F* treshist3=NULL;
  TH2F* treshist4=NULL;
  TH2F* treshist5=NULL;
/*
  if (trt->FindKey("residual")) reshist1 = (TH2F*) trt->FindKey("residual")->ReadObj();
  else throw(string("residual histograms not found!"));
  if (det1->FindKey("residual")) reshist2 = (TH2F*) det1->FindKey("residual")->ReadObj();
  else throw(string("residual histograms not found!"));
  if (det2->FindKey("residual")) reshist3 = (TH2F*) det2->FindKey("residual")->ReadObj();
  else throw(string("residual histograms not found!"));
  if (det3->FindKey("residual")) reshist4 = (TH2F*) det3->FindKey("residual")->ReadObj();
  else throw(string("residual histograms not found!"));
  if (det4->FindKey("residual")) reshist5 = (TH2F*) det4->FindKey("residual")->ReadObj();
  else throw(string("residual histograms not found!"));

  if (trt->FindKey("timeresidual")) treshist1 = (TH2F*) trt->FindKey("timeresidual")->ReadObj();
  else throw(string("timeresidual histograms not found!"));
  if (det1->FindKey("timeresidual")) treshist2 = (TH2F*) det1->FindKey("timeresidual")->ReadObj();
  else throw(string("timeresidual histograms not found!"));
  if (det2->FindKey("timeresidual")) treshist3 = (TH2F*) det2->FindKey("timeresidual")->ReadObj();
  else throw(string("timeresidual histograms not found!"));
  if (det3->FindKey("timeresidual")) treshist4 = (TH2F*) det3->FindKey("timeresidual")->ReadObj();
  else throw(string("timeresidual histograms not found!"));
  if (det4->FindKey("timeresidual")) treshist5 = (TH2F*) det4->FindKey("timeresidual")->ReadObj();
  else throw(string("timeresidual histograms not found!"));
*/

 if (trt->FindKey("residual")) reshist1 = (TH2F*) trt->FindKey("residual")->ReadObj();
 if(det1)  {if (det1->FindKey("residual")) reshist2 = (TH2F*) det1->FindKey("residual")->ReadObj();}
 if(det2)  {if (det2->FindKey("residual")) reshist3 = (TH2F*) det2->FindKey("residual")->ReadObj();}
 if(det3)  {if (det3->FindKey("residual")) reshist4 = (TH2F*) det3->FindKey("residual")->ReadObj();}
 if(det4)  {if (det4->FindKey("residual")) reshist5 = (TH2F*) det4->FindKey("residual")->ReadObj();}

  if (trt->FindKey("timeresidual")) treshist1 = (TH2F*) trt->FindKey("timeresidual")->ReadObj();
  if(det1)   {if (det1->FindKey("timeresidual")) treshist2 = (TH2F*) det1->FindKey("timeresidual")->ReadObj();}
  if(det2)   {if (det2->FindKey("timeresidual")) treshist3 = (TH2F*) det2->FindKey("timeresidual")->ReadObj();}
  if(det3)   {if (det3->FindKey("timeresidual")) treshist4 = (TH2F*) det3->FindKey("timeresidual")->ReadObj();}
  if(det4)   {if (det4->FindKey("timeresidual")) treshist5 = (TH2F*) det4->FindKey("timeresidual")->ReadObj();}


  if(reshist1)  reshist1->SetTitle(Form("residual whole TRT"));
  if(reshist2)  reshist2->SetTitle(Form("residual (barrel side C)"));
  if(reshist3)  reshist3->SetTitle(Form("residual (barrel side A)"));
  if(reshist4)  reshist4->SetTitle(Form("residual (endcap side C)"));
  if(reshist5)  reshist5->SetTitle(Form("residual (endcap side A)"));
  if(reshist2)  reshist2->SetFillColor(46);
  if(reshist3)  reshist3->SetFillColor(46);
  if(reshist4)  reshist4->SetFillColor(46);
  if(reshist5)  reshist5->SetFillColor(46);

  if(treshist1)  treshist1->SetTitle(Form("time-residual whole TRT"));
  if(treshist2)  treshist2->SetTitle(Form("time-residual (barrel side C)"));
  if(treshist3)  treshist3->SetTitle(Form("time-residual (barrel side A)"));
  if(treshist4)  treshist4->SetTitle(Form("time-residual (endcap side C)"));
  if(treshist5)  treshist5->SetTitle(Form("time-residual (endcap side A)"));
  if(treshist2)  treshist2->SetFillColor(38);
  if(treshist3)  treshist3->SetFillColor(38);
  if(treshist4)  treshist4->SetFillColor(38);
  if(treshist5)  treshist5->SetFillColor(38);

  if(isAr){
   if(reshist1)  reshist1->SetTitle(Form("residual Argon whole TRT"));
   if(reshist2)  reshist2->SetTitle(Form("residual Argon (barrel side C)"));
   if(reshist3)  reshist3->SetTitle(Form("residual Argon (barrel side A)"));
   if(reshist4)  reshist4->SetTitle(Form("residual Argon (endcap side C)"));
   if(reshist5)  reshist5->SetTitle(Form("residual Argon (endcap side A)"));
   if(treshist1) treshist1->SetTitle(Form("time-residual Argon whole TRT"));
   if(treshist2) treshist2->SetTitle(Form("time-residual Argon (barrel side C)"));
   if(treshist3) treshist3->SetTitle(Form("time-residual Argon (barrel side A)"));
   if(treshist4) treshist4->SetTitle(Form("time-residual Argon (endcap side C)"));
   if(treshist5) treshist5->SetTitle(Form("time-residual Argon (endcap side A)"));
  }

  TF1* resfit1 =        NULL;
  TF1* resfit2 =        NULL;
  TF1* resfit3 =        NULL;
  TF1* resfit4 =        NULL;
  TF1* resfit5 =        NULL;
  TF1* tresfit1 =       NULL;
  TF1* tresfit2 =       NULL;
  TF1* tresfit3 =       NULL;
  TF1* tresfit4 =       NULL;
  TF1* tresfit5 =       NULL;

  if(reshist1) resfit1 = (TF1*)reshist1->GetListOfFunctions()->First();
  if(reshist2) resfit2 = (TF1*)reshist2->GetListOfFunctions()->First();
  if(reshist3) resfit3 = (TF1*)reshist3->GetListOfFunctions()->First();
  if(reshist4) resfit4 = (TF1*)reshist4->GetListOfFunctions()->First();
  if(reshist5) resfit5 = (TF1*)reshist5->GetListOfFunctions()->First();
  if(treshist1) tresfit1 = (TF1*)treshist1->GetListOfFunctions()->First();
  if(treshist2) tresfit2 = (TF1*)treshist2->GetListOfFunctions()->First();
  if(treshist3) tresfit3 = (TF1*)treshist3->GetListOfFunctions()->First();
  if(treshist4) tresfit4 = (TF1*)treshist4->GetListOfFunctions()->First();
  if(treshist5) tresfit5 = (TF1*)treshist5->GetListOfFunctions()->First();


  TText* mintxt = new TText();
  mintxt->SetNDC(kTRUE);

  this->Divide(4,2);
//  this->SetLogy();
  // ((TPad*) this->GetPrimitive("resplots_1"))->SetLogy();  

  //gStyle->SetOptFit(10);    
  //gStyle->SetOptFit(0000);    

  //TF1* test = (TF1*)treshist2->FindObject("gaus");
  //test->SetParNames("C","M","S");

  this->cd(1);
if(reshist2){
  reshist2->Draw();
  mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",resfit2->GetParameter(1)));
  mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",resfit2->GetParameter(2)));
}
  this->cd(2);
  if( !isAr &&  reshist3){
    reshist3->Draw();
    mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",resfit3->GetParameter(1)));
    mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",resfit3->GetParameter(2)));
  }
  this->cd(3);

if(reshist4){
  reshist4->Draw();
  mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",resfit4->GetParameter(1)));
  mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",resfit4->GetParameter(2)));
}
  this->cd(4);
if(reshist5){
  reshist5->Draw();
  mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",resfit5->GetParameter(1)));
  mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",resfit5->GetParameter(2)));
}
  this->cd(5);
cout << "5" << endl;
if(treshist2){
cout << "5" << endl;
  treshist2->Draw();
  mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",tresfit2->GetParameter(1)));
  mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",tresfit2->GetParameter(2)));
}
  this->cd(6);
  if(!isAr && treshist3){
    treshist3->Draw();
    mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",tresfit3->GetParameter(1)));
    mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",tresfit3->GetParameter(2)));
  }
  this->cd(7);
if(treshist4){
  treshist4->Draw();
  mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",tresfit4->GetParameter(1)));
  mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",tresfit4->GetParameter(2)));
}
  this->cd(8);
if(treshist5){
  treshist5->Draw();
  mintxt->DrawText(0.15,0.85,Form("Mean: %.3f",tresfit5->GetParameter(1)));
  mintxt->DrawText(0.15,0.80,Form("Sigma: %.3f",tresfit5->GetParameter(2)));
}
  gStyle->SetOptStat(0);    
}


//================================================================

class TRTPlots: public TCanvas{
public:
  TRTPlots(TFile*,bool) throw(string);
};

TRTPlots::TRTPlots(TFile* file, bool isAr = false) throw(string){

  this->SetName("resplots");

  file->cd();

  TDirectory* trt =  NULL;
  if (!isAr) trt  = (TDirectory*)file->FindKey("TRT_all")->ReadObj();
  else       trt  = (TDirectory*)file->FindKey("TRT_Ar_all")->ReadObj();
    
  TH2F* reshist1=NULL;
  if (trt->FindKey("residual")) reshist1 = (TH2F*) trt->FindKey("residual")->ReadObj();
  else throw(string("residual histograms not found!"));

  TH2F* treshist1=NULL;
  if (trt->FindKey("timeresidual")) treshist1 = (TH2F*) trt->FindKey("timeresidual")->ReadObj();
  else throw(string("timeresidual histograms not found!"));

  reshist1->SetTitle(Form("residual whole TRT"));
  if(isAr)	reshist1->SetTitle(Form("Argon residual whole TRT"));
  reshist1->SetFillColor(46);
  treshist1->SetTitle(Form("time-residual whole TRT"));
  if(isAr)	treshist1->SetTitle(Form("Argon time-residual whole TRT"));
  treshist1->SetFillColor(38);

  this->Divide(1,2,0.01,0.01);
  this->SetLogy();
  // ((TPad*) this->GetPrimitive("resplots_1"))->SetLogy();  

  //gStyle->SetOptFit(1011);    
  //gStyle->SetOptStat(11);    
  //gStyle->SetOptFit(0);    
  //gStyle->SetOptStat(0);    

  this->cd(1);
  reshist1->Draw();
  this->cd(2);
  treshist1->Draw();

}


//================================================================

class TBinnedRes: public TCanvas{
public: 
  TBinnedRes(TFile*,vector<TH1D*>);
private:
  TH1F* hist2;
};

TBinnedRes::TBinnedRes(TFile* file, vector<TH1D*> reshists){

  this->Divide(((int)reshists.size())/5,5);
  for (int ihist=0; ihist<(int)reshists.size(); ihist++){
    this->cd(ihist+1);
    reshists[ihist]->SetTitle("");
    reshists[ihist]->Draw();
    reshists[ihist]->GetYaxis()->SetRangeUser(0,1.1*reshists[ihist]->GetMaximum());
    TLine* zlin = new TLine(0,0,0,reshists[ihist]->GetMaximum()); zlin->SetLineStyle(2); zlin->SetLineColor(4); zlin->Draw();
  }

   
     
  //gStyle->SetOptFit(0);    
  //gStyle->SetOptStat(0);    

  this->SetGrid();
}


//================================================================

class RtBinning: public TCanvas{
public: 
  RtBinning(TDirectory*,char*);
private:
  TH1F* hist2;
};

RtBinning::RtBinning(TDirectory* file, char* detector){

  TDirectory* binhist = (TDirectory*)(((TDirectory*)file->Get(detector))->Get("binhist"));
  int nhists=binhist->GetListOfKeys()->GetEntries();

  this->Divide(nhists/5,5);
  for (int ihist=0; ihist<nhists; ihist++){
    this->cd(ihist+1);
    hist2=(TH1F*)(binhist->Get(binhist->GetListOfKeys()->At(ihist)->GetName()));
    hist2->SetTitle("");
    hist2->Draw();
  }

  //gStyle->SetOptFit(0);    
  //gStyle->SetOptStat(0);    

  this->SetGrid();
}


//================================================================

class RresTbin: public TCanvas{
public: 
  RresTbin(TFile*, char*, int, bool);
  vector<TH1D*> reshists;
  map<double,double> resmap;
private:
  TGraphErrors* thegraph;
  double* rdata;
  double* erdata;
  double* tdata;
  double* etdata;
};

RresTbin::RresTbin(TFile* file, char* detname, int det, bool isAr = false){ 
 

  TF1 ff("fitfunc","gaus");
  TF1* ff2 = new TF1("fitfunc2","pol3");

  TDirectory* trt = NULL;

  if (!isAr)    trt = (TDirectory*) file->FindKey("TRT_all")->ReadObj();
  else          trt = (TDirectory*) file->FindKey("TRT_Ar_all")->ReadObj();
  
TDirectory* detdir = (TDirectory*) trt->FindKey(detname)->ReadObj();
  TDirectory* binhist = (TDirectory*) detdir->FindKey("reshists")->ReadObj();
  
  if(binhist){


    TVectorD* tbins = (TVectorD*)file->FindKey("tbins")->ReadObj();
    //TVectorD* rbins = (TVectorD*)binhist->Get("rbins");

    //cout << tbins << endl;
    //tbins->Print();
    
    if(tbins){

      rdata=new double[tbins->GetNoElements()];
      erdata=new double[tbins->GetNoElements()];
      tdata=new double[tbins->GetNoElements()];
      etdata=new double[tbins->GetNoElements()];
      thegraph=new TGraphErrors(tbins->GetNoElements());

      int ipoint=0;
      for (int ihist=0; ihist<tbins->GetNoElements(); ihist++){
	
	TH1D* reshist;

	if (det==4) reshist = (TH1D*)binhist->Get(Form("res_tbin%i_bar",ihist));
	else if (det==5) reshist = (TH1D*)binhist->Get(Form("res_tbin%i_trt",ihist));
	else  reshist = (TH1D*)binhist->Get(Form("res_tbin%i_%i",ihist,det));
	//double maxbin = reshist->GetBinCenter(reshist->GetMaximumBin());
	double lolim = -0.1;//reshist->GetBinCenter(reshist->GetMaximumBin()-10);
	double hilim = 0.1;//reshist->GetBinCenter(reshist->GetMaximumBin()+10);
	//reshist->Fit("gaus","Q","",lolim,hilim);
	
	if (reshist->Fit(&ff,"Q","",lolim,hilim)+1) {
	  rdata[ipoint]=ff.GetParameter(1);
	  erdata[ipoint]=ff.GetParError(1);
	  tdata[ipoint]=(*tbins)[ihist];

	  etdata[ipoint]=0;
	  //printf("%f %f\n",ff.GetParameter(1),ff.GetParError(1));
	  ipoint++;


	}
	
	reshists.push_back(reshist);

      }
      
      for (int ip=0; ip<ipoint; ip++){
	//cout << rdata[ip]  << " ";
	resmap[tdata[ip]]=rdata[ip];
      }
      //cout << endl;

      thegraph = new TGraphErrors(ipoint,tdata,rdata,etdata,erdata);

      //thegraph->Fit(ff2,"Q","",0,45);

      //for (int ip=0; ip<=3; ip++){
      //cout << ff2->GetParameter(ip) << endl;
      //resmap[(double)ip]=ff2->GetParameter(ip);
      //}
      
      string detstr[8]={"endcap A","barrel A","","barrel C","endcap C","","whole barrel","whole TRT"};

      thegraph->SetMarkerStyle(20);
      thegraph->SetTitle(Form("t-binned absolute residual (%s)",detstr[det+2].data()));
      thegraph->Draw("ap");
      //thegraph->GetYaxis()->SetTitle("|r|-|r_{track}| /#mum");
      thegraph->GetYaxis()->SetTitle("|r|-|r_{track}| /mm");
      thegraph->GetYaxis()->SetRangeUser(-0.150,0.150);
      thegraph->GetXaxis()->SetRangeUser(0,50);
      thegraph->GetXaxis()->SetTitle("t-t0 /ns");
    
      printf("RPOLDIFF %i %e %e %e %e\n",det,ff2->GetParameter(0),ff2->GetParameter(1),ff2->GetParameter(2),ff2->GetParameter(3));

      this->SetGrid();
     
    }
  }
}


//================================================================

class FirstPage: public TCanvas{
public: 
  FirstPage(char*, TFile*) throw(string);
  float runnumber,t0offset;
  int iter;
private:
};

FirstPage::FirstPage(char* filename, TFile* file) throw(string){

  this->cd();

  TNtuple* Tracktuple = (TNtuple*)file->Get("tracktuple");
  TNtuple* TRTtuple      = (TNtuple*)file->Get("TRTtuple");
  TNtuple* Detectortuple = (TNtuple*)file->Get("Detectortuple");

  TDatime* dt= new TDatime();

  TPaveText* pt = new TPaveText(.1,.2,.9,.9);

  if (!Tracktuple) throw(string("tracktuple not found!"));

  pt->AddText(" ");
  //pt->AddText(Form("Dataset: %s",tag));

  if (string(filename).find("_histograms.root")!=string::npos) iter = atoi(string(filename).substr(string(filename).find("_histograms.root")-2,2).data());
  else iter=99;

  runnumber = 0;
  pt->AddText(Form("Iteration number: %i", iter)); 
  pt->AddText(Form("Time: %02i/%02i/%i %02i:%02i",dt->GetDay(),dt->GetMonth(),dt->GetYear(),dt->GetHour(),dt->GetMinute()));
  Tracktuple->SetBranchAddress("run",&runnumber);
  Tracktuple->GetEntry(0);
  pt->AddText(Form("Run number: %i",(int)runnumber));
  for(int i=1; i<Tracktuple->GetEntries();i++){
    float oldrun= runnumber;
    Tracktuple->GetEntry(i);
    if(oldrun!=runnumber )         pt->AddText(Form("Run number: %i",(int)runnumber));
  }
  pt->AddText(Form("Total no. tracks: %i",Tracktuple->GetEntries()));
	
  if (!TRTtuple) throw(string("TRTtuple not found!"));
  if (!TRTtuple) throw(string("Detectortuple not found!"));

  float tothits,bhits,bhits1,bhits2,bhits3,bhits4;
  float res, res1,res2,res3,res4;
  float t0, oldt, tres1,tres2,tres3,tres4;
  float tresw, tres1w, tres2w, tres3w, tres4w;

  TRTtuple->SetBranchAddress("nt0",&tothits);
  Detectortuple->SetBranchAddress("nt0",&bhits);
  Detectortuple->SetBranchAddress("res",&res);  
  Detectortuple->SetBranchAddress("tres",&tresw);
  Detectortuple->SetBranchAddress("t0",&t0);
  Detectortuple->SetBranchAddress("oldt0",&oldt);

  TRTtuple->GetEntry(0);
  Detectortuple->GetEntry(0);
  bhits1=bhits;
  res1=res;
  tres1w=tresw;
  tres1=t0-oldt;

  Detectortuple->GetEntry(1);
  bhits2=bhits;
  res2=res;
  tres2w=tresw;
  tres2=t0-oldt;

  Detectortuple->GetEntry(2);  
  res3=res;
  bhits3=bhits;
  tres3w=tresw;
  tres3=t0-oldt;

  Detectortuple->GetEntry(3);
  res4=res;
  bhits4=bhits;
  tres4w=tresw;
  tres4=t0-oldt;

// Get the precion hits vs tube hits:

  TDirectory* errors=NULL  ;
  bool iserrors = false;
  if (file->FindKey("Errors")) {
        errors = (TDirectory*) file->FindKey("Errors")->ReadObj();
        iserrors = true;
  }

	double ratiotrt = 0;
	double ratioba  = 0;
	double ratiobc  = 0;
	double ratioea  = 0;
	double ratioec  = 0;

  if (iserrors) {
         TH2F*      residual_trt    	=  (TH2F*)errors->FindKey("tresidual_trt")->ReadObj();
         TH2F*      residual_ba  	=  (TH2F*)errors->FindKey("tresidual_ba")->ReadObj();
         TH2F*      residual_bc  	=  (TH2F*)errors->FindKey("tresidual_bc")->ReadObj();
         TH2F*      residual_ea  	=  (TH2F*)errors->FindKey("tresidual_ea")->ReadObj();
         TH2F*      residual_ec 	=  (TH2F*)errors->FindKey("tresidual_ec")->ReadObj();

         TH2F*      residual_trtP        =  (TH2F*)errors->FindKey("tresidual_trtP")->ReadObj();
         TH2F*      residual_baP         =  (TH2F*)errors->FindKey("tresidual_baP")->ReadObj();
         TH2F*      residual_bcP         =  (TH2F*)errors->FindKey("tresidual_bcP")->ReadObj();
         TH2F*      residual_eaP         =  (TH2F*)errors->FindKey("tresidual_eaP")->ReadObj();
         TH2F*      residual_ecP         =  (TH2F*)errors->FindKey("tresidual_ecP")->ReadObj();

	if (residual_trt->GetEntries() > 0 ){   ratiotrt = residual_trtP->GetEntries() /residual_trt->GetEntries() ;  	}
	if (residual_ba->GetEntries() > 0 ){   ratioba = residual_baP->GetEntries() /residual_ba->GetEntries() ;  	}
	if (residual_bc->GetEntries() > 0 ){   ratiobc = residual_bcP->GetEntries() /residual_bc->GetEntries() ;  	}
	if (residual_ea->GetEntries() > 0 ){   ratioea = residual_eaP->GetEntries() /residual_ea->GetEntries() ;  	}
	if (residual_ec->GetEntries() > 0 ){   ratioec = residual_ecP->GetEntries() /residual_ec->GetEntries() ;  	}
//	cout << residual_eaP->GetEntries() /residual_ea->GetEntries()  << "  asdfasdfas" << endl;
}


  pt->AddText("Barrels:");
  pt->AddText(Form("  (side C) hits: %i  residual: %.4f  microns T0 diff: %.3f  ns  ",(int)bhits1,res1,tres1));
  pt->AddText(Form("  (side C) Time res Width: %.2f   ns Ratio prec hits: %.3f  ",tres1w,ratioba));
  pt->AddText(Form("  (side A) hits: %i  residual: %.4f  microns T0 diff: %.3f  ns ",(int)bhits3,res3,tres3));
  pt->AddText(Form("  (side A) Time res Width: %.2f   ns Ratio prec hits: %.3f ",tres3w,ratiobc));
  pt->AddText(" Endcaps:");
  pt->AddText(Form(" (side C) hits: %i  residual: %.4f  microns T0 diff: %.3f ns",(int)bhits2,res2,tres2));
  pt->AddText(Form(" (side C) Time res Width: %.2f   ns Ratio prec hits: %.3f ",tres2w,ratioea));
  pt->AddText(Form(" (side A) hits: %i  residual: %.4f  microns T0 diff: %.3f ns",(int)bhits4,res4,tres4));
  pt->AddText(Form(" (side A) Time res Width: %.2f   ns Ratio prec hits: %.3f ",tres4w,ratioec));

  pt->SetLabel(Form("TRT calibration iteration summary"));
  
  pt->Draw();
}


//================================================================

class SettingsInfo: public TCanvas{
public: 
  SettingsInfo(char*) throw(string);
  map<string,string> set;
private:
};

SettingsInfo::SettingsInfo(char* filename) throw(string){
  
  this->cd();
  
  TPaveText* pt = new TPaveText(.1,.1,0.9,0.9);
  pt->SetTextAlign(11);
  pt->AddText(" ");
  pt->AddText(" ");
  pt->SetLabel(Form("TRT calibration Main Settings"));
  
  string line;
  ifstream myfile(filename);
  if (myfile.is_open()){
    while (! myfile.eof() ){
      getline (myfile,line);
      if (line.find("#") && line.find("Clean") && line.find("Submit") && line.find("Relink") && line.find("JobPrefix")  && line.find("Tag") && line.find("WWW")  )  {
	int space = line.find_first_of(" ");
	if (space!=-1){
	  //pt->AddText(line.c_str());
	  set[trim(line.substr(0,line.find("=")-1))]=trim(line.substr(line.find("=")+1,line.size()));
	}
      }
    }
    for (map<string,string>::iterator is = set.begin(); is != set.end(); is++){
      pt->AddText( (string(is->first + " . . . . . . . . . . . . " + is->second)).c_str() );
    }
    myfile.close();
  }
  else throw(string("config file not found: ") + string(filename));

  pt->Draw();
}


//================================================================

class TrackTupleInfo: public TCanvas{
public: 
  TrackTupleInfo(TFile*) throw(string);
private:
  TNtuple* Tracktuple;
};

TrackTupleInfo::TrackTupleInfo(TFile* file) throw(string){

  Tracktuple = (TNtuple*)file->Get("tracktuple");
  if (!Tracktuple) throw(string("tracktuple not found!"));
  
  this->SetName("c1");
  this->Divide(1,2,0.01,0.01);
  ((TPad*) this->GetPrimitive("c1_1"))->SetLogy();  
  this->cd(1);
  TH1F* trackhits   = new TH1F("trackhits","Accepted hits per track",60,-0.5,59.5);
  TH1F* trackhits1   = new TH1F("trackhits1","Accepted hits per track BarrelA",60,-0.5,59.5);
  TH1F* trackhits2   = new TH1F("trackhits2","Accepted hits per track EndcaoA",60,-0.5,59.5);
  TH1F* trackhits3   = new TH1F("trackhits3","Accepted hits per track BarrelC",60,-0.5,59.5);
  TH1F* trackhits4   = new TH1F("trackhits4","Accepted hits per track EndcaoC",60,-0.5,59.5);
  
  TH1F* eventphase   = new TH1F("eventphase","Event Phase",80,-18,-1);
  TH1F* eventphase1  = new TH1F("eventphase1","Eventphase1",80,-18,-1);
  TH1F* eventphase2  = new TH1F("eventphase2","Eventphase2",80,-18,-1);
  TH1F* eventphase3  = new TH1F("eventphase3","Eventphase3",80,-18,-1);
  TH1F* eventphase4  = new TH1F("eventphase4","Eventphase4",80,-18,-1);
  
  Tracktuple->Draw("nhits>>trackhits");
  
  Tracktuple->Draw("nhits>>trackhits1","-log(tan(theta/2))>0 && -log(tan(theta/2))< 1 ","same");
  Tracktuple->Draw("nhits>>trackhits2","-log(tan(theta/2))>1","same");
  Tracktuple->Draw("nhits>>trackhits3","-log(tan(theta/2))<0 && -log(tan(theta/2))> -1 ","same");
  Tracktuple->Draw("nhits>>trackhits4","-log(tan(theta/2))<-1 ","same");
  trackhits1->SetLineColor(2);
  trackhits2->SetLineColor(3);
  trackhits3->SetLineColor(4);
  trackhits4->SetLineColor(5);
  trackhits1->Draw("same");
  trackhits2->Draw("same");
  trackhits3->Draw("same");
  trackhits4->Draw("same");
  TLegend *leg1 = new TLegend(0.7155172,0.1038136,0.8965517,0.2542373,NULL,"brNDC");
  leg1->AddEntry(trackhits,"all tracks","l");
  leg1->AddEntry(trackhits1,"0 < eta < 1","l");
  leg1->AddEntry(trackhits2,"eta > 1","l");
  leg1->AddEntry(trackhits3,"-1 < eta < 0","l");
  leg1->AddEntry(trackhits4,"eta < -1","l");
  leg1->SetTextSize(0.03);
  leg1->Draw();
  
  trackhits->SetTitle(Form("Accepted  precission hits per track (%i tracks in total)",Tracktuple->GetEntries()));
  trackhits->GetXaxis()->SetTitle("accepted precission hits");
  trackhits->GetYaxis()->SetTitle("No. tracks");
  this->SetName("c1");
  this->cd(2);
  ((TPad*) this->GetPrimitive("c1_2"))->SetLogy();  

  Tracktuple->Draw("epold>>eventphase");
  Tracktuple->Draw("epold>>eventphase1","-log(tan(theta/2))>0 && -log(tan(theta/2))< 1","same");
  Tracktuple->Draw("epold>>eventphase2","-log(tan(theta/2))>1","same");
  Tracktuple->Draw("epold>>eventphase3","-log(tan(theta/2))<0 && -log(tan(theta/2))> -1","same");
  Tracktuple->Draw("epold>>eventphase4","-log(tan(theta/2))<-1","same");
  
  eventphase1->SetLineColor(2);
  eventphase2->SetLineColor(3);
  eventphase3->SetLineColor(4);
  eventphase4->SetLineColor(5);
  
  eventphase->SetTitle("Eventphase");
  eventphase->GetXaxis()->SetTitle("eventphase");
  eventphase->GetYaxis()->SetTitle("No. tracks");
  
  TLegend *leg2 = new TLegend(0.7155172,0.1038136,0.8965517,0.2542373,NULL,"brNDC");
  leg2->AddEntry(trackhits1,"0 < eta < 1","l");
  leg2->AddEntry(trackhits2,"eta > 1","l");
  leg2->AddEntry(trackhits3,"-1 < eta < 0","l");
  leg2->AddEntry(trackhits4,"eta < -1","l");
  leg2->SetTextSize(0.03);
  leg2->Draw();  
}


//================================================================

TGraphErrors * GetMean( TH2F* histo){

	int const nxbins = histo->GetNbinsX() ;
	TGraphErrors * Means = new TGraphErrors(nxbins);
	float mean  = 0;
	float rms   = 0;
	 TF1 * ff = new TF1("ff","gaus");
	 for (int i=0;i<nxbins;i++) {
    		TH1D* slice  =  histo->ProjectionY("slice",i+1,i+1) ;
		mean = slice->GetMean();
		rms  = slice->GetRMS();
                ff->SetParameter(0,slice->GetEntries());
                ff->SetParameter(1,mean);
                ff->SetParameter(2,rms);
		int fitresult = slice->Fit("ff","QR","", mean - 1.5*rms , mean + 1.5*rms   );
		mean = ff->GetParameter(1);
		rms = ff->GetParameter(2);
		for (int j = 0; j< 6 ; j++){
			fitresult = slice->Fit("ff","QR","", mean - 1.5*rms , mean + 1.5*rms   );
			mean = ff->GetParameter(1);
			rms = ff->GetParameter(2);
		}
		fitresult = slice->Fit("ff","QR","", mean - 1.5*rms , mean + 1.5*rms   );
		float x = histo->GetBinCenter(i + 1);
		Means->SetPoint(i,x, mean);
		Means->SetPointError(i,0.5*histo->GetBinWidth(i),  ff->GetParError(1));
		delete slice;
	}
	delete ff;
	return Means;
}

TGraphErrors * GetMeanE( TH2F* histo){

	int const nxbins = histo->GetNbinsX() ;
	TGraphErrors * Means = new TGraphErrors(nxbins);
	float mean  = 0;
	float rms   = 0;
	 TF1 * ff = new TF1("ff","gaus");
	 for (int i=0;i<nxbins;i++) {
    		TH1D* slice  =  histo->ProjectionY("slice",i+1,i+1) ;
		mean = slice->GetMean();
		rms  = slice->GetRMS();
		float x = histo->GetBinCenter(i + 1);
		Means->SetPoint(i,x, mean);
		Means->SetPointError(i,0.5*histo->GetBinWidth(i),  ff->GetParError(1));
		delete slice;
	}
	delete ff;
	return Means;
}

TGraphErrors * GetPn( TH2F* histo){

	int const nxbins = histo->GetNbinsX() ;
	TGraphErrors * Means = new TGraphErrors(nxbins);
	float mean  = 0;
	float rms   = 0;
	 TF1 * ff = new TF1("ff","gaus");
	 for (int i=0;i<nxbins;i++) {
    		TH1D* slice  =  histo->ProjectionY("slice",i+1,i+1) ;
		mean = slice->GetEntries();
		float x = histo->GetBinCenter(i + 1);
		Means->SetPoint(i,x, mean);
		Means->SetPointError(i,0.5*histo->GetBinWidth(i),  ff->GetParError(1));
		delete slice;
	}
	delete ff;
	return Means;
}

TGraphErrors * GetWidth( TH2F* histo){
	int const nxbins = histo->GetNbinsX() ;
	TGraphErrors * Means = new TGraphErrors(nxbins);
	float mean  = 0;
	float rms   = 0;
	 TF1 * ff = new TF1("ff","gaus");
	 for (int i=0;i<nxbins;i++) {
    		TH1D* slice  =  histo->ProjectionY("slice",i+1,i+1) ;
		mean = slice->GetMean();
		rms  = slice->GetRMS();
		ff->SetParameter(0,slice->GetEntries());
		ff->SetParameter(1,mean);
		ff->SetParameter(2,rms);
		int fitresult = slice->Fit("ff","QR","", mean - 1.5*rms , mean + 1.5*rms   );
		mean = ff->GetParameter(1);
		rms = ff->GetParameter(2);
		for (int j = 0; j< 6 ; j++){
			fitresult = slice->Fit("ff","QR","", mean - 1.5*rms , mean + 1.5*rms   );
			mean = ff->GetParameter(1);
			rms = ff->GetParameter(2);
		}
		fitresult = slice->Fit("ff","QR","", mean - 1.5*rms , mean + 1.5*rms   );
		float x = histo->GetBinCenter(i+1);
		Means->SetPoint(i,x, rms);
		Means->SetPointError(i,0.5*histo->GetBinWidth(i),  ff->GetParError(2));
		delete slice;
	}
	delete ff;
	return Means;
}


TGraphErrors * GetEntries( TH2F* histo){
        int const nxbins = histo->GetNbinsX() ;
        TGraphErrors * Means = new TGraphErrors(nxbins);
        float mean  = 0;
        float rms   = 0;
         TF1 * ff = new TF1("ff","gaus");
         for (int i=0;i<nxbins;i++) {
                TH1D* slice  =  histo->ProjectionY("slice",i+1,i+1) ;
                mean = slice->GetEntries();
                rms  = sqrt(mean);
		float x = histo->GetBinCenter(i+1);
                Means->SetPoint(i,x, mean);
                Means->SetPointError(i,0.5*histo->GetBinWidth(i), rms );
                delete slice;
        }
        delete ff;
        return Means;
}


TGraphErrors * GetPWidth( TH2F* histo){
	int const nxbins = histo->GetNbinsX() ;
	TGraphErrors * Means = new TGraphErrors(nxbins);
	float mean  = 0;
	float rms   = 0;
	 TF1 * ff = new TF1("ff","gaus");
	 for (int i=0;i<nxbins;i++) {
    		TH1D* slice  =  histo->ProjectionY("slice",i+1,i+1) ;
		mean = slice->GetMean();
		rms  = slice->GetRMS();
		ff->SetParameter(0,slice->GetEntries());
		ff->SetParameter(1,mean);
		ff->SetParameter(2,rms);
		int fitresult = slice->Fit("ff","QR","",  - 2.5 , + 2.5   );
		mean = ff->GetParameter(1);
		rms = ff->GetParameter(2);
		fitresult = slice->Fit("ff","QR","", -2.5, 2.5   );
		mean = ff->GetParameter(1);
		rms = ff->GetParameter(2);
		fitresult = slice->Fit("ff","QR","", -2.5 , 2.5   );
		float x = histo->GetBinCenter(i+1);
		Means->SetPoint(i,x, rms);
		Means->SetPointError(i,0.5*histo->GetBinWidth(i),  ff->GetParError(2));
		delete slice;
	}
	delete ff;
	return Means;
}






class PtDependence: public TCanvas{
public: 
  PtDependence(TFile*) throw(string);
private:
  TDirectory* errors;
};

PtDependence::PtDependence(TFile* file) throw(string){
  errors = (TDirectory*) file->FindKey("Errors")->ReadObj();
  if (!errors) throw(string("No errors folder!!!!!!!!!!!!!!!"));
  this->Divide(2,3,0.01,0.01);
        // Read the histos:
         TH2F*      residual_trt    	=  (TH2F*)errors->FindKey("residual_trt")->ReadObj();
         TH2F*      residual_ba  	=  (TH2F*)errors->FindKey("residual_ba")->ReadObj();
         TH2F*      residual_bc  	=  (TH2F*)errors->FindKey("residual_bc")->ReadObj();
         TH2F*      residual_ea  	=  (TH2F*)errors->FindKey("residual_ea")->ReadObj();
         TH2F*      residual_ec 	=  (TH2F*)errors->FindKey("residual_ec")->ReadObj();
         TH2F*      tresidual_trt    	=  (TH2F*)errors->FindKey("tresidual_trt")->ReadObj();
         TH2F*      tresidual_ba  	=  (TH2F*)errors->FindKey("tresidual_ba")->ReadObj();
         TH2F*      tresidual_bc  	=  (TH2F*)errors->FindKey("tresidual_bc")->ReadObj();
         TH2F*      tresidual_ea  	=  (TH2F*)errors->FindKey("tresidual_ea")->ReadObj();
         TH2F*      tresidual_ec 	=  (TH2F*)errors->FindKey("tresidual_ec")->ReadObj();
         TH2F*      pull_trtP   =  (TH2F*)errors->FindKey("pull_trtP")->ReadObj();
         TH2F*      pull_baP  	=  (TH2F*)errors->FindKey("pull_baP")->ReadObj();
         TH2F*      pull_bcP  	=  (TH2F*)errors->FindKey("pull_bcP")->ReadObj();
         TH2F*      pull_eaP  	=  (TH2F*)errors->FindKey("pull_eaP")->ReadObj();
         TH2F*      pull_ecP  	=  (TH2F*)errors->FindKey("pull_ecP")->ReadObj();
         TH2F*      residual_trtP    	=  (TH2F*)errors->FindKey("residual_trtP")->ReadObj();
         TH2F*      residual_baP 	=  (TH2F*)errors->FindKey("residual_baP")->ReadObj();
         TH2F*      residual_bcP  	=  (TH2F*)errors->FindKey("residual_bcP")->ReadObj();
         TH2F*      residual_eaP  	=  (TH2F*)errors->FindKey("residual_eaP")->ReadObj();
         TH2F*      residual_ecP 	=  (TH2F*)errors->FindKey("residual_ecP")->ReadObj();
         TH2F*      tresidual_trtP    	=  (TH2F*)errors->FindKey("tresidual_trtP")->ReadObj();
         TH2F*      tresidual_baP 	=  (TH2F*)errors->FindKey("tresidual_baP")->ReadObj();
         TH2F*      tresidual_bcP  	=  (TH2F*)errors->FindKey("tresidual_bcP")->ReadObj();
         TH2F*      tresidual_eaP  	=  (TH2F*)errors->FindKey("tresidual_eaP")->ReadObj();
         TH2F*      tresidual_ecP 	=  (TH2F*)errors->FindKey("tresidual_ecP")->ReadObj();
         TH2F*      trackerrors_trt    	=  (TH2F*)errors->FindKey("trackerrors_trt")->ReadObj();
         TH2F*      trackerrors_ba    	=  (TH2F*)errors->FindKey("trackerrors_ba")->ReadObj();
         TH2F*      trackerrors_bc    	=  (TH2F*)errors->FindKey("trackerrors_bc")->ReadObj();
         TH2F*      trackerrors_ea    	=  (TH2F*)errors->FindKey("trackerrors_ea")->ReadObj();
         TH2F*      trackerrors_ec    	=  (TH2F*)errors->FindKey("trackerrors_ec")->ReadObj();

  this->cd(1);
		// Time Residual Mean Vs Pt:
	TGraphErrors * tresTRT = GetMean(tresidual_trtP);   
	TGraphErrors * tresBA  = GetMean(tresidual_baP);    tresBA->SetLineColor(kRed); tresBA->SetMarkerColor(kRed);  		
	TGraphErrors * tresBC  = GetMean(tresidual_bcP);    tresBC->SetLineColor(kBlue); tresBC->SetMarkerColor(kBlue); 	
	TGraphErrors * tresEA  = GetMean(tresidual_eaP);    tresEA->SetLineColor(kGreen); tresEA->SetMarkerColor(kGreen);  
	TGraphErrors * tresEC  = GetMean(tresidual_ecP);    tresEC->SetLineColor(kYellow); tresEC->SetMarkerColor(kYellow);  

	tresTRT->SetTitle ("Time Residual Mean VS PT");
	tresTRT->GetXaxis()->SetTitle ("Pt (GeV)");
	tresTRT->GetYaxis()->SetTitle ("ns");
	tresTRT->GetYaxis()->SetRangeUser(-0.8,0.4);
	tresTRT	->Draw("ALP");
	tresBA  ->Draw("LP");
	tresBC  ->Draw("LP");
	tresEA  ->Draw("LP");
	tresEC  ->Draw("LP");

  this->cd(3);
		// Time Residual Width Vs Pt

	TGraphErrors * tresTRTw = GetWidth(tresidual_trtP);   
	TGraphErrors * tresBAw  = GetWidth(tresidual_baP);     tresBAw->SetLineColor(kRed); tresBAw->SetMarkerColor(kRed);  
	TGraphErrors * tresBCw  = GetWidth(tresidual_bcP);     tresBCw->SetLineColor(kBlue); tresBCw->SetMarkerColor(kBlue);  
	TGraphErrors * tresEAw  = GetWidth(tresidual_eaP);     tresEAw->SetLineColor(kGreen); tresEAw->SetMarkerColor(kGreen); 
	TGraphErrors * tresECw  = GetWidth(tresidual_ecP);     tresECw->SetLineColor(kYellow); tresECw->SetMarkerColor(kYellow);  

	tresTRTw->SetTitle ("Time Residual Width VS PT");
	tresTRTw->GetXaxis()->SetTitle ("Pt (GeV)");
	tresTRTw->GetYaxis()->SetTitle ("ns");
	tresTRTw->GetYaxis()->SetRangeUser(2.9, 4.0);
	tresTRTw	->Draw("ALP");
	tresBAw  ->Draw("LP");
	tresBCw  ->Draw("LP");
	tresEAw  ->Draw("LP");
	tresECw  ->Draw("LP");
  this->cd(5);

		// Residual Vs Pt:	
        TGraphErrors * resTRTw = GetWidth(residual_trtP);   
        TGraphErrors * resBAw  = GetWidth(residual_baP);     resBAw->SetLineColor(kRed); resBAw->SetMarkerColor(kRed);  
        TGraphErrors * resBCw  = GetWidth(residual_bcP);     resBCw->SetLineColor(kBlue); resBCw->SetMarkerColor(kBlue);  
        TGraphErrors * resEAw  = GetWidth(residual_eaP);     resEAw->SetLineColor(kGreen); resEAw->SetMarkerColor(kGreen);  
        TGraphErrors * resECw  = GetWidth(residual_ecP);     resECw->SetLineColor(kYellow); resECw->SetMarkerColor(kYellow); 

        resTRTw->SetTitle ("Residual Width VS PT");
        resTRTw->GetXaxis()->SetTitle ("Pt (GeV)");
        resTRTw->GetYaxis()->SetTitle ("mm");
	resTRTw->GetYaxis()->SetRangeUser(0.11,0.18);
        resTRTw        ->Draw("ALP");
        resBAw  ->Draw("LP");
        resBCw  ->Draw("LP");
        resEAw  ->Draw("LP");
        resECw  ->Draw("LP");


  this->cd(2);
		// Pull Width vs  Pt:	
        TGraphErrors * pullTRTw = GetPWidth(pull_trtP);   
        TGraphErrors * pullBAw  = GetPWidth(pull_baP);     pullBAw->SetLineColor(kRed); pullBAw->SetMarkerColor(kRed);  
        TGraphErrors * pullBCw  = GetPWidth(pull_bcP);     pullBCw->SetLineColor(kBlue); pullBCw->SetMarkerColor(kBlue);  
        TGraphErrors * pullEAw  = GetPWidth(pull_eaP);     pullEAw->SetLineColor(kGreen); pullEAw->SetMarkerColor(kGreen);  
        TGraphErrors * pullECw  = GetPWidth(pull_ecP);     pullECw->SetLineColor(kYellow); pullECw->SetMarkerColor(kYellow); 

        pullTRTw->SetTitle ("Pull Width VS PT");
        pullTRTw->GetXaxis()->SetTitle ("Pt (GeV)");
        pullTRTw->GetYaxis()->SetTitle ("  ");
	pullTRTw->GetYaxis()->SetRangeUser(0.7,1.3);
        pullTRTw        ->Draw("ALP");
        pullBAw  ->Draw("LP");
        pullBCw  ->Draw("LP");
        pullEAw  ->Draw("LP");
        pullECw  ->Draw("LP");

  this->cd(4);
		// Pull Width vs  Pt:	
        TGraphErrors * trkTRTw = GetMean(trackerrors_trt);   
        TGraphErrors * trkBAw  = GetMean(trackerrors_ba);     trkBAw->SetLineColor(kRed); trkBAw->SetMarkerColor(kRed);  
        TGraphErrors * trkBCw  = GetMean(trackerrors_bc);     trkBCw->SetLineColor(kBlue); trkBCw->SetMarkerColor(kBlue);  
        TGraphErrors * trkEAw  = GetMean(trackerrors_ea);     trkEAw->SetLineColor(kGreen); trkEAw->SetMarkerColor(kGreen);  
        TGraphErrors * trkECw  = GetMean(trackerrors_ec);     trkECw->SetLineColor(kYellow); trkECw->SetMarkerColor(kYellow); 

        trkTRTw->SetTitle ("Trk error Mean Width VS PT");
        trkTRTw->GetXaxis()->SetTitle ("Pt (GeV)");
        trkTRTw->GetYaxis()->SetTitle ("mm");
	trkTRTw->GetYaxis()->SetRangeUser(0.0,0.07);
        trkTRTw        ->Draw("ALP");
        trkBAw  ->Draw("LP");
        trkBCw  ->Draw("LP");
        trkEAw  ->Draw("LP");
        trkECw  ->Draw("LP");


  this->cd(6);

                // Residual Vs Pt:      
        TGraphErrors * presTRTw = GetWidth(residual_trt);
        TGraphErrors * presBAw  = GetWidth(residual_ba);     presBAw->SetLineColor(kRed); presBAw->SetMarkerColor(kRed);
        TGraphErrors * presBCw  = GetWidth(residual_bc);     presBCw->SetLineColor(kBlue); presBCw->SetMarkerColor(kBlue);
        TGraphErrors * presEAw  = GetWidth(residual_ea);     presEAw->SetLineColor(kGreen); presEAw->SetMarkerColor(kGreen);
        TGraphErrors * presECw  = GetWidth(residual_ec);     presECw->SetLineColor(kYellow); presECw->SetMarkerColor(kYellow);

        presTRTw->SetTitle ("Residual Width all hits VS PT");
        presTRTw->GetXaxis()->SetTitle ("Pt (GeV)");
        presTRTw->GetYaxis()->SetTitle ("mm");
        presTRTw->GetYaxis()->SetRangeUser(0.11,0.18);
        presTRTw        ->Draw("ALP");
        presBAw  ->Draw("LP");
        presBCw  ->Draw("LP");
        presEAw  ->Draw("LP");
        presECw  ->Draw("LP");
}




class TBDependence: public TCanvas{
public: 
  TBDependence(TFile*) throw(string);
private:
  TDirectory* errors;
};

TBDependence::TBDependence(TFile* file) throw(string){
  errors = (TDirectory*) file->FindKey("Errors")->ReadObj();
  if (!errors) throw(string("No errors folder!!!!!!!!!!!!!!!"));
  this->Divide(2,3,0.01,0.01);

        // Read the histos:

         TH2F*  pull_vs_tb_trt	 =  (TH2F*)errors->FindKey("pull_vs_tb_trt")->ReadObj();
         TH2F*  pull_vs_tb_ba	 =  (TH2F*)errors->FindKey("pull_vs_tb_ba")->ReadObj();
         TH2F*  pull_vs_tb_bc	 =  (TH2F*)errors->FindKey("pull_vs_tb_bc")->ReadObj();
         TH2F*  pull_vs_tb_ea	 =  (TH2F*)errors->FindKey("pull_vs_tb_ea")->ReadObj();
         TH2F*  pull_vs_tb_ec	 =  (TH2F*)errors->FindKey("pull_vs_tb_ec")->ReadObj();

         TH2F*  errors_vs_tb_trt	 =  (TH2F*)errors->FindKey("errors_vs_tb_trt")->ReadObj();
         TH2F*  errors_vs_tb_ba	 =  (TH2F*)errors->FindKey("errors_vs_tb_ba")->ReadObj();
         TH2F*  errors_vs_tb_bc	 =  (TH2F*)errors->FindKey("errors_vs_tb_bc")->ReadObj();
         TH2F*  errors_vs_tb_ea	 =  (TH2F*)errors->FindKey("errors_vs_tb_ea")->ReadObj();
         TH2F*  errors_vs_tb_ec	 =  (TH2F*)errors->FindKey("errors_vs_tb_ec")->ReadObj();
 
         TH2F*  residual_vs_tb_trt		=  (TH2F*)errors->FindKey("residual_vs_tb_trt")->ReadObj();
         TH2F*  residual_vs_tb_ba	 	=  (TH2F*)errors->FindKey("residual_vs_tb_ba")->ReadObj();
         TH2F*  residual_vs_tb_bc	 	=  (TH2F*)errors->FindKey("residual_vs_tb_bc")->ReadObj();
         TH2F*  residual_vs_tb_ea	 	=  (TH2F*)errors->FindKey("residual_vs_tb_ea")->ReadObj();
         TH2F*  residual_vs_tb_ec	 	=  (TH2F*)errors->FindKey("residual_vs_tb_ec")->ReadObj();

         TH2F*  trackerrors_vs_tb_trt	=  (TH2F*)errors->FindKey("trackerrors_vs_tb_trt")->ReadObj();
         TH2F*  trackerrors_vs_tb_ba 	=  (TH2F*)errors->FindKey("trackerrors_vs_tb_ba")->ReadObj();
         TH2F*  trackerrors_vs_tb_bc 	=  (TH2F*)errors->FindKey("trackerrors_vs_tb_bc")->ReadObj();
         TH2F*  trackerrors_vs_tb_ea 	=  (TH2F*)errors->FindKey("trackerrors_vs_tb_ea")->ReadObj();
         TH2F*  trackerrors_vs_tb_ec 	=  (TH2F*)errors->FindKey("trackerrors_vs_tb_ec")->ReadObj();




  this->cd(1);
		// Pull widht Vs Pt:
	TGraphErrors * tresTRT = GetPWidth(pull_vs_tb_trt);   
	TGraphErrors * tresBA  = GetPWidth(pull_vs_tb_ba);    tresBA->SetLineColor(kRed); tresBA->SetMarkerColor(kRed);  		
	TGraphErrors * tresBC  = GetPWidth(pull_vs_tb_bc);    tresBC->SetLineColor(kBlue); tresBC->SetMarkerColor(kBlue); 	
	TGraphErrors * tresEA  = GetPWidth(pull_vs_tb_ea);    tresEA->SetLineColor(kGreen); tresEA->SetMarkerColor(kGreen);  
	TGraphErrors * tresEC  = GetPWidth(pull_vs_tb_ec);    tresEC->SetLineColor(kYellow); tresEC->SetMarkerColor(kYellow);  

	tresTRT->SetTitle ("Pull Width VS Drift Time. Pt>2GeV");
	tresTRT->GetXaxis()->SetTitle ("Drift Time ");
	tresTRT->GetYaxis()->SetTitle ("   ");
	tresTRT->GetYaxis()->SetRangeUser(0.7,1.3);
	tresTRT	->Draw("ALP");
	tresBA  ->Draw("LP");
	tresBC  ->Draw("LP");
	tresEA  ->Draw("LP");
	tresEC  ->Draw("LP");

  this->cd(2);
	TGraphErrors * errorsTRT = GetMeanE(errors_vs_tb_trt);   
	TGraphErrors * errorsBA  = GetMeanE(errors_vs_tb_ba);    errorsBA->SetLineColor(kRed); 		errorsBA->SetMarkerColor(kRed);  		
	TGraphErrors * errorsBC  = GetMeanE(errors_vs_tb_bc);    errorsBC->SetLineColor(kBlue); 	errorsBC->SetMarkerColor(kBlue); 	
	TGraphErrors * errorsEA  = GetMeanE(errors_vs_tb_ea);    errorsEA->SetLineColor(kGreen); 	errorsEA->SetMarkerColor(kGreen);  
	TGraphErrors * errorsEC  = GetMeanE(errors_vs_tb_ec);    errorsEC->SetLineColor(kYellow); 	errorsEC->SetMarkerColor(kYellow);  
	errorsTRT->SetTitle ("Error VS Drift Time");
	errorsTRT->GetXaxis()->SetTitle ("Drift Time");
	errorsTRT->GetYaxis()->SetTitle ("mm");
	errorsTRT->GetYaxis()->SetRangeUser(0.05,0.24);
	errorsTRT ->Draw("ALP");
	errorsBA  ->Draw("LP");
	errorsBC  ->Draw("LP");
	errorsEA  ->Draw("LP");
	errorsEC  ->Draw("LP");


  this->cd(3);
	TGraphErrors * ntresBA  = GetPn(pull_vs_tb_ba);    ntresBA->SetLineColor(kRed); ntresBA->SetMarkerColor(kRed);  		
	TGraphErrors * ntresBC  = GetPn(pull_vs_tb_bc);    ntresBC->SetLineColor(kBlue); ntresBC->SetMarkerColor(kBlue); 	
	TGraphErrors * ntresEA  = GetPn(pull_vs_tb_ea);    ntresEA->SetLineColor(kGreen); ntresEA->SetMarkerColor(kGreen);  
	TGraphErrors * ntresEC  = GetPn(pull_vs_tb_ec);    ntresEC->SetLineColor(kYellow); ntresEC->SetMarkerColor(kYellow);  
	ntresEA->SetTitle ("Number of hits VS Drift Time. Pt>2GeV");
	ntresEA->GetXaxis()->SetTitle ("Drift Time");
	ntresEA->GetYaxis()->SetTitle (" hits  ");
	ntresEA  ->Draw("ALP");
	ntresEC  ->Draw("LP");
	ntresBA  ->Draw("LP");
	ntresBC  ->Draw("LP");


  this->cd(4);
	TGraphErrors * terrorsTRT = GetMean(trackerrors_vs_tb_trt);   
	TGraphErrors * terrorsBA  = GetMean(trackerrors_vs_tb_ba);    terrorsBA->SetLineColor(kRed); 		terrorsBA->SetMarkerColor(kRed);  		
	TGraphErrors * terrorsBC  = GetMean(trackerrors_vs_tb_bc);    terrorsBC->SetLineColor(kBlue); 		terrorsBC->SetMarkerColor(kBlue); 	
	TGraphErrors * terrorsEA  = GetMean(trackerrors_vs_tb_ea);    terrorsEA->SetLineColor(kGreen); 	terrorsEA->SetMarkerColor(kGreen);  
	TGraphErrors * terrorsEC  = GetMean(trackerrors_vs_tb_ec);    terrorsEC->SetLineColor(kYellow); 	terrorsEC->SetMarkerColor(kYellow);  
	terrorsTRT->SetTitle ("Track Error VS Drift Time");
	terrorsTRT->GetXaxis()->SetTitle ("Drift Time");
	terrorsTRT->GetYaxis()->SetTitle ("mm");
	terrorsTRT->GetYaxis()->SetRangeUser(0.0,0.07);
	terrorsTRT ->Draw("ALP");
	terrorsBA  ->Draw("LP");
	terrorsBC  ->Draw("LP");
	terrorsEA  ->Draw("LP");
	terrorsEC  ->Draw("LP");


  this->cd(5);
	TGraphErrors * residualTRT = GetWidth(residual_vs_tb_trt);   
	TGraphErrors * residualBA  = GetWidth(residual_vs_tb_ba);    residualBA->SetLineColor(kRed); 		residualBA->SetMarkerColor(kRed);  		
	TGraphErrors * residualBC   = GetWidth(residual_vs_tb_bc);    residualBC->SetLineColor(kBlue); 		residualBC->SetMarkerColor(kBlue); 	
	TGraphErrors * residualEA   = GetWidth(residual_vs_tb_ea);    residualEA->SetLineColor(kGreen); 		residualEA->SetMarkerColor(kGreen);  
	TGraphErrors * residualEC   = GetWidth(residual_vs_tb_ec);    residualEC->SetLineColor(kYellow); 		residualEC->SetMarkerColor(kYellow);  
	residualTRT->SetTitle ("Residual VS Drift Time. Pt > 2GeV");
	residualTRT->GetXaxis()->SetTitle ("Drift Time ");
	residualTRT->GetYaxis()->SetTitle ("mm");
	residualTRT->GetYaxis()->SetRangeUser(0.05,0.24);
	residualTRT ->Draw("ALP");
	residualBA  ->Draw("LP");
	residualBC  ->Draw("LP");
	residualEA  ->Draw("LP");
	residualEC  ->Draw("LP");

  this->cd(6);

         TH2F*      residual_trt    	=  (TH2F*)errors->FindKey("tresidual_trt")->ReadObj();
         TH2F*      residual_ba  	=  (TH2F*)errors->FindKey("tresidual_ba")->ReadObj();
         TH2F*      residual_bc  	=  (TH2F*)errors->FindKey("tresidual_bc")->ReadObj();
         TH2F*      residual_ea  	=  (TH2F*)errors->FindKey("tresidual_ea")->ReadObj();
         TH2F*      residual_ec 	=  (TH2F*)errors->FindKey("tresidual_ec")->ReadObj();

         TH2F*      residual_trtP        =  (TH2F*)errors->FindKey("tresidual_trtP")->ReadObj();
         TH2F*      residual_baP         =  (TH2F*)errors->FindKey("tresidual_baP")->ReadObj();
         TH2F*      residual_bcP         =  (TH2F*)errors->FindKey("tresidual_bcP")->ReadObj();
         TH2F*      residual_eaP         =  (TH2F*)errors->FindKey("tresidual_eaP")->ReadObj();
         TH2F*      residual_ecP         =  (TH2F*)errors->FindKey("tresidual_ecP")->ReadObj();

	TGraphErrors * residualTRT1  = GetEntries(residual_trt);   
	TGraphErrors * residualBA1   = GetEntries(residual_ba);    residualBA1->SetLineColor(kRed); 		residualBA1->SetMarkerColor(kRed);  		
	TGraphErrors * residualBC1   = GetEntries(residual_bc);    residualBC1->SetLineColor(kBlue); 		residualBC1->SetMarkerColor(kBlue); 	
	TGraphErrors * residualEA1   = GetEntries(residual_ea);    residualEA1->SetLineColor(kGreen); 		residualEA1->SetMarkerColor(kGreen);  
	TGraphErrors * residualEC1   = GetEntries(residual_ec);    residualEC1->SetLineColor(kYellow); 		residualEC1->SetMarkerColor(kYellow);  

	TGraphErrors * residualTRT2  = GetEntries(residual_trtP);   
	TGraphErrors * residualBA2   = GetEntries(residual_baP);    residualBA2->SetLineColor(kRed); 		residualBA2->SetMarkerColor(kRed);  		
	TGraphErrors * residualBC2   = GetEntries(residual_bcP);    residualBC2->SetLineColor(kBlue); 		residualBC2->SetMarkerColor(kBlue); 	
	TGraphErrors * residualEA2   = GetEntries(residual_eaP);    residualEA2->SetLineColor(kGreen); 		residualEA2->SetMarkerColor(kGreen);  
	TGraphErrors * residualEC2   = GetEntries(residual_ecP);    residualEC2->SetLineColor(kYellow); 		residualEC2->SetMarkerColor(kYellow);  

	int nbins = residualTRT1->GetN();

	TGraphErrors * ratioTRT  =  new TGraphErrors(nbins);
	TGraphErrors * ratioBA   =  new TGraphErrors(nbins);
	TGraphErrors * ratioBC   =  new TGraphErrors(nbins);
	TGraphErrors * ratioEA   =  new TGraphErrors(nbins);
	TGraphErrors * ratioEC   =  new TGraphErrors(nbins);

	for (int i = 0; i< nbins; i++){
		double prec 	= 0;
		double all 	= 0;
		double x 	= 0;
		residualTRT1->GetPoint(i,x,all);
		residualTRT2->GetPoint(i,x,prec);
		cout << i << " " << x << " " << prec << " " << all << endl;
		if( all > 0 ){		ratioTRT->SetPoint(i,x,prec/all); }
		else {			ratioTRT->SetPoint(i,x,0); }
		residualBA1	->GetPoint(i,x,all);
		residualBA2	->GetPoint(i,x,prec);
		cout << i << " " << x << " " << prec << " " << all << endl;
		if (all > 0 ){	ratioBA		->SetPoint(i,x,prec/all); }
		else {			ratioBC->SetPoint(i,x,0); }

		residualBC1	->GetPoint(i,x,all);
		residualBC2	->GetPoint(i,x,prec);
		cout << i << " " << x << " " << prec << " " << all << endl;
                if (all > 0) {	ratioBC		->SetPoint(i,x,prec/all);  }
		else {			ratioBC->SetPoint(i,x,0); }

		residualEA1	->GetPoint(i,x,all);
		residualEA2	->GetPoint(i,x,prec);
		cout << i << " " << x << " " << prec << " " << all << endl;
                if (all > 0) {		ratioEA		->SetPoint(i,x,prec/all); }
		else {			ratioEA->SetPoint(i,x,0); }

		residualEC1	->GetPoint(i,x,all);
		residualEC2	->GetPoint(i,x,prec);
		cout << i << " " << x << " " << prec << " " << all << endl;
                if (all > 0) {		ratioEC		->SetPoint(i,x,prec/all); }
		else {			ratioEC->SetPoint(i,x,0); }
		cout << endl;

	}

        ratioTRT->SetTitle ("Ratio Precision Hits Vs Pt. Pt > 2GeV");
        ratioTRT->GetXaxis()->SetTitle ("Precission Hits/Total Hits ");
        ratioTRT->GetYaxis()->SetTitle ("pt");
        ratioTRT->GetYaxis()->SetRangeUser(0.05,0.24);
        ratioTRT->GetYaxis()->SetRangeUser(0.3,1);

	ratioBA->SetLineColor(kRed);
	ratioBC->SetLineColor(kBlue);
	ratioEA->SetLineColor(kGreen);
	ratioEC->SetLineColor(kYellow);

	ratioTRT->Draw("alp");
	ratioBA ->Draw("lp");
	ratioBC ->Draw("lp");
	ratioEA ->Draw("lp");
	ratioEC ->Draw("lp");


}













class ToTDependence: public TCanvas{
public: 
  ToTDependence(TFile*, bool, bool, bool isAr) throw(string);
private:
  TDirectory* errors;
};


ToTDependence::ToTDependence(TFile* file, bool isba = true, bool isht = false, bool isAr = false) throw(string){
  errors = NULL;
  cout <<"READING CORRECTIONS: " << endl;
  if (!isAr)    errors = (TDirectory*) file->FindKey("Correction")->ReadObj();
  else          errors = (TDirectory*) file->FindKey("CorrectionAr")->ReadObj();

  this->Divide(2,3,0.01,0.01);

        // Read the histos:

  	TH2F* tres_vs_ToT_ba = NULL;
  	TH2F* tres_vs_ToT_bc = NULL;
	// Residual:
  	TH2F* res_vs_ToT_ba = NULL;
	TH2F* res_vs_ToT_bc = NULL;


      if(!isAr){
        if (!isht){
                if(isba){
        if(errors->FindKey("tres_vs_ToT_ba"))   tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tres_vs_ToT_ba")->ReadObj();
        if(errors->FindKey("res_vs_ToT_ba"))    res_vs_ToT_ba   =       (TH2F*)errors->FindKey("res_vs_ToT_ba")->ReadObj();
        if(errors->FindKey("tres_vs_ToT_bc"))   tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tres_vs_ToT_bc")->ReadObj();
        if(errors->FindKey("res_vs_ToT_bc"))    res_vs_ToT_bc   =       (TH2F*)errors->FindKey("res_vs_ToT_bc")->ReadObj();
                }
                else{
        if(errors->FindKey("tres_vs_ToT_ea"))   tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tres_vs_ToT_ea")->ReadObj();
        if(errors->FindKey("res_vs_ToT_ea"))    res_vs_ToT_ba   =       (TH2F*)errors->FindKey("res_vs_ToT_ea")->ReadObj();
        if(errors->FindKey("tres_vs_ToT_ec"))   tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tres_vs_ToT_ec")->ReadObj();
        if(errors->FindKey("res_vs_ToT_ec"))    res_vs_ToT_bc   =       (TH2F*)errors->FindKey("res_vs_ToT_ec")->ReadObj();
                }
        }
        else{
                if(isba){
        if(errors->FindKey("tres_vs_HT_ba"))   tres_vs_ToT_ba   =       (TH2F*)errors->FindKey("tres_vs_HT_ba")->ReadObj();
        if(errors->FindKey("res_vs_HT_ba"))    res_vs_ToT_ba    =       (TH2F*)errors->FindKey("res_vs_HT_ba")->ReadObj();
        if(errors->FindKey("tres_vs_HT_bc"))   tres_vs_ToT_bc   =       (TH2F*)errors->FindKey("tres_vs_HT_bc")->ReadObj();
        if(errors->FindKey("res_vs_HT_bc"))    res_vs_ToT_bc    =       (TH2F*)errors->FindKey("res_vs_HT_bc")->ReadObj();
                }
                else{
        if(errors->FindKey("tres_vs_HT_ea"))   tres_vs_ToT_ba   =       (TH2F*)errors->FindKey("tres_vs_HT_ea")->ReadObj();
        if(errors->FindKey("res_vs_HT_ea"))    res_vs_ToT_ba    =       (TH2F*)errors->FindKey("res_vs_HT_ea")->ReadObj();
        if(errors->FindKey("tres_vs_HT_ec"))   tres_vs_ToT_bc   =       (TH2F*)errors->FindKey("tres_vs_HT_ec")->ReadObj();
        if(errors->FindKey("res_vs_HT_ec"))    res_vs_ToT_bc    =       (TH2F*)errors->FindKey("res_vs_HT_ec")->ReadObj();
                }
        }
      }
      else {
        if (!isht){
                if(isba){
        if(errors->FindKey("tresAr_vs_ToT_ba"))  tres_vs_ToT_ba =       (TH2F*)errors->FindKey("tresAr_vs_ToT_ba")->ReadObj();
        if(errors->FindKey("resAr_vs_ToT_ba"))   res_vs_ToT_ba  =       (TH2F*)errors->FindKey("resAr_vs_ToT_ba")->ReadObj();
        if(errors->FindKey("tresAr_vs_ToT_bc"))  tres_vs_ToT_bc =       (TH2F*)errors->FindKey("tresAr_vs_ToT_bc")->ReadObj();
        if(errors->FindKey("resAr_vs_ToT_bc"))   res_vs_ToT_bc  =       (TH2F*)errors->FindKey("resAr_vs_ToT_bc")->ReadObj();
                }
                else{
        if(errors->FindKey("tresAr_vs_ToT_ea"))  tres_vs_ToT_ba =       (TH2F*)errors->FindKey("tresAr_vs_ToT_ea")->ReadObj();
        if(errors->FindKey("resAr_vs_ToT_ea"))   res_vs_ToT_ba  =       (TH2F*)errors->FindKey("resAr_vs_ToT_ea")->ReadObj();
        if(errors->FindKey("tresAr_vs_ToT_ec"))  tres_vs_ToT_bc =       (TH2F*)errors->FindKey("tresAr_vs_ToT_ec")->ReadObj();
        if(errors->FindKey("resAr_vs_ToT_ec"))   res_vs_ToT_bc  =       (TH2F*)errors->FindKey("resAr_vs_ToT_ec")->ReadObj();
                }
        }
        else{
                if(isba){
        if(errors->FindKey("tresAr_vs_HT_ba"))  tres_vs_ToT_ba =        (TH2F*)errors->FindKey("tresAr_vs_HT_ba")->ReadObj();
        if(errors->FindKey("resAr_vs_HT_ba"))   res_vs_ToT_ba   =       (TH2F*)errors->FindKey("resAr_vs_HT_ba")->ReadObj();
        if(errors->FindKey("tresAr_vs_HT_bc"))  tres_vs_ToT_bc =        (TH2F*)errors->FindKey("tresAr_vs_HT_bc")->ReadObj();
        if(errors->FindKey("resAr_vs_HT_bc"))   res_vs_ToT_bc   =       (TH2F*)errors->FindKey("resAr_vs_HT_bc")->ReadObj();
                }
                else{
        if(errors->FindKey("tresAr_vs_HT_ea"))  tres_vs_ToT_ba =        (TH2F*)errors->FindKey("tresAr_vs_HT_ea")->ReadObj();
        if(errors->FindKey("resAr_vs_HT_ea"))   res_vs_ToT_ba   =       (TH2F*)errors->FindKey("resAr_vs_HT_ea")->ReadObj();
        if(errors->FindKey("tresAr_vs_HT_ec"))  tres_vs_ToT_bc =        (TH2F*)errors->FindKey("tresAr_vs_HT_ec")->ReadObj();
        if(errors->FindKey("resAr_vs_HT_ec"))   res_vs_ToT_bc   =       (TH2F*)errors->FindKey("resAr_vs_HT_ec")->ReadObj();
                }
        }
      }





  this->cd(1);
		// Time Residual Mean Vs Pt:
	TGraphErrors * tresBA  = GetMean(tres_vs_ToT_ba);    tresBA->SetLineColor(kRed); tresBA->SetMarkerColor(kRed);  		
	if(isba) 	tres_vs_ToT_ba->SetTitle ("Barrel A Time Residual Mean VS ToT");
	else 	 	tres_vs_ToT_ba->SetTitle ("Endcap A Time Residual Mean VS ToT");
	tres_vs_ToT_ba->GetXaxis()->SetTitle ("ToT (bin)");
	tres_vs_ToT_ba->GetYaxis()->SetTitle ("Time Residual (ns)");

	if(isht){
		if(isba) 	tres_vs_ToT_ba->SetTitle ("Barrel A Time Residual Mean VS HT");
		else 	 	tres_vs_ToT_ba->SetTitle ("Endcap A Time Residual Mean VS HT");
		tres_vs_ToT_ba->GetXaxis()->SetTitle ("HT (bin)");
	}

	
	tres_vs_ToT_ba->Draw("colz");
	tresBA->Draw("same");


  this->cd(3);
	TGraphErrors * tresBC  = GetMean(tres_vs_ToT_bc);    tresBC->SetLineColor(kBlue); tresBC->SetMarkerColor(kBlue);  		
	if(isba) 	tres_vs_ToT_bc->SetTitle ("Barrel C Time Residual Mean VS ToT");
	else 	 	tres_vs_ToT_bc->SetTitle ("Endcap C Time Residual Mean VS ToT");
	tres_vs_ToT_bc->GetXaxis()->SetTitle ("ToT (bin)");
	tres_vs_ToT_bc->GetYaxis()->SetTitle ("Time Residual (ns)");
	if(isht){
		if(isba) 	tres_vs_ToT_bc->SetTitle ("Barrel C Time Residual Mean VS HT");
		else 	 	tres_vs_ToT_bc->SetTitle ("Endcap C Time Residual Mean VS HT");
		tres_vs_ToT_bc->GetXaxis()->SetTitle ("HT (bin)");
	}

	tres_vs_ToT_bc->Draw("colz");
	tresBC->Draw("same");


  this->cd(5);
	        tresBA->SetTitle("Barrel Time Residual Mean vs ToT");
		if (!isba) tresBA->SetTitle("Endcap Time Residual Mean vs ToT");
		tresBA->GetXaxis()->SetTitle("ToT (bin)");
		
		if(isht){
		        tresBA->SetTitle("Barrel Time Residual Mean vs HT");
			if (!isba) tresBA->SetTitle("Endcap Time Residual Mean vs HT");
			tresBA->GetXaxis()->SetTitle("HT");
		}
	tresBA->GetXaxis()->SetRangeUser(-0.5,23.5);
	tresBA->GetYaxis()->SetRangeUser(-10,10);
	if(isht){
		tresBA->GetXaxis()->SetRangeUser(-0.5,1.5);
		tresBA->GetYaxis()->SetRangeUser(-3,3);
	}
	tresBA->Draw("alp");
	tresBC->Draw("same");



  this->cd(2);
	TGraphErrors * resBA1  = GetMean(res_vs_ToT_ba);    resBA1->SetLineColor(kRed);  resBA1->SetMarkerColor(kRed);  		
	if(isba) 	res_vs_ToT_ba->SetTitle ("Barrel A Residual Mean VS ToT");
	else 	 	res_vs_ToT_ba->SetTitle ("Endcap A Residual Mean VS ToT");
	res_vs_ToT_ba->GetXaxis()->SetTitle ("ToT (bin)");
	res_vs_ToT_ba->GetYaxis()->SetTitle ("Residual (mm)");
	if(isht){
		if(isba) 	res_vs_ToT_ba->SetTitle ("Barrel A Residual Mean VS HT");
		else 	 	res_vs_ToT_ba->SetTitle ("Endcap A Residual Mean VS HT");
		res_vs_ToT_ba->GetXaxis()->SetTitle ("HT (bin)");
	}
	res_vs_ToT_ba->Draw("colz");
	resBA1->Draw("same");


  this->cd(4);
	TGraphErrors * resBC1  = GetMean(res_vs_ToT_bc);    resBC1->SetLineColor(kBlue); resBC1->SetMarkerColor(kBlue);  		
	if(isba) 	res_vs_ToT_bc->SetTitle ("Barrel C Residual Mean VS ToT");
	else 	 	res_vs_ToT_bc->SetTitle ("Endcap C Residual Mean VS ToT");
	res_vs_ToT_bc->GetXaxis()->SetTitle ("ToT (bin)");
	res_vs_ToT_bc->GetYaxis()->SetTitle ("Residual (mm)");
	if(isht){
		if(isba) 	res_vs_ToT_bc->SetTitle ("Barrel C Residual Mean VS HT");
		else 	 	res_vs_ToT_bc->SetTitle ("Endcap C Residual Mean VS HT");
		res_vs_ToT_bc->GetXaxis()->SetTitle ("HT (bin)");
	}
	res_vs_ToT_bc->Draw("colz");
	resBC1->Draw("same");



  this->cd(6);

	TGraphErrors * resBA  = GetWidth(res_vs_ToT_ba);    resBA->SetLineColor(kRed);  resBA->SetMarkerColor(kRed);  		
	TGraphErrors * resBC  = GetWidth(res_vs_ToT_bc);    resBC->SetLineColor(kBlue); resBC->SetMarkerColor(kBlue);  		
	        resBA->SetTitle("Barrel Time Residual Width vs ToT");
		if (!isba) resBA->SetTitle("Endcap Time Residual Width vs ToT");
		resBA->GetXaxis()->SetTitle("ToT (bin)");
		
		if(isht){
		        resBA->SetTitle("Barrel Residual Width vs HT");
			if (!isba) resBA->SetTitle("Endcap Residual Width vs HT");
			resBA->GetXaxis()->SetTitle("HT");
		}
	resBA->GetXaxis()->SetRangeUser(-0.5,23.5);
	resBA->GetYaxis()->SetRangeUser(0.08,0.2);
	if(isht){
		resBA->GetXaxis()->SetRangeUser(-0.5,1.5);
		resBA->GetYaxis()->SetRangeUser(0.08,0.2);
	}
	resBA->Draw("alp");
	resBC->Draw("same");

}












class SinDependence: public TCanvas{
public: 
  SinDependence(TFile*, bool, bool, bool) throw(string);
private:
  TDirectory* errors;
};

SinDependence::SinDependence(TFile* file, bool isba = true, bool isht = false, bool isAr = false) throw(string){
  errors = NULL;


  if(!isAr)     errors = (TDirectory*) file->FindKey("Correction")->ReadObj();
  else          errors = (TDirectory*) file->FindKey("CorrectionAr")->ReadObj();
  if (!errors) throw(string("No errors folder!!!!!!!!!!!!!!!"));

  this->Divide(2,3,0.01,0.01);

        // Read the histos:

  	TH2F* tres_vs_ToT_ba = NULL;
  	TH2F* tres_vs_ToT_bc = NULL;
	// Residual:
  	TH2F* res_vs_ToT_ba = NULL;
	TH2F* res_vs_ToT_bc = NULL;

      if(!isAr){
        if (!isht){
                if(isba){ //sin
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tres_vs_SinOverP_bc")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey("res_vs_SinOverP_ba")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tres_vs_SinOverP_bc")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey("res_vs_SinOverP_bc")->ReadObj();
                }
                else{
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tres_vs_SinOverP_ea")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey( "res_vs_SinOverP_ea")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tres_vs_SinOverP_ec")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey( "res_vs_SinOverP_ec")->ReadObj();
                }
        }
        else{
                if(isba){ //cos
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tres_vs_CosOverP_bc")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey("res_vs_CosOverP_ba")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tres_vs_CosOverP_bc")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey("res_vs_CosOverP_bc")->ReadObj();
                }
                else{
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tres_vs_CosOverP_ea")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey( "res_vs_CosOverP_ea")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tres_vs_CosOverP_ec")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey( "res_vs_CosOverP_ec")->ReadObj();
                }
        }
      }
      else {
        if (!isht){
                if(isba){ //sin
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tresAr_vs_SinOverP_bc")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey("resAr_vs_SinOverP_ba")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tresAr_vs_SinOverP_bc")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey("resAr_vs_SinOverP_bc")->ReadObj();
                }
                else{
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tresAr_vs_SinOverP_ea")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey( "resAr_vs_SinOverP_ea")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tresAr_vs_SinOverP_ec")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey( "resAr_vs_SinOverP_ec")->ReadObj();
                }
        }
        else{
                if(isba){ //cos
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tresAr_vs_CosOverP_bc")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey("resAr_vs_CosOverP_ba")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tresAr_vs_CosOverP_bc")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey("resAr_vs_CosOverP_bc")->ReadObj();
                }
                else{
                        tres_vs_ToT_ba  =       (TH2F*)errors->FindKey("tresAr_vs_CosOverP_ea")->ReadObj();
                        res_vs_ToT_ba   =       (TH2F*)errors->FindKey( "resAr_vs_CosOverP_ea")->ReadObj();
                        tres_vs_ToT_bc  =       (TH2F*)errors->FindKey("tresAr_vs_CosOverP_ec")->ReadObj();
                        res_vs_ToT_bc   =       (TH2F*)errors->FindKey( "resAr_vs_CosOverP_ec")->ReadObj();
                }
        }
      }




  this->cd(1);
		// Time Residual Mean Vs Pt:
	TGraphErrors * tresBA  = GetMean(tres_vs_ToT_ba);    tresBA->SetLineColor(kRed); tresBA->SetMarkerColor(kRed);  		
	if(isba) 	tres_vs_ToT_ba->SetTitle ("Barrel A Time Residual Mean VS sin(Theta)/p");
	else 	 	tres_vs_ToT_ba->SetTitle ("Endcap A Time Residual Mean VS sin(Theta)/p");
	tres_vs_ToT_ba->GetXaxis()->SetTitle ("sin(Theta)/p");
	tres_vs_ToT_ba->GetYaxis()->SetTitle ("Time Residual (ns)");

	if(isht){
		if(isba) 	tres_vs_ToT_ba->SetTitle ("Barrel A Time Residual Mean VS cos(Theta)/p");
		else 	 	tres_vs_ToT_ba->SetTitle ("Endcap A Time Residual Mean VS cos(Theta)/p");
		tres_vs_ToT_ba->GetXaxis()->SetTitle ("cos(Theta)/p");
	}

	
	tres_vs_ToT_ba->Draw("colz");
	tresBA->Draw("same");


  this->cd(3);
	TGraphErrors * tresBC  = GetMean(tres_vs_ToT_bc);    tresBC->SetLineColor(kBlue); tresBC->SetMarkerColor(kBlue);  		
	if(isba) 	tres_vs_ToT_bc->SetTitle ("Barrel C Time Residual Mean VS sin(Theta)/p");
	else 	 	tres_vs_ToT_bc->SetTitle ("Endcap C Time Residual Mean VS sin(Theta)/p");
	tres_vs_ToT_bc->GetXaxis()->SetTitle ("sin(Theta)/p");
	tres_vs_ToT_bc->GetYaxis()->SetTitle ("Time Residual (ns)");
	if(isht){
		if(isba) 	tres_vs_ToT_bc->SetTitle ("Barrel C Time Residual Mean VS cos(Theta)/p");
		else 	 	tres_vs_ToT_bc->SetTitle ("Endcap C Time Residual Mean VS cos(Theta)/p");
		tres_vs_ToT_bc->GetXaxis()->SetTitle ("cos(Theta)/p");
	}
	tres_vs_ToT_bc->Draw("colz");
	tresBC->Draw("same");

  this->cd(5);
	        tresBA->SetTitle("Barrel Time Residual Mean vs Sin(theta) / p");
		if (!isba) tresBA->SetTitle("Endcap Time Residual Mean vs Sin(theta) / p");
		
		if(isht){
		        tresBA->SetTitle("Barrel Time Residual Mean vs Cos(theta) / p");
			if (!isba) tresBA->SetTitle("Endcap Time Residual Mean vs Cos(theta) / p");
		}

	tresBA->GetXaxis()->SetRangeUser(0,1);
	tresBA->GetYaxis()->SetRangeUser(-1,1.5);
	tresBA->Draw("alp");
	tresBC->Draw("same");



  this->cd(2);
	TGraphErrors * resBA1  = GetMean(res_vs_ToT_ba);    resBA1->SetLineColor(kRed);  resBA1->SetMarkerColor(kRed);  		
	if(isba) 	res_vs_ToT_ba->SetTitle ("Barrel A Residual Mean VS sin(Theta)/p");
	else 	 	res_vs_ToT_ba->SetTitle ("Endcap A Residual Mean VS sin(Theta)/p");
	res_vs_ToT_ba->GetXaxis()->SetTitle ("sin(Theta)/p");
	res_vs_ToT_ba->GetYaxis()->SetTitle ("Residual (mm)");
	if(isht){
		if(isba) 	res_vs_ToT_ba->SetTitle ("Barrel A Residual Mean VS cos(Theta)/p");
		else 	 	res_vs_ToT_ba->SetTitle ("Endcap A Residual Mean VS cos(Theta)/p");
		res_vs_ToT_ba->GetXaxis()->SetTitle ("cos(Theta)/p");
	}
	res_vs_ToT_ba->Draw("colz");
	resBA1->Draw("same");


  this->cd(4);
	TGraphErrors * resBC1  = GetMean(res_vs_ToT_bc);    resBC1->SetLineColor(kBlue); resBC1->SetMarkerColor(kBlue);  		
	if(isba) 	res_vs_ToT_bc->SetTitle ("Barrel C Residual Mean VS sin(Theta)/p");
	else 	 	res_vs_ToT_bc->SetTitle ("Endcap C Residual Mean VS sin(Theta)/p");
	res_vs_ToT_bc->GetXaxis()->SetTitle ("sin(Theta)/p");
	res_vs_ToT_bc->GetYaxis()->SetTitle ("Residual (mm)");
	if(isht){
		if(isba) 	res_vs_ToT_bc->SetTitle ("Barrel C Residual Mean VS cos(Theta)/p");
		else 	 	res_vs_ToT_bc->SetTitle ("Endcap C Residual Mean VS cos(Theta)/p");
		res_vs_ToT_bc->GetXaxis()->SetTitle ("cos(Theta)/p");
	}
	res_vs_ToT_bc->Draw("colz");
	resBC1->Draw("same");



  this->cd(6);

	TGraphErrors * resBA  = GetWidth(res_vs_ToT_ba);    resBA->SetLineColor(kRed);  resBA->SetMarkerColor(kRed);  		
	TGraphErrors * resBC  = GetWidth(res_vs_ToT_bc);    resBC->SetLineColor(kBlue); resBC->SetMarkerColor(kBlue);  		
	        resBA->SetTitle("Barrel Residual Width vs Sin(theta) / p");
		if (!isba) resBA->SetTitle("Endcap Residual Widht vs Sin(theta) / p");
		
		if(isht){
		        resBA->SetTitle("Barrel Residual Width vs Cos(theta) / p");
			if (!isba) resBA->SetTitle("Endcap Residual Widht vs Cos(theta) / p");
		}

	resBA->GetXaxis()->SetRangeUser(0,1);
	resBA->GetYaxis()->SetRangeUser(0.08,0.20);
	resBA->Draw("alp");
	resBC->Draw("same");

		


}














//================================================================
//================================================================

int itersum(int argc, char* argv[]) {

  TFile* datafile = new TFile(argv[2]);  
  if (datafile->IsZombie()){
    cout << "ERROR! INPUT FILE DOES NOT EXIST!" << endl;
    exit(-1);
  }

  bool isAr = false;
  if (datafile->FindKey("TRT_Ar_all")){
                isAr= true;
                cout <<" This run contains straws with Argon" << endl;
        }



// Here we have to do the fix, because the trees are duplicated


  TFile* refdatafile;
  bool useref=false;
  if (argc>4) {
    useref=true;
    refdatafile = new TFile(argv[4]);
    if (refdatafile->IsZombie()){
      cout << "INPUT REFERENCE FILE DOES NOT EXIST! RUNNING WITHOUT IT!" << endl;
      useref=false;
    }  
    else cout << "INPUT REFERENCE FILE OK!" << endl;
  }

  // First page on Shifter report, general information
  cout << "MAKING FirstPage" << endl; 
  FirstPage* firstpage = NULL;
  try { firstpage = new FirstPage(argv[2],datafile); } catch (string e) { cout << e << endl; }
  
  //Add a page with the settings:
  cout << "MAKING Settings" << endl; 
  SettingsInfo* settings = NULL;
  //if (argv[3]) try { settings = new SettingsInfo(argv[3]); } catch (string e) { cout << e << endl; }

  //Tracktuple info
  cout << "MAKING TracktupleInfo" << endl; 
  TrackTupleInfo* tracktupleinfo = NULL;
  try { tracktupleinfo = new TrackTupleInfo(datafile); } catch (string e) { cout << e << endl; }

  //Hit statistics
  HitStatistics* hitstat=NULL;
  cout << "MAKING HitStatistics" << endl; 
  hitstat= new HitStatistics(argv[2]);

  HitStatistics* hitstatAr=NULL;
  if(isAr){
          cout << "MAKING Ar HitStatistics" << endl;
          hitstatAr= new HitStatistics(argv[2], true);
  }

  //TRT Global Results:
  cout << "MAKING TRTPlots" << endl; 
  TRTPlots* TRTresplots=NULL;
  try { TRTresplots = new TRTPlots(datafile); } catch (string e) { cout << e << endl; }
  TRTPlots* TRTresplotsAr=NULL;
  if(isAr)  try { TRTresplotsAr = new TRTPlots(datafile, true); } catch (string e) { cout << e << endl; }

  //Residual and time residual (TRT and both barrels)
  cout << "MAKING ResidualPlots" << endl; 
  ResidualPlots* resplots=NULL;
  try { resplots = new ResidualPlots(datafile); } catch (string e) { cout << e << endl; }
  ResidualPlots* resplotsAr=NULL;
  if(isAr) try { resplotsAr = new ResidualPlots(datafile, true); } catch (string e) { cout << e << endl; }
 
  //Do t0 calibration type:
  cout << "MAKING T0CalibTypeXY" << endl; 
  T0CalibTypeXY* hitstatxy= new T0CalibTypeXY(argv[2]);
  T0CalibTypeXY* hitstatxyAr = NULL;
  if (isAr)     hitstatxyAr= new T0CalibTypeXY(argv[2], true);

  //Do hits plots:
  cout << "MAKING XYMaps(nt0)" << endl; 
  XYMaps* nhits= new XYMaps(argv[2],"nt0");
  XYMaps* nhitsAr= NULL;
  if (isAr) nhitsAr = new XYMaps(argv[2],"nt0",true);
  //Do res plot:
  cout << "MAKING XYMaps(res)" << endl; 
  XYMaps* resxy0= new XYMaps(argv[2],"res");
  XYMaps* resxy0Ar= NULL;
  if (isAr) resxy0Ar  = new XYMaps(argv[2],"res",true);
  //Do residual center plot:
  cout << "MAKING XYMaps(resmean)" << endl; 
  XYMaps* resxy= new XYMaps(argv[2],"abs(resMean)");
  XYMaps* resxyAr= NULL;
  if (isAr) resxyAr  = new XYMaps(argv[2],"abs(resMean)",true);
  //Do t0s plots:
  cout << "MAKING XYMaps(t0)" << endl; 
  XYMaps* xymap= new XYMaps(argv[2],"t0");
  XYMaps* xymapAr= NULL;
  if (isAr) xymapAr  = new XYMaps(argv[2],"t0",true);
  //Do Oldt0plots:
  cout << "MAKING XYMaps(oldt0)" << endl; 
  XYMaps* oldt0= new XYMaps(argv[2],"oldt0");
  XYMaps* oldt0Ar= NULL;
  if (isAr) oldt0Ar  = new XYMaps(argv[2],"oldt0",true);
  //Do t0 diff:
  cout << "MAKING XYMaps(t0-oldt0)" << endl; 
  XYMaps* oldt01= new XYMaps(argv[2],"abs(t0-oldt0)");
  XYMaps* oldt01Ar= NULL;
  if (isAr) oldt01Ar  = new XYMaps(argv[2],"t0-oldt0",true);
  //Do t0 diff mean:
  cout << "MAKING XYMaps(tresMean)" << endl;
  XYMaps* oldt0111= new XYMaps(argv[2],"abs(tresMean)");
  XYMaps* oldt0111Ar= NULL;
  if (isAr) oldt0111Ar  = new XYMaps(argv[2],"abs(tresMean)",true);
  //Do t0 width:
  cout << "MAKING XYMaps(tres)" << endl; 
  XYMaps* oldt011= new XYMaps(argv[2],"(tres)");
  XYMaps* oldt011Ar= NULL;
  if (isAr) oldt011Ar = new XYMaps(argv[2],"(tres)",true);
  
  //Add the plot for Chip T0 (board offset) variations 
  ChipVariations* chipgraphA=NULL;
  ChipVariations* chipgraphC=NULL;
  if (datafile->FindKey("TRT_all")){
    cout << "MAKING ChipVariations" << endl; 
    chipgraphA= new ChipVariations(argv[2],1);
    chipgraphC= new ChipVariations(argv[2],-1);
  }
  
  //Add the plot for Board T0 variations 
  BoardVariationsOldT0* refboardgraphA=NULL;
  BoardVariationsOldT0* refboardgraphC=NULL;
  BoardVariationsDiff* boarddiffgraphA=NULL;
  BoardVariationsDiff* boarddiffgraphC=NULL;
  if (useref){
    cout << "MAKING BoardVariationsOldT0" << endl; 
    refboardgraphA= new BoardVariationsOldT0(argv[4],1);
    refboardgraphC= new BoardVariationsOldT0(argv[4],-1);
    refboardgraphA->SetLineColor(2);
    refboardgraphC->SetLineColor(2);
    cout << "MAKING BoardVariationsDiff" << endl; 
    boarddiffgraphA= new BoardVariationsDiff(argv[2],argv[4],1);
    boarddiffgraphC= new BoardVariationsDiff(argv[2],argv[4],-1);
  } 
  //Add the plot for Board T0 variations 
  cout << "MAKING BoardVariations" << endl; 
  BoardVariations* boardgraphA= new BoardVariations(argv[2],1);
  BoardVariations* boardgraphC= new BoardVariations(argv[2],-1);
  //Add the plot for the residual
  cout << "MAKING BoardVariationsRes" << endl; 
  BoardVariationsRes* boardgraphA1= new BoardVariationsRes(argv[2],1);
  BoardVariationsRes* boardgraphC1= new BoardVariationsRes(argv[2],-1);
  //Add the plot for the Time residual
  cout << "MAKING BoardVariationsTRes" << endl; 
  BoardVariationsTRes* boardgraphA2= new BoardVariationsTRes(argv[2],1);
  BoardVariationsTRes* boardgraphC2= new BoardVariationsTRes(argv[2],-1);
  //Add the plot for the sigma Time residual
  cout << "MAKING BoardVariationsTRes1" << endl; 
  BoardVariationsTRes1* boardgraphA3= new BoardVariationsTRes1(argv[2],1);
  BoardVariationsTRes1* boardgraphC3= new BoardVariationsTRes1(argv[2],-1);

  //RtGraphs* rtrelation11=NULL;
  RtGraphs* rtrelation_bar=NULL;
  RtGraphs* rtrelation21=NULL;
  RtGraphs* rtrelation31=NULL;
  RtGraphs* rtrelation41=NULL;
  RtGraphs* rtrelation51=NULL;
  RtBinning* rtbinning_bar=NULL;
  RtBinning* rtbinning1=NULL;
  RtBinning* rtbinning2=NULL;
  RtBinning* rtbinning3=NULL;
  RtBinning* rtbinning4=NULL;
  if (datafile->FindKey("TRT_all")){
    cout << "MAKING RtGraphs" << endl;     
    TDirectory* trt = (TDirectory*) datafile->FindKey("TRT_all")->ReadObj();
    //if (trt->FindKey("WholeBarrel_1")){
    // try {rtrelation11= new RtGraphs(argv[2],"WholeBarrel_1");}
    // catch (string e) {cout << e << endl;}  
    //}
    if (trt->FindKey("WholeBarrel_1")){
      try {rtrelation_bar= new RtGraphs(argv[2],"WholeBarrel_1");}
      catch (string e) {cout << e << endl;}  
    }
    if (trt->FindKey("Detector_-1")){
      try {rtrelation21= new RtGraphs(argv[2],"Detector_-1");}
      catch (string e) {cout << e << endl;}  
    }
    if (trt->FindKey("Detector_1")){
      try {rtrelation31= new RtGraphs(argv[2],"Detector_1");}
      catch (string e) {cout << e << endl;}  
    }
    if (trt->FindKey("Detector_-2")){
      try {rtrelation41= new RtGraphs(argv[2],"Detector_-2");}
      catch (string e) {cout << e << endl;}  
    }
    if (trt->FindKey("Detector_2")){
      try {rtrelation51= new RtGraphs(argv[2],"Detector_2");}
      catch (string e) {cout << e << endl;}  
    }
    //r plots for each t-bin
    cout << "MAKING Rt binning" << endl; 
    rtbinning_bar=new RtBinning(trt,"WholeBarrel_1");
    rtbinning1=new RtBinning(trt,"Detector_-1");
    rtbinning2=new RtBinning(trt,"Detector_1");
    rtbinning3=new RtBinning(trt,"Detector_-2");
    rtbinning4=new RtBinning(trt,"Detector_2");
  }

  //Binned residual plots
  RresTbin* binres_bar=NULL;
  RresTbin* binres1=NULL;
  RresTbin* binres2=NULL;
  RresTbin* binres3=NULL;
  RresTbin* binres4=NULL;
  TBinnedRes* tbinnedres_bar=NULL;
  TBinnedRes* tbinnedres1=NULL;
  TBinnedRes* tbinnedres2=NULL;
  TBinnedRes* tbinnedres3=NULL;
  TBinnedRes* tbinnedres4=NULL;
  //if (datafile->FindKey("binhist")){
  cout << "MAKING Binned Residual" << endl; 
  binres_bar=new RresTbin(datafile,"WholeBarrel_1",4);
  tbinnedres_bar=new TBinnedRes(datafile,binres_bar->reshists);
  binres1=new RresTbin(datafile,"Detector_-1",-1);
  tbinnedres1=new TBinnedRes(datafile,binres1->reshists);
  binres2=new RresTbin(datafile,"Detector_1",1);
  tbinnedres2=new TBinnedRes(datafile,binres2->reshists);
  binres3=new RresTbin(datafile,"Detector_-2",-2);
  tbinnedres3=new TBinnedRes(datafile,binres3->reshists);
  binres4=new RresTbin(datafile,"Detector_2",2);
  tbinnedres4=new TBinnedRes(datafile,binres4->reshists);
  //}
  
  RtColor* rtcol=NULL;
  RtColor* rtcol1=NULL;
  RtColor* rtcol2=NULL;
  RtColor* rtcol3=NULL;
  RtColor* rtcol4=NULL;
  RtColor* rtcol5=NULL;
  //Rt relation, default plot
  if (datafile->FindKey("TRT_all")){
    cout << "MAKING RtColor" << endl; 
    rtcol= new RtColor(argv[2],"");
    TDirectory* trt = (TDirectory*) datafile->FindKey("TRT_all")->ReadObj(); 
    if (trt->FindKey("WholeBarrel_1")){
      try {rtcol1= new RtColor(argv[2],"WholeBarrel_1");} 
      catch (string e){cout << e << endl;}
    }
    if (trt->FindKey("Detector_-1")){
      try {rtcol2= new RtColor(argv[2],"Detector_-1");} 
      catch (string e){cout << e << endl;}
    }
    if (trt->FindKey("Detector_1")){
      try {rtcol3= new RtColor(argv[2],"Detector_1");} 
      catch (string e){cout << e << endl;}
    }
    if (trt->FindKey("Detector_-2")){
      try {rtcol4= new RtColor(argv[2],"Detector_-2");} 
      catch (string e){cout << e << endl;}
     }
    if (trt->FindKey("Detector_2")){
      try {rtcol5= new RtColor(argv[2],"Detector_2");} 
      catch (string e){cout << e << endl;}
    }    
  }

                //ARGON!!
  RtGraphs* rtArrelation_bar=NULL;
  RtGraphs* rtArrelation21=NULL;
  RtGraphs* rtArrelation31=NULL;
  RtGraphs* rtArrelation41=NULL;
  RtGraphs* rtArrelation51=NULL;
  RtBinning* rtArbinning_bar=NULL;
  RtBinning* rtArbinning1=NULL;
  RtBinning* rtArbinning2=NULL;
  RtBinning* rtArbinning3=NULL;
  RtBinning* rtArbinning4=NULL;
  if (datafile->FindKey("TRT_Ar_all")){
    cout << "MAKING AR RtGraphs" << endl;
    TDirectory* trtAr = (TDirectory*) datafile->FindKey("TRT_Ar_all")->ReadObj();
    cout << "MAKING AR RtGraphs0" << endl;
    if (trtAr->FindKey("WholeBarrel_Ar_1")){
      TDirectory* detAr = (TDirectory*) trtAr->FindKey("WholeBarrel_Ar_1")->ReadObj();
        if(detAr->FindKey("binhist")){
      rtArbinning_bar   =new RtBinning(trtAr,"WholeBarrel_Ar_1");
      try {rtArrelation_bar= new RtGraphs(argv[2],"WholeBarrel_Ar_1", true);}
      catch (string e) {cout << e << endl;}
        }
    }
    cout << "MAKING AR RtGraphs" << endl;
    if (trtAr->FindKey("Detector_Ar_-1")){
      rtArbinning1      =new RtBinning(trtAr,"Detector_Ar_-1");
      try {rtArrelation21= new RtGraphs(argv[2],"Detector_Ar_-1", true);}
      catch (string e) {cout << e << endl;}
    }
    cout << "MAKING AR RtGraphs" << endl;
    if (trtAr->FindKey("Detector_Ar_1")){
      rtArbinning2      =new RtBinning(trtAr,"Detector_Ar_1");
      try {rtArrelation31= new RtGraphs(argv[2],"Detector_Ar_1", true);}
      catch (string e) {cout << e << endl;}
    }
    cout << "MAKING AR RtGraphs" << endl;
    if (trtAr->FindKey("Detector_Ar_-2")){
	cout << "trying trtArgon:!!! " << endl;
      rtArbinning3      =new RtBinning(trtAr,"Detector_Ar_-2");
      try {rtArrelation41= new RtGraphs(argv[2],"Detector_Ar_-2", true);}
      catch (string e) {cout << e << endl;}
    }
    if (trtAr->FindKey("Detector_Ar_2")){

  	TDirectory* test = (TDirectory*)(((TDirectory*)trtAr->Get("Detector_Ar_2")));
	if(test->FindKey("binhist")){
      	      rtArbinning4      =new RtBinning(trtAr,"Detector_Ar_2");
	      try {rtArrelation51= new RtGraphs(argv[2],"Detector_Ar_2", true);}
	      catch (string e) {cout << e << endl;}
	}
    }
    //r plots for each t-bin
    cout << "MAKING AR Rt binning" << endl;
  }


  //Binned residual plots
  RresTbin* binresAr_bar_bar=NULL;
  RresTbin* binresAr_bar1=NULL;
  RresTbin* binresAr_bar2=NULL;
  RresTbin* binresAr_bar3=NULL;
  RresTbin* binresAr_bar4=NULL;
  TBinnedRes* tbinnedArres_bar=NULL;
  TBinnedRes* tbinnedArres1=NULL;
  TBinnedRes* tbinnedArres2=NULL;
  TBinnedRes* tbinnedArres3=NULL;
  TBinnedRes* tbinnedArres4=NULL;

   cout << "MAKING RT Binned Residual" << endl;
/*
  TDirectory* trtAr = (TDirectory*) datafile->FindKey("TRT_Ar_all")->ReadObj();
  if (trtAr->FindKey("WholeBarrel_Ar_1"))       binresAr_bar_bar        =       new RresTbin(datafile,"WholeBarrel_Ar_1",4, true);
  if (trtAr->FindKey("Detector_Ar_-1"))         binresAr_bar1           =       new RresTbin(datafile,"Detector_Ar_-1",-1, true);
  if (trtAr->FindKey("Detector_Ar_1"))          binresAr_bar2           =       new RresTbin(datafile,"Detector_Ar_1",1,true);
  if (trtAr->FindKey("Detector_Ar_-2"))         binresAr_bar3           =       new RresTbin(datafile,"Detector_Ar_-2",-2,true);
  if (trtAr->FindKey("Detector_Ar_2"))          binresAr_bar4           =       new RresTbin(datafile,"Detector_Ar_2",2,true);
*/
  cout << "MAKING RT Binned Abs Residual" << endl;
/*
  tbinnedArres_bar      =       new TBinnedRes(datafile,binresAr_bar_bar->reshists);
  tbinnedArres1         =       new TBinnedRes(datafile,binresAr_bar1->reshists);
  tbinnedArres2         =       new TBinnedRes(datafile,binresAr_bar2->reshists);
  tbinnedArres3         =       new TBinnedRes(datafile,binresAr_bar3->reshists);
  tbinnedArres4         =       new TBinnedRes(datafile,binresAr_bar4->reshists);
*/

  RtColor* rtArcol=NULL;
  RtColor* rtArcol1=NULL;
  RtColor* rtArcol2=NULL;
  RtColor* rtArcol3=NULL;
  RtColor* rtArcol4=NULL;
  RtColor* rtArcol5=NULL;
  //Rt relation, default plot
  if (datafile->FindKey("TRT_Ar_all")){
    cout << "MAKING AR RtColor" << endl;
    rtArcol= new RtColor(argv[2],"", true);
    TDirectory* trtAr = (TDirectory*) datafile->FindKey("TRT_Ar_all")->ReadObj();
    if (trtAr->FindKey("WholeBarrel_Ar_1")){
      cout << "MAKING AR RtColor whole barrel" << endl;
      try {rtArcol1= new RtColor(argv[2],"WholeBarrel_Ar_1", true);}
      catch (string e){cout << e << endl;}
    }
    if (trtAr->FindKey("Detector_Ar_-1")){
      cout << "MAKING AR RtColor det -1" << endl;
      try {rtArcol2= new RtColor(argv[2],"Detector_Ar_-1", true);}
      catch (string e){cout << e << endl;}
    }
    if (trtAr->FindKey("Detector_Ar_1")){
      cout << "MAKING AR RtColor det 1" << endl;
      try {rtArcol3= new RtColor(argv[2],"Detector_Ar_1", true);}
      catch (string e){cout << e << endl;}
    }
    if (trtAr->FindKey("Detector_Ar_-2")){
      cout << "MAKING AR RtColor det -2" << endl;
      try {rtArcol4= new RtColor(argv[2],"Detector_Ar_-2", true);}
      catch (string e){cout << e << endl;}
     }
    if (trtAr->FindKey("Detector_Ar_2")){
      cout << "MAKING AR RtColor det 2" << endl;
      try {rtArcol5= new RtColor(argv[2],"Detector_Ar_2", true);}
      catch (string e){cout << e << endl;}
    }
  }







  
  
  TCanvas* textpage;

  TCanvas* c1= new TCanvas();
  c1->Print("itersum.ps[");
  c1->cd();

  if (firstpage) firstpage->Print("itersum.ps"); c1->Clear();
  if (settings) settings->Print("itersum.ps"); c1->Clear();  
  if (tracktupleinfo) tracktupleinfo->Print("itersum.ps"); c1->Clear();
  if (hitstat) hitstat->Print("itersum.ps");  c1->Clear();
  if (hitstatAr) hitstatAr->Print("itersum.ps");  c1->Clear();

//  textpage = new TextPage("RESIDUAL PLOTS"); textpage->Print("itersum.ps"); c1->Clear();

  gStyle->SetOptStat(1);
 // if (TRTresplots) TRTresplots->Print("itersum.ps");  c1->Clear();
 // if (TRTresplotsAr) TRTresplotsAr->Print("itersum.ps");  c1->Clear();
  gStyle->SetOptStat(0);
  if (resplots)   resplots->Print("itersum.ps");  c1->Clear();  
  if (resplotsAr) resplotsAr->Print("itersum.ps");  c1->Clear();
  
  textpage = new TextPage(" BARREL PLOTS "); textpage->Print("itersum.ps"); c1->Clear();
  
  hitstatxy->Print("itersum.ps");  c1->Clear();
  if(isAr)      hitstatxyAr->Print("itersum.ps");  c1->Clear();
  nhits->Print("itersum.ps");  c1->Clear();
  if(isAr)  nhitsAr->Print("itersum.ps");  c1->Clear();
  resxy0->Print("itersum.ps");  c1->Clear();
  if(isAr)  resxy0Ar->Print("itersum.ps");  c1->Clear();
  resxy->Print("itersum.ps");  c1->Clear();
  if(isAr)  resxyAr->Print("itersum.ps");  c1->Clear();
  xymap->Print("itersum.ps");  c1->Clear();
  if(isAr)  xymapAr->Print("itersum.ps");  c1->Clear();
if(do_expert){
	  oldt0->Print("itersum.ps");  c1->Clear();
	  if(isAr)  oldt0Ar->Print("itersum.ps");  c1->Clear();
	}
  oldt01->Print("itersum.ps");  c1->Clear();
  if(isAr)  oldt01Ar->Print("itersum.ps");  c1->Clear();
if(do_expert){
	  oldt0111->Print("itersum.ps");  c1->Clear();
	  if(isAr)  oldt0111Ar->Print("itersum.ps");  c1->Clear();
	}
  oldt011->Print("itersum.ps");  c1->Clear();
  if(isAr)  oldt011Ar->Print("itersum.ps");  c1->Clear(); 
 
//  textpage = new TextPage(" T0 PLOTS "); textpage->Print("itersum.ps"); c1->Clear();
  
  double lowe=-3.5,upe=3.5;
if(do_expert)  if (chipgraphA && chipgraphC){
    chipgraphA->GetYaxis()->SetRangeUser(lowe,upe);
    chipgraphC->GetYaxis()->SetRangeUser(lowe,upe);
    c1->cd();
    c1->Divide(1,2,0.01,0.01);
    c1->cd(1);
    chipgraphA->Draw(); chipgraphA->DrawLines(lowe,upe);
    c1->cd(2);
    chipgraphC->Draw(); chipgraphC->DrawLines(lowe,upe);
    c1->Print("itersum.ps");
    c1->Clear();
  }

  c1->cd();
  c1->Divide(1,2,0.01,0.01);
  c1->cd(1);
  boardgraphA->Draw("apl");
  boardgraphA->DrawLines();
  if (refboardgraphA) refboardgraphA->Draw("pl");
  TLegend* leg = new TLegend(0.8,0.8,0.98,0.95);
  leg->AddEntry(boardgraphA,"data","l");
  if (refboardgraphA) leg->AddEntry(refboardgraphA,"ref (input to 1st it)","l");
  leg->SetTextSize(0.03);
  leg->Draw();    
  c1->cd(2);
  boardgraphC->Draw("apl");
  boardgraphC->DrawLines();
  if (refboardgraphC) refboardgraphC->Draw("pl");
  c1->Print("itersum.ps");
  c1->Clear();

  //Add the plot for Board T0 variations differenses 
  if (useref){
    c1->cd();
    c1->Divide(1,2,0.01,0.01);
    c1->cd(1);
    boarddiffgraphA->Draw("ap");
    boarddiffgraphA->DrawLines();
    //TLegend* leg = new TLegend(0.8,0.8,0.98,0.95);
    //leg->AddEntry(boardgraphA,"data","l");
    //if (refboardgraphA) leg->AddEntry(refboardgraphA,"input to 1st it","l");
    //leg->SetTextSize(0.03);
    //leg->Draw();    
    c1->cd(2);
    boarddiffgraphC->Draw("ap");
    boarddiffgraphC->DrawLines();
if(do_expert)    c1->Print("itersum.ps");
    c1->Clear();
  }

  c1->cd();
  c1->Divide(1,2,0.01,0.01);
  c1->cd(1);
  boardgraphA1->Draw("apl");
  boardgraphA1->DrawLines();
  c1->cd(2);
  boardgraphC1->Draw("apl");
  boardgraphC1->DrawLines();
  c1->Print("itersum.ps");
  c1->Clear();

  c1->cd();
  c1->Divide(1,2,0.01,0.01);
  c1->cd(1);
  boardgraphA2->Draw("apl");
  boardgraphA2->DrawLines();
  c1->cd(2);
  boardgraphC2->Draw("apl");
  boardgraphC2->DrawLines();
  c1->Print("itersum.ps");
  c1->Clear();

  c1->cd();
  c1->Divide(1,2,0.01,0.01);
  c1->cd(1);
  boardgraphA3->Draw("apl");
  boardgraphA3->DrawLines();
  c1->cd(2);
  boardgraphC3->Draw("apl");
  boardgraphC3->DrawLines();
  c1->Print("itersum.ps");
  c1->Clear();

  
  
//################################################################                   ADD PLOTS FOR ENDCAPS:
  
  c1->Clear();
  textpage = new TextPage("ENDCAP PLOTS");
  textpage->Print("itersum.ps");
  c1->Clear();	
  
  cout << "MAKING PLOTS FOR ENDCAPS" << endl;
	
//Do hits fit type:
  XYMapsEC* ntypeEC= new XYMapsEC(argv[2],"ftype");
  ntypeEC->Print("itersum.ps");
  XYMapsEC* ntypeECAr= NULL;
  if(isAr)      {
                ntypeECAr = new XYMapsEC(argv[2],"ftype", true);
                ntypeECAr->Print("itersum.ps");
  }

//Do hits plots:
  XYMapsEC* nhitsEC= new XYMapsEC(argv[2],"nt0");
  nhitsEC->Print("itersum.ps");
  XYMapsEC* nhitsECAr= NULL;
  if(isAr)      {
                nhitsECAr = new XYMapsEC(argv[2],"nt0", true);
                nhitsECAr->Print("itersum.ps");
  }
//Do res plot:
  XYMapsEC* resxy0EC = new XYMapsEC(argv[2],"res");
  resxy0EC->Print("itersum.ps");
  XYMapsEC* resxy0ECAr= NULL;
  if(isAr)      {
                resxy0ECAr = new XYMapsEC(argv[2],"res", true);
                resxy0ECAr->Print("itersum.ps");
  }

  XYMapsEC* resxyM0EC = new XYMapsEC(argv[2],"abs(resMean)");
  resxyM0EC->Print("itersum.ps");
  XYMapsEC* resxyM0ECAr= NULL;
  if(isAr)      {
                resxyM0ECAr = new XYMapsEC(argv[2],"abs(resMean)", true);
                resxyM0ECAr->Print("itersum.ps");
  }

//Do t0s plots:
  XYMapsEC* xymapEC= new XYMapsEC(argv[2],"t0");
  xymapEC->Print("itersum.ps");
  XYMapsEC* xymapECAr= NULL;
  if(isAr)      {
                xymapECAr = new XYMapsEC(argv[2],"t0", true);
                xymapECAr->Print("itersum.ps");
  }

//Do Oldt0plots
XYMapsEC* oldt0EC = NULL;
if(do_expert)   oldt0EC= new XYMapsEC(argv[2],"oldt0");
if(do_expert)   oldt0EC->Print("itersum.ps");
  XYMapsEC* oldt0ECAr= NULL;
if(do_expert) {
  if(isAr)      {
                oldt0ECAr = new XYMapsEC(argv[2],"oldt0", true);
                oldt0ECAr->Print("itersum.ps");
  }
}

//Do t0 diff:
  XYMapsEC* oldt01EC= new XYMapsEC(argv[2],"abs(t0-oldt0)");
  oldt01EC->Print("itersum.ps");
  XYMapsEC* oldt01ECAr= NULL;
  if(isAr)      {
                oldt01ECAr = new XYMapsEC(argv[2],"abs(t0-oldt0)", true);
                oldt01ECAr->Print("itersum.ps");
  }

//Do t0 widght:

XYMapsEC* oldt0111EC= NULL;
if(do_expert)  oldt0111EC= new XYMapsEC(argv[2],"abs(tresMean)");
if(do_expert)  oldt0111EC->Print("itersum.ps");
  XYMapsEC* oldt0111ECAr= NULL;
if(do_expert){
  if(isAr)      {
                oldt0111ECAr = new XYMapsEC(argv[2],"abs(tresMean)", true);
                oldt0111ECAr->Print("itersum.ps");
  }
}
//Do t0 widght:
  XYMapsEC* oldt011EC= new XYMapsEC(argv[2],"tres");
  oldt011EC->Print("itersum.ps");
  XYMapsEC* oldt011ECAr= NULL;
  if(isAr)      {
                oldt011ECAr = new XYMapsEC(argv[2],"tres", true);
                oldt011ECAr->Print("itersum.ps");
  }

   // ###################################      RT PLOTS: ########################################################


  textpage = new TextPage(" RT PLOTS "); textpage->Print("itersum.ps"); c1->Clear();	

  //rtrelation11->Print("itersum.ps");
  //c1->Clear();




  gStyle->SetOptStat(1);
  gStyle->SetOptFit(0);
if(do_expert)    if (rtrelation_bar) 	rtrelation_bar->Print("itersum.ps"); 	c1->Clear();
if(do_expert)    if (rtbinning_bar) 	rtbinning_bar->Print("itersum.ps"); 	c1->Clear();    
if(do_expert)    if (rtrelation21) 	rtrelation21->Print("itersum.ps"); 	c1->Clear();
if(do_expert)    if (rtbinning1) 	rtbinning1->Print("itersum.ps"); 	c1->Clear();    
if(do_expert)    if (rtrelation31) 	rtrelation31->Print("itersum.ps"); 	c1->Clear();
if(do_expert)    if (rtbinning2) 	rtbinning2->Print("itersum.ps"); 	c1->Clear();    
if(do_expert)    if (rtrelation41) 	rtrelation41->Print("itersum.ps"); 	c1->Clear();
if(do_expert)    if (rtbinning3) 	rtbinning3->Print("itersum.ps"); 	c1->Clear();    
if(do_expert)    if (rtrelation51) 	rtrelation51->Print("itersum.ps"); 	c1->Clear();
if(do_expert)    if (rtbinning4) 	rtbinning4->Print("itersum.ps"); 	c1->Clear();    
 
if(do_expert)  if (binres_bar) binres_bar->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (tbinnedres_bar) tbinnedres_bar->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (binres1) binres1->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (tbinnedres1) tbinnedres1->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (binres2) binres2->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (tbinnedres2) tbinnedres2->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (binres3) binres3->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (tbinnedres3) tbinnedres3->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (binres4) binres4->Print("itersum.ps");  c1->Clear();
if(do_expert)  if (tbinnedres4) tbinnedres4->Print("itersum.ps");  c1->Clear();


 
  gStyle->SetOptStat(0);
if(do_expert)  if (rtcol) rtcol->Print("itersum.ps"); c1->Clear();
if(do_expert)  if (rtcol1) rtcol1->Print("itersum.ps"); c1->Clear();
  if (rtcol2) rtcol2->Print("itersum.ps"); c1->Clear();
	if (isAr && rtArcol2 ) rtArcol2->Print("itersum.ps"); c1->Clear(); 
  if (rtcol3) rtcol3->Print("itersum.ps"); c1->Clear();
	if (isAr && rtArcol3 ) rtArcol3->Print("itersum.ps"); c1->Clear(); 
  if (rtcol4) rtcol4->Print("itersum.ps"); c1->Clear();
	if (isAr && rtArcol4 ) rtArcol4->Print("itersum.ps"); c1->Clear(); 
  if (rtcol5) rtcol5->Print("itersum.ps"); c1->Clear();
	if (isAr && rtArcol5 ) rtArcol5->Print("itersum.ps"); c1->Clear(); 
  
  // Plot all RTs together....
  
  DGraph* dgraph0=NULL; //trt
  DGraph* dgraph1=NULL; // -1
  DGraph* dgraph2=NULL; // 1
  DGraph* dgraph3=NULL; // -2
  DGraph* dgraph4=NULL; // 2
  DGraph* dgraph5=NULL; // whole barrel
  bool isinverted=false;
  if (datafile->FindKey("TRT_all")){
    TDirectory* trt = (TDirectory*) datafile->FindKey("TRT_all")->ReadObj();
    cout << "MAKING DGraphs" << endl; 
    c1->cd();
    c1->SetGrid();
    dgraph0 = new DGraph(argv[2],"-2,-1","",-2,-1,isinverted);
    if(trt->FindKey("WholeBarrel_1"))        dgraph5 = new DGraph(argv[2],"-2,-1","WholeBarrel_1",-1,-1,isinverted);
    else {
      if(trt->FindKey("Detector_1"))        dgraph1 = new DGraph(argv[2],"-2,-1","Detector_1",1,-1,isinverted);
      if(trt->FindKey("Detector_-1"))        dgraph2 = new DGraph(argv[2],"-2,-1","Detector_-1",1,-1,isinverted);
    }
    if(trt->FindKey("Detector_2"))        dgraph3 = new DGraph(argv[2],"-2,-1","Detector_2",1,-1,isinverted);
    if(trt->FindKey("Detector_-2"))        dgraph4 = new DGraph(argv[2],"-2,-1","Detector_-2",1,-1,isinverted);
    
    dgraph0->GetXaxis()->SetRangeUser(-5,45);
    TLegend* leg = new TLegend(0.8,0.8,0.98,0.95);
    if (dgraph0!=NULL){
      dgraph0->SetLineWidth(3);
      dgraph0->SetLineColor(1);
      if (isinverted) dgraph0->SetTitle("r(t)");
      else dgraph0->SetTitle("r(t)");
      dgraph0->GetXaxis()->SetTitle("t/ns");
      if (isinverted) dgraph0->GetYaxis()->SetTitle("R / (mm)");
      else dgraph0->GetYaxis()->SetTitle("R / (mm)");
      if (dgraph0->rtgraph) dgraph0->Draw("apl");
      if (dgraph0->rtgraph) leg->AddEntry(dgraph0,"Whole TRT","l");
    }
    if (dgraph1!=NULL){
      dgraph1->SetLineWidth(3);
      dgraph1->SetLineColor(3);
      if (dgraph1->rtgraph) dgraph1->Draw("pl");
      if (dgraph1->rtgraph) leg->AddEntry(dgraph1,"Barrel C Side","l");
    }
    
    if (dgraph2!=NULL){
      dgraph2->SetLineWidth(3);
      dgraph2->SetLineColor(2);
      if (dgraph2->rtgraph) dgraph2->Draw("pl");
      if (dgraph2->rtgraph) leg->AddEntry(dgraph2,"Barrel A Side","l");
    }
    
    if (dgraph3!=NULL){
      dgraph3->SetLineWidth(3);
      dgraph3->SetLineColor(6);
      if (dgraph3->rtgraph) dgraph3->Draw("pl");
      if (dgraph3->rtgraph) leg->AddEntry(dgraph3,"Endcap C side","l");
    }
    
    if (dgraph4!=NULL){
      dgraph4->SetLineWidth(3);
      dgraph4->SetLineColor(7);
      if (dgraph4->rtgraph) dgraph4->Draw("pl");
      if (dgraph4->rtgraph) leg->AddEntry(dgraph4,"Endcap A side","l");
    }
    if (dgraph5!=NULL){
      dgraph5->SetLineWidth(3);
      dgraph5->SetLineColor(8);
      if (dgraph5->rtgraph) dgraph5->Draw("pl");
      if (dgraph5->rtgraph) leg->AddEntry(dgraph5,"Whole Barrel","l");
    }
    
    leg->SetTextSize(0.03);
    leg->Draw();    
    c1->Print("itersum.ps");
    c1->Clear();
  }
  
  
  //Plot drift velocity:
  DvGraph* dvgraph0=NULL; //trt
  DvGraph* dvgraph1=NULL; // -1
  DvGraph* dvgraph2=NULL; // 1
  DvGraph* dvgraph3=NULL; // -2
  DvGraph* dvgraph4=NULL; // 2
  DvGraph* dvgraph5=NULL; // whole barrel
 if(do_expert){
  //  bool isinverted=false;
  if (datafile->FindKey("TRT_all")){
    TDirectory* trt = (TDirectory*) datafile->FindKey("TRT_all")->ReadObj();
    cout << "MAKING DvGraphs" << endl; 
    c1->cd();
    c1->SetGrid();
    dvgraph0 = new DvGraph(argv[2],"-2,-1","",-2,-1,isinverted);
    if(trt->FindKey("WholeBarrel_1"))        dvgraph5 = new DvGraph(argv[2],"-2,-1","WholeBarrel_1",-1,-1,isinverted);
    else {
      if(trt->FindKey("Detector_1"))        dvgraph1 = new DvGraph(argv[2],"-2,-1","Detector_1",1,-1,isinverted);
      if(trt->FindKey("Detector_-1"))        dvgraph2 = new DvGraph(argv[2],"-2,-1","Detector_-1",1,-1,isinverted);
    }
    if(trt->FindKey("Detector_2"))        dvgraph3 = new DvGraph(argv[2],"-2,-1","Detector_2",1,-1,isinverted);
    if(trt->FindKey("Detector_-2"))        dvgraph4 = new DvGraph(argv[2],"-2,-1","Detector_-2",1,-1,isinverted);
    
    dvgraph0->GetXaxis()->SetRangeUser(0,2);
    TLegend* leg = new TLegend(0.8,0.8,0.98,0.95);
    if (dvgraph0!=NULL){
      dvgraph0->SetLineWidth(3);
      dvgraph0->SetLineColor(1);
      if (isinverted) dvgraph0->SetTitle("dt/dr");
      else dvgraph0->SetTitle("dr/dt");
      dvgraph0->GetXaxis()->SetTitle("r/mm");
      if (isinverted) dvgraph0->GetYaxis()->SetTitle("dt/dr / (ns/mm)");
      else dvgraph0->GetYaxis()->SetTitle("dr/dt / (mm/ns)");
      if (dvgraph0->rtgraph) dvgraph0->Draw("apl");
      if (dvgraph0->rtgraph) leg->AddEntry(dvgraph0,"Whole TRT","l");
    }
    if (dvgraph1!=NULL){
      dvgraph1->SetLineWidth(3);
      dvgraph1->SetLineColor(3);
      if (dvgraph1->rtgraph) dvgraph1->Draw("pl");
      if (dvgraph1->rtgraph) leg->AddEntry(dvgraph1,"Barrel C Side","l");
    }
    
    if (dvgraph2!=NULL){
      dvgraph2->SetLineWidth(3);
      dvgraph2->SetLineColor(2);
      if (dvgraph2->rtgraph) dvgraph2->Draw("pl");
      if (dvgraph2->rtgraph) leg->AddEntry(dvgraph2,"Barrel A Side","l");
    }
    
    if (dvgraph3!=NULL){
      dvgraph3->SetLineWidth(3);
      dvgraph3->SetLineColor(6);
      if (dvgraph3->rtgraph) dvgraph3->Draw("pl");
      if (dvgraph3->rtgraph) leg->AddEntry(dvgraph3,"Endcap C side","l");
    }
    
    if (dvgraph4!=NULL){
      dvgraph4->SetLineWidth(3);
      dvgraph4->SetLineColor(7);
      if (dvgraph4->rtgraph) dvgraph4->Draw("pl");
      if (dvgraph4->rtgraph) leg->AddEntry(dvgraph4,"Endcap A side","l");
    }
    if (dvgraph5!=NULL){
      dvgraph5->SetLineWidth(3);
      dvgraph5->SetLineColor(8);
      if (dvgraph5->rtgraph) dvgraph5->Draw("pl");
      if (dvgraph5->rtgraph) leg->AddEntry(dvgraph5,"Whole Barrel","l");
    }
    
    leg->SetTextSize(0.03);
    leg->Draw();    
    c1->Print("itersum.ps");
    c1->Clear();
  }
 }











  // ###################################   ADD PLOTS FOR TRACKING   #######################################33
  c1->Clear();
  textpage = new TextPage("Track plots: Only for Experts!!!");
  textpage->Print("itersum.ps");
  c1->Clear();	

  bool istracktuple = false;
  TNtuple* Tracktuple=NULL  ;
  if (datafile->FindKey("tracktuple")) {
  	Tracktuple    = (TNtuple*)datafile->Get("tracktuple") ;
	istracktuple=true;
  }

  
  cout << "MAKING TRACK PLOTS" << endl;
  c1->Clear();
  if (istracktuple){
    //first Residuals:
    if( Tracktuple->GetListOfBranches()->FindObject("trackres") ){
      c1->cd();
      c1->Divide(2,1,0.01,0.01);
      c1->cd(1);
      Tracktuple->Draw("trackres>>histRes(100,0,1.5)");
      TH1F *histRes = (TH1F*)gPad->GetPrimitive("histRes");
      histRes->SetTitle("Residual on track");
      histRes->GetXaxis()->SetTitle("Residual (mm)");
      histRes->Draw();
      
      c1->cd(2);
      Tracktuple->Draw("trackres:t-ttrack-t0>>histRes2d(100,-12,12,100,0,1)","","colz");
      TH2F *histRes2d = (TH2F*)gPad->GetPrimitive("histRes2d");
      histRes2d->SetTitle("Track time residual VS track residual");
      histRes2d->GetXaxis()->SetTitle("Time residual (ns)");
      histRes2d->GetYaxis()->SetTitle("Track Residual (mm)");
      histRes2d->Draw("colz");
      TProfile* res2d = histRes2d->ProfileX();
      res2d->Draw("same");
      
      c1->Print("itersum.ps");
      c1->Clear();
    }
    
    //first pt:
    if( Tracktuple->GetListOfBranches()->FindObject("pt") ){
      c1->cd();
      c1->Divide(2,1,0.01,0.01);
      c1->cd(1);
      Tracktuple->Draw("pt/1000>>histPt(100,0,20)");
      TH1F *histPt = (TH1F*)gPad->GetPrimitive("histPt");
      histPt->SetTitle("Transverse momentum on track");
      histPt->GetXaxis()->SetTitle("Pt (GeV)");
      histPt->Draw();
      
      c1->cd(2);
      Tracktuple->Draw("trackres:pt/1000>>histPt2d(100,0,20,100,0,1.5)","","colz");
      TH2F *histPt2d = (TH2F*)gPad->GetPrimitive("histPt2d");
      histPt2d->SetTitle("Transverse momentum on track VS track residual");
      histPt2d->GetXaxis()->SetTitle("Pt (GeV)");
      histPt2d->GetYaxis()->SetTitle("Track Residual (mm)");
      histPt2d->Draw("COLZ");
      TProfile* pt2d = histPt2d->ProfileX();
      pt2d->Draw("same");
      c1->Print("itersum.ps");
      c1->Clear();
    }
    //first theta:
    if( Tracktuple->GetListOfBranches()->FindObject("theta") ){
      c1->cd();
      c1->Divide(2,1,0.01,0.01);
      c1->cd(1);
      Tracktuple->Draw("theta>>histTheta(100,0,4)");
      TH1F *histTheta = (TH1F*)gPad->GetPrimitive("histTheta");
      histTheta->SetTitle("Theta distribution on track");
      histTheta->GetXaxis()->SetTitle("Theta (rad)");
      histTheta->Draw();
      
      c1->cd(2);
      Tracktuple->Draw("trackres:theta>>histTheta2d(100,0,4,100,0,1.5)","","colz");
      TH2F *histTheta2d = (TH2F*)gPad->GetPrimitive("histTheta2d");
      histTheta2d->SetTitle("Theta distrib. on track VS track residual");
      histTheta2d->GetXaxis()->SetTitle("Theta (rad)");
      histTheta2d->GetYaxis()->SetTitle("Track Residual (mm)");
      histTheta2d->Draw("COLZ");
      TProfile* theta2d = histTheta2d->ProfileX();
      theta2d->Draw("same");
      c1->Print("itersum.ps");
      c1->Clear();
    }
    //first phi:
    if( Tracktuple->GetListOfBranches()->FindObject("phi") ){
      c1->cd();
      c1->Divide(2,1,0.01,0.01);
      c1->cd(1);
      Tracktuple->Draw("phi>>histPhi(100,-3.5,0)");
      TH1F *histPhi = (TH1F*)gPad->GetPrimitive("histPhi");
      histPhi->SetTitle("Phi distribution on track");
      histPhi->GetXaxis()->SetTitle("Phi (rad)");
      histPhi->Draw();
      
      c1->cd(2);
      Tracktuple->Draw("trackres:phi>>histPhi2d(100,-3.5,0,100,0,1.5)","","colz");
      TH2F *histPhi2d = (TH2F*)gPad->GetPrimitive("histPhi2d");
      histPhi2d->SetTitle("Phi distrib. on track VS track residual");
      histPhi2d->GetXaxis()->SetTitle("Phi (rad)");
      histPhi2d->GetYaxis()->SetTitle("Track Residual (mm)");
      histPhi2d->Draw("COLZ");
      TProfile* phi2d = histPhi2d->ProfileX();
      phi2d->Draw("same");
      c1->Print("itersum.ps");
      c1->Clear();
    }
    //first d0:
    if( Tracktuple->GetListOfBranches()->FindObject("d0") ){
      c1->cd();
      c1->Divide(2,1,0.01,0.01);
      c1->cd(1);
      Tracktuple->Draw("d0>>histD0(100,-5,5)");
      TH1F *histD0 = (TH1F*)gPad->GetPrimitive("histD0");
      histD0->SetTitle("D0 distribution on track");
      histD0->GetXaxis()->SetTitle("D0 (mm)");
      histD0->Draw();
      
      c1->cd(2);
      Tracktuple->Draw("trackres:d0>>histD02d(100,-5,5,100,0,1.5)","","colz");
      TH2F *histD02d = (TH2F*)gPad->GetPrimitive("histD02d");
      histD02d->SetTitle("D0 distrib. on track VS track residual");
      histD02d->GetXaxis()->SetTitle("D0 (rad)");
      histD02d->GetYaxis()->SetTitle("Track Residual (mm)");
      histD02d->Draw("COLZ");
      TProfile* d02d = histD02d->ProfileX();
      d02d->Draw("same");
      c1->Print("itersum.ps");
      c1->Clear();
    }
    
  }

  // ###################################   ADD PLOTS FOR Pulls and Errors   #######################################33

  bool iserrors = false;
  TDirectory* errors=NULL  ;
  if (datafile->FindKey("Errors")) {
	errors = (TDirectory*) datafile->FindKey("Errors")->ReadObj();
	iserrors = true;
  }

  c1->Clear();
  textpage = new TextPage(" Error Plots and Pt dependence. Only for Experts!!!");
  textpage->Print("itersum.ps");
  c1->Clear();	
  
  cout << "Making Error/Pt plots:" << endl;
  c1->Clear();
  if (iserrors){
    //first Residuals:
	  cout << "Start with Pt plots:" << endl;
	errors->cd();

	PtDependence* ptstuff = new PtDependence(datafile);	
  	ptstuff->Print("itersum.ps");

	TBDependence* tbstuff = new TBDependence(datafile);	
  	tbstuff->Print("itersum.ps");



  }






  // ###################################   ADD PLOTS FOR ToT and HT   #######################################33

  bool iscorrections = false;
  TDirectory* corrections=NULL  ;
  if (datafile->FindKey("Correction")) {
	corrections = (TDirectory*) datafile->FindKey("Correction")->ReadObj();
	iscorrections = true;
  }

  c1->Clear();
  textpage = new TextPage(" ToT Corrections and HT corrections: ");
  textpage->Print("itersum.ps");
  c1->Clear();	
  
  cout << "Making ToT / HT Plots" << endl;
  c1->Clear();
  if (iscorrections){
    //first Residuals:
	cout << "Start with ToT plots:" << endl;
	corrections->cd();

	ToTDependence* totstuff = new ToTDependence(datafile, true);	// Barrels:	
  	totstuff->Print("itersum.ps");

	ToTDependence* totstuff2 = new ToTDependence(datafile, false);	// Barrels:	
  	totstuff2->Print("itersum.ps");

	cout << "Start with HT plots:" << endl;

	ToTDependence* htstuff = new ToTDependence(datafile, true, true );	// Barrels:	
  	htstuff->Print("itersum.ps");

	ToTDependence* htstuff2 = new ToTDependence(datafile, false, true);	// Barrels:	
  	htstuff2->Print("itersum.ps");

	cout << "Start with sin(theta) / p  plots:" << endl;

	SinDependence* sinstuff = new SinDependence(datafile, true);	// Barrels:	
  	sinstuff->Print("itersum.ps");

	SinDependence* sinstuff2 = new SinDependence(datafile, false);	// Barrels:	
  	sinstuff2->Print("itersum.ps");

	cout << "Start with cos(theta) / p  plots:" << endl;

	SinDependence* cosstuff = new SinDependence(datafile, true, true);	// Barrels:	
  	cosstuff->Print("itersum.ps");

	SinDependence* cosstuff2 = new SinDependence(datafile, false, true);	// Barrels:	
  	cosstuff2->Print("itersum.ps");


  }






  // ###################################   ADD PLOTS FOR ToT and HT  ARGON  #######################################33

  bool iscorrectionsAr = false;
  TDirectory* correctionsAr=NULL  ;
  if (datafile->FindKey("CorrectionAr")) {
        correctionsAr = (TDirectory*) datafile->FindKey("CorrectionAr")->ReadObj();
        iscorrectionsAr = true;
  }

  if (iscorrectionsAr){
  	c1->Clear();
  	textpage = new TextPage(" ToT Corrections and HT corrections: ARGON");
  	textpage->Print("itersum.ps");
  	c1->Clear();

  	cout << "Making ToT / HT Plots ARGON" << endl;
  	c1->Clear();
    //first Residuals:
        cout << "Start with ToT plots:" << endl;
        corrections->cd();

        ToTDependence* totstuff = new ToTDependence(datafile, true, false, true);       // Barrels:     
        totstuff->Print("itersum.ps");

        ToTDependence* totstuff2 = new ToTDependence(datafile, false, false, true);     // Barrels:     
        totstuff2->Print("itersum.ps");

        cout << "Start with HT plots:" << endl;

        ToTDependence* htstuff = new ToTDependence(datafile, true, true, true );        // Barrels:     
        htstuff->Print("itersum.ps");

        ToTDependence* htstuff2 = new ToTDependence(datafile, false, true, true);       // Barrels:     
        htstuff2->Print("itersum.ps");

        cout << "Start with sin(theta) / p  plots:" << endl;

        SinDependence* sinstuff = new SinDependence(datafile, true, false, true);       // Barrels:     
        sinstuff->Print("itersum.ps");

//      SinDependence* sinstuff2 = new SinDependence(datafile, false, false, true);     // Barrels:     
//      sinstuff2->Print("itersum.ps");

        cout << "Start with cos(theta) / p  plots:" << endl;

//      SinDependence* cosstuff = new SinDependence(datafile, true, true, true);        // Barrels:     
//      cosstuff->Print("itersum.ps");

        SinDependence* cosstuff2 = new SinDependence(datafile, false, true, true);      // Barrels:     
        cosstuff2->Print("itersum.ps");


  }


















  datafile->Close();
  if (useref) refdatafile->Close();
  
  c1->Print("itersum.ps]");  

//   cout << "RDIFF -1 ";
//   for (map<double,double>::iterator rtpoint = rtrelation21->rtmap.begin(); rtpoint !=  rtrelation21->rtmap.end(); rtpoint++){
//     cout << binres1->resmap[rtpoint->second] << " ";
//   }
//   cout << endl;	
//   cout << "RDIFF 1 ";
//   for (map<double,double>::iterator rtpoint = rtrelation31->rtmap.begin(); rtpoint !=  rtrelation31->rtmap.end(); rtpoint++){
//     cout << binres2->resmap[rtpoint->second] << " ";
//   }
//   cout << endl;
  
//   cout << "RDIFF -2 ";
//   for (map<double,double>::iterator rtpoint = rtrelation41->rtmap.begin(); rtpoint !=  rtrelation41->rtmap.end(); rtpoint++){
//     cout << binres3->resmap[rtpoint->second] << " ";
//   }
//   cout << endl;
  
//   cout << "RDIFF 2 ";
//   for (map<double,double>::iterator rtpoint = rtrelation51->rtmap.begin(); rtpoint !=  rtrelation51->rtmap.end(); rtpoint++){
//     cout << binres4->resmap[rtpoint->second] << " ";
//   }
//   cout << endl;
  

  //add some extra info to each page
  if (settings && firstpage){
    string fixfloat, fieldonoff;
    if (settings->set["UsePol0"].find("True")!=string::npos) fixfloat=" - floating";
    else fixfloat=" - fixed";
    if (settings->set["RtRelation"].find("binned")!=string::npos) fixfloat="";
    if (settings->set["Solenoid"].find("True")!=string::npos) fieldonoff="ON";
    else fieldonoff="OFF";
    
    string pmarking = 
                    string("/Helvetica-Bold findfont 36 scalefont setfont 40 2190 moveto (Run:) show ") +
                    string(Form("/Helvetica findfont 36 scalefont setfont 240 2190 moveto (%i) show ",(int)firstpage->runnumber)) +

                    string("/Helvetica-Bold findfont 36 scalefont setfont 40 2150 moveto (Iteration:) show ") +
                    string(Form("/Helvetica findfont 36 scalefont setfont 240 2150 moveto (%02i) show ",firstpage->iter)) +

                    string("/Helvetica-Bold findfont 36 scalefont setfont 40 2110 moveto (Rt relation:) show ") +
                    string(Form("/Helvetica findfont 36 scalefont setfont 240 2110 moveto (%s%s) show ",settings->set["RtRelation"].data(),fixfloat.data())) +



                    string("/Helvetica-Bold findfont 36 scalefont setfont 540 2190 moveto (Rt binning:) show ") +
                    string(Form("/Helvetica findfont 36 scalefont setfont 740 2190 moveto (%s) show ",settings->set["RtBinning"].data())) +

                    string("/Helvetica-Bold findfont 36 scalefont setfont 540 2150 moveto (Rt level:) show ") +
      //string(Form("/Helvetica findfont 36 scalefont setfont 740 2050 moveto (%s) show ",settings->set["DoRt"].data())) +

                    string("/Helvetica-Bold findfont 36 scalefont setfont 540 2110 moveto (T0 level:) show ") +
      //string(Form("/Helvetica findfont 36 scalefont setfont 740 2010 moveto (%s) show ",settings->set["DoT0"].data())) +



                    string("/Helvetica-Bold findfont 36 scalefont setfont 1040 2190 moveto (Solenoid:) show ") +
                    string(Form("/Helvetica findfont 36 scalefont setfont 1260 2190 moveto (%s) show ",fieldonoff.data())) +

                    string("/Helvetica-Bold findfont 36 scalefont setfont 1040 2150 moveto (SW release:) show ") +
                    string(Form("/Helvetica findfont 36 scalefont setfont 1260 2150 moveto (%s) show ",settings->set["ReleaseRec"].data())) +

                    string("/Helvetica-Bold findfont 36 scalefont setfont 1040 2110 moveto (T0 Offset:) show ") +
                    string(Form("/Helvetica findfont 36 scalefont setfont 1260 2110 moveto (%s) show ",settings->set["T0Offset"].data())) +


                    string("/Helvetica-Bold findfont 30 scalefont setfont 40 2060 moveto (Data:) show ") +
                    string(Form("/Helvetica findfont 30 scalefont setfont 240 2060 moveto (%s) show ",settings->set["Inputdir"].data())) +

                    string("/Helvetica-Bold findfont 30 scalefont setfont 40 2020 moveto (Start const:) show ") +
                    string(Form("/Helvetica findfont 30 scalefont setfont 240 2020 moveto (%s) show ",settings->set["StartConst"].data()))
    ;
    system((string("sed -i 's?showpage?" + pmarking + "showpage?g' itersum.ps")).data());
    //cout << ((string("sed -i 's?showpage?" + pmarking + "showpage?g' itersum.ps")).data()) << endl;
  }


  //Add some graphs in a new direcrory "analysis"
  datafile = new TFile(argv[2],"UPDATE");
  datafile->cd();
  if (datafile->FindKey("analysis")) datafile->rmdir("analysis");
  TDirectory* adir = datafile->mkdir("analysis","analysis");
  adir->cd();
  if (chipgraphA) chipgraphA->Write();
  if (chipgraphC) chipgraphC->Write();
  boardgraphA->Write();
  boardgraphC->Write();
  if (refboardgraphA) refboardgraphA->Write();
  if (refboardgraphC) refboardgraphC->Write();
  if (dvgraph0) dvgraph0->Write();
  if (dvgraph1) dvgraph1->Write();
  if (dvgraph2) dvgraph2->Write();
  datafile->Write();
  datafile->Close();

  
  return 0;
  
}

int tt_remove(int argc, char* argv[]){

  TFile* itersumfile = new TFile(argv[2],"UPDATE");
  if (itersumfile->FindKey("tracktuple")) {
        itersumfile->Delete("tracktuple;*");
  }
  if (itersumfile->FindKey("binhist")) {
        itersumfile->rmdir("binhist");
  }
  if (itersumfile->FindKey("analysis")) {
        itersumfile->rmdir("analysis");
  }

  itersumfile->Write();
  itersumfile->Close();

  return 0;

}

int main(int argc, char* argv[]){

  gErrorIgnoreLevel = kFatal; // Explicitly remove all messages

  if ((string(argv[1])).find("itersum")!=std::string::npos) {
    cout << "GENERATING ITERATION SUMMARY PLOTS" << endl;
    itersum(argc,argv);
  }

  if ((string(argv[1])).find("restore")!=std::string::npos) {
    cout << "REMOVING THE TRACKTUPLE, BINHIST AND ANALYSE DIRECTORY" << endl;
    tt_remove(argc,argv);
  }


  return 0;

}
