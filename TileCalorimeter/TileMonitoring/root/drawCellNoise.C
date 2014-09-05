/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// 
// ROOT macro drawCellNoise.C
// Author : C. Clement - 2014-08-12
// Purpose: Draw the cell noise summary histograms produced by TileMonitoring/TileCellNoiseMonTool
// Output : .pdf files of the plots are located in subdir plots/
//
///////////////////////////////////////////////////////////////////////////////
{
  gROOT->Reset();

  /////////// SET IF YOU WISH TO USE ATLAS STYLE
  Bool_t doATLASstyle=true;

  /////////// GIVE PATH TO ATLAS STYLE
  TString PathToAtlasStyle;
  if(doATLASstyle) {
    PathToAtlasStyle="/Users/clement/MyDesk/SUSY/analysis_dil_2013_summer/analysis/histo_level_macros/Common/trunk/";
    gROOT->LoadMacro(PathToAtlasStyle+"AtlasStyle.C");
    SetAtlasStyle();
  } // doATLASstyle

  //////////// COLORS FOR 2D MAPS AND MARGIN FOR Z AXIS
  gStyle->SetPalette(1,0);
  gStyle->SetPadRightMargin(0.2);


  //////////// GIVE THE INPUT FILENAME HERE
  TString filename = "tilemon_201572_0.root";

  //////////// GIVE THE FOLDER NAME FOR THE CELL NOISE
  TString DirectoryName="/Tile/CellNoise/";


  
  TFile * cell_noise_file = TFile::Open(filename);

  gDirectory->cd(DirectoryName);
  
  const Int_t Npartitions= 4;
  const Int_t Ngains     = 2;
  const Int_t Nvars      = 7;

  TH2F * h_cell_noise_map[Npartitions][Ngains][Nvars];
  Float_t MaxZ = 1000.; // max of z-scale

  const char *partitionName[Npartitions] = {"LBA","LBC","EBA","EBC"};
  const char *gainName     [Ngains]      = {"LG","HG"};
  const char *GainFolder   [Ngains]      = {"LG","HG"};
  const char *gainNameNice [Ngains]      = {"Low Gain","High Gain"};
  Bool_t enabledGain       [Ngains]      = {  0,   1};
  const char *varName      [Nvars]       = {             "sigma1",              "sigma2",          "R",     "chi2",        "chi2prb", "rms",         "rmsOsig"};
  Bool_t enabledVar        [Nvars]       = {                    1,                     1,            1,          1,                1,     1,                 1};
  const char *varNameNice  [Nvars]       = {"#sigma_{1} (2G fit) [MeV]", 
					    "#sigma_{2} (2G fit) [MeV]", 
					    "R (2G fit)", 
					    "#chi^{2}", 
					    "#chi^{2} prob.", 
					    "RMS [MeV]", 
					    "RMS/#sigma_{1}" };

  float Maximum          [Ngains][Nvars] = {                800,                    2000,          50,        100,                 1,  1600,                10, 
							     50,                     200,          50,        100,                 1,   100,                 7};
  
  char canvasName [100];
  char canvasTitle[100];

  char histoName  [100];
  char histoTitle [100];
  char CurrentTitle[50];
  
  TCanvas * c_cell_noise_map[Npartitions][Ngains][Nvars];
  TString ZAxisLabel;
  TString fullHistoName;
  TString PrintingName;

  for(Int_t ipart=0;ipart<Npartitions;++ipart)
    {
      for(Int_t igain=0;igain<Ngains;++igain)
	{
	  if(enabledGain[igain])
	    {
	      for(Int_t ivar=0;ivar<Nvars;++ivar)
		{
		  if(enabledVar[ivar]) 
		    {
		      sprintf(canvasName, "c_%s_%s_%s",partitionName[ipart],gainName[igain],varName[ivar]);
		      sprintf(canvasTitle,"c_%s_%s_%s",partitionName[ipart],gainName[igain],varName[ivar]);
		      c_cell_noise_map[ipart][igain][ivar] = new TCanvas(canvasName,canvasTitle,600,600);
		      
		      sprintf(histoName, "map_%s_%s",varName[ivar],partitionName[ipart]);
		      cout << " histoName = "<< histoName << endl;
		      sprintf(histoTitle, "CellNoise - %s - %s - ",varNameNice[ivar],partitionName[ipart],gainNameNice[igain]);
		      cout << " histoTitle = "<< histoName << endl;

		      
		      fullHistoName=DirectoryName+GainFolder[igain]+"/"+histoName;
		      cout << " histoName with full path = " << fullHistoName  << endl;

		      h_cell_noise_map [ipart][igain][ivar] = (TH2F*) (cell_noise_file->Get(fullHistoName)->Clone(histoName));
		      gROOT->ForceStyle();
		      h_cell_noise_map [ipart][igain][ivar]->SetTitle(histoName);
		      gROOT->ForceStyle();
		      c_cell_noise_map [ipart][igain][ivar]->cd();
		      h_cell_noise_map [ipart][igain][ivar]->GetZaxis()->SetRangeUser(0,MaxZ);
		      h_cell_noise_map [ipart][igain][ivar]->GetYaxis()->SetTitle("Module #");
		      h_cell_noise_map [ipart][igain][ivar]->GetXaxis()->SetTitle("Cell ID");
		      ZAxisLabel = varNameNice[ivar];
		      h_cell_noise_map [ipart][igain][ivar]->GetZaxis()->SetTitle(ZAxisLabel);
		      h_cell_noise_map [ipart][igain][ivar]->GetXaxis()->SetLabelSize(0.03);
		      h_cell_noise_map [ipart][igain][ivar]->GetYaxis()->SetLabelSize(0.03);
		      h_cell_noise_map [ipart][igain][ivar]->GetZaxis()->SetLabelSize(0.02);
		      h_cell_noise_map [ipart][igain][ivar]->GetZaxis()->SetRangeUser(0,Maximum[igain][ivar]);
		      h_cell_noise_map [ipart][igain][ivar]->Draw("colz");
		      if   (strcmp(varName[ivar],"chi2prb")==0  ) {   
			gPad->SetLogz(1);   
			h_cell_noise_map [ipart][igain][ivar]->GetZaxis()->SetRangeUser(0.0001,Maximum[igain][ivar]);
		      }
		      else {   gPad->SetLogz(0);   }
		      
		      TLatex HistoTitle;
		      HistoTitle.SetTextSize(0.033);
		      HistoTitle.SetTextAlign(12);
		      sprintf(CurrentTitle,"%s - %s - %s",partitionName[ipart],gainNameNice[igain],varNameNice[ivar]);
		      HistoTitle.DrawLatex(1, 67, CurrentTitle);
		      
		      PrintingName  = "plots/";
		      PrintingName += c_cell_noise_map [ipart][igain][ivar]->GetName();
		      PrintingName += ".pdf";
		      cout << "PrintingName = " << PrintingName << endl;
		      c_cell_noise_map [ipart][igain][ivar]->Print(PrintingName);
		    } // if(enabledVar[ivar])
		} // ivar
	    } // igain
	} // igain
    } // ipart

}
