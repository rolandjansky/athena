/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void corrVsPixelHits() {

  if (fileResults!=NULL && !SCTOnly) {
  
  cout << " Pixel Alignment Corrections Vs Pixel Hits..." << endl;

  cCorrVsPixelHits = new TCanvas("cCorrVsPixelHits","Pixel Alignment Corrections Vs Hits",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cCorrVsPixelHits->Divide(3,2);

  Color_t icolor = 33;

  TGraph *hCorrVsPixelHits[6];
  TCut cutDetType = "dettype==1";

  for (int i=0; i<6; i++) {
    
    if (i==0) sprintf(name,"Tx");
    else if (i==1) sprintf(name,"Ty");
    else if (i==2) sprintf(name,"Tz");
    else if (i==3) sprintf(name,"Rx");
    else if (i==4) sprintf(name,"Ry");
    else if (i==5) sprintf(name,"Rz");

    if (i<3)
      sprintf(command,"mm");
    else
      sprintf(command,"mrad");

    sprintf(name2,"%s:hits",name);

    cCorrVsPixelHits->cd(i+1);
    sprintf(name2,"%s",name2);
    treeConsts->Draw(name2,cutDetType,"goff");

    hCorrVsPixelHits[i] = new TGraph(treeConsts->GetSelectedRows(), treeConsts->GetV2(), treeConsts->GetV1());
    sprintf(name2,"Local Pixel align correction - #Delta%s Vs Hits per Module",name);
    hCorrVsPixelHits[i]->SetTitle(name2);
    hCorrVsPixelHits[i]->GetXaxis()->SetTitle("Hits per module");
    sprintf(name2,"#Delta%s_{Pixel} (%s)",name,command);
    hCorrVsPixelHits[i]->GetYaxis()->SetTitle(name2);
    hCorrVsPixelHits[i]->GetYaxis()->SetLabelSize(0.025);
    hCorrVsPixelHits[i]->GetXaxis()->SetLabelSize(0.03);
    hCorrVsPixelHits[i]->SetMarkerStyle(4);
    hCorrVsPixelHits[i]->SetMarkerSize(0.5);
    hCorrVsPixelHits[i]->SetMarkerColor(icolor);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hCorrVsPixelHits[i]->Draw("AP");
    setTitleFont(font);
    setTitleSize(0.85);
  
  }

  cCorrVsPixelHits->Update();

  }

  return;
}
