/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sct_trt_Residuals(TString trkCol){
	sct_trt_Residuals(trkCol, 0, "");
}

void sct_trt_Residuals(TString trkCol, TString trkCol2){
	sct_trt_Residuals(trkCol, 0, trkCol2);
}

void sct_trt_Residuals(TString trkCol, TCanvas *cSctTRTRes){
	sct_trt_Residuals(trkCol, cSctTRTRes, "");
}

void sct_trt_Residuals(TString trkCol, TCanvas *cSctTRTRes, TString trkCol2) {
	TH1F *hSCT_TrkRes;
	TH1F *hSCT_EndCapATrkRes;
	TH1F *hSCT_EndCapCTrkRes;
	
	TH1F *hTRT_TrkRes;
	TH1F *hTRT_EndCapATrkRes;
	TH1F *hTRT_EndCapCTrkRes;

	TH1F *hSCT_TrkRes_2;
	TH1F *hSCT_EndCapATrkRes_2;
	TH1F *hSCT_EndCapCTrkRes_2;
	
	TH1F *hTRT_TrkRes_2;
	TH1F *hTRT_EndCapATrkRes_2;
	TH1F *hTRT_EndCapCTrkRes_2;
	
	TString histname;
	TString histtitle;

	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cSctTRTRes){
		cSctTRTRes = new TCanvas("SctTRTResiduals","SCT and TRT Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
	}
	cSctTRTRes->Divide(3,2);
	//
	// -----------------------------------------------------------------
	//

	cSctTRTRes->cd(1);
	histname = "IDAlignMon/"+trkCol+"/Residuals/sct_ecc_biased_residualx";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hSCT_EndCapCTrkRes = (TH1F*)file->Get(histname);
	if (hSCT_EndCapCTrkRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}

	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": SCT ECC: Biased Residuals";
	hSCT_EndCapCTrkRes->SetTitle(histtitle.Data());
	hSCT_EndCapCTrkRes->GetXaxis()->SetTitle("Residuals [mm]");
	hSCT_EndCapCTrkRes->GetYaxis()->SetTitle("Entries");
	hSCT_EndCapCTrkRes->SetLineColor(Color+2);
	hSCT_EndCapCTrkRes->SetFillColor(Color);
	hSCT_EndCapCTrkRes->SetLineWidth(1);
	hSCT_EndCapCTrkRes->SetFillStyle(3001);
	if (normalizePlots) hSCT_EndCapCTrkRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hSCT_EndCapCTrkRes->GetMean()!=0.0) {
	 hSCT_EndCapCTrkRes->DrawCopy("hist");	
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_ecc_biased_residualx";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hSCT_EndCapCTrkRes_2 = (TH1F*)file->Get(histname);	
		hSCT_EndCapCTrkRes_2->SetLineColor(ColorForSecond+2);
		hSCT_EndCapCTrkRes_2->SetFillColor(ColorForSecond);
		hSCT_EndCapCTrkRes_2->SetLineWidth(1);
		hSCT_EndCapCTrkRes_2->SetFillStyle(3001);
		if (normalizePlots) hSCT_EndCapCTrkRes_2->Scale(1./(float)iEvents);
		hSCT_EndCapCTrkRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hSCT_EndCapCTrkRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"SCT EndCap C");
	 t->DrawLatex(0.3,0.5,name);
	 sprintf(name,"no processed");
	 t->DrawLatex(0.33,0.44,name);

	 TLine *TLineBorder;
	 TLineBorder = new TLine(0.05,0.90,0.92,0.90);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.10,0.92,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.90,0.05,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.92,0.90,0.92,0.10);
	 TLineBorder->Draw();
	}

	//
	// -----------------------------------------------------------------
	//


	cSctTRTRes->cd(2);
	
	histname = "IDAlignMon/"+trkCol+"/Residuals/sct_b_biasedresidualx";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hSCT_TrkRes = (TH1F*)file->Get(histname);	
	if (hSCT_TrkRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}

	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": SCT Barrel: Biased Residuals";
	hSCT_TrkRes->SetTitle(histtitle.Data());
	hSCT_TrkRes->GetXaxis()->SetTitle("Residuals [mm]");
	hSCT_TrkRes->GetYaxis()->SetTitle("Entries");
	hSCT_TrkRes->SetLineColor(Color+2);
	hSCT_TrkRes->SetFillColor(Color);
	hSCT_TrkRes->SetLineWidth(1);
	hSCT_TrkRes->SetFillStyle(3001);
	if (normalizePlots) hSCT_TrkRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hSCT_TrkRes->GetMean()!=0.0) {
	 hSCT_TrkRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_b_biasedresidualx";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hSCT_TrkRes_2 = (TH1F*)file->Get(histname);	
		hSCT_TrkRes_2->SetLineColor(ColorForSecond+2);
		hSCT_TrkRes_2->SetFillColor(ColorForSecond);
		hSCT_TrkRes_2->SetLineWidth(1);
		hSCT_TrkRes_2->SetFillStyle(3001);
		if (normalizePlots) hSCT_TrkRes_2->Scale(1./(float)iEvents);
		hSCT_TrkRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hSCT_TrkRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"SCT Barrel");
	 t->DrawLatex(0.3,0.5,name);
	 sprintf(name,"no processed");
	 t->DrawLatex(0.33,0.44,name);

	 TLine *TLineBorder;
	 TLineBorder = new TLine(0.05,0.90,0.92,0.90);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.10,0.92,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.90,0.05,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.92,0.90,0.92,0.10);
	 TLineBorder->Draw();
	}

	//
	// -----------------------------------------------------------------
	//
	
	cSctTRTRes->cd(3);
	histname = "IDAlignMon/"+trkCol+"/Residuals/sct_eca_biased_residualx";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	
	hSCT_EndCapATrkRes = (TH1F*)file->Get(histname);	
	if (hSCT_EndCapATrkRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}

	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": SCT ECA: Biased Residuals";
	hSCT_EndCapATrkRes->SetTitle(histtitle.Data());
	hSCT_EndCapATrkRes->GetXaxis()->SetTitle("Residuals [mm]");
	hSCT_EndCapATrkRes->GetYaxis()->SetTitle("Entries");
	hSCT_EndCapATrkRes->SetLineColor(Color+2);
	hSCT_EndCapATrkRes->SetFillColor(Color);
	hSCT_EndCapATrkRes->SetLineWidth(1);
	hSCT_EndCapATrkRes->SetFillStyle(3001);
	if (normalizePlots) hSCT_EndCapATrkRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hSCT_EndCapATrkRes->GetMean()!=0.0) {
	 hSCT_EndCapATrkRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_eca_biased_residualx";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hSCT_EndCapATrkRes_2 = (TH1F*)file->Get(histname);	
		hSCT_EndCapATrkRes_2->SetLineColor(ColorForSecond+2);
		hSCT_EndCapATrkRes_2->SetFillColor(ColorForSecond);
		hSCT_EndCapATrkRes_2->SetLineWidth(1);
		hSCT_EndCapATrkRes_2->SetFillStyle(3001);
		if (normalizePlots) hSCT_EndCapATrkRes_2->Scale(1./(float)iEvents);
		hSCT_EndCapATrkRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hSCT_EndCapATrkRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"SCT ECA");
	 t->DrawLatex(0.3,0.5,name);
	 sprintf(name,"no processed");
	 t->DrawLatex(0.33,0.44,name);
	 
	 TLine *TLineBorder;
	 TLineBorder = new TLine(0.05,0.90,0.92,0.90);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.10,0.92,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.90,0.05,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.92,0.90,0.92,0.10);
	 TLineBorder->Draw();
	}

	//
	// -----------------------------------------------------------------
	//
	
	cSctTRTRes->cd(4);
	histname = "IDAlignMon/"+trkCol+"/Residuals/trt_ec_residualR_Endcap_C";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTRT_EndCapCTrkRes = (TH1F*)file->Get(histname);	
	if (hTRT_EndCapCTrkRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": TRT ECC: Unbiased Residuals";
	hTRT_EndCapCTrkRes->SetTitle(histtitle.Data());
	hTRT_EndCapCTrkRes->GetXaxis()->SetTitle("Residuals [mm]");
	hTRT_EndCapCTrkRes->GetYaxis()->SetTitle("Entries");
	hTRT_EndCapCTrkRes->SetLineColor(Color+2);
	hTRT_EndCapCTrkRes->SetFillColor(Color);
	hTRT_EndCapCTrkRes->SetLineWidth(1);
	hTRT_EndCapCTrkRes->SetFillStyle(3001);
	if (normalizePlots) hTRT_EndCapCTrkRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hTRT_EndCapCTrkRes->GetMean()!=0.0) {
	 hTRT_EndCapCTrkRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/trt_ec_residualR_Endcap_C";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTRT_EndCapCTrkRes_2 = (TH1F*)file->Get(histname);	
		hTRT_EndCapCTrkRes_2->SetLineColor(ColorForSecond+2);
		hTRT_EndCapCTrkRes_2->SetFillColor(ColorForSecond);
		hTRT_EndCapCTrkRes_2->SetLineWidth(1);
		hTRT_EndCapCTrkRes_2->SetFillStyle(3001);
		if (normalizePlots) hTRT_EndCapCTrkRes_2->Scale(1./(float)iEvents);
		hTRT_EndCapCTrkRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hTRT_EndCapCTrkRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"TRT EndCap C");
	 t->DrawLatex(0.3,0.5,name);
	 sprintf(name,"no processed");
	 t->DrawLatex(0.33,0.44,name);

	 TLine *TLineBorder;
	 TLineBorder = new TLine(0.05,0.90,0.92,0.90);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.10,0.92,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.90,0.05,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.92,0.90,0.92,0.10);
	 TLineBorder->Draw();
	}
	
	cSctTRTRes->cd(5);
	histname = "IDAlignMon/"+trkCol+"/Residuals/trt_b_residualR";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTRT_TrkRes = (TH1F*)file->Get(histname);	
	if (hTRT_TrkRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": TRT Barrel: Unbiased Residuals";
	hTRT_TrkRes->SetTitle(histtitle.Data());
	hTRT_TrkRes->GetXaxis()->SetTitle("Residuals [mm]");
	hTRT_TrkRes->GetYaxis()->SetTitle("Entries");
	hTRT_TrkRes->SetLineColor(Color+2);
	hTRT_TrkRes->SetLineWidth(1);
	hTRT_TrkRes->SetFillColor(Color);
	hTRT_TrkRes->SetFillStyle(3001);
	if (normalizePlots) hTRT_TrkRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hTRT_TrkRes->GetMean()!=0.0) {
	 hTRT_TrkRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/trt_b_residualR";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTRT_TrkRes_2 = (TH1F*)file->Get(histname);	
		hTRT_TrkRes_2->SetLineColor(ColorForSecond+2);
		hTRT_TrkRes_2->SetFillColor(ColorForSecond);
		hTRT_TrkRes_2->SetLineWidth(1);
		hTRT_TrkRes_2->SetFillStyle(3001);
		if (normalizePlots) hTRT_TrkRes_2->Scale(1./(float)iEvents);
		hTRT_TrkRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hTRT_TrkRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"TRT Barrel");
	 t->DrawLatex(0.3,0.5,name);
	 sprintf(name,"no processed");
	 t->DrawLatex(0.33,0.44,name);

	 TLine *TLineBorder;
	 TLineBorder = new TLine(0.05,0.90,0.92,0.90);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.10,0.92,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.90,0.05,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.92,0.90,0.92,0.10);
	 TLineBorder->Draw();
	}

	//
	// -----------------------------------------------------------------
	//


	cSctTRTRes->cd(6);
	histname = "IDAlignMon/"+trkCol+"/Residuals/trt_ec_residualR_Endcap_A";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTRT_EndCapATrkRes = (TH1F*)file->Get(histname);	
	if (hTRT_EndCapATrkRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": TRT ECA: Unbiased Residuals";
	hTRT_EndCapATrkRes->SetTitle(histtitle.Data());
	hTRT_EndCapATrkRes->GetXaxis()->SetTitle("Residuals [mm]");
	hTRT_EndCapATrkRes->GetYaxis()->SetTitle("Entries");
	hTRT_EndCapATrkRes->SetLineColor(Color+2);
	hTRT_EndCapATrkRes->SetFillColor(Color);
	hTRT_EndCapATrkRes->SetLineWidth(1);
	hTRT_EndCapATrkRes->SetFillStyle(3001);
	if (normalizePlots) hTRT_EndCapATrkRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hTRT_EndCapATrkRes->GetMean()!=0.0) {
	 hTRT_EndCapATrkRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/trt_ec_residualR_Endcap_A";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTRT_EndCapATrkRes_2 = (TH1F*)file->Get(histname);	
		hTRT_EndCapATrkRes_2->SetLineColor(ColorForSecond+2);
		hTRT_EndCapATrkRes_2->SetFillColor(ColorForSecond);
		hTRT_EndCapATrkRes_2->SetLineWidth(1);
		hTRT_EndCapATrkRes_2->SetFillStyle(3001);
		if (normalizePlots) hTRT_EndCapATrkRes_2->Scale(1./(float)iEvents);
		hTRT_EndCapATrkRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hTRT_EndCapATrkRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"TRT EndCap A");
	 t->DrawLatex(0.3,0.5,name);
	 sprintf(name,"no processed");
	 t->DrawLatex(0.33,0.44,name);

	 TLine *TLineBorder;
	 TLineBorder = new TLine(0.05,0.90,0.92,0.90);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.10,0.92,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.05,0.90,0.05,0.10);
	 TLineBorder->Draw();
	 TLineBorder = new TLine(0.92,0.90,0.92,0.10);
	 TLineBorder->Draw();
	}

	

	
	return;

}

