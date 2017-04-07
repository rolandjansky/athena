/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hitsPerLayer(TString TrkCol_1, TString TrkCol_2) {
	TH1F * hHitsPerLayerEndcapATrkCol1;
	TH1F * hHitsPerLayerBarrelTrkCol1; 
	TH1F * hHitsPerLayerEndcapCTrkCol1;
	TH1F * hHitsPerLayerEndcapATrkCol2;
	TH1F * hHitsPerLayerBarrelTrkCol2; 
	TH1F * hHitsPerLayerEndcapCTrkCol2;
	TString histname;
	TString histtitle;
	
	cout << " <hitsPerLayer> Hits per layer..." << endl;
 
	cHitsPerLayer = new TCanvas("HitsPerLayer","Hits per layer",CanvasSizeX6[0],CanvasSizeX6[1]);
	cHitsPerLayer->Divide(3,2);

	cHitsPerLayer->cd(1);
		
	histname = "IDAlignMon/"+TrkCol_1+"/HitEfficiencies/measurements_vs_layer_ecc";
	if(PrintLevel>0) cout << " <hitsPerLayer> Plotting Histogram: " << histname << endl;
	hHitsPerLayerEndcapCTrkCol1 = (TH1F*)file->Get(histname);
	histtitle.Clear();
	histtitle += TrkCol_1.Data();
	histtitle += " Hits per disk (ECC)";
	hHitsPerLayerEndcapCTrkCol1->SetTitle(histtitle.Data());
	if (normalizePlots) hHitsPerLayerEndcapCTrkCol1->Scale(1./(float)iEvents);
	hHitsPerLayerEndcapCTrkCol1->SetStats(false);
	hHitsPerLayerEndcapCTrkCol1->GetXaxis()->SetTitle("EndCap C Disk");
	hHitsPerLayerEndcapCTrkCol1->GetXaxis()->SetTitleOffset(1.3);
	hHitsPerLayerEndcapCTrkCol1->GetYaxis()->SetTitle("Hits");
	hHitsPerLayerEndcapCTrkCol1->SetLineColor(kGray+2);
	hHitsPerLayerEndcapCTrkCol1->SetLineWidth(2);
	hHitsPerLayerEndcapCTrkCol1->Draw("hist");
	if (!AtlasStyle) {
		gPad->SetGridy();
		gPad->SetGridx();
	}	
	setTitleFont(font);
	ScalePlots();
	
	//
	// -----------------------------------------------------------------
	//

	cHitsPerLayer->cd(2);
    histname = "IDAlignMon/"+TrkCol_1+"/HitEfficiencies/measurements_vs_layer_barrel";
    if(PrintLevel>0) cout << " <hitsPerLayer> Plotting Histogram: " << histname << endl;
    hHitsPerLayerBarrelTrkCol1 = (TH1F*)file->Get(histname);
    histtitle.Clear();
    histtitle += TrkCol_1.Data();
    histtitle += " Hits per barrel layer";
    hHitsPerLayerBarrelTrkCol1->SetTitle(histtitle.Data());
    if (normalizePlots) hHitsPerLayerBarrelTrkCol1->Scale(1./(float)iEvents);
    hHitsPerLayerBarrelTrkCol1->SetStats(false);
    hHitsPerLayerBarrelTrkCol1->GetXaxis()->SetTitle("Barrel Layer");
    hHitsPerLayerBarrelTrkCol1->GetYaxis()->SetTitleOffset(2.0);
    hHitsPerLayerBarrelTrkCol1->GetYaxis()->SetTitle("Hits");
    hHitsPerLayerBarrelTrkCol1->SetLineColor(kGray+2);
    hHitsPerLayerBarrelTrkCol1->SetLineWidth(2);
    hHitsPerLayerBarrelTrkCol1->Draw("hist");
    if (!AtlasStyle) {
        gPad->SetGridy();
        gPad->SetGridx();
    }
    setTitleFont(font);
    ScalePlots();


	//
	// -----------------------------------------------------------------
	//

	cHitsPerLayer->cd(3);
	histname = "IDAlignMon/"+TrkCol_1+"/HitEfficiencies/measurements_vs_layer_eca";
	if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	hHitsPerLayerEndcapATrkCol1 = (TH1F*)file->Get(histname);
	histtitle.Clear();
	histtitle += TrkCol_1.Data();
	histtitle += " Hits per disk (ECA)";
	hHitsPerLayerEndcapATrkCol1->SetTitle(histtitle.Data());
	if (normalizePlots) hHitsPerLayerEndcapATrkCol1->Scale(1./(float)iEvents);
	hHitsPerLayerEndcapATrkCol1->SetStats(false);
	hHitsPerLayerEndcapATrkCol1->GetXaxis()->SetTitle("EndCapA Disk");
	hHitsPerLayerEndcapATrkCol1->GetXaxis()->SetTitleOffset(1.3);
	hHitsPerLayerEndcapATrkCol1->GetYaxis()->SetTitle("Hits");
	hHitsPerLayerEndcapATrkCol1->SetLineColor(kGray+2);
	hHitsPerLayerEndcapATrkCol1->SetLineWidth(2);
	hHitsPerLayerEndcapATrkCol1->Draw("hist");
	if (!AtlasStyle) {
		gPad->SetGridy();
		gPad->SetGridx();
	}
	setTitleFont(font);
	ScalePlots();
	
	//
	// -----------------------------------------------------------------
	//


      cHitsPerLayer->cd(4);
      if (TrkCol_2 !=""){
	  histname = "IDAlignMon/"+TrkCol_2+"/HitEfficiencies/measurements_vs_layer_ecc";
	  if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	  hHitsPerLayerEndcapCTrkCol2 = (TH1F*)file->Get(histname);
	  histtitle.Clear();
	  histtitle += TrkCol_2.Data();
	  histtitle += " Hits per disk (ECC)";
	  hHitsPerLayerEndcapCTrkCol2->SetTitle(histtitle.Data());
	  if (normalizePlots) hHitsPerLayerEndcapCTrkCol2->Scale(1./(float)iEvents);
	  hHitsPerLayerEndcapCTrkCol2->SetStats(false);
	  hHitsPerLayerEndcapCTrkCol2->GetXaxis()->SetTitle(" EndCapC Disk");
	  hHitsPerLayerEndcapCTrkCol2->GetXaxis()->SetTitleOffset(1.3);
	  hHitsPerLayerEndcapCTrkCol2->GetYaxis()->SetTitle("Hits");
	  hHitsPerLayerEndcapCTrkCol2->SetLineColor(kRed);
	  hHitsPerLayerEndcapCTrkCol2->SetLineWidth(2);
	  hHitsPerLayerEndcapCTrkCol2->Draw("hist");
	  if (!AtlasStyle) {
        gPad->SetGridy();
		gPad->SetGridx();
	  }
	  setTitleFont(font);
	  ScalePlots();
    }

	//
	// -----------------------------------------------------------------
	//

      cHitsPerLayer->cd(5);
      if (TrkCol_2 !=""){
	  histname = "IDAlignMon/"+TrkCol_2+"/HitEfficiencies/measurements_vs_layer_barrel";
	  if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	  hHitsPerLayerBarrelTrkCol2 = (TH1F*)file->Get(histname);
	  histtitle.Clear();
	  histtitle += TrkCol_2.Data();
	  histtitle += " Hits per barrel layer";
	  hHitsPerLayerBarrelTrkCol2->SetTitle(histtitle.Data());
	  if (normalizePlots) hHitsPerLayerBarrelTrkCol2->Scale(1./(float)iEvents);
	  hHitsPerLayerBarrelTrkCol2->SetStats(false);
	  hHitsPerLayerBarrelTrkCol2->GetXaxis()->SetTitle("Barrel Layer");
	  hHitsPerLayerBarrelTrkCol2->GetYaxis()->SetTitle("Hits");
	  hHitsPerLayerBarrelTrkCol2->SetLineColor(kRed);
	  hHitsPerLayerBarrelTrkCol2->SetLineWidth(2);
	  hHitsPerLayerBarrelTrkCol2->Draw("hist");
	  if (!AtlasStyle) {
		gPad->SetGridy();
		gPad->SetGridx();
	  }
	  setTitleFont(font);
	  ScalePlots();
    }

	//
	// -----------------------------------------------------------------
	//

	cHitsPerLayer->cd(6);
    if(TrkCol_2 !=""){
	  histname = "IDAlignMon/"+TrkCol_2+"/HitEfficiencies/measurements_vs_layer_eca";
	  if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
	  hHitsPerLayerEndcapATrkCol2 = (TH1F*)file->Get(histname);
	  histtitle.Clear();
	  histtitle += TrkCol_2.Data();
	  histtitle += " Hits per disk (ECA)";
	  hHitsPerLayerEndcapATrkCol2->SetTitle(histtitle.Data());
	  if (normalizePlots) hHitsPerLayerEndcapATrkCol2->Scale(1./(float)iEvents);
	  hHitsPerLayerEndcapATrkCol2->SetStats(false);
	  hHitsPerLayerEndcapATrkCol2->GetXaxis()->SetTitle("EndCapA Disk");
	  hHitsPerLayerEndcapATrkCol2->GetXaxis()->SetTitleOffset(1.3);
	  hHitsPerLayerEndcapATrkCol2->GetYaxis()->SetTitle("Hits");
	  hHitsPerLayerEndcapATrkCol2->SetLineColor(kRed);
	  hHitsPerLayerEndcapATrkCol2->SetLineWidth(2);
	  hHitsPerLayerEndcapATrkCol2->Draw("hist");
	  if (!AtlasStyle) {
		gPad->SetGridy();
		gPad->SetGridx();
	  }
	  setTitleFont(font);
	  ScalePlots();
    }

}
