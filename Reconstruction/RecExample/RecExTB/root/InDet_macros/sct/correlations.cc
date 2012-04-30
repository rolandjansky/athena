/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void correlations(int SaveOpt=0,int Phi=0) {

  cout<<"Building the correlations..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_rdo_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  tree->SetBranchAddress("sct_rdo_nhits",&rdo_nhits);
  tree->SetBranchAddress("sct_rdo_strips",rdo_strips);
  tree->SetBranchAddress("sct_rdo_layer",rdo_layer);
  tree->SetBranchAddress("sct_rdo_phi",rdo_phi);
  tree->SetBranchAddress("sct_rdo_side",rdo_side);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH2F *ihisto[6];

  for (Int_t i=0;i<6;i++) {
    sprintf(name,"Correlations %d (phi=%d)",i,Phi);
    sprintf(name2,"correlations_%d_phi%d",i,Phi);
    ihisto[i] = new TH2F(name2,name,768,-0.5,767.5,768,-0.5,767.5);
    if (Phi==0) ihisto[i]->SetMarkerColor(kRed); 
    else ihisto[i]->SetMarkerColor(kBlue);
  }

  for (Int_t i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout);  
    
    tree->GetEntry(i);
    counter=0;
    
    if (rdo_nhits==8) {
      for(int j=0; j<rdo_nhits; j++) {
 	if (rdo_phi[j]==Phi) counter++;
      }
      if (counter==8) {
	for(int j=0; j<rdo_nhits; j++) {
	  if (rdo_layer[j]==0 && rdo_side[j]==0) istrips[0] = rdo_strips[j];
	  else if (rdo_layer[j]==1 && rdo_side[j]==0) istrips[2] = rdo_strips[j];
	  else if (rdo_layer[j]==2 && rdo_side[j]==0) istrips[4] = rdo_strips[j];
	  else if (rdo_layer[j]==3 && rdo_side[j]==0) istrips[6] = rdo_strips[j];
	  else if (rdo_layer[j]==0 && rdo_side[j]==1) istrips[1] = rdo_strips[j];
	  else if (rdo_layer[j]==1 && rdo_side[j]==1) istrips[3] = rdo_strips[j];
	  else if (rdo_layer[j]==2 && rdo_side[j]==1) istrips[5] = rdo_strips[j];
	  else if (rdo_layer[j]==3 && rdo_side[j]==1) istrips[7] = rdo_strips[j];
	}
	
	ihisto[0]->Fill(istrips[0],istrips[2]);
	ihisto[1]->Fill(istrips[0],istrips[4]);
	ihisto[2]->Fill(istrips[0],istrips[6]);
	ihisto[3]->Fill(istrips[1],istrips[3]);
	ihisto[4]->Fill(istrips[1],istrips[5]);
	ihisto[5]->Fill(istrips[1],istrips[7]);
      }
    }
  }  

  cout << "!\n" << endl;

  sprintf(name,"SCT Offline Monitor - Correlations (phi=%d)",Phi);
  sprintf(name2,"correlations_phi_%d",Phi);
  c11 = new TCanvas(name2,name);
  c11->Divide(3,2);

  for (Int_t i=0;i<6;i++) {
    c11->cd(i+1);
    if (i<3) {
    sprintf(axisX,"strip - [%d,%d,%d]",0,Phi,0);
    sprintf(axisY,"strip - [%d,%d,%d]",i+1,Phi,0);
    }
    else {
    sprintf(axisX,"strip - [%d,%d,%d]",0,Phi,1);
    sprintf(axisY,"strip - [%d,%d,%d]",i-2,Phi,1);
    }
    ihisto[i]->GetXaxis()->SetTitle(axisX);
    ihisto[i]->GetYaxis()->SetTitle(axisY);
    ihisto[i]->DrawCopy();
    c11->Update();
    delete ihisto[i];
  }


  sprintf(title, "run %d (%d events)", run, entries);
  c11->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  c11->Update();

  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("SCT_histograms.ps");
  else if(SaveOpt==2) {sprintf(sname,"SCTcorrelations_phi_%d.gif",Phi); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
