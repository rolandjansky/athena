/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trt_rpc_time(int SaveOpt=0,int STATION=2, int DOUBLR=1, int GASGAP=1) {

  cout<<"Building the correlations between TRT and RPC (Muons)..."<<endl;

  // Style
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_rdo_*",1);
  tree->SetBranchStatus("r*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);


  // These variables should be moved to monitor.h
  int trt_nrdos;
  int trt_rdo_timepll[MAX], trt_rdo_layer[MAX], trt_rdo_phisector[MAX], trt_rdo_strawlayer[MAX];
  int nrpcdig;
  float rtime[MAX],rdoublr[MAX],rdoublz[MAX],rstation[MAX],rmeasphi[MAX],rgasgap[MAX];

  int MEASPHI=1;

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // TRT
  tree->SetBranchAddress("trt_rdo_nrdos",&trt_nrdos);
  tree->SetBranchAddress("trt_rdo_timepll",trt_rdo_timepll);
  tree->SetBranchAddress("trt_rdo_layer",trt_rdo_layer);
  tree->SetBranchAddress("trt_rdo_phisector",trt_rdo_phisector);
  tree->SetBranchAddress("trt_rdo_strawlayer",trt_rdo_strawlayer);
  // RPC
  tree->SetBranchAddress("nrpcdig",&nrpcdig);
  tree->SetBranchAddress("rtime",rtime);
  tree->SetBranchAddress("rdoublr",rdoublr);
  tree->SetBranchAddress("rdoublz",rdoublz);
  tree->SetBranchAddress("rstation",rstation);
  tree->SetBranchAddress("rmeasphi",rmeasphi);
  tree->SetBranchAddress("rgasgap",rgasgap);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH2F *trtrpchisto;
  sprintf(name,"corr_trt_rpc_%d_%d_%d",STATION,DOUBLR,GASGAP);
  c11 = new TCanvas(name,"Correlations Offline Monitor - TRT [layer,phi] / RPC [station,doublr,gasgap]");
  c11->Divide(3,2);
  int index=1;

  for (int iphi=0;iphi<2;iphi++) { 
    for (int ilayer=0;ilayer<3;ilayer++) { 
      
      sprintf(name,"TRT [%d,%d] / RPC Correlations",ilayer,iphi);
      sprintf(name2,"corr_trtrpc_%d%d%d%d_%d%d",STATION,DOUBLR,GASGAP,MEASPHI,ilayer,iphi);
      trtrpchisto = new TH2F(name2,name,18,-0.5,17.5,65,-0.5,64.5);
      
      for (int i=0;i<entries;i++) {

	if(i%100 == 0) cout<<"."; fflush(stdout); 

	tree->GetEntry(i);
	for(int j=0; j<trt_nrdos; j++) {
	  if (trt_rdo_phisector[j]==iphi) {
	    if (trt_rdo_layer[j]==ilayer) {
	      for(int jj=0; jj<nrpcdig; jj++) {
		if (rstation[jj]==STATION && rdoublr[jj]==DOUBLR && rgasgap[jj]==GASGAP && rdoublz[jj]==1 && rmeasphi[jj]==MEASPHI) trtrpchisto->Fill(trt_rdo_timepll[j],rtime[jj]);
	      }	
	    }
	  }
	}
      }
      c11->cd(index);
      sprintf(axisX,"TRT timepll [%d,%d]",ilayer,iphi);
      sprintf(axisY,"rtime (RPC) [%d,%d,%d]",STATION,DOUBLR,GASGAP);
      trtrpchisto->GetXaxis()->SetTitle(axisX);
      trtrpchisto->GetYaxis()->SetTitle(axisY);
      trtrpchisto->DrawCopy("COLZ");
      c11->Update();
      index++;
    }
  }

  cout << "!\n" << endl;

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

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRTRPC_time.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
