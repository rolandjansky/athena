/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCTConstants() {

  if (fileResults!=NULL && !PixelOnly) {
  
  cout << " SCT Alignment Corrections..." << endl;

  cSCTConsts = new TCanvas("cSCTConsts","SCT Alignment Corrections",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cSCTConsts->Divide(3,2);

  Color_t icolor = 33;

  TH1F *hSCTConst[6];
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

    cSCTConsts->cd(i+1);   
    sprintf(name2,"%s>>hSCTConst%s",name,name);
    treeConsts->Draw(name2,cutDetType);

    sprintf(name2,"hSCTConst%s",name);
    hSCTConst[i] = (TH1F*)gDirectory->FindObject(name2);
    sprintf(name2,"Local SCT align correction - #Delta%s",name);
    hSCTConst[i]->SetTitle(name2);
    hSCTConst[i]->GetYaxis()->SetTitle("Entries");
    sprintf(name2,"#Delta%s_{SCT} (%s)",name,command);
    hSCTConst[i]->GetXaxis()->SetTitle(name2);
    hSCTConst[i]->GetXaxis()->SetNdivisions(8);
    hSCTConst[i]->GetXaxis()->SetTitleOffset(1.25);
    hSCTConst[i]->SetFillColor(icolor);
    hSCTConst[i]->SetLineColor(icolor+4);
    hSCTConst[i]->SetFillStyle(3001);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hSCTConst[i]->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
  }

  cSCTConsts->Update();

  }

  return;
}
