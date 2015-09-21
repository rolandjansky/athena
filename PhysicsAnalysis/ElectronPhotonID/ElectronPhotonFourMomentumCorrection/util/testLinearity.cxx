/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

#include "TProfile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"
#include "TLatex.h"

using namespace egEnergyCorr;
using namespace std;

int main() {

  bool doJPsi = true;
  bool doLine = true;
  bool doTotal = true;
  ESModel model = es2011d;
  
  int year;
  double lumi, roots;
  if( model==es2011d ) {
    year = 2011;
    lumi = 4.7;
    roots = 7;
  } else if( model==es2012c ) {
    year = 2012;
    lumi = 20.3;
    roots = 8;
  }

  AtlasRoot::egammaEnergyCorrectionTool ect11d;
  ect11d.setESModel(model);  
  ect11d.initialize();
  //  ect11d.setDebug(true);
  
  TFile fp("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/example.root");
  TProfile* p0 = (TProfile*) fp.Get("ElectronProfiles/p0Data");
  TProfile* p1 = (TProfile*) fp.Get("ElectronProfiles/p1Data");
  TProfile* p2 = (TProfile*) fp.Get("ElectronProfiles/p2Data");
  TProfile* p3 = (TProfile*) fp.Get("ElectronProfiles/p3Data");

  int runnumber = 210000;

  // create and fill all histograms

  TH2D* hVar[Scale::LastScaleVariation];
  for (Scale::Variation var = Scale::None; var<Scale::LastScaleVariation; var = Scale::Variation(var+1))
    hVar[var] = new TH2D(ect11d.variationName(var).c_str(), ect11d.variationName(var).c_str(), 96, -2.4, 2.4, 96, 4.5,  100.5);

  for (Scale::Variation var = Scale::None; var<Scale::LastScaleVariation; var = Scale::Variation(var+1))
    for(int i=1; i<=hVar[var]->GetNbinsX(); i++)
      for(int j=1; j<=hVar[var]->GetNbinsY(); j++) {
	
	double eta = hVar[var]->GetXaxis()->GetBinCenter(i);
	double et  = hVar[var]->GetYaxis()->GetBinCenter(j);
	double eIn = et * cosh(eta) * 1E3;
	double phi = 1.;
	
	double e0r = p0->GetBinContent( p0->FindBin(eta) ) * 1E3 * et/40.;
	double e1r = p1->GetBinContent( p1->FindBin(eta) ) * 1E3 * et/40.;
	double e2r = p2->GetBinContent( p2->FindBin(eta) ) * 1E3 * et/40.;
	double e3r = p3->GetBinContent( p3->FindBin(eta) ) * 1E3 * et/40.;

	AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation pi(e0r, e1r, e2r, e3r, eta, phi, eta, eIn, eta, phi);

	double eVar = ect11d.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
						var, egEnergyCorr::Resolution::None);
	
	double eNom = ect11d.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
						egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None);
	
	hVar[var]->SetBinContent(i, j, eVar/eNom - 1);

      }
  
  // draw
  
  // ... to select which variations to plot
  // ... in this case : electron Up systematics only

  int isActive[Scale::LastScaleVariation] = { 0, 0,                      // None, Nominal
					      0, 0,                      // Momentum
					      0, 0, 0, 0, 0, 0,          // Z scales
					      0, 0, 0, 0, 0, 0, 0, 0,    // LAr systematics
					      0, 0,                      // G4
					      0, 0, 0, 0,                // Sampling scales
					      0, 0, 0, 0, 0, 0,          // Material
					      0, 0,                      // Gain
					      0, 0,                      // Leakage
					      0, 0, 0, 0, 0, 0,          // Conversion
					      0, 0,                      // Pedestal
					      0, 0 };                    // All

  TFile* fjpsi;
  TH1D* hjpsi = 0;  
  if( model==es2012c ) {
    fjpsi = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/scales_delta_Jpsi_Z_syst_fP20.root");
    hjpsi = (TH1D*) fjpsi->Get("h_scale_sym");
  }

  TFile* fline;
  if( model==es2012c )
    fline = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/Combine_MatandSum_AllEtaBins_More.root");
  else if( model==es2011d )
    fline = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/Combine_MatandSum_AllEtaBins_2011.root");    
  TGraphErrors* g0 = (TGraphErrors*) fline->Get("Linearity_Bin0");
  TGraphErrors* g1 = (TGraphErrors*) fline->Get("Linearity_Bin1");
  TGraphErrors* g2 = (TGraphErrors*) fline->Get("Linearity_Bin2");
  TGraphErrors* g3 = (TGraphErrors*) fline->Get("Linearity_Bin3");
  TGraphErrors* g4 = (TGraphErrors*) fline->Get("Linearity_Bin4");
  TGraphErrors* g5 = (TGraphErrors*) fline->Get("Linearity_Bin5");

  TCanvas c;
  gStyle->SetOptStat(0000000);

  // average uncertainties over broader ranges?
  
  bool doAverage = true;
  // double etaBins[] = { -2.4, -1.55, -1.37, -0.8, -0.4, -0.2, 0., 0.2, 0.4, 0.8, 1.37, 1.55, 2.4};
  double etaBins[] = {-2.4, -1.82, -1.55, -1.37, -1.0, -0.6, 0, 0.6, 1.0, 1.37, 1.55, 1.82, 2.4};

  int nBins = hVar[Scale::AllUp]->GetNbinsX();
  if( doAverage )
    nBins = sizeof(etaBins)/sizeof(double) - 1;

  for(int i=1; i<=nBins; i++) {

    char htitle[99];

    int imin, imax;
    double eta, etamin, etamax;
    if( !doAverage ) {
      eta = hVar[Scale::AllUp]->GetBinCenter(i);
      etamin = hVar[Scale::AllUp]->GetBinLowEdge(i);
      etamax = hVar[Scale::AllUp]->GetBinLowEdge(i+1);
      imin = imax = i;
    } else {
      eta = .5*(etaBins[i-1]+etaBins[i]);
      etamin = etaBins[i-1];
      etamax = etaBins[i];      
      imin = hVar[Scale::AllUp]->FindBin(etamin);
      imax = hVar[Scale::AllUp]->FindBin(etamax);
    }

    cout << nBins << " " << eta << " " << etamin << " " << etamax << " " << imin << " " << imax << endl;
    
    sprintf(htitle,"Electrons, %3.2f<|#eta|<%3.2f", etamin, etamax);    

    TH1D* hprojp = hVar[Scale::AllUp]->ProjectionY("_py", imin, imax);
    for(int ix = 1; ix<=hprojp->GetNbinsX(); ix++)
      hprojp->SetBinContent(ix, hprojp->GetBinContent(ix)/(imax-imin+1));
    hprojp->SetMinimum(-.025);
    hprojp->SetMaximum(.025);
    hprojp->SetTitle("");
    hprojp->SetFillColor(kBlue-10);
    hprojp->SetFillStyle(1001);
    hprojp->SetLineColor(kBlue-10);
    hprojp->GetXaxis()->SetTitle("E_{T} [GeV]");
    hprojp->GetYaxis()->SetTitle("Uncertainty");
    hprojp->GetXaxis()->SetTitleOffset(1.25*gStyle->GetTitleXOffset());
    hprojp->GetYaxis()->SetTitleOffset(1.25*gStyle->GetTitleYOffset());
    
    TH1D* hprojm = hVar[Scale::AllDown]->ProjectionY("_py", imin, imax); 
    for(int ix = 1; ix<=hprojm->GetNbinsX(); ix++)
      hprojm->SetBinContent(ix, hprojm->GetBinContent(ix)/(imax-imin+1));
    hprojm->SetFillColor(kBlue-10);
    hprojm->SetFillStyle(1001);
    hprojm->SetLineColor(kBlue-10);
    
    if( !doTotal ) {
      hprojp->SetFillColor(0);
      hprojp->SetLineColor(0);
      hprojm->SetFillColor(0);
      hprojm->SetLineColor(0);
    }

    hprojp->DrawCopy();
    hprojm->DrawCopy("same");    

    TLegend l(0.7,0.65,0.9,0.85,NULL,"brNDC");
    l.SetBorderSize(0);
    l.SetTextSize(0.04);
    l.SetLineColor(1);
    l.SetLineStyle(1);
    l.SetLineWidth(1);
    l.SetFillColor(0);
    l.SetFillStyle(0);

    int icolor = 1;
    for (Scale::Variation var = Scale::None; var<Scale::LastScaleVariation; var = Scale::Variation(var+1)) {

      if( !isActive[var] )
	continue;

      TH1D* hproj = hVar[var]->ProjectionY("_py", imin, imax); 
      for(int ix = 1; ix<=hproj->GetNbinsX(); ix++)
	hproj->SetBinContent(ix, hproj->GetBinContent(ix)/(imax-imin+1));

      if (ect11d.variationName(var).find("Up") != std::string::npos) {
	icolor++;
	if( icolor%10==0 )
	  icolor++;
	hproj->SetLineStyle(1); 
      } else if (ect11d.variationName(var).find("Down") != std::string::npos)
	hproj->SetLineStyle(2); 

      hproj->SetLineColor(icolor);  
      hproj->SetLineWidth(2); 
      hproj->DrawCopy("same");

      l.AddEntry(hproj, ect11d.variationName(var).c_str(), "l");

    }
    
    if( doJPsi && hjpsi ) {
      
      int ijpsi = hjpsi->FindBin(eta-.0001*eta/fabs(eta));
      double alpha = hjpsi->GetBinContent(ijpsi);
      double alerr = hjpsi->GetBinError(ijpsi);
      TGraphErrors* graph = new TGraphErrors(); graph->SetPoint(0,10.,alpha); graph->SetPointError(0,0.1,alerr);
      graph->SetMarkerStyle(20);
      graph->SetMarkerColor(1);
      graph->SetMarkerSize(1.25);
      graph->SetLineWidth(2);
      graph->Draw("pe");
      l.AddEntry(graph, "#alpha_{J/#Psi}", "lpe");
      
    }

    if( doLine ) {
      
      TGraphErrors* linGraph=0;
      if( fabs(eta)<0.6 )
	linGraph = g0;
      else if( fabs(eta)<1.0 )
	linGraph = g1;
      else if( fabs(eta)<1.37 )
	linGraph = g2;
      else if( fabs(eta)<1.55 )
	linGraph = g3;
      else if( fabs(eta)<1.82 )
	linGraph = g4;
      else if( fabs(eta)<2.47 )
	linGraph = g5;

      if( !linGraph ) 
	continue;

      linGraph->SetMarkerStyle(21);
      linGraph->SetMarkerColor(1);
      linGraph->SetLineColor(1);
      linGraph->SetMarkerSize(1.25);
      linGraph->SetLineWidth(2);
      linGraph->Draw("pe");
      l.AddEntry(linGraph, "#alpha_{Z}(E_{T})", "lpe");
      
    }
    
    if( doTotal )
      l.AddEntry(hprojp, "Total", "f");

    l.Draw();

    TLine *line = new TLine(4.5,0,100.5,0);
    line->SetLineColor(kBlue);
    line->SetLineStyle(2);
    line->SetLineWidth(2);
    line->Draw();
    
    TLatex* tex = new TLatex(0.15,0.8,htitle);
    tex->SetNDC();
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();

    tex = new TLatex(0.15,0.17,"ATLAS Internal");
    tex->SetNDC();
    tex->SetTextFont(72);
    tex->SetLineWidth(2);
    tex->Draw();
    
    sprintf(htitle,"#sqrt{s}=%1.0f TeV, #int#it{L}d#it{t}=%4.1f fb^{-1}", roots, lumi);    
    tex = new TLatex(0.5,0.17,htitle);
    tex->SetNDC();
    tex->SetLineWidth(2);
    tex->Draw();
    
    
    gPad->RedrawAxis();
    char plotname[99];
    sprintf(plotname,"lin_data_eta%d_%d.pdf",i,year);
    c.SaveAs(plotname);
    sprintf(plotname,"lin_data_eta%d_%d.png",i,year);
    c.SaveAs(plotname);
    sprintf(plotname,"lin_data_eta%d_%d.C",i,year);
    c.SaveAs(plotname);

  }

}
