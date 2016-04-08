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
 
std::vector<std::vector<int> > readData(const char* fn){

  std::ifstream listIn(fn);
  std::vector<int> values, seconds;
  int value, second;
  std::cout<<"reading data"<<"\n";
  while (listIn>>second>>value) {
    std::cout<<"dataL "<<value<<" second "<<second<<"\n";
    values.push_back(value);
    seconds.push_back(second);
  }
  std::vector<std::vector<int> > data;
  data.push_back(values);
  data.push_back(seconds);

  return data;

}
  

void printInt(int t){ std::cout<<t<<std::endl;}
void printVec(std::vector<int> t){std::for_each(t.begin(), t.end(), printInt);}

int RTTTimePlot(const char* title){

  const char* fn = (std::string(title)+".dat").c_str();

  std::cout<<"hello"<<std::endl;
  std::vector<std::vector<int> > data = readData(fn);
  std::cout<<"hello2"<<std::endl;


  
  std::vector<int> values    = data[0];
  std::vector<int> seconds   = data[1];

  double mintime = *std::min_element(seconds.begin(), seconds.end());
  double maxtime = *std::max_element(seconds.begin(), seconds.end());
  // std::cout<<"mintime"<<" "<<mintime<<std::endl;
  // std::cout<<"maxtime"<<" "<<maxtime<<std::endl;
 
  mintime = mintime;
  maxtime = maxtime;

  double toff = mintime;  
  gStyle->SetTimeOffset(toff);  
  mintime =mintime - toff;
  int secperday = 24*3600;
  maxtime =maxtime - toff+secperday;

  // std::cout<<"mintime"<<" "<<mintime<<std::endl;
  // std::cout<<"maxtime"<<" "<<maxtime<<std::endl;
  int ndays = int((maxtime - mintime)/secperday);

  

  TH1F* h1 = new TH1F("h1", title,   ndays, mintime, maxtime);

  std::vector<int>::const_iterator iterv = values.begin();
  std::vector<int>::const_iterator iters = seconds.begin();

  for(;iterv<values.end();++iterv){
    // std::cout<<*iterl<<" "<<*iterh<<" "<<*iters<<std::ends;
    double v = *iterv;
    double s = *iters - toff;
    std::cout<<v<<" "<<s<<std::endl;
    h1->Fill(s, v);
    
    ++iters;
  }


  h1->GetXaxis()->SetTimeDisplay(1); 
  h1->GetXaxis()->SetTimeFormat("%d/%m/%y");

  const char* title2 = (std::string(title)+std::string(".gif")).c_str();
  // TPostScript ps(title2, 112);
  
  TCanvas* canvas = new TCanvas(title, title, 1);
  canvas -> Divide(1,1);
  canvas->cd(1);
  h1->SetStats(kFALSE);
  h1->GetXaxis()->SetTitle("NICOS build date");  
  h1->GetXaxis()->SetNdivisions(-ndays);
  h1->GetYaxis()->SetTitle("Number");  
  h1->SetFillColor(42);
  h1->Draw();

  // ps.Close();
  canvas->Print(title2);
  return 0;   
  
}
