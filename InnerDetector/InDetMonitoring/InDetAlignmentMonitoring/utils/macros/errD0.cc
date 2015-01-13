/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void errD0(TString trkCol){
	errD0(trkCol, 0, "");
}

void errD0(TString trkCol, TString trkCol2){
	errD0(trkCol, 0, trkCol2);
}

void errD0(TString trkCol, TCanvas *cErrD0){
	errD0(trkCol, cErrD0, "");
}

void errD0(TString trkCol, TCanvas *cErrD0, TString trkCol2) {
	gStyle->SetOptStat(0);
	TH1F *hErrD0;
	TH2D *hTemp;
	TProfile *hErrD0VsEta;
	TProfile *hErrD0VsPt;
	TProfile *hErrD0VsP;
	TProfile *hErrD0VsPhi0;
	//TProfile *hErrD0VsD0;
	TH1F *hErrD0_2;
	TProfile *hErrD0VsEta_2;
	TProfile *hErrD0VsPt_2;
	TProfile *hErrD0VsP_2;
	TProfile *hErrD0VsPhi0_2;
	//TProfile *hErrD0VsD0_2;
	cout << " Error d0..." << endl;
	
	TString histname;
	Color_t Color;
	bool IsHistogramMissing =false;
	int  missinghistopad = 0;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	if(!cErrD0)
		cErrD0 = new TCanvas("cErrD0","Error D0",
							CanvasSizeX6[0],CanvasSizeX6[1]);
							
	cErrD0->Divide(3,2);

	cErrD0->cd(1);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0";
	if(PrintLevel>0) cout << " <errD0> Plotting Histogram: " << histname << endl;
	hErrD0 = (TH1F*)file->Get(histname);
	if (hErrD0 != NULL ) {
	  hErrD0->SetTitle("D0 err");
	  hErrD0->GetXaxis()->SetTitle("#sigma (d0) (mm)");
	  hErrD0->GetYaxis()->SetTitle("Tracks");
	  hErrD0->SetLineColor(Color+2);
	  hErrD0->SetFillColor(Color);
	  hErrD0->SetFillStyle(3001);
	  hErrD0->SetMinimum(0.);
	  if (normalizePlots) hErrD0->Scale(1./(float)iEvents);
	  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	  hErrD0->DrawCopy("hist");
	  setTitleFont(font);
	  ScalePlots();

	  if(trkCol2 !=""){
	    histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0";
	    if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	    hErrD0_2 = (TH1F*)file->Get(histname);	
	    hErrD0_2->SetLineColor(ColorForSecond+2);
	    hErrD0_2->SetFillColor(ColorForSecond);
	    hErrD0_2->SetLineWidth(1);
	    hErrD0_2->SetFillStyle(3001);
	    if (normalizePlots) hErrD0_2->Scale(1./(float)iEvents);
	    hErrD0_2->DrawCopy("histsame");
	  }
	}
	else {
	  if (!IsHistogramMissing) {
	    IsHistogramMissing = true;
	    missinghistopad = 1;
	  } 
	}
	//
	// -----------------------------------------------------------------
	//

	cErrD0->cd(2);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsEta";
	hTemp = NULL;
	hTemp = (TH2D*)file->Get(histname);
	if (hTemp != NULL) {
	  hTemp->Print();
	  hErrD0VsEta = hTemp->ProfileX();
	  hErrD0VsEta->SetTitle("#sigma(d0) vs #eta");
	  hErrD0VsEta->GetXaxis()->SetTitle("#eta");
	  hErrD0VsEta->GetYaxis()->SetTitle("#sigma(d0) (mm)");
	  hErrD0VsEta->SetLineColor(Color+2);
	  hErrD0VsEta->SetFillColor(Color);
	  hErrD0VsEta->SetFillStyle(3001);
	  hErrD0VsEta->SetMinimum(0.);
	  if (normalizePlots) hErrD0VsEta->Scale(1./(float)iEvents);
	  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	  hErrD0VsEta->DrawCopy("");
	  setTitleFont(font);
	  ScalePlots();
	  if(trkCol2 !=""){
	    histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsEta";
	    if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	    hErrD0VsEta_2 = ((TH2D*)file->Get(histname))->ProfileX();
	    hErrD0VsEta_2->SetLineColor(ColorForSecond+2);
	    hErrD0VsEta_2->SetFillColor(ColorForSecond);
	    hErrD0VsEta_2->SetLineWidth(1);
	    hErrD0VsEta_2->SetFillStyle(3001);
	    if (normalizePlots) hErrD0VsEta_2->Scale(1./(float)iEvents);
	    hErrD0VsEta_2->DrawCopy("same");
	  }
	}
	else {
	  if (!IsHistogramMissing) {
	    IsHistogramMissing = true;
	    missinghistopad = 2;
	  } 
	}
	  //
	// -----------------------------------------------------------------
	//
	
	cErrD0->cd(3);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsPt";
	hTemp = NULL;
	hTemp = (TH2D*)file->Get(histname);
	if (hTemp != NULL) {
	  if(PrintLevel>0) cout << " <errD0> Plotting Histogram: " << histname << endl;
	  hErrD0VsPt = hTemp->ProfileX();
	  hErrD0VsPt->SetTitle("#sigma(d0) vs pT");
	  hErrD0VsPt->GetXaxis()->SetTitle("pT (GeV)");
	  hErrD0VsPt->GetYaxis()->SetTitle("#sigma(d0 (mm)");
	  hErrD0VsPt->SetLineColor(Color+2);
	  hErrD0VsPt->SetFillColor(Color);
	  hErrD0VsPt->SetFillStyle(3001);
	  hErrD0VsPt->SetMinimum(0.);
	  if (normalizePlots) hErrD0VsPt->Scale(1./(float)iEvents);
	  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	  hErrD0VsPt->DrawCopy("");
	  setTitleFont(font);
	  ScalePlots();
	  if(trkCol2 !=""){
	    histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsPt";
	    if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	    hErrD0VsPt_2 =((TH2D*)file->Get(histname))->ProfileX();
	    hErrD0VsPt_2->SetLineColor(ColorForSecond+2);
	    hErrD0VsPt_2->SetFillColor(ColorForSecond);
	    hErrD0VsPt_2->SetLineWidth(1);
	    hErrD0VsPt_2->SetFillStyle(3001);
	    if (normalizePlots) hErrD0VsPt_2->Scale(1./(float)iEvents);
	    hErrD0VsPt_2->DrawCopy("same");
	  }
	}
	else {
	  if (!IsHistogramMissing) {
	    IsHistogramMissing = true;
	    missinghistopad = 3;
	  } 
	}

	//
	// -----------------------------------------------------------------
	//

	cErrD0->cd(4);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsP";
	hTemp = NULL;
	hTemp = (TH2D*)file->Get(histname);
	if (hTemp != NULL) {
	  if(PrintLevel>0) cout << " <errD0> Plotting Histogram: " << histname << endl;
	  hErrD0VsP = hTemp->ProfileX();
	  hErrD0VsP->SetTitle("#sigma(d0) vs p");
	  hErrD0VsP->GetXaxis()->SetTitle("p (GeV)");
	  hErrD0VsP->GetYaxis()->SetTitle("#sigma(d0 (mm)");
	  hErrD0VsP->SetLineColor(Color+2);
	  hErrD0VsP->SetFillColor(Color);
	  hErrD0VsP->SetFillStyle(3001);
	  hErrD0VsP->SetMinimum(0.);
	  if (normalizePlots) hErrD0VsP->Scale(1./(float)iEvents);
	  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	  hErrD0VsP->DrawCopy("");
	  setTitleFont(font);
	  ScalePlots();
	  if(trkCol2 !=""){
	    histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsP";
	    if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	    hErrD0VsP_2 = ((TH2D*)file->Get(histname))->ProfileX();
	    hErrD0VsP_2->SetLineColor(ColorForSecond+2);
	    hErrD0VsP_2->SetFillColor(ColorForSecond);
	    hErrD0VsP_2->SetLineWidth(1);
	    hErrD0VsP_2->SetFillStyle(3001);
	    if (normalizePlots) hErrD0VsP_2->Scale(1./(float)iEvents);
	    hErrD0VsP_2->DrawCopy("same");
	  }
	}
	else {
	  if (!IsHistogramMissing) {
	    IsHistogramMissing = true;
	    missinghistopad = 4;
	  } 
	}

	//
	// -----------------------------------------------------------------
	//

	cErrD0->cd(5);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsPhi";
	hTemp = NULL;
	hTemp = (TH2D*)file->Get(histname);
	if (hTemp != NULL) {
	  if(PrintLevel>0) cout << " <errD0> Plotting Histogram: " << histname << endl;
	  hErrD0VsPhi0 = hTemp->ProfileX();
	  hErrD0VsPhi0->SetTitle("#sigma(d0) vs #phi0");
	  hErrD0VsPhi0->GetXaxis()->SetTitle("#phi0 (rad)");
	  hErrD0VsPhi0->GetYaxis()->SetTitle("#sigma(d0 (mm)");
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
	    hErrD0VsPhi0_2 = ((TH2D*)file->Get(histname))->ProfileX();
	    hErrD0VsPhi0_2->SetLineColor(ColorForSecond+2);
	    hErrD0VsPhi0_2->SetFillColor(ColorForSecond);
	    hErrD0VsPhi0_2->SetLineWidth(1);
	    hErrD0VsPhi0_2->SetFillStyle(3001);
	    if (normalizePlots) hErrD0VsPhi0_2->Scale(1./(float)iEvents);
	    hErrD0VsPhi0_2->DrawCopy("same");
	  }
	}
	else {
	  if (!IsHistogramMissing) {
	    IsHistogramMissing = true;
	    missinghistopad = 5;
	  } 
	}
 

	//
	// -----------------------------------------------------------------
	//
