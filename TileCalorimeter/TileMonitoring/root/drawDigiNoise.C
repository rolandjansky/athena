/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// 
// ROOT macro drawDigiNoise.C
// Author : C. Clement - 2014-08-12
// Purpose: Draw the cell noise summary histograms produced by TileMonitoring/TileDigiNoiseMonTool
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
  //TString filename = "tilemon_201572_0.root";
  TString filename = "tilemon_201555_0.root";

  //////////// GIVE THE FOLDER NAME FOR THE CELL NOISE
  TString DirectoryName="/Tile/DigiNoise/";


  TFile * noise_file = TFile::Open(filename);
  gDirectory->cd(DirectoryName);
  
  const Int_t Npartitions= 4;
  const Int_t Ngains     = 2;
  const Int_t Ntypes     = 2;

  TH2F * h_digi_noise_map[Npartitions][Ngains][Ntypes];

  const char *partitionName[Npartitions] = {"LBA","LBC","EBA","EBC"};
  const char *gainName     [Ngains]      = {"lo","hi"};
  const char *typeName     [Ntypes]      = {"lfn","hfn"};

  const char *gainNameNice [Ngains]      = {"LG","HG"};
  const char *typeNameNice [Ntypes]      = {"Low Freq. Noise","High Freq. Noise"};
 
  char canvasName  [50];
  char canvasTitle [50];

  char histoName   [50];
  char histoTitle  [50];
  char CurrentTitle[50];
  TString PrintingName;

  TCanvas * c_digi_noise_map[Npartitions][Ngains][Ntypes];
  
  for(Int_t ipart=0;ipart<Npartitions;++ipart)
    {
      for(Int_t igain=0;igain<Ngains;++igain)
	{
	  for(Int_t itype=0;itype<Ntypes;++itype)
	    {
	      sprintf(canvasName, "c_%s_%s_%s",partitionName[ipart],gainName[igain],typeName[itype]);
	      sprintf(canvasTitle,"c_%s_%s_%s",partitionName[ipart],gainName[igain],typeName[itype]);
	      c_digi_noise_map[ipart][igain][itype] = new TCanvas(canvasName,canvasTitle,600,600);

	      sprintf(histoName, "noisemap_%s__%s_%s",partitionName[ipart],gainName[igain],typeName[itype]);
	      cout << " histoName = "<< histoName << endl;

	      //sprintf(histoTitle,"h_%s_%s_%s",partitionName[ipart],gainName[igain],typeName[itype]);
	      h_digi_noise_map [ipart][igain][itype] = (TH2F*) (noise_file->Get(DirectoryName+"/"+histoName)->Clone());
	      c_digi_noise_map [ipart][igain][itype]->cd();
	      h_digi_noise_map [ipart][igain][itype]->GetZaxis()->SetRangeUser(0,3);
	      h_digi_noise_map [ipart][igain][itype]->GetYaxis()->SetTitle("");
	      h_digi_noise_map [ipart][igain][itype]->GetYaxis()->SetLabelSize(0.03);
	      h_digi_noise_map [ipart][igain][itype]->Draw("colz");

	      TLatex HistoTitle;
	      HistoTitle.SetTextSize(0.033);
	      HistoTitle.SetTextAlign(12);
	      sprintf(CurrentTitle,"%s - %s - %s",partitionName[ipart],gainNameNice[igain],typeNameNice[itype]);
	      HistoTitle.DrawLatex(1, 67, CurrentTitle);

	      PrintingName  = "plots_diginoise/";
	      PrintingName += c_digi_noise_map[ipart][igain][itype] -> GetName();
	      PrintingName += ".pdf";
	      cout << "PrintingName = " << PrintingName << endl;
	      c_digi_noise_map[ipart][igain][itype]->Print(PrintingName);

	    } // itype
	} // igain
    } // ipart

}
