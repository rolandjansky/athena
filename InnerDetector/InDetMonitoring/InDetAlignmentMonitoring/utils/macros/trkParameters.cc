/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trkParameters(TString trkCol){
	trkParameters(trkCol, 0, "");
}

void trkParameters(TString trkCol, TString trkCol2){
	trkParameters(trkCol, 0, trkCol2);
}

void trkParameters(TString trkCol, TCanvas *cTrkParameters){
	trkParameters(trkCol, cTrkParameters, "");
}

void trkParameters(TString trkCol, TCanvas *cTrkParameters, TString trkCol2) {


	TH1F *hTrk_d0;
	TH1F *hTrk_z0;
	TH1F *hTrk_phi0;
	TH1F *hTrk_Pt;
	TH1F *hTrk_eta;
	TH1F *hTrk_chi2;
	TH1F *hTrk_d0_2;
	TH1F *hTrk_z0_2;
	TH1F *hTrk_phi0_2;
	TH1F *hTrk_Pt_2;
	TH1F *hTrk_eta_2;
	TH1F *hTrk_chi2_2;
	cout << " Track Parameters..." << endl;
	
	TString histname;
	TString histtitle;
	Color_t Color;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	if(!cTrkParameters)
		cTrkParameters = new TCanvas("cTrkParameters","Track Parameters",
							CanvasSizeX6[0],CanvasSizeX6[1]);
							
	cTrkParameters->Divide(3,2);

	cTrkParameters->cd(1);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/d0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_d0 = (TH1F*)file->Get(histname);
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": d_{0}";
	hTrk_d0->SetTitle(histtitle.Data());
	hTrk_d0->GetXaxis()->SetTitle("d_{0} (mm)");
	hTrk_d0->GetYaxis()->SetTitle("Tracks");
	hTrk_d0->SetLineColor(Color+2);
	hTrk_d0->SetFillColor(Color);
	hTrk_d0->SetFillStyle(3001);
	hTrk_d0->SetMinimum(0.);
	if (normalizePlots) hTrk_d0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_d0->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();

	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/d0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_d0_2 = (TH1F*)file->Get(histname);	
		hTrk_d0_2->SetLineColor(ColorForSecond+2);
		hTrk_d0_2->SetFillColor(ColorForSecond);
		hTrk_d0_2->SetLineWidth(1);
		hTrk_d0_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_d0_2->Scale(1./(float)iEvents);
		hTrk_d0_2->DrawCopy("histsame");
	}

	//
	// -----------------------------------------------------------------
	//

	cTrkParameters->cd(2);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/z0";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_z0 = (TH1F*)file->Get(histname);
	hTrk_z0->SetTitle("Trk Parameter: z_{0}");
	hTrk_z0->GetXaxis()->SetTitle("z_{0} (mm)");
	hTrk_z0->GetYaxis()->SetTitle("Tracks");
	hTrk_z0->SetLineColor(Color+2);
	hTrk_z0->SetFillColor(Color);
	hTrk_z0->SetFillStyle(3001);
	hTrk_z0->SetMinimum(0.);
	if (normalizePlots) hTrk_z0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_z0->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/z0";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_z0_2 = (TH1F*)file->Get(histname);	
		hTrk_z0_2->SetLineColor(ColorForSecond+2);
		hTrk_z0_2->SetFillColor(ColorForSecond);
		hTrk_z0_2->SetLineWidth(1);
		hTrk_z0_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_z0_2->Scale(1./(float)iEvents);
		hTrk_z0_2->DrawCopy("histsame");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cTrkParameters->cd(3);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/phi";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_phi0 = (TH1F*)file->Get(histname);
	hTrk_phi0->SetTitle("Trk Parameter: #phi_{0}");
	hTrk_phi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
	hTrk_phi0->GetYaxis()->SetTitle("Tracks");
	hTrk_phi0->SetLineColor(Color+2);
	hTrk_phi0->SetFillColor(Color);
	hTrk_phi0->SetFillStyle(3001);
	hTrk_phi0->SetMinimum(0.);
	if (normalizePlots) hTrk_phi0->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_phi0->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/phi";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_phi0_2 = (TH1F*)file->Get(histname);	
		hTrk_phi0_2->SetLineColor(ColorForSecond+2);
		hTrk_phi0_2->SetFillColor(ColorForSecond);
		hTrk_phi0_2->SetLineWidth(1);
		hTrk_phi0_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_phi0_2->Scale(1./(float)iEvents);
		hTrk_phi0_2->DrawCopy("histsame");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cTrkParameters->cd(4);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/eta";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_eta = (TH1F*)file->Get(histname);
	hTrk_eta->SetTitle("Trk Parameter: #eta");
	hTrk_eta->GetXaxis()->SetTitle("#eta = -ln (tan(#theta/2))");
	hTrk_eta->GetYaxis()->SetTitle("Tracks");
	hTrk_eta->SetLineColor(Color+2);
	hTrk_eta->SetFillColor(Color);
	hTrk_eta->SetFillStyle(3001);
	hTrk_eta->SetMinimum(0.);
	if (normalizePlots) hTrk_eta->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_eta->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/eta";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_eta_2 = (TH1F*)file->Get(histname);	
		hTrk_eta_2->SetLineColor(ColorForSecond+2);
		hTrk_eta_2->SetFillColor(ColorForSecond);
		hTrk_eta_2->SetLineWidth(1);
		hTrk_eta_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_eta_2->Scale(1./(float)iEvents);
		hTrk_eta_2->DrawCopy("histsame");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cTrkParameters->cd(5);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/pT";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_Pt = (TH1F*)file->Get(histname);
	gPad->SetLogy();
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": P_{T}";
	hTrk_Pt->SetTitle(histtitle.Data());
	hTrk_Pt->GetXaxis()->SetTitle("P_{T} (GeV)");
	hTrk_Pt->GetYaxis()->SetTitle("Tracks");
	hTrk_Pt->SetLineColor(Color+2);
	hTrk_Pt->SetFillColor(Color);
	hTrk_Pt->SetFillStyle(3001);
	if (normalizePlots) hTrk_Pt->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_Pt->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/pT";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_Pt_2 = (TH1F*)file->Get(histname);	
		hTrk_Pt_2->SetLineColor(ColorForSecond+2);
		hTrk_Pt_2->SetFillColor(ColorForSecond);
		hTrk_Pt_2->SetLineWidth(1);
		hTrk_Pt_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_Pt_2->Scale(1./(float)iEvents);
		hTrk_Pt_2->DrawCopy("histsame");
	}

	

	//
	// -----------------------------------------------------------------
	//

	cTrkParameters->cd(6);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/chi2oDoF";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_chi2 = (TH1F*)file->Get(histname);	
	hTrk_chi2->SetTitle("Chi2");
	hTrk_chi2->GetXaxis()->SetTitle("Chi2");
	hTrk_chi2->GetYaxis()->SetTitle("Tracks");
	hTrk_chi2->SetLineColor(Color+2);
	hTrk_chi2->SetFillColor(Color);
	hTrk_chi2->SetFillStyle(3001);
	if (normalizePlots) hTrk_chi2->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_chi2->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/chi2oDoF";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_chi2_2 = (TH1F*)file->Get(histname);	
		hTrk_chi2_2->SetLineColor(ColorForSecond+2);
		hTrk_chi2_2->SetFillColor(ColorForSecond);
		hTrk_chi2_2->SetLineWidth(1);
		hTrk_chi2_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_chi2_2->Scale(1./(float)iEvents);
		hTrk_chi2_2->DrawCopy("histsame");
	}



	cTrkParameters->Update();

}
