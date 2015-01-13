/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void errPt(TString trkCol){
	errPt(trkCol, 0, "");
}

void errPt(TString trkCol, TString trkCol2){
	errPt(trkCol, 0, trkCol2);
}

void errPt(TString trkCol, TCanvas *cErrPt){
	errPt(trkCol, cErrPt, "");
}

void errPt(TString trkCol, TCanvas *cErrPt, TString trkCol2) {
	gStyle->SetOptStat(0);

	TH1F *hErrPt;
	TProfile *hErrPtVsEta;
	TProfile *hErrPtVsPt;
	//TProfile *hErrPtVsP;
	TProfile *hErrPtVsPhi0;
	//TProfile *hErrPtVsD0;
	TH1F *hErrPt_2;
	TProfile *hErrPtVsEta_2;
	TProfile *hErrPtVsPt_2;
	//TProfile *hErrPtVsP_2;
	TProfile *hErrPtVsPhi0_2;
	//TProfile *hErrPtVsD0_2;
	cout << " Track Parameters..." << endl;
	
	TString histname;
	Color_t Color;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	if(!cErrPt)
		cErrPt = new TCanvas("cErrPt","Error Pt",
							CanvasSizeX6[0],CanvasSizeX6[1]);
							
	cErrPt->Divide(3,2);

	cErrPt->cd(1);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPt";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPt = (TH1F*)file->Get(histname);
	hErrPt->SetTitle("Pt err");
	hErrPt->GetXaxis()->SetTitle("#sigma (pT) (GeV)");
	hErrPt->GetYaxis()->SetTitle("Tracks");
	hErrPt->SetLineColor(Color+2);
	hErrPt->SetFillColor(Color);
	hErrPt->SetFillStyle(3001);
	hErrPt->SetMinimum(0.);
	if (normalizePlots) hErrPt->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPt->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();

	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPt_2 = (TH1F*)file->Get(histname);	
		hErrPt_2->SetLineColor(ColorForSecond+2);
		hErrPt_2->SetFillColor(ColorForSecond);
		hErrPt_2->SetLineWidth(1);
		hErrPt_2->SetFillStyle(3001);
		if (normalizePlots) hErrPt_2->Scale(1./(float)iEvents);
		hErrPt_2->DrawCopy("histsame");
	}

	//
	// -----------------------------------------------------------------
	//

	cErrPt->cd(2);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPtVsEta";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPtVsEta =((TH2D*)file->Get(histname))->ProfileX();
	hErrPtVsEta->SetTitle("#sigma(pT) vs #eta");
	hErrPtVsEta->GetXaxis()->SetTitle("#eta");
	hErrPtVsEta->GetYaxis()->SetTitle("#sigma(pT) (GeV)");
	hErrPtVsEta->SetLineColor(Color+2);
	hErrPtVsEta->SetFillColor(Color);
	hErrPtVsEta->SetFillStyle(3001);
	hErrPtVsEta->SetMinimum(0.);
	if (normalizePlots) hErrPtVsEta->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPtVsEta->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPtVsEta";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPtVsEta_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrPtVsEta_2->SetLineColor(ColorForSecond+2);
		hErrPtVsEta_2->SetFillColor(ColorForSecond);
		hErrPtVsEta_2->SetLineWidth(1);
		hErrPtVsEta_2->SetFillStyle(3001);
		if (normalizePlots) hErrPtVsEta_2->Scale(1./(float)iEvents);
		hErrPtVsEta_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//
	
	cErrPt->cd(3);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPtVsPt";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPtVsPt = ((TH2D*)file->Get(histname))->ProfileX();
	hErrPtVsPt->SetTitle("#sigma(pT) vs pT");
	hErrPtVsPt->GetXaxis()->SetTitle("pT (GeV)");
	hErrPtVsPt->GetYaxis()->SetTitle("#sigma(pT) (GeV)");
	hErrPtVsPt->SetLineColor(Color+2);
	hErrPtVsPt->SetFillColor(Color);
	hErrPtVsPt->SetFillStyle(3001);
	hErrPtVsPt->SetMinimum(0.);
	if (normalizePlots) hErrPtVsPt->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPtVsPt->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPtVsPt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPtVsPt_2 =((TH2D*)file->Get(histname))->ProfileX();
		hErrPtVsPt_2->SetLineColor(ColorForSecond+2);
		hErrPtVsPt_2->SetFillColor(ColorForSecond);
		hErrPtVsPt_2->SetLineWidth(1);
		hErrPtVsPt_2->SetFillStyle(3001);
		if (normalizePlots) hErrPtVsPt_2->Scale(1./(float)iEvents);
		hErrPtVsPt_2->DrawCopy("same");
	}
	
	//
	// -----------------------------------------------------------------
	//
