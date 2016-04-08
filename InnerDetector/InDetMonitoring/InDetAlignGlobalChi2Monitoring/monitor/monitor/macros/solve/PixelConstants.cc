/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PixelConstants() {

  if (fileResults!=NULL && !SCTOnly) {
  
  cout << " Pixel Alignment Corrections..." << endl;

  cPixelConsts = new TCanvas("cPixelConsts","Pixel Alignment Corrections",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cPixelConsts->Divide(3,2);

  Color_t icolor = 33;

  TH1F *hPixelConst[6];
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

    cPixelConsts->cd(i+1);   
    sprintf(name2,"%s>>hPixelConst%s",name,name);
    treeConsts->Draw(name2,cutDetType);

    sprintf(name2,"hPixelConst%s",name);
    hPixelConst[i] = (TH1F*)gDirectory->FindObject(name2);
    sprintf(name2,"Local Pixel align correction - #Delta%s",name);
    hPixelConst[i]->SetTitle(name2);
    hPixelConst[i]->GetYaxis()->SetTitle("Entries");
    sprintf(name2,"#Delta%s_{Pixel} (%s)",name,command);
    hPixelConst[i]->GetXaxis()->SetTitle(name2);
    hPixelConst[i]->GetXaxis()->SetNdivisions(8);
    hPixelConst[i]->GetXaxis()->SetTitleOffset(1.25);
    hPixelConst[i]->SetFillColor(icolor);
    hPixelConst[i]->SetLineColor(icolor+4);
    hPixelConst[i]->SetFillStyle(3001);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hPixelConst[i]->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
  }

  cPixelConsts->Update();

  }

  return;
}
