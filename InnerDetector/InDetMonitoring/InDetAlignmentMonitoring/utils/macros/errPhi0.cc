/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void errPhi0(TString trkCol){
	errPhi0(trkCol, 0, "");
}

void errPhi0(TString trkCol, TString trkCol2){
	errPhi0(trkCol, 0, trkCol2);
}

void errPhi0(TString trkCol, TCanvas *cErrPhi0){
	errPhi0(trkCol, cErrPhi0, "");
}

void errPhi0(TString trkCol, TCanvas *cErrPhi0, TString trkCol2) {
	gStyle->SetOptStat(0);

	TH1F *hErrPhi0;
	TProfile *hErrPhi0VsEta;
	TProfile *hErrPhi0VsPt;
	TProfile *hErrPhi0VsP;
	TProfile *hErrPhi0VsPhi0;
	//TProfile *hErrPhi0VsD0;
	TH1F *hErrPhi0_2;
	TProfile *hErrPhi0VsEta_2;
	TProfile *hErrPhi0VsPt_2;
	TProfile *hErrPhi0VsP_2;
	TProfile *hErrPhi0VsPhi0_2;
	//TProfile *hErrPhi0VsD0_2;
	cout << " Track Parameters..." << endl;
	
	TString histname;
	Color_t Color;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	if(!cErrPhi0)
		cErrPhi0 = new TCanvas("cErrPhi0","Error Phi0",
							CanvasSizeX6[0],CanvasSizeX6[1]);
							
	cErrPhi0->Divide(3,2);

	cErrPhi0->cd(1);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPhi0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPhi0 = (TH1F*)file->Get(histname);
	hErrPhi0->SetTitle("Phi0 Error");
	hErrPhi0->GetXaxis()->SetTitle("#sigma (#phi0) (rad)");
	hErrPhi0->GetYaxis()->SetTitle("Tracks");
	hErrPhi0->SetLineColor(Color+2);
	hErrPhi0->SetFillColor(Color);
	hErrPhi0->SetFillStyle(3001);
	hErrPhi0->SetMinimum(0.);
	if (normalizePlots) hErrPhi0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPhi0->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();

	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPhi0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPhi0_2 = (TH1F*)file->Get(histname);	
		hErrPhi0_2->SetLineColor(ColorForSecond+2);
		hErrPhi0_2->SetFillColor(ColorForSecond);
		hErrPhi0_2->SetLineWidth(1);
		hErrPhi0_2->SetFillStyle(3001);
		if (normalizePlots) hErrPhi0_2->Scale(1./(float)iEvents);
		hErrPhi0_2->DrawCopy("histsame");
	}

	//
	// -----------------------------------------------------------------
	//

	cErrPhi0->cd(2);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPhi0VsEta";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPhi0VsEta =((TH2D*)file->Get(histname))->ProfileX();
	hErrPhi0VsEta->SetTitle("#sigma(#phi0) vs #eta");
	hErrPhi0VsEta->GetXaxis()->SetTitle("#eta");
	hErrPhi0VsEta->GetYaxis()->SetTitle("#sigma (#phi0) (rad)");
	hErrPhi0VsEta->SetLineColor(Color+2);
	hErrPhi0VsEta->SetFillColor(Color);
	hErrPhi0VsEta->SetFillStyle(3001);
	hErrPhi0VsEta->SetMinimum(0.);
	if (normalizePlots) hErrPhi0VsEta->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPhi0VsEta->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPhi0VsEta";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPhi0VsEta_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrPhi0VsEta_2->SetLineColor(ColorForSecond+2);
		hErrPhi0VsEta_2->SetFillColor(ColorForSecond);
		hErrPhi0VsEta_2->SetLineWidth(1);
		hErrPhi0VsEta_2->SetFillStyle(3001);
		if (normalizePlots) hErrPhi0VsEta_2->Scale(1./(float)iEvents);
		hErrPhi0VsEta_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//
	
	cErrPhi0->cd(3);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPhi0VsPt";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPhi0VsPt = ((TH2D*)file->Get(histname))->ProfileX();
	hErrPhi0VsPt->SetTitle("#sigma(#phi0) vs pT");
	hErrPhi0VsPt->GetXaxis()->SetTitle("pT (GeV)");
	hErrPhi0VsPt->GetYaxis()->SetTitle("#sigma (#phi0) (rad)");
	hErrPhi0VsPt->SetLineColor(Color+2);
	hErrPhi0VsPt->SetFillColor(Color);
	hErrPhi0VsPt->SetFillStyle(3001);
	hErrPhi0VsPt->SetMinimum(0.);
	if (normalizePlots) hErrPhi0VsPt->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPhi0VsPt->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPhi0VsPt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPhi0VsPt_2 =((TH2D*)file->Get(histname))->ProfileX();
		hErrPhi0VsPt_2->SetLineColor(ColorForSecond+2);
		hErrPhi0VsPt_2->SetFillColor(ColorForSecond);
		hErrPhi0VsPt_2->SetLineWidth(1);
		hErrPhi0VsPt_2->SetFillStyle(3001);
		if (normalizePlots) hErrPhi0VsPt_2->Scale(1./(float)iEvents);
		hErrPhi0VsPt_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cErrPhi0->cd(4);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPhi0VsP";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPhi0VsP = ((TH2D*)file->Get(histname))->ProfileX();
	//gPad->SetLogy();
	hErrPhi0VsP->SetTitle("#sigma(#phi0) vs p");
	hErrPhi0VsP->GetXaxis()->SetTitle("p (GeV)");
	hErrPhi0VsP->GetYaxis()->SetTitle("#sigma (#phi0) (rad)");
	hErrPhi0VsP->SetLineColor(Color+2);
	hErrPhi0VsP->SetFillColor(Color);
	hErrPhi0VsP->SetFillStyle(3001);
	hErrPhi0VsP->SetMinimum(0.);
	if (normalizePlots) hErrPhi0VsP->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPhi0VsP->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPhi0VsP";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPhi0VsP_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrPhi0VsP_2->SetLineColor(ColorForSecond+2);
		hErrPhi0VsP_2->SetFillColor(ColorForSecond);
		hErrPhi0VsP_2->SetLineWidth(1);
		hErrPhi0VsP_2->SetFillStyle(3001);
		if (normalizePlots) hErrPhi0VsP_2->Scale(1./(float)iEvents);
		hErrPhi0VsP_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cErrPhi0->cd(5);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPhi0VsPhi0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPhi0VsPhi0 = ((TH2D*)file->Get(histname))->ProfileX();
	//gPad->SetLogy();
	hErrPhi0VsPhi0->SetTitle("#sigma(#phi0) vs #phi0");
	hErrPhi0VsPhi0->GetXaxis()->SetTitle("#phi0 (rad)");
	hErrPhi0VsPhi0->GetYaxis()->SetTitle("#sigma (#phi0) (rad)");
	hErrPhi0VsPhi0->SetLineColor(Color+2);
	hErrPhi0VsPhi0->SetFillColor(Color);
	hErrPhi0VsPhi0->SetFillStyle(3001);
	hErrPhi0VsPhi0->SetMinimum(0.);
	if (normalizePlots) hErrPhi0VsPhi0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPhi0VsPhi0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPhi0VsPhi0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPhi0VsPhi0_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrPhi0VsPhi0_2->SetLineColor(ColorForSecond+2);
		hErrPhi0VsPhi0_2->SetFillColor(ColorForSecond);
		hErrPhi0VsPhi0_2->SetLineWidth(1);
		hErrPhi0VsPhi0_2->SetFillStyle(3001);
		if (normalizePlots) hErrPhi0VsPhi0_2->Scale(1./(float)iEvents);
		hErrPhi0VsPhi0_2->DrawCopy("same");
	}

	

	//
	// -----------------------------------------------------------------
	//
