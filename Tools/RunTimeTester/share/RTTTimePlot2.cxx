#include "TPostScript.h"
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdlib.h>
 
const std::vector<int>* readData(const char* fn){

  std::ifstream listIn(fn);
  std::vector<int>*  dataLow = new vector<int>;
  std::vector<int>*  dataHi  = new vector<int>;
  std::vector<int>*  seconds = new vector<int>;
  int dataL, dataH, second;
  while (listIn>>second>>dataH>>dataL) {
    //std::cout<<"dataL "<<dataL<<" dataH "<<dataH<<" second "<<second<<"\n";
    dataLow->push_back(dataL);
    dataHi->push_back(dataH);
    seconds->push_back(second);
  }
  //std::cout<<"hello3"<<std::endl;
  const std::vector<int>* vectors = new std::vector<int>[3] ;
  vectors[0] = dataLow;
  vectors[1] = dataHi;
  vectors[2] = seconds;
  return vectors;
}
  

void printInt(int t){ std::cout<<t<<std::endl;}
void printVec(std::vector<int> t){std::for_each(t.begin(), t.end(), printInt);}

//int RTTTimePlot2(const char* fn, const char* title){
int RTTTimePlot2(const char* title){

  const char* fn = (std::string(title)+".dat").c_str();

  std::vector<int>* data = readData(fn);


  std::vector<int> low       = data[0];
  std::vector<int> high      = data[1];
  std::vector<int> seconds   = data[2];

  double mintime = *std::min_element(seconds.begin(), seconds.end());
  double maxtime = *std::max_element(seconds.begin(), seconds.end());

  double toff = mintime;  
  gStyle->SetTimeOffset(toff);  
  mintime =mintime - toff;
  int secperday = 24*3600;
  maxtime =maxtime - toff+secperday;

  int ndays = int((maxtime - mintime)/secperday);

  

  TH1F* h1 = new TH1F("h1", title,   ndays, mintime, maxtime);
  TH1F* h2 = new TH1F("h2", title,   ndays, mintime, maxtime);

  std::vector<int>::const_iterator iterl = low.begin();
  std::vector<int>::const_iterator iterh = high.begin();
  std::vector<int>::const_iterator iters = seconds.begin();

  for(;iterl<low.end();++iterl){
    
    double l = *iterl;
    double h = *iterh;
    double s = *iters - toff;
    h1->Fill(s, h);
    h2->Fill(s, l);
    
    ++iterh;
    ++iters;
  }


  h1->GetXaxis()->SetTimeDisplay(1); 
  h1->GetXaxis()->SetTimeFormat("%d/%m/%y");
  h1->GetXaxis()->SetNdivisions(-ndays/3);
  
  h2->GetXaxis()->SetTimeDisplay(1); 
  h2->GetXaxis()->SetTimeFormat("%d/%m/%y");
  h2->GetXaxis()->SetNdivisions(-ndays/3);

  //const char* title2 = (std::string(title)+std::string(".gif")).c_str();
  //std::cout<<"printing 0 "<< title2<<std::endl;

  // TPostScript ps(title2, 113);
  TCanvas* canvas = new TCanvas(title, title, 1);
  canvas -> Divide(1,1);
  canvas->cd(1);
  h1->SetStats(kFALSE);
  h2->SetStats(kFALSE);
  h2->SetFillColor(142);
  h1->GetXaxis()->SetTitle("NICOS build date");  
  h1->GetYaxis()->SetTitle("OK/All");  
  h1->SetFillColor(42);
  h1->Draw();
  h2->Draw("SAME");

  // ps.Close();
  const char* title2 = (std::string(title)+std::string(".gif")).c_str();
  canvas->Print(title2);
  return 0;   
  
}
