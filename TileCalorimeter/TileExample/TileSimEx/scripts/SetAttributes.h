/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// #########################################
// Author: Dengfeng Zhang
// dengfeng.zhang@cern.ch
// #########################################

// Set the attributes of pads, histograms, graphs, hstack ......
// SetHistFillAttributes(): Set fillcolor and fillstyle of the histogram
// SetHistLineAttributes(): Set Line attributes of hist
// SetHistAxisAttributes(): Set Axis attributes of hist
// SetHistMarkerAttributes(): Set Axis attributes of hist
// SetHistAttributes(): Set attributes(fill, line axis marker) of hist
// SetPadFillAttributes): Set fill attributes of apd
// SetPadAttributes(): Set attributes(fill, line axis marker) of pad
// SetGraphAttributes(): Set attributes(fill, line axis marker) of graph
// SetGraphFillAttributes()
// SetGraphLineAttributes()
// SetGraphMarkerAttributes()
// SetGraphAxisAttributes()
// SetGraphErrorsAttributes(): Set attributes(fill, line axis marker) of grapherrors
// SetGraphErrorsFillAttributes()
// SetGraphErrorsLineAttributes()
// SetGraphErrorsMarkerAttributes()
// SetGraphErrorsAxisAttributes()
// SetGraphAsymmErrorsAttributes(): Set attributes(fill, line axis marker) of graphasymmerrors
// SetGraphAsymmErrorsFillAttributes()
// SetGraphAsymmErrorsLineAttributes()
// SetGraphAsymmErrorsMarkerAttributes()
// SetGraphAsymmErrorsAxisAttributes()
//
// SetHStackAttributes()
// std includes
#include <sstream>
#include <vector>
#include <fstream>
// include root
#include "TH1F.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TRandom3.h"
#include "TMultiGraph.h"
//
/*
int DrawATLASLabels(float xstart, float ystart, int labeltype)
{
  TLatex m_latex;
  m_latex.SetTextSize(0.03) ;
  m_latex.SetTextColor(kBlack) ;
  m_latex.SetTextFont(72) ; 
  m_latex.DrawLatex(xstart, ystart, "ATLAS") ;
  m_latex.SetTextFont(42) ;
  float spacing = 0.15 ;
  switch(labeltype)
  { 
    case 0:
          break ;
    case 1:
          m_latex.DrawLatex(xstart+spacing, ystart, "Preliminary") ;
          break ;
    case 2:
          m_latex.DrawLatex(xstart+spacing, ystart, "Internal") ;
          break ;
    case 3:
          m_latex.DrawLatex(xstart+spacing, ystart, "Simulation Preliminary") ;
          break ;
    case 4:
          m_latex.DrawLatex(xstart+spacing, ystart, "Work in Progress") ;
          break ; 
   default: break ;
  }
  return 0;
}
*/
int SetHistFillAttributes(TH1F* hist, int fillstyle=1001, int fillcolor=0)
{
  hist->SetFillStyle(fillstyle) ;
  hist->SetFillColor(fillcolor) ;
  return 0 ;
}

int SetHistLineAttributes(TH1F* hist, int linestyle=1, int linewidth=2, int linecolor=1)
{
  hist->SetLineColor(linecolor) ;
  hist->SetLineWidth(linewidth) ;
  hist->SetLineStyle(linestyle) ; 
  return 0 ;
}

int SetHistAxisAttributes(TH1F* hist, 
    float xlabelsize=0.05, float xtitleoffset=1.4, float xtitlesize=0.05,
    float ylabelsize=0.05, float ytitleoffset=1.4, float ytitlesize=0.05)
{
  hist->GetXaxis()->SetTitleSize(xtitlesize) ;
  hist->GetXaxis()->SetTitleOffset(xtitleoffset) ;
  hist->GetXaxis()->SetLabelSize(xlabelsize) ;
  hist->GetYaxis()->SetTitleSize(ytitlesize) ;
  hist->GetYaxis()->SetTitleOffset(ytitleoffset) ;
  hist->GetYaxis()->SetLabelSize(ylabelsize) ;
  return 0 ;
}

