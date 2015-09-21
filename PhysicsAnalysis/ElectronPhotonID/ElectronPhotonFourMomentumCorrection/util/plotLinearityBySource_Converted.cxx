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

using namespace egEnergyCorr;
using namespace std;

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect11d;
  ect11d.setESModel(es2011d);  
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

	double ptconv = 93580.;
	double pt1conv = 55200.;
	double pt2conv = 38400.;
	double Rconv = 268.;
	int n1PixHits = 1;
	int n2PixHits = 1;
	int n1SCTHits = 6;
	int n2SCTHits = 5;

	AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation piCnv(e0r, e1r, e2r, e3r, eta, phi, eIn, eta, phi,
									 ptconv, pt1conv, pt2conv, n1PixHits, n1SCTHits, n2PixHits, n2SCTHits, Rconv);

	double eVar = ect11d.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, piCnv, 
						var, egEnergyCorr::Resolution::None);
	
	double eNom = ect11d.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, piCnv, 
						egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None);
	
	hVar[var]->SetBinContent(i, j, eVar/eNom - 1);

      }
  
  // draw
  
  TCanvas c;
  gStyle->SetOptStat(0000000);

  // vs Eta

  // ... average uncertainties over broader ranges? 
 
  bool doAverage = true;
  double etaBins[] = {-2.4, -1.82, -1.55, -1.37, -1.0, -0.6, 0, 0.6, 1.0, 1.37, 1.55, 1.82, 2.4};

  int nBins = hVar[Scale::AllUp]->GetNbinsX();
  if( doAverage )
    nBins = sizeof(etaBins)/sizeof(double) - 1;

  // ... loop over eta bins

  for(int i=1; i<=nBins; i++) {

    int imin, imax;
    double etamin, etamax;
    if( !doAverage ) {
      etamin = hVar[Scale::AllUp]->GetBinLowEdge(i);
      etamax = hVar[Scale::AllUp]->GetBinLowEdge(i+1);
      imin = imax = i;
    } else {
      etamin = etaBins[i-1];
      etamax = etaBins[i];      
      imin = hVar[Scale::AllUp]->FindBin(etamin);
      imax = hVar[Scale::AllUp]->FindBin(etamax);
    }

    char htitle[99];
    sprintf(htitle,"Converted photons, %3.2f<|#eta|<%3.2f", etamin, etamax);    

    // ... envelope 
    
    TH1D* hprojp = hVar[Scale::AllUp]->ProjectionY("_py", imin, imax);
    for(int ix = 1; ix<=hprojp->GetNbinsX(); ix++)
      hprojp->SetBinContent(ix, hprojp->GetBinContent(ix)/(imax-imin+1));

    hprojp->SetMinimum(-.01);
    hprojp->SetMaximum(.01);
    hprojp->SetTitle("");
    hprojp->SetFillColor(kBlue-10);
    hprojp->SetFillStyle(1001);
    hprojp->SetLineColor(kBlue-10);
    hprojp->GetXaxis()->SetTitle("E_{T} [GeV]");
    hprojp->GetYaxis()->SetTitle("Uncertainty");
    hprojp->GetXaxis()->SetTitleOffset(1.25*gStyle->GetTitleXOffset());
    hprojp->GetYaxis()->SetTitleOffset(1.25*gStyle->GetTitleYOffset());
    hprojp->SetTitle(htitle);
    
    TH1D* hprojm = hVar[Scale::AllDown]->ProjectionY("_py", imin, imax); 
    for(int ix = 1; ix<=hprojm->GetNbinsX(); ix++)
      hprojm->SetBinContent(ix, hprojm->GetBinContent(ix)/(imax-imin+1));
    hprojm->SetFillColor(kBlue-10);
    hprojm->SetFillStyle(1001);
    hprojm->SetLineColor(kBlue-10);

    // ... plot each variation
    
    int icolor = 4;
    for (Scale::Variation var = Scale::None; var<Scale::LastScaleVariation; var = Scale::Variation(var+1)) {

      string varname = ect11d.variationName(var);

      hprojp->DrawCopy();
      hprojm->DrawCopy("same");    

      TH1D* hproj = hVar[var]->ProjectionY("_py", imin, imax); 
      for(int ix = 1; ix<=hproj->GetNbinsX(); ix++)
	hproj->SetBinContent(ix, hproj->GetBinContent(ix)/(imax-imin+1));

      if (ect11d.variationName(var).find("Up") == std::string::npos)
	continue;

      hproj->SetLineStyle(1); 
      hproj->SetLineColor(icolor);  
      hproj->SetLineWidth(2); 
      hproj->DrawCopy("same");

      TLegend l(.35, .15, .65, .25);
      l.SetTextSize(.05);
      l.SetTextFont(42);
      l.SetBorderSize(0);
      l.SetFillColor(0);  
      l.AddEntry(hproj, ect11d.variationName(var).c_str(), "l");
      l.AddEntry(hprojp, "Total", "f");
      l.Draw();    
      
      gPad->RedrawAxis();
      char plotname[99];
      sprintf(plotname,"lin_eta%d_%s_Converted.png",i,varname.c_str());
      c.SaveAs(plotname);

    }

  }

  // vs ET (three points : 10, 40, 60 GeV)

  // ... loop over ET bins

  nBins = hVar[Scale::AllUp]->GetNbinsY();
  for(int i=1; i<=nBins; i++) {

    double ET = hVar[Scale::AllUp]->GetYaxis()->GetBinCenter(i);
    if( !(ET>10.5 && ET<11.5) && !(ET>39.5 && ET<40.5) && !(ET>59.5 && ET<60.5) )
      continue;

    char htitle[99];
    sprintf(htitle,"Converted photons, ET=%f GeV", ET);    

    // ... envelope 
    
    TH1D* hprojp = hVar[Scale::AllUp]->ProjectionX("_px", i, i);
    hprojp->SetMinimum(0.);
    hprojp->SetMaximum(.01);
    hprojp->SetTitle("");
    hprojp->SetFillColor(kBlue-10);
    hprojp->SetFillStyle(1001);
    hprojp->SetLineColor(kBlue-10);
    hprojp->GetXaxis()->SetTitle("#eta");
    hprojp->GetYaxis()->SetTitle("Uncertainty");
    hprojp->GetXaxis()->SetTitleOffset(1.25*gStyle->GetTitleXOffset());
    hprojp->GetYaxis()->SetTitleOffset(1.25*gStyle->GetTitleYOffset());
    hprojp->SetTitle(htitle);
    
    TH1D* hprojm = hVar[Scale::AllDown]->ProjectionX("_px", i, i); 
    hprojm->SetFillColor(kBlue-10);
    hprojm->SetFillStyle(1001);
    hprojm->SetLineColor(kBlue-10);

    // ... plot each variation
    
    int icolor = 4;
    for (Scale::Variation var = Scale::None; var<Scale::LastScaleVariation; var = Scale::Variation(var+1)) {

      string varname = ect11d.variationName(var);

      hprojp->DrawCopy();
      hprojm->DrawCopy("same");    

      TH1D* hproj = hVar[var]->ProjectionX("_py", i, i); 

      if (ect11d.variationName(var).find("Up") == std::string::npos)
	continue;

      for(int iii=1; iii<=hproj->GetNbinsX(); iii++)
	hproj->SetBinContent(iii,fabs(hproj->GetBinContent(iii)));

      hproj->SetLineStyle(1); 
      hproj->SetLineColor(icolor);  
      hproj->SetLineWidth(2); 
      hproj->DrawCopy("same");

      TLegend l(.35, .75, .65, .85);
      l.SetTextSize(.05);
      l.SetTextFont(42);
      l.SetBorderSize(0);
      l.SetFillColor(0);  
      l.AddEntry(hproj, ect11d.variationName(var).c_str(), "l");
      l.AddEntry(hprojp, "Total", "f");
      l.Draw();    
      
      gPad->RedrawAxis();
      char plotname[99];
      sprintf(plotname,"lin_ET%d_%s_Converted.png",i,varname.c_str());
      c.SaveAs(plotname);

    }

  }

}
