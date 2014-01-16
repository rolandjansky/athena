/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Author:   M.Schott(LMU)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class provides an Interface from Athena to Root, for
//		        the creating of more comples Histograms and Graphs, e.g. the
//		        Overlapping of Histograms and Graphs,...
//		        
// Modified:    Venkatesh S. Kaushik on Fri Apr 18 11:09:41 CEST 2008
//              Added two methods to pass histogram info to MuonPhysicsMonitoring
///////////////////////////////////////////////////////////////////////////

#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TMath.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "MuonValUtils/MSRootInterface.h"


#include "GaudiKernel/MsgStream.h"


//MSRootInterface::MSRootInterface(const std::string & type, const std::string & name, const IInterface* parent) :
//    ManagedMonitorToolBase(type, name, parent) , m_log( msgSvc(), name ) {
//      clear();
//    }

MSRootInterface::MSRootInterface()
{
  clear();
  //	TH1F *Hist1	= new TH1F("Test","test", 100, 0, 100);
}


void MSRootInterface::createDirectory(string filename, string directory)
{
  setDirectory(filename, directory);

  std::string name = filename+":/";
  TDirectory ttDir;
  ttDir.cd(name.c_str());
  TDirectory *sDir = gDirectory;
  sDir->mkdir(directory.c_str());
  ttDir.cd(name.c_str());
}

void MSRootInterface::createDirectory(string filename, string subDir, string directory)
{
  setDirectory(filename, directory);

  std::string name = filename+":/"+subDir+"/";
  TDirectory ttDir;
  ttDir.cd(name.c_str());
  TDirectory *sDir = gDirectory;
  sDir->mkdir(directory.c_str());
  ttDir.cd(name.c_str());
}

void MSRootInterface::setDirectory(string filename, string directory)
{
  if (directory=="") m_FullDirectory = filename+":/";
  else m_FullDirectory = filename+":/"+directory;

  m_Directory=directory;
  m_FileName=filename;
}

void MSRootInterface::clear()
{
  m_Directory="";
  mapGraph.clear();
  mapHistos.clear();
  map2DHistos.clear();

}


void MSRootInterface::setNewGraph(string id, string name, string xAxis, string yAxis)
{
  MSOutputDescription output;
  output.name=name;
  output.xAxis=xAxis;
  output.yAxis=yAxis;
  mapOutputDescription[id]=output;
}


//------------------------------------------------------------------------------//
//	1 Dimensional Histograms						//
//------------------------------------------------------------------------------//


void MSRootInterface::setNewHistogram(string id, string name, string xAxis, string yAxis, int bins, float start, float end)
{
  MSOutputDescription		output;
  output.name			= name;
  output.xAxis			= xAxis;
  output.yAxis			= yAxis;
  output.bins			= bins;
  output.start			= start;
  output.end			= end;
  vector<MSHistogrammDescription>	  dummy;
  mapHistos[id]			= dummy;
  mapOutputDescription[id]	= output;
}

void MSRootInterface::setNewHistogram(string id, string name, string xAxis, string yAxis, vector<double> binsv)
{
  MSOutputDescription		output;
  output.name			= name;
  output.xAxis			= xAxis;
  output.yAxis			= yAxis;
  output.bins			= binsv.size()-1;
  output.binsv                    = binsv;                       
  output.start			= -99;
  output.end			= -99;
  vector<MSHistogrammDescription>	  dummy;
  mapHistos[id]			= dummy;
  mapOutputDescription[id]	= output;
}

void MSRootInterface::clearHistogram(string id)
{
  if (mapHistos.find(id) == mapHistos.end())	return;

  vector<MSHistogrammDescription>	  dummy;
  dummy.clear();
  mapHistos[id]			= dummy;
}

void MSRootInterface::fill(string id, float value)
{
  if (mapHistos.find(id) == mapHistos.end())	return;
  MSHistogrammDescription e;
  e.value=value;
  e.weight=1.0;
  e.error=0.;
  mapHistos[id].push_back(e);
}


void MSRootInterface::fill(string id, float value, float weight)
{
  if (mapHistos.find(id) == mapHistos.end())	return;
  MSHistogrammDescription e;
  e.value=value;
  e.weight=weight;
  e.error=0;
  mapHistos[id].push_back(e);
}

void MSRootInterface::fill(string id, float value, float weight, float error)
{
  if (mapHistos.find(id) == mapHistos.end())	return;
  MSHistogrammDescription e;
  e.value=value;
  e.weight=weight;
  e.error=error;
  mapHistos[id].push_back(e);
}

void MSRootInterface::getHistogramInfo(string id1, MSOutputDescription &outputDes, vector<double> &output)
{
  if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;
  if (mapHistos.find(id1) == mapHistos.end())				return;

  outputDes = mapOutputDescription[id1];
  output.clear();
  for (long i=0; i<(long)mapHistos[id1].size(); i++)
    {
      output.push_back(mapHistos[id1][i].value);
    }
}

/***** This implementation works if MSRootInterface inherits from ManagedMonitorToolBase
 //VSK -- Begin
 StatusCode MSRootInterface::RegisterWithMonGroup( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, string id1) {
    
 MsgStream log(msgSvc(), name());
    
 StatusCode sc_success = StatusCode::SUCCESS;
 StatusCode sc_failure = StatusCode::FAILURE;
 StatusCode sc;
 std::string m_generic_path_muonphysicsmonitoring = "Muon/MuonPhysicsMonitoring/Zmumu/";

 //declare a group of histograms
 MonGroup muonphysmon_shift( this, m_generic_path_muonphysicsmonitoring+"/Overview", shift, run );

 if (isNewEventsBlock){}
 if (isNewLumiBlock){}
 if (isNewRun) {

 // Check to see if the Histogram is created by the MSRootInterface
 if (m_FullDirectory!="")  gDirectory->Cd(m_FullDirectory.c_str());

 if (mapOutputDescription.find(id1) == mapOutputDescription.end()) {
 m_log << MSG::ERROR << " Could not find Histogram Description: " << id1 << endreq;
 return sc_failure;
 }
 if (mapHistos.find(id1) == mapHistos.end()) {
 m_log << MSG::ERROR << " Could not find Histogram with id: " << id1 << endreq;
 return sc_failure;
 }


 // Get the histogram from MSRootInterface
 TH1F *m_regHistogram;
 if(mapOutputDescription[id1].start == mapOutputDescription[id1].end &&
 mapOutputDescription[id1].start == -99 &&
 mapOutputDescription[id1].end == -99) {
 double* bins = &(mapOutputDescription[id1].binsv[0]);
 m_regHistogram  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, bins);
 } else {
 m_regHistogram  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), 
 mapOutputDescription[id1].bins, mapOutputDescription[id1].start, mapOutputDescription[id1].end);
 }

 // Register the histogram to MonGroup
 sc=muonphysmon_shift.regHist(m_regHistogram);
 if(sc.isFailure()) {
 m_log << MSG::ERROR << "Failed to register histogram: " << id1 << endreq;
 return sc;
 }

 }
 return sc; // If I am here, then I registered the histogram successfully
 }
 //VSK --End
 **********************************/