int SetHistMarkerAttributes(TH1F* hist, int markerstyle=20, int markercolor=1, int markersize=1)
{
  hist->SetMarkerStyle(markerstyle) ;
  hist->SetMarkerColor(markercolor) ;
  hist->SetMarkerSize(markersize) ;
  return 0 ;
} 
int SetHistAttributes(TH1F* hist, int linestyle=1, int linewidth=2, int linecolor=1, 
                      int fillstyle=1001, int fillcolor=0,
                      int markerstyle=20, int markercolor=1, int markersize=1,
                      float xlabelsize=0.05, float xtitleoffset=1.4, float xtitlesize=0.05,
                      float ylabelsize=0.05, float ytitleoffset=1.4, float ytitlesize=0.05,
                      bool displaystats=false)
{
  SetHistFillAttributes(hist, fillstyle, fillcolor) ;
  SetHistAxisAttributes(hist,xlabelsize, xtitleoffset, xtitlesize, ylabelsize, ytitleoffset, ytitlesize) ;
  SetHistLineAttributes(hist, linestyle, linewidth, linecolor) ;
  SetHistMarkerAttributes(hist, markerstyle, markercolor, markersize) ;
  hist->SetStats(displaystats) ;

  return 0 ;
}

int SetPadFillAttributes(TPad* pad, int fillstyle=0, float fillcolor=0)
{
  pad->SetFillStyle(fillstyle) ;
  pad->SetFillColor(fillcolor) ;
  return 0 ;
}

int SetPadAttributes(TPad* pad, int fillstyle=0, int fillcolor=0, 
 float leftmargin=0.15, float rightmargin=0.05, float bottommargin=0.15, float topmargin=0.05,
int bordermode=0, bool logx=false, bool logy=true, bool settickx=true, bool setticky=true)
{
  SetPadFillAttributes(pad, fillstyle, fillcolor) ;
  pad->SetMargin(leftmargin, rightmargin, bottommargin, topmargin) ;
  pad->SetBorderMode(bordermode) ;

  pad->SetLogx(logx) ;
  pad->SetLogy(logy) ;

  pad->SetTickx(settickx) ;
  pad->SetTicky(setticky) ;
  return 0 ;
}
/*
int SetLatexAttributes(TLatex* latex)
{
  return 0 ;
}

int SetPaveTextAttributes(TPaveText* pavetext)
{
  return 0 ;
}

int SetLegendAttributes(TLegend* legend)
{
  return 0 ;
}
*/
int  SetGraphLineAttributes(TGraph* gr, int linestyle=1, int linewidth=2, int linecolor=1)
{
  gr->SetLineColor(linecolor) ;
  gr->SetLineWidth(linewidth) ;
  gr->SetLineStyle(linestyle) ;
  return 0 ;
}

int SetGraphFillAttributes(TGraph* gr, int fillstyle=1001, int fillcolor=0)
{
  gr->SetFillStyle(fillstyle) ;
  gr->SetFillColor(fillcolor) ;
  return 0 ;
}

int SetGraphMarkerAttributes(TGraph* gr, int markerstyle=20, int markercolor=1, int markersize=1)
{
  gr->SetMarkerStyle(markerstyle) ;
  gr->SetMarkerColor(markercolor) ;
  gr->SetMarkerSize(markersize) ; 
  return 0 ;
}

int SetGraphAxisAttributes(TGraph* gr,
                    float xlabelsize=0.05, float xtitleoffset=1.4, float xtitlesize=0.05,
                    float ylabelsize=0.05, float ytitleoffset=1.4, float ytitlesize=0.05)
{
  gr->GetXaxis()->SetTitleSize(xtitlesize) ;
  gr->GetXaxis()->SetTitleOffset(xtitleoffset) ;
  gr->GetXaxis()->SetLabelSize(xlabelsize) ;
  gr->GetYaxis()->SetTitleSize(ytitlesize) ;
  gr->GetYaxis()->SetTitleOffset(ytitleoffset) ;
  gr->GetYaxis()->SetLabelSize(ylabelsize) ;
  return 0 ;
}

