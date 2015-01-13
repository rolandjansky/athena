/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void CosmicSegments(TString trkCol){
	pixelResiduals(trkCol, NULL, "");
}

void CosmicSegments(TString trkCol, TCanvas *myCanvas[]){
	CosmicSegments(trkCol, myCanvas, "");
}

void CosmicSegments(TString trkCol_1, TCanvas *myCanvas[], TString trkCol_2) {
  cout << " <CosmicSegments> collections: " << trkCol_1.Data() << endl;
  
  TString histname;
  TH1F* myHisto;
  TH2F* myHisto2D;
  TProfile *myProfile;

  std::vector <TString> TrackCollection;
  if (trkCol_1 != "") TrackCollection.push_back(trkCol_1);
  if (trkCol_2 != "") TrackCollection.push_back(trkCol_2);
  if (myCanvas[0] != NULL) myCanvas[0]->Divide(3,2);
  if (myCanvas[1] != NULL) myCanvas[1]->Divide(3,2);
  if (myCanvas[2] != NULL) myCanvas[2]->Divide(3,2);
  if (myCanvas[3] != NULL) myCanvas[3]->Divide(3,2);
  if (myCanvas[4] != NULL) myCanvas[4]->Divide(3,2);


  for (int i=0; i < (int) TrackCollection.size(); i++) {

    // silicon hits
    myCanvas[0]->cd(i*3+1);
    // check if pixel histos exist
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_pixUp";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("Pixel hits"); 
      myHisto->DrawCopy();
    }
    else {// in older version pix and sct hist were not there
      myCanvas[0]->cd(i*3+2);
      histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_siUp";
      cout << " Histname ("<< i <<") = " << histname.Data() << endl;
      myHisto = (TH1F*)file->Get(histname);
      if (myHisto != NULL) {
	myHisto->GetXaxis()->SetTitle("Hits in silicon"); 
	myHisto->DrawCopy();
      }
    }

    // SCT hits
    myCanvas[0]->cd(i*3+2);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_sctUp";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("SCT hits"); 
      myHisto->DrawCopy();
    }

    // TRT hits
    myCanvas[0]->cd(i*3+3);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_trtUp";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("Hits in TRT"); 
      myHisto->DrawCopy();
    }

    // pix hits in low segment
    myCanvas[0]->cd(i*3+4);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_pixLow";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("Hits in silicon"); 
      myHisto->DrawCopy();
    }
    else {
      // silicon hits
      myCanvas[0]->cd(i*3+5);
      histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_siLow";
      cout << " Histname ("<< i <<") = " << histname.Data() << endl;
      myHisto = (TH1F*)file->Get(histname);
      if (myHisto != NULL) {
	myHisto->GetXaxis()->SetTitle("Hits in silicon"); 
	myHisto->DrawCopy();
      }
    }

    // SCT hits
    myCanvas[0]->cd(i*3+5);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_sctLow";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("SCT hits"); 
      myHisto->DrawCopy();
    }

    // TRT hits
    myCanvas[0]->cd(i*3+6);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/nhits_trtLow";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("Hits in TRT"); 
      myHisto->DrawCopy();
    }

    ///////////////////////////////////////
    // Upper segments track parameters
    ///////////////////////////////////////
    myCanvas[1]->cd(i*3+1);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Upper_d0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("d_{0} [mm]"); 
      myHisto->DrawCopy();
    }

    myCanvas[1]->cd(i*3+2);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Upper_z0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("z_{0} [mm]"); 
      myHisto->DrawCopy();
    }

    myCanvas[1]->cd(i*3+3);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Upper_phi0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#phi_{0} [rad]"); 
      myHisto->DrawCopy();
    }

    myCanvas[1]->cd(i*3+4);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Upper_eta0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#eta"); 
      myHisto->DrawCopy();
    }

    myCanvas[1]->cd(i*3+5);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Upper_qOverPt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("q/p_{T} [GeV^{-1}]"); 
      myHisto->DrawCopy();
    }

    myCanvas[1]->cd(i*3+6);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Upper_Pt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("p_{T} [GeV]"); 
      myHisto->DrawCopy();
    }


    // Lower segments track parameters
    myCanvas[2]->cd(i*3+1);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Lower_d0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("d_{0} [mm]"); 
      myHisto->DrawCopy();
    }

    myCanvas[2]->cd(i*3+2);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Lower_z0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("z_{0} [mm]"); 
      myHisto->DrawCopy();
    }

    myCanvas[2]->cd(i*3+3);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Lower_phi0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#phi_{0} [rad]"); 
      myHisto->DrawCopy();
    }

    myCanvas[2]->cd(i*3+4);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Lower_eta0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#eta"); 
      myHisto->DrawCopy();
    }

    myCanvas[2]->cd(i*3+5);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Lower_qOverPt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("q/p_{T} [GeV^{-1}]"); 
      myHisto->DrawCopy();
    }

    myCanvas[2]->cd(i*3+6);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/Lower_Pt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("p_{T} [GeV]"); 
      myHisto->DrawCopy();
    }


    // Delta = Upper-lower segments track parameters
    myCanvas[3]->cd(i*3+1);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_d0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#Delta d_{0} [mm]"); 
      myHisto->DrawCopy();
    }

    myCanvas[3]->cd(i*3+2);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_z0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#Delta z_{0} [mm]"); 
      myHisto->DrawCopy();
    }

    myCanvas[3]->cd(i*3+3);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_phi0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#Delta #phi_{0} [rad]"); 
      myHisto->DrawCopy();
    }

    myCanvas[3]->cd(i*3+4);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_eta0";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#Delta #eta"); 
      myHisto->DrawCopy();
    }

    myCanvas[3]->cd(i*3+5);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_pt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#Delta p_{T} [GeV]"); 
      myHisto->DrawCopy();
    }

    // Delta vs pt = Upper-lower segments track parameters vs Pt
    myCanvas[4]->cd(i*3+1);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_d0VsPt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto2D = (TH2F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto2D->GetXaxis()->SetTitle("Track p_{T} [GeV]"); 
      myHisto2D->GetYaxis()->SetTitle("#Delta d_{0} [mm]"); 
      // myHisto2D->DrawCopy("colz");
      myProfile = myHisto2D->ProfileX();
      SymmetricYaxis(myProfile, true, true);
      myProfile->SetStats(kFALSE);
      myProfile->SetMarkerStyle(20);
      myProfile->GetYaxis()->SetTitle("#Delta d_{0} [mm]"); 
      myProfile->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }

    myCanvas[4]->cd(i*3+2);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_z0VsPt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto2D = (TH2F*)file->Get(histname);
    if (myHisto2D != NULL) {
      myHisto2D->GetXaxis()->SetTitle("Track p_{T} [GeV]"); 
      myHisto2D->GetYaxis()->SetTitle("#Delta z_{0} [mm]"); 
      //      myHisto2D->DrawCopy("colz");
      myProfile = myHisto2D->ProfileX();
      SymmetricYaxis(myProfile, true, true);
      myProfile->SetStats(kFALSE);
      myProfile->SetMarkerStyle(20);
      myProfile->GetYaxis()->SetTitle("#Delta z_{0} [mm]"); 
      myProfile->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }

    myCanvas[4]->cd(i*3+3);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_phi0VsPt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto2D = (TH2F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto2D->GetXaxis()->SetTitle("Track p_{T} [GeV]"); 
      myHisto2D->GetYaxis()->SetTitle("#Delta #phi_{0} [rad]"); 
      myHisto2D->DrawCopy("colz");
      myProfile = myHisto2D->ProfileX();
      SymmetricYaxis(myProfile, true, true);
      myProfile->GetYaxis()->SetTitle("#Delta #phi_{0} [rad]"); 
      myProfile->SetStats(kFALSE);
      myProfile->SetMarkerStyle(20);
      myProfile->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }

    myCanvas[4]->cd(i*3+4);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/delta_eta0VsPt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto2D = (TH2F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto2D->GetXaxis()->SetTitle("Track p_{T} [GeV]"); 
      myHisto2D->GetXaxis()->SetTitle("#Delta #eta"); 
      myHisto2D->DrawCopy("colz");
      myProfile = myHisto2D->ProfileX();
      SymmetricYaxis(myProfile, true, true);
      myProfile->SetStats(kFALSE);
      myProfile->GetYaxis()->SetTitle("#Delta #eta"); 
      myProfile->SetMarkerStyle(20);
      myProfile->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }

    myCanvas[4]->cd(i*3+5);
    histname = "IDAlignMon/"+TrackCollection.at(i)+"/TrackSegments/reldelta_pt";
    cout << " Histname ("<< i <<") = " << histname.Data() << endl;
    myHisto = (TH1F*)file->Get(histname);
    if (myHisto != NULL) {
      myHisto->GetXaxis()->SetTitle("#Delta p_{T} [relative]"); 
      myHisto->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }



  }
  
  return;
}