/*
//VSK -- Begin v1
bool MSRootInterface::GetHistograms(vector<TH1*>& histos, vector<string>& histonames) {

// Check to see where histogram is created by the MSRootInterface -- VSK
if (m_FullDirectory!="")  gDirectory->Cd(m_FullDirectory.c_str());
    
// Loop over all the required monitoring histograms -- VSK
for (int iT = 0; iT < histonames.size(); iT++) {
        
std::string id1 = histonames[iT];
if (mapOutputDescription.find(id1) == mapOutputDescription.end()) {
// Could not find Histogram Description -- VSK
return false;
}
if (mapHistos.find(id1) == mapHistos.end()) {
// Could not find Histogram with id: id1 -- VSK
return false;
}

      
// Get the histogram from MSRootInterface -- VSK
TH1F *m_regHistogram;
if(mapOutputDescription[id1].start == mapOutputDescription[id1].end &&
mapOutputDescription[id1].start == -99 &&
mapOutputDescription[id1].end == -99) {
double* bins = &(mapOutputDescription[id1].binsv[0]);
m_regHistogram  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, bins);
} else {
m_regHistogram  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(),
mapOutputDescription[id1].bins, mapOutputDescription[id1].start, mapOutputDescription[id1].end);
} // else

// Save the histogram pointer in an array -- VSK
histos.push_back((TH1*)m_regHistogram->Clone());
}
// If I am here, GetHistograms was successful -- VSK
return true;
}
// VSK -- End v1
*/

vector<TH1*> MSRootInterface::GetHistograms(vector<string>& histonames) {

  std::vector<TH1*>histos;

  // Check to see where histogram is created by the MSRootInterface -- VSK
  if (m_FullDirectory!="")  gDirectory->Cd(m_FullDirectory.c_str());

  // Loop over all the required monitoring histograms -- VSK
  for (long iT = 0; iT < (long) histonames.size(); iT++) {

    std::string id1 = histonames[iT];
    if (mapOutputDescription.find(id1) == mapOutputDescription.end()) {
      // Could not find Histogram Description -- VSK
      return histos;
    }
    if (mapHistos.find(id1) == mapHistos.end()) {
      // Could not find Histogram with id: id1 -- VSK
      return histos;
    }


    // Get the histogram from MSRootInterface -- VSK
    TH1F *m_regHistogram;
    if(mapOutputDescription[id1].start == mapOutputDescription[id1].end &&
       mapOutputDescription[id1].start == -99 &&
       mapOutputDescription[id1].end == -99) {
      double* bins = &(mapOutputDescription[id1].binsv[0]);
      m_regHistogram  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, bins);
    } else {
      m_regHistogram  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(),
				 mapOutputDescription[id1].bins, mapOutputDescription[id1].start, mapOutputDescription[id1].end);
    } // else

    // Save the histogram pointer in an array -- VSK
    // Clone the histogram
    TH1 *hMon = (TH1*) m_regHistogram->Clone();
    // "_mon" is suffixed to its name -- to avoid Memory leak ROOT needs unique histogram names -- VSK
    string newname = string(m_regHistogram->GetName()) + string("_mon") ; 
    hMon->SetName(newname.c_str());
    // Save the histogram -- VSK
    histos.push_back(hMon);
  }
  // If I am here, GetHistograms was successful -- VSK
  return histos;
}



bool MSRootInterface::FillHistograms(vector<TH1*>& histos) {
    
  bool retVal = false;
  // Loop over all the required monitoring histograms -- VSK
  for (long iT = 0; iT < (long) histos.size(); iT++) {
    std::string id = histos[iT]->GetName(), id1;
    std::string::size_type pos=id.length();
    if((pos = id.find("_mon")) != std::string::npos) id1 = id.erase(pos);
    if (mapOutputDescription.find(id1) == mapOutputDescription.end()) {
      // Could not find Histogram Description -- VSK
      return retVal;
    }
    if (mapHistos.find(id1) == mapHistos.end()) {
      // Could not find Histogram with id: id1 -- VSK
      return retVal;
    }
    // Get the latest entry to fill histogram -- VSK
    float value, weight = 1.0, error = 0.;
    long hcur = (long) mapHistos[id1].size();
        
    // If hcur is 0, there are no entries to fill -- VSK
    if(hcur > 0) {
      value = (float)mapHistos[id1][hcur-1].value;
      weight = (float)mapHistos[id1][hcur-1].weight;
      error = (float)mapHistos[id1][hcur-1].error;
      //histos[iT]->Sumw2(); // Do we need this everytime we fill? don't know -- (we don't need it)
      histos[iT]->Fill(value,weight);
      histos[iT]->SetBinError(hcur,error); 
      retVal = true;
    } // if
        
  } // for

  return retVal;
}
//VSK -- End


void MSRootInterface::writeHistogram(string id1)
{
  if (m_FullDirectory!="")	gDirectory->Cd(m_FullDirectory.c_str());

  if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;
  if (mapHistos.find(id1) == mapHistos.end())				return;

  TH1F *Hist1;

  if(mapOutputDescription[id1].start == mapOutputDescription[id1].end &&
     mapOutputDescription[id1].start == -99 &&
     mapOutputDescription[id1].end == -99) {

    double* bins = &(mapOutputDescription[id1].binsv[0]);
    Hist1  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, bins);
  }else{
    Hist1  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, mapOutputDescription[id1].start, mapOutputDescription[id1].end);
  }

  Hist1->Sumw2();

  for (long i=0; i<(long)mapHistos[id1].size(); i++) {
    Hist1->Fill(mapHistos[id1][i].value, mapHistos[id1][i].weight);
    Hist1->SetBinError(i+1, mapHistos[id1][i].error);
  }

  Hist1->SetFillColor(7);
  Hist1->SetFillStyle(1001);
  Hist1->SetMarkerStyle(20);
  Hist1->SetMarkerColor(4);
  Hist1->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
  Hist1->GetXaxis()->SetLabelFont(42);
  Hist1->GetXaxis()->SetLabelSize(0.05);
  Hist1->GetXaxis()->SetTitleSize(0.06);
  Hist1->GetXaxis()->SetTitleOffset(0.85);
  Hist1->GetXaxis()->SetTitleFont(52);
  Hist1->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
  Hist1->GetYaxis()->SetLabelFont(42);
  Hist1->GetYaxis()->SetLabelSize(0.05);
  Hist1->GetYaxis()->SetTitleSize(0.06);
  Hist1->GetYaxis()->SetTitleOffset(0.85);
  Hist1->GetYaxis()->SetTitleFont(52);
  Hist1->GetZaxis()->SetLabelSize(0.03);
  Hist1->GetZaxis()->SetTitleSize(0.03);
  Hist1->Write(id1.c_str());
  Hist1->Draw(id1.c_str());

  delete Hist1;
}


