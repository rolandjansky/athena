/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void errCotTheta(TString trkCol){
	errCotTheta(trkCol, 0, "");
}

void errCotTheta(TString trkCol, TString trkCol2){
	errCotTheta(trkCol, 0, trkCol2);
}

void errCotTheta(TString trkCol, TCanvas *cErrCotTheta){
	errCotTheta(trkCol, cErrCotTheta, "");
}

void errCotTheta(TString trkCol, TCanvas *cErrCotTheta, TString trkCol2) {
	gStyle->SetOptStat(0);

	TH1F *hErrCotTheta0;
	TProfile *hErrCotTheta0VsEta;
	TProfile *hErrCotTheta0VsPt;
	TProfile *hErrCotTheta0VsP;
	TProfile *hErrCotTheta0VsPhi0;
	//TProfile *hErrCotTheta0VsD0;
	TH1F *hErrCotTheta0_2;
	TProfile *hErrCotTheta0VsEta_2;
	TProfile *hErrCotTheta0VsPt_2;
	TProfile *hErrCotTheta0VsP_2;
	TProfile *hErrCotTheta0VsPhi0_2;
	//TProfile *hErrCotTheta0VsD0_2;
	
	cout << " Error CotTheta vs track parameters..." << endl;
	
	TString histname;
	Color_t Color;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	if(!cErrCotTheta)
		cErrCotTheta = new TCanvas("cErrCotTheta","Error Phi0",
							CanvasSizeX6[0],CanvasSizeX6[1]);
							
	cErrCotTheta->Divide(3,2);

	cErrCotTheta->cd(1);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errCotTheta";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrCotTheta0 = (TH1F*)file->Get(histname);
	hErrCotTheta0->SetTitle("Cot Theta Error");
	hErrCotTheta0->GetXaxis()->SetTitle("#sigma (cot(#theta))");
	hErrCotTheta0->GetYaxis()->SetTitle("Tracks");
	hErrCotTheta0->SetLineColor(Color+2);
	hErrCotTheta0->SetFillColor(Color);
	hErrCotTheta0->SetFillStyle(3001);
	hErrCotTheta0->SetMinimum(0.);
	if (normalizePlots) hErrCotTheta0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrCotTheta0->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();

	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errCotTheta";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrCotTheta0_2 = (TH1F*)file->Get(histname);	
		hErrCotTheta0_2->SetLineColor(ColorForSecond+2);
		hErrCotTheta0_2->SetFillColor(ColorForSecond);
		hErrCotTheta0_2->SetLineWidth(1);
		hErrCotTheta0_2->SetFillStyle(3001);
		if (normalizePlots) hErrCotTheta0_2->Scale(1./(float)iEvents);
		hErrCotTheta0_2->DrawCopy("histsame");
	}

	//
	// -----------------------------------------------------------------
	//

	cErrCotTheta->cd(2);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errCotThetaVsEta";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrCotTheta0VsEta =((TH2D*)file->Get(histname))->ProfileX();
	hErrCotTheta0VsEta->SetTitle("#sigma (cot(#theta)) vs #eta");
	hErrCotTheta0VsEta->GetXaxis()->SetTitle("#eta");
	hErrCotTheta0VsEta->GetYaxis()->SetTitle("#sigma (cot(#theta))");
	hErrCotTheta0VsEta->SetLineColor(Color+2);
	hErrCotTheta0VsEta->SetFillColor(Color);
	hErrCotTheta0VsEta->SetFillStyle(3001);
	hErrCotTheta0VsEta->SetMinimum(0.);
	if (normalizePlots) hErrCotTheta0VsEta->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrCotTheta0VsEta->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errCotThetaVsEta";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrCotTheta0VsEta_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrCotTheta0VsEta_2->SetLineColor(ColorForSecond+2);
		hErrCotTheta0VsEta_2->SetFillColor(ColorForSecond);
		hErrCotTheta0VsEta_2->SetLineWidth(1);
		hErrCotTheta0VsEta_2->SetFillStyle(3001);
		if (normalizePlots) hErrCotTheta0VsEta_2->Scale(1./(float)iEvents);
		hErrCotTheta0VsEta_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//
	
	cErrCotTheta->cd(3);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errCotThetaVsPt";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrCotTheta0VsPt = ((TH2D*)file->Get(histname))->ProfileX();
	hErrCotTheta0VsPt->SetTitle("#sigma (cot(#theta)) vs pT");
	hErrCotTheta0VsPt->GetXaxis()->SetTitle("pT (GeV)");
	hErrCotTheta0VsPt->GetYaxis()->SetTitle("#sigma (cot(#theta))");
	hErrCotTheta0VsPt->SetLineColor(Color+2);
	hErrCotTheta0VsPt->SetFillColor(Color);
	hErrCotTheta0VsPt->SetFillStyle(3001);
	hErrCotTheta0VsPt->SetMinimum(0.);
	if (normalizePlots) hErrCotTheta0VsPt->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrCotTheta0VsPt->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errCotThetaVsPt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrCotTheta0VsPt_2 =((TH2D*)file->Get(histname))->ProfileX();
		hErrCotTheta0VsPt_2->SetLineColor(ColorForSecond+2);
		hErrCotTheta0VsPt_2->SetFillColor(ColorForSecond);
		hErrCotTheta0VsPt_2->SetLineWidth(1);
		hErrCotTheta0VsPt_2->SetFillStyle(3001);
		if (normalizePlots) hErrCotTheta0VsPt_2->Scale(1./(float)iEvents);
		hErrCotTheta0VsPt_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cErrCotTheta->cd(4);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errCotThetaVsP";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrCotTheta0VsP = ((TH2D*)file->Get(histname))->ProfileX();
	//gPad->SetLogy();
	hErrCotTheta0VsP->SetTitle("#sigma (cot(#theta)) vs p");
	hErrCotTheta0VsP->GetXaxis()->SetTitle("p (GeV)");
	hErrCotTheta0VsP->GetYaxis()->SetTitle("#sigma (cot(#theta))");
	hErrCotTheta0VsP->SetLineColor(Color+2);
	hErrCotTheta0VsP->SetFillColor(Color);
	hErrCotTheta0VsP->SetFillStyle(3001);
	hErrCotTheta0VsP->SetMinimum(0.);
	if (normalizePlots) hErrCotTheta0VsP->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrCotTheta0VsP->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errCotThetaVsP";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrCotTheta0VsP_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrCotTheta0VsP_2->SetLineColor(ColorForSecond+2);
		hErrCotTheta0VsP_2->SetFillColor(ColorForSecond);
		hErrCotTheta0VsP_2->SetLineWidth(1);
		hErrCotTheta0VsP_2->SetFillStyle(3001);
		if (normalizePlots) hErrCotTheta0VsP_2->Scale(1./(float)iEvents);
		hErrCotTheta0VsP_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cErrCotTheta->cd(5);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errCotThetaVsPhi";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrCotTheta0VsPhi0 = ((TH2D*)file->Get(histname))->ProfileX();
	//gPad->SetLogy();
	hErrCotTheta0VsPhi0->SetTitle("#sigma (cot(#theta)) vs #phi0");
	hErrCotTheta0VsPhi0->GetXaxis()->SetTitle("#phi0 (rad)");
	hErrCotTheta0VsPhi0->GetYaxis()->SetTitle("#sigma (cot(#theta))");
	hErrCotTheta0VsPhi0->SetLineColor(Color+2);
	hErrCotTheta0VsPhi0->SetFillColor(Color);
	hErrCotTheta0VsPhi0->SetFillStyle(3001);
	hErrCotTheta0VsPhi0->SetMinimum(0.);
	if (normalizePlots) hErrCotTheta0VsPhi0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrCotTheta0VsPhi0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errCotThetaVsPhi";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrCotTheta0VsPhi0_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrCotTheta0VsPhi0_2->SetLineColor(ColorForSecond+2);
		hErrCotTheta0VsPhi0_2->SetFillColor(ColorForSecond);
		hErrCotTheta0VsPhi0_2->SetLineWidth(1);
		hErrCotTheta0VsPhi0_2->SetFillStyle(3001);
		if (normalizePlots) hErrCotTheta0VsPhi0_2->Scale(1./(float)iEvents);
		hErrCotTheta0VsPhi0_2->DrawCopy("same");
	}

	

	//
	// -----------------------------------------------------------------
	//