int SetGraphAttributes(TGraph* gr, int linestyle=1, int linewidth=2, int linecolor=1,
                      int fillstyle=1001, int fillcolor=0,
                      int markerstyle=20, int markercolor=1, int markersize=1,
                      float xlabelsize=0.05, float xtitleoffset=1.4, float xtitlesize=0.05,
                      float ylabelsize=0.05, float ytitleoffset=1.4, float ytitlesize=0.05)
{
  SetGraphLineAttributes(gr, linestyle, linewidth, linecolor) ;
  SetGraphFillAttributes(gr, fillstyle, fillcolor) ;
  SetGraphMarkerAttributes(gr, markerstyle, markercolor, markersize) ;
  SetGraphAxisAttributes(gr, xlabelsize, xtitleoffset, xtitlesize, ylabelsize, ytitleoffset, ytitlesize) ;
  return 0 ;
}

int  SetGraphErrorsLineAttributes(TGraphErrors* gre, int linestyle=1, int linewidth=2, int linecolor=1)
{
  gre->SetLineColor(linecolor) ;
  gre->SetLineWidth(linewidth) ;
  gre->SetLineStyle(linestyle) ;
  return 0 ;
}

int SetGraphErrorsFillAttributes(TGraph* gre, int fillstyle=1001, int fillcolor=0)
{
  gre->SetFillStyle(fillstyle) ;
  gre->SetFillColor(fillcolor) ;
  return 0 ;
}

int SetGraphErrorsMarkerAttributes(TGraph* gre, int markerstyle=20, int markercolor=1, int markersize=1)
{
  gre->SetMarkerStyle(markerstyle) ;
  gre->SetMarkerColor(markercolor) ;
  gre->SetMarkerSize(markersize) ; 
  return 0 ;
}

int SetGraphErrorsAxisAttributes(TGraph* gre,
                    float xlabelsize=0.05, float xtitleoffset=1.4, float xtitlesize=0.05,
                    float ylabelsize=0.05, float ytitleoffset=1.5, float ytitlesize=0.05)
{
  gre->GetXaxis()->SetTitleSize(xtitlesize) ;
  gre->GetXaxis()->SetTitleOffset(xtitleoffset) ;
  gre->GetXaxis()->SetLabelSize(xlabelsize) ;
  gre->GetYaxis()->SetTitleSize(ytitlesize) ;
  gre->GetYaxis()->SetTitleOffset(ytitleoffset) ;
  gre->GetYaxis()->SetLabelSize(ylabelsize) ;
  return 0 ;
}

int SetGraphErrorsAttributes(TGraphErrors* gre,
          int linestyle=1, int linewidth=2, int linecolor=1,
          int fillstyle=1001, int fillcolor=0,
          int markerstyle=20, int markercolor=1, int markersize=1,
          float xlabelsize=0.05, float xtitleoffset=1.4, float xtitlesize=0.05,
          float ylabelsize=0.05, float ytitleoffset=1.5, float ytitlesize=0.05)
{
  SetGraphErrorsLineAttributes(gre, linestyle, linewidth, linecolor) ;
  SetGraphErrorsFillAttributes(gre, fillstyle, fillcolor) ;
  SetGraphErrorsMarkerAttributes(gre, markerstyle, markercolor, markersize) ;
  SetGraphErrorsAxisAttributes(gre, xlabelsize, xtitleoffset, xtitlesize, ylabelsize, ytitleoffset, ytitlesize) ;
  return 0 ;
}