void MSRootInterface::writeOverlayHistograms(string id1, string id2, string outputName)
{
  if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;
  if (mapHistos.find(id1) == mapHistos.end())				return;
  if (mapOutputDescription.find(id2) == mapOutputDescription.end())	return;
  if (mapHistos.find(id2) == mapHistos.end())				return;

  if (m_FullDirectory!="")	gDirectory->Cd(m_FullDirectory.c_str());
	
  TH1F *Hist1, *Hist2(NULL);

  if(mapOutputDescription[id1].start == mapOutputDescription[id1].end &&
     mapOutputDescription[id1].start == -99 &&
     mapOutputDescription[id1].end == -99) {
	  
    double* bins = &(mapOutputDescription[id1].binsv[0]);
    Hist1  = new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, bins);
  }else{
    Hist1	= new TH1F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, mapOutputDescription[id1].start, mapOutputDescription[id1].end);
  }
	
  if(mapOutputDescription[id2].start == mapOutputDescription[id2].end &&
     mapOutputDescription[id2].start == -99 &&
     mapOutputDescription[id2].end == -99) {
	  
    double* bins = &(mapOutputDescription[id2].binsv[0]);
    Hist1  = new TH1F(id2.c_str(), mapOutputDescription[id2].name.c_str(), mapOutputDescription[id2].bins, bins);
  }else{
    Hist2	= new TH1F(id2.c_str(), mapOutputDescription[id2].name.c_str(), mapOutputDescription[id2].bins, mapOutputDescription[id2].start, mapOutputDescription[id2].end);
  }
	
  Hist1->Sumw2();
  Hist2->Sumw2();

  for (long i=0; i<(long)mapHistos[id1].size(); i++) {
    Hist1->Fill(mapHistos[id1][i].value, mapHistos[id1][i].weight); 
    Hist1->SetBinError(i+1, mapHistos[id1][i].error);
  }
  for (long i=0; i<(long)mapHistos[id2].size(); i++) {
    Hist2->Fill(mapHistos[id2][i].value, mapHistos[id2][i].weight);
    Hist2->SetBinError(i+1, mapHistos[id2][i].error);
  }

  TCanvas *histcanvas = new TCanvas("histcanvas", "Histogram",1,40,797,640);
  histcanvas->ToggleEventStatus();
  histcanvas->SetHighLightColor(2);
  histcanvas->SetFillColor(0);
  histcanvas->SetBorderMode(0);
  histcanvas->SetBorderSize(1);
  histcanvas->SetGridx();
  histcanvas->SetGridy();
  histcanvas->SetLeftMargin(0.1);
  histcanvas->SetRightMargin(0.05);
  histcanvas->SetTopMargin(0.05);
  histcanvas->SetBottomMargin(0.12);
  histcanvas->SetFrameFillStyle(0);
  histcanvas->SetFrameLineStyle(0);
  histcanvas->SetFrameLineWidth(2);
  histcanvas->SetFrameBorderMode(0);
  histcanvas->SetFrameFillStyle(0);
  histcanvas->SetFrameLineStyle(0);
  histcanvas->SetFrameLineWidth(2);
  histcanvas->SetFrameBorderMode(0);

  Hist1->SetFillColor(7);
  Hist1->SetFillStyle(1001);
  Hist1->SetMarkerStyle(20);
  Hist1->SetMarkerColor(4);
  Hist1->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
  Hist1->GetXaxis()->SetLabelFont(42);
  Hist1->GetXaxis()->SetLabelSize(0.05);
  Hist1->GetXaxis()->SetTitleSize(0.06);
  Hist1->GetXaxis()->SetTitleOffset(0.85);
  Hist1->GetXaxis()->SetTitleFont(52);
  Hist1->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
  Hist1->GetYaxis()->SetLabelFont(42);
  Hist1->GetYaxis()->SetLabelSize(0.05);
  Hist1->GetYaxis()->SetTitleSize(0.06);
  Hist1->GetYaxis()->SetTitleOffset(0.85);
  Hist1->GetYaxis()->SetTitleFont(52);
  Hist1->GetZaxis()->SetLabelSize(0.03);
  Hist1->GetZaxis()->SetTitleSize(0.03);
  Hist1->Draw("");

  Hist2->SetFillColor(8);
  Hist2->SetFillStyle(3001);
  Hist2->SetMarkerStyle(21);
  Hist2->SetMarkerColor(2);
  Hist2->GetXaxis()->SetTitle(mapOutputDescription[id2].xAxis.c_str());
  Hist2->GetXaxis()->SetLabelFont(42);
  Hist2->GetXaxis()->SetLabelSize(0.05);
  Hist2->GetXaxis()->SetTitleSize(0.06);
  Hist2->GetXaxis()->SetTitleOffset(0.85);
  Hist2->GetXaxis()->SetTitleFont(52);
  Hist2->GetYaxis()->SetTitle(mapOutputDescription[id2].yAxis.c_str());
  Hist2->GetYaxis()->SetLabelFont(42);
  Hist2->GetYaxis()->SetLabelSize(0.05);
  Hist2->GetYaxis()->SetTitleSize(0.06);
  Hist2->GetYaxis()->SetTitleOffset(0.85);
  Hist2->GetYaxis()->SetTitleFont(52);
  Hist2->GetZaxis()->SetLabelSize(0.03);
  Hist2->GetZaxis()->SetTitleSize(0.03);
  Hist2->Draw("same");

  TLegend *leg = new TLegend(0.7,0.7,0.98,0.98,NULL,"brNDC");
  leg->SetLineColor(1);
  leg->SetLineStyle(1);
  leg->SetLineWidth(1);
  leg->SetFillColor(0);
  leg->SetFillStyle(1001);
  TLegendEntry *entry=leg->AddEntry(id1.c_str(),mapOutputDescription[id1].name.c_str(),"f");
  entry->SetFillColor(1);
  entry->SetFillStyle(3005);
  entry->SetLineColor(1);
  entry->SetLineStyle(1);
  entry->SetLineWidth(1);
  entry->SetMarkerColor(1);
  entry->SetMarkerStyle(21);
  entry->SetMarkerSize(2);
  entry->SetTextAlign(12);
  entry->SetTextColor(1);
  entry=leg->AddEntry(id2.c_str(),mapOutputDescription[id2].name.c_str(),"f");
  entry->SetFillStyle(1001);
  entry->SetLineColor(1);
  entry->SetLineStyle(1);
  entry->SetLineWidth(1);
  entry->SetMarkerColor(1);
  entry->SetMarkerStyle(21);
  entry->SetMarkerSize(1);
  entry->SetTextAlign(12);
  entry->SetTextColor(1);
  leg->Draw();

  histcanvas->Modified();
  histcanvas->cd();
  histcanvas->SetSelected(histcanvas);
  histcanvas->Write(outputName.c_str());
  histcanvas->Draw(outputName.c_str());
  delete Hist1;
  delete Hist2;
  delete histcanvas;
}


