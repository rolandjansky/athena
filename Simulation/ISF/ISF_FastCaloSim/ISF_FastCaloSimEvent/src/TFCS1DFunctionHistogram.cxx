/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

using namespace std;
#include "ISF_FastCaloSimEvent/TFCS1DFunctionHistogram.h"
#include "TMath.h"
#include <iostream>

//=============================================
//======= TFCS1DFunctionHistogram =========
//=============================================

TFCS1DFunctionHistogram::TFCS1DFunctionHistogram(TH1* hist, int verbose, double cut_maxdev)
{
  Initialize(hist, verbose,cut_maxdev);
}

void TFCS1DFunctionHistogram::Initialize(TH1* hist, int verbose,double cut_maxdev)
{

  smart_rebin_loop(hist, verbose,cut_maxdev);

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

  // cleanup
  delete[] histoVals;

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

  delete[] bins;

  return h_out;

}


void TFCS1DFunctionHistogram::smart_rebin_loop(TH1* hist, int verbose, double cut_maxdev)
{

  m_HistoContents.clear();
  m_HistoBorders.clear();

  double maxdev=-1;
  double change=0.0001; double step=0.00005;
  TH1D* h_input=0; TH1D* h_output;
  int i=0;
  while(1)
  {
    if(i==0) h_input=(TH1D*)hist->Clone("h_input");

    TH1D* h_out=smart_rebin(h_input,change); h_out->SetName("h_out");

    maxdev=get_maxdev(hist,h_out);
    if(verbose==2) cout<<"Iteration nr. "<<i<<" change "<<change<<" bins "<<h_out->GetNbinsX()<<"-> maxdev="<<maxdev<<endl;

    if(maxdev<cut_maxdev)
    {
      h_input=(TH1D*)h_out->Clone("h_input");
      change+=step;
      i++;
    }
    if(maxdev>cut_maxdev)
    {
      change-=step;
      h_output=(TH1D*)h_input->Clone("h_output");
      break;
    }

    delete h_out;

  }

  cout<<"Info: Rebinned histogram has "<<h_output->GetNbinsX()<<" bins."<<endl;

  //correct normalization (last bin content must be 1)
  double sf=1.0/h_output->GetBinContent(h_output->GetNbinsX());
  h_output->Scale(sf);

  //store:

  for(int b=1;b<=h_output->GetNbinsX();b++)
    m_HistoContents.push_back(h_output->GetBinContent(b));

  for(int b=1;b<=h_output->GetNbinsX();b++)
    m_HistoBorders.push_back((float)h_output->GetBinLowEdge(b));

  m_HistoBorders.push_back((float)h_output->GetXaxis()->GetXmax());

}

TH1D* TFCS1DFunctionHistogram::smart_rebin(TH1D* h_input, double change)
{

  //int debug=0;

  vector<double> content;
  vector<double> binborder;

  content.clear();
  binborder.clear();

  binborder.push_back(h_input->GetXaxis()->GetXmin());

  double sum=0;
  double sumwidth=0;
  int count=0;
  for(int b=1;b<h_input->GetNbinsX();b++)
  {
    double thisBin=h_input->GetBinContent(b);
    double nextBin=h_input->GetBinContent(b+1);
    double width=h_input->GetBinWidth(b);
    if(fabs(nextBin/thisBin-1.0)>=change)
    {
      sum+=thisBin*width;
      sumwidth+=width;
      count++;
      binborder.push_back(h_input->GetBinLowEdge(b+1));

      //divide the sum by the number of summed bins:
      //content.push_back(sum/(double)count);
      content.push_back(sum/sumwidth);

      //Reset the sum:
      sum=0;
      sumwidth=0;
      count=0;
    }
    else
    {
      //do not add this bin to the new vector of bins, but add to the sum
      sum+=thisBin*width;
      sumwidth+=width;
      count++;
    }
  }
  //last bin is special:
  binborder.push_back(h_input->GetXaxis()->GetXmax());
  if(count==0) //that means the previous-to-last bin was written out, so simpy add the last one:
  {
    content.push_back(h_input->GetBinContent(h_input->GetNbinsX()));
  }
  if(count>0)
  {
    sum+=h_input->GetBinContent(h_input->GetNbinsX())*h_input->GetBinWidth(h_input->GetNbinsX());
    sumwidth+=h_input->GetBinWidth(h_input->GetNbinsX());
    count++;
    content.push_back(sum/sumwidth);
  }

  double* bins=new double[content.size()+1];
  for(unsigned int i=0;i<binborder.size();i++)
    bins[i]=binborder[i];

  TH1D* h_out=new TH1D("h_out","h_out",content.size(),bins);
  for(unsigned int b=1;b<=content.size();b++)
    h_out->SetBinContent(b,content[b-1]);

  delete [] bins;

  return h_out;

}

double TFCS1DFunctionHistogram::rnd_to_fct(double rnd)
{

  //double value1=sample_from_histovalues(rnd);
  double value2=get_inverse(rnd);
  return value2;

}


double TFCS1DFunctionHistogram::linear(double x1,double x2,double y1,double y2,double x)
{
  double y=-1;

  double eps=0.0000000001;
  if((x2-x1)<eps) y=y1;
  else
  {
    double m=(y2-y1)/(x2-x1);
    double n=y1-m*x1;
    y=m*x+n;
  }

  return y;
}


double TFCS1DFunctionHistogram::get_inverse(double rnd)
{

  TH1* hist=vector_to_histo(); hist->SetName("hist");

  double value = 0.;
  for(int b=1;b<=hist->GetNbinsX();b++)
  {
    double y=hist->GetBinContent(b);
    if(y>rnd && b!=1)
    {
      //use linear extrapolation to get exact x value
      double x1=hist->GetBinCenter(b-1);
      double x2=hist->GetBinCenter(b);
      double y1=hist->GetBinContent(b-1);
      double y2=hist->GetBinContent(b);
      double y=linear(y1,y2,x1,x2,rnd);
      //cout<<"x1 "<<x1<<" x2 "<<x2<<" y1 "<<y1<<" y2 "<<y2 <<" rnd "<<rnd<<" value "<<y<<endl;
      value=y;
      b=hist->GetNbinsX()+1;
    }
  }

  return value;
}
