void CaloTests_plotAllCaloNoise13(char * fname = "ntuple_elecnoise.root", int iMode=0) {

  ifstream ifile(fname,ios::in);
      
      if ( ifile.fail() )
	{
	  std::cout << fname << " not found !" << std::endl;
	  continue;
	}
      ifile.close();


  TFile * f = new TFile(fname);
  TCanvas * c0 = new TCanvas();
  gPad->SetLogy();
  gPad->SetGridx();
  gPad->SetGridy();
  TTree *allCaloNoises = (TTree *)f->Get("CollectionTree");
  allCaloNoises->SetMarkerSize(0.7);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  TH2F *base;
  if ( iMode == 0 ) {
    //    base = new TH2F("base","base",100,-5,5.,1000,2.,3000.);
    base = new TH2F("base","base",100,-5,5.,1000,1.,1000.);
    base->SetYTitle("Electronics Noise (MeV)");
  }
  else if (iMode == 1 ) {
    base = new TH2F("base","base",100,-5,5.,1000,1.,40000.);
    base->SetYTitle("Pile-Up Noise (MeV)");  
  }
  else {
    base = new TH2F("base","base",100,-5,5.,1000,1e4,1e9);
    base->SetYTitle("Cell Volume (mm^{3})");  
  }
  base->SetXTitle("#eta");
  base->Draw();
  TLegend * tla;
  if ( iMode != 2 ) 
    tla = new TLegend(0.1,0.1,0.2,0.4);
  else
    tla = new TLegend(0.1,0.5,0.2,0.9);
  tla->SetFillColor(10);
  tla->SetBorderSize(1);
  TLegend * tlb;
  if ( iMode == 0 ) 
    tlb = new TLegend(0.8,0.1,0.9,0.4);
  else if ( iMode == 1) 
    tlb = new TLegend(0.8,0.4-4./7.*0.3,0.9,0.4);
  else
    tlb = new TLegend(0.8,0.5,0.9,0.9);
  tlb->SetFillColor(10);
  tlb->SetBorderSize(1);
  c0->Modified();
  c0->Update();
  float iColor=45,dColor=8;
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(20);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>fcal1","cell_calo_topo==2&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>fcal1","cell_calo_topo==2&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>fcal1","cell_calo_topo==2&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tla->AddEntry(fcal1,"FCal1","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(21);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>fcal2","cell_calo_topo==2&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>fcal2","cell_calo_topo==2&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>fcal2","cell_calo_topo==2&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tla->AddEntry(fcal2,"FCal2","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(22);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>fcal3","cell_calo_topo==2&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>fcal3","cell_calo_topo==2&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>fcal3","cell_calo_topo==2&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tla->AddEntry(fcal3,"FCal3","p");
  iColor = 65;
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(20);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>hec1","cell_calo_topo==1&&cell_samp_topo==0&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>hec1","cell_calo_topo==1&&cell_samp_topo==0&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>hec1","cell_calo_topo==1&&cell_samp_topo==0&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tla->AddEntry(hec1,"HEC1","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(21);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>hec2","cell_calo_topo==1&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>hec2","cell_calo_topo==1&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>hec2","cell_calo_topo==1&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tla->AddEntry(hec2,"HEC2","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(22);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>hec3","cell_calo_topo==1&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>hec3","cell_calo_topo==1&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>hec3","cell_calo_topo==1&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tla->AddEntry(hec3,"HEC3","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(23);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>hec4","cell_calo_topo==1&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>hec4","cell_calo_topo==1&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>hec4","cell_calo_topo==1&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tla->AddEntry(hec4,"HEC4","p");
  iColor = 55;
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(20);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>em0","cell_calo_topo==0&&cell_samp_topo==0&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>em0","cell_calo_topo==0&&cell_samp_topo==0&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>em0","cell_calo_topo==0&&cell_samp_topo==0&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tlb->AddEntry(em0,"PS","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(21);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>em1","cell_calo_topo==0&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>em1","cell_calo_topo==0&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>em1","cell_calo_topo==0&&cell_samp_topo==1&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tlb->AddEntry(em1,"EM1","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(22);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>em2","cell_calo_topo==0&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>em2","cell_calo_topo==0&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>em2","cell_calo_topo==0&&cell_samp_topo==2&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tlb->AddEntry(em2,"EM2","p");
  allCaloNoises->SetMarkerColor(iColor+=dColor);
  allCaloNoises->SetMarkerStyle(23);
  if ( iMode == 0 ) 
    allCaloNoises->Draw("cellnoise_topo:celleta_topo>>em3","cell_calo_topo==0&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  else if ( iMode == 1 ) 
    allCaloNoises->Draw("sqrt(pow(celltotnoise_topo,2)-pow(cellnoise_topo,2)):celleta_topo>>em3","cell_calo_topo==0&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  else 
    allCaloNoises->Draw("cellvol_topo:celleta_topo>>em3","cell_calo_topo==0&&cell_samp_topo==3&&cell_iphi_topo==0","same");
  c0->Modified();
  c0->Update();
  tlb->AddEntry(em3,"EM3","p");
  if ( iMode != 1) {
    iColor = 75;
    allCaloNoises->SetMarkerColor(iColor+=dColor);
    allCaloNoises->SetMarkerStyle(20);
    if ( iMode == 0 )
      allCaloNoises->Draw("cellnoise_topo:celleta_topo>>tile1","cell_calo_topo==3&&cell_samp_topo==0&&cell_ireg_topo<3&&cell_iphi_topo==0","same");
    else
      allCaloNoises->Draw("cellvol_topo:celleta_topo>>tile1","cell_calo_topo==3&&cell_samp_topo==0&&cell_ireg_topo<3&&cell_iphi_topo==0","same");
    c0->Modified();
    c0->Update();
    tlb->AddEntry(tile1,"Tile1","p");
    allCaloNoises->SetMarkerColor(iColor+=dColor);
    allCaloNoises->SetMarkerStyle(21);
    if ( iMode == 0 )
      allCaloNoises->Draw("cellnoise_topo:celleta_topo>>tile2","cell_calo_topo==3&&cell_samp_topo==1&&cell_ireg_topo<3&&cell_iphi_topo==0","same");
    else
      allCaloNoises->Draw("cellvol_topo:celleta_topo>>tile2","cell_calo_topo==3&&cell_samp_topo==1&&cell_ireg_topo<3&&cell_iphi_topo==0","same");
    c0->Modified();
    c0->Update();
    tlb->AddEntry(tile2,"Tile2","p");
    allCaloNoises->SetMarkerColor(iColor+=dColor);
    allCaloNoises->SetMarkerStyle(22);
    if ( iMode == 0 )
      allCaloNoises->Draw("cellnoise_topo:celleta_topo>>tile3","cell_calo_topo==3&&cell_samp_topo==2&&cell_ireg_topo<3&&cell_iphi_topo==0","same");
    else
      allCaloNoises->Draw("cellvol_topo:celleta_topo>>tile3","cell_calo_topo==3&&cell_samp_topo==2&&cell_ireg_topo<3&&cell_iphi_topo==0","same");
    c0->Modified();
    c0->Update();
    tlb->AddEntry(tile3,"Tile3","p");
  }
  tla->Draw();
  tlb->Draw();

  TString name="plotAllCaloNoise13.gif";
  c0 -> SaveAs(name);

}