double MSRootInterface::getMeanOfHistogram(string id)
{
  long i;
  double mean=0.0;

  if (mapHistos.find(id) == mapHistos.end())				return -1.0;

  for (i=0; i<(long)mapHistos[id].size(); i++)
    mean+=(mapHistos[id][i].value*mapHistos[id][i].weight);
  return mean/((double)(i));
}

double MSRootInterface::getRMSOfHistogram(string id)
{
  long	i;
  double	rms=0.0;
  double	mean=0.0;

  if (mapHistos.find(id) == mapHistos.end())				return -1.0;
  mean = getMeanOfHistogram(id);

  for (i=0; i<(long)mapHistos[id].size(); i++)
    rms+=pow((mean-(mapHistos[id][i].value*mapHistos[id][i].weight)),2);

  return sqrt(rms/((double)(i)));
}


// lower is the limit where ratio=e.g 80% of the entries lie above
// higher is the limit where ratio=e.g 80% of the entries lie above
void   MSRootInterface::getRangeLimitsOfHistogram(string id, double ratio, double &lower, double &higher)
{
  lower=-1.0;
  higher=-1.0;
  if (mapHistos.find(id) == mapHistos.end())	return;
  if (ratio<0.0)	return;
  if (1.0<ratio)	return;

  double val;
  double stepsize;
  long i;

  // find lowest and highest values
  lower=(mapHistos[id][0].value*mapHistos[id][0].weight);
  higher=(mapHistos[id][0].value*mapHistos[id][0].weight);
  for (i=0; i<(long)mapHistos[id].size(); i++)
    {
      val=(mapHistos[id][i].value*mapHistos[id][i].weight);
      if (val<lower) lower=val;
      if (higher<val) higher=val;
    }

  // Estimate Stepsize
  stepsize=(higher-lower)/((double)mapHistos[id].size());
  long	max=(long)mapHistos[id].size();
  long	akt;
  // get Lower Limit
  akt=max;
  while	((((double)(akt))/((double)(max)))>ratio)
    {
      lower+=stepsize;
      akt=0;
      for (i=0; i<(long)mapHistos[id].size(); i++)
	{
	  val=(mapHistos[id][i].value*mapHistos[id][i].weight);
	  if (val>lower) akt++;
	}
    }

  // get Upper Limit
  akt=max;
  while	((((double)(akt))/((double)(max)))>ratio)
    {
      higher-=stepsize;
      akt=0;
      for (i=0; i<(long)mapHistos[id].size(); i++)
	{
	  val=(mapHistos[id][i].value*mapHistos[id][i].weight);
	  if (higher>val) akt++;
	}
    }
}

Double_t fit_bwg(Double_t *x, Double_t *par)
{
  Double_t bw[120]=
    {0.00127797,	0.00137711,	0.00142562,	0.00142367,	0.00138008,	0.00137223,
     0.00139764,	0.00140826,	0.00142374,	0.00144498,	0.00147656,	0.00152875,
     0.00159296,	0.00165442,	0.00169682,	0.00171987,	0.00174258,	0.00178496,
     0.00184745,	0.00191102,	0.00195734,	0.00198604,	0.00201324,	0.00205803,
     0.00212945,	0.00221961,	0.00230676,	0.0023716,	0.00241754,	0.00247452,
     0.00257754,	0.00273701,	0.00292863,	0.0031115,	0.00325597,	0.00335793,
     0.00343724,	0.00353078,	0.00367936,	0.00390191,	0.00417382,	0.00444031,
     0.00466217,	0.00485039,	0.00506092,	0.0053681,	0.00584267,	0.00652931,
     0.0074183,	0.00844359,	0.00954148,	0.0107367,	0.012165,	0.0139602,	
     0.0165062,	0.0199373,	0.0249048,	0.0322575,	0.0425649,	0.0587746,
     0.0798347,	0.0991731,	0.103405,	0.085914,	0.0626074,	0.0435083,
     0.0283674,	0.0191374,	0.0139471,	0.0107776,	0.00879563,	0.00726877,
     0.0060503,	0.00507493,	0.0043013,	0.00370481,	0.00324928,	0.00287549,
     0.00253455,	0.00222211,	0.00196554,	0.00178791,	0.00166775,	0.00154067,
     0.00139951,	0.00128836,	0.00119617,	0.00109696,	0.000985893,	0.000873227,
     0.000785877,	0.000714149,	0.000643602,	0.000590844,	0.000555048,	0.00052091,
     0.000490724,	0.000455894,	0.00041586,	0.000387901,	0.0003621,	0.000343127,
     0.000333103,	0.000308865,	0.000275397,	0.000254986,	0.000243638,	0.000227633,
     0.000211036,	0.000199103,	0.000192421,	0.000188789,	0.000183909,	0.000176311,
     0.000167005,	0.000158099,	0.000151712,	0.000147755,	0.000144633,	0.000141605};

  Double_t xMean = x[0]-90000.0;
  Double_t fitval=0.0; 

  for (int il=0;il<120;il++)
    {
      Double_t bwb=60000.25+500.0*il;
      Double_t gc=(bwb-par[0]);
      Double_t arg = 0;
		
      if (par[2] != 0) arg = (xMean - gc)/par[1];
      fitval += bw[il]*par[2]*TMath::Exp(-0.5*arg*arg);
    }

  return fitval;
}

