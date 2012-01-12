/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int plotTauData(const char * filename, int select = 0, const char * psfile = 0)
{
  TString l2path, efpath;

  if(select == 1 ) {
    l2path = "Histogramming/Top-LVL2-L2-Segment-1-Gatherer/EXPERT/";
    efpath = "Histogramming/Top-EF-EBEF-Segment/EXPERT/";
  }else if( select ==2 ) {
    l2path = "Histogramming-L2-Segment-1-1-iss/L2PU-1/Histogramming-L2-Segment-1-1-iss.L2PU-1./EXPERT/";
    efpath = "Histogramming-EF-Segment-1-1-iss/PT-1/Histogramming-EF-Segment-1-1-iss.PT-1./EXPERT/";
  }else if( select ==3 ) {
    l2path = "";
    efpath = "";
  }else{
    cout << "Usage : \n";
    cout << "    .x plotTauData(const char * filename, int select = 0, const char * psfile = 0)\n";
    cout<< " where select = \n";
    cout<< "         1 for online DATA histograms\n"
	<< "         2 for offline DATA histograms\n"
	<< "         3 for MC histograms\n";
    return;

  }
		   
  


  gStyle->SetOptStat(1111111);
  //gStyle->SetPadTopMargin(0.2);
  gStyle->SetPadGridY(true);
  gStyle->SetPadGridX(true);
  gStyle->SetStatY(0.6); 
  gStyle->SetStatStyle(0)   ; 
  //gStyle->SetTitleY(1);
  TFile file(filename);
  TCanvas * c1 = (TCanvas*)gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new  TCanvas("c1","",10,10,800,600);
  
  c1->SetTitle("L2Calo");
  
  TString psname;
  if( psfile ) psname.Append(psfile);
  else         psname.Append("tauslice.eps");
  c1->Print(psname+TString("["));
  plotL2Calo(file, psname, l2path);
  plotL2ID(file, psname, l2path);
  plotL2Final(file, psname, l2path);
  plotEF(file,psname,"TrigTauRecMerged_Tau", efpath );
  plotEF(file,psname,"TrigTauRecMerged_Tau_caloOnly", efpath );
  plotEF(file,psname,"TrigTauRec_Tau", efpath );
  plotDec(file, psname, 2, l2path);
  plotDec(file, psname, 3, efpath);
  c1->Print(psname+TString("]"));
  return 0;

}

void plotL2Calo(TFile & file, TString & psname, TString path)
{
  TPaveLabel* pl = new TPaveLabel(0.8,0.95,1,1.,"T2CaloTau_Tau");
  pl->SetBorderSize(0);
  pl->SetFillStyle(0);

  c1->Divide(2,2);
  pl->Draw();
  c1->cd(1); plotHist(file, path+"T2CaloTau_Tau/EtCalib");
  c1->cd(2); plotHist(file, path+"T2CaloTau_Tau/EtRaw");
  c1->cd(3); plotHist(file, path+"T2CaloTau_Tau/RawEtNar");
  c1->Print(psname);

  
  c1->cd(1);  plotHist(file,path+"T2CaloTau_Tau/EME");
  c1->cd(2);  plotHist(file,path+"T2CaloTau_Tau/HadE");
  c1->cd(3);  plotHist(file,path+"T2CaloTau_Tau/EMEtNar");
  c1->cd(4);  plotHist(file,path+"T2CaloTau_Tau/EMenergyNar");
  c1->Print(psname);

  c1->cd(1);  plotHist(file,path+"T2CaloTau_Tau/IsoFrac");
  c1->cd(2);  plotHist(file,path+"T2CaloTau_Tau/StripWidth");
  c1->cd(3);  plotHist(file,path+"T2CaloTau_Tau/EMRadius");
  c1->cd(4);  plotHist(file,path+"T2CaloTau_Tau/EMenergyWid");
  c1->Print(psname);

  c1->Clear(); c1->Divide(2,3);  pl->Draw();
  c1->cd(1);  plotHist(file,path+"T2CaloTau_Tau/Phi");
  c1->cd(2);  plotHist(file,path+"T2CaloTau_Tau/Eta");
  c1->cd(3);  plotHist(file,path+"T2CaloTau_Tau/PhiL1_vs_EtaL1","colz");
  c1->cd(4);  plotHist(file,path+"T2CaloTau_Tau/EtaL2vsL1");
  c1->cd(5);  plotHist(file,path+"T2CaloTau_Tau/PhiL2vsL1");
  // plotHist(file,path+"T2CaloTau_Tau/");
  c1->Print(psname);
}