/*
	cErrPt->cd(4);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPtVsP";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPtVsP = ((TH2D*)file->Get(histname))->ProfileX();
	//gPad->SetLogy();
	hErrPtVsP->SetTitle("#sigma(pT) vs p");
	hErrPtVsP->GetXaxis()->SetTitle("p (GeV)");
	hErrPtVsP->GetYaxis()->SetTitle("#sigma(pT) (GeV)");
	hErrPtVsP->SetLineColor(Color+2);
	hErrPtVsP->SetFillColor(Color);
	hErrPtVsP->SetFillStyle(3001);
	hErrPtVsP->SetMinimum(0.);
	if (normalizePlots) hErrPtVsP->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPtVsP->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPtVsP";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPtVsP_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrPtVsP_2->SetLineColor(ColorForSecond+2);
		hErrPtVsP_2->SetFillColor(ColorForSecond);
		hErrPtVsP_2->SetLineWidth(1);
		hErrPtVsP_2->SetFillStyle(3001);
		if (normalizePlots) hErrPtVsP_2->Scale(1./(float)iEvents);
		hErrPtVsP_2->DrawCopy("same");
	}
	*/
	//
	// -----------------------------------------------------------------
	//

	cErrPt->cd(5);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPtVsPhi0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPtVsPhi0 = ((TH2D*)file->Get(histname))->ProfileX();
	//gPad->SetLogy();
	hErrPtVsPhi0->SetTitle("#sigma(pT) vs #phi0");
	hErrPtVsPhi0->GetXaxis()->SetTitle("#phi0 (rad)");
	hErrPtVsPhi0->GetYaxis()->SetTitle("#sigma(pT) (GeV)");
	hErrPtVsPhi0->SetLineColor(Color+2);
	hErrPtVsPhi0->SetFillColor(Color);
	hErrPtVsPhi0->SetFillStyle(3001);
	hErrPtVsPhi0->SetMinimum(0.);
	if (normalizePlots) hErrPtVsPhi0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPtVsPhi0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPtVsPhi0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPtVsPhi0_2 = ((TH2D*)file->Get(histname))->ProfileX();
		hErrPtVsPhi0_2->SetLineColor(ColorForSecond+2);
		hErrPtVsPhi0_2->SetFillColor(ColorForSecond);
		hErrPtVsPhi0_2->SetLineWidth(1);
		hErrPtVsPhi0_2->SetFillStyle(3001);
		if (normalizePlots) hErrPtVsPhi0_2->Scale(1./(float)iEvents);
		hErrPtVsPhi0_2->DrawCopy("same");
	}

	

	//
	// -----------------------------------------------------------------
	//
/*
	cErrPt->cd(6);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errPtVsD0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrPtVsD0 = (TH1F*)file->Get(histname);	
	hErrPtVsD0->SetTitle("#sigma(pT) vs d0");
	hErrPtVsD0->GetXaxis()->SetTitle("#sigma(pT) vs d0");
	hErrPtVsD0->GetYaxis()->SetTitle("#sigma(pT) (GeV)");
	hErrPtVsD0->SetLineColor(Color+2);
	hErrPtVsD0->SetFillColor(Color);
	hErrPtVsD0->SetFillStyle(3001);
	if (normalizePlots) hErrPtVsD0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrPtVsD0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errPtVsD0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrPtVsD0_2 = (TH1F*)file->Get(histname);	
		hErrPtVsD0_2->SetLineColor(ColorForSecond+2);
		hErrPtVsD0_2->SetFillColor(ColorForSecond);
		hErrPtVsD0_2->SetLineWidth(1);
		hErrPtVsD0_2->SetFillStyle(3001);
		if (normalizePlots) hErrPtVsD0_2->Scale(1./(float)iEvents);
		hErrPtVsD0_2->DrawCopy("same");
	}


*/
	cErrPt->Update();
	gStyle->SetOptStat(111110);
}