bool MSRootInterface::fitFunctionToHistogram(	string id, string functionName, double lowerRange, double higherRange,
						vector<MSFitParameterDescription> &FitParameters)
{
  if (mapHistos.find(id) == mapHistos.end())	return false;
  if (functionName=="")	return false;

  MSFitParameterDescription	msFitParameterDescription;

  TF1 *fitFunction=0;
  if (functionName=="Gauss")
    {
      functionName="[2]*exp(-((x[0]-[0])/[1])*((x[0]-[0])/[1]))";
      FitParameters.clear();
      msFitParameterDescription.start=90000.;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=2000.;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=100.;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=10.;
      FitParameters.push_back(msFitParameterDescription);
      fitFunction= new TF1("fitFunction",functionName.c_str(), lowerRange, higherRange);
    }
  if (functionName=="ZBosonFitFunction")
    {
      fitFunction = new TF1("fit_bwg",fit_bwg,60,110, 3);
      FitParameters.clear();
      msFitParameterDescription.start=90000.;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=2000.;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=2000.;
      FitParameters.push_back(msFitParameterDescription);
    }
  if (functionName=="PTResolutionFitFunction")
    {
      functionName="[3] + [1]*exp(-((x[0]-[0])/[2])*((x[0]-[0])/[2])) + 0.5 [1]*exp(-((x[0]-[0]-0.01)/(3.0*[2]))*((x[0]-[0]-0.01)/(3.0*[2])))";
      FitParameters.clear();
      msFitParameterDescription.start=.0;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=10.;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=0.12;
      FitParameters.push_back(msFitParameterDescription);
      msFitParameterDescription.start=100.;
      FitParameters.push_back(msFitParameterDescription);

      fitFunction= new TF1("fitFunction",functionName.c_str(), lowerRange, higherRange);
    }

  for (int i=0; i<(long)FitParameters.size(); i++)	fitFunction->SetParameter(i,FitParameters[i].start);

  // Calculate Z-Width overall
  TH1F mHistoZWidthOverall("Internal_Histo","Internal_Histo",50, lowerRange,higherRange);
  for (int i=0; i<(long)mapHistos[id].size(); i++)
    {
      mHistoZWidthOverall.Fill(mapHistos[id][i].value*mapHistos[id][i].weight);
    }
  mHistoZWidthOverall.Fit(fitFunction);

  for (int i=0; i<(long)FitParameters.size(); i++)
    {
      if (i==1)
	{
	  FitParameters[i].fit=fabs(fitFunction->GetParameter(i));
	  FitParameters[i].error=fabs(fitFunction->GetParError(i));
	} else
	  {
	    FitParameters[i].fit=(fitFunction->GetParameter(i));
	    FitParameters[i].error=(fitFunction->GetParError(i));
	  }
    }

  return true;
}


//------------------------------------------------------------------------------//
//	2 Dimensional Histograms						//
//------------------------------------------------------------------------------//

void MSRootInterface::setNew2DHistogram(string id, string name, string xAxis, string yAxis, 
					int bins, float start, float end,
					int bins2, float start2, float end2)
{
  MSOutputDescription		output;
  output.name			= name;
  output.xAxis			= xAxis;
  output.yAxis			= yAxis;
  output.bins			= bins;
  output.start			= start;
  output.end			= end;
  output.bins_2d			= bins2;
  output.start_2d			= start2;
  output.end_2d			= end2;
  vector<MS2DHistogrammDescription>	  dummy;
  map2DHistos[id]			= dummy;
  mapOutputDescription[id]	= output;
}


void MSRootInterface::fill2D(string id, float valueX, float valueY, float weight)
{
  if (map2DHistos.find(id) == map2DHistos.end())	return;
  MS2DHistogrammDescription e;
  e.valueX=valueX;
  e.valueY=valueY;
  e.weight=weight;
  map2DHistos[id].push_back(e);
}


void MSRootInterface::write2DHistogram(string id1)
{
  if (m_FullDirectory!="")	gDirectory->Cd(m_FullDirectory.c_str());

  if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;
  if (map2DHistos.find(id1) == map2DHistos.end())				return;

  TH2F *Hist1	= new TH2F(id1.c_str(), mapOutputDescription[id1].name.c_str(), mapOutputDescription[id1].bins, mapOutputDescription[id1].start, mapOutputDescription[id1].end, mapOutputDescription[id1].bins_2d, mapOutputDescription[id1].start_2d, mapOutputDescription[id1].end_2d);

  for (long i=0; i<(long)map2DHistos[id1].size(); i++)	Hist1->Fill(map2DHistos[id1][i].valueX, map2DHistos[id1][i].valueY, map2DHistos[id1][i].weight);

  Hist1->SetFillColor(30);
  Hist1->SetFillStyle(1001);
  Hist1->SetMarkerStyle(7);
  Hist1->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
  Hist1->GetXaxis()->SetLabelFont(42);
  Hist1->GetXaxis()->SetLabelSize(0.05);
  Hist1->GetXaxis()->SetTitleSize(0.06);
  Hist1->GetXaxis()->SetTitleOffset(0.85);
  Hist1->GetXaxis()->SetTitleFont(52);
  Hist1->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
  Hist1->GetYaxis()->SetLabelFont(42);
  Hist1->GetYaxis()->SetLabelSize(0.05);
  Hist1->GetYaxis()->SetTitleSize(0.06);
  Hist1->GetYaxis()->SetTitleOffset(0.85);
  Hist1->GetYaxis()->SetTitleFont(52);
  Hist1->GetZaxis()->SetLabelSize(0.03);
  Hist1->GetZaxis()->SetTitleSize(0.03);

  Hist1->Write(id1.c_str());
  Hist1->Draw(id1.c_str());

  Hist1->Draw("colz"); 

  /*	histcanvas->Modified();
	histcanvas->cd();
	histcanvas->SetSelected(histcanvas);
	histcanvas->Write(mapOutputDescription[id1].name.c_str());
	histcanvas->Draw(mapOutputDescription[id1].name.c_str());*/

  delete Hist1;
  //	delete histcanvas;
}

//------------------------------------------------------------------------------//
//      Histograms as 2 Graphs                                                  //
//------------------------------------------------------------------------------//

void MSRootInterface::setHistogramPoints(string id, MSEfficiency &msEfficiency, int Dimension)
{
  double Eff;
  double EffError;
  double meanOfBin1;
  double widthOfBin1;
  
  if ((Dimension==1) || (Dimension==2) || (Dimension==3))
    {
      for (int j=0; j<(int)	msEfficiency.getNumberOfBins(Dimension); j++)
	{
	  msEfficiency.getDimBinInfo(Dimension, j, meanOfBin1, widthOfBin1);
	  if (Dimension==1) msEfficiency.getEfficiency1(meanOfBin1, Eff, EffError);
	  if (Dimension==2) msEfficiency.getEfficiency2(meanOfBin1, Eff, EffError);
	  if (Dimension==3) msEfficiency.getEfficiency3(meanOfBin1, Eff, EffError);
	  
	  fill(id, meanOfBin1, Eff, EffError);
	}
    }
}

void MSRootInterface::setHistogramPoints(string id, MSResolution &msResolution, string BinID, string sType)
{
  double PTResMean;
  double PTResWidth;
  double PT68Width;
  double ErrWidth;
  double meanOfBin;
  double widthOfBin;
	
  double ZBosonMean;
  double ErrMean;
  double ZBosonSigma;
  double ErrSigma;

  //if (mapGraph.find(id) == mapGraph.end())	return;
	
  for (int j=0; j<(int)msResolution.getNumberOfBins(BinID); j++)
    {
      msResolution.getDimBinInfo(BinID, j, meanOfBin, widthOfBin);
      if ((sType=="GausSigma") || (sType=="Sigma68") || (sType=="MeanValue"))
	{
	  msResolution.getResolution(BinID, meanOfBin, PTResMean, ErrMean, PT68Width, PTResWidth, ErrWidth);
	  if (sType=="GausSigma") fill(id, meanOfBin, PTResWidth, ErrWidth);
	  if (sType=="Sigma68")   fill(id, meanOfBin, PT68Width,  ErrWidth);
	  if (sType=="MeanValue") fill(id, meanOfBin, PTResMean,  ErrMean);
	}	     
      if ((sType=="ZBoson_Width") || (sType=="ZBoson_Mean"))
	{
	  msResolution.getZBosonProperties(BinID, meanOfBin, ZBosonMean, ErrMean, ZBosonSigma, ErrSigma);
	  if (sType=="ZBoson_Mean")
	    {
	      fill(id, meanOfBin, ZBosonMean, ErrMean);
	    }
	  if (sType=="ZBoson_Width")
	    {
	      fill(id, meanOfBin, ZBosonSigma, ErrSigma);
	    }
	}
    }
}


