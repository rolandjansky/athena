/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCS1DFunctionHistogram.h"
#include "TMath.h"
#include <iostream>

using namespace std;

//=============================================
//======= TFCSFunctionRegression =========
//=============================================

TFCS1DFunctionHistogram::TFCS1DFunctionHistogram()
{
}

TFCS1DFunctionHistogram::TFCS1DFunctionHistogram(TH1* hist)
{
  Initialize(hist);
}

void TFCS1DFunctionHistogram::Initialize(TH1* hist)
{
	
 //smart rebinning
	
 int do_histo_iteration=1;
 double maxdev=100;
 int iter=0; //0
 while(maxdev>5 && do_histo_iteration && iter<=10)
 {
  smart_rebinning(hist,1.0-iter*0.1);
  TH1* h_rebinned=vector_to_histo();
  maxdev=get_maxdev(hist,h_rebinned);
  delete h_rebinned;
  cout<<"Histo Iteration "<<iter<<" maxdev "<<maxdev<<endl;
  iter++;
 }
  
 //m_histo=(TH1*)hist->Clone("m_histo");
  
  //test the sampling
  /*
  TH1D* h_test=new TH1D("h_test","h_test",5000,m_histo->GetXaxis()->GetXmin(),m_histo->GetXaxis()->GetXmax());
  for(int i=0;i<10000;i++)
  {
   double random=myRandom->Uniform(1);
   double *histoVals=histo_to_array();
   double value=sample_from_histo(random,histoVals);
   h_test->Fill(value);
  }
  TH1* h_cum=get_cumul(h_test);
  TFile* testfile=new TFile("samplingtest.root","RECREATE");
  testfile->Add(h_test);
  testfile->Add(h_cum);
  testfile->Add(m_histo);
  testfile->Write();
  */
  
}


double* TFCS1DFunctionHistogram::histo_to_array(TH1* hist)
{
 
 TH1D* h_clone=(TH1D*)hist->Clone("h_clone");
 h_clone->Scale(1.0/h_clone->Integral());
 
 double *histoVals=new double[h_clone->GetNbinsX()];
 histoVals[0]=h_clone->GetBinContent(1);
 for (int i=1; i<h_clone->GetNbinsX(); i++)
 {
  histoVals[i]=histoVals[i-1] + h_clone->GetBinContent(i+1);
 }
 return histoVals;
 
}

double TFCS1DFunctionHistogram::sample_from_histo(TH1* hist, double random)
{
 
 double* histoVals=histo_to_array(hist);
 double value=0.0;
 int chosenBin = (int)TMath::BinarySearch(hist->GetNbinsX(), histoVals, random);
 value = hist->GetBinCenter(chosenBin+2);
 
 return value;
 
}

double TFCS1DFunctionHistogram::sample_from_histovalues(double random)
{ 
 double value=0.0;

 TH1* hist=vector_to_histo(); hist->SetName("hist");
 double *histoVals=histo_to_array(hist);
 int chosenBin = (int)TMath::BinarySearch(hist->GetNbinsX(), histoVals, random);
 value = hist->GetBinCenter(chosenBin+2);
 
 return value;
}



TH1* TFCS1DFunctionHistogram::vector_to_histo()
{
 
 double *bins=new double[m_HistoBorders.size()];
 for(unsigned int i=0;i<m_HistoBorders.size();i++)
  bins[i]=m_HistoBorders[i];

 TH1* h_out=new TH1D("h_out","h_out",m_HistoBorders.size()-1,bins);
 for(int b=1;b<=h_out->GetNbinsX();b++)
 	h_out->SetBinContent(b,m_HistoContents[b-1]);
 
 return h_out;
 
}

void TFCS1DFunctionHistogram::smart_rebinning(TH1* hist, double change /*in percent*/ )
{
 
 int debug=0;
 
 //do the rebin and save the bins in a vector
 vector<double> content;
 vector<double> binborder;
 
 binborder.push_back(hist->GetXaxis()->GetXmin());
 
 change=change/100.0;
 double sum=0;
 int count=0;
 for(int b=1;b<hist->GetNbinsX();b++)
 {
  double thisBin=hist->GetBinContent(b);
  double nextBin=hist->GetBinContent(b+1);
  if(debug) cout<<"b "<<b<<" begin, thisBin "<<thisBin<<" nextBin "<<nextBin<<" sum "<<sum<<endl;
  if(fabs(nextBin/thisBin-1.0)>=change)
  {
   sum+=thisBin;
   count++;
 	 binborder.push_back(hist->GetBinCenter(b)+0.5*hist->GetBinWidth(b));
 	 //divide the sum by the number of summed bins:
 	 content.push_back(sum/(double)count);
   //Reset the sum:
   sum=0;
   count=0;
  }
  if(fabs(nextBin/thisBin-1.0)<change)
  {
   //do not add this bin to the new vector of bins, but add to the sum
   sum+=thisBin;
   count++;
  }
 }
 //last bin is special:
 binborder.push_back(hist->GetXaxis()->GetXmax());
 if(count==0) //that means the previous-to-last bin was written out, so simpy add the last one:
 {
  content.push_back(hist->GetBinContent(hist->GetNbinsX()));
 }
 if(count>0)
 {
 	sum+=hist->GetBinContent(hist->GetNbinsX());
 	count++;
 	content.push_back(sum/(double)count);
 }
 
 //safe into the member variables:
 
 m_HistoContents.clear();
 m_HistoBorders.clear();
 
 for(unsigned int i=0;i<content.size();i++)
  m_HistoContents.push_back((float)content[i]);
 for(unsigned int i=0;i<binborder.size();i++)
  m_HistoBorders.push_back((float)binborder[i]);
 
}


double TFCS1DFunctionHistogram::rnd_to_fct(double rnd)
{

  double value=sample_from_histovalues(rnd);
  return value;

}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS1DFunctionHistogram)