/*
	cErrCotTheta->cd(6);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errCotThetaVsD0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrCotTheta0VsD0 = (TH1F*)file->Get(histname);	
	hErrCotTheta0VsD0->SetTitle("#sigma (cot(#theta)) vs d0");
	hErrCotTheta0VsD0->GetXaxis()->SetTitle("#sigma (cot(#theta)) vs d0");
	hErrCotTheta0VsD0->GetYaxis()->SetTitle("#sigma (cot(#theta))");
	hErrCotTheta0VsD0->SetLineColor(Color+2);
	hErrCotTheta0VsD0->SetFillColor(Color);
	hErrCotTheta0VsD0->SetFillStyle(3001);
	if (normalizePlots) hErrCotTheta0VsD0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrCotTheta0VsD0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errCotThetaVsD0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrCotTheta0VsD0_2 = (TH1F*)file->Get(histname);	
		hErrCotTheta0VsD0_2->SetLineColor(ColorForSecond+2);
		hErrCotTheta0VsD0_2->SetFillColor(ColorForSecond);
		hErrCotTheta0VsD0_2->SetLineWidth(1);
		hErrCotTheta0VsD0_2->SetFillStyle(3001);
		if (normalizePlots) hErrCotTheta0VsD0_2->Scale(1./(float)iEvents);
		hErrCotTheta0VsD0_2->DrawCopy("same");
	}


*/
	cErrCotTheta->Update();
	gStyle->SetOptStat(111110);
}
