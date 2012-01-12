/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"
#include "TrigTauPerformAnalysis/TrigTauHistogram.h"
#include <iostream>
using namespace std;

TrigTauHistogram::Definition TrigTauHistogram::histDefinitions[TrigTauHistogram::NONE] = 
  {
    Definition( PTHIST,  "pt", "pT, MeV", 50, 0,100000),
    Definition( ETAHIST, "eta","#eta",    61, -3,3),
    Definition( PHIHIST, "phi","#phi",    65, -3.2,3.2),
    Definition( EXTERNAL, "external","external var",  50, 0,100000)
  };

TrigTauHistogram::TrigTauHistogram(HISTTYPE  type )
  : m_type(type), m_externalVar(0)
{}
 
TrigTauHistogram::~TrigTauHistogram() {} // we do not care about mem leak, but may be we should?

void TrigTauHistogram::doHist( HISTTYPE  type) // change histogramming type!
{
  m_type = type;
  // clean old histograms
  for(std::vector<TH1F*>::iterator ihist = m_hist.begin();
      ihist != m_hist.end(); ++ihist)
    { delete *ihist; *ihist=0; }
  m_hist.erase(m_hist.begin(), m_hist.end());
  return ;
}


void TrigTauHistogram::setHistLimits(HISTTYPE type, int nbin, float xmin, float xmax)
{
  histDefinitions[type].nbin = nbin;
  histDefinitions[type].xmin = xmin;
  histDefinitions[type].xmax = xmax;
}
void  TrigTauHistogram::setHistLimits(HISTTYPE type, TString baseName, TString xaxistext, int nbin, float xmin, float xmax)
{
  histDefinitions[type].baseName = baseName;
  histDefinitions[type].xaxistext = xaxistext;
  histDefinitions[type].nbin = nbin;
  histDefinitions[type].xmin = xmin;
  histDefinitions[type].xmax = xmax;
}

int  TrigTauHistogram::book(const char * name, const char * title)
{
  if( m_type == NONE ){ cout << "can not book NONE type of histogram\n"; return 1;}
int index =  m_hist.size();

  TString histname(name);
  char number[20];
  sprintf(number,"_%d", index);
  histname.Append(number);
  histname.Append("_");
  histname.Append(histDefinitions[m_type].baseName);
  int nbin  = histDefinitions[m_type].nbin;
  double xmin  = histDefinitions[m_type].xmin;
  double xmax  = histDefinitions[m_type].xmax;
  gROOT->cd();
  //TH1F * h1 = new TH1F("h1","",  nbin, xmin,xmax);
  TH1F * h1 = new TH1F(histname, TString(title), nbin, xmin,xmax);
  h1->GetXaxis()->SetTitle(histDefinitions[m_type].xaxistext);
  h1->GetYaxis()->SetTitle("Nevents");
  h1->Sumw2();
  h1->SetLineColor(index+1);

  m_hist.push_back(h1);

  return index; // return index
}

void TrigTauHistogram::fill(unsigned int index, float var)
{
  if( m_type == EXTERNAL ) var = m_externalVar;
  if( index < m_hist.size()  ){ m_hist[index]->Fill(var);}
} 

void TrigTauHistogram::fill(unsigned int index, const TLorentzVector & vec)
{
  if( index >= m_hist.size()  )return;

  float var = 0;
  switch ( m_type ) {
  case PTHIST  : var = vec.Pt(); break;
  case ETAHIST : var = vec.Eta(); break;
  case PHIHIST : var = vec.Phi(); break;
  default : return;
  }

  m_hist[index]->Fill(var); 
} 

int TrigTauHistogram::nHists()const
{
  return  m_hist.size();
}


int TrigTauHistogram::plotHist(unsigned int index, const char * opt)
{
  if( index < m_hist.size()  ) m_hist[index]->Draw(opt);
  return 0;
}

int TrigTauHistogram::plotEfficiency(unsigned int indexAfterCut, 
				     unsigned int indexBeforeCut, const char * opt)
{
  if( indexAfterCut >= m_hist.size() || indexBeforeCut >= m_hist.size() ) return 1;
  TString name=  m_hist[indexAfterCut]->GetName();
  name.Append("_eff");
  TH1F * clone = (TH1F*)m_hist[indexAfterCut]->Clone(name);
  //clone->Divide( m_hist[indexBeforeCut]);

  unsigned int nbin = clone->GetXaxis()->GetNbins();
  for( unsigned int i = 1; i <= nbin ; ++i )
    {
      double Nbefore = m_hist[indexBeforeCut]->GetBinContent(i);
      if( Nbefore < 0.1e-10) continue;
      double Nafter = m_hist[indexAfterCut]->GetBinContent(i);
      double eff = Nafter/Nbefore;
      double effError = sqrt(eff*(1-eff)/Nbefore);
      clone->SetBinContent(i,eff);
      clone->SetBinError(i,effError);
    }

  
  clone->Draw(opt);

  return 0;
}


int TrigTauHistogram::plotAll()
{
  if(  m_hist.empty() ) return 1;
  std::vector<TH1F*>::iterator ihist = m_hist.begin();
  (*ihist)->Draw(); 
  ihist++;

  for( ; ihist != m_hist.end(); ++ihist )
    {
      (*ihist)->Draw("same");
    }

  return 0;
}

int TrigTauHistogram::plotAllEfficiencies(int opt)
{
  unsigned int nHist =  m_hist.size();
  int color = 1;
  for(unsigned int i = 1; i <nHist;  ++i)
    {
      color++;
    if( i == 1 )    plotEfficiency(i,0,"");
    else{
      if( opt == -1 ) plotEfficiency(i,i-1,"same");
      else            plotEfficiency(i,0,"same");
    }
    }
  return 0;
}
