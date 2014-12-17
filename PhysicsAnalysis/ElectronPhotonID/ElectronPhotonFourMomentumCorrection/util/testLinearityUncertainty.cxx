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

int main() {

  bool doJPsiScales     = false;
  bool doZUncertainty   = true;//true;
  bool doLArSyst        = true;//true;
  bool doSamplings      = true;//true;
  bool doMaterial       = true;//true;
  bool doGain           = true;//true;
  bool doPhotonSpecials = false;

	double ymax = 0.05;
	double ymin = -0.05;
	double max_pt = 1500;

  AtlasRoot::egammaEnergyCorrectionTool ect12c;
  ect12c.setESModel(egEnergyCorr::es2012c);
  ect12c.initialize();
  //  ect12c.setDebug(true);
  
  TFile fp("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/example.root");
  TProfile* p0 = (TProfile*) fp.Get("ElectronProfiles/p0Data");
  TProfile* p1 = (TProfile*) fp.Get("ElectronProfiles/p1Data");
  TProfile* p2 = (TProfile*) fp.Get("ElectronProfiles/p2Data");
  TProfile* p3 = (TProfile*) fp.Get("ElectronProfiles/p3Data");

  int runnumber = 210000;

  TH2D m2012c01p("m2012cZeeStatp","m2012cZeeStatp", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c02p("m2012cZeeSystp","m2012cZeeSystp", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c03p("m2012cLArCalip","m2012cLArCalip", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c04p("m2012cLArUncop","m2012cLarUncop", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c05p("m2012cLArElecp","m2012cLArElecp", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c06p("m2012cPresampp","m2012cPresampp", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c07p("m2012cLayer12p","m2012cLayer12p", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c08p("m2012cMatIDetp","m2012cMatIDetp", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c09p("m2012cMatCryop","m2012cMatCryop", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c10p("m2012cMatCalop","m2012cMatCalop", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c11p("m2012cLeakagep","m2012cLeakagep", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c12p("m2012cConvMisp","m2012cConvMisp", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c13p("m2012cGainOffp","m2012cGainOffp", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c14p("m2012cAllSystp","m2012cAllSystp", 12, 0., 2.4, 96, 4.5,  max_pt);

  TH2D m2012c01m("m2012cZeeStatm","m2012cZeeStatm", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c02m("m2012cZeeSystm","m2012cZeeSystm", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c03m("m2012cLArCalim","m2012cLArCalim", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c04m("m2012cLArUncom","m2012cLarUncom", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c05m("m2012cLArElecm","m2012cLArElecm", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c06m("m2012cPresampm","m2012cPresampm", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c07m("m2012cLayer12m","m2012cLayer12m", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c08m("m2012cMatIDetm","m2012cMatIDetm", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c09m("m2012cMatCryom","m2012cMatCryom", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c10m("m2012cMatCalom","m2012cMatCalom", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c11m("m2012cLeakagem","m2012cLeakagem", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c12m("m2012cConvMism","m2012cConvMism", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c13m("m2012cGainOffm","m2012cGainOffm", 12, 0., 2.4, 96, 4.5,  max_pt);
  TH2D m2012c14m("m2012cAllSystm","m2012cAllSystm", 12, 0., 2.4, 96, 4.5,  max_pt);

  for(int i=1; i<=m2012c01p.GetNbinsX(); i++)
    for(int j=1; j<=m2012c01p.GetNbinsY(); j++) {
    
      double eta = m2012c01p.GetXaxis()->GetBinCenter(i);
      // if(fabs(eta)>1.4&&fabs(eta)<1.6)
      // 	continue;
      
      double et  = m2012c01p.GetYaxis()->GetBinCenter(j);
      double eIn = et * cosh(eta) * 1E3;
      double phi = 1.;
    
      double e0r = p0->GetBinContent( p0->FindBin(eta) ) * 1E3 * et/40.;
      double e1r = p1->GetBinContent( p1->FindBin(eta) ) * 1E3 * et/40.;
      double e2r = p2->GetBinContent( p2->FindBin(eta) ) * 1E3 * et/40.;
      double e3r = p3->GetBinContent( p3->FindBin(eta) ) * 1E3 * et/40.;

      double eVar01p = 0.;
      double eVar02p = 0.;
      double eVar03p = 0.;
      double eVar04p = 0.;
      double eVar05p = 0.;
      double eVar06p = 0.;
      double eVar07p = 0.;
      double eVar08p = 0.;
      double eVar09p = 0.;
      double eVar10p = 0.;
      double eVar11p = 0.;
      double eVar12p = 0.;
      double eVar13p = 0.;
      double eVar14p = 0.;
      double eVar01m = 0.;
      double eVar02m = 0.;
      double eVar03m = 0.;
      double eVar04m = 0.;
      double eVar05m = 0.;
      double eVar06m = 0.;
      double eVar07m = 0.;
      double eVar08m = 0.;
      double eVar09m = 0.;
      double eVar10m = 0.;
      double eVar11m = 0.;
      double eVar12m = 0.;
      double eVar13m = 0.;
      double eVar14m = 0.;
      double eNom = 0.;

      AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation pi(e0r, e1r, e2r, e3r, eta, phi, eta, eIn, eta, phi);
      
      // es2012c, Syst (MC)
      
      if( doZUncertainty ) {

	eVar01p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::ZeeStatUp, egEnergyCorr::Resolution::None);
	
	eVar02p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::ZeeSystUp, egEnergyCorr::Resolution::None);
	
	eVar01m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::ZeeStatDown, egEnergyCorr::Resolution::None);
	
	eVar02m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::ZeeSystDown, egEnergyCorr::Resolution::None);
      
      }

      if( doLArSyst ) {

	eVar03p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::LArCalibUp, egEnergyCorr::Resolution::None);
	
	eVar04p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::LArUnconvCalibUp, egEnergyCorr::Resolution::None);
	
	eVar05p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::LArElecUnconvUp, egEnergyCorr::Resolution::None);

	eVar03m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::LArCalibDown, egEnergyCorr::Resolution::None);
	
	eVar04m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::LArUnconvCalibDown, egEnergyCorr::Resolution::None);
	
	eVar05m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					 egEnergyCorr::Scale::LArElecUnconvDown, egEnergyCorr::Resolution::None);

       }

      if( doSamplings ) {
	
	eVar06p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::PSUp, egEnergyCorr::Resolution::None);
	
	eVar07p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::S12Up, egEnergyCorr::Resolution::None);
	
	eVar06m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::PSDown, egEnergyCorr::Resolution::None);
	
	eVar07m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::S12Down, egEnergyCorr::Resolution::None);
	
      }

      if( doMaterial ) {

	eVar08p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::MatIDUp, egEnergyCorr::Resolution::None);
	
	eVar09p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::MatCryoUp, egEnergyCorr::Resolution::None);
	
	eVar10p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::MatCaloUp, egEnergyCorr::Resolution::None);
	
	eVar08m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::MatIDDown, egEnergyCorr::Resolution::None);
	
	eVar09m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::MatCryoDown, egEnergyCorr::Resolution::None);
	
	eVar10m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::MatCaloDown, egEnergyCorr::Resolution::None);
	
      }

      if( doPhotonSpecials ) {

	eVar11p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::LeakageUnconvUp, egEnergyCorr::Resolution::None);
	
	eVar12p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::ConvEfficiencyUp, egEnergyCorr::Resolution::None);
	
	eVar11m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::LeakageUnconvDown, egEnergyCorr::Resolution::None);
	
	eVar12m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::ConvEfficiencyDown, egEnergyCorr::Resolution::None);

      }

      if( doGain ) {
	
	eVar13p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::L2GainUp, egEnergyCorr::Resolution::None);
	
	eVar13m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					    egEnergyCorr::Scale::L2GainDown, egEnergyCorr::Resolution::None);
	
      }

      
      eVar14p = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					  egEnergyCorr::Scale::AllUp, egEnergyCorr::Resolution::None);
      
      eVar14m = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
					  egEnergyCorr::Scale::AllDown, egEnergyCorr::Resolution::None);
      
      
      eNom = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
				       egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None);
      

      m2012c01p.SetBinContent(i, j, eNom/eVar01p - 1);
      m2012c02p.SetBinContent(i, j, eNom/eVar02p - 1);
      m2012c03p.SetBinContent(i, j, eNom/eVar03p - 1);
      m2012c04p.SetBinContent(i, j, eNom/eVar04p - 1);
      m2012c05p.SetBinContent(i, j, eNom/eVar05p - 1);
      m2012c06p.SetBinContent(i, j, eNom/eVar06p - 1);
      m2012c07p.SetBinContent(i, j, eNom/eVar07p - 1);
      m2012c08p.SetBinContent(i, j, eNom/eVar08p - 1);
      m2012c09p.SetBinContent(i, j, eNom/eVar09p - 1);
      m2012c10p.SetBinContent(i, j, eNom/eVar10p - 1);
      m2012c11p.SetBinContent(i, j, eNom/eVar11p - 1);
      m2012c12p.SetBinContent(i, j, eNom/eVar12p - 1);
      m2012c13p.SetBinContent(i, j, eNom/eVar13p - 1);
      m2012c14p.SetBinContent(i, j, eNom/eVar14p - 1);

      m2012c01m.SetBinContent(i, j, eNom/eVar01m - 1);
      m2012c02m.SetBinContent(i, j, eNom/eVar02m - 1);
      m2012c03m.SetBinContent(i, j, eNom/eVar03m - 1);
      m2012c04m.SetBinContent(i, j, eNom/eVar04m - 1);
      m2012c05m.SetBinContent(i, j, eNom/eVar05m - 1);
      m2012c06m.SetBinContent(i, j, eNom/eVar06m - 1);
      m2012c07m.SetBinContent(i, j, eNom/eVar07m - 1);
      m2012c08m.SetBinContent(i, j, eNom/eVar08m - 1);
      m2012c09m.SetBinContent(i, j, eNom/eVar09m - 1);
      m2012c10m.SetBinContent(i, j, eNom/eVar10m - 1);
      m2012c11m.SetBinContent(i, j, eNom/eVar11m - 1);
      m2012c12m.SetBinContent(i, j, eNom/eVar12m - 1);
      m2012c13m.SetBinContent(i, j, eNom/eVar13m - 1);
      m2012c14m.SetBinContent(i, j, eNom/eVar14m - 1);

  }

  // draw

  TFile fjpsi("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/scales_delta_Jpsi_Z_syst_fP20.root");
  TH1D* hjpsi = (TH1D*) fjpsi.Get("h_scale");
  
  TCanvas c;
  gStyle->SetOptStat(0000000);

  for(int i=1; i<=m2012c01p.GetNbinsX(); i++) {

    char htitle[99];
    double eta = m2012c14p.GetXaxis()->GetBinCenter(i);
    double etamin = m2012c14p.GetXaxis()->GetBinLowEdge(i);
    double etamax = m2012c14p.GetXaxis()->GetBinLowEdge(i+1);
    sprintf(htitle,"%3.2f<|#eta|<%3.2f", etamin, etamax);    

    TH1D* hprojp = m2012c14p.ProjectionY("_py", i, i);
    hprojp->SetMinimum(ymin);
    hprojp->SetMaximum(ymax);
    hprojp->SetTitle("");
    hprojp->SetFillColor(kBlue-10);
    hprojp->SetFillStyle(1001);
    hprojp->SetLineColor(kBlue-10);
    hprojp->GetXaxis()->SetTitle("E_{T} [GeV]");
    hprojp->GetYaxis()->SetTitle("Uncertainty");
    hprojp->GetXaxis()->SetTitleOffset(1.25*gStyle->GetTitleXOffset());
    hprojp->GetYaxis()->SetTitleOffset(1.25*gStyle->GetTitleYOffset());
    hprojp->SetTitle(htitle);
    hprojp->DrawCopy();
    
    TH1D* hprojm = m2012c14m.ProjectionY("_py", i, i); 
    hprojm->SetFillColor(kBlue-10);
    hprojm->SetFillStyle(1001);
    hprojm->SetLineColor(kBlue-10);
    hprojm->DrawCopy("same");    
    
    int ijpsi = hjpsi->FindBin(eta);
    double alpha = hjpsi->GetBinContent(ijpsi);
    double alerr = hjpsi->GetBinError(ijpsi);
    TGraphErrors graph; graph.SetPoint(0,11.5,alpha); graph.SetPointError(0,0.1,alerr);

    TH1D* hproj01 = m2012c01p.ProjectionY("_py", i, i); hproj01->SetLineColor(2);  hproj01->SetLineWidth(2); hproj01->DrawCopy("same");
    TH1D* hproj02 = m2012c02p.ProjectionY("_py", i, i); hproj02->SetLineColor(3);  hproj02->SetLineWidth(2); hproj02->DrawCopy("same");
    TH1D* hproj03 = m2012c03p.ProjectionY("_py", i, i); hproj03->SetLineColor(4);  hproj03->SetLineWidth(2); hproj03->DrawCopy("same");
    TH1D* hproj04 = m2012c04p.ProjectionY("_py", i, i); hproj04->SetLineColor(5);  hproj04->SetLineWidth(2); hproj04->DrawCopy("same");
    TH1D* hproj05 = m2012c05p.ProjectionY("_py", i, i); hproj05->SetLineColor(6);  hproj05->SetLineWidth(2); hproj05->DrawCopy("same");
    TH1D* hproj06 = m2012c06p.ProjectionY("_py", i, i); hproj06->SetLineColor(7);  hproj06->SetLineWidth(2); hproj06->DrawCopy("same");
    TH1D* hproj07 = m2012c07p.ProjectionY("_py", i, i); hproj07->SetLineColor(8);  hproj07->SetLineWidth(2); hproj07->DrawCopy("same");
    TH1D* hproj08 = m2012c08p.ProjectionY("_py", i, i); hproj08->SetLineColor(9);  hproj08->SetLineWidth(2); hproj08->DrawCopy("same");
    TH1D* hproj09 = m2012c09p.ProjectionY("_py", i, i); hproj09->SetLineColor(21); hproj09->SetLineWidth(2); hproj09->DrawCopy("same");
    TH1D* hproj10 = m2012c10p.ProjectionY("_py", i, i); hproj10->SetLineColor(31); hproj10->SetLineWidth(2); hproj10->DrawCopy("same");
    TH1D* hproj11 = m2012c11p.ProjectionY("_py", i, i); hproj11->SetLineColor(41); hproj11->SetLineWidth(2); hproj11->DrawCopy("same");
    TH1D* hproj12 = m2012c12p.ProjectionY("_py", i, i); hproj12->SetLineColor(51); hproj12->SetLineWidth(2); hproj12->DrawCopy("same");
    TH1D* hproj13 = m2012c13p.ProjectionY("_py", i, i); hproj13->SetLineColor(15); hproj13->SetLineWidth(2); hproj13->DrawCopy("same");
    TH1D* hproj14 = m2012c13m.ProjectionY("_py", i, i); hproj14->SetLineColor(15); hproj14->SetLineWidth(2); hproj14->DrawCopy("same");

    if( doJPsiScales ) {

      graph.SetMarkerStyle(20);
      graph.SetMarkerColor(1);
      graph.SetMarkerSize(1);
      graph.SetLineWidth(2);
      graph.Draw("lpe");
    
      TLegend l0(.2, .15, .35, .35);
      l0.SetTextSize(.04);
      l0.SetBorderSize(0);
      l0.SetFillColor(0);
      l0.AddEntry(&graph, "#alpha(J/Psi)", "lpe");
      l0.AddEntry((TObject*)0x0, "", "");
      l0.AddEntry((TObject*)0x0, "", "");
      l0.AddEntry((TObject*)0x0, "", "");
      l0.AddEntry((TObject*)0x0, "", "");
      l0.Draw();

    }

    TLegend l(.25, .60, .7, .80);
    l.SetTextSize(.04);
    l.SetNColumns(3);
    l.SetBorderSize(0);
    l.SetFillColor(0);

    if( doZUncertainty ) {
      l.AddEntry(hproj01, "ZeeStat", "l");
      l.AddEntry(hproj02, "ZeeSyst", "l");
    }

    if( doLArSyst ) {
      l.AddEntry(hproj03, "LArCalib", "l");
      l.AddEntry(hproj04, "LArUnconv", "l");
      l.AddEntry(hproj05, "LArElec", "l");
    }
    
    if( doSamplings ) {
      l.AddEntry(hproj06, "PS", "l");
      l.AddEntry(hproj07, "S12", "l");
    }

    if( doMaterial ) {
      l.AddEntry(hproj08, "MatID", "l");
      l.AddEntry(hproj09, "MatCalo", "l");
      l.AddEntry(hproj10, "MatCryo", "l");
    }

    if( doGain )
      l.AddEntry(hproj13, "L2Gain", "l");
    
    if( doPhotonSpecials ) {
      l.AddEntry(hproj11, "Leakage", "l");
      l.AddEntry(hproj12, "ConvMis", "l");
    }

    l.AddEntry(hprojp, "Total", "f");
    l.Draw();

    gPad->RedrawAxis();
    
    char plotname[99];
    sprintf(plotname,"lin_eta%d.png",i);
    c.SaveAs(plotname);

  }

  TH1D* hprojetap = m2012c14p.ProjectionX("_px", 7, 7);
  TH1D* hprojetam = m2012c14m.ProjectionX("_px", 7, 7);
  hprojetap->SetMinimum(ymin);
  hprojetap->SetMaximum(ymax);
  hprojetap->SetTitle("");
  hprojetap->SetFillColor(kBlue-10);
  hprojetap->SetFillStyle(1001);
  hprojetap->SetLineColor(kBlue-10);
  hprojetap->GetXaxis()->SetTitle("#eta");
  hprojetap->GetYaxis()->SetTitle("#alpha(J/Psi) - #alpha(Z)");
  hprojetap->GetXaxis()->SetTitleOffset(1.25*gStyle->GetTitleXOffset());
  hprojetap->GetYaxis()->SetTitleOffset(1.25*gStyle->GetTitleYOffset());
  hprojetap->SetTitle("<p_{T}> = 11.5 Gev");
  hprojetap->DrawCopy();
  hprojetam->SetFillColor(kBlue-10);
  hprojetam->SetFillStyle(1001);
  hprojetam->SetLineColor(kBlue-10);
  hprojetam->DrawCopy("same");    
  gPad->RedrawAxis();
  hjpsi->Draw("same");
  hjpsi->SetMarkerColor(1);
  hjpsi->SetLineColor(1);
  c.SaveAs("unif_pt11.png");
    

  // write out

  TFile fOut("testLinearityUncertainty.root","recreate");

  m2012c01p.Write();
  m2012c02p.Write();
  m2012c03p.Write();
  m2012c04p.Write();
  m2012c05p.Write();
  m2012c06p.Write();
  m2012c07p.Write();
  m2012c08p.Write();
  m2012c09p.Write();
  m2012c10p.Write();
  m2012c11p.Write();
  m2012c12p.Write();
  m2012c13p.Write();
  m2012c14p.Write();

  m2012c01m.Write();
  m2012c02m.Write();
  m2012c03m.Write();
  m2012c04m.Write();
  m2012c05m.Write();
  m2012c06m.Write();
  m2012c07m.Write();
  m2012c08m.Write();
  m2012c09m.Write();
  m2012c10m.Write();
  m2012c11m.Write();
  m2012c12m.Write();
  m2012c13m.Write();
  m2012c14m.Write();

  fOut.Close();
  
  return 0;
}