void plotL2ID(TFile & file, TString & psname, TString path)
{
  TPaveLabel* pl = new TPaveLabel(0.8,0.95,1,1.,"T2IDTau_Tau");
  pl->SetBorderSize(0);
  pl->SetFillStyle(0);

  c1->SetTitle(TString("T2IDTau_Tau"));

  c1->Clear(); 
  c1->Divide(2,2);pl->Draw();
  c1->cd(1); plotHist(file, path+"T2IDTau_Tau/NTrk");
  c1->cd(2); plotHist(file, path+"T2IDTau_Tau/NTrkIso");
  c1->cd(3); plotHist(file, path+"T2IDTau_Tau/NTrkSlow");
  c1->cd(3); plotHist(file, path+"T2IDTau_Tau/charge");
  c1->Print(psname);
  
  c1->cd(1);  plotHist(file,path+"T2IDTau_Tau/sumPtRatio");
  c1->cd(2);  plotHist(file,path+"T2IDTau_Tau/PtMaxTrk");
  c1->cd(3);  plotHist(file,path+"T2IDTau_Tau/PhiL1_vs_EtaL1","colz");
  c1->Print(psname);
}

void plotL2Final(TFile & file, TString & psname, TString path)
{
  TPaveLabel* pl = new TPaveLabel(0.8,0.95,1,1.,"T2TauFinal_Tau");
  pl->SetBorderSize(0);
  pl->SetFillStyle(0);
  c1->SetTitle(TString("T2TauFinal_Tau"));

  c1->Clear();
  c1->Divide(2,2); pl->Draw();
  c1->cd(1); plotHist(file, path+"T2TauFinal_Tau/NMatchedTracks");
  c1->cd(2); plotHist(file, path+"T2TauFinal_Tau/EtCalibCluster");
  c1->cd(3); plotHist(file, path+"T2TauFinal_Tau/SimpleEtFlow");
  c1->cd(3); plotHist(file, path+"T2TauFinal_Tau/EtCombined");
  c1->Print(psname);
  
  c1->Clear(); 
  c1->Divide(2,3);pl->Draw();
  c1->cd(1);  plotHist(file,path+"T2TauFinal_Tau/dEtaTrigTau_cluster");
  c1->cd(2);  plotHist(file,path+"T2TauFinal_Tau/DeltaEtaTkClust");
  c1->cd(3);  plotHist(file,path+"T2TauFinal_Tau/DeltaPhiTkClust");
  c1->cd(4);  plotHist(file,path+"T2TauFinal_Tau/PhiL1_vs_EtaL1","colz");
  c1->cd(5);  plotHist(file,path+"T2TauFinal_Tau/dPhiTrigTau_cluster");
  c1->cd(6);  plotHist(file,path+"T2TauFinal_Tau/DeltaRTkClust");
  c1->Print(psname);
}

