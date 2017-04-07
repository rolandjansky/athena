/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void residuals(TString trkCol) {
	cout << " <residuals> Summary table and Residuals..." << endl;

	TH1F *hSCTBa_Res;
	TH1F *hPixelBa_Res;
	TH1F *hTRTBa_Res;
	TString histname;
	
	cInfoRes = new TCanvas("GlobalResiduals","Global Residuals",CanvasSizeX4[0],CanvasSizeX4[1]);
	cInfoRes->Divide(2,2);

	cInfoRes->cd(2);
	histname = "IDAlignMon/"+trkCol+"/Residuals/sct_b_residualx";
	if(PrintLevel>0) cout << " <residual> Plotting Histogram: " << histname << endl;
	
	hSCTBa_Res = (TH1F*)file->Get(histname);
	hSCTBa_Res->SetTitle("SCT barrel residuals");
	hSCTBa_Res->GetXaxis()->SetTitle("Residuals [mm]");
	hSCTBa_Res->GetYaxis()->SetTitle("Entries");
	hSCTBa_Res->SetLineColor(ColorForFirst+2);
	hSCTBa_Res->SetLineWidth(1);
	hSCTBa_Res->SetFillColor(ColorForFirst);
	hSCTBa_Res->SetFillStyle(3001);
	if (normalizePlots) hSCTBa_Res->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hSCTBa_Res->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();	
   
	//
	// -----------------------------------------------------------------
	//

	cInfoRes->cd(3);
  	histname = "IDAlignMon/"+trkCol+"/Residuals/pix_b_residualx";
	if(PrintLevel>0) cout << " <residuals> Plotting Histogram: " << histname << endl;
	hPixelBa_Res = (TH1F*)file->Get(histname);
	hPixelBa_Res->SetTitle("Pixel barrel local x residuals");
	hPixelBa_Res->GetXaxis()->SetTitle("Local x residuals [mm]");
	hPixelBa_Res->GetYaxis()->SetTitle("Entries");
	hPixelBa_Res->SetLineColor(ColorForFirst+2);
	hPixelBa_Res->SetLineWidth(1);
	hPixelBa_Res->SetFillColor(ColorForFirst);
	hPixelBa_Res->SetFillStyle(3001);
	if (normalizePlots) hPixelBa_Res->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
		gPad->SetGridy();
		gPad->SetGridx();
	}
	  hPixelBa_Res->DrawCopy("hist");
	  setTitleFont(font);
	  ScalePlots();
   
	//
	// -----------------------------------------------------------------
	//
	cInfoRes->cd(4);
	histname = "IDAlignMon/"+trkCol+"/Residuals/trt_b_residualR";
	if(PrintLevel>0) cout << " <residuals> Plotting Histogram: " << histname << endl;
	
	hTRTBa_Res = (TH1F*)file->Get(histname);
	hTRTBa_Res->SetTitle("TRT barrel residuals");
	hTRTBa_Res->GetXaxis()->SetTitle("TRT residuals [mm]");
	hTRTBa_Res->GetYaxis()->SetTitle("Entries");
	hTRTBa_Res->SetLineColor(ColorForFirst+2);
	hTRTBa_Res->SetLineWidth(1);
	hTRTBa_Res->SetFillColor(ColorForFirst);
	hTRTBa_Res->SetFillStyle(3001);
	if (normalizePlots) hTRTBa_Res->Scale(1./(float)iEvents);
	if (!AtlasStyle) {
		gPad->SetGridy();
		gPad->SetGridx();
	}
	hTRTBa_Res->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
   
	// SUMMARY
	cInfoRes->cd(1);
	TLine *TLineBorder;
	TLineBorder = new TLine(0.02,0.98,0.98,0.98);
	TLineBorder->SetLineColor(kRed);
	TLineBorder->Draw();
	TLineBorder = new TLine(0.02,0.02,0.98,0.02);
	TLineBorder->SetLineColor(kRed);
	TLineBorder->Draw();
	TLineBorder = new TLine(0.02,0.98,0.02,0.02);
	TLineBorder->SetLineColor(kRed);
	TLineBorder->Draw();
	TLineBorder = new TLine(0.98,0.98,0.98,0.02);
	TLineBorder->SetLineColor(kRed);
	TLineBorder->Draw();

  	TString name;
	TLatex *t = new TLatex();
	t->SetNDC();
	t->SetTextFont(font);
	t->SetTextSize(0.05);
	t->SetTextAlign(12);
	
	t->DrawLatex(0.08,0.90,aSetup);
	t->SetTextSize(0.035);

	//sprintf(name,"%s",aSim);
	//if (iCosmics) t->DrawLatex(0.8,0.83,name);
	//else t->DrawLatex(0.8,0.90,name);

	TLine *line1;
	line1 = new TLine(0.06,0.87,0.95,0.87);
	line1->Draw();

	// Information
	t->SetTextSize(0.035);
	name = "Iteration: ";
	name+=IterNumber;
	t->DrawLatex(0.08,0.82,name);
	
	name = "Events: ";
	name += iEvents;
	t->DrawLatex(0.08,0.78,name);
	name = "Track Collection 1 (#color[920-2]{gray}): " +  FirstTrackCollection;
	t->DrawLatex(0.08,0.74,name);
	name = "Tracks: ";
	name += iTracks_1;
	t->DrawLatex(0.08,0.70,name);
	name = " - Total hits: ";
	name += iHits_1;
	t->DrawLatex(0.08,0.66,name);
	name = " -- Pixel hits: ";
	name += iPixelHits_1;
	t->DrawLatex(0.08,0.62,name);
	name = " -- SCT hits: ";
	name += iSCTHits_1;
	t->DrawLatex(0.08,0.58,name);
	name = " -- TRT hits: ";
	name += iTRTHits_1;
	t->DrawLatex(0.08,0.54,name);
	name = "Track Collection 2 (#color[632]{red}): " + SecondTrackCollection;
	t->DrawLatex(0.08,0.50,name);
	name = "Tracks: ";
	name += iTracks_2;
	t->DrawLatex(0.08,0.46,name);
	name = " - Total hits: ";
	name += iHits_2;
	t->DrawLatex(0.08,0.42,name);
	name = " -- Pixel hits: ";
	name += iPixelHits_2;
	t->DrawLatex(0.08,0.38,name);
	name = " -- SCT hits: ";
	name += iSCTHits_2;
	t->DrawLatex(0.08,0.34,name);
	name = " -- TRT hits: ";
	name += iTRTHits_2;
	t->DrawLatex(0.08,0.30,name);

	
	if (iBField!=999) {
	  name = "B Field: ";
	  name += iBField;
	  name += " Tesla";
	  t->DrawLatex(0.08,0.07,name);
	}


	cInfoRes->Update();
	
  
}