/*
	cErrPhi0->cd(6);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPhi0VsD0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPhi0VsD0 = (TH1F*)file->Get(histname);	
	hErrPhi0VsD0->SetTitle("#sigma(#phi0) vs d0");
	hErrPhi0VsD0->GetXaxis()->SetTitle("#sigma(#phi0) vs d0");
	hErrPhi0VsD0->GetYaxis()->SetTitle("#sigma (#phi0) (rad)");
	hErrPhi0VsD0->SetLineColor(Color+2);
	hErrPhi0VsD0->SetFillColor(Color);
	hErrPhi0VsD0->SetFillStyle(3001);
	if (normalizePlots) hErrPhi0VsD0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPhi0VsD0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPhi0VsD0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPhi0VsD0_2 = (TH1F*)file->Get(histname);	
		hErrPhi0VsD0_2->SetLineColor(ColorForSecond+2);
		hErrPhi0VsD0_2->SetFillColor(ColorForSecond);
		hErrPhi0VsD0_2->SetLineWidth(1);
		hErrPhi0VsD0_2->SetFillStyle(3001);
		if (normalizePlots) hErrPhi0VsD0_2->Scale(1./(float)iEvents);
		hErrPhi0VsD0_2->DrawCopy("same");
	}


*/
	cErrPhi0->Update();
	gStyle->SetOptStat(111110);
}
