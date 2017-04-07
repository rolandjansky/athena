/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void errD0VsPhi0(TString trkCol){
	errD0VsPhi0(trkCol, 0, "");
}

void errD0VsPhi0(TString trkCol, TString trkCol2){
	errD0VsPhi0(trkCol, 0, trkCol2);
}

void errD0VsPhi0(TString trkCol, TCanvas *cErrD0VsPhi0){
	errD0VsPhi0(trkCol, cErrD0VsPhi0, "");
}

void errD0VsPhi0(TString trkCol, TCanvas *cErrD0VsPhi0, TString trkCol2) {
	gStyle->SetOptStat(0);
	TProfile *hErrD0VsPhi0;
	TProfile *hErrD0VsPhi0VsPhi0Ba;
	TProfile *hErrD0VsPhi0VsPhi0ECC;
	TProfile *hErrD0VsPhi0VsPhi0ECA;

	TProfile *hErrD0VsPhi0_2;
	TProfile *hErrD0VsPhi0VsPhi0Ba_2;
	TProfile *hErrD0VsPhi0VsPhi0ECC_2;
	TProfile *hErrD0VsPhi0VsPhi0ECA_2;
	cout << " Error d0 vs Phi..." << endl;
	
	TString histname;
	Color_t Color;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	if(!cErrD0VsPhi0)
		cErrD0VsPhi0 = new TCanvas("cErrD0VsPhi0","Error D0 vs Phi0'",
							CanvasSizeX4[0],CanvasSizeX4[1]);
							
	cErrD0VsPhi0->Divide(2,2);

	cErrD0VsPhi0->cd(1);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsPhi";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrD0VsPhi0 =((TH2D*)file->Get(histname))->ProfileX();
	hErrD0VsPhi0->SetTitle("D0 err vs Phi0");
	hErrD0VsPhi0->GetXaxis()->SetTitle("Phi0 (rad)");
	hErrD0VsPhi0->GetYaxis()->SetTitle("#sigma (d0) (mm)");
	hErrD0VsPhi0->SetLineColor(Color+2);
	hErrD0VsPhi0->SetFillColor(Color);
	hErrD0VsPhi0->SetFillStyle(3001);
	hErrD0VsPhi0->SetMinimum(0.);
	if (normalizePlots) hErrD0VsPhi0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrD0VsPhi0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();

	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsPhi";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrD0VsPhi0_2 =((TH2D*)file->Get(histname))->ProfileX();
		hErrD0VsPhi0_2->SetLineColor(ColorForSecond+2);
		hErrD0VsPhi0_2->SetFillColor(ColorForSecond);
		hErrD0VsPhi0_2->SetLineWidth(1);
		hErrD0VsPhi0_2->SetFillStyle(3001);
		if (normalizePlots) hErrD0VsPhi0_2->Scale(1./(float)iEvents);
		hErrD0VsPhi0_2->DrawCopy("same");
	}

	//
	// -----------------------------------------------------------------
	//

	cErrD0VsPhi0->cd(2);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsPhiBarrel";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrD0VsPhi0VsPhi0Ba =((TH2D*)file->Get(histname))->ProfileX();
	hErrD0VsPhi0VsPhi0Ba->SetTitle("D0 err vs Phi0 (Barrel)");
	hErrD0VsPhi0VsPhi0Ba->GetXaxis()->SetTitle("Phi0 (rad)");
	hErrD0VsPhi0VsPhi0Ba->GetYaxis()->SetTitle("#sigma (d0) (mm)");
	hErrD0VsPhi0VsPhi0Ba->SetLineColor(Color+2);
	hErrD0VsPhi0VsPhi0Ba->SetFillColor(Color);
	hErrD0VsPhi0VsPhi0Ba->SetFillStyle(3001);
	hErrD0VsPhi0VsPhi0Ba->SetMinimum(0.);
	if (normalizePlots) hErrD0VsPhi0VsPhi0Ba->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrD0VsPhi0VsPhi0Ba->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsPhiBarrel";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrD0VsPhi0VsPhi0Ba_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrD0VsPhi0VsPhi0Ba_2->SetLineColor(ColorForSecond+2);
		hErrD0VsPhi0VsPhi0Ba_2->SetFillColor(ColorForSecond);
		hErrD0VsPhi0VsPhi0Ba_2->SetLineWidth(1);
		hErrD0VsPhi0VsPhi0Ba_2->SetFillStyle(3001);
		if (normalizePlots) hErrD0VsPhi0VsPhi0Ba_2->Scale(1./(float)iEvents);
		hErrD0VsPhi0VsPhi0Ba_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//
	
	cErrD0VsPhi0->cd(3);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsPhiECC";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrD0VsPhi0VsPhi0ECC = ((TH2D*)file->Get(histname))->ProfileX();
	hErrD0VsPhi0VsPhi0ECC->SetTitle("D0 err vs Phi0 (ECC)");
	hErrD0VsPhi0VsPhi0ECC->GetXaxis()->SetTitle("Phi0 (rad)");
	hErrD0VsPhi0VsPhi0ECC->GetYaxis()->SetTitle("#sigma (d0) (mm)");
	hErrD0VsPhi0VsPhi0ECC->SetLineColor(Color+2);
	hErrD0VsPhi0VsPhi0ECC->SetFillColor(Color);
	hErrD0VsPhi0VsPhi0ECC->SetFillStyle(3001);
	hErrD0VsPhi0VsPhi0ECC->SetMinimum(0.);
	if (normalizePlots) hErrD0VsPhi0VsPhi0ECC->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrD0VsPhi0VsPhi0ECC->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsPhiECC";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrD0VsPhi0VsPhi0ECC_2 =((TH2D*)file->Get(histname))->ProfileX();
		hErrD0VsPhi0VsPhi0ECC_2->SetLineColor(ColorForSecond+2);
		hErrD0VsPhi0VsPhi0ECC_2->SetFillColor(ColorForSecond);
		hErrD0VsPhi0VsPhi0ECC_2->SetLineWidth(1);
		hErrD0VsPhi0VsPhi0ECC_2->SetFillStyle(3001);
		if (normalizePlots) hErrD0VsPhi0VsPhi0ECC_2->Scale(1./(float)iEvents);
		hErrD0VsPhi0VsPhi0ECC_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cErrD0VsPhi0->cd(4);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsPhiECA";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrD0VsPhi0VsPhi0ECA = ((TH2D*)file->Get(histname))->ProfileX();
	hErrD0VsPhi0VsPhi0ECA->SetTitle("D0 err vs Phi0 (ECA)");
	hErrD0VsPhi0VsPhi0ECA->GetXaxis()->SetTitle("Phi0 (rad)");
	hErrD0VsPhi0VsPhi0ECA->GetYaxis()->SetTitle("#sigma (d0) (mm)");
	hErrD0VsPhi0VsPhi0ECA->SetLineColor(Color+2);
	hErrD0VsPhi0VsPhi0ECA->SetFillColor(Color);
	hErrD0VsPhi0VsPhi0ECA->SetFillStyle(3001);
	hErrD0VsPhi0VsPhi0ECA->SetMinimum(0.);
	if (normalizePlots) hErrD0VsPhi0VsPhi0ECA->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrD0VsPhi0VsPhi0ECA->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsPhiECA";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrD0VsPhi0VsPhi0ECA_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrD0VsPhi0VsPhi0ECA_2->SetLineColor(ColorForSecond+2);
		hErrD0VsPhi0VsPhi0ECA_2->SetFillColor(ColorForSecond);
		hErrD0VsPhi0VsPhi0ECA_2->SetLineWidth(1);
		hErrD0VsPhi0VsPhi0ECA_2->SetFillStyle(3001);
		if (normalizePlots) hErrD0VsPhi0VsPhi0ECA_2->Scale(1./(float)iEvents);
		hErrD0VsPhi0VsPhi0ECA_2->DrawCopy("same");
	}
	
	
	cErrD0VsPhi0->Update();
	gStyle->SetOptStat(111110);
}
