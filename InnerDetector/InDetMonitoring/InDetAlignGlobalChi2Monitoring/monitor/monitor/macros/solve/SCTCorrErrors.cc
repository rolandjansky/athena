/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCTCorrErrors() {

  if (fileResults!=NULL && !PixelOnly) {
  
  cout << " SCT Alignment Correction Errors..." << endl;

  cSCTErrorConsts = new TCanvas("cSCTErrorConsts","SCT Alignment Correction Errors",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cSCTErrorConsts->Divide(3,2);

  Color_t icolor = kMagenta-2;

  TH1F *hSCTConstError[6];
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

    sprintf(name2,"error%s",name);

    cSCTErrorConsts->cd(i+1);   
    sprintf(name2,"%s>>hSCTConstError%s",name2,name);
    treeConsts->Draw(name2,cutDetType);

    sprintf(name2,"hSCTConstError%s",name);    
    hSCTConstError[i] = (TH1F*)gDirectory->FindObject(name2);
    sprintf(name2,"Local SCT align correction error - #Delta%s",name);
    hSCTConstError[i]->SetTitle(name2);
    hSCTConstError[i]->GetYaxis()->SetTitle("Entries");
    sprintf(name2,"#sigma(#Delta%s)_{SCT} (%s)",name,command);
    hSCTConstError[i]->GetXaxis()->SetTitle(name2);
    hSCTConstError[i]->GetXaxis()->SetNdivisions(8);
    hSCTConstError[i]->GetXaxis()->SetTitleOffset(1.25);
    hSCTConstError[i]->SetFillColor(icolor);
    hSCTConstError[i]->SetLineColor(icolor+4);
    hSCTConstError[i]->SetFillStyle(3001);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hSCTConstError[i]->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
  }

  cSCTErrorConsts->Update();

  }

  return;
}
