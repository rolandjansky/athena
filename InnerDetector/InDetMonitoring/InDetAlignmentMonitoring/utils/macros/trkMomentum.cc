/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trkMomentum(TString trkCol){
	trkMomentum(trkCol, 0, "");
}

void trkMomentum(TString trkCol, TString trkCol2){
	trkMomentum(trkCol, 0, trkCol2);
}

void trkMomentum(TString trkCol, TCanvas *cTrkMomentum){
	trkMomentum(trkCol, cTrkMomentum, "");
}

void trkMomentum(TString trkCol, TCanvas *cTrkMomentum, TString trkCol2) {


	TH1F *hTrkPt;
	TH1F *hTrk_PtRes;
	TH1F *hTrk_PtResOverP;
	TH1F *hTrk_Pt_n;
	TH1F *hTrk_Pt_p;
	TH1F *hTrk_Pt_diff;
	TH1F *hTrkPt_2;
	TH1F *hTrk_PtRes_2;
	TH1F *hTrk_PtResOverP_2;
	TH1F *hTrk_Pt_n_2;
	TH1F *hTrk_Pt_p_2;
	TH1F *hTrk_Pt_diff_2;
	cout << " Track Parameters..." << endl;
	
	TString histname;
	TString histtitle;

	Color_t Color;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	if(!cTrkMomentum)
		cTrkMomentum = new TCanvas("cTrkMomentum","Track Momentum",
							CanvasSizeX6[0],CanvasSizeX6[1]);
							
	cTrkMomentum->Divide(3,2);

	cTrkMomentum->cd(1);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/pT";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrkPt = (TH1F*)file->Get(histname);
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": p_{T}";
	hTrkPt->SetTitle(histtitle.Data());
	hTrkPt->GetXaxis()->SetTitle("p_{T} [GeV]");
	hTrkPt->GetYaxis()->SetTitle("Tracks");
	hTrkPt->SetLineColor(Color+2);
	hTrkPt->SetFillColor(Color);
	hTrkPt->SetFillStyle(3001);
	hTrkPt->SetMinimum(0.);
	if (normalizePlots) hTrkPt->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrkPt->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();

	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/pT";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrkPt_2 = (TH1F*)file->Get(histname);	
		hTrkPt_2->SetLineColor(ColorForSecond+2);
		hTrkPt_2->SetFillColor(ColorForSecond);
		hTrkPt_2->SetLineWidth(1);
		hTrkPt_2->SetFillStyle(3001);
		if (normalizePlots) hTrkPt_2->Scale(1./(float)iEvents);
		hTrkPt_2->DrawCopy("histsame");
	}

	//
	// -----------------------------------------------------------------
	//

	cTrkMomentum->cd(2);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/pTRes";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_PtRes = (TH1F*)file->Get(histname);
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": p_{T} resolution";
	hTrk_PtRes->SetTitle(histtitle.Data());
	hTrk_PtRes->GetXaxis()->SetTitle("p_{T} resol. [GeV]");
	hTrk_PtRes->GetYaxis()->SetTitle("Tracks");
	hTrk_PtRes->SetLineColor(Color+2);
	hTrk_PtRes->SetFillColor(Color);
	hTrk_PtRes->SetFillStyle(3001);
	hTrk_PtRes->SetMinimum(0.);
	hTrk_PtRes->GetXaxis()->SetRangeUser(0.,0.15);
	if (normalizePlots) hTrk_PtRes->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_PtRes->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/pTRes";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_PtRes_2 = (TH1F*)file->Get(histname);	
		hTrk_PtRes_2->SetLineColor(ColorForSecond+2);
		hTrk_PtRes_2->SetFillColor(ColorForSecond);
		hTrk_PtRes_2->SetLineWidth(1);
		hTrk_PtRes_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_PtRes_2->Scale(1./(float)iEvents);
		hTrk_PtRes_2->DrawCopy("histsame");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cTrkMomentum->cd(3);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/pTResOverP";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_PtResOverP = (TH1F*)file->Get(histname);
	hTrk_PtResOverP->SetTitle("Trk pT Res Over P");
	hTrk_PtResOverP->GetXaxis()->SetTitle("pT resol. over P");
	hTrk_PtResOverP->GetYaxis()->SetTitle("Tracks");
	hTrk_PtResOverP->SetLineColor(Color+2);
	hTrk_PtResOverP->SetFillColor(Color);
	hTrk_PtResOverP->SetFillStyle(3001);
	hTrk_PtResOverP->SetMinimum(0.);
	if (normalizePlots) hTrk_PtResOverP->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_PtResOverP->DrawCopy("hist");
	hTrk_PtResOverP->GetXaxis()->SetRangeUser(0.,5E-3);
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/pTResOverP";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_PtResOverP_2 = (TH1F*)file->Get(histname);	
		hTrk_PtResOverP_2->SetLineColor(ColorForSecond+2);
		hTrk_PtResOverP_2->SetFillColor(ColorForSecond);
		hTrk_PtResOverP_2->SetLineWidth(1);
		hTrk_PtResOverP_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_PtResOverP_2->Scale(1./(float)iEvents);
		hTrk_PtResOverP_2->DrawCopy("histsame");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cTrkMomentum->cd(4);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/pT_p";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_Pt_p = (TH1F*)file->Get(histname);
	//gPad->SetLogy();
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": p_{T} (positive particles)";
	hTrk_Pt_p->SetTitle(histtitle.Data());
	hTrk_Pt_p->GetXaxis()->SetTitle("p_{T} [GeV]");
	hTrk_Pt_p->GetYaxis()->SetTitle("Tracks");
	hTrk_Pt_p->SetLineColor(Color+2);
	hTrk_Pt_p->SetFillColor(Color);
	hTrk_Pt_p->SetFillStyle(3001);
	hTrk_Pt_p->SetMinimum(0.);
	if (normalizePlots) hTrk_Pt_p->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_Pt_p->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/pT_p";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_Pt_p_2 = (TH1F*)file->Get(histname);	
		hTrk_Pt_p_2->SetLineColor(ColorForSecond+2);
		hTrk_Pt_p_2->SetFillColor(ColorForSecond);
		hTrk_Pt_p_2->SetLineWidth(1);
		hTrk_Pt_p_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_Pt_p_2->Scale(1./(float)iEvents);
		hTrk_Pt_p_2->DrawCopy("histsame");
	}
	
	//
	// -----------------------------------------------------------------
	//

	cTrkMomentum->cd(5);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/pT_n";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_Pt_n = (TH1F*)file->Get(histname);
	histtitle.Clear();
	histtitle += trkCol.Data();
	histtitle += ": p_{T} (negative particles)";
	hTrk_Pt_n->SetTitle(histtitle.Data());
	hTrk_Pt_n->GetXaxis()->SetTitle("p_{T} [GeV]");
	hTrk_Pt_n->GetYaxis()->SetTitle("Tracks");
	hTrk_Pt_n->SetLineColor(Color+2);
	hTrk_Pt_n->SetFillColor(Color);
	hTrk_Pt_n->SetFillStyle(3001);
	if (normalizePlots) hTrk_Pt_n->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_Pt_n->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/pT_n";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_Pt_n_2 = (TH1F*)file->Get(histname);	
		hTrk_Pt_n_2->SetLineColor(ColorForSecond+2);
		hTrk_Pt_n_2->SetFillColor(ColorForSecond);
		hTrk_Pt_n_2->SetLineWidth(1);
		hTrk_Pt_n_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_Pt_n_2->Scale(1./(float)iEvents);
		hTrk_Pt_n_2->DrawCopy("histsame");
	}

	

	//
	// -----------------------------------------------------------------
	//

	cTrkMomentum->cd(6);
	histname = "IDAlignMon/"+trkCol+"/GenericTracks/pT_diff";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hTrk_Pt_diff = (TH1F*)file->Get(histname);	
	hTrk_Pt_diff->SetTitle("Difference Neg-Pos Tracks Vs P_{T}");
	hTrk_Pt_diff->GetXaxis()->SetTitle("p_{T} [GeV]");
	hTrk_Pt_diff->GetYaxis()->SetTitle("Tracks");
	hTrk_Pt_diff->SetLineColor(Color+2);
	hTrk_Pt_diff->SetFillColor(Color);
	hTrk_Pt_diff->SetFillStyle(3001);
	if (normalizePlots) hTrk_Pt_diff->Scale(1./(float)iEvents);
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	hTrk_Pt_diff->DrawCopy("hist");
	setTitleFont(font);
	ScalePlots();
	if(trkCol2 !=""){
		histname = "IDAlignMon/"+trkCol2+"/GenericTracks/pT_diff";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hTrk_Pt_diff_2 = (TH1F*)file->Get(histname);	
		hTrk_Pt_diff_2->SetLineColor(ColorForSecond+2);
		hTrk_Pt_diff_2->SetFillColor(ColorForSecond);
		hTrk_Pt_diff_2->SetLineWidth(1);
		hTrk_Pt_diff_2->SetFillStyle(3001);
		if (normalizePlots) hTrk_Pt_diff_2->Scale(1./(float)iEvents);
		hTrk_Pt_diff_2->DrawCopy("histsame");
	}



	cTrkMomentum->Update();

}