int SetMGraphAttributes(TMultiGraph *mg, 
          float xlabelsize=0.05, float xtitleoffset=1.4, float xtitlesize=0.05,
          float ylabelsize=0.05, float ytitleoffset=1.5, float ytitlesize=0.05)
{
  mg->GetXaxis()->SetTitleSize(xtitlesize) ;
  mg->GetXaxis()->SetTitleOffset(xtitleoffset) ;
  mg->GetXaxis()->SetLabelSize(xlabelsize) ;
  mg->GetYaxis()->SetTitleSize(ytitlesize) ;
  mg->GetYaxis()->SetTitleOffset(ytitleoffset) ;
  mg->GetYaxis()->SetLabelSize(ylabelsize) ; 
  return 0 ;
}

int SetGraphAsymmErrorsLineAttributes(TGraphAsymmErrors* grae, int linestyle=1, int linewidth=2, int linecolor=1)
{
  grae->SetLineColor(linecolor) ;
  grae->SetLineWidth(linewidth) ;
  grae->SetLineStyle(linestyle) ;
  return 0 ;
}

int SetGraphAsymmErrorsFillAttributes(TGraphAsymmErrors* grae, int fillstyle=1001, int fillcolor=0)
{
  grae->SetFillStyle(fillstyle) ;
  grae->SetFillColor(fillcolor) ;
  return 0 ;
}

int SetGraphAsymmErrorsMarkerAttributes(TGraphAsymmErrors* grae, int markerstyle=20, int markercolor=1, int markersize=1)
{
  grae->SetMarkerStyle(markerstyle) ;
  grae->SetMarkerColor(markercolor) ;
  grae->SetMarkerSize(markersize) ; 
  return 0 ;
}

int SetGraphAsymmErrorsAxisAttributes(TGraphAsymmErrors* grae,
                    float xlabelsize=0.03, float xtitleoffset=1., float xtitlesize=0.04,
                    float ylabelsize=0.03, float ytitleoffset=1., float ytitlesize=0.04)
{
  grae->GetXaxis()->SetTitleSize(xtitlesize) ;
  grae->GetXaxis()->SetTitleOffset(xtitleoffset) ;
  grae->GetXaxis()->SetLabelSize(xlabelsize) ;
  grae->GetYaxis()->SetTitleSize(ytitlesize) ;
  grae->GetYaxis()->SetTitleOffset(ytitleoffset) ;
  grae->GetYaxis()->SetLabelSize(ylabelsize) ;
  return 0 ;
}

int SetGraphAsymmErrorsAttributes(TGraphAsymmErrors* grae,
                      int linestyle=1, int linewidth=2, int linecolor=1,
                      int fillstyle=1001, int fillcolor=0,
                      int markerstyle=20, int markercolor=1, int markersize=1,
                      float xlabelsize=0.03, float xtitleoffset=1., float xtitlesize=0.04,
                      float ylabelsize=0.03, float ytitleoffset=1., float ytitlesize=0.04)
{
  SetGraphAsymmErrorsLineAttributes(grae, linestyle, linewidth, linecolor) ;
  SetGraphAsymmErrorsFillAttributes(grae, fillstyle, fillcolor) ;
  SetGraphAsymmErrorsMarkerAttributes(grae, markerstyle, markercolor, markersize) ;
  SetGraphAsymmErrorsAxisAttributes(grae, xlabelsize, xtitleoffset, xtitlesize, ylabelsize, ytitleoffset, ytitlesize) ;
  return 0 ;
}

int SetHStackAttributes(THStack* hstack, 
    float xlabelsize=0.04, float xtitleoffset=1., float xtitlesize=0.04,
    float ylabelsize=0.04, float ytitleoffset=1., float ytitlesize=0.04)
{
  hstack->GetXaxis()->SetTitleSize(xtitlesize) ;
  hstack->GetXaxis()->SetTitleOffset(xtitleoffset) ;
  hstack->GetXaxis()->SetLabelSize(xlabelsize) ;
  hstack->GetYaxis()->SetTitleSize(ytitlesize) ;
  hstack->GetYaxis()->SetTitleOffset(ytitleoffset) ;
  hstack->GetYaxis()->SetLabelSize(ylabelsize) ;
  return 0 ;
}
