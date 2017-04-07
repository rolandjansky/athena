/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctECHitmap(TString TrkCol, int ec){
	sctECHitmap(TrkCol, ec, 0);
}

void sctECHitmap(TString TrkCol, int ec,TCanvas *cSCTHitMapEC){
  TH3F *hTmpHist[2];
  TH2F *hSctEc[2][9];
  TString histname;
  TString title;
  bool setCommonMax = true;
  const int iNSCT_Discs  = 9;
  
  Char_t name[100];
  int maxx[2];
  int maxb[9];
  
  //------------
  // Endcaps
  //------------
  cout << "   - SCT EndCaps..." << endl;
  if(!cSCTHitMapEC){
    if(!ec)
      cSCTHitMapEC = new TCanvas("cSCTHitMapECA","SCT Endcap A Hitmap",CanvasSizeX9[0],CanvasSizeX9[1],
				 CanvasSizeX9[2], CanvasSizeX9[3]);
    else
      cSCTHitMapEC = new TCanvas("cSCTHitMapECC","SCT Endcap C Hitmap",CanvasSizeX9[0],CanvasSizeX9[1],
				 CanvasSizeX9[2], CanvasSizeX9[3]);
  }
  cSCTHitMapEC->Divide(3,3);
  // get histograms and determine maximum
  maxx[ec]=0;
  
  for(int disc=0; disc<iNSCT_Discs; disc++){
    if(!ec) histname = "IDAlignMon/"+TrkCol+ "/Residuals/sct_eca_d";
    else histname = "IDAlignMon/"+TrkCol+ "/Residuals/sct_ecc_d";
    histname += disc;
    histname += "_s0_biased_xresvsmodetaphi_3d";
    if(PrintLevel>0) cout << " <sctECHitmap> Plotting Histogram: " << histname << endl;
    hTmpHist[0] = (TH3F*)file->Get(histname); 

    if (hTmpHist[0] == NULL) { // check if histogram exists
      std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
      return;
    }

    if(!ec) histname = "IDAlignMon/"+TrkCol+ "/Residuals/sct_eca_d";
    else histname = "IDAlignMon/"+TrkCol+ "/Residuals/sct_ecc_d";
    histname += disc;
    histname += "_s1_biased_xresvsmodetaphi_3d";
    if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
    hTmpHist[1] = (TH3F*)file->Get(histname); 
    if(TrkCol == FirstTrackCollection){
			if (!ec) sprintf(name,"HitmapECASCTDisc%dTrkCol_1",disc);
			else sprintf(name,"HitmapECCSCTDisc%dTrkCol_1",disc);
			}
		else{
			if (!ec) sprintf(name,"HitmapECASCTD%dTrkCol_2",disc);
			else sprintf(name,"HitmapECCSCTDisc%dTrkCol_2",disc);
		}
		hSctEc[ec][disc] = new TH2F(name,name,hTmpHist[0]->GetNbinsX(), hTmpHist[0]->GetXaxis()->GetXmin(),hTmpHist[0]->GetXaxis()->GetXmax(),
											  hTmpHist[0]->GetNbinsY(), hTmpHist[0]->GetYaxis()->GetXmin(),hTmpHist[0]->GetYaxis()->GetXmax());
		
		for(int xbins = 1; xbins<=hTmpHist[0]->GetNbinsX(); xbins++){
			for(int ybins = 1; ybins<=hTmpHist[0]->GetNbinsY(); ybins++){
				int hits = 0;
				for (int zbins = 1; zbins<=hTmpHist[0]->GetNbinsZ(); zbins++){
					hits += (int)hTmpHist[0]->GetBinContent(xbins,ybins,zbins) + 
							(int)hTmpHist[1]->GetBinContent(xbins,ybins,zbins);
				}
				hSctEc[ec][disc]->SetBinContent(xbins,ybins,hits);
			}
		}
		hTmpHist[0]=0;
		hTmpHist[1]=0;
	maxb[disc] = (int) hSctEc[ec][disc]->GetBinContent(hSctEc[ec][disc]->GetMaximumBin());
	maxx[ec] = maxb[disc]>maxx[ec] ? maxb[disc] : maxx[ec];
	}
	// loop in discs
	for(int disc=0; disc<iNSCT_Discs; disc++){
	  sprintf(name,"Hitmap ECC SCT Disc %d %s",disc, TrkCol.Data());
	  if (!ec) sprintf(name,"Hitmap ECA SCT Disc %d %s",disc, TrkCol.Data());
		TH2F *boxsct = new TH2F(name, name, 156, -10, 10, 100, -10, 10);
		boxsct->SetStats(kFALSE);
		for(int bx=1; bx<=hSctEc[ec][disc]->GetNbinsX(); bx++)      	
			for(int by=1; by<=hSctEc[ec][disc]->GetNbinsY(); by++)
				boxsct->SetBinContent(50+bx, 50+by, hSctEc[ec][disc]->GetBinContent(bx,by));
		if(setCommonMax){
			boxsct->SetMaximum(maxx[ec]);
			hSctEc[ec][disc]->SetMaximum(maxx[ec]);
	  	}
		TBox *blanksct = new TBox(-10,-10, 10, 9.85);
		blanksct->SetFillColor(kWhite);
		cSCTHitMapEC->cd(disc+1);
		boxsct->GetXaxis()->SetLabelColor(kWhite);
		boxsct->GetYaxis()->SetLabelColor(kWhite);
		boxsct->GetYaxis()->SetTitle("Y axis");
		boxsct->GetXaxis()->SetTitle("X axis");
		boxsct->GetYaxis()->SetTitleOffset(0.8);
		boxsct->Draw("colz");
		blanksct->Draw();
		gPad->RedrawAxis();
		draw_sct_ec_modules(hSctEc[ec][disc], disc);
		setTitleFont(font);
		gPad->Update();
			     
    }
	return;
}