//------------------------------------------------------------------------------//
//	2 Graphs								//
//------------------------------------------------------------------------------//


void MSRootInterface::setGraphPoint(string id, float x, float y, float dx, float dy)
{
  MSGraphPointDescription point;

  point.X=x;
  point.Y=y;
  point.DX=dx;
  point.DY=dy;
  mapGraph[id].push_back(point);
}


void MSRootInterface::setGraphPoints(string id, MSEfficiency &msEfficiency, int Dimension)
{
  double Eff;
  double EffError;
  double meanOfBin1;
  double widthOfBin1;

  if ((Dimension==1) || (Dimension==2) || (Dimension==3))
    {
      for (int j=0; j<(int)	msEfficiency.getNumberOfBins(Dimension); j++)
	{
	  msEfficiency.getDimBinInfo(Dimension, j, meanOfBin1, widthOfBin1);
	  if (Dimension==1) msEfficiency.getEfficiency1(meanOfBin1, Eff, EffError);
	  if (Dimension==2) msEfficiency.getEfficiency2(meanOfBin1, Eff, EffError);
	  if (Dimension==3) msEfficiency.getEfficiency3(meanOfBin1, Eff, EffError);
	  setGraphPoint(id,meanOfBin1,Eff,widthOfBin1,EffError);
	}
    }
}


void MSRootInterface::setGraphPoints(string id, MSResolution &msResolution, string BinID, string sType)
{
  double PTResMean;
  double PTResWidth;
  double PT68Width;
  double ErrWidth;
  double meanOfBin;
  double widthOfBin;
	
  double ZBosonMean;
  double ErrMean;
  double ZBosonSigma;
  double ErrSigma;

  //if (mapGraph.find(id) == mapGraph.end())	return;
	
  for (int j=0; j<(int)msResolution.getNumberOfBins(BinID); j++)
    {
      msResolution.getDimBinInfo(BinID, j, meanOfBin, widthOfBin);
      if ((sType=="GausSigma") || (sType=="Sigma68") || (sType=="MeanValue"))
	{
	  msResolution.getResolution(BinID, meanOfBin, PTResMean, ErrMean, PT68Width, PTResWidth, ErrWidth);
	  if (sType=="GausSigma") setGraphPoint(id,meanOfBin,PTResWidth,widthOfBin,ErrWidth);
	  if (sType=="Sigma68") setGraphPoint(id,meanOfBin,PT68Width,widthOfBin,ErrWidth);
	  if (sType=="MeanValue") setGraphPoint(id,meanOfBin,PTResMean,widthOfBin,ErrMean);
	}	     
      if ((sType=="ZBoson_Width") || (sType=="ZBoson_Mean"))
	{
	  msResolution.getZBosonProperties(BinID, meanOfBin, ZBosonMean, ErrMean, ZBosonSigma, ErrSigma);
	  if (sType=="ZBoson_Mean")
	    {
	      setGraphPoint(id,meanOfBin,ZBosonMean,widthOfBin,ErrMean);
	    }
	  if (sType=="ZBoson_Width")
	    {
	      setGraphPoint(id,meanOfBin,ZBosonSigma,widthOfBin,ErrSigma);
	    }
	}
    }
}

void MSRootInterface::clearGraph(string id)
{
  if (mapGraph.find(id) == mapGraph.end())	return;
  mapGraph[id].clear();
}


void MSRootInterface::getRangeForGraph(string id, float &x1, float &y1, float &x2, float &y2)
{
  if (mapOutputDescription.find(id) == mapOutputDescription.end())	return;

  int nNumberOfEntries=(int)mapGraph[id].size();

  x1=0.0;
  y1=0.0;
  x2=1.0;
  y2=1.0;

  if (nNumberOfEntries<2) return;

  x1	= mapGraph[id][0].X;
  y1	= mapGraph[id][0].Y;
  x2	= mapGraph[id][0].X;
  y2	= mapGraph[id][0].Y;

  for (int i=0; i<nNumberOfEntries; i++)
    {
      if (x1>mapGraph[id][i].X) x1=mapGraph[id][i].X;
      if (x2<mapGraph[id][i].X) x2=mapGraph[id][i].X;
      if (y1>mapGraph[id][i].Y) y1=mapGraph[id][i].Y;
      if (y2<mapGraph[id][i].Y) y2=mapGraph[id][i].Y;
    }
  if (x1<0.0) x1*=1.2; else x1*=0.8;
  if (y1<0.0) y1*=1.2; else y1*=0.8;
  if (x2<0.0) x2*=0.8; else x2*=1.2;
  if (y2<0.0) y2*=0.8; else y2*=1.2;
}