void plotDec(TFile & file, TString & psname ,int level, TString path)
{

  TH2F * h2;

  TString dir = path+"TrigSteer_L2/";
  TString tag = "L2";
  if( level == 3 ){
    dir = path+"TrigSteer_EF/";
    tag = "EF";
  }

  TPaveLabel* pl = new TPaveLabel(0.8,0.95,1,1.,"TrigSteer_"+tag);
  pl->SetBorderSize(0);
  pl->SetFillStyle(0);

  c1->Clear();
  plotSteerHist2(file,dir+"ErrorCodes_vs_Chains_"+tag+"_reason","text");pl->Draw();
  c1->Print(psname);
  plotSteerHist1(file,dir+"allTEnumber_"+tag+"_runsummary");pl->Draw();
  c1->Print(psname);
  plotSteerHist1(file,dir+"alllvl1TEnumber_"+tag+"_runsummary");pl->Draw();
  c1->Print(psname);
  plotSteerHist2(file,dir+"alllvl1TEnumberevent_"+tag+"_runsummary","col,text");pl->Draw();
  c1->Print(psname);
  plotSteerHist1(file,dir+"m_etaphi_EM_all","text");pl->Draw();
  c1->Print(psname);

  plotSteerHist1(file,dir+"chainAcceptancePT_"+tag+"_runsummary");pl->Draw();
  c1->Print(psname);
  plotSteerHist1(file,dir+"chainAcceptancePS_"+tag+"_runsummary");pl->Draw();
  c1->Print(psname);
  plotSteerHist1(file,dir+"chainAcceptance_"+tag+"_runsummary");pl->Draw();
  c1->Print(psname);
  plotSteerHist2(file,dir+"signatureAcceptance_"+tag+"_runsummary","col,text");pl->Draw();
  c1->Print(psname);
}


void plotEF(TFile & file, TString & psname,const char * algo, TString path)
{
  c1->SetTitle(TString("EF"));
  TString tag(algo);

  TPaveLabel* pl = new TPaveLabel(0.8,0.95,1,1.,tag);
  pl->SetBorderSize(0);
  pl->SetFillStyle(0);

  c1->Clear(); 
  c1->Divide(2,3);pl->Draw();
  c1->cd(1);  plotHist(file,path+""+tag+"/nCand");
  c1->cd(2);  plotHist(file,path+""+tag+"/NTools");
  c1->cd(3);  plotHist(file,path+""+tag+"/EMFrac");
  c1->cd(4);  plotHist(file,path+""+tag+"/EF_TauEMRadius");
  c1->cd(5);  plotHist(file,path+""+tag+"/EMCalibEt");
  c1->cd(6);  plotHist(file,path+""+tag+"/IsoFrac");
  c1->Print(psname);
  
  c1->Clear(); 
  c1->Divide(2,2);pl->Draw();
  c1->cd(1);  plotHist(file,path+""+tag+"/NTrk");
  c1->cd(2);  plotHist(file,path+""+tag+"/PtMaxTrk");
  c1->cd(3);  plotHist(file,path+""+tag+"/nRoI_EFTauTracks");
  c1->cd(4);  plotHist(file,path+""+tag+"/nRoI_EFTauCells");

  c1->Print(psname);
  
  c1->Clear(); 
  c1->Divide(2,2);pl->Draw();
  c1->cd(1);  plotHist(file,path+""+tag+"/PhiL1_vs_EtaL1","colz");
  c1->cd(2);  plotHist(file,path+""+tag+"/dPhiEFTau_RoI");
  c1->cd(3);  plotHist(file,path+""+tag+"/dEtaEFTau_RoI");
  c1->Print(psname);

}

