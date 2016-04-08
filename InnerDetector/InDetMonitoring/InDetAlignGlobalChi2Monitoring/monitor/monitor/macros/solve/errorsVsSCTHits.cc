/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void errorsVsSCTHits() {

  if (fileResults!=NULL && !PixelOnly) {
  
  cout << " SCT Alignment Correction Errors Vs SCT Hits..." << endl;

  cErrorsVsSCTHits = new TCanvas("cErrorsVsSCTHits","Alignment Correction Errors Vs SCT Hits",
			   CanvasSizeX6[0],CanvasSizeX6[1]);
  cErrorsVsSCTHits->Divide(3,2);

  Color_t icolor = kMagenta-2;

  TGraph *hErrorsVsSCTHits[6];
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

    sprintf(name2,"error%s:hits",name);

    cErrorsVsSCTHits->cd(i+1);
    sprintf(name2,"%s>>hErrorsVsSCTHits%s",name2,name);
    treeConsts->Draw(name2,cutDetType,"goff");

    hErrorsVsSCTHits[i] = new TGraph(treeConsts->GetSelectedRows(), treeConsts->GetV2(), treeConsts->GetV1());
    sprintf(name2,"Local SCT align correction error - #sigma(#Delta%s) Vs Hits per Module",name);
    hErrorsVsSCTHits[i]->SetTitle(name2);
    hErrorsVsSCTHits[i]->GetXaxis()->SetTitle("Hits per module");
    sprintf(name2,"#sigma(#Delta%s_{SCT}) (%s)",name,command);
    hErrorsVsSCTHits[i]->GetYaxis()->SetTitle(name2);
    hErrorsVsSCTHits[i]->GetYaxis()->SetLabelSize(0.025);
    hErrorsVsSCTHits[i]->GetXaxis()->SetLabelSize(0.03);
    hErrorsVsSCTHits[i]->SetMarkerStyle(4);
    hErrorsVsSCTHits[i]->SetMarkerSize(0.5);
    hErrorsVsSCTHits[i]->SetMarkerColor(icolor);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hErrorsVsSCTHits[i]->Draw("AP");
    setTitleFont(font);
    setTitleSize(0.95);
  
  }

  cErrorsVsSCTHits->Update();

  }

  return;
}