void MSRootInterface::writeComparisionGraph(vector<string> vec_id, string outputName)
{
  if (m_FullDirectory!="")	gDirectory->Cd(m_FullDirectory.c_str());

  for (int k=0; k<(int)vec_id.size(); k++)
    {
      if (mapGraph.find(vec_id[k]) == mapGraph.end()) return;
      if (mapOutputDescription.find(vec_id[k]) == mapOutputDescription.end())	return;
    }

  float	rx1, rx2, ry1, ry2;

  for (int k=0; k<(int)vec_id.size(); k++)
    {
      if ((int)mapGraph[vec_id[k]].size()==0) return;
    }

  getRangeForGraph(vec_id[0], rx1, ry1, rx2, ry2);

  TCanvas *m_canvas = new TCanvas("m_canvas", "m_canvas",194,194,784,574);
  m_canvas->SetHighLightColor(2);
  m_canvas->Range(rx1, ry1, rx2, ry2);
  m_canvas->SetFillColor(0);
  m_canvas->SetBorderMode(0);
  m_canvas->SetBorderSize(1);
  m_canvas->SetGridx();
  m_canvas->SetGridy();
  m_canvas->SetLeftMargin(0.15);
  m_canvas->SetRightMargin(0.05);
  m_canvas->SetTopMargin(.1);
  m_canvas->SetBottomMargin(0.15);
  m_canvas->SetFrameFillStyle(0);
  m_canvas->SetFrameLineStyle(0);
  m_canvas->SetFrameBorderMode(0);
  m_canvas->SetFrameFillStyle(0);
  m_canvas->SetFrameLineStyle(0);
  m_canvas->SetFrameBorderMode(0);

  map<int, TGraphErrors*> map_TGraph;

  for (int k=0; k<(int)vec_id.size(); k++)
    {
      map_TGraph[k] = new TGraphErrors((int)mapGraph[vec_id[k]].size());
      map_TGraph[k]->SetName(vec_id[k].c_str());
      map_TGraph[k]->SetTitle(vec_id[k].c_str());
      map_TGraph[k]->SetFillColor(2+k);
      map_TGraph[k]->SetLineColor(102+k);
      map_TGraph[k]->SetMarkerColor(2+k);
      map_TGraph[k]->SetMarkerStyle(20+k);
      map_TGraph[k]->SetMarkerSize(1.6);
      for (int i=0; i<(int)mapGraph[vec_id[k]].size(); i++)
	{
	  map_TGraph[k]->SetPoint(i,mapGraph[vec_id[k]][i].X,mapGraph[vec_id[k]][i].Y);
	  map_TGraph[k]->SetPointError(i,mapGraph[vec_id[k]][i].DX,mapGraph[vec_id[k]][i].DY);
	}
    }

  TH1F *UnderLyingHisto1 = new TH1F(outputName.c_str(),outputName.c_str(),100,rx1,rx2);
  UnderLyingHisto1->SetMinimum(ry1);
  UnderLyingHisto1->SetMaximum(ry2);
  UnderLyingHisto1->SetDirectory(0);
  UnderLyingHisto1->SetStats(0);
  UnderLyingHisto1->SetFillStyle(0);
  UnderLyingHisto1->SetFillColor(0);
  UnderLyingHisto1->SetLineWidth(2);
  UnderLyingHisto1->SetMarkerStyle(7);
  UnderLyingHisto1->GetXaxis()->SetTitle(mapOutputDescription[vec_id[0]].xAxis.c_str());
  UnderLyingHisto1->GetXaxis()->SetLabelFont(42);
  UnderLyingHisto1->GetXaxis()->SetLabelSize(0.05);
  UnderLyingHisto1->GetXaxis()->SetTitleSize(0.06);
  UnderLyingHisto1->GetXaxis()->SetTitleFont(52);
  UnderLyingHisto1->GetYaxis()->SetTitle(mapOutputDescription[vec_id[0]].yAxis.c_str());
  UnderLyingHisto1->GetYaxis()->SetLabelFont(42);
  UnderLyingHisto1->GetYaxis()->SetLabelSize(0.05);
  UnderLyingHisto1->GetYaxis()->SetTitleSize(0.06);
  UnderLyingHisto1->GetYaxis()->SetTitleOffset(0.9);
  UnderLyingHisto1->GetYaxis()->SetTitleFont(52);
  UnderLyingHisto1->GetZaxis()->SetLabelSize(0.05);
  UnderLyingHisto1->GetZaxis()->SetTitleSize(0.06);  

  for (int k=0; k<(int)vec_id.size(); k++)
    {
      map_TGraph[k]->SetHistogram(UnderLyingHisto1);
      if (k==0)	map_TGraph[k]->Draw("ap");
      else map_TGraph[k]->Draw("psame");
     
    }

  TLegend *leg = new TLegend(0.20,0.75,0.95,0.95,NULL,"brNDC");
  leg->SetTextFont(52);
  leg->SetTextSize(0.04);
  leg->SetLineColor(1);
  leg->SetLineStyle(1);
  leg->SetLineWidth(1);
  leg->SetFillColor(0);
  leg->SetFillStyle(1001);

  map<int, TLegendEntry*> map_TLengedEntry;
  for (int k=0; k<(int)vec_id.size(); k++)
    {
      map_TLengedEntry[k]= leg->AddEntry(vec_id[k].c_str(),mapOutputDescription[vec_id[k]].name.c_str(),"p");
      map_TLengedEntry[k]->SetLineColor(1);
      map_TLengedEntry[k]->SetLineStyle(1);
      map_TLengedEntry[k]->SetLineWidth(2);
      map_TLengedEntry[k]->SetMarkerColor(2+k);
      map_TLengedEntry[k]->SetMarkerStyle(20+k);
      map_TLengedEntry[k]->SetMarkerSize(4);
      map_TLengedEntry[k]->SetTextAlign(12);
      map_TLengedEntry[k]->SetTextColor(1);
    }
  leg->Draw();

  m_canvas->Modified();
  m_canvas->cd();
  m_canvas->SetSelected(m_canvas);
  m_canvas->Write(outputName.c_str());
  m_canvas->Draw(outputName.c_str());
}

