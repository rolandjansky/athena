/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// Usage:
// cat results_iterA*.txt > results_allA.txt
// cat results_iterC*.txt > results_allC.txt
// root
// root [0] .L plotIteration.C+
// root [1] plotIteration("results_allA.txt");
// root [1] plotIteration("results_allC.txt");
//

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "TMultiGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TROOT.h"
#include <numeric> //for std::iota
#include <algorithm>

void splitline(const std::string & s, std::vector<std::string> & words);
void splitdataline(const std::string & s, std::string & name, std::vector<double> & cols);
void plotIteration(const char * filename = 0, bool getNextAC = true);
void readResults(unsigned int ndet, 
		 std::vector<int> & iterOrig,
		 std::string * det, 
		 std::vector<float> * pull, 
		 std::vector<float> * pullError, 
		 std::vector<float> * rms, 
		 std::vector<float> * rmsError, 
		 std::vector<float> * aprev, 
		 std::vector<float> * cprev,
		 std::vector<float> * a, 
		 std::vector<float> * c,
		 const char * filename,
		 bool getNextAC = true);

TGraphErrors * makeGraph(const std::vector<float> &y_in);
TGraphErrors * makeGraph(const std::vector<float> &y_in, const std::vector<float> &ey_in);
TGraphErrors * makeGraph(const std::vector<int> &iter, const std::vector<float> &y_in, const std::vector<float> &ey_in);
TGraphErrors * makeGraph(const std::vector<int> &iter, const std::vector<float> &y_in);



/*
int main() {
  plotIteration();
  return 0;
}
*/