/*
	cErrD0->cd(6);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/errD0VsD0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hErrD0VsD0 = (TH1F*)file->Get(histname);	
	hErrD0VsD0->SetTitle("#sigma(d0) vs d0");
	hErrD0VsD0->GetXaxis()->SetTitle("#sigma(d0) vs d0");
	hErrD0VsD0->GetYaxis()->SetTitle("#sigma(d0 (mm)");
	hErrD0VsD0->SetLineColor(Color+2);
	hErrD0VsD0->SetFillColor(Color);
	hErrD0VsD0->SetFillStyle(3001);
	if (normalizePlots) hErrD0VsD0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hErrD0VsD0->DrawCopy("");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/errD0VsD0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hErrD0VsD0_2 = (TH1F*)file->Get(histname);	
		hErrD0VsD0_2->SetLineColor(ColorForSecond+2);
		hErrD0VsD0_2->SetFillColor(ColorForSecond);
		hErrD0VsD0_2->SetLineWidth(1);
		hErrD0VsD0_2->SetFillStyle(3001);
		if (normalizePlots) hErrD0VsD0_2->Scale(1./(float)iEvents);
		hErrD0VsD0_2->DrawCopy("same");
	}


*/
	cErrD0->Update();
	gStyle->SetOptStat(111110);

	// if some histo is missing
	if (IsHistogramMissing) {
	    TH1F* hBlank = new TH1F ("hBlank"," ",1, 0., 1.);
	    cErrD0->cd(missinghistopad);
	    hBlank->SetMaximum(1.);
	    hBlank->SetMinimum(-1.);
	    hBlank->Draw();
	    TText myWarning;
	    myWarning.DrawText(0., 0., " errD0.cc PROBLEMS. PLEASE CHECK ");	    
	}
	return;
}