void plotSteerHist2(TFile & file, const char * histname, const char * opt = "text")
{
  TH1* h1 = ((TH1*)file.Get(histname));
  if( h1 == 0 ) return;

  float bottomPad = gStyle->GetPadBottomMargin();
  int stat = gStyle->GetOptStat();  gStyle->SetOptStat(0);
  gPad->SetBottomMargin(0.5);
  gPad->SetLeftMargin(0.2);
  //gPad->ForceStyle();


  TAxis * axis  = h1->GetXaxis();
  TAxis * axisY = h1->GetYaxis();

  std::vector<TString> newLabels;

  int nbins = axis->GetNbins();
  for(unsigned int ibin = 1; ibin <= nbins ; ++ibin)
    {
      TString label(axis->GetBinLabel(ibin));
      if( label.Contains("HALO")) continue;
      if( label.Contains("tau") || label.Contains("Tau") ||  label.Contains("TAU") ||  label.Contains("HA") ||
	  label.Contains("trk") || label.Contains("trk"))
	{
	  //if( tag2 == "" || label.Contains(tag2) ) {
	  //if( tag3 == "" || label.Contains(tag3) ) 
	      newLabels.push_back(label);
	    //	cout << "           taken\n";}
	      //}
	}
    }
  
  int nNew = newLabels.size();
  TString newName(h1->GetName());

  int nY =  axisY->GetNbins();

  newName.Append("_sub");
  TH2F * hNew = (TH2F*)gDirectory->Get(newName);
  if( hNew ) delete hNew;
  hNew = new TH2F(newName,h1->GetTitle(), nNew, 0, nNew, nY,0, nY);
  hNew->GetYaxis()->ImportAttributes(axisY);
  //hNew->SetTextSize(0.4);
  hNew->SetLabelSize(0.04);
  hNew->SetStats(false);

  // name axis
  for(unsigned int ibin = 1; ibin <= nY ; ++ibin)
    hNew->GetYaxis()->SetBinLabel(ibin,axisY->GetBinLabel(ibin));

  for(unsigned int ibin = 1; ibin <= nNew ; ++ibin)
    {
      hNew->GetXaxis()->SetBinLabel(ibin,newLabels[ibin-1]);
      for(unsigned int jbin = 0; jbin <= nY+1; ++jbin )
	hNew->SetBinContent(ibin, jbin,
			    h1->GetBinContent(h1->GetXaxis()->FindBin(newLabels[ibin-1]), jbin));
      

    }

  hNew->LabelsOption("v");
  hNew->Draw(opt);
  gStyle->SetOptStat(stat);
  gStyle->SetPadBottomMargin(bottomPad);

}

void plotSteerHist1(TFile & file, const char * histname, const char * opt = "")
{
  TH1* h1 = ((TH1*)file.Get(histname));
  if( h1 == 0 ) return;

  float bottomPad = gStyle->GetPadBottomMargin();
  int stat = gStyle->GetOptStat();
  gStyle->SetOptStat(0);
  gPad->SetBottomMargin(0.5);
  //gPad->ForceStyle();


  TAxis * axis  = h1->GetXaxis();
  TAxis * axisY = h1->GetYaxis();

  std::vector<TString> newLabels;

  int nbins = axis->GetNbins();
  for(unsigned int ibin = 1; ibin <= nbins ; ++ibin)
    {
      TString label(axis->GetBinLabel(ibin));
       if( label.Contains("HALO")) continue;
     if( label.Contains("tau") || label.Contains("Tau") ||  label.Contains("TAU") ||  label.Contains("HA") ||
	  label.Contains("trk") || label.Contains("trk"))
	{
	  //if( tag2 == "" || label.Contains(tag2) ) {
	  //if( tag3 == "" || label.Contains(tag3) ) 
	      newLabels.push_back(label);
	    //	cout << "           taken\n";}
	      //}
	}
    }
  
  int nNew = newLabels.size();
  TString newName(h1->GetName());

  newName.Append("_sub");
  TH1F * hNew = (TH1F*)gDirectory->Get(newName);
  if( hNew ) delete hNew;
  hNew = new TH1F(newName,h1->GetTitle(), nNew, 0, nNew);
  hNew->GetYaxis()->ImportAttributes(axisY);
  hNew->SetLabelSize(0.04);
  hNew->SetStats(false);

  for(unsigned int ibin = 1; ibin <= nNew ; ++ibin)
    {
      hNew->GetXaxis()->SetBinLabel(ibin,newLabels[ibin-1]);
      hNew->SetBinContent(ibin,h1->GetBinContent(h1->GetXaxis()->FindBin(newLabels[ibin-1])));
    }

  hNew->LabelsOption("v");
  hNew->SetLineColor(4);  hNew->Draw(opt);
  gStyle->SetOptStat(stat);
  gStyle->SetPadBottomMargin(bottomPad);

}


void plotHist(TFile & file, const char *histname, const char * opt = "")
{
  if( file.Get(histname) == 0 ){
    cout << " Histogram " << histname << " is not found \n";
    return;
  }
  ((TH1*)file.Get(histname))->SetLineColor(4);
  ((TH1*)file.Get(histname))->Draw(opt);
}