void
plotIteration(const char * filename, bool getNextAC)
{
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();

  if (!filename) filename = "results.txt";

  const unsigned int NDET = 8;
  std::vector<int> iteration;
  std::vector<float> pull[NDET]; 
  std::vector<float> pullError[NDET]; 
  std::vector<float> rms[NDET]; 
  std::vector<float> rmsError[NDET]; 
  std::vector<float> aprev[NDET];
  std::vector<float> cprev[NDET];
  std::vector<float> a[NDET];
  std::vector<float> c[NDET];
  std::string det[NDET];

  int mtype[] = {20,21,22,23,24,25,26,27};
  int color[] = {kBlack,kRed,kBlue,kGreen,kCyan,kMagenta,30,47};

  //int style = 0; // Put legeng within graph
  int style = 1; // Put legend at side

  readResults(NDET, iteration, det, pull, pullError, rms, rmsError, aprev, cprev, a, c, filename, getNextAC);

  /*
  for (unsigned int i = 0; i < iteration.size(); i++) {
    std::cout << "Iteration:" << iteration[i] << std::endl;
    for (int unsigned j=0; j < NDET; j++) {
      std::cout << det[j] << " " 
 		<< pull[j][i] << " " 
  		<< pullError[j][i] << " "
 		<< rms[j][i] << " " 
  		<< rmsError[j][i] << " "
  		<< aprev[j][i] << " "
  		<< cprev[j][i] << " "
  		<< a[j][i] << " "
  		<< c[j][i] 
		<< std::endl;
    }
  }
  */

  unsigned int ndet2 = NDET;
  //unsigned int ndet2 = 6;
  int canvW = 700;
  int canvH = 500;
  int canvX = 100;
  int canvY = 100;
  int canvStep = 100;

  {
    TCanvas * c1  = new TCanvas("c1","Pull Sigma", canvX, canvY, canvW, canvH);
    if (style == 1) c1->SetRightMargin(0.2);
    TLegend * leg = 0;
    if (style == 1) {
      leg = new TLegend(0.803,0.544,0.993,0.951);
    } else {
      leg = new TLegend(0.573,0.18,0.83,0.53);
    }
   
    TMultiGraph * mg = new TMultiGraph();
 

    for (unsigned int j=0; j < ndet2; j++) { 
      TGraphErrors * gr = makeGraph(iteration, pull[j], pullError[j]);
      gr->SetMarkerColor(color[j]);
      gr->SetMarkerStyle(mtype[j]);
      leg->AddEntry(gr,det[j].c_str(),"pl");
      mg->Add(gr);
   }

    mg->Draw("ALP");
    mg->GetXaxis()->SetTitle("Iteration");
    mg->GetYaxis()->SetTitle("Pull");
    leg->Draw();
  }

  {
    TCanvas * c2  = new TCanvas("c2","Pull RMS", canvX, canvY, canvW, canvH);
    if (style == 1) c2->SetRightMargin(0.2);
    TLegend * leg = 0;
    if (style == 1) {
      leg = new TLegend(0.803,0.544,0.993,0.951);
    } else {
      leg = new TLegend(0.573,0.18,0.83,0.53);
    }
   
    TMultiGraph * mg = new TMultiGraph();
 

    for (unsigned int j=0; j < ndet2; j++) { 
      TGraphErrors * gr = makeGraph(iteration, rms[j], rmsError[j]);
      gr->SetMarkerColor(color[j]);
      gr->SetMarkerStyle(mtype[j]);
      leg->AddEntry(gr,det[j].c_str(),"pl");
      mg->Add(gr);
   }

    mg->Draw("ALP");
    mg->GetXaxis()->SetTitle("Iteration");
    mg->GetYaxis()->SetTitle("RMS");
    leg->Draw();
  }


  {
    canvX += canvStep;
    canvY += canvStep;
    TCanvas * c3  = new TCanvas("c3","Parameter A",canvX, canvY, canvW, canvH);
    if (style == 1) c3->SetRightMargin(0.2);
        TLegend * leg = 0;
    if (style == 1) {
      leg = new TLegend(0.803,0.544,0.993,0.951);
    } else {
      leg = new TLegend(0.573,0.18,0.83,0.53);
    }

    TMultiGraph * mg = new TMultiGraph();

    for (unsigned int j=0; j < ndet2; j++) { 
      TGraphErrors * gr = makeGraph(iteration,aprev[j]);
      gr->SetMarkerColor(color[j]);
      gr->SetMarkerStyle(mtype[j]);
      mg->Add(gr);
      leg->AddEntry(gr,det[j].c_str(),"pl");
    }
    
    mg->Draw("ALP");
    mg->GetXaxis()->SetTitle("Iteration");
    mg->GetYaxis()->SetTitle("A");
    leg->Draw();
  }

  {
    canvX += canvStep;
    canvY += canvStep;
    TCanvas * c4  = new TCanvas("c4","Parameter C",canvX, canvY, canvW, canvH);
    if (style == 1) c4->SetRightMargin(0.2);
    TLegend * leg = 0;
    if (style == 1) {
      leg = new TLegend(0.803,0.544,0.993,0.951);
    } else {
      leg = new TLegend(0.573,0.18,0.83,0.53);
    }
    
    TMultiGraph * mg = new TMultiGraph();

    for (unsigned int j=0; j < ndet2; j++) { 
      TGraphErrors * gr = makeGraph(iteration,cprev[j]);
      gr->SetMarkerColor(color[j]);
      gr->SetMarkerStyle(mtype[j]);
      leg->AddEntry(gr,det[j].c_str(),"pl");
      mg->Add(gr);
   }
    
    mg->Draw("ALP");
    mg->GetXaxis()->SetTitle("Iteration");
    mg->GetYaxis()->SetTitle("C (micron)");
    //mg->GetHistogram()-GetXaxis()>SetRangeUser(0,10);
    leg->Draw();
  }

    
} 

