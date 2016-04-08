/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PixelCorrErrors() {

  if (fileResults!=NULL && !SCTOnly) {
  
  cout << " Pixel Alignment Correction Errors..." << endl;

  cPixelErrorConsts = new TCanvas("cPixelErrorConsts","Pixel Alignment Correction Errors",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cPixelErrorConsts->Divide(3,2);

  Color_t icolor = kMagenta-2;

  TH1F *hPixelConstError[6];
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

    sprintf(name2,"error%s",name);

    cPixelErrorConsts->cd(i+1);   
    sprintf(name2,"%s>>hPixelConstError%s",name2,name);
    treeConsts->Draw(name2,cutDetType);

    sprintf(name2,"hPixelConstError%s",name);    
    hPixelConstError[i] = (TH1F*)gDirectory->FindObject(name2);
    sprintf(name2,"Local Pixel align correction error - #Delta%s",name);
    hPixelConstError[i]->SetTitle(name2);
    hPixelConstError[i]->GetYaxis()->SetTitle("Entries");
    sprintf(name2,"#sigma(#Delta%s)_{Pixel} (%s)",name,command);
    hPixelConstError[i]->GetXaxis()->SetTitle(name2);
    hPixelConstError[i]->GetXaxis()->SetNdivisions(8);
    hPixelConstError[i]->GetXaxis()->SetTitleOffset(1.25);
    hPixelConstError[i]->SetFillColor(icolor);
    hPixelConstError[i]->SetLineColor(icolor+4);
    hPixelConstError[i]->SetFillStyle(3001);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hPixelConstError[i]->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
  }

  cPixelErrorConsts->Update();

  }

  return;
}
