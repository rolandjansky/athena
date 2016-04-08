/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void BarrelResVsIters(Int_t level = 1) {

  Bool_t showErrors = false;

  if(!PlotNtuple) {
    cout << " Filling Barrel Residuals per Layer Vs Iteration..." << endl;

    // Define the ntuple    
    if (IterCounter==0) {
      ntupleSCTResVsIter = new TNtuple("ntupleSCTResVsIter","ntupleSCTResVsIter",
				       "iter:dettype:bec:layer:mean:sigma:errormean:errorsigma");
      ntuplePIXPhiResVsIter = new TNtuple("ntuplePIXResVsIter","ntuplePIXResVsIter",
					  "iter:dettype:bec:layer:mean:sigma:errormean:errorsigma");
      ntuplePIXEtaResVsIter = new TNtuple("ntuplePIXResVsIter","ntuplePIXResVsIter",
					  "iter:dettype:bec:layer:mean:sigma:errormean:errorsigma");
    }
    
    TH1F *HDSCT[4];
    TH1F *HDPIX_ETA[3];
    TH1F *HDPIX_PHI[3];
  
    for (int i=0; i<4; i++) {
      sprintf(name,"trkana/SCTRes/hSCTResBaLay%dWide_%d",i,level);
      HDSCT[i] = (TH1F*)file->Get(name);
      FitValues fitLX = DrawFitFunc(HDSCT[i],kRed,false);
      ntupleSCTResVsIter->Fill(IterCounter,2,0,i,
			       1000*fitLX.FitMean,1000*fitLX.FitSigma,
			       1000*fitLX.FitErrorMean,1000*fitLX.FitErrorSigma);
    }
    
    for (int i=0; i<3; i++) {
      //Definim Res=1 per a la coordenada Eta y Res=0 per a la Phi
      sprintf(name_eta,"trkana/PIXRes/hPIXEtaResBaLay%dWide_%d",i,level);
      HDPIX_ETA[i] = (TH1F*)file->Get(name_eta);
      FitValues fitLX = DrawFitFunc(HDPIX_ETA[i],kRed,false);
      ntuplePIXEtaResVsIter->Fill(IterCounter,1,0,i,
				  1000*fitLX.FitMean,1000*fitLX.FitSigma,
				  1000*fitLX.FitErrorMean,1000*fitLX.FitErrorSigma);
      
      sprintf(name_phi,"trkana/PIXRes/hPIXPhiResBaLay%dWide_%d",i,level);
      HDPIX_PHI[i] = (TH1F*)file->Get(name_phi);
      FitValues fitLX = DrawFitFunc(HDPIX_PHI[i],kRed,false);
      ntuplePIXPhiResVsIter->Fill(IterCounter,1,0,i,
				  1000*fitLX.FitMean,1000*fitLX.FitSigma,
				  1000*fitLX.FitErrorMean,1000*fitLX.FitErrorSigma);
      
    }
   
  } 

  else {
    // ntupleSCTResVsIter->Scan();
    // ntuplePIXPhiResVsIter->Scan();
    // ntuplePIXEtaResVsIter->Scan();
    
    cBarrelResVsIters = new TCanvas("cBarrelResVsIters"," Barrel Residuals per Layer Vs Iteration",
				    CanvasSizeX6[0],CanvasSizeX6[1]);
    cBarrelResVsIters->Divide(3,2);
    
    TGraphErrors *hGraph[6][6];
    TLegend *leg0[6];
    //For SCT
    for (int i=0; i<6; i=i+3){
      cBarrelResVsIters->cd(i+1);
      for (int j=0; j<4; j++) {
	sprintf(name,"dettype==2 && bec==0 && layer==%d",j);
	
	if (i==0)  sprintf(name2,"mean");
	else if (i==3) sprintf(name2,"sigma");
		
	if (!showErrors) {
	  sprintf(name3,"%s:iter",name2);
	  ntupleSCTResVsIter->Draw(name3,name,"goff");
	  hGraph[i][j] = new TGraphErrors(ntupleSCTResVsIter->GetSelectedRows(), 
					  ntupleSCTResVsIter->GetV2(),
					  ntupleSCTResVsIter->GetV1());
	}
	
	else {
	  sprintf(name3,"%s:iter:error%s",name2,name2);
	  ntupleSCTResVsIter->Draw(name3,name,"goff");
	  hGraph[i][j] = new TGraphErrors(ntupleSCTResVsIter->GetSelectedRows(), 
					  ntupleSCTResVsIter->GetV2(),
					  ntupleSCTResVsIter->GetV1(),
					  ntupleSCTResVsIter->GetV3());
	}
	
	hGraph[i][j]->GetXaxis()->SetTitle("Iteration Number");
	sprintf(name3,"Residual %s (#mum)",name2);
	hGraph[i][j]->GetYaxis()->SetTitle(name3);
	hGraph[i][j]->SetMarkerStyle(4);
	hGraph[i][j]->SetMarkerSize(0.4);
	hGraph[i][j]->SetMarkerColor(ColorForSCTBarrel[j]);
	hGraph[i][j]->SetLineWidth(1);
	hGraph[i][j]->SetLineColor(ColorForSCTBarrel[j]);
	hGraph[i][j]->GetXaxis()->SetRangeUser(0.0,(float) IterCounter-1);
	hGraph[i][j]->GetXaxis()->SetNdivisions(IterCounter);
	if(i==0){
	  hGraph[i][j]->GetYaxis()->SetRangeUser(-50.0,50.0);
	  hGraph[i][j]->SetTitle("SCT Barrel Mean Residuals Vs Iteration");
	}
	else{
	  hGraph[i][j]->GetYaxis()->SetRangeUser(30.0,80.0);
	  hGraph[i][j]->SetTitle(" SCT Barrel Sigma Residuals Vs Iteration");
	}
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	if (j==0) hGraph[i][j]->Draw("ALP");
	else hGraph[i][j]->Draw("LP");    
	
      }
      
      
      cBarrelResVsIters->Update();
      drawVerticalLine(hGraph[i][0],(double) IterCounter-1);
      gPad->Update();
      setTitleFont(font);
      setTitleSize(0.68);

      // Legend     
      leg0[i] = new TLegend(0.70,0.79,0.98,0.98);
      for (int j=0; j<4; j++) {
	sprintf(name,"SCT Barrel %d",j);
	leg0[i]->AddEntry(hGraph[i][j],name,"L");
      }
      leg0[i]->SetTextSize(0.035);
      leg0[i]->SetFillColor(kWhite);
      leg0[i]->Draw();
    }
        
    //For PIXELS PHI
    for (int i=1; i<6; i=i+3){
      cBarrelResVsIters->cd(i+1);
      for (int j=0; j<3; j++) {
	sprintf(name,"dettype==1 && bec==0 && layer==%d",j);
	
	if (i==1) sprintf(name2,"mean");
	else if (i==4) sprintf(name2,"sigma");
	
	if (!showErrors) {
	  sprintf(name3,"%s:iter",name2);
	  ntuplePIXPhiResVsIter->Draw(name3,name,"goff");
	  hGraph[i][j] = new TGraphErrors(ntuplePIXPhiResVsIter->GetSelectedRows(), 
					  ntuplePIXPhiResVsIter->GetV2(),
					  ntuplePIXPhiResVsIter->GetV1());
	}
	
	else {
	  sprintf(name3,"%s:iter:error%s",name2,name2);
	  ntuplePIXPhiResVsIter->Draw(name3,name,"goff");
	  hGraph[i][j] = new TGraphErrors(ntuplePIXPhiResVsIter->GetSelectedRows(), 
					  ntuplePIXPhiResVsIter->GetV2(),
					  ntuplePIXPhiResVsIter->GetV1(),
					  ntuplePIXPhiResVsIter->GetV3());
	}
		
	hGraph[i][j]->GetXaxis()->SetTitle("Iteration Number");
	sprintf(name3,"Residual #phi %s (#mum)",name2);
	hGraph[i][j]->GetYaxis()->SetTitle(name3);
	hGraph[i][j]->SetMarkerStyle(4);
	hGraph[i][j]->SetMarkerSize(0.4);
	hGraph[i][j]->SetMarkerColor(ColorForSCTBarrel[j]);
	hGraph[i][j]->SetLineWidth(1);
	hGraph[i][j]->SetLineColor(ColorForSCTBarrel[j]);
	hGraph[i][j]->GetXaxis()->SetRangeUser(0.0,(float) IterCounter-1);
	hGraph[i][j]->GetXaxis()->SetNdivisions(IterCounter);
	if(i==1){
	  hGraph[i][j]->GetYaxis()->SetRangeUser(-35.0,35.0);
	  hGraph[i][j]->SetTitle("Pixel Phi Barrel Mean Res Vs Iter");
	}
	else{
	  hGraph[i][j]->GetYaxis()->SetRangeUser(20.0,50.0);
	  hGraph[i][j]->SetTitle(" Pixel Phi Barrel Sigma Res Vs Iter");
	}
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	if (j==0) hGraph[i][j]->Draw("ALP");
	else hGraph[i][j]->Draw("LP");    
	
      }
            
      cBarrelResVsIters->Update();
      drawVerticalLine(hGraph[i][0],(double) IterCounter-1);
      gPad->Update();
      setTitleFont(font);
      setTitleSize(0.65);

      // Legend     
      leg0[i] = new TLegend(0.68,0.79,0.98,0.98);
      for (int j=0; j<3; j++) {
	sprintf(name,"Pixel Barrel %d",j);
	leg0[i]->AddEntry(hGraph[i][j],name,"L");
      }
      
      leg0[i]->SetTextSize(0.035);
      leg0[i]->SetFillColor(kWhite);
      leg0[i]->Draw();
    }
   

    //For PIXELS ETA
    for (int i=2; i<6; i=i+3){
      cBarrelResVsIters->cd(i+1);
      for (int j=0; j<3; j++) {
	sprintf(name,"dettype==1 && bec==0 && layer==%d",j);
	
	if (i==2)  sprintf(name2,"mean");
	else if (i==5) sprintf(name2,"sigma");
	
	if (!showErrors) {
	  sprintf(name3,"%s:iter",name2);
	  ntuplePIXEtaResVsIter->Draw(name3,name,"goff");
	  hGraph[i][j] = new TGraphErrors(ntuplePIXEtaResVsIter->GetSelectedRows(), 
					  ntuplePIXEtaResVsIter->GetV2(),
					  ntuplePIXEtaResVsIter->GetV1());
	}
	
	else {
	  sprintf(name3,"%s:iter:error%s",name2,name2);
	  ntuplePIXPhiResVsIter->Draw(name3,name,"goff");
	  hGraph[i][j] = new TGraphErrors(ntuplePIXEtaResVsIter->GetSelectedRows(), 
					  ntuplePIXEtaResVsIter->GetV2(),
					  ntuplePIXEtaResVsIter->GetV1(),
					  ntuplePIXEtaResVsIter->GetV3());
	}
		
	hGraph[i][j]->GetXaxis()->SetTitle("Iteration Number");
	sprintf(name3,"Residual #eta %s (#mum)",name2);
	hGraph[i][j]->GetYaxis()->SetTitle(name3);
	hGraph[i][j]->SetMarkerStyle(4);
	hGraph[i][j]->SetMarkerSize(0.4);
	hGraph[i][j]->SetMarkerColor(ColorForSCTBarrel[j]);
	hGraph[i][j]->SetLineWidth(1);
	hGraph[i][j]->SetLineColor(ColorForSCTBarrel[j]);
	hGraph[i][j]->GetXaxis()->SetRangeUser(0.0,(float) IterCounter-1);
	hGraph[i][j]->GetXaxis()->SetNdivisions(IterCounter);
	if(i==2){
	  hGraph[i][j]->GetYaxis()->SetRangeUser(-35.0,35.0);
	  hGraph[i][j]->SetTitle("Pixel Eta Barrel Mean Res Vs Iter");
	}
	else{
	  hGraph[i][j]->GetYaxis()->SetRangeUser(20.0,100.0);
	  hGraph[i][j]->SetTitle(" Pixel Eta Barrel Sigma Res Vs Iter");
	}
	if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
	if (j==0) hGraph[i][j]->Draw("ALP");
	else hGraph[i][j]->Draw("LP");    
	
      }
            
      cBarrelResVsIters->Update();
      drawVerticalLine(hGraph[i][0],(double) IterCounter-1);
      gPad->Update();
      setTitleFont(font);
      setTitleSize(0.65);

      // Legend
      leg0[i] = new TLegend(0.68,0.79,0.98,0.98);
      for (int j=0; j<3; j++) {
	sprintf(name,"Pixel Barrel %d",j);
	leg0[i]->AddEntry(hGraph[i][j],name,"L");
      }
      
      leg0[i]->SetTextSize(0.035);
      leg0[i]->SetFillColor(kWhite);
      leg0[i]->Draw();
    }
    
  }
  return;
}