void MSRootInterface::writeDifferenceGraph(vector<string> vec_id, string outputName)
{
  if (vec_id.size()<2) return;

  if (m_FullDirectory!="")	gDirectory->Cd(m_FullDirectory.c_str());

  for (int k=0; k<(int)vec_id.size(); k++)
    {
      if (mapGraph.find(vec_id[k]) == mapGraph.end()) return;
      if (mapOutputDescription.find(vec_id[k]) == mapOutputDescription.end())	return;
    }

  float	rx1, rx2, ry1, ry2;

  for (int k=1; k<(int)vec_id.size(); k++)
    {
      if ((int)mapGraph[vec_id[k]].size()==0) return;
    }

  ry1=0.0;
  getRangeForGraph(vec_id[0], rx1, ry1, rx2, ry2);

  TCanvas *m_canvas = new TCanvas("m_canvas", "m_canvas",194,194,784,574);
  m_canvas->SetHighLightColor(2);
  m_canvas->Range(rx1, ry1, rx2, ry2);
  m_canvas->SetFillColor(0);
  m_canvas->SetBorderMode(0);
  m_canvas->SetBorderSize(1);
  m_canvas->SetGridx();
  m_canvas->SetGridy();
  m_canvas->SetLeftMargin(0.15);
  m_canvas->SetRightMargin(0.05);
  m_canvas->SetTopMargin(.1);
  m_canvas->SetBottomMargin(0.15);
  m_canvas->SetFrameFillStyle(0);
  m_canvas->SetFrameLineStyle(0);
  m_canvas->SetFrameBorderMode(0);
  m_canvas->SetFrameFillStyle(0);
  m_canvas->SetFrameLineStyle(0);
  m_canvas->SetFrameBorderMode(0);

  map<int, TGraphErrors*> map_TGraph;

  for (int k=1; k<(int)vec_id.size(); k++)
    {
      map_TGraph[k] = new TGraphErrors((int)mapGraph[vec_id[k]].size());
      map_TGraph[k]->SetName(vec_id[k].c_str());
      map_TGraph[k]->SetTitle(vec_id[k].c_str());
      map_TGraph[k]->SetFillColor(2+k);
      map_TGraph[k]->SetLineColor(102+k);
      map_TGraph[k]->SetMarkerColor(2+k);
      map_TGraph[k]->SetMarkerStyle(20+k);
      map_TGraph[k]->SetMarkerSize(1.6);
      for (int i=0; i<(int)mapGraph[vec_id[k]].size(); i++)
	{
	  map_TGraph[k]->SetPoint(i,mapGraph[vec_id[0]][i].X,mapGraph[vec_id[0]][i].Y-mapGraph[vec_id[k]][i].Y);
	  map_TGraph[k]->SetPointError(i,mapGraph[vec_id[0]][i].DX,sqrt(pow(mapGraph[vec_id[0]][i].DY,2)+pow(mapGraph[vec_id[k]][i].DY,2)));

	}
    }

  TH1F *UnderLyingHisto1 = new TH1F(outputName.c_str(),outputName.c_str(),100,rx1,rx2);
  UnderLyingHisto1->SetMinimum(ry1);
  UnderLyingHisto1->SetMaximum(ry2);
  UnderLyingHisto1->SetDirectory(0);
  UnderLyingHisto1->SetStats(0);
  UnderLyingHisto1->SetFillStyle(0);
  UnderLyingHisto1->SetLineWidth(2);
  UnderLyingHisto1->SetMarkerStyle(7);
  UnderLyingHisto1->GetXaxis()->SetTitle(mapOutputDescription[vec_id[0]].xAxis.c_str());
  UnderLyingHisto1->GetXaxis()->SetLabelFont(42);
  UnderLyingHisto1->GetXaxis()->SetLabelSize(0.05);
  UnderLyingHisto1->GetXaxis()->SetTitleSize(0.06);
  UnderLyingHisto1->GetXaxis()->SetTitleFont(52);
  UnderLyingHisto1->GetYaxis()->SetTitle(mapOutputDescription[vec_id[0]].yAxis.c_str());
  UnderLyingHisto1->GetYaxis()->SetLabelFont(42);
  UnderLyingHisto1->GetYaxis()->SetLabelSize(0.05);
  UnderLyingHisto1->GetYaxis()->SetTitleSize(0.06);
  UnderLyingHisto1->GetYaxis()->SetTitleOffset(0.9);
  UnderLyingHisto1->GetYaxis()->SetTitleFont(52);
  UnderLyingHisto1->GetZaxis()->SetLabelSize(0.05);
  UnderLyingHisto1->GetZaxis()->SetTitleSize(0.06);

  for (int k=1; k<(int)vec_id.size(); k++)
    {
      map_TGraph[k]->SetHistogram(UnderLyingHisto1);
      if (k==1)	map_TGraph[k]->Draw("ap");
      else		map_TGraph[k]->Draw("apsame");
    }

  TLegend *leg = new TLegend(0.40,0.75,0.95,0.95,NULL,"brNDC");
  leg->SetTextFont(52);
  leg->SetTextSize(0.04);
  leg->SetLineColor(1);
  leg->SetLineStyle(1);
  leg->SetLineWidth(1);
  leg->SetFillColor(0);
  leg->SetFillStyle(1001);

  map<int, TLegendEntry*> map_TLengedEntry;
  for (int k=1; k<(int)vec_id.size(); k++)
    {
      map_TLengedEntry[k]= leg->AddEntry(vec_id[k].c_str(),mapOutputDescription[vec_id[k]].name.c_str(),"p");
      map_TLengedEntry[k]->SetLineColor(1);
      map_TLengedEntry[k]->SetLineStyle(1);
      map_TLengedEntry[k]->SetLineWidth(2);
      map_TLengedEntry[k]->SetMarkerColor(2+k);
      map_TLengedEntry[k]->SetMarkerStyle(20+k);
      map_TLengedEntry[k]->SetMarkerSize(4);
      map_TLengedEntry[k]->SetTextAlign(12);
      map_TLengedEntry[k]->SetTextColor(1);
    }
  leg->Draw();

  m_canvas->Modified();
  m_canvas->cd();
  m_canvas->SetSelected(m_canvas);
  m_canvas->Write(outputName.c_str());
  m_canvas->Draw(outputName.c_str());
}


void MSRootInterface::writeGraph(string id1)
{
  if (m_FullDirectory!="")	gDirectory->Cd(m_FullDirectory.c_str());

  if (mapGraph.find(id1) == mapGraph.end()) 				return;
  if (mapOutputDescription.find(id1) == mapOutputDescription.end())	return;

  float	rx1, rx2, ry1, ry2;

  int nNumberOfEntries1=(int)mapGraph[id1].size();

  if (nNumberOfEntries1==0) return;

  getRangeForGraph(id1, rx1, ry1, rx2, ry2);

  TCanvas*c1 = new TCanvas("c1","gerrors2",194,194,784,574);

  TGraphErrors *gre1 = new TGraphErrors(nNumberOfEntries1);
  gre1->SetName(id1.c_str());
  gre1->SetTitle(id1.c_str());
  gre1->SetFillColor(30);
  gre1->SetLineColor(15);
  gre1->SetMarkerColor(1);
  gre1->SetMarkerStyle(22);
  gre1->SetMarkerSize(1.6);
  for (int i=0; i<nNumberOfEntries1; i++)
    {
      gre1->SetPoint(i,mapGraph[id1][i].X,mapGraph[id1][i].Y);
      gre1->SetPointError(i,mapGraph[id1][i].DX,mapGraph[id1][i].DY);
    }

  // draw a frame to define the range
  TH1F *hr = c1->DrawFrame(rx1, ry1, rx2, ry2);
  hr->GetXaxis()->SetTitle(mapOutputDescription[id1].xAxis.c_str());
  hr->GetXaxis()->SetLabelFont(42);
  hr->GetXaxis()->SetLabelSize(0.05);
  hr->GetXaxis()->SetTitleSize(0.06);
  hr->GetXaxis()->SetTitleFont(52);
  hr->GetYaxis()->SetTitle(mapOutputDescription[id1].yAxis.c_str());
  hr->GetYaxis()->SetLabelFont(42);
  hr->GetYaxis()->SetLabelSize(0.05);
  hr->GetYaxis()->SetTitleSize(0.06);
  hr->GetYaxis()->SetTitleOffset(0.9);
  hr->GetYaxis()->SetTitleFont(52);

  /*	gre1->SetHistogram(hr);
	gre1->Draw("LP");
	gre1->Write(mapOutputDescription[id1].name.c_str());
	gre1->Draw(mapOutputDescription[id1].name.c_str());*/

  gre1->Draw("");
  gre1->Draw("LP");
  c1->Modified();
  c1->cd();
  c1->Write(mapOutputDescription[id1].name.c_str());
  c1->Draw(mapOutputDescription[id1].name.c_str());
}

 
