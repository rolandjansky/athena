/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelResiduals(TString trkCol){
	pixelResiduals(trkCol, 0, "");
}

void pixelResiduals(TString trkCol, TString trkCol2){
	pixelResiduals(trkCol, 0, trkCol2);
}

void pixelResiduals(TString trkCol, TCanvas *cPixRes){
	pixelResiduals(trkCol, cPixRes, "");
}

void pixelResiduals(TString trkCol, TCanvas *cPixRes, TString trkCol2) {
	TH1F *hPixel_TrkPhiRes;
	TH1F *hPixel_TrkEtaRes;
	TH1F *hPixel_EndCapATrkPhiRes;
	TH1F *hPixel_EndCapATrkEtaRes;
	TH1F *hPixel_EndCapCTrkPhiRes;
	TH1F *hPixel_EndCapCTrkEtaRes;
	
	TH1F *hPixel_TrkPhiRes_2;
	TH1F *hPixel_TrkEtaRes_2;
	TH1F *hPixel_EndCapATrkPhiRes_2;
	TH1F *hPixel_EndCapATrkEtaRes_2;
	TH1F *hPixel_EndCapCTrkPhiRes_2;
	TH1F *hPixel_EndCapCTrkEtaRes_2;
	
	TString histname;
	TString histtitle;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cPixRes){
		cPixRes = new TCanvas("PixResiduals","All Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
	}
	cPixRes->Divide(3,2);
	
	cPixRes->cd(2);
	
	histname = "IDAlignMon/"+trkCol+"/Residuals/pix_b_biasedresidualx";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hPixel_TrkPhiRes = (TH1F*)file->Get(histname);	

	if (hPixel_TrkPhiRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}

	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": Pixel Barrel. Biased Residuals local x";
	hPixel_TrkPhiRes->SetTitle(histtitle.Data());
	hPixel_TrkPhiRes->GetXaxis()->SetTitle("Local x residuals (mm)");
	hPixel_TrkPhiRes->GetYaxis()->SetTitle("Entries");
	hPixel_TrkPhiRes->SetLineColor(Color+2);
	hPixel_TrkPhiRes->SetFillColor(Color);
	hPixel_TrkPhiRes->SetLineWidth(1);
	hPixel_TrkPhiRes->SetFillStyle(3001);
	if (normalizePlots) hPixel_TrkPhiRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hPixel_TrkPhiRes->GetMean()!=0.0) {
	 hPixel_TrkPhiRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_b_biasedresidualx";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixel_TrkPhiRes_2 = (TH1F*)file->Get(histname);	
		hPixel_TrkPhiRes_2->SetLineColor(ColorForSecond+2);
		hPixel_TrkPhiRes_2->SetFillColor(ColorForSecond);
		hPixel_TrkPhiRes_2->SetLineWidth(1);
		hPixel_TrkPhiRes_2->SetFillStyle(3001);
		if (normalizePlots) hPixel_TrkPhiRes_2->Scale(1./(float)iEvents);
		hPixel_TrkPhiRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hPixel_TrkPhiRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"Pixel Barrel");
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

	cPixRes->cd(5);
	histname = "IDAlignMon/"+trkCol+"/Residuals/pix_b_biasedresidualy";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hPixel_TrkEtaRes = (TH1F*)file->Get(histname);	
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": Pixel Barrel. Biased Residuals local y";
	hPixel_TrkEtaRes->SetTitle(histtitle.Data());
	hPixel_TrkEtaRes->GetXaxis()->SetTitle("Local y residuals (mm)");
	hPixel_TrkEtaRes->GetYaxis()->SetTitle("Entries");
	hPixel_TrkEtaRes->SetLineColor(Color+2);
	hPixel_TrkEtaRes->SetLineWidth(1);
	hPixel_TrkEtaRes->SetFillColor(Color);
	hPixel_TrkEtaRes->SetFillStyle(3001);
	if (normalizePlots) hPixel_TrkEtaRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hPixel_TrkEtaRes->GetMean()!=0.0) {
	 hPixel_TrkEtaRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_b_biasedresidualy";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixel_TrkEtaRes_2 = (TH1F*)file->Get(histname);	
		hPixel_TrkEtaRes_2->SetLineColor(ColorForSecond+2);
		hPixel_TrkEtaRes_2->SetFillColor(ColorForSecond);
		hPixel_TrkEtaRes_2->SetLineWidth(1);
		hPixel_TrkEtaRes_2->SetFillStyle(3001);
		if (normalizePlots) hPixel_TrkEtaRes_2->Scale(1./(float)iEvents);
		hPixel_TrkEtaRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hPixel_TrkEtaRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"Pixel Barrel");
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

	cPixRes->cd(3);
	histname = "IDAlignMon/"+trkCol+"/Residuals/pix_eca_biased_residualx";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	
	hPixel_EndCapATrkPhiRes = (TH1F*)file->Get(histname);	
	if (hPixel_EndCapATrkPhiRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": Pixel ECA. Biased Residuals local x";
	hPixel_EndCapATrkPhiRes->SetTitle(histtitle.Data());
	hPixel_EndCapATrkPhiRes->GetXaxis()->SetTitle("Local x residuals (mm)");
	hPixel_EndCapATrkPhiRes->GetYaxis()->SetTitle("Entries");
	hPixel_EndCapATrkPhiRes->SetLineColor(Color+2);
	hPixel_EndCapATrkPhiRes->SetFillColor(Color);
	hPixel_EndCapATrkPhiRes->SetLineWidth(1);
	hPixel_EndCapATrkPhiRes->SetFillStyle(3001);
	if (normalizePlots) hPixel_EndCapATrkPhiRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hPixel_EndCapATrkPhiRes->GetMean()!=0.0) {
	 hPixel_EndCapATrkPhiRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_eca_biased_residualx";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixel_EndCapATrkPhiRes_2 = (TH1F*)file->Get(histname);	
		hPixel_EndCapATrkPhiRes_2->SetLineColor(ColorForSecond+2);
		hPixel_EndCapATrkPhiRes_2->SetFillColor(ColorForSecond);
		hPixel_EndCapATrkPhiRes_2->SetLineWidth(1);
		hPixel_EndCapATrkPhiRes_2->SetFillStyle(3001);
		if (normalizePlots) hPixel_EndCapATrkPhiRes_2->Scale(1./(float)iEvents);
		hPixel_EndCapATrkPhiRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hPixel_EndCapATrkPhiRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"Pixel ECA");
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

	cPixRes->cd(6);
	histname = "IDAlignMon/"+trkCol+"/Residuals/pix_eca_biased_residualy";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hPixel_EndCapATrkEtaRes = (TH1F*)file->Get(histname);	
	if (hPixel_EndCapATrkPhiRes == NULL) { // check if histogram exists
	  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
	  return;
	}
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": Pixel ECA. Biased Residuals local y";
	hPixel_EndCapATrkEtaRes->SetTitle(histtitle.Data());
	hPixel_EndCapATrkEtaRes->GetXaxis()->SetTitle("Local y residuals (mm)");
	hPixel_EndCapATrkEtaRes->GetYaxis()->SetTitle("Entries");
	hPixel_EndCapATrkEtaRes->SetLineColor(Color+2);
	hPixel_EndCapATrkEtaRes->SetFillColor(Color);
	hPixel_EndCapATrkEtaRes->SetLineWidth(1);
	hPixel_EndCapATrkEtaRes->SetFillStyle(3001);
	if (normalizePlots) hPixel_EndCapATrkEtaRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hPixel_EndCapATrkEtaRes->GetMean()!=0.0) {
	 hPixel_EndCapATrkEtaRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_eca_biased_residualy";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixel_EndCapATrkEtaRes_2 = (TH1F*)file->Get(histname);	
		hPixel_EndCapATrkEtaRes_2->SetLineColor(ColorForSecond+2);
		hPixel_EndCapATrkEtaRes_2->SetFillColor(ColorForSecond);
		hPixel_EndCapATrkEtaRes_2->SetLineWidth(1);
		hPixel_EndCapATrkEtaRes_2->SetFillStyle(3001);
		if (normalizePlots) hPixel_EndCapATrkEtaRes_2->Scale(1./(float)iEvents);
		hPixel_EndCapATrkEtaRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hPixel_EndCapATrkEtaRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"Pixel EndCap A");
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

	cPixRes->cd(1);
	histname = "IDAlignMon/"+trkCol+"/Residuals/pix_ecc_biased_residualx";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hPixel_EndCapCTrkPhiRes = (TH1F*)file->Get(histname);	
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": Pixel ECC. Biased Residuals local x";
	hPixel_EndCapCTrkPhiRes->SetTitle(histtitle.Data());
	hPixel_EndCapCTrkPhiRes->GetXaxis()->SetTitle("Local x residuals (mm)");
	hPixel_EndCapCTrkPhiRes->GetYaxis()->SetTitle("Entries");
	hPixel_EndCapCTrkPhiRes->SetLineColor(Color+2);
	hPixel_EndCapCTrkPhiRes->SetFillColor(Color);
	hPixel_EndCapCTrkPhiRes->SetLineWidth(1);
	hPixel_EndCapCTrkPhiRes->SetFillStyle(3001);
	if (normalizePlots) hPixel_EndCapCTrkPhiRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	if (hPixel_EndCapCTrkPhiRes->GetMean()!=0.0) {
	 hPixel_EndCapCTrkPhiRes->DrawCopy("hist");	
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_ecc_biased_residualx";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixel_EndCapCTrkPhiRes_2 = (TH1F*)file->Get(histname);	
		hPixel_EndCapCTrkPhiRes_2->SetLineColor(ColorForSecond+2);
		hPixel_EndCapCTrkPhiRes_2->SetFillColor(ColorForSecond);
		hPixel_EndCapCTrkPhiRes_2->SetLineWidth(1);
		hPixel_EndCapCTrkPhiRes_2->SetFillStyle(3001);
		if (normalizePlots) hPixel_EndCapCTrkPhiRes_2->Scale(1./(float)iEvents);
		hPixel_EndCapCTrkPhiRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hPixel_EndCapCTrkPhiRes->Delete();
	 TLatex *t = new TLatex();
	 t->SetNDC();
	 t->SetTextFont(42);
	 t->SetTextColor(kRed);
	 t->SetTextSize(0.06);
	 t->SetTextAngle(25);
	 sprintf(name,"Pixel EndCap C");
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

	cPixRes->cd(4);
	histname = "IDAlignMon/"+trkCol+"/Residuals/pix_ecc_biased_residualy";
	if(PrintLevel>0) cout << " <pixelResiduals> Plotting Histogram: " << histname << endl;
	hPixel_EndCapCTrkEtaRes = (TH1F*)file->Get(histname);	
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": Pixel ECC. Biased Residuals local y";
	hPixel_EndCapCTrkEtaRes->SetTitle(histtitle.Data());
	hPixel_EndCapCTrkEtaRes->GetXaxis()->SetTitle("Local y residuals (mm)");
	hPixel_EndCapCTrkEtaRes->GetYaxis()->SetTitle("Entries");
	hPixel_EndCapCTrkEtaRes->SetLineColor(Color+2);
	hPixel_EndCapCTrkEtaRes->SetFillColor(Color);
	hPixel_EndCapCTrkEtaRes->SetLineWidth(1);
	hPixel_EndCapCTrkEtaRes->SetFillStyle(3001);
	if (normalizePlots) hPixel_EndCapCTrkEtaRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
	 gPad->SetGridy();
	 gPad->SetGridx();
	}
	cout << " point 1" << endl;
	if (hPixel_EndCapCTrkEtaRes->GetMean()!=0.0) {
	 hPixel_EndCapCTrkEtaRes->DrawCopy("hist");
	 setTitleFont(font);
	 ScalePlots();
	 setTitleSize(0.67);
	 if(trkCol2 !=""){
	 	histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_ecc_biased_residualy";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixel_EndCapCTrkEtaRes_2 = (TH1F*)file->Get(histname);	
		hPixel_EndCapCTrkEtaRes_2->SetLineColor(ColorForSecond+2);
		hPixel_EndCapCTrkEtaRes_2->SetFillColor(ColorForSecond);
		hPixel_EndCapCTrkEtaRes_2->SetLineWidth(1);
		hPixel_EndCapCTrkEtaRes_2->SetFillStyle(3001);
		if (normalizePlots) hPixel_EndCapCTrkEtaRes_2->Scale(1./(float)iEvents);
		hPixel_EndCapCTrkEtaRes_2->DrawCopy("histsame");
		}
	}
	else {
	 hPixel_EndCapCTrkEtaRes->Delete();
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
	
	return;

}
