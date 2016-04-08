/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void errorsVsPixelHits() {

  if (fileResults!=NULL && !SCTOnly) {
  
  cout << " Pixel Alignment Correction Errors Vs Pixel Hits..." << endl;

  cErrorsVsPixelHits = new TCanvas("cErrorsVsPixelHits","Alignment Correction Errors Vs Pixel Hits",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cErrorsVsPixelHits->Divide(3,2);

  Color_t icolor = kMagenta-2;

  TGraph *hErrorsVsPixelHits[6];
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

    sprintf(name2,"error%s:hits",name);

    cErrorsVsPixelHits->cd(i+1);
    sprintf(name2,"%s>>hErrorsVsPixelHits%s",name2,name);
    treeConsts->Draw(name2,cutDetType,"goff");

    hErrorsVsPixelHits[i] = new TGraph(treeConsts->GetSelectedRows(), treeConsts->GetV2(), treeConsts->GetV1());
    sprintf(name2,"Local Pixel align correction error - #sigma(#Delta%s) Vs Hits per Module",name);
    hErrorsVsPixelHits[i]->SetTitle(name2);
    hErrorsVsPixelHits[i]->GetXaxis()->SetTitle("Hits per module");
    sprintf(name2,"#sigma(#Delta%s_{Pixel}) (%s)",name,command);
    hErrorsVsPixelHits[i]->GetYaxis()->SetTitle(name2);
    hErrorsVsPixelHits[i]->GetYaxis()->SetLabelSize(0.025);
    hErrorsVsPixelHits[i]->GetXaxis()->SetLabelSize(0.03);
    hErrorsVsPixelHits[i]->SetMarkerStyle(4);
    hErrorsVsPixelHits[i]->SetMarkerSize(0.5);
    hErrorsVsPixelHits[i]->SetMarkerColor(icolor);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hErrorsVsPixelHits[i]->Draw("AP");
    setTitleFont(font);
    setTitleSize(0.95);
  
  }

  cErrorsVsPixelHits->Update();

  }

  return;
}
