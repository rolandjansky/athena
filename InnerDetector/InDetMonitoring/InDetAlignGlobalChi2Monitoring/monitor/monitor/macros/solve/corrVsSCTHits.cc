/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void corrVsSCTHits() {

  if (fileResults!=NULL && !PixelOnly) {
  
  cout << " SCT Alignment Corrections Vs SCT Hits..." << endl;

  cCorrVsSCTHits = new TCanvas("cCorrVsSCTHits","SCT Alignment Corrections Vs Hits",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cCorrVsSCTHits->Divide(3,2);

  Color_t icolor = 33;

  TGraph *hCorrVsSCTHits[6];
  TCut cutDetType = "dettype==2";

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

    cCorrVsSCTHits->cd(i+1);
    sprintf(name2,"%s",name2);
    treeConsts->Draw(name2,cutDetType,"goff");

    hCorrVsSCTHits[i] = new TGraph(treeConsts->GetSelectedRows(), treeConsts->GetV2(), treeConsts->GetV1());
    sprintf(name2,"Local SCT align correction - #Delta%s Vs Hits per Module",name);
    hCorrVsSCTHits[i]->SetTitle(name2);
    hCorrVsSCTHits[i]->GetXaxis()->SetTitle("Hits per module");
    sprintf(name2,"#Delta%s_{SCT} (%s)",name,command);
    hCorrVsSCTHits[i]->GetYaxis()->SetTitle(name2);
    hCorrVsSCTHits[i]->GetYaxis()->SetLabelSize(0.025);
    hCorrVsSCTHits[i]->GetXaxis()->SetLabelSize(0.03);
    hCorrVsSCTHits[i]->SetMarkerStyle(4);
    hCorrVsSCTHits[i]->SetMarkerSize(0.5);
    hCorrVsSCTHits[i]->SetMarkerColor(icolor);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hCorrVsSCTHits[i]->Draw("AP");
    setTitleFont(font);
    setTitleSize(0.85);
  
  }

  cCorrVsSCTHits->Update();

  }

  return;
}
