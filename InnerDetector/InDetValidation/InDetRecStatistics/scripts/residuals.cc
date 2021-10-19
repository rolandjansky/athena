/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: residuals.cc,v 1.10 2008-07-23 20:51:23 adye Exp $
#include <string>

#include "TROOT.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TCut.h"

using std::string;

void residuals(TTree *ntuple, const string & plotprefix="", const string & plotext="",
               const TCut hit_cut="")
{
  // turn on outlier display, increase some text sizes
  TStyle plotStyle ("PlotStyle", "My Plot Style");
  plotStyle.cd();
  plotStyle.SetOptStat("mro");
  plotStyle.SetTitleFontSize(0.1);
  plotStyle.SetStatFontSize(0.08);

  TCanvas *c_respix = new TCanvas("c_respix","residuals in pixel detector",800,800);
  TCanvas *c_ressct = new TCanvas("c_ressct","residuals in SCT",1);
  TCanvas *c_restrt = new TCanvas("c_restrt","residuals in TRT",1);
  TCanvas *c_hitpix = new TCanvas("c_hitpix","hits in pixel detector",800,400);
  TCanvas *c_hitsct = new TCanvas("c_hitsct","hits in SCT",800,400);
  TCanvas *c_hittrt = new TCanvas("c_hittrt","hits in TRT",800,400);

  c_respix->Divide(2,3);
  c_ressct->Divide(2,4);
  c_restrt->Divide(2,2);
  c_hitpix->Divide(2,1);
  c_hitsct->Divide(2,1);
  c_hittrt->Divide(2,1);

  const int s_bins       = 50;
  const double s_resmin   = -100;
  const double s_resmax   =  100;
  const double s_resmin2   = -1000;
  const double s_resmax2   =  1000;
  //  const double s_resmin3   = -1000;
  //  const double s_resmax3   =  1000;

  TH1F *xrespix1 = new TH1F("xrespix1",             "x-residuals pixel layer 1",        s_bins, s_resmin, s_resmax);
  TH1F *xrespix2 = new TH1F("xrespix2",             "x-residuals pixel layer 2",        s_bins, s_resmin, s_resmax);
  TH1F *xrespix3 = new TH1F("xrespix3",             "x-residuals pixel layer 3",        s_bins, s_resmin, s_resmax);
  TH1F *yrespix1 = new TH1F("yrespix1",             "y-residuals pixel layer 1",        s_bins, s_resmin2, s_resmax2);
  TH1F *yrespix2 = new TH1F("yrespix2",             "y-residuals pixel layer 2",        s_bins, s_resmin2, s_resmax2);
  TH1F *yrespix3 = new TH1F("yrespix3",             "y-residuals pixel layer 3",        s_bins, s_resmin2, s_resmax2);

  TH1F *xrespix1nt = new TH1F("xrespix1nt",             "x-residuals pixel layer 1",        s_bins, s_resmin, s_resmax);
  TH1F *xrespix2nt = new TH1F("xrespix2nt",             "x-residuals pixel layer 2",        s_bins, s_resmin, s_resmax);
  TH1F *xrespix3nt = new TH1F("xrespix3nt",             "x-residuals pixel layer 3",        s_bins, s_resmin, s_resmax);
  TH1F *yrespix1nt = new TH1F("yrespix1nt",             "y-residuals pixel layer 1",        s_bins, s_resmin2, s_resmax2);
  TH1F *yrespix2nt = new TH1F("yrespix2nt",             "y-residuals pixel layer 2",        s_bins, s_resmin2, s_resmax2);
  TH1F *yrespix3nt = new TH1F("yrespix3nt",             "y-residuals pixel layer 3",        s_bins, s_resmin2, s_resmax2);

  TH1F *xressct1 = new TH1F("xressct1",             "x-residuals SCT barrel layer 1",        s_bins, s_resmin, s_resmax);
  TH1F *xressct2 = new TH1F("xressct2",             "x-residuals SCT barrel layer 2",        s_bins, s_resmin, s_resmax);
  TH1F *xressct3 = new TH1F("xressct3",             "x-residuals SCT barrel layer 3",        s_bins, s_resmin, s_resmax);
  TH1F *xressct4 = new TH1F("xressct4",             "x-residuals SCT barrel layer 4",        s_bins, s_resmin, s_resmax);
  TH1F *yressct1 = new TH1F("yressct1",             "y-residuals SCT endcap layer 1",        s_bins, s_resmin, s_resmax);
  TH1F *yressct2 = new TH1F("yressct2",             "y-residuals SCT endcap layer 2",        s_bins, s_resmin, s_resmax);
  TH1F *yressct3 = new TH1F("yressct3",             "y-residuals SCT endcap layer 3",        s_bins, s_resmin, s_resmax);
  TH1F *yressct4 = new TH1F("yressct4",             "y-residuals SCT endcap layer 4",        s_bins, s_resmin, s_resmax);

  TH1F *xrestrt2 = new TH1F("xrestrt2",             "x-residuals, TRT hits -ve endcap",      s_bins, s_resmin2, s_resmax2);
  TH1F *xrestrt3 = new TH1F("xrestrt3",             "x-residuals, TRT hits -ve barrel",      s_bins, s_resmin2, s_resmax2);
  TH1F *xrestrt4 = new TH1F("xrestrt4",             "x-residuals, TRT hits +ve barrel",      s_bins, s_resmin2, s_resmax2);
  TH1F *xrestrt5 = new TH1F("xrestrt5",             "x-residuals, TRT hits +ve endcap",      s_bins, s_resmin2, s_resmax2);


  xrespix1->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  xrespix2->GetXaxis()->SetTitle("#Deltax [#mum]");  
  xrespix3->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  yrespix1->GetXaxis()->SetTitle("#Deltay [#mum]"); 
  yrespix2->GetXaxis()->SetTitle("#Deltay [#mum]"); 
  yrespix3->GetXaxis()->SetTitle("#Deltay [#mum]"); 

  xressct1->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  xressct2->GetXaxis()->SetTitle("#Deltax [#mum]");  
  xressct3->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  xressct4->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  yressct1->GetXaxis()->SetTitle("#Deltay [#mum]"); 
  yressct2->GetXaxis()->SetTitle("#Deltay [#mum]"); 
  yressct3->GetXaxis()->SetTitle("#Deltay [#mum]"); 
  yressct4->GetXaxis()->SetTitle("#Deltay [#mum]"); 

  xrestrt2->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  xrestrt3->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  xrestrt4->GetXaxis()->SetTitle("#Deltax [#mum]"); 
  xrestrt5->GetXaxis()->SetTitle("#Deltax [#mum]"); 

  if (*hit_cut.GetTitle()) cout << "cut: " << hit_cut.GetTitle() << endl;

  TCut pixcut (hit_cut && "hit_dettype==0");
  TCut pixcut1 (pixcut && "hit_layer==0");
  TCut pixcut2 (pixcut && "hit_layer==1");
  TCut pixcut3 (pixcut && "hit_layer==2");

  TCut bcut ("hit_isbarrel==0");
  TCut ecut ("hit_isbarrel!=0");
  TCut sctcut (hit_cut && "hit_dettype==1");
  TCut sctcut1 (sctcut && "hit_layer==0");
  TCut sctcut2 (sctcut && "hit_layer==1");
  TCut sctcut3 (sctcut && "hit_layer==2");
  TCut sctcut4 (sctcut && "hit_layer==3");
#if DUPOLD==1
  TCut sctmapcut ("hit_dettype==1 && hit_isbarrel!=0 && hit_layer==0");
#else
  TCut sctmapcut (sctcut1 && ecut);
#endif

  TCut trtcut (hit_cut && "hit_dettype==2");

  ntuple->Project("xrespix1",    "hit_unbiasedresidualx*1000", pixcut1);
  ntuple->Project("xrespix2",    "hit_unbiasedresidualx*1000", pixcut2);
  ntuple->Project("xrespix3",    "hit_unbiasedresidualx*1000", pixcut3);
  ntuple->Project("yrespix1",    "hit_unbiasedresidualy*1000", pixcut1);
  ntuple->Project("yrespix2",    "hit_unbiasedresidualy*1000", pixcut2);
  ntuple->Project("yrespix3",    "hit_unbiasedresidualy*1000", pixcut3);

/*
  nt->Project("xrespix1nt",    "hit_unbiasedresidualx*1000", pixcut1);
  nt->Project("xrespix2nt",    "hit_unbiasedresidualx*1000", pixcut2);
  nt->Project("xrespix3nt",    "hit_unbiasedresidualx*1000", pixcut3);
  nt->Project("yrespix1nt",    "hit_unbiasedresidualy*1000", pixcut1);
  nt->Project("yrespix2nt",    "hit_unbiasedresidualy*1000", pixcut2);
  nt->Project("yrespix3nt",    "hit_unbiasedresidualy*1000", pixcut3);
*/
  // draw map of pixel hits
  c_hitpix->cd(1);  ntuple->Draw("hit_globaly:hit_globalx",pixcut);
  c_hitpix->cd(2);  ntuple->Draw("hit_globaly:hit_globalz",pixcut);

  // draw map of SCT hits
  c_hitsct->cd(1);  ntuple->Draw("hit_globalx:hit_globaly:hit_globalz",sctmapcut);
  c_hitsct->cd(2);  ntuple->Draw(            "hit_globaly:hit_globalz",sctmapcut);

  // draw map of TRT hits
  c_hittrt->cd(1);  ntuple->Draw("hit_globaly:hit_globalx",trtcut);
  c_hittrt->cd(2);  ntuple->Draw("hit_globaly:hit_globalz",trtcut);

  ntuple->Project("xressct1",    "hit_unbiasedresidualx*1000", sctcut1 && bcut);
  ntuple->Project("xressct2",    "hit_unbiasedresidualx*1000", sctcut2 && bcut);
  ntuple->Project("xressct3",    "hit_unbiasedresidualx*1000", sctcut3 && bcut);
  ntuple->Project("xressct4",    "hit_unbiasedresidualx*1000", sctcut4 && bcut);
  ntuple->Project("yressct1",    "hit_unbiasedresidualx*1000", sctcut1 && ecut);
  ntuple->Project("yressct2",    "hit_unbiasedresidualx*1000", sctcut2 && ecut);
  ntuple->Project("yressct3",    "hit_unbiasedresidualx*1000", sctcut3 && ecut);
  ntuple->Project("yressct4",    "hit_unbiasedresidualx*1000", sctcut4 && ecut);

  ntuple->Project("xrestrt2",    "hit_unbiasedresidualx*1000", trtcut && "hit_isbarrel==-2 ");
  ntuple->Project("xrestrt3",    "hit_unbiasedresidualx*1000", trtcut && "hit_isbarrel==-1 ");
  ntuple->Project("xrestrt4",    "hit_unbiasedresidualx*1000", trtcut && "hit_isbarrel== 1 ");
  ntuple->Project("xrestrt5",    "hit_unbiasedresidualx*1000", trtcut && "hit_isbarrel== 2 ");


  // draw pulls
 
  // pull12.SetLineType(2);
  
  plotStyle.SetOptFit();
 
  c_ressct->cd(1); xressct1->Draw();  xressct1->Fit("gaus");
  c_ressct->cd(3); xressct2->Draw();  xressct2->Fit("gaus");
  c_ressct->cd(5); xressct3->Draw();  xressct3->Fit("gaus");
  c_ressct->cd(7); xressct4->Draw();  xressct4->Fit("gaus");
  c_ressct->cd(2); yressct1->Draw();  yressct1->Fit("gaus");
  c_ressct->cd(4); yressct2->Draw();  yressct2->Fit("gaus");
  c_ressct->cd(6); yressct3->Draw();  yressct3->Fit("gaus");
  c_ressct->cd(8); yressct4->Draw();  yressct4->Fit("gaus");
 
  xrespix1nt->SetLineColor(3);
  xrespix2nt->SetLineColor(3);
  xrespix3nt->SetLineColor(3);
  yrespix1nt->SetLineColor(3);
  yrespix2nt->SetLineColor(3);
  yrespix3nt->SetLineColor(3);

  c_respix->cd(1); xrespix1->Draw();  xrespix1->Fit("gaus"); 
  c_respix->cd(3); xrespix2->Draw();  xrespix2->Fit("gaus");   
  c_respix->cd(5); xrespix3->Draw();  xrespix3->Fit("gaus");   
  c_respix->cd(2); yrespix1->Draw();  yrespix1->Fit("gaus");   
  c_respix->cd(4); yrespix2->Draw();  yrespix2->Fit("gaus");   
  c_respix->cd(6); yrespix3->Draw();  yrespix3->Fit("gaus");   

  c_restrt->cd(1); xrestrt2->Draw();  xrestrt2->Fit("gaus");
  c_restrt->cd(2); xrestrt3->Draw();  xrestrt3->Fit("gaus");
  c_restrt->cd(3); xrestrt4->Draw();  xrestrt4->Fit("gaus");
  c_restrt->cd(4); xrestrt5->Draw();  xrestrt5->Fit("gaus");

  if (plotext != "") {
    string plotfile;
    plotfile = plotprefix+string("residuals_pixels")+plotext; c_respix->Print(plotfile.c_str());
    plotfile = plotprefix+string("hitglobal_pixels")+plotext; c_hitpix->Print(plotfile.c_str());
    plotfile = plotprefix+string("residuals_sct_nt")+plotext; c_ressct->Print(plotfile.c_str());
    plotfile = plotprefix+string("hitglobal_sct_nt")+plotext; c_hitsct->Print(plotfile.c_str());
    plotfile = plotprefix+string("residuals_trt"   )+plotext; c_restrt->Print(plotfile.c_str());
    plotfile = plotprefix+string("hitglobal_trt"   )+plotext; c_hittrt->Print(plotfile.c_str());
  }

  gROOT->SetStyle("Default");
  delete c_respix;
  delete c_ressct;
  delete c_restrt;
  delete c_hitpix;
  delete c_hitsct;
  delete c_hittrt;
}  
