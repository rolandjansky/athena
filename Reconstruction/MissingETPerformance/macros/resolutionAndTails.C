/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////
//Produces plots of different MET types in RefMET algorithm, 
//computes res and tails for them and compares to other samples.
//Written by Jet Goodson, jgoodson@cern.ch
//Last modified 16 March, 2009
/////////////////////////////////////

void resolutionAndTails()
{
  gStyle->SetOptStat(0);

  //////////////////////Things to change based on file.
  // const int scaleSwitch = 0; //set to 1 to scale by cross section, if samples are the same 

  const int numberMETTypes = 2;   ///the number of different types of MET you want, ie, individual terms like Muon MET, CellOut, etc
  const int numberInputFiles = 2;  //the number of samples you want to compare, ie, J6, Zmumu, ttbar, two different reconstructions of the same sample, etc.

  TFile *outputFile = new TFile("ResolutionAndTails.root", "RECREATE");

  string METTypes[numberMETTypes] = {"RefFinalResolution", "MuonBoyResolution"};  //the MET types you want from the AANT
  string inputFileNames[numberInputFiles] = {"METPerformance.root", "METPerformance_v1500.root"}; //the different samples. If more than one file for each sample, merge them before you put it here.
  string inputSamples[numberInputFiles]= {"Zmumu","Zmumu, Relaxed Cuts"}; //The names of the inputs, for labeling of plots and legends.
  Double_t inputCrossSections[numberInputFiles] = {1,1}; //used for scaling.
  string legendTitle[numberMETTypes] = {"#splitline{RefFinal MET, AANT 1}{e344_s479_d126_r605}","#splitline{Muon MET Term, AANT 2}{e344_s479_d126_r605}"};
  Int_t inputColors[numberInputFiles] = {2,4};
  Int_t inputLineWidth[numberInputFiles] = {3,3};
  Int_t inputLineStyle[numberInputFiles] = {1, 1};
  string typeXaxisLabel[numberMETTypes] = {"#slash{E}_{T}^{TRUTH} - #slash{E}_{T}^{RECO} (GeV)","#slash{E}_{T, #mu}^{TRUTH} - #slash{E}_{T, #mu}^{RECO} (GeV)"};
  string typeYaxisLabel[numberMETTypes] = {"#", "#"};
  TFile *inputFile[numberInputFiles];
  //////////////////////////////////////

  TH1D *h_MET_x[numberMETTypes][numberInputFiles]; //holds the MET_x of each type of MET for each input sample
  TH1D *h_MET_y[numberMETTypes][numberInputFiles]; //ditto for MET_y
  TH1D *h_MET_xAndY[numberMETTypes][numberInputFiles]; //holds the sum of the two preceeding histograms.
  Double_t scaleSize[numberMETTypes][numberInputFiles]; //used to scale histograms to compare datasets with different number of events. 
  Double_t resolution_xAndY[numberMETTypes][numberInputFiles]; //holds resolution values for eachs type of MET for each dataset.
  Double_t tails_xAndY[numberMETTypes][numberInputFiles]; //ditto for tails.
  string name; //holds the name of the histogram during retrieval.
  Int_t nBins = 0; //sizes of the histograms. Assumed the same for each.
  Int_t high = 0;
  Int_t low = 0;
  TCanvas *canvas[numberMETTypes]; //a canvas for drawing the overlayed histograms.
  TLegend *legend[numberMETTypes]; //a legend for each.
  Double_t maximum[numberMETTypes]; //Gets the max value for each so that hte histograms can be drawn without cutting off the top.
  Int_t maxSample = 0; //holds index of max sample.
  Int_t maxEntries[numberMETTypes]; //maxEntries and minEntries are used for scaling the histograms together.
  Int_t minEntries[numberMETTypes];
  Int_t minLuminosity[numberMETTypes];
  char c_input[100] = "yarg!"; //used for holding char values to name legends and entries.
  char c_entry[100] = "bleg";
  Double_t luminosity = 0; //placeholder for int luminosity calc for scaling.

  for(Int_t k = 0; k < numberMETTypes; ++k) //just setting arrays to initial values.
    {
      maximum[k] = 0;
      maxEntries[k] = 0;
      minEntries[k] = 10000000000000;
      minLuminosity[k] = 10000000000;
    }
 
  for(Int_t i = 0; i < numberInputFiles; ++i) //loop over input datasets.
    { 
      inputFile[i] = new TFile(inputFileNames[i].c_str()); //open input file.
      for(Int_t j = 0; j < numberMETTypes; ++j) //loop over MET ypes.
	{
	  
	  name = "Resolution/h_" + METTypes[j] + "_x"; //name of histogram wanted
	  h_MET_x[j][i] = ((TH1D)inputFile[i]->Get(name.c_str())); //retrieval
	  name = "Resolution/h_" + METTypes[j] + "_x" + inputSamples[i];
	  //h_MET_x[j][i]->SetName(name.c_str());
	  name = "Resolution/h_" + METTypes[j] + "_y"; 
	  h_MET_y[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name = "Resolution/h_" + METTypes[j] + "_y" + inputSamples[i];
	  //h_MET_y[j][i]->SetName(name.c_str());
	  //h_MET_y[j][i]->Draw();
	  outputFile->cd(); //switch to the output root file and write original histograms.
	  h_MET_y[j][i]->Write();
	  h_MET_x[j][i]->Write();
	 
	  name = METTypes[j] + "xAndY_" + inputSamples[i]; //set up xAndY histo.
	  nBins = h_MET_x[j][i]->GetNbinsX();
	  low = h_MET_x[j][i]->GetBinLowEdge(0) + h_MET_x[j][i]->GetBinWidth(0);
	  high = h_MET_x[j][i]->GetNbinsX()*h_MET_x[j][i]->GetBinWidth(0) + h_MET_x[j][i]->GetBinLowEdge(0) + h_MET_x[j][i]->GetBinWidth(0);
	  

	  h_MET_xAndY[j][i] = new TH1D(name.c_str(), name.c_str(), h_MET_x[j][i]->GetNbinsX(), h_MET_x[j][i]->GetBinLowEdge(0) + h_MET_x[j][i]->GetBinWidth(0), h_MET_x[j][i]->GetNbinsX()*h_MET_x[j][i]->GetBinWidth(0) + h_MET_x[j][i]->GetBinLowEdge(0) + h_MET_x[j][i]->GetBinWidth(0));
	  h_MET_xAndY[j][i]->Add(h_MET_x[j][i], h_MET_y[j][i]);
	  luminosity = h_MET_x[j][i]->GetEntries()/inputCrossSections[i];
	  if(i == 0) minEntries[j] = 10000000000;//h_MET_xAndY[j][i]->GetEntries();  //get minimum and maximum values for scaling.
	  if(h_MET_x[j][i]->GetMaximum() > maximum[j])
	    {
	      maximum[j] = h_MET_x[j][i]->GetMaximum();
	      maxSample = i;
	    }
	  if(h_MET_x[j][i]->GetEntries() > maxEntries[j]) maxEntries[j] = h_MET_x[j][i]->GetEntries();
	  if(h_MET_x[j][i]->GetEntries() < minEntries[j]) minEntries[j] = h_MET_x[j][i]->GetEntries();
	  if(luminosity < minLuminosity[j]) minLuminosity[j] = luminosity; //will scale to sample with minimum luminosity

	}//end of first type loop
      //inputFile[i]->Close(); //sometimes this can make a program act funny. If so, just don't do it.
    }//end of second input loop



  for(i = 0; i < numberInputFiles; ++i) //loop over data sets
    { 
      for(j = 0; j < numberMETTypes; ++j) //loop over MET types.
	{
	  scaleSize[j][i] = 1.0; //Default scale value.
	  //if(h_MET_x[j][i]->GetEntries() != 0) scaleSize[j][i] = static_cast<Double_t>(minEntries[j])/static_cast<Double_t>(abs(h_MET_x[j][i]->GetEntries())); //calculate scale value.
	  //h_MET_x[j][i]->Sumw2(); //didn't work, screws up formatting of histogram.
	  if(h_MET_x[j][i]->GetEntries() != 0) scaleSize[j][i] = static_cast<Double_t>(minLuminosity[j])/static_cast<Double_t>(abs(h_MET_x[j][i]->GetEntries()/inputCrossSections[i]));
	 

	  cout << "For " << METTypes[j] << " and " << inputSamples[i] << " the underflow and overflow is " << h_MET_xAndY[j][i]->GetBinContent(0) << " and " << h_MET_xAndY[j][i]->GetBinContent(nBins + 1) << " out of " << h_MET_xAndY[j][i]->GetEntries()  << " entries, before application of normalization factor " << scaleSize[j][i] << endl;
	  h_MET_xAndY[j][i]->Scale(scaleSize[j][i]); //actually scale the histogram.
	  
	  if(i == 0) //set up canvas for drawing.
	    {
	      name = METTypes[j] + "_Canvas";
	      canvas[j] = canvaser(name, 800, 600);
	      legend[j] = new TLegend(.6,.6,.98,.98, legendTitle[j].c_str());
	    }
	  
	 

	  histogrammar(h_MET_xAndY[j][i], inputColors[i], typeXaxisLabel[j], typeYaxisLabel[j], maximum[j], inputLineWidth[i], inputLineStyle[i]); //define histogram how I want.
	  h_MET_xAndY[j][i]->Write();

	  canvas[j]->cd();
	  legendary(legend[j], h_MET_xAndY[j][i], nBins, low, high, inputSamples[i].c_str());  
	  if(i == 0)  h_MET_xAndY[j][i]->Draw();
	  if(i > 0) h_MET_xAndY[j][i]->Draw("SAME");
	  if(i == numberInputFiles - 1)
	    {
	      legend[j]->Draw("SAME");
	      canvas[j]->Write();
	    }
	  
	}//end of METTypes loop
      
    }//end of input file loop.


  outputFile->Write();
  outputFile->Close();

}//end of macro




double tails(TH1D *histogramOne, TH1D *histogramTwo, int bins, int low, int high)//calculates tails of MET_true - MET_reco distributions.
{
  TF1 *fit1 = new TF1("fitted1", "gaus", -1.5*histogramOne->GetRMS(), 1.5*histogramOne->GetRMS());
  TAxis *axis1 = histogramOne->GetXaxis();
  histogramOne->Fit("fitted1","RNQ");

  TF1 *fit2 = new TF1("fitted2", "gaus", -1.5*histogramTwo->GetRMS(), 1.5*histogramTwo->GetRMS());
  histogramTwo->Fit("fitted2","RNQ");
  if(histogramOne->GetEntries() > 0)
    {
  return (histogramOne->Integral(low, axis1->FindBin(fit1->GetParameter(1) - 3.0*fit2->GetParameter(2))) + histogramOne->Integral(axis1->FindBin(fit1->GetParameter(1) + 3.0*fit2->GetParameter(2)), high))/histogramOne->GetEntries();
    }
  else if
    {
      return -1;
    }
}


double resolution(TH1D *histogram, int bin, int low, int highs)//calculates resoltuion of MET_true - MET_reco  distributions.
{
  TF1 *fit = new TF1("fitted", "gaus", -1.5*histogram->GetRMS(), 1.5*histogram->GetRMS());
  TAxis *axis = histogram->GetXaxis();
  histogram->Fit("fitted","RNQ");
  return fit->GetParameter(2);
}


TCanvas *canvaser(string name, int ex, int why)//sets up a canvas with the options I prefer.
{
  TCanvas *canvas = new TCanvas(name.c_str(), name.c_str(), 0,0,ex,why);
  canvas->SetFillColor(0);
  canvas->GetFrame()->SetFillColor(21);
  canvas->GetFrame()->SetBorderMode(-1);

  return canvas;
}


TH1 histogrammar(TH1D *histogram, int color, string exAxis, string whyAxis, double maximum, int lineWidth, int lineStyle) //sets up a histogram like I want.
{
  //int maximum = 0;
  histogram->SetLineWidth(3);
  histogram->SetLineColor(color);
  histogram->SetLineStyle(lineStyle);
  histogram->SetLineWidth(lineWidth);
  histogram->GetXaxis()->SetTitle(exAxis.c_str());
  histogram->GetYaxis()->SetTitle(whyAxis.c_str());
  histogram->GetYaxis()->SetTitleOffset(1.3);
  //maximum = histogram->GetMaximum();
  histogram->SetMaximum(3*maximum + 1);
  return histogram;
}



TLegend *legendary(TLegend *legend, TH1D *histogram, int binCount, int lowBin, int maxBin, char variable[100])//automatically sets up legends for resolution histograms.
{
  char c_words[100] = "blargh";
  char c_variable[100] = "#splitline{";
  strcat(c_variable, variable);
  sprintf(c_words, ", #sigma = %03f}", resolution(histogram, binCount, lowBin, maxBin));
  strcat(c_variable, c_words);
  strcpy(c_words, "{");
  strcat(c_words, variable);
  strcat(c_variable, c_words);
  sprintf(c_words, ", Tails = %03f}", tails(histogram, histogram, binCount, lowBin, maxBin));
  strcat(c_variable, c_words);

  legend->AddEntry(histogram, c_variable, "l");
  return legend;

}


TLegend *simpleLegend(TLegend *legend, TGraph *graph1, TGraph *graph2, char variable1[100], char variable2[100]) //Quick two entry legend maker.
{

  legend->AddEntry(graph1, variable1, "l");
  legend->AddEntry(graph2, variable2, "l");

  return legend;
}
