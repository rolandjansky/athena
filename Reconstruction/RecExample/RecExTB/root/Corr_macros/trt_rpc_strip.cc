/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trt_rpc_strip(int SaveOpt=0,int STATION=2, int DOUBLR=1, int GASGAP=1) {

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
  int trt_rdo_straw[MAX], trt_rdo_layer[MAX], trt_rdo_phisector[MAX], trt_rdo_strawlayer[MAX];
  int nrpcdig;
  float rstrip[MAX],rstripY[MAX],rdoublr[MAX],rdoublz[MAX],rstation[MAX],rmeasphi[MAX],rgasgap[MAX];

  int MEASPHI=1;

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // TRT
  tree->SetBranchAddress("trt_rdo_nrdos",&trt_nrdos);
  tree->SetBranchAddress("trt_rdo_straw",trt_rdo_straw);
  tree->SetBranchAddress("trt_rdo_layer",trt_rdo_layer);
  tree->SetBranchAddress("trt_rdo_phisector",trt_rdo_phisector);
  tree->SetBranchAddress("trt_rdo_strawlayer",trt_rdo_strawlayer);
  // RPC
  tree->SetBranchAddress("nrpcdig",&nrpcdig);
  tree->SetBranchAddress("rstrip",rstrip);
  tree->SetBranchAddress("rstripy",rstripY);
  tree->SetBranchAddress("rdoublr",rdoublr);
  tree->SetBranchAddress("rdoublz",rdoublz);
  tree->SetBranchAddress("rstation",rstation);
  tree->SetBranchAddress("rmeasphi",rmeasphi);
  tree->SetBranchAddress("rgasgap",rgasgap);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH2F *trtrpchisto;
  sprintf(name,"corr_trt_rpc_strip_%d_%d_%d",STATION,DOUBLR,GASGAP);
  c11 = new TCanvas(name,"Correlations Offline Monitor - TRT [layer,phi] / RPC [station,doublr,gasgap]");
  c11->Divide(3,2);
  int index=1;

  for (int iphi=0;iphi<2;iphi++) { 
    for (int ilayer=0;ilayer<3;ilayer++) { 
      
      sprintf(name,"TRT [%d,%d] / RPC Correlations",ilayer,iphi);
      sprintf(name2,"corr_trtrpc_%d%d%d%d_%d%d",STATION,DOUBLR,GASGAP,MEASPHI,ilayer,iphi);
      trtrpchisto = new TH2F(name2,name,30,-0.5,29.5,80,-0.5,79.5);
      //      trtrpchisto = new TH2F(name2,name,30,-0.5,29.5,200,-0.5,199.5);
      
      for (int i=0;i<entries;i++) {

	if(i%100 == 0) cout<<"."; fflush(stdout); 

	tree->GetEntry(i);
	for(int j=0; j<trt_nrdos; j++) {
	  if (trt_rdo_phisector[j]==iphi) {
	    if (trt_rdo_layer[j]==ilayer) {
	      if (trt_rdo_strawlayer[j]==10) {
		for(int jj=0; jj<nrpcdig; jj++) {
		  if (rstation[jj]==STATION && rdoublr[jj]==DOUBLR && rgasgap[jj]==GASGAP && rdoublz[jj]==1 && rmeasphi[jj]==MEASPHI) trtrpchisto->Fill(trt_rdo_straw[j],rstrip[jj]);
		}	
	      }
	    }
	  }
	}
      }
      c11->cd(index);
      sprintf(axisX,"straw (TRT) [%d,%d]",ilayer,iphi);
      sprintf(axisY,"rstrip (RPC) [%d,%d,%d]",STATION,DOUBLR,GASGAP);
      trtrpchisto->GetXaxis()->SetTitle(axisX);
      trtrpchisto->GetYaxis()->SetTitle(axisY);
      trtrpchisto->DrawCopy("COLZ");
      c11->Update();
      index++;
    }
  }
  
  cout << "!\n" << endl;

  gStyle->SetOptStat("nemr");
  
  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRTRPC_strip.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