void  
readResults(unsigned int ndet, 
	    std::vector<int> & iterOrig,
	    std::string * det, 
	    std::vector<float> * pull, 
	    std::vector<float> * pullError, 
	    std::vector<float> * rms, 
	    std::vector<float> * rmsError, 
	    std::vector<float> * aprev, 
	    std::vector<float> * cprev,
	    std::vector<float> * a, 
	    std::vector<float> * c,
	    const char * filename,
	    bool getNextAC)
{
  //int iteration = 0;
  std::ifstream infile(filename);
  if (!infile) std::cout << "Problem opening file: " << filename << std::endl;

  while(infile) {
    std::string tmpline;
    std::getline(infile, tmpline);
    //std::cout << tmpline << std::endl;
    std::istringstream istr(tmpline);
    std::string s;
    int iorig;
    istr >> s >> iorig;
    if (s == "Iteration:") {
      //std::cout <<  "Iteration:" << iorig << std::endl;
      if (iorig >= 100) iorig = -1;
      iterOrig.push_back(iorig);
  
      std::vector<std::string> words;

      // Previous values of A and C
      do {
	words.clear();
	std::getline(infile, tmpline);
	splitline(tmpline, words);
      } while (infile && !(words.size() > 0 && words[0] == "Det"));
	
      //std::cout << tmpline << std::endl;
      if (words.size() > 0 && words[0] == "Det") {
	for (unsigned int i = 0; i < ndet; i++) {
	  std::getline(infile, tmpline);
	  std::vector<double> cols;
	  cols.push_back(0); // "Det" col
	  std::string name;
	  splitdataline(tmpline, name, cols);
	  det[i] = name;
	  for (unsigned int j = 0; j < cols.size(); j++) {
	    //std::cout << "words[" << j << "]:" << words[j] << cols[j] << std::endl;
	    if (words[j] == "A") aprev[i].push_back(cols[j]);
	    if (words[j] == "C") cprev[i].push_back(cols[j]*1000);
	  }
	}
      }  

      // Resulting pulls and next values of A and C
      do {
	words.clear();
	std::getline(infile, tmpline);
	splitline(tmpline, words);
      } while (infile && !(words.size() > 0 && words[0] == "Det"));

      if (words.size() > 0 && words[0] == "Det") {
	for (unsigned int i = 0; i < ndet; i++) {
	  std::getline(infile, tmpline);
	  std::vector<double> cols;
	  cols.push_back(0); // "Det" col
	  std::string name;
	  splitdataline(tmpline, name, cols);
	  det[i] = name;
	  for (unsigned int j = 0; j < cols.size(); j++) {
	    //std::cout << "words[" << j << "]:" << words[j] << ": " << cols[j] << std::endl;
	    if (words[j] == "A") a[i].push_back(cols[j]);
	    if (words[j] == "C") c[i].push_back(cols[j]*1000);
	    if (words[j] == "Pull") pull[i].push_back(cols[j]);
	    if (words[j] == "Pull_err") pullError[i].push_back(cols[j]);
	    if (words[j] == "RMS") rms[i].push_back(cols[j]);
	    if (words[j] == "RMS_err") rmsError[i].push_back(cols[j]);
	  }
	}
      }  
    }
  }

  // Add next value of a and c
  if (getNextAC) {
    for (unsigned int i = 0; i < ndet; i++) {
      if (!c[i].empty()) cprev[i].push_back(c[i].back());
      if (!a[i].empty()) aprev[i].push_back(a[i].back());
    }
    if (!iterOrig.empty()) iterOrig.push_back(iterOrig.back()+1);
  }
 
}

void splitline(const std::string & s,  std::vector<std::string> & words) {
  std::istringstream istr(s);
  std::string tmp;
  while (istr) { 
    istr >> tmp;
    if (istr) words.push_back(tmp);
  }      
}

void splitdataline(const std::string & s, std::string & name, std::vector<double> & cols) {
  std::istringstream istr(s);
  std::string name2;
  istr >> name >> name2;
  name += " "+name2;
  while (istr) { 
    double tmp;
    istr >> tmp;
    if (istr) cols.push_back(tmp);
  }      
}

TGraphErrors * makeGraph(const std::vector<float> &y_in)
{
  std::vector<int> emptyiter;
  std::vector<float> empty;
  return makeGraph(emptyiter, y_in, empty);
}


TGraphErrors * makeGraph(const std::vector<int> &iter, const std::vector<float> &y_in)
{
  std::vector<float> empty;
  return makeGraph(iter, y_in, empty);
}  

TGraphErrors * makeGraph(const std::vector<float> &y_in, const std::vector<float> &ey_in)
{
  std::vector<int> emptyiter;
  return makeGraph(emptyiter, y_in, ey_in);
}

TGraphErrors * makeGraph(const std::vector<int> &iter, const std::vector<float> &y_in, const std::vector<float> &ey_in)
{
  size_t npoints = y_in.size();
  std::vector<float> y(y_in); //initialise y to y_in values
  std::vector<float> ex(npoints, 0.0f); //initialise ex to zero
  //
  std::vector<float> x(npoints, 0.0f); //initialise x to zero
  std::iota (x.begin(),x.end(),0.f); //initialise x with 0,1,2,3 etc ..
  std::copy(iter.begin(),iter.end(), x.begin()); //..but overwrite with 'iter' values
  //
  std::vector<float> ey(npoints, 0.0f);  //initialise ey to zero..
  std::copy(ey_in.begin(), ey_in.end(), ey.begin()); //..but overwrite with ey_in values
  //TGraphErrors takes array pointers as arguments
  TGraphErrors * g = new TGraphErrors(npoints,x.data(),y.data(),ex.data(),ey.data());
  return g;
}
      
